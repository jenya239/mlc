// spawn_task stress (TRACK_CONCURRENCY_TEST_HARNESS STEP=3).
// g++ -std=c++20 -pthread -I../include -o stress_spawn stress_spawn.cpp

#include "mlc/concurrency/spawn.hpp"
#include "mlc/core/task.hpp"

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static std::atomic<int> passed{0};
static std::atomic<int> failed{0};

#define CHECK(expression) do { \
    if (expression) { passed.fetch_add(1, std::memory_order_relaxed); } \
    else { failed.fetch_add(1, std::memory_order_relaxed); std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_many_concurrent_spawns() {
    constexpr int task_count = 64;
    std::vector<mlc::Task<int>> tasks;
    tasks.reserve(task_count);
    for (int index = 0; index < task_count; ++index) {
        tasks.push_back(mlc::concurrency::spawn_task([index] { return index * 2; }));
    }
    long long sum = 0;
    for (auto& task : tasks)
        sum += mlc::block_on(task);
    const long long expected = static_cast<long long>(task_count) * (task_count - 1);
    CHECK(sum == expected);
}

void test_exception_inside_spawn_does_not_terminate() {
    mlc::Task<int> task = mlc::concurrency::spawn_task([]() -> int {
        throw std::runtime_error("spawn boom");
    });
    bool threw = false;
    try {
        mlc::block_on(task);
    } catch (const std::runtime_error& error) {
        threw = true;
        CHECK(std::string(error.what()) == "spawn boom");
    }
    CHECK(threw);
    mlc::Task<int> ok = mlc::concurrency::spawn_task([] { return 1; });
    CHECK(mlc::block_on(ok) == 1);
}

void test_spawn_results_under_load() {
    constexpr int rounds = 32;
    std::atomic<int> hits{0};
    for (int round = 0; round < rounds; ++round) {
        std::vector<mlc::Task<int>> tasks;
        for (int index = 0; index < 8; ++index) {
            tasks.push_back(mlc::concurrency::spawn_task([&hits, index] {
                hits.fetch_add(1, std::memory_order_relaxed);
                return index;
            }));
        }
        for (int index = 0; index < 8; ++index)
            CHECK(mlc::block_on(tasks[static_cast<size_t>(index)]) == index);
    }
    CHECK(hits.load() == rounds * 8);
}

int main() {
    test_many_concurrent_spawns();
    test_exception_inside_spawn_does_not_terminate();
    test_spawn_results_under_load();
    const int failed_count = failed.load();
    const int passed_count = passed.load();
    if (failed_count == 0) {
        std::cout << "ALL " << passed_count << " checks PASSED\n";
    } else {
        std::cout << passed_count << " passed, " << failed_count << " FAILED\n";
    }
    return failed_count > 0 ? 1 : 0;
}
