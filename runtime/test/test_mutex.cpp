// Mutex scoped lock smoke (TRACK_CONCURRENCY STEP=6).
// g++ -std=c++20 -pthread -I../include -o test_mutex test_mutex.cpp

#include "mlc/concurrency/mutex.hpp"
#include "mlc/core/array.hpp"
#include <iostream>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_mutex_lock_mutate_array() {
    mlc::concurrency::Mutex<mlc::Array<int>> guarded{mlc::Array<int>()};
    const int length = guarded.lock([](mlc::Array<int>& values) {
        values.push_back(7);
        values.push_back(8);
        return values.length();
    });
    CHECK(length == 2);
}

void test_mutex_cross_thread() {
    mlc::concurrency::Mutex<int> guarded(0);
    std::thread worker([&] {
        guarded.lock([](int& counter) { counter = counter + 1; });
    });
    worker.join();
    CHECK(guarded.lock([](int& counter) { return counter; }) == 1);
}

int main() {
    test_mutex_lock_mutate_array();
    test_mutex_cross_thread();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
