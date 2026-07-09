// Channel stress matrix Layer 2 (TRACK_CONCURRENCY_TEST_HARNESS STEP=2).
// Cancel-during-send/recv deferred to T5 (StopToken). Mode A: real threads.
// g++ -std=c++20 -pthread -I../include -o stress_channel stress_channel.cpp

#include "mlc/concurrency/channel.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

static std::atomic<int> passed{0};
static std::atomic<int> failed{0};

#define CHECK(expression) do { \
    if (expression) { passed.fetch_add(1, std::memory_order_relaxed); } \
    else { failed.fetch_add(1, std::memory_order_relaxed); std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_single_sender_receiver() {
    mlc::concurrency::Channel<int> channel(4);
    std::atomic<int> sum{0};
    std::thread sender([&] {
        for (int value = 1; value <= 100; ++value)
            CHECK(channel.send(value));
        channel.close();
    });
    std::thread receiver([&] {
        while (true) {
            auto value = channel.receive();
            if (!value.has_value()) break;
            sum.fetch_add(*value, std::memory_order_relaxed);
        }
    });
    sender.join();
    receiver.join();
    CHECK(sum.load() == 5050);
}

void test_many_senders_one_receiver() {
    constexpr int sender_count = 8;
    constexpr int messages_per_sender = 500;
    constexpr int total = sender_count * messages_per_sender;
    mlc::concurrency::Channel<int> channel(8);
    std::atomic<int> next_id{0};
    std::atomic<int> senders_done{0};
    std::atomic<int> receive_count{0};

    std::vector<std::thread> senders;
    for (int index = 0; index < sender_count; ++index) {
        senders.emplace_back([&] {
            while (true) {
                const int message_id = next_id.fetch_add(1, std::memory_order_relaxed);
                if (message_id >= total) break;
                CHECK(channel.send(message_id));
            }
            if (senders_done.fetch_add(1, std::memory_order_acq_rel) + 1 == sender_count)
                channel.close();
        });
    }
    std::thread receiver([&] {
        while (true) {
            auto value = channel.receive();
            if (!value.has_value()) break;
            receive_count.fetch_add(1, std::memory_order_relaxed);
        }
    });
    for (auto& sender : senders) sender.join();
    receiver.join();
    CHECK(receive_count.load() == total);
}

void test_one_sender_many_receivers() {
    constexpr int receiver_count = 8;
    constexpr int total = 4000;
    mlc::concurrency::Channel<int> channel(8);
    std::atomic<int> receive_count{0};

    std::thread sender([&] {
        for (int value = 0; value < total; ++value)
            CHECK(channel.send(value));
        channel.close();
    });
    std::vector<std::thread> receivers;
    for (int index = 0; index < receiver_count; ++index) {
        receivers.emplace_back([&] {
            while (true) {
                auto value = channel.receive();
                if (!value.has_value()) break;
                receive_count.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    sender.join();
    for (auto& receiver : receivers) receiver.join();
    CHECK(receive_count.load() == total);
}

void test_full_queue_send_blocks() {
    mlc::concurrency::Channel<int> channel(1);
    CHECK(channel.try_send(1));
    std::atomic<bool> send_started{false};
    std::atomic<bool> send_finished{false};
    std::thread blocked_sender([&] {
        send_started.store(true, std::memory_order_release);
        CHECK(channel.send(2));
        send_finished.store(true, std::memory_order_release);
    });
    while (!send_started.load(std::memory_order_acquire))
        std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(!send_finished.load(std::memory_order_acquire));
    auto first = channel.receive();
    CHECK(first.has_value() && *first == 1);
    blocked_sender.join();
    CHECK(send_finished.load(std::memory_order_acquire));
    auto second = channel.receive();
    CHECK(second.has_value() && *second == 2);
    channel.close();
}

void test_empty_queue_recv_blocks() {
    mlc::concurrency::Channel<int> channel(4);
    std::atomic<bool> recv_started{false};
    std::atomic<bool> recv_finished{false};
    std::optional<int> received;
    std::thread blocked_receiver([&] {
        recv_started.store(true, std::memory_order_release);
        received = channel.receive();
        recv_finished.store(true, std::memory_order_release);
    });
    while (!recv_started.load(std::memory_order_acquire))
        std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(!recv_finished.load(std::memory_order_acquire));
    CHECK(channel.send(7));
    blocked_receiver.join();
    CHECK(recv_finished.load(std::memory_order_acquire));
    CHECK(received.has_value() && *received == 7);
    channel.close();
}

void test_close_during_send() {
    mlc::concurrency::Channel<int> channel(1);
    CHECK(channel.try_send(1));
    std::atomic<bool> send_started{false};
    std::atomic<bool> send_result{true};
    std::thread blocked_sender([&] {
        send_started.store(true, std::memory_order_release);
        send_result.store(channel.send(2), std::memory_order_release);
    });
    while (!send_started.load(std::memory_order_acquire))
        std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    channel.close();
    blocked_sender.join();
    CHECK(!send_result.load(std::memory_order_acquire));
}

void test_close_during_recv() {
    mlc::concurrency::Channel<int> channel(4);
    std::atomic<bool> recv_started{false};
    std::optional<int> received = 0;
    std::thread blocked_receiver([&] {
        recv_started.store(true, std::memory_order_release);
        received = channel.receive();
    });
    while (!recv_started.load(std::memory_order_acquire))
        std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    channel.close();
    blocked_receiver.join();
    CHECK(!received.has_value());
}

void test_sender_handle_drop() {
    // Production Channel has no separate Sender; drop one shared handle, other still works.
    auto channel = std::make_optional(mlc::concurrency::Channel<int>(4));
    mlc::concurrency::Channel<int> sender_copy = *channel;
    channel.reset();
    CHECK(sender_copy.send(9));
    auto value = sender_copy.receive();
    CHECK(value.has_value() && *value == 9);
    sender_copy.close();
}

void test_receiver_exits_early() {
    mlc::concurrency::Channel<int> channel(8);
    std::atomic<bool> receiver_done{false};
    std::thread receiver([&] {
        auto value = channel.receive();
        CHECK(value.has_value() && *value == 1);
        receiver_done.store(true, std::memory_order_release);
    });
    CHECK(channel.send(1));
    while (!receiver_done.load(std::memory_order_acquire))
        std::this_thread::yield();
    receiver.join();
    CHECK(channel.send(2));
    channel.close();
    auto leftover = channel.receive();
    CHECK(leftover.has_value() && *leftover == 2);
    CHECK(!channel.receive().has_value());
}

void test_rapid_open_close() {
    for (int iteration = 0; iteration < 1000; ++iteration) {
        mlc::concurrency::Channel<int> channel(2);
        CHECK(channel.try_send(iteration));
        channel.close();
        CHECK(!channel.send(0));
        auto value = channel.receive();
        CHECK(value.has_value() && *value == iteration);
        CHECK(!channel.receive().has_value());
    }
}

void test_one_million_messages() {
    constexpr int total = 1000000;
    mlc::concurrency::Channel<int> channel(64);
    std::atomic<long long> sum{0};
    std::thread sender([&] {
        for (int value = 0; value < total; ++value)
            CHECK(channel.send(value));
        channel.close();
    });
    std::thread receiver([&] {
        while (true) {
            auto value = channel.receive();
            if (!value.has_value()) break;
            sum.fetch_add(*value, std::memory_order_relaxed);
        }
    });
    sender.join();
    receiver.join();
    const long long expected =
        static_cast<long long>(total) * (static_cast<long long>(total) - 1) / 2;
    CHECK(sum.load() == expected);
}

void test_multi_producer_consumer_stress() {
    constexpr int producer_count = 4;
    constexpr int consumer_count = 4;
    constexpr int messages_per_producer = 2500;
    constexpr int total_messages = producer_count * messages_per_producer;
    constexpr int channel_capacity = 8;
    constexpr long long expected_sum =
        static_cast<long long>(total_messages) * (total_messages - 1) / 2;

    mlc::concurrency::Channel<int> channel(channel_capacity);
    std::atomic<int> next_message_id{0};
    std::atomic<int> producers_finished{0};
    std::atomic<long long> receive_sum{0};
    std::atomic<int> receive_count{0};
    std::atomic<bool> send_failed{false};

    auto producer = [&]() {
        while (true) {
            const int message_id = next_message_id.fetch_add(1, std::memory_order_relaxed);
            if (message_id >= total_messages) break;
            if (!channel.send(message_id)) send_failed.store(true, std::memory_order_relaxed);
        }
        if (producers_finished.fetch_add(1, std::memory_order_acq_rel) + 1 == producer_count) {
            channel.close();
        }
    };

    auto consumer = [&]() {
        while (true) {
            const auto value = channel.receive();
            if (!value.has_value()) break;
            receive_sum.fetch_add(*value, std::memory_order_relaxed);
            receive_count.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread producers[producer_count];
    std::thread consumers[consumer_count];
    for (int index = 0; index < producer_count; ++index) producers[index] = std::thread(producer);
    for (int index = 0; index < consumer_count; ++index) consumers[index] = std::thread(consumer);
    for (int index = 0; index < producer_count; ++index) producers[index].join();
    for (int index = 0; index < consumer_count; ++index) consumers[index].join();

    CHECK(!send_failed.load(std::memory_order_relaxed));
    CHECK(receive_count.load(std::memory_order_relaxed) == total_messages);
    CHECK(receive_sum.load(std::memory_order_relaxed) == expected_sum);
}

int main() {
    test_single_sender_receiver();
    test_many_senders_one_receiver();
    test_one_sender_many_receivers();
    test_full_queue_send_blocks();
    test_empty_queue_recv_blocks();
    test_close_during_send();
    test_close_during_recv();
    test_sender_handle_drop();
    test_receiver_exits_early();
    test_rapid_open_close();
    test_multi_producer_consumer_stress();
    test_one_million_messages();
    const int failed_count = failed.load();
    const int passed_count = passed.load();
    if (failed_count == 0) {
        std::cout << "ALL " << passed_count << " checks PASSED\n";
    } else {
        std::cout << passed_count << " passed, " << failed_count << " FAILED\n";
    }
    return failed_count > 0 ? 1 : 0;
}
