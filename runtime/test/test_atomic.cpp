// AtomicI32 concurrent fetch_add (TRACK_CONCURRENCY_ATOMICS).
// g++ -std=c++20 -pthread -I../include -o test_atomic test_atomic.cpp

#include "mlc/concurrency/atomic.hpp"
#include <iostream>
#include <thread>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_atomic_i32_single_thread() {
    mlc::concurrency::AtomicI32 counter(10);
    CHECK(counter.load() == 10);
    CHECK(counter.fetch_add(5) == 10);
    CHECK(counter.load() == 15);
    CHECK(counter.fetch_sub(3) == 15);
    CHECK(counter.load() == 12);
    counter.store(0);
    CHECK(counter.exchange(7) == 0);
    CHECK(counter.load() == 7);
    int32_t expected = 7;
    CHECK(counter.compare_exchange(expected, 9) == true);
    CHECK(counter.load() == 9);
}

void test_atomic_i32_concurrent_fetch_add_sum() {
    constexpr int thread_count = 8;
    constexpr int increments_per_thread = 10000;
    mlc::concurrency::AtomicI32 counter(0);
    std::vector<std::thread> workers;
    workers.reserve(thread_count);
    for (int thread_index = 0; thread_index < thread_count; ++thread_index) {
        workers.emplace_back([&] {
            for (int step = 0; step < increments_per_thread; ++step) {
                (void)counter.fetch_add(1);
            }
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
    CHECK(counter.load() == thread_count * increments_per_thread);
}

void test_atomic_bool_round_trip() {
    mlc::concurrency::AtomicBool flag(false);
    CHECK(flag.load() == false);
    flag.store(true);
    CHECK(flag.exchange(false) == true);
    CHECK(flag.load() == false);
}

int main() {
    test_atomic_i32_single_thread();
    test_atomic_i32_concurrent_fetch_add_sum();
    test_atomic_bool_round_trip();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
