#pragma once

// Ruby/C++ TCP surface (TRACK_FFI_SHIM_MIGRATION STEP=5).
// Former tcp.hpp — fd-as-token handle API + TcpListener/TcpStream over tcp_abi.hpp.
// Residual RAII classes for websocket until WEBSOCKET track.

#include "mlc/core/result.hpp"
#include "mlc/core/string.hpp"
#include "mlc/net/tcp_abi.hpp"

#include <cstdint>
#include <optional>
#include <utility>
#include <variant>

namespace mlc {
namespace net {

class TcpStream {
  int file_descriptor_ = -1;

  explicit TcpStream(int file_descriptor) : file_descriptor_(file_descriptor) {}

  friend class TcpListener;

public:
  // Adopt a raw fd (fd-as-token / WebSocket.upgrade). Caller must not close fd.
  [[nodiscard]] static TcpStream adopt(std::int32_t file_descriptor) {
    return TcpStream(file_descriptor);
  }

  TcpStream() = default;

  TcpStream(const TcpStream&) = delete;
  TcpStream& operator=(const TcpStream&) = delete;

  TcpStream(TcpStream&& other) noexcept : file_descriptor_(other.file_descriptor_) {
    other.file_descriptor_ = -1;
  }

  TcpStream& operator=(TcpStream&& other) noexcept {
    if (this != &other) {
      tcp_close(file_descriptor_);
      file_descriptor_ = other.file_descriptor_;
      other.file_descriptor_ = -1;
    }
    return *this;
  }

  ~TcpStream() { tcp_close(file_descriptor_); }

  [[nodiscard]] bool is_open() const noexcept { return file_descriptor_ >= 0; }

  void close() {
    tcp_close(file_descriptor_);
    file_descriptor_ = -1;
  }

  [[nodiscard]] result::Result<String, String> read(std::int32_t max_bytes) {
    const String data = tcp_recv(file_descriptor_, max_bytes);
    if (tcp_recv_status() < 0) {
      return result::err(table_last_error());
    }
    return result::ok(data);
  }

  [[nodiscard]] result::Result<bool, String> write_all(const String& data) {
    if (tcp_send_all(file_descriptor_, data) == 0) {
      return result::err(table_last_error());
    }
    return result::ok(true);
  }
};

class TcpListener {
  int file_descriptor_ = -1;

  explicit TcpListener(int file_descriptor) : file_descriptor_(file_descriptor) {}

public:
  TcpListener() = default;

  TcpListener(const TcpListener&) = delete;
  TcpListener& operator=(const TcpListener&) = delete;

  TcpListener(TcpListener&& other) noexcept : file_descriptor_(other.file_descriptor_) {
    other.file_descriptor_ = -1;
  }

  TcpListener& operator=(TcpListener&& other) noexcept {
    if (this != &other) {
      tcp_close(file_descriptor_);
      file_descriptor_ = other.file_descriptor_;
      other.file_descriptor_ = -1;
    }
    return *this;
  }

  ~TcpListener() { tcp_close(file_descriptor_); }

  [[nodiscard]] bool is_open() const noexcept { return file_descriptor_ >= 0; }

  [[nodiscard]] std::int32_t port() const noexcept {
    const std::int32_t bound = tcp_getsockname_port(file_descriptor_);
    return bound < 0 ? 0 : bound;
  }

  void close() {
    tcp_close(file_descriptor_);
    file_descriptor_ = -1;
  }

  [[nodiscard]] static result::Result<TcpListener, String> bind(
      const String& host,
      std::int32_t port
  ) {
    if (port < 0 || port > 65535) {
      return result::err(String("TcpListener.bind: port out of range"));
    }
    const std::int32_t listen_socket = tcp_socket();
    if (listen_socket < 0) {
      return result::err(tcp_errno_message(String("TcpListener.bind: socket")));
    }
    if (tcp_set_reuseaddr(listen_socket) < 0) {
      tcp_close(listen_socket);
      return result::err(tcp_errno_message(String("TcpListener.bind: setsockopt")));
    }
    if (tcp_bind_ipv4(listen_socket, host, port) < 0) {
      const String message = table_last_error();
      tcp_close(listen_socket);
      return result::err(message);
    }
    if (tcp_listen(listen_socket, 128) < 0) {
      tcp_close(listen_socket);
      return result::err(tcp_errno_message(String("TcpListener.bind: listen")));
    }
    if (tcp_getsockname_port(listen_socket) < 0) {
      const String message = table_last_error();
      tcp_close(listen_socket);
      return result::err(message);
    }
    return result::ok(TcpListener(listen_socket));
  }

