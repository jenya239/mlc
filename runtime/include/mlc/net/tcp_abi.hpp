#pragma once

// Thin POSIX TCP ABI for TRACK_FFI_SHIM_MIGRATION STEP=4.
// Tokens are real fds (fd-as-token). Control flow / Ruby surface: tcp_bridge.hpp.

#include "mlc/core/string.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

namespace mlc {
namespace net {

namespace abi_detail {

struct TcpAbiState {
  String last_error;
  std::int32_t recv_status = 1;
};

inline TcpAbiState& tcp_abi_state() {
  static TcpAbiState state;
  return state;
}

inline String system_error_message(const char* prefix) {
  const char* detail = std::strerror(errno);
  if (detail == nullptr) {
    return String(prefix);
  }
  return String(std::string(prefix) + ": " + detail);
}

} // namespace abi_detail

inline void table_set_error(String message) {
  abi_detail::tcp_abi_state().last_error = std::move(message);
}

inline String table_last_error() {
  return abi_detail::tcp_abi_state().last_error;
}

inline String tcp_errno_message(String prefix) {
  return abi_detail::system_error_message(prefix.c_str());
}

inline std::int32_t tcp_socket() {
  return ::socket(AF_INET, SOCK_STREAM, 0);
}

inline std::int32_t tcp_set_reuseaddr(std::int32_t file_descriptor) {
  if (file_descriptor < 0) {
    return -1;
  }
  const int reuse = 1;
  return ::setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}

// 0 ok; -1 error. timeout_seconds=0 clears the receive deadline.
inline std::int32_t tcp_set_recv_timeout(std::int32_t file_descriptor, std::int32_t timeout_seconds) {
  if (file_descriptor < 0 || timeout_seconds < 0) {
    return -1;
  }
  timeval timeout{};
  timeout.tv_sec = static_cast<time_t>(timeout_seconds);
  timeout.tv_usec = 0;
  return ::setsockopt(file_descriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

// 0 ok; -1 invalid host (sets last_error); -2 bind errno (sets last_error).
inline std::int32_t tcp_bind_ipv4(std::int32_t file_descriptor, String host, std::int32_t port) {
  if (file_descriptor < 0) {
    return -2;
  }
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<std::uint16_t>(port));
  if (::inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
    table_set_error(String("TcpListener.bind: invalid IPv4 host"));
    return -1;
  }
  if (::bind(file_descriptor, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
    table_set_error(abi_detail::system_error_message("TcpListener.bind"));
    return -2;
  }
  return 0;
}

inline std::int32_t tcp_listen(std::int32_t file_descriptor, std::int32_t backlog) {
  if (file_descriptor < 0) {
    return -1;
  }
  return ::listen(file_descriptor, backlog);
}

// Bound port, or -1 on error (sets last_error).
inline std::int32_t tcp_getsockname_port(std::int32_t file_descriptor) {
  if (file_descriptor < 0) {
    return -1;
  }
  sockaddr_in bound_address{};
  socklen_t bound_length = sizeof(bound_address);
  if (::getsockname(
          file_descriptor,
          reinterpret_cast<sockaddr*>(&bound_address),
          &bound_length
      ) < 0) {
    table_set_error(abi_detail::system_error_message("TcpListener.bind: getsockname"));
    return -1;
  }
  return static_cast<std::int32_t>(ntohs(bound_address.sin_port));
}

inline std::int32_t tcp_accept(std::int32_t listener_fd) {
  if (listener_fd < 0) {
    return -1;
  }
  return ::accept(listener_fd, nullptr, nullptr);
}

// On success recv_status=1 (incl. EOF empty). On error recv_status=-1 + last_error.
inline String tcp_recv(std::int32_t stream_fd, std::int32_t max_bytes) {
  if (stream_fd < 0) {
    table_set_error(String("TcpStream.read: closed"));
    abi_detail::tcp_abi_state().recv_status = -1;
    return String();
  }
  if (max_bytes <= 0) {
    table_set_error(String("TcpStream.read: max_bytes must be > 0"));
    abi_detail::tcp_abi_state().recv_status = -1;
    return String();
  }
  std::string buffer(static_cast<std::size_t>(max_bytes), '\0');
  const ssize_t received = ::recv(stream_fd, buffer.data(), buffer.size(), 0);
  if (received < 0) {
    table_set_error(abi_detail::system_error_message("TcpStream.read"));
    abi_detail::tcp_abi_state().recv_status = -1;
    return String();
  }
  buffer.resize(static_cast<std::size_t>(received));
  abi_detail::tcp_abi_state().recv_status = 1;
  return String(std::move(buffer));
}

inline std::int32_t tcp_recv_status() {
  return abi_detail::tcp_abi_state().recv_status;
}

// 1 ok; 0 error (sets last_error).
inline std::int32_t tcp_send_all(std::int32_t stream_fd, String data) {
  if (stream_fd < 0) {
    table_set_error(String("TcpStream.write_all: closed"));
    return 0;
  }
  const char* cursor = data.c_str();
  std::size_t remaining = data.size();
  while (remaining > 0) {
    const ssize_t sent = ::send(stream_fd, cursor, remaining, MSG_NOSIGNAL);
    if (sent < 0) {
      table_set_error(abi_detail::system_error_message("TcpStream.write_all"));
      return 0;
    }
    if (sent == 0) {
      table_set_error(String("TcpStream.write_all: short write"));
      return 0;
    }
    cursor += sent;
    remaining -= static_cast<std::size_t>(sent);
  }
  return 1;
}

inline void tcp_close(std::int32_t file_descriptor) {
  if (file_descriptor >= 0) {
    ::close(file_descriptor);
  }
}

} // namespace net
} // namespace mlc
