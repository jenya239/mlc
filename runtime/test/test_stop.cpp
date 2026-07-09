// StopSource / StopToken smoke (TRACK_CONCURRENCY_V2 STEP=5).
// g++ -std=c++20 -pthread -I../include -o test_stop test_stop.cpp

#include "mlc/concurrency/stop.hpp"
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

void test_stop_source_initially_not_requested() {
    mlc::concurrency::StopSource stop;
    CHECK(!stop.requested());
    CHECK(!stop.token().requested());
}

void test_stop_source_request_propagates_to_token() {
    mlc::concurrency::StopSource stop;
    mlc::concurrency::StopToken token = stop.token();
    CHECK(!token.requested());
    stop.request();
    CHECK(stop.requested());
    CHECK(token.requested());
}

void test_stop_token_copy_shares_state() {
    mlc::concurrency::StopSource stop;
    mlc::concurrency::StopToken first = stop.token();
    mlc::concurrency::StopToken second = first;
    stop.request();
    CHECK(first.requested());
    CHECK(second.requested());
}

void test_worker_observes_request() {
    mlc::concurrency::StopSource stop;
    std::atomic<bool> saw_request{false};
    std::thread worker([token = stop.token(), &saw_request] {
        while (!token.requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        saw_request.store(true);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    CHECK(!saw_request.load());
    stop.request();
    worker.join();
    CHECK(saw_request.load());
}

int main() {
    test_stop_source_initially_not_requested();
    test_stop_source_request_propagates_to_token();
    test_stop_token_copy_shares_state();
    test_worker_observes_request();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
