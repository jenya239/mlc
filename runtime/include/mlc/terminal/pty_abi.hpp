#pragma once

// Thin PTY ABI for TRACK_EDITOR_TERMINAL §102b.
// forkpty (glibc/libutil) handles slave tty setup (setsid, TIOCSCTTY,
// stdin/stdout/stderr dup) internally — no manual posix_openpt plumbing
// needed. Handles cross the MLC boundary as the raw master fd (i32).

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace terminal {

// forkpty + execl("/bin/sh", "sh", "-c", command_line) in the child.
// Returns the master fd, or -1 on error.
std::int32_t pty_spawn(String command_line);

// write() to the master fd. Returns bytes written, or -1 on error.
std::int32_t pty_write(std::int32_t master_fd, String bytes);

// poll(timeout_ms) + a single read() of up to max_bytes. Empty string on
// timeout, EOF, or error.
String pty_read(std::int32_t master_fd, std::int32_t max_bytes, std::int32_t timeout_ms);

// Loops pty_read until EOF/error (read() returns <= 0, e.g. EIO once the
// child exits and closes the slave) or max_total_bytes is reached, whichever
// comes first. Each individual read is bounded by timeout_ms.
String pty_read_until_eof(std::int32_t master_fd, std::int32_t max_total_bytes, std::int32_t timeout_ms);

// Best-effort: SIGHUP → short wait → SIGKILL fallback → close(master_fd).
// 0 = ok, negative = error.
std::int32_t pty_close(std::int32_t master_fd);

// Count entries under /proc/self/fd (for teardown leak gates).
std::int32_t pty_open_fd_count();

// 1 if the most recent pty_close reaped its child via waitpid; else 0.
std::int32_t pty_last_close_reaped();

// ioctl(TIOCSWINSZ) on the master fd — the kernel delivers SIGWINCH to the
// slave's foreground process group as a side effect when the size actually
// changes (standard tty behavior, not done manually here). 0 = ok, negative
// = error.
std::int32_t pty_resize(std::int32_t master_fd, std::int32_t rows, std::int32_t columns);

} // namespace terminal
} // namespace mlc
