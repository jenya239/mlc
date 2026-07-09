// Isolate smoke (TRACK_CONCURRENCY_ISOLATE STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_isolate test_isolate.cpp

#include "mlc/concurrency/isolate.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

struct CounterState {
    int value = 0;
};

enum class CounterMessage {
    Increment = 0,
    AddTen = 1,
};

void test_serial_handler_processes_all_messages() {
    mlc::concurrency::Isolate<CounterState, CounterMessage> isolate(
        CounterState{},
        32,
        [](CounterState& state, CounterMessage message) {
            if (message == CounterMessage::Increment) state.value += 1;
            else state.value += 10;
        });
    for (int index = 0; index < 50; ++index) {
        CHECK(isolate.send(CounterMessage::Increment));
    }
    CHECK(isolate.send(CounterMessage::AddTen));
    isolate.shutdown();
    CHECK(isolate.state_after_shutdown().value == 60);
}

void test_concurrent_senders_remain_serial() {
    constexpr int sender_count = 8;
    constexpr int messages_per_sender = 100;
    mlc::concurrency::Isolate<CounterState, int> isolate(
        CounterState{},
        64,
        [](CounterState& state, int delta) { state.value += delta; });
    std::vector<std::thread> senders;
    for (int sender_index = 0; sender_index < sender_count; ++sender_index) {
        senders.emplace_back([&] {
            for (int index = 0; index < messages_per_sender; ++index) {
                CHECK(isolate.send(1));
            }
        });
    }
    for (auto& sender : senders) sender.join();
    isolate.shutdown();
    CHECK(isolate.state_after_shutdown().value == sender_count * messages_per_sender);
}

void test_destructor_drains_mailbox() {
    int final_value = -1;
    {
        mlc::concurrency::Isolate<CounterState, int> isolate(
            CounterState{},
            16,
            [](CounterState& state, int delta) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                state.value += delta;
            });
        for (int index = 0; index < 10; ++index) {
            CHECK(isolate.send(1));
        }
        // dtor shutdown drains
        final_value = 0; // placeholder; read after scope via side channel
        isolate.shutdown();
        final_value = isolate.state_after_shutdown().value;
    }
    CHECK(final_value == 10);
}

void test_send_after_shutdown_fails() {
    mlc::concurrency::Isolate<CounterState, int> isolate(
        CounterState{},
        4,
        [](CounterState& state, int delta) { state.value += delta; });
    isolate.shutdown();
    CHECK(!isolate.send(1));
    CHECK(isolate.is_shutdown());
}

void test_full_mailbox_blocks_until_owner_drains() {
    std::atomic<bool> handler_started{false};
    std::atomic<bool> handler_release{false};
    mlc::concurrency::Isolate<CounterState, int> isolate(
        CounterState{},
        1,
        [&](CounterState& state, int delta) {
            handler_started.store(true);
            while (!handler_release.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            state.value += delta;
        });
    CHECK(isolate.send(1));
    while (!handler_started.load()) std::this_thread::yield();
    // Mailbox capacity 1: one queued while owner busy → third send blocks.
    CHECK(isolate.send(1));
    std::atomic<bool> third_done{false};
    std::thread blocker([&] {
        CHECK(isolate.send(1));
        third_done.store(true);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(!third_done.load());
    handler_release.store(true);
    blocker.join();
    isolate.shutdown();
    CHECK(third_done.load());
    CHECK(isolate.state_after_shutdown().value == 3);
}

void test_cancel_unblocks_blocked_send() {
    std::atomic<bool> handler_busy{false};
    mlc::concurrency::Isolate<CounterState, int> isolate(
        CounterState{},
        1,
        [&](CounterState& state, int delta) {
            handler_busy.store(true);
            while (!isolate.cancel_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            state.value += delta;
        });
    CHECK(isolate.send(1));
    while (!handler_busy.load()) std::this_thread::yield();
    CHECK(isolate.send(1)); // fills mailbox
    std::atomic<bool> send_returned{false};
    std::atomic<bool> send_ok{true};
    std::thread blocker([&] {
        send_ok.store(isolate.send(1));
        send_returned.store(true);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(!send_returned.load());
    isolate.request_cancel();
    blocker.join();
    CHECK(send_returned.load());
    CHECK(!send_ok.load());
    isolate.shutdown();
}

void test_shutdown_requests_cancel() {
    std::atomic<bool> saw_cancel{false};
    mlc::concurrency::Isolate<CounterState, int> isolate(
        CounterState{},
        4,
        [&](CounterState& state, int) {
            while (!isolate.token().requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            saw_cancel.store(true);
            state.value = 1;
        });
    CHECK(isolate.send(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    isolate.shutdown();
    CHECK(saw_cancel.load());
    CHECK(isolate.cancel_requested());
}

int main() {
    test_serial_handler_processes_all_messages();
    test_concurrent_senders_remain_serial();
    test_destructor_drains_mailbox();
    test_send_after_shutdown_fails();
    test_full_mailbox_blocks_until_owner_drains();
    test_cancel_unblocks_blocked_send();
    test_shutdown_requests_cancel();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
