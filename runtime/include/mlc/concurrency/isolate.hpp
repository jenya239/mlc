#pragma once

// Serial state machine + bounded mailbox (TRACK_CONCURRENCY_ISOLATE STEP=2+3).
// Guarantee: handler runs on one owner thread; State is never concurrent.
// Overflow policy v1: Block (send waits). Drop/Reject deferred.
// shutdown/dtor: request_cancel → close mailbox → join owner.
// Blocked send wakes on cancel (ChannelStatus::Cancelled).

#include "mlc/concurrency/channel.hpp"
#include "mlc/concurrency/stop.hpp"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <thread>
#include <utility>

namespace mlc::concurrency {

template<typename State, typename Message>
class Isolate {
    StopSource stop_;
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

    bool enqueue(Message message) {
        if (shut_down_) return false;
        const ChannelStatus status = mailbox_.send(std::move(message), stop_.token());
        return status == ChannelStatus::Ok;
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

    [[nodiscard]] StopToken token() const noexcept { return stop_.token(); }

    void request_cancel() noexcept { stop_.request(); }

    [[nodiscard]] bool cancel_requested() const noexcept { return stop_.requested(); }

    // Enqueue a message. Blocks if the mailbox is full. Returns false after shutdown/cancel.
    bool send(const Message& message) { return enqueue(message); }

    bool send(Message&& message) { return enqueue(std::move(message)); }

    // request_cancel, close mailbox, join owner. Idempotent. Drains queued messages first.
    void shutdown() {
        if (shut_down_) return;
        shut_down_ = true;
        request_cancel();
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
