#include "mlc/concurrency/spawn.hpp"
#include "mlc/reactor/event_loop.hpp"
#include "mlc/reactor/sleep_task.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

namespace {

int fail(int code, const std::string& message) {
    std::cerr << message << "\n";
    return code;
}

}  // namespace

int main() {
    mlc::Task<int> spawned = mlc::concurrency::spawn_task([] { return 42; });
    const int spawned_value = spawned.block_on();
    if (spawned_value != 42) {
        return fail(1, "spawn returned " + std::to_string(spawned_value));
    }
    if (mlc::reactor::EventLoop::has_current()) {
        return fail(1, "spawn created an event loop");
    }
    std::cout << "spawn_without_event_loop\n";

    mlc::Task<void> waiting = mlc::reactor::sleep_for_milliseconds(30);
    waiting.resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (waiting.is_ready()) {
        return fail(2, "is_ready pumped the event loop");
    }
    std::cout << "is_ready_does_not_pump\n";
    waiting.block_on();
    if (!waiting.is_ready()) {
        return fail(2, "block_on left the armed sleep unfinished");
    }

    const auto started = std::chrono::steady_clock::now();
    mlc::Task<void> slept = mlc::reactor::sleep_for_milliseconds(30);
    slept.block_on();
    const auto elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::steady_clock::now() - started)
                                          .count();
    if (!slept.is_ready()) {
        return fail(3, "block_on returned before the sleep finished");
    }
    if (elapsed_milliseconds < 20 || elapsed_milliseconds >= 500) {
        return fail(3, "block_on sleep elapsed_milliseconds=" + std::to_string(elapsed_milliseconds));
    }
    std::cout << "block_on_sleep elapsed_milliseconds=" << elapsed_milliseconds << "\n";
    return 0;
}
