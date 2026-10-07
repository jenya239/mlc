#pragma once

// SQLite ABI for std/db/sqlite. Handles are i32. Pointers stay in this header.
//
// Invariants:
// - SQLITE_VERSION_NUMBER >= 3007015 (sqlite3_errstr).
// - One connection per thread: SQLITE_OPEN_NOMUTEX, no URI, no shared cache.
// - bind_text / bind_blob use SQLITE_TRANSIENT only.
// - errmsg and column text are copied into String before the next SQLite call.
// - A missing handle is code 21, including a handle used from another thread.
// - sqlite3_close_v2 is not used.
// - Published handle numbers are never reused.

#include "mlc/core/string.hpp"

#include <sqlite3.h>

#if SQLITE_VERSION_NUMBER < 3007015
#error "mlc/db/sqlite_abi.hpp requires SQLite >= 3.7.15"
#endif

#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

namespace mlc {
namespace db {

namespace sqlite_detail {

struct StatementEntry {
  sqlite3_stmt* statement;
  std::int32_t connection_handle;
  int step_result;
};

struct Table {
  std::unordered_map<std::int32_t, sqlite3*> connections;
  std::unordered_map<std::int32_t, StatementEntry> statements;
  std::int32_t next_handle;
  int code;
  int extended;
  String message;

  Table() : next_handle(1), code(0), extended(0), message() {}

