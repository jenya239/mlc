#pragma once

// Serial state machine + bounded mailbox (TRACK_CONCURRENCY_ISOLATE STEP=2).
// Guarantee: handler runs on one owner thread; State is never concurrent.
// Overflow policy v1: Block (send waits). Drop/Reject deferred.

#include "mlc/concurrency/channel.hpp"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <thread>
#include <utility>

namespace mlc::concurrency {

template<typename State, typename Message>
class Isolate {
    Channel<Message> mailbox_;
    State state_;
    std::function<void(State&, Message)> handler_;
    std::thread owner_;
    bool shut_down_ = false;

    void owner_loop() {
        while (true) {
            auto message = mailbox_.receive();
            if (!message.has_value()) break;
            handler_(state_, std::move(*message));
        }
    }

public:
    template<typename Handler>
    Isolate(State initial_state, std::size_t mailbox_capacity, Handler&& handler)
        : mailbox_(mailbox_capacity == 0 ? 1 : mailbox_capacity)
        , state_(std::move(initial_state))
        , handler_(std::forward<Handler>(handler)) {
        if (mailbox_capacity == 0) {
            throw std::invalid_argument("Isolate mailbox_capacity must be >= 1");
        }
        owner_ = std::thread([this] { owner_loop(); });
    }

    Isolate(const Isolate&) = delete;
    Isolate& operator=(const Isolate&) = delete;
    Isolate(Isolate&&) = delete;
    Isolate& operator=(Isolate&&) = delete;

    ~Isolate() { shutdown(); }

    // Enqueue a message. Blocks if the mailbox is full. Returns false after shutdown.
    bool send(const Message& message) {
        if (shut_down_) return false;
        return mailbox_.send(message);
    }

    bool send(Message&& message) {
        if (shut_down_) return false;
        return mailbox_.send(std::move(message));
    }

    // Close mailbox and join the owner thread. Idempotent. Drains queued messages first.
    void shutdown() {
        if (shut_down_) return;
        shut_down_ = true;
        mailbox_.close();
        if (owner_.joinable()) owner_.join();
    }

    [[nodiscard]] bool is_shutdown() const noexcept { return shut_down_; }

    // Read-only snapshot after shutdown (owner joined). Undefined if still running.
    [[nodiscard]] const State& state_after_shutdown() const {
        if (!shut_down_) {
            throw std::logic_error("Isolate::state_after_shutdown requires shutdown");
        }
        return state_;
    }
};

} // namespace mlc::concurrency
