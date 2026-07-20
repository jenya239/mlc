// Channel smoke tests (TRACK_CONCURRENCY STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_channel test_channel.cpp

#include "mlc/concurrency/channel.hpp"
#include "mlc/core/string.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <optional>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_capacity_validation() {
    bool threw = false;
    try { mlc::concurrency::Channel<int>(1048577); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    mlc::concurrency::Channel<int> rendezvous(0);
    CHECK(rendezvous.capacity() == 0);
}

void test_rendezvous_handoff() {
    mlc::concurrency::Channel<int> channel(0);
    std::atomic<bool> send_done{false};
    std::atomic<bool> send_ok{false};
    std::thread sender([&] {
        send_ok.store(channel.send(42), std::memory_order_release);
        send_done.store(true, std::memory_order_release);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(!send_done.load(std::memory_order_acquire));
    auto value = channel.receive();
    sender.join();
    CHECK(value.has_value() && *value == 42);
    CHECK(send_ok.load(std::memory_order_acquire));
    CHECK(send_done.load(std::memory_order_acquire));
}

void test_rendezvous_close_unblocks_send() {
    mlc::concurrency::Channel<int> channel(0);
    std::atomic<bool> send_finished{false};
    std::atomic<bool> send_result{true};
    std::thread sender([&] {
        send_result.store(channel.send(1), std::memory_order_release);
        send_finished.store(true, std::memory_order_release);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(!send_finished.load(std::memory_order_acquire));
    channel.close();
    sender.join();
    CHECK(!send_result.load(std::memory_order_acquire));
}

void test_rendezvous_close_unblocks_recv() {
    mlc::concurrency::Channel<int> channel(0);
    std::optional<int> received = 0;
    std::thread receiver([&] { received = channel.receive(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    channel.close();
    receiver.join();
    CHECK(!received.has_value());
}

void test_rendezvous_try_send_without_receiver() {
    mlc::concurrency::Channel<int> channel(0);
    CHECK(!channel.try_send(1));
}

void test_bounded_try_send() {
    mlc::concurrency::Channel<int> channel(1);
    CHECK(channel.try_send(1));
    CHECK(!channel.try_send(2));
    auto value = channel.try_receive();
    CHECK(value.has_value() && *value == 1);
}

void test_copy_isolation() {
    mlc::concurrency::Channel<int> channel(4);
    int original = 10;
    CHECK(channel.send(original));
    original = 99;
    auto received = channel.receive();
    CHECK(received.has_value() && *received == 10);
}

void test_close_blocks_send() {
    mlc::concurrency::Channel<int> channel(4);
    channel.close();
    CHECK(!channel.try_send(1));
    CHECK(!channel.send(1));
}

void test_receive_after_close_empty() {
    mlc::concurrency::Channel<int> channel(4);
    channel.close();
    auto empty = channel.receive();
    CHECK(!empty.has_value());
}

void test_threaded_round_trip() {
    mlc::concurrency::Channel<int> channel(2);
    std::atomic<int> sum{0};

    std::thread producer([&] {
        for (int index = 1; index <= 5; ++index) {
            CHECK(channel.send(index));
        }
        channel.close();
    });

    std::thread consumer([&] {
        while (true) {
            auto value = channel.receive();
            if (!value.has_value()) break;
            sum += *value;
        }
    });

    producer.join();
    consumer.join();
    CHECK(sum.load() == 15);
}

void test_string_copy() {
    mlc::concurrency::Channel<mlc::String> channel(1);
    mlc::String message("hello");
    CHECK(channel.send(message));
    message = mlc::String("mutated");
    auto received = channel.receive();
    CHECK(received.has_value());
    CHECK(received->as_std_string() == "hello");
}

void test_sender_receiver_open_channel() {
    auto [sender, receiver] = mlc::concurrency::open_channel<int>(4);
    CHECK(sender.send(7));
    auto value = receiver.receive();
    CHECK(value.has_value() && *value == 7);
}

void test_sender_clone_last_drop_closes() {
    auto [sender, receiver] = mlc::concurrency::open_channel<int>(4);
    auto sender_copy = sender.clone();
    CHECK(sender.send(1));
    CHECK(sender_copy.send(2));
    sender = mlc::concurrency::Sender<int>();
    CHECK(sender_copy.send(3));
    sender_copy = mlc::concurrency::Sender<int>();
    auto first = receiver.receive();
    auto second = receiver.receive();
    auto third = receiver.receive();
    auto closed = receiver.receive();
    CHECK(first.has_value() && *first == 1);
    CHECK(second.has_value() && *second == 2);
    CHECK(third.has_value() && *third == 3);
    CHECK(!closed.has_value());
    CHECK(receiver.is_closed());
}

void test_sender_explicit_close_wakes_receiver() {
    auto [sender, receiver] = mlc::concurrency::open_channel<int>(4);
    std::optional<int> received = 0;
    std::thread waiter([&] { received = receiver.receive(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    sender.close();
    waiter.join();
    CHECK(!received.has_value());
    CHECK(receiver.is_closed());
}

void test_channel_sender_receiver_handles() {
    mlc::concurrency::Channel<int> channel(2);
    auto sender = channel.sender();
    auto receiver = channel.receiver();
    CHECK(sender.send(9));
    auto value = receiver.receive();
    CHECK(value.has_value() && *value == 9);
}

void test_cancel_unblocks_receive() {
    mlc::concurrency::Channel<int> channel(1);
    mlc::concurrency::StopSource stop;
    mlc::concurrency::ChannelReceiveResult<int> result{
        mlc::concurrency::ChannelStatus::Ok, std::nullopt};
    std::thread waiter([&] { result = channel.receive(stop.token()); });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    stop.request();
    waiter.join();
    CHECK(result.status == mlc::concurrency::ChannelStatus::Cancelled);
    CHECK(!result.value.has_value());
}

void test_cancel_unblocks_send_when_full() {
    mlc::concurrency::Channel<int> channel(1);
    CHECK(channel.send(1));
    mlc::concurrency::StopSource stop;
    std::atomic<int> status_value{-1};
    std::thread waiter([&] {
        status_value.store(
            static_cast<int>(channel.send(2, stop.token())),
            std::memory_order_release);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    stop.request();
    waiter.join();
    CHECK(status_value.load() == static_cast<int>(mlc::concurrency::ChannelStatus::Cancelled));
}

void test_cancellable_send_receive_ok() {
    mlc::concurrency::Channel<int> channel(2);
    mlc::concurrency::StopSource stop;
    CHECK(channel.send(7, stop.token()) == mlc::concurrency::ChannelStatus::Ok);
    auto result = channel.receive(stop.token());
    CHECK(result.status == mlc::concurrency::ChannelStatus::Ok);
    CHECK(result.value.has_value() && *result.value == 7);
}

void test_unbounded_accepts_past_fixed_capacity() {
    auto channel = mlc::concurrency::make_unbounded_channel<int>();
    CHECK(channel.is_unbounded());
    constexpr int send_count = 256;
    for (int index = 0; index < send_count; ++index) {
        CHECK(channel.try_send(index));
    }
    CHECK(channel.size() == static_cast<size_t>(send_count));
    for (int index = 0; index < send_count; ++index) {
        auto value = channel.receive();
        CHECK(value.has_value() && *value == index);
    }
    CHECK(channel.size() == 0);
}

int main() {
    test_capacity_validation();
    test_rendezvous_handoff();
    test_rendezvous_close_unblocks_send();
    test_rendezvous_close_unblocks_recv();
    test_rendezvous_try_send_without_receiver();
    test_bounded_try_send();
    test_copy_isolation();
    test_close_blocks_send();
    test_receive_after_close_empty();
    test_threaded_round_trip();
    test_string_copy();
    test_sender_receiver_open_channel();
    test_sender_clone_last_drop_closes();
    test_sender_explicit_close_wakes_receiver();
    test_channel_sender_receiver_handles();
    test_cancel_unblocks_receive();
    test_cancel_unblocks_send_when_full();
    test_cancellable_send_receive_ok();
    test_unbounded_accepts_past_fixed_capacity();

    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
