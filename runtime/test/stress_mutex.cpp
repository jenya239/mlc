// Mutex stress (TRACK_CONCURRENCY_TEST_HARNESS STEP=3).
// g++ -std=c++20 -pthread -I../include -o stress_mutex stress_mutex.cpp

#include "mlc/concurrency/mutex.hpp"

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_high_contention() {
    constexpr int thread_count = 8;
    constexpr int increments_per_thread = 10000;
    mlc::concurrency::Mutex<int> guarded(0);
    std::vector<std::thread> workers;
    for (int index = 0; index < thread_count; ++index) {
        workers.emplace_back([&] {
            for (int step = 0; step < increments_per_thread; ++step) {
                guarded.lock([](int& counter) { counter = counter + 1; });
            }
        });
    }
    for (auto& worker : workers) worker.join();
    const int total = guarded.lock([](int& counter) { return counter; });
    CHECK(total == thread_count * increments_per_thread);
}

void test_exception_releases_lock() {
    mlc::concurrency::Mutex<int> guarded(0);
    bool threw = false;
    try {
        guarded.lock([](int& counter) -> int {
            counter = 1;
            throw std::runtime_error("inside lock");
        });
    } catch (const std::runtime_error&) {
        threw = true;
    }
    CHECK(threw);
    const int value = guarded.lock([](int& counter) { return counter; });
    CHECK(value == 1);
    guarded.lock([](int& counter) { counter = 2; });
    CHECK(guarded.lock([](int& counter) { return counter; }) == 2);
}

int main() {
    test_high_contention();
    test_exception_releases_lock();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
