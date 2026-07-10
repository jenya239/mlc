#pragma once

// Blocking IPv4 TCP listener/stream (TRACK_STDLIB_NET_SERVER STEP=2).
// Errors as mlc::result::Result<_, mlc::String> — no exceptions on I/O failure.

#include "mlc/core/result.hpp"
#include "mlc/core/string.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <variant>
#include <vector>

namespace mlc {
namespace net {

namespace detail {

inline String system_error_message(const char* prefix) {
  const char* detail = std::strerror(errno);
  if (detail == nullptr) {
    return String(prefix);
  }
  return String(std::string(prefix) + ": " + detail);
}

inline void close_file_descriptor(int& file_descriptor) {
  if (file_descriptor >= 0) {
    ::close(file_descriptor);
    file_descriptor = -1;
  }
}

} // namespace detail

class TcpStream {
  int file_descriptor_ = -1;

  explicit TcpStream(int file_descriptor) : file_descriptor_(file_descriptor) {}

  friend class TcpListener;

public:
  TcpStream() = default;

  TcpStream(const TcpStream&) = delete;
  TcpStream& operator=(const TcpStream&) = delete;

  TcpStream(TcpStream&& other) noexcept : file_descriptor_(other.file_descriptor_) {
    other.file_descriptor_ = -1;
  }

  TcpStream& operator=(TcpStream&& other) noexcept {
    if (this != &other) {
      detail::close_file_descriptor(file_descriptor_);
      file_descriptor_ = other.file_descriptor_;
      other.file_descriptor_ = -1;
    }
    return *this;
  }

  ~TcpStream() { detail::close_file_descriptor(file_descriptor_); }

  [[nodiscard]] bool is_open() const noexcept { return file_descriptor_ >= 0; }

  void close() { detail::close_file_descriptor(file_descriptor_); }

  // Read up to max_bytes. Ok("") on peer close (EOF).
  [[nodiscard]] result::Result<String, String> read(std::int32_t max_bytes) {
    if (file_descriptor_ < 0) {
      return result::err(String("TcpStream.read: closed"));
    }
    if (max_bytes <= 0) {
      return result::err(String("TcpStream.read: max_bytes must be > 0"));
    }
    std::string buffer(static_cast<std::size_t>(max_bytes), '\0');
    const ssize_t received = ::recv(file_descriptor_, buffer.data(), buffer.size(), 0);
    if (received < 0) {
      return result::err(detail::system_error_message("TcpStream.read"));
    }
    buffer.resize(static_cast<std::size_t>(received));
    return result::ok(String(std::move(buffer)));
  }

  // Write entire buffer; loops until done or error.
  [[nodiscard]] result::Result<bool, String> write_all(const String& data) {
    if (file_descriptor_ < 0) {
      return result::err(String("TcpStream.write_all: closed"));
    }
    const char* cursor = data.c_str();
    std::size_t remaining = data.size();
    while (remaining > 0) {
      const ssize_t sent = ::send(file_descriptor_, cursor, remaining, MSG_NOSIGNAL);
      if (sent < 0) {
        return result::err(detail::system_error_message("TcpStream.write_all"));
      }
      if (sent == 0) {
        return result::err(String("TcpStream.write_all: short write"));
      }
      cursor += sent;
      remaining -= static_cast<std::size_t>(sent);
    }
    return result::ok(true);
  }
};

class TcpListener {
  int file_descriptor_ = -1;
  std::uint16_t bound_port_ = 0;

  explicit TcpListener(int file_descriptor, std::uint16_t bound_port)
      : file_descriptor_(file_descriptor), bound_port_(bound_port) {}

public:
  TcpListener() = default;

  TcpListener(const TcpListener&) = delete;
  TcpListener& operator=(const TcpListener&) = delete;

  TcpListener(TcpListener&& other) noexcept
      : file_descriptor_(other.file_descriptor_), bound_port_(other.bound_port_) {
    other.file_descriptor_ = -1;
    other.bound_port_ = 0;
  }

  TcpListener& operator=(TcpListener&& other) noexcept {
    if (this != &other) {
      detail::close_file_descriptor(file_descriptor_);
      file_descriptor_ = other.file_descriptor_;
      bound_port_ = other.bound_port_;
      other.file_descriptor_ = -1;
      other.bound_port_ = 0;
    }
    return *this;
  }

  ~TcpListener() { detail::close_file_descriptor(file_descriptor_); }

  [[nodiscard]] bool is_open() const noexcept { return file_descriptor_ >= 0; }

