#pragma once

// Blocking Postgres client via libpq (TRACK_STDLIB_POSTGRES STEP=2).
// MLC surface: opaque i32 handles + last_error (Tcp pattern).

#include "mlc/core/string.hpp"

#if __has_include(<libpq-fe.h>)
#include <libpq-fe.h>
#elif __has_include(<postgresql/libpq-fe.h>)
#include <postgresql/libpq-fe.h>
#else
#error "mlc/db/postgres.hpp requires libpq-fe.h (install libpq-dev)"
#endif

#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace mlc {
namespace db {

namespace detail {

struct PostgresHandleTable {
  std::mutex mutex;
  std::vector<PGconn*> connections;
  std::vector<PGresult*> results;
  String last_error;
};

inline PostgresHandleTable& postgres_handle_table() {
  static PostgresHandleTable table;
  return table;
}

inline void set_last_error_unlocked(String message) {
  postgres_handle_table().last_error = std::move(message);
}

inline void set_last_error(String message) {
  std::lock_guard<std::mutex> lock(postgres_handle_table().mutex);
  set_last_error_unlocked(std::move(message));
}

inline String pq_error_message(PGconn* connection) {
  const char* message = PQerrorMessage(connection);
  if (message == nullptr || message[0] == '\0') {
    return String("Postgres: unknown libpq error");
  }
  return String(message);
}

inline bool result_status_ok(ExecStatusType status) {
  return status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK;
}

} // namespace detail

inline String last_error() {
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  return detail::postgres_handle_table().last_error;
}

inline std::optional<std::int32_t> connect(const String& conninfo) {
  PGconn* connection = PQconnectdb(conninfo.c_str());
  if (connection == nullptr) {
    detail::set_last_error(String("Postgres.connect: PQconnectdb returned null"));
    return std::nullopt;
  }
  if (PQstatus(connection) != CONNECTION_OK) {
    detail::set_last_error(detail::pq_error_message(connection));
    PQfinish(connection);
    return std::nullopt;
  }
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  detail::postgres_handle_table().connections.push_back(connection);
  return static_cast<std::int32_t>(detail::postgres_handle_table().connections.size());
}

inline std::optional<std::int32_t> exec(std::int32_t connection_handle, const String& sql) {
  PGconn* connection = nullptr;
  {
    std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
    if (connection_handle <= 0 ||
        static_cast<std::size_t>(connection_handle) >
            detail::postgres_handle_table().connections.size()) {
      detail::set_last_error_unlocked(String("Postgres.exec: invalid connection handle"));
      return std::nullopt;
    }
    connection =
        detail::postgres_handle_table().connections[static_cast<std::size_t>(connection_handle) - 1];
  }
  if (connection == nullptr) {
    detail::set_last_error(String("Postgres.exec: connection closed"));
    return std::nullopt;
  }
  PGresult* result = PQexec(connection, sql.c_str());
  if (result == nullptr) {
    detail::set_last_error(detail::pq_error_message(connection));
    return std::nullopt;
  }
  const ExecStatusType status = PQresultStatus(result);
  if (!detail::result_status_ok(status)) {
    const char* message = PQresultErrorMessage(result);
    if (message == nullptr || message[0] == '\0') {
      detail::set_last_error(detail::pq_error_message(connection));
    } else {
      detail::set_last_error(String(message));
    }
    PQclear(result);
    return std::nullopt;
  }
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  detail::postgres_handle_table().results.push_back(result);
  return static_cast<std::int32_t>(detail::postgres_handle_table().results.size());
}

inline std::int32_t ntuples(std::int32_t result_handle) {
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  if (result_handle <= 0 ||
      static_cast<std::size_t>(result_handle) > detail::postgres_handle_table().results.size()) {
    return 0;
  }
  PGresult* result =
      detail::postgres_handle_table().results[static_cast<std::size_t>(result_handle) - 1];
  if (result == nullptr) {
    return 0;
  }
  return static_cast<std::int32_t>(PQntuples(result));
}

inline std::int32_t nfields(std::int32_t result_handle) {
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  if (result_handle <= 0 ||
      static_cast<std::size_t>(result_handle) > detail::postgres_handle_table().results.size()) {
    return 0;
  }
  PGresult* result =
      detail::postgres_handle_table().results[static_cast<std::size_t>(result_handle) - 1];
  if (result == nullptr) {
    return 0;
  }
  return static_cast<std::int32_t>(PQnfields(result));
}

inline std::optional<String> getvalue(
    std::int32_t result_handle,
    std::int32_t row,
    std::int32_t column
) {
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  if (result_handle <= 0 ||
      static_cast<std::size_t>(result_handle) > detail::postgres_handle_table().results.size()) {
    detail::set_last_error_unlocked(String("Postgres.getvalue: invalid result handle"));
    return std::nullopt;
  }
  PGresult* result =
      detail::postgres_handle_table().results[static_cast<std::size_t>(result_handle) - 1];
  if (result == nullptr) {
    detail::set_last_error_unlocked(String("Postgres.getvalue: result cleared"));
    return std::nullopt;
  }
  if (row < 0 || column < 0 || row >= PQntuples(result) || column >= PQnfields(result)) {
    detail::set_last_error_unlocked(String("Postgres.getvalue: row/column out of range"));
    return std::nullopt;
  }
  if (PQgetisnull(result, row, column) != 0) {
    return std::nullopt;
  }
  const char* value = PQgetvalue(result, row, column);
  if (value == nullptr) {
    return std::nullopt;
  }
  return String(value);
}

inline void clear(std::int32_t result_handle) {
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  if (result_handle <= 0 ||
      static_cast<std::size_t>(result_handle) > detail::postgres_handle_table().results.size()) {
    return;
  }
  PGresult*& result =
      detail::postgres_handle_table().results[static_cast<std::size_t>(result_handle) - 1];
  if (result != nullptr) {
    PQclear(result);
    result = nullptr;
  }
}

inline void finish(std::int32_t connection_handle) {
  std::lock_guard<std::mutex> lock(detail::postgres_handle_table().mutex);
  if (connection_handle <= 0 ||
      static_cast<std::size_t>(connection_handle) >
          detail::postgres_handle_table().connections.size()) {
    return;
  }
  PGconn*& connection =
      detail::postgres_handle_table().connections[static_cast<std::size_t>(connection_handle) - 1];
  if (connection != nullptr) {
    PQfinish(connection);
    connection = nullptr;
  }
}

} // namespace db
} // namespace mlc
