#pragma once

// Blocking IPv4 TCP listener/stream (TRACK_STDLIB_NET_SERVER STEP=2).
// Errors as mlc::result::Result<_, mlc::String> — no exceptions on I/O failure.

#include "mlc/core/result.hpp"
#include "mlc/core/string.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

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

} // namespace net
} // namespace mlc
