// Deterministic TestScheduler smoke (TRACK_CONCURRENCY_TEST_HARNESS STEP=1).
// g++ -std=c++20 -pthread -I../include -o test_scheduler test_scheduler.cpp

#include "mlc/concurrency/testing/channel.hpp"
#include "mlc/concurrency/testing/mutex.hpp"
#include "mlc/concurrency/testing/scheduler.hpp"

#include <iostream>
#include <string>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

static std::vector<std::string> run_mutex_scenario(uint64_t seed) {
    mlc::concurrency::testing::TestScheduler scheduler(seed);
    mlc::concurrency::testing::TestMutex<int> guarded(scheduler, 0);

    scheduler.spawn([&] {
        guarded.lock([&](int& counter) {
            scheduler.log_event("a_enter");
            counter = counter + 1;
            scheduler.log_event("a_inc");
        });
        scheduler.log_event("a_done");
    });
    scheduler.spawn([&] {
        guarded.lock([&](int& counter) {
            scheduler.log_event("b_enter");
            counter = counter + 10;
            scheduler.log_event("b_inc");
        });
        scheduler.log_event("b_done");
    });
    scheduler.join();
    return scheduler.events();
}

static std::vector<std::string> run_channel_scenario(uint64_t seed) {
    mlc::concurrency::testing::TestScheduler scheduler(seed);
    mlc::concurrency::testing::TestChannel<int> channel(scheduler, 1);

    scheduler.spawn([&] {
        CHECK(channel.send(42));
        scheduler.log_event("sent");
    });
    scheduler.spawn([&] {
        auto value = channel.receive();
        CHECK(value.has_value() && *value == 42);
        scheduler.log_event("received");
    });
    scheduler.join();
    return scheduler.events();
}

void test_same_seed_identical_mutex_log() {
    auto first = run_mutex_scenario(7);
    auto second = run_mutex_scenario(7);
    CHECK(first == second);
    CHECK(!first.empty());
}

void test_same_seed_identical_channel_log() {
    auto first = run_channel_scenario(11);
    auto second = run_channel_scenario(11);
    CHECK(first == second);
    CHECK(first.size() == 2);
}

void test_mutex_final_value() {
    mlc::concurrency::testing::TestScheduler scheduler(99);
    mlc::concurrency::testing::TestMutex<int> guarded(scheduler, 0);
    int last_seen = 0;
    scheduler.spawn([&] {
        guarded.lock([&](int& counter) {
            counter = counter + 1;
            last_seen = counter;
        });
    });
    scheduler.spawn([&] {
        guarded.lock([&](int& counter) {
            counter = counter + 1;
            last_seen = counter;
        });
    });
    scheduler.join();
    CHECK(last_seen == 2);
}

int main() {
    test_same_seed_identical_mutex_log();
    test_same_seed_identical_channel_log();
    test_mutex_final_value();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