  ~Table() {
    std::vector<std::int32_t> statement_handles;
    statement_handles.reserve(statements.size());
    for (const auto& entry : statements) {
      statement_handles.push_back(entry.first);
    }
    for (std::int32_t handle : statement_handles) {
      auto found = statements.find(handle);
      if (found != statements.end() && found->second.statement != nullptr) {
        sqlite3_finalize(found->second.statement);
        found->second.statement = nullptr;
      }
    }
    statements.clear();
    std::vector<std::int32_t> connection_handles;
    connection_handles.reserve(connections.size());
    for (const auto& entry : connections) {
      connection_handles.push_back(entry.first);
    }
    for (std::int32_t handle : connection_handles) {
      auto found = connections.find(handle);
      if (found != connections.end() && found->second != nullptr) {
        sqlite3_close(found->second);
        found->second = nullptr;
      }
    }
    connections.clear();
  }
};

inline Table& table() {
  static thread_local Table instance;
  return instance;
}

inline void clear_error() {
  Table& slot = table();
  slot.code = 0;
  slot.extended = 0;
  slot.message = String();
}

inline bool same_text(const String& value, const char* literal) {
  if (literal == nullptr) {
    return false;
  }
  const std::size_t length = std::strlen(literal);
  if (static_cast<std::size_t>(value.byte_size()) != length) {
    return false;
  }
  return std::memcmp(value.raw_data(), literal, length) == 0;
}

inline void store_failure(int primary, int extended, String message) {
  int code = primary & 0xFF;
  if (code == 0) {
    code = 21;
  }
  if (message.byte_size() == 0 || same_text(message, "not an error")) {
    const char* fallback = sqlite3_errstr(code);
    message = String(fallback != nullptr ? fallback : "Sqlite: unknown error");
  }
  Table& slot = table();
  slot.code = code;
  slot.extended = extended != 0 ? extended : code;
  slot.message = message;
}

inline void store_sqlite_failure(sqlite3* connection, int result_code) {
  const char* text = connection != nullptr ? sqlite3_errmsg(connection) : nullptr;
  String message(text != nullptr ? text : "");
  const int extended = connection != nullptr ? sqlite3_extended_errcode(connection) : 0;
  store_failure(result_code, extended, message);
}

inline void store_named_failure(const char* function_name, const char* detail) {
  std::string text("Sqlite.");
  text += function_name != nullptr ? function_name : "sqlite";
  text += ": ";
  text += detail != nullptr ? detail : "failed";
  store_failure(21, 21, String(text));
}

inline sqlite3* find_connection(std::int32_t connection_handle) {
  auto found = table().connections.find(connection_handle);
  if (found == table().connections.end()) {
    return nullptr;
  }
  return found->second;
}

inline StatementEntry* find_statement(std::int32_t statement_handle) {
  auto found = table().statements.find(statement_handle);
  if (found == table().statements.end() || found->second.statement == nullptr) {
    return nullptr;
  }
  return &found->second;
}

inline std::int32_t take_handle() {
  Table& slot = table();
  if (slot.next_handle <= 0) {
    store_named_failure("open", "handle space exhausted");
    return 0;
  }
  const std::int32_t handle = slot.next_handle;
  if (slot.next_handle == INT32_MAX) {
    slot.next_handle = 0;
  } else {
    slot.next_handle += 1;
  }
  return handle;
}

inline int hex_value(char digit) {
  if (digit >= '0' && digit <= '9') {
    return digit - '0';
  }
  if (digit >= 'a' && digit <= 'f') {
    return digit - 'a' + 10;
  }
  if (digit >= 'A' && digit <= 'F') {
    return digit - 'A' + 10;
  }
  return -1;
}

inline String bytes_to_hex(const unsigned char* data, int size) {
  if (data == nullptr || size <= 0) {
    return String();
  }
  std::string text;
  text.resize(static_cast<std::size_t>(size) * 2);
  for (int index = 0; index < size; index += 1) {
    const unsigned char byte = data[index];
    const char digits[] = "0123456789abcdef";
    text[static_cast<std::size_t>(index) * 2] = digits[(byte >> 4) & 0x0F];
    text[static_cast<std::size_t>(index) * 2 + 1] = digits[byte & 0x0F];
  }
  return String(text);
}

inline void finalize_connection_statements(std::int32_t connection_handle) {
  std::vector<std::int32_t> owned;
  for (const auto& entry : table().statements) {
    if (entry.second.connection_handle == connection_handle) {
      owned.push_back(entry.first);
    }
  }
  for (std::int32_t handle : owned) {
    auto found = table().statements.find(handle);
    if (found == table().statements.end()) {
      continue;
    }
    if (found->second.statement != nullptr) {
      sqlite3_finalize(found->second.statement);
      found->second.statement = nullptr;
    }
    table().statements.erase(found);
  }
}

}  // namespace sqlite_detail

inline String sqlite_libversion_s() {
  const char* text = sqlite3_libversion();
  return String(text != nullptr ? text : "");
}

inline std::int32_t sqlite_libversion_number_i() {
  return static_cast<std::int32_t>(sqlite3_libversion_number());
}

inline std::int32_t sqlite_open_i(String path, std::int32_t open_kind) {
  sqlite_detail::clear_error();
  int flags = SQLITE_OPEN_NOMUTEX;
  if (open_kind == 1) {
    flags |= SQLITE_OPEN_READONLY;
  } else {
    flags |= SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  }
  sqlite3* connection = nullptr;
  const int result = sqlite3_open_v2(path.c_str(), &connection, flags, nullptr);
  if (result != SQLITE_OK || connection == nullptr) {
    if (connection == nullptr) {
      sqlite_detail::store_failure(7, 7, String(sqlite3_errstr(SQLITE_NOMEM)));
      return 0;
    }
    sqlite_detail::store_sqlite_failure(connection, result != SQLITE_OK ? result : SQLITE_NOMEM);
    sqlite3_close(connection);
    return 0;
  }
  const std::int32_t handle = sqlite_detail::take_handle();
  if (handle == 0) {
    sqlite3_close(connection);
    return 0;
  }
  sqlite_detail::table().connections.emplace(handle, connection);
  return handle;
}

inline std::int32_t sqlite_close_i(std::int32_t connection_handle) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("close_db", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  sqlite_detail::finalize_connection_statements(connection_handle);
  connection = sqlite_detail::find_connection(connection_handle);
  const int result = sqlite3_close(connection);
  if (result != SQLITE_OK) {
    sqlite_detail::store_sqlite_failure(connection, result);
    return sqlite_detail::table().code;
  }
  sqlite_detail::table().connections.erase(connection_handle);
  return 0;
}

inline std::int32_t sqlite_exec_i(std::int32_t connection_handle, String sql) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("exec", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  char* error_text = nullptr;
  const int result = sqlite3_exec(connection, sql.c_str(), nullptr, nullptr, &error_text);
  if (result != SQLITE_OK) {
    std::string copied;
    if (error_text != nullptr) {
      copied = error_text;
      sqlite3_free(error_text);
    } else {
      const char* text = sqlite3_errmsg(connection);
      copied = text != nullptr ? text : "";
    }
    const int extended = sqlite3_extended_errcode(connection);
    sqlite_detail::store_failure(result, extended, String(copied));
    return sqlite_detail::table().code;
  }
  return 0;
}

inline std::int32_t sqlite_busy_timeout_i(std::int32_t connection_handle, std::int32_t milliseconds) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("busy_timeout", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  const int result = sqlite3_busy_timeout(connection, milliseconds);
  if (result != SQLITE_OK) {
    sqlite_detail::store_sqlite_failure(connection, result);
    return sqlite_detail::table().code;
  }
  return 0;
}

inline std::int32_t sqlite_changes_i(std::int32_t connection_handle) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("changes", "invalid or closed handle");
    return -1;
  }
  return static_cast<std::int32_t>(sqlite3_changes(connection));
}

