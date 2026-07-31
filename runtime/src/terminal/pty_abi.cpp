#include "mlc/terminal/pty_abi.hpp"

#include <pty.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <unordered_map>
#include <vector>

namespace mlc {
namespace terminal {
namespace {

std::unordered_map<int, pid_t>& pty_child_pids() {
  static thread_local std::unordered_map<int, pid_t> pids;
  return pids;
}

bool poll_readable(std::int32_t master_fd, std::int32_t timeout_ms) {
  struct pollfd poll_descriptor{};
  poll_descriptor.fd = master_fd;
  poll_descriptor.events = POLLIN;
  return ::poll(&poll_descriptor, 1, timeout_ms) > 0;
}

} // namespace

std::int32_t pty_spawn(String command_line) {
  int master_fd = -1;
  pid_t child_pid = forkpty(&master_fd, nullptr, nullptr, nullptr);
  if (child_pid < 0) {
    return -1;
  }
  if (child_pid == 0) {
    execl("/bin/sh", "sh", "-c", command_line.c_str(), static_cast<char*>(nullptr));
    _exit(127);
  }
  pty_child_pids()[master_fd] = child_pid;
  return static_cast<std::int32_t>(master_fd);
}

std::int32_t pty_write(std::int32_t master_fd, String bytes) {
  const std::string_view view = bytes.view();
  const ssize_t written = ::write(master_fd, view.data(), view.size());
  return written < 0 ? -1 : static_cast<std::int32_t>(written);
}

String pty_read(std::int32_t master_fd, std::int32_t max_bytes, std::int32_t timeout_ms) {
  if (max_bytes <= 0 || !poll_readable(master_fd, timeout_ms)) {
    return String(std::string());
  }
  std::vector<char> buffer(static_cast<std::size_t>(max_bytes));
  const ssize_t bytes_read = ::read(master_fd, buffer.data(), buffer.size());
  if (bytes_read <= 0) {
    return String(std::string());
  }
  return String(std::string(buffer.data(), static_cast<std::size_t>(bytes_read)));
}

String pty_read_until_eof(std::int32_t master_fd, std::int32_t max_total_bytes, std::int32_t timeout_ms) {
  std::string accumulated;
  char buffer[4096];
  while (static_cast<std::int32_t>(accumulated.size()) < max_total_bytes) {
    if (!poll_readable(master_fd, timeout_ms)) {
      break;
    }
    const ssize_t bytes_read = ::read(master_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
      break;
    }
    accumulated.append(buffer, static_cast<std::size_t>(bytes_read));
  }
  if (static_cast<std::int32_t>(accumulated.size()) > max_total_bytes) {
    accumulated.resize(static_cast<std::size_t>(max_total_bytes));
  }
  return String(std::move(accumulated));
}

std::int32_t pty_close(std::int32_t master_fd) {
  auto& pids = pty_child_pids();
  auto found = pids.find(master_fd);
  if (found != pids.end()) {
    ::waitpid(found->second, nullptr, WNOHANG);
    pids.erase(found);
  }
  return ::close(master_fd) == 0 ? 0 : -1;
}

std::int32_t pty_resize(std::int32_t master_fd, std::int32_t rows, std::int32_t columns) {
  if (rows <= 0 || columns <= 0) {
    return -1;
  }
  struct winsize window_size{};
  window_size.ws_row = static_cast<unsigned short>(rows);
  window_size.ws_col = static_cast<unsigned short>(columns);
  return ::ioctl(master_fd, TIOCSWINSZ, &window_size) == 0 ? 0 : -1;
}

} // namespace terminal
} // namespace mlc
