#pragma once

// TRACK_STDLIB_WEBSOCKET_TO_MLC STEP=5 — connection handle table + error slot.
// Protocol (SHA1/frames/HTTP) lives in websocket.mlc.

#include "mlc/core/string.hpp"
#include "mlc/net/tcp_abi.hpp"
#include "mlc/net/websocket_abi.hpp"

#include <cstdint>
#include <mutex>
#include <string>
#include <variant>
#include <vector>

namespace mlc {
namespace websocket {

namespace detail {

struct ConnectionEntry {
  int file_descriptor = -1;
  std::string read_buffer;
  bool closed = false;
};

struct ConnectionTable {
  std::mutex mutex;
  std::vector<ConnectionEntry> connections;
  String last_error;
};

inline ConnectionTable& connection_table() {
  static ConnectionTable table;
  return table;
}

inline ConnectionEntry* connection_at(std::int32_t handle) {
  if (handle <= 0) {
    return nullptr;
  }
  const std::size_t index = static_cast<std::size_t>(handle) - 1u;
  if (index >= connection_table().connections.size()) {
    return nullptr;
  }
  return &connection_table().connections[index];
}

} // namespace detail

inline void set_error(String message) {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::connection_table().last_error = std::move(message);
}

inline void clear_error() {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::connection_table().last_error = String();
}

inline String last_error_mlc() {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  return detail::connection_table().last_error;
}

inline void table_set_error(String message) {
  set_error(std::move(message));
}

inline String table_last_error() {
  return last_error_mlc();
}

// Register an owned Tcp fd as a WebSocket connection. Returns 1-based handle.
inline std::int32_t conn_open(std::int32_t file_descriptor) {
  if (file_descriptor < 0) {
    set_error(String("WebSocket: invalid stream handle"));
    return -1;
  }
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::ConnectionEntry entry;
  entry.file_descriptor = file_descriptor;
  detail::connection_table().connections.push_back(std::move(entry));
  detail::connection_table().last_error = String();
  return static_cast<std::int32_t>(detail::connection_table().connections.size());
}

inline std::int32_t conn_fd(std::int32_t handle) {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::ConnectionEntry* entry = detail::connection_at(handle);
  if (entry == nullptr || entry->closed || entry->file_descriptor < 0) {
    return -1;
  }
  return entry->file_descriptor;
}

inline std::int32_t conn_is_open(std::int32_t handle) {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::ConnectionEntry* entry = detail::connection_at(handle);
  if (entry == nullptr || entry->closed || entry->file_descriptor < 0) {
    return 0;
  }
  return 1;
}

inline String conn_get_buffer(std::int32_t handle) {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::ConnectionEntry* entry = detail::connection_at(handle);
  if (entry == nullptr) {
    return String();
  }
  return String(entry->read_buffer);
}

inline void conn_set_buffer(std::int32_t handle, String buffer) {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::ConnectionEntry* entry = detail::connection_at(handle);
  if (entry == nullptr) {
    return;
  }
  entry->read_buffer.assign(buffer.view().data(), buffer.view().size());
}

inline void conn_close(std::int32_t handle) {
  std::lock_guard<std::mutex> lock(detail::connection_table().mutex);
  detail::ConnectionEntry* entry = detail::connection_at(handle);
  if (entry == nullptr) {
    return;
  }
  if (entry->file_descriptor >= 0) {
    net::tcp_close(entry->file_descriptor);
    entry->file_descriptor = -1;
  }
  entry->closed = true;
  entry->read_buffer.clear();
}

// Thin Tcp I/O for MLC WebSocket bodies (avoid Option match in websocket.mlc).
inline String ws_tcp_read(std::int32_t file_descriptor, std::int32_t max_bytes) {
  return net::tcp_recv(file_descriptor, max_bytes);
}

inline std::int32_t ws_tcp_read_status() {
  return net::tcp_recv_status();
}

inline std::int32_t ws_tcp_write_all(std::int32_t file_descriptor, String data) {
  return net::tcp_send_all(file_descriptor, data);
}

// Ruby MLC.compile residual (registry :extern → mlc::websocket::*).
// Real protocol is MLC (mlcc). These symbols satisfy Ruby codegen links.
struct WsHandleOk {
  std::int32_t field0 = 0;
};
struct WsHandleNone {};
using WsHandleResult = std::variant<WsHandleOk, WsHandleNone>;

struct WsTextOk {
  String field0;
};
struct WsTextNone {};
using WsTextResult = std::variant<WsTextOk, WsTextNone>;

inline WsHandleResult upgrade(std::int32_t /*stream*/) {
  set_error(String("WebSocket.upgrade: use mlcc (MLC bodies); Ruby residual stub"));
  return WsHandleNone{};
}

inline WsTextResult read_text(std::int32_t /*connection*/) {
  set_error(String("WebSocket.read_text: use mlcc (MLC bodies); Ruby residual stub"));
  return WsTextNone{};
}

inline bool write_text(std::int32_t /*connection*/, const String& /*data*/) {
  set_error(String("WebSocket.write_text: use mlcc (MLC bodies); Ruby residual stub"));
  return false;
}

inline void close(std::int32_t connection) {
  conn_close(connection);
}

inline String last_error() {
  return last_error_mlc();
}

} // namespace websocket
} // namespace mlc
