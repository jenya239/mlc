// ThreadPool smoke (TRACK_CONCURRENCY_ISOLATE STEP=1).
// g++ -std=c++20 -pthread -I../include -o test_thread_pool test_thread_pool.cpp

#include "mlc/concurrency/thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_submit_runs_on_workers() {
    std::atomic<int> counter{0};
    {
        mlc::concurrency::ThreadPool pool(4, 16);
        for (int index = 0; index < 40; ++index) {
            CHECK(pool.submit([&] { counter.fetch_add(1); }));
        }
        pool.shutdown();
    }
    CHECK(counter.load() == 40);
}

void test_destructor_drains_queued_jobs() {
    std::atomic<int> counter{0};
    {
        mlc::concurrency::ThreadPool pool(2, 8);
        for (int index = 0; index < 20; ++index) {
            CHECK(pool.submit([&] {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                counter.fetch_add(1);
            }));
        }
    }
    CHECK(counter.load() == 20);
}

void test_submit_after_shutdown_fails() {
    mlc::concurrency::ThreadPool pool(1, 4);
    pool.shutdown();
    CHECK(!pool.submit([] {}));
    CHECK(pool.is_shutdown());
}

void test_full_queue_blocks_until_worker_drains() {
    std::atomic<int> started{0};
    std::atomic<int> finished{0};
    mlc::concurrency::ThreadPool pool(1, 1);
    CHECK(pool.submit([&] {
        started.store(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        finished.fetch_add(1);
    }));
    while (started.load() == 0) std::this_thread::yield();
    // Queue holds one more job; third submit blocks until first finishes.
    CHECK(pool.submit([&] { finished.fetch_add(1); }));
    std::atomic<bool> third_done{false};
    std::thread blocker([&] {
        CHECK(pool.submit([&] { finished.fetch_add(1); }));
        third_done.store(true);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    CHECK(!third_done.load());
    blocker.join();
    pool.shutdown();
    CHECK(finished.load() == 3);
    CHECK(third_done.load());
}

int main() {
    test_submit_runs_on_workers();
    test_destructor_drains_queued_jobs();
    test_submit_after_shutdown_fails();
    test_full_queue_blocks_until_worker_drains();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