inline std::int64_t sqlite_last_insert_rowid_l(std::int32_t connection_handle) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("last_insert_rowid", "invalid or closed handle");
    return 0;
  }
  return static_cast<std::int64_t>(sqlite3_last_insert_rowid(connection));
}

inline std::int32_t sqlite_autocommit_i(std::int32_t connection_handle) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("in_transaction", "invalid or closed handle");
    return -1;
  }
  return sqlite3_get_autocommit(connection) != 0 ? 1 : 0;
}

inline std::int32_t sqlite_prepare_i(std::int32_t connection_handle, String sql) {
  sqlite_detail::clear_error();
  sqlite3* connection = sqlite_detail::find_connection(connection_handle);
  if (connection == nullptr) {
    sqlite_detail::store_named_failure("prepare", "invalid or closed handle");
    return 0;
  }
  sqlite3_stmt* statement = nullptr;
  const char* tail = nullptr;
  const int result = sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, &tail);
  if (result != SQLITE_OK) {
    if (statement != nullptr) {
      sqlite3_finalize(statement);
    }
    sqlite_detail::store_sqlite_failure(connection, result);
    return 0;
  }
  if (statement == nullptr) {
    sqlite_detail::store_named_failure("prepare", "empty SQL");
    return 0;
  }
  sqlite3_stmt* extra = nullptr;
  const char* extra_tail = nullptr;
  const char* rest = tail != nullptr ? tail : "";
  const int extra_result = sqlite3_prepare_v2(connection, rest, -1, &extra, &extra_tail);
  if (extra != nullptr || extra_result != SQLITE_OK) {
    if (extra != nullptr) {
      sqlite3_finalize(extra);
    }
    sqlite3_finalize(statement);
    if (extra != nullptr && extra_result == SQLITE_OK) {
      sqlite_detail::store_named_failure("prepare", "trailing SQL");
    } else {
      sqlite_detail::store_sqlite_failure(connection, extra_result != SQLITE_OK ? extra_result : 21);
    }
    return 0;
  }
  const std::int32_t handle = sqlite_detail::take_handle();
  if (handle == 0) {
    sqlite3_finalize(statement);
    return 0;
  }
  sqlite_detail::StatementEntry entry;
  entry.statement = statement;
  entry.connection_handle = connection_handle;
  entry.step_result = SQLITE_OK;
  sqlite_detail::table().statements.emplace(handle, entry);
  return handle;
}