  // Port actually bound (useful when bind(..., 0) requests an ephemeral port).
  [[nodiscard]] std::int32_t port() const noexcept { return static_cast<std::int32_t>(bound_port_); }

  void close() { detail::close_file_descriptor(file_descriptor_); }

  [[nodiscard]] static result::Result<TcpListener, String> bind(
      const String& host,
      std::int32_t port
  ) {
    if (port < 0 || port > 65535) {
      return result::err(String("TcpListener.bind: port out of range"));
    }

    int listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) {
      return result::err(detail::system_error_message("TcpListener.bind: socket"));
    }

    const int reuse = 1;
    if (::setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
      detail::close_file_descriptor(listen_socket);
      return result::err(detail::system_error_message("TcpListener.bind: setsockopt"));
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    if (::inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
      detail::close_file_descriptor(listen_socket);
      return result::err(String("TcpListener.bind: invalid IPv4 host"));
    }

    if (::bind(listen_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
      detail::close_file_descriptor(listen_socket);
      return result::err(detail::system_error_message("TcpListener.bind"));
    }

    if (::listen(listen_socket, 128) < 0) {
      detail::close_file_descriptor(listen_socket);
      return result::err(detail::system_error_message("TcpListener.bind: listen"));
    }

    sockaddr_in bound_address{};
    socklen_t bound_length = sizeof(bound_address);
    if (::getsockname(listen_socket, reinterpret_cast<sockaddr*>(&bound_address), &bound_length) < 0) {
      detail::close_file_descriptor(listen_socket);
      return result::err(detail::system_error_message("TcpListener.bind: getsockname"));
    }

    return result::ok(TcpListener(listen_socket, ntohs(bound_address.sin_port)));
  }

  [[nodiscard]] result::Result<TcpStream, String> accept() {
    if (file_descriptor_ < 0) {
      return result::err(String("TcpListener.accept: closed"));
    }
    const int client_socket = ::accept(file_descriptor_, nullptr, nullptr);
    if (client_socket < 0) {
      return result::err(detail::system_error_message("TcpListener.accept"));
    }
    return result::ok(TcpStream(client_socket));
  }
};

// MLC stdlib bridge (STEP=3): opaque i32 handles (1-based table indices, not fds).
// Option + last_error — Result<_,string> extern mapping is not reliable yet.

namespace detail {

struct TcpHandleTable {
  std::mutex mutex;
  std::vector<std::shared_ptr<TcpListener>> listeners;
  std::vector<std::shared_ptr<TcpStream>> streams;
  String last_error;
};

inline TcpHandleTable& tcp_handle_table() {
  static TcpHandleTable table;
  return table;
}

inline void set_last_error(String message) {
  std::lock_guard<std::mutex> lock(tcp_handle_table().mutex);
  tcp_handle_table().last_error = std::move(message);
}

} // namespace detail

inline String last_error() {
  std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
  return detail::tcp_handle_table().last_error;
}

inline std::optional<std::int32_t> bind(const String& host, std::int32_t port) {
  auto bind_result = TcpListener::bind(host, port);
  if (std::holds_alternative<result::Err<String>>(bind_result)) {
    detail::set_last_error(std::get<result::Err<String>>(bind_result)._0);
    return std::nullopt;
  }
  auto listener = std::make_shared<TcpListener>(
      std::move(std::get<result::Ok<TcpListener>>(bind_result)._0)
  );
  std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
  detail::tcp_handle_table().listeners.push_back(std::move(listener));
  return static_cast<std::int32_t>(detail::tcp_handle_table().listeners.size());
}

inline std::optional<std::int32_t> accept(std::int32_t listener_handle) {
  std::shared_ptr<TcpListener> listener;
  {
    std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
    if (listener_handle <= 0 ||
        static_cast<std::size_t>(listener_handle) > detail::tcp_handle_table().listeners.size()) {
      detail::tcp_handle_table().last_error = String("Tcp.accept: invalid listener handle");
      return std::nullopt;
    }
    listener = detail::tcp_handle_table().listeners[static_cast<std::size_t>(listener_handle) - 1];
  }
  if (!listener || !listener->is_open()) {
    detail::set_last_error(String("Tcp.accept: listener closed"));
    return std::nullopt;
  }
  auto accept_result = listener->accept();
  if (std::holds_alternative<result::Err<String>>(accept_result)) {
    detail::set_last_error(std::get<result::Err<String>>(accept_result)._0);
    return std::nullopt;
  }
  auto stream = std::make_shared<TcpStream>(
      std::move(std::get<result::Ok<TcpStream>>(accept_result)._0)
  );
  std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
  detail::tcp_handle_table().streams.push_back(std::move(stream));
  return static_cast<std::int32_t>(detail::tcp_handle_table().streams.size());
}

inline std::optional<String> read(std::int32_t stream_handle, std::int32_t max_bytes) {
  std::shared_ptr<TcpStream> stream;
  {
    std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
    if (stream_handle <= 0 ||
        static_cast<std::size_t>(stream_handle) > detail::tcp_handle_table().streams.size()) {
      detail::tcp_handle_table().last_error = String("Tcp.read: invalid stream handle");
      return std::nullopt;
    }
    stream = detail::tcp_handle_table().streams[static_cast<std::size_t>(stream_handle) - 1];
  }
  if (!stream || !stream->is_open()) {
    detail::set_last_error(String("Tcp.read: stream closed"));
    return std::nullopt;
  }
  auto read_result = stream->read(max_bytes);
  if (std::holds_alternative<result::Err<String>>(read_result)) {
    detail::set_last_error(std::get<result::Err<String>>(read_result)._0);
    return std::nullopt;
  }
  return std::get<result::Ok<String>>(read_result)._0;
}

inline bool write_all(std::int32_t stream_handle, const String& data) {
  std::shared_ptr<TcpStream> stream;
  {
    std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
    if (stream_handle <= 0 ||
        static_cast<std::size_t>(stream_handle) > detail::tcp_handle_table().streams.size()) {
      detail::tcp_handle_table().last_error = String("Tcp.write_all: invalid stream handle");
      return false;
    }
    stream = detail::tcp_handle_table().streams[static_cast<std::size_t>(stream_handle) - 1];
  }
  if (!stream || !stream->is_open()) {
    detail::set_last_error(String("Tcp.write_all: stream closed"));
    return false;
  }
  auto write_result = stream->write_all(data);
  if (std::holds_alternative<result::Err<String>>(write_result)) {
    detail::set_last_error(std::get<result::Err<String>>(write_result)._0);
    return false;
  }
  return true;
}

inline void close_listener(std::int32_t listener_handle) {
  std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
  if (listener_handle <= 0 ||
      static_cast<std::size_t>(listener_handle) > detail::tcp_handle_table().listeners.size()) {
    return;
  }
  auto& listener = detail::tcp_handle_table().listeners[static_cast<std::size_t>(listener_handle) - 1];
  if (listener) {
    listener->close();
    listener.reset();
  }
}

inline void close_stream(std::int32_t stream_handle) {
  std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
  if (stream_handle <= 0 ||
      static_cast<std::size_t>(stream_handle) > detail::tcp_handle_table().streams.size()) {
    return;
  }
  auto& stream = detail::tcp_handle_table().streams[static_cast<std::size_t>(stream_handle) - 1];
  if (stream) {
    stream->close();
    stream.reset();
  }
}

inline std::int32_t port(std::int32_t listener_handle) {
  std::lock_guard<std::mutex> lock(detail::tcp_handle_table().mutex);
  if (listener_handle <= 0 ||
      static_cast<std::size_t>(listener_handle) > detail::tcp_handle_table().listeners.size()) {
    return 0;
  }
  const auto& listener = detail::tcp_handle_table().listeners[static_cast<std::size_t>(listener_handle) - 1];
  if (!listener) {
    return 0;
  }
  return listener->port();
}

// By-value entry points for mlcc FFI binders (TRACK_PIPELINE_MERGE_TCP_SPAWN).
// mlcc emits `T(*)(mlc::String, …)` casts; the primary API uses `const String&`.
inline std::optional<std::int32_t> bind_mlc(String host, std::int32_t port_value) {
  return bind(host, port_value);
}
inline std::optional<std::int32_t> accept_mlc(std::int32_t listener_handle) {
  return accept(listener_handle);
}
inline std::optional<String> read_mlc(std::int32_t stream_handle, std::int32_t max_bytes) {
  return read(stream_handle, max_bytes);
}
inline bool write_all_mlc(std::int32_t stream_handle, String data) {
  return write_all(stream_handle, data);
}
inline void close_listener_mlc(std::int32_t listener_handle) {
  close_listener(listener_handle);
}
inline void close_stream_mlc(std::int32_t stream_handle) {
  close_stream(stream_handle);
}
inline std::int32_t port_mlc(std::int32_t listener_handle) {
  return port(listener_handle);
}
inline String last_error_mlc() {
  return last_error();
}

} // namespace net
} // namespace mlc
