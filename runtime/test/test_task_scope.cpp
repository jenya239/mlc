// TaskScope smoke (TRACK_CONCURRENCY_TASKSCOPE STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_task_scope test_task_scope.cpp

#include "mlc/concurrency/task_scope.hpp"
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

void test_spawn_join_runs_children() {
    std::atomic<int> counter{0};
    {
        mlc::concurrency::TaskScope scope;
        scope.spawn([&] { counter.fetch_add(1); });
        scope.spawn([&] { counter.fetch_add(1); });
        scope.join();
    }
    CHECK(counter.load() == 2);
}

void test_destructor_joins_without_explicit_join() {
    std::atomic<int> counter{0};
    {
        mlc::concurrency::TaskScope scope;
        scope.spawn([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            counter.store(7);
        });
    }
    CHECK(counter.load() == 7);
}

void test_cancel_reaches_spawn_with_token() {
    std::atomic<bool> saw_cancel{false};
    {
        mlc::concurrency::TaskScope scope;
        scope.spawn_with_token([&](mlc::concurrency::StopToken token) {
            while (!token.requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            saw_cancel.store(true);
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        scope.request_cancel();
        scope.join();
    }
    CHECK(saw_cancel.load());
}

void test_destructor_requests_cancel() {
    std::atomic<bool> saw_cancel{false};
    {
        mlc::concurrency::TaskScope scope;
        scope.spawn_with_token([&](mlc::concurrency::StopToken token) {
            while (!token.requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            saw_cancel.store(true);
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    CHECK(saw_cancel.load());
}

int main() {
    test_spawn_join_runs_children();
    test_destructor_joins_without_explicit_join();
    test_cancel_reaches_spawn_with_token();
    test_destructor_requests_cancel();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
