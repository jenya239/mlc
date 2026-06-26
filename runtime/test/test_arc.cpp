// Arc atomic refcount smoke (TRACK_CONCURRENCY STEP=5).
// g++ -std=c++20 -pthread -I../include -o test_arc test_arc.cpp

#include "mlc/concurrency/arc.hpp"
#include <iostream>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_arc_copy_refcount() {
    mlc::concurrency::Arc<int> original(42);
    mlc::concurrency::Arc<int> copy = original;
    CHECK(original.use_count() == 2);
    CHECK(copy.use_count() == 2);
    CHECK(*copy == 42);
}

void test_arc_cross_thread_read() {
    mlc::concurrency::Arc<int> shared_value(99);
    int received = 0;
    std::thread worker([&] {
        mlc::concurrency::Arc<int> local_copy = shared_value;
        received = *local_copy;
    });
    worker.join();
    CHECK(received == 99);
    CHECK(shared_value.use_count() == 1);
}

int main() {
    test_arc_copy_refcount();
    test_arc_cross_thread_read();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
