// Multi-producer / multi-consumer channel stress (TRACK_CONCURRENCY STEP=7).
// g++ -std=c++20 -pthread -I../include -o stress_channel stress_channel.cpp

#include "mlc/concurrency/channel.hpp"
#include <atomic>
#include <iostream>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

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
    test_multi_producer_consumer_stress();
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
