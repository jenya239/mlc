#pragma once

// Thin libpq ABI + i32 handle table for TRACK_FFI_SHIM_MIGRATION STEP=2.
// Pointers cross the MLC boundary as i64 (Ruby lacks `extern type` / RawPointer
// PGconn). Business Option/status logic lives in postgres.mlc (mlcc).

#include "mlc/core/string.hpp"

#if __has_include(<libpq-fe.h>)
#include <libpq-fe.h>
#elif __has_include(<postgresql/libpq-fe.h>)
#include <postgresql/libpq-fe.h>
#else
#error "mlc/db/postgres_abi.hpp requires libpq-fe.h (install libpq-dev)"
#endif

#include <cstdint>
#include <vector>

namespace mlc {
namespace db {

namespace abi_detail {

struct PostgresAbiTable {
  std::vector<PGconn*> connections;
  std::vector<PGresult*> results;
  String last_error;
};

inline PostgresAbiTable& postgres_abi_table() {
  static PostgresAbiTable table;
  return table;
}

inline std::int64_t pointer_to_i64(void* pointer) {
  return reinterpret_cast<std::int64_t>(pointer);
}

inline PGconn* i64_to_conn(std::int64_t pointer) {
  return reinterpret_cast<PGconn*>(pointer);
}

inline PGresult* i64_to_result(std::int64_t pointer) {
  return reinterpret_cast<PGresult*>(pointer);
}

} // namespace abi_detail

inline std::int64_t pq_connectdb_s(String conninfo) {
  return abi_detail::pointer_to_i64(PQconnectdb(conninfo.c_str()));
}

inline std::int32_t pq_status_i(std::int64_t connection) {
  PGconn* pointer = abi_detail::i64_to_conn(connection);
  if (pointer == nullptr) {
    return static_cast<std::int32_t>(CONNECTION_BAD);
  }
  return static_cast<std::int32_t>(PQstatus(pointer));
}

inline void pq_finish_p(std::int64_t connection) {
  PGconn* pointer = abi_detail::i64_to_conn(connection);
  if (pointer != nullptr) {
    PQfinish(pointer);
  }
}

inline std::int64_t pq_exec_s(std::int64_t connection, String sql) {
  PGconn* pointer = abi_detail::i64_to_conn(connection);
  if (pointer == nullptr) {
    return 0;
  }
  return abi_detail::pointer_to_i64(PQexec(pointer, sql.c_str()));
}

inline std::int32_t pq_result_status_i(std::int64_t result) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer == nullptr) {
    return static_cast<std::int32_t>(PGRES_FATAL_ERROR);
  }
  return static_cast<std::int32_t>(PQresultStatus(pointer));
}

inline void pq_clear_p(std::int64_t result) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer != nullptr) {
    PQclear(pointer);
  }
}

inline std::int32_t pq_ntuples_i(std::int64_t result) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer == nullptr) {
    return 0;
  }
  return static_cast<std::int32_t>(PQntuples(pointer));
}

inline std::int32_t pq_nfields_i(std::int64_t result) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer == nullptr) {
    return 0;
  }
  return static_cast<std::int32_t>(PQnfields(pointer));
}

inline std::int32_t pq_getisnull_i(std::int64_t result, std::int32_t row, std::int32_t column) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer == nullptr) {
    return 1;
  }
  return static_cast<std::int32_t>(PQgetisnull(pointer, row, column));
}

inline String pq_getvalue_s(std::int64_t result, std::int32_t row, std::int32_t column) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer == nullptr) {
    return String();
  }
  const char* value = PQgetvalue(pointer, row, column);
  if (value == nullptr) {
    return String();
  }
  return String(value);
}

inline String pq_error_message_s(std::int64_t connection) {
  PGconn* pointer = abi_detail::i64_to_conn(connection);
  if (pointer == nullptr) {
    return String("Postgres: null connection");
  }
  const char* message = PQerrorMessage(pointer);
  if (message == nullptr || message[0] == '\0') {
    return String("Postgres: unknown libpq error");
  }
  return String(message);
}

inline String pq_result_error_message_s(std::int64_t result) {
  PGresult* pointer = abi_detail::i64_to_result(result);
  if (pointer == nullptr) {
    return String("Postgres: null result");
  }
  const char* message = PQresultErrorMessage(pointer);
  if (message == nullptr || message[0] == '\0') {
    return String();
  }
  return String(message);
}

inline std::int32_t table_push_conn(std::int64_t connection) {
  abi_detail::postgres_abi_table().connections.push_back(abi_detail::i64_to_conn(connection));
  return static_cast<std::int32_t>(abi_detail::postgres_abi_table().connections.size());
}

inline std::int64_t table_get_conn(std::int32_t handle) {
  if (handle <= 0 ||
      static_cast<std::size_t>(handle) > abi_detail::postgres_abi_table().connections.size()) {
    return 0;
  }
  return abi_detail::pointer_to_i64(
      abi_detail::postgres_abi_table().connections[static_cast<std::size_t>(handle) - 1]);
}

inline void table_finish_conn(std::int32_t handle) {
  if (handle <= 0 ||
      static_cast<std::size_t>(handle) > abi_detail::postgres_abi_table().connections.size()) {
    return;
  }
  PGconn*& connection =
      abi_detail::postgres_abi_table().connections[static_cast<std::size_t>(handle) - 1];
  if (connection != nullptr) {
    PQfinish(connection);
    connection = nullptr;
  }
}

inline std::int32_t table_push_result(std::int64_t result) {
  abi_detail::postgres_abi_table().results.push_back(abi_detail::i64_to_result(result));
  return static_cast<std::int32_t>(abi_detail::postgres_abi_table().results.size());
}

inline std::int64_t table_get_result(std::int32_t handle) {
  if (handle <= 0 ||
      static_cast<std::size_t>(handle) > abi_detail::postgres_abi_table().results.size()) {
    return 0;
  }
  return abi_detail::pointer_to_i64(
      abi_detail::postgres_abi_table().results[static_cast<std::size_t>(handle) - 1]);
}

inline void table_clear_result(std::int32_t handle) {
  if (handle <= 0 ||
      static_cast<std::size_t>(handle) > abi_detail::postgres_abi_table().results.size()) {
    return;
  }
  PGresult*& result =
      abi_detail::postgres_abi_table().results[static_cast<std::size_t>(handle) - 1];
  if (result != nullptr) {
    PQclear(result);
    result = nullptr;
  }
}

inline void table_set_error(String message) {
  abi_detail::postgres_abi_table().last_error = std::move(message);
}

inline String table_last_error() {
  return abi_detail::postgres_abi_table().last_error;
}

} // namespace db
} // namespace mlc