inline std::int32_t bind_status(const char* function_name, std::int32_t statement_handle, int result) {
  if (result == SQLITE_OK) {
    return 0;
  }
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  sqlite3* connection = nullptr;
  if (entry != nullptr) {
    connection = sqlite_detail::find_connection(entry->connection_handle);
  }
  if (connection != nullptr) {
    sqlite_detail::store_sqlite_failure(connection, result);
  } else {
    sqlite_detail::store_named_failure(function_name, "invalid or closed handle");
  }
  return sqlite_detail::table().code;
}

inline std::int32_t sqlite_bind_null_i(std::int32_t statement_handle, std::int32_t parameter_index) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("bind_null", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  return bind_status("bind_null", statement_handle, sqlite3_bind_null(entry->statement, parameter_index));
}

inline std::int32_t sqlite_bind_i64_i(std::int32_t statement_handle, std::int32_t parameter_index, std::int64_t value) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("bind_i64", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  return bind_status(
      "bind_i64",
      statement_handle,
      sqlite3_bind_int64(entry->statement, parameter_index, static_cast<sqlite3_int64>(value)));
}

inline std::int32_t sqlite_bind_f64_i(std::int32_t statement_handle, std::int32_t parameter_index, double value) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("bind_f64", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  return bind_status("bind_f64", statement_handle, sqlite3_bind_double(entry->statement, parameter_index, value));
}

inline std::int32_t sqlite_bind_text_i(std::int32_t statement_handle, std::int32_t parameter_index, String value) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("bind_text", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  const char* data = value.raw_data();
  int length = 0;
  const int size = value.byte_size();
  while (length < size && data[length] != '\0') {
    length += 1;
  }
  return bind_status(
      "bind_text",
      statement_handle,
      sqlite3_bind_text(entry->statement, parameter_index, data, length, SQLITE_TRANSIENT));
}

inline std::int32_t sqlite_bind_blob_hex_i(std::int32_t statement_handle, std::int32_t parameter_index, String hex) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("bind_blob_hex", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  const int size = hex.byte_size();
  if ((size % 2) != 0) {
    sqlite_detail::store_named_failure("bind_blob_hex", "invalid hex");
    return sqlite_detail::table().code;
  }
  std::vector<unsigned char> bytes;
  bytes.resize(static_cast<std::size_t>(size / 2));
  const char* data = hex.raw_data();
  for (int index = 0; index < size; index += 2) {
    const int high = sqlite_detail::hex_value(data[index]);
    const int low = sqlite_detail::hex_value(data[index + 1]);
    if (high < 0 || low < 0) {
      sqlite_detail::store_named_failure("bind_blob_hex", "invalid hex");
      return sqlite_detail::table().code;
    }
    bytes[static_cast<std::size_t>(index / 2)] = static_cast<unsigned char>((high << 4) | low);
  }
  const void* pointer = bytes.empty() ? static_cast<const void*>("") : static_cast<const void*>(bytes.data());
  return bind_status(
      "bind_blob_hex",
      statement_handle,
      sqlite3_bind_blob(
          entry->statement,
          parameter_index,
          pointer,
          static_cast<int>(bytes.size()),
          SQLITE_TRANSIENT));
}

inline std::int32_t sqlite_step_i(std::int32_t statement_handle) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("step", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  const int result = sqlite3_step(entry->statement);
  entry->step_result = result;
  if (result == SQLITE_ROW || result == SQLITE_DONE) {
    return result;
  }
  sqlite3* connection = sqlite_detail::find_connection(entry->connection_handle);
  sqlite_detail::store_sqlite_failure(connection, result);
  return sqlite_detail::table().code;
}

inline std::int32_t sqlite_reset_i(std::int32_t statement_handle) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("reset", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  const int previous = entry->step_result;
  const int result = sqlite3_reset(entry->statement);
  entry->step_result = SQLITE_OK;
  if (result != SQLITE_OK && result == previous) {
    return 0;
  }
  if (result != SQLITE_OK) {
    sqlite3* connection = sqlite_detail::find_connection(entry->connection_handle);
    sqlite_detail::store_sqlite_failure(connection, result);
    return sqlite_detail::table().code;
  }
  return 0;
}

