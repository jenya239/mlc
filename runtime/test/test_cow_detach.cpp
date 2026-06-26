// COW detach semantics (TRACK_CONCURRENCY STEP=1).
// g++ -std=c++20 -I../include -o test_cow_detach test_cow_detach.cpp

#include "mlc/core/array.hpp"
#include "mlc/core/hashmap.hpp"
#include "mlc/core/string.hpp"
#include <cassert>
#include <iostream>

static int passed = 0;
static int failed = 0;

#define CHECK(expr) do { \
    if (expr) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expr " at line " << __LINE__ << "\n"; } \
} while(0)

void test_array_push_detach() {
    mlc::Array<int> original;
    original.push_back(1);
    original.push_back(2);
    mlc::Array<int> shared = original;
    shared.push_back(3);
    CHECK(original.length() == 2);
    CHECK(shared.length() == 3);
}

void test_hashmap_set_detach() {
    mlc::HashMap<mlc::String, int> original;
    original.set(mlc::String("key"), 1);
    mlc::HashMap<mlc::String, int> shared = original;
    shared.set(mlc::String("key"), 2);
    CHECK(original.get(mlc::String("key")) == 1);
    CHECK(shared.get(mlc::String("key")) == 2);
}

void test_array_reverse_isolated() {
    mlc::Array<int> original;
    original.push_back(1);
    original.push_back(2);
    mlc::Array<int> reversed = original.reverse();
    CHECK(original[0] == 1);
    CHECK(reversed[0] == 2);
}

int main() {
    test_array_push_detach();
    test_hashmap_set_detach();
    test_array_reverse_isolated();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
