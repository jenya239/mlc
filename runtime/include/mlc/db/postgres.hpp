#pragma once

// Blocking Postgres client via libpq (Ruby / C++ path).
// TRACK_FFI_SHIM_MIGRATION STEP=2: control flow mirrors postgres.mlc; storage
// and String→cstr live in postgres_abi.hpp (shared with mlcc MLC wrappers).

#include "mlc/core/string.hpp"
#include "mlc/db/postgres_abi.hpp"

#include <cstdint>
#include <optional>

namespace mlc {
namespace db {

inline String last_error() {
  return table_last_error();
}

inline std::optional<std::int32_t> connect(const String& conninfo) {
  const std::int64_t connection = pq_connectdb_s(conninfo);
  if (connection == 0) {
    table_set_error(String("Postgres.connect: PQconnectdb returned null"));
    return std::nullopt;
  }
  if (pq_status_i(connection) != 0) {
    table_set_error(pq_error_message_s(connection));
    pq_finish_p(connection);
    return std::nullopt;
  }
  return table_push_conn(connection);
}

inline std::optional<std::int32_t> exec(std::int32_t connection_handle, const String& sql) {
  const std::int64_t connection = table_get_conn(connection_handle);
  if (connection == 0) {
    table_set_error(String("Postgres.exec: invalid or closed connection handle"));
    return std::nullopt;
  }
  const std::int64_t result = pq_exec_s(connection, sql);
  if (result == 0) {
    table_set_error(pq_error_message_s(connection));
    return std::nullopt;
  }
  const std::int32_t status = pq_result_status_i(result);
  if (!(status == 2 || status == 1)) {
    const String message = pq_result_error_message_s(result);
    if (message.size() == 0) {
      table_set_error(pq_error_message_s(connection));
    } else {
      table_set_error(message);
    }
    pq_clear_p(result);
    return std::nullopt;
  }
  return table_push_result(result);
}

inline std::int32_t ntuples(std::int32_t result_handle) {
  const std::int64_t result = table_get_result(result_handle);
  if (result == 0) {
    return 0;
  }
  return pq_ntuples_i(result);
}

inline std::int32_t nfields(std::int32_t result_handle) {
  const std::int64_t result = table_get_result(result_handle);
  if (result == 0) {
    return 0;
  }
  return pq_nfields_i(result);
}

inline std::optional<String> getvalue(
    std::int32_t result_handle,
    std::int32_t row,
    std::int32_t column
) {
  const std::int64_t result = table_get_result(result_handle);
  if (result == 0) {
    table_set_error(String("Postgres.getvalue: invalid or cleared result handle"));
    return std::nullopt;
  }
  if (row < 0 || column < 0 || row >= pq_ntuples_i(result) || column >= pq_nfields_i(result)) {
    table_set_error(String("Postgres.getvalue: row/column out of range"));
    return std::nullopt;
  }
  if (pq_getisnull_i(result, row, column) != 0) {
    return std::nullopt;
  }
  return pq_getvalue_s(result, row, column);
}

inline void clear(std::int32_t result_handle) {
  table_clear_result(result_handle);
}

inline void finish(std::int32_t connection_handle) {
  table_finish_conn(connection_handle);
}

} // namespace db
} // namespace mlc