inline std::int32_t sqlite_clear_bindings_i(std::int32_t statement_handle) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("clear_bindings", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  const int result = sqlite3_clear_bindings(entry->statement);
  if (result != SQLITE_OK) {
    sqlite3* connection = sqlite_detail::find_connection(entry->connection_handle);
    sqlite_detail::store_sqlite_failure(connection, result);
    return sqlite_detail::table().code;
  }
  return 0;
}

inline std::int32_t sqlite_finalize_i(std::int32_t statement_handle) {
  sqlite_detail::clear_error();
  auto found = sqlite_detail::table().statements.find(statement_handle);
  if (found == sqlite_detail::table().statements.end() || found->second.statement == nullptr) {
    sqlite_detail::store_named_failure("finalize", "invalid or closed handle");
    return sqlite_detail::table().code;
  }
  sqlite3_finalize(found->second.statement);
  found->second.statement = nullptr;
  sqlite_detail::table().statements.erase(found);
  sqlite_detail::clear_error();
  return 0;
}

inline std::int32_t sqlite_column_count_i(std::int32_t statement_handle) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("column_count", "invalid or closed handle");
    return -1;
  }
  return static_cast<std::int32_t>(sqlite3_column_count(entry->statement));
}

inline std::int32_t sqlite_column_type_i(std::int32_t statement_handle, std::int32_t column_index) {
  sqlite_detail::clear_error();
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    sqlite_detail::store_named_failure("column", "invalid or closed handle");
    return -1;
  }
  const int available = sqlite3_data_count(entry->statement);
  if (available <= 0) {
    sqlite_detail::store_named_failure("column", "no current row");
    return -1;
  }
  if (column_index < 0 || column_index >= available) {
    sqlite_detail::store_failure(25, 25, String("Sqlite.column: column index out of range"));
    return -1;
  }
  return static_cast<std::int32_t>(sqlite3_column_type(entry->statement, column_index));
}

inline std::int64_t sqlite_column_i64_l(std::int32_t statement_handle, std::int32_t column_index) {
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    return 0;
  }
  return static_cast<std::int64_t>(sqlite3_column_int64(entry->statement, column_index));
}

inline double sqlite_column_f64_d(std::int32_t statement_handle, std::int32_t column_index) {
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    return 0.0;
  }
  return sqlite3_column_double(entry->statement, column_index);
}

inline String sqlite_column_text_s(std::int32_t statement_handle, std::int32_t column_index) {
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    return String();
  }
  const int size = sqlite3_column_bytes(entry->statement, column_index);
  const unsigned char* text = sqlite3_column_text(entry->statement, column_index);
  if (text == nullptr || size <= 0) {
    return String();
  }
  return String(reinterpret_cast<const char*>(text), static_cast<std::size_t>(size));
}

inline String sqlite_column_blob_hex_s(std::int32_t statement_handle, std::int32_t column_index) {
  sqlite_detail::StatementEntry* entry = sqlite_detail::find_statement(statement_handle);
  if (entry == nullptr) {
    return String();
  }
  const int size = sqlite3_column_bytes(entry->statement, column_index);
  if (size <= 0) {
    return String();
  }
  const void* blob = sqlite3_column_blob(entry->statement, column_index);
  if (blob == nullptr) {
    return String();
  }
  return sqlite_detail::bytes_to_hex(static_cast<const unsigned char*>(blob), size);
}

inline std::int32_t sqlite_last_error_code_i() {
  return sqlite_detail::table().code;
}

inline std::int32_t sqlite_last_error_extended_i() {
  return sqlite_detail::table().extended;
}

inline String sqlite_last_error_message_s() {
  return sqlite_detail::table().message;
}

}  // namespace db
}  // namespace mlc
