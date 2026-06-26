// Channel smoke tests (TRACK_CONCURRENCY STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_channel test_channel.cpp

#include "mlc/concurrency/channel.hpp"
#include "mlc/core/string.hpp"
#include <atomic>
#include <iostream>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while(0)

void test_capacity_validation() {
    bool threw = false;
    try { mlc::concurrency::Channel<int>(0); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
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

int main() {
    test_capacity_validation();
    test_bounded_try_send();
    test_copy_isolation();
    test_close_blocks_send();
    test_receive_after_close_empty();
    test_threaded_round_trip();
    test_string_copy();

    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
