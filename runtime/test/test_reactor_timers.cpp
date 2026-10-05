#include "mlc/reactor/sleep_task.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>

namespace {

int read_thread_count() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        constexpr std::string_view prefix = "Threads:";
        if (line.rfind(prefix, 0) == 0) {
            return std::stoi(line.substr(prefix.size()));
        }
    }
    return -1;
}

int fail(int code, const std::string& message) {
    std::cerr << message << "\n";
    return code;
}

}  // namespace

int main() {
    mlc::Task<void> first = mlc::reactor::sleep_for_milliseconds(50);
    mlc::Task<void> second = mlc::reactor::sleep_for_milliseconds(100);
    first.resume();
    second.resume();

    const int thread_count_before = read_thread_count();
    if (thread_count_before < 1) {
        return fail(1, "thread count unreadable before wait");
    }

    const auto started = std::chrono::steady_clock::now();
    mlc::reactor::EventLoop::current().run_until([&] { return first.is_ready(); });
    if (!first.is_ready()) {
        return fail(2, "first sleep did not finish");
    }
    if (second.is_ready()) {
        return fail(2, "second sleep finished together with the first");
    }
    mlc::reactor::EventLoop::current().run_until([&] { return second.is_ready(); });
    const auto elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::steady_clock::now() - started)
                                          .count();
    if (!second.is_ready()) {
        return fail(2, "second sleep did not finish");
    }
    if (elapsed_milliseconds >= 140) {
        return fail(3, "sleeps overlapped too slowly: " + std::to_string(elapsed_milliseconds));
    }

    const int thread_count_after = read_thread_count();
    if (thread_count_after != thread_count_before) {
        return fail(
            4,
            "thread count changed " + std::to_string(thread_count_before) + " -> " +
                std::to_string(thread_count_after));
    }

    mlc::reactor::EventLoop* local_loop = &mlc::reactor::EventLoop::current();
    mlc::reactor::EventLoop* remote_loop = nullptr;
    {
        std::thread checker([&] { remote_loop = &mlc::reactor::EventLoop::current(); });
        checker.join();
    }
    if (remote_loop == nullptr || remote_loop == local_loop) {
        return fail(5, "event loop address matched across threads");
    }

    std::cout << "timers_overlapped elapsed_milliseconds=" << elapsed_milliseconds << "\n";
    std::cout << "thread_count_unchanged count=" << thread_count_after << "\n";
    std::cout << "event_loop_per_thread\n";
    return 0;
}
