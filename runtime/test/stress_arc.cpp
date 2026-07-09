// Arc clone/drop stress (TRACK_CONCURRENCY_TEST_HARNESS STEP=3).
// g++ -std=c++20 -pthread -I../include -o stress_arc stress_arc.cpp

#include "mlc/concurrency/arc.hpp"

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_concurrent_clone_drop() {
    constexpr int thread_count = 8;
    constexpr int iterations = 50000;
    mlc::concurrency::Arc<int> shared(42);
    std::atomic<int> observed{0};
    std::vector<std::thread> workers;
    for (int index = 0; index < thread_count; ++index) {
        workers.emplace_back([&] {
            for (int step = 0; step < iterations; ++step) {
                mlc::concurrency::Arc<int> local = shared;
                observed.fetch_add(*local, std::memory_order_relaxed);
            }
        });
    }
    for (auto& worker : workers) worker.join();
    CHECK(shared.use_count() == 1);
    CHECK(*shared == 42);
    CHECK(observed.load() == thread_count * iterations * 42);
}

void test_concurrent_assign_drop() {
    constexpr int thread_count = 4;
    constexpr int iterations = 20000;
    mlc::concurrency::Arc<int> shared(7);
    std::vector<std::thread> workers;
    for (int index = 0; index < thread_count; ++index) {
        workers.emplace_back([&] {
            for (int step = 0; step < iterations; ++step) {
                mlc::concurrency::Arc<int> local(step);
                local = shared;
                CHECK(*local == 7);
            }
        });
    }
    for (auto& worker : workers) worker.join();
    CHECK(shared.use_count() == 1);
    CHECK(*shared == 7);
}

int main() {
    test_concurrent_clone_drop();
    test_concurrent_assign_drop();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