  [[nodiscard]] result::Result<TcpStream, String> accept() {
    if (file_descriptor_ < 0) {
      return result::err(String("TcpListener.accept: closed"));
    }
    const std::int32_t client_socket = tcp_accept(file_descriptor_);
    if (client_socket < 0) {
      return result::err(tcp_errno_message(String("TcpListener.accept")));
    }
    return result::ok(TcpStream(client_socket));
  }
};

// MLC stdlib bridge: opaque i32 tokens == real fds (fd-as-token).

inline String last_error() {
  return table_last_error();
}

inline std::optional<std::int32_t> bind(const String& host, std::int32_t port) {
  if (port < 0 || port > 65535) {
    table_set_error(String("TcpListener.bind: port out of range"));
    return std::nullopt;
  }
  const std::int32_t listen_socket = tcp_socket();
  if (listen_socket < 0) {
    table_set_error(tcp_errno_message(String("TcpListener.bind: socket")));
    return std::nullopt;
  }
  if (tcp_set_reuseaddr(listen_socket) < 0) {
    table_set_error(tcp_errno_message(String("TcpListener.bind: setsockopt")));
    tcp_close(listen_socket);
    return std::nullopt;
  }
  if (tcp_bind_ipv4(listen_socket, host, port) < 0) {
    tcp_close(listen_socket);
    return std::nullopt;
  }
  if (tcp_listen(listen_socket, 128) < 0) {
    table_set_error(tcp_errno_message(String("TcpListener.bind: listen")));
    tcp_close(listen_socket);
    return std::nullopt;
  }
  if (tcp_getsockname_port(listen_socket) < 0) {
    tcp_close(listen_socket);
    return std::nullopt;
  }
  return listen_socket;
}

inline std::optional<std::int32_t> accept(std::int32_t listener_fd) {
  if (listener_fd < 0) {
    table_set_error(String("Tcp.accept: listener closed"));
    return std::nullopt;
  }
  const std::int32_t client_socket = tcp_accept(listener_fd);
  if (client_socket < 0) {
    table_set_error(tcp_errno_message(String("TcpListener.accept")));
    return std::nullopt;
  }
  return client_socket;
}

inline std::optional<String> read(std::int32_t stream_fd, std::int32_t max_bytes) {
  const String data = tcp_recv(stream_fd, max_bytes);
  if (tcp_recv_status() < 0) {
    return std::nullopt;
  }
  return data;
}

inline bool set_recv_timeout(std::int32_t stream_fd, std::int32_t timeout_seconds) {
  if (tcp_set_recv_timeout(stream_fd, timeout_seconds) < 0) {
    table_set_error(tcp_errno_message(String("Tcp.set_recv_timeout")));
    return false;
  }
  return true;
}

inline bool write_all(std::int32_t stream_fd, const String& data) {
  return tcp_send_all(stream_fd, data) != 0;
}

inline void close_listener(std::int32_t listener_fd) {
  tcp_close(listener_fd);
}

inline void close_stream(std::int32_t stream_fd) {
  tcp_close(stream_fd);
}

inline std::int32_t port(std::int32_t listener_fd) {
  const std::int32_t bound = tcp_getsockname_port(listener_fd);
  return bound < 0 ? 0 : bound;
}

inline std::optional<std::int32_t> bind_mlc(String host, std::int32_t port_value) {
  return bind(host, port_value);
}
inline std::optional<std::int32_t> accept_mlc(std::int32_t listener_fd) {
  return accept(listener_fd);
}
inline std::optional<String> read_mlc(std::int32_t stream_fd, std::int32_t max_bytes) {
  return read(stream_fd, max_bytes);
}
inline bool set_recv_timeout_mlc(std::int32_t stream_fd, std::int32_t timeout_seconds) {
  return set_recv_timeout(stream_fd, timeout_seconds);
}
inline bool write_all_mlc(std::int32_t stream_fd, String data) {
  return write_all(stream_fd, data);
}
inline void close_listener_mlc(std::int32_t listener_fd) {
  close_listener(listener_fd);
}
inline void close_stream_mlc(std::int32_t stream_fd) {
  close_stream(stream_fd);
}
inline std::int32_t port_mlc(std::int32_t listener_fd) {
  return port(listener_fd);
}
inline String last_error_mlc() {
  return last_error();
}

} // namespace net
} // namespace mlc
