// spawn_task + Task smoke (TRACK_CONCURRENCY STEP=3).
// g++ -std=c++20 -pthread -I../include -o test_spawn test_spawn.cpp

#include "mlc/concurrency/spawn.hpp"
#include "mlc/core/task.hpp"
#include <iostream>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_spawn_task_int() {
    mlc::Task<int> task = mlc::concurrency::spawn_task([] { return 42; });
    CHECK(mlc::block_on(task) == 42);
}

void test_spawn_task_void() {
    int counter = 0;
    mlc::Task<void> task = mlc::concurrency::spawn_task([&] { counter = 7; });
    mlc::block_on(task);
    CHECK(counter == 7);
}

int main() {
    test_spawn_task_int();
    test_spawn_task_void();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
