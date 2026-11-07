#pragma once

// Invariants:
// - The child is a separate process group. Timeout and the output cap kill that group.
// - stdin is /dev/null. There is no pseudo-terminal and no interactive input.
// - stdout and stderr share one pipe. The captured text stops at output_byte_limit.

#include "mlc/core/string.hpp"

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

namespace mlc {
namespace process {

inline String command_capture_value(
    String command,
    String directory,
    int32_t timeout_milliseconds,
    int32_t output_byte_limit) {
    if (command.raw_size() == 0) {
        return String("refused: command");
    }
    if (timeout_milliseconds < 0) {
        timeout_milliseconds = 0;
    }
    if (output_byte_limit < 0) {
        output_byte_limit = 0;
    }

    int pipe_ends[2];
    if (pipe2(pipe_ends, O_CLOEXEC) != 0) {
        return String("refused: spawn");
    }
    const int read_end = pipe_ends[0];
    const int write_end = pipe_ends[1];
    const pid_t process_id = fork();
    if (process_id < 0) {
        close(read_end);
        close(write_end);
        return String("refused: spawn");
    }
    if (process_id == 0) {
        setpgid(0, 0);
        if (dup2(write_end, STDOUT_FILENO) < 0 || dup2(write_end, STDERR_FILENO) < 0) {
            _exit(127);
        }
        close(read_end);
        close(write_end);
        const int null_input = open("/dev/null", O_RDONLY | O_CLOEXEC);
        if (null_input >= 0) {
            dup2(null_input, STDIN_FILENO);
            close(null_input);
        }
        if (directory.raw_size() > 0 && chdir(directory.c_str()) != 0) {
            _exit(127);
        }
        execl("/bin/sh", "sh", "-c", command.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    setpgid(process_id, process_id);
    close(write_end);
    int flags = fcntl(read_end, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(read_end, F_SETFL, flags | O_NONBLOCK);
    }

    std::string captured;
    bool child_finished = false;
    bool timed_out = false;
    bool truncated = false;
    int status = 0;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_milliseconds);
    while (true) {
        if (!child_finished) {
            const pid_t waited = waitpid(process_id, &status, WNOHANG);
            if (waited == process_id || (waited < 0 && errno != EINTR)) {
                child_finished = true;
            }
        }
        int wait_milliseconds = 0;
        if (!child_finished) {
            const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                deadline - std::chrono::steady_clock::now());
            if (remaining.count() <= 0) {
                timed_out = true;
                break;
            }
            wait_milliseconds = remaining.count() > 50 ? 50 : static_cast<int>(remaining.count());
        }
        pollfd ready;
        ready.fd = read_end;
        ready.events = POLLIN;
        ready.revents = 0;
        const int poll_result = poll(&ready, 1, wait_milliseconds);
        if (poll_result > 0) {
            char buffer[4096];
            const ssize_t count = read(read_end, buffer, sizeof(buffer));
            if (count > 0) {
                const size_t room = static_cast<size_t>(output_byte_limit) - captured.size();
                if (static_cast<size_t>(count) > room) {
                    captured.append(buffer, room);
                    truncated = true;
                    break;
                }
                captured.append(buffer, static_cast<size_t>(count));
            } else if (count == 0 || (count < 0 && errno != EINTR && (child_finished || errno != EAGAIN))) {
                break;
            }
        } else if (child_finished) {
            break;
        }
    }

    if (!child_finished) {
        if (kill(-process_id, SIGKILL) != 0) {
            kill(process_id, SIGKILL);
        }
        waitpid(process_id, &status, 0);
    }
    close(read_end);

    int exit_code = 127;
    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    }
    std::string report;
    if (timed_out) {
        report = "timeout\n";
    } else if (truncated) {
        report = "truncated\n";
    } else {
        report = "exit " + std::to_string(exit_code) + "\n";
    }
    report += captured;
    return String(report);
}

}  // namespace process
}  // namespace mlc
