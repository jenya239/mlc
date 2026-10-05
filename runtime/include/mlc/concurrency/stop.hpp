#pragma once

// Cooperative cancellation (TRACK_CONCURRENCY_V2 STEP=5).
// Wraps C++20 stop_source/stop_token; do not expose std types in the public API.

#include <functional>
#include <memory>
#include <stop_token>
#include <utility>

namespace mlc::concurrency {

class StopSubscription {
    using Callback = std::function<void()>;
    std::unique_ptr<std::stop_callback<Callback>> callback_;

public:
    StopSubscription() = default;

    explicit StopSubscription(std::unique_ptr<std::stop_callback<Callback>> callback) noexcept
        : callback_(std::move(callback)) {}

    StopSubscription(const StopSubscription&) = delete;
    StopSubscription& operator=(const StopSubscription&) = delete;

    StopSubscription(StopSubscription&&) noexcept = default;
    StopSubscription& operator=(StopSubscription&&) noexcept = default;

    ~StopSubscription() { unsubscribe(); }

    void unsubscribe() noexcept { callback_.reset(); }
};

class StopToken {
    std::stop_token token_;

public:
    StopToken() noexcept = default;

    explicit StopToken(std::stop_token token) noexcept : token_(std::move(token)) {}

    [[nodiscard]] bool requested() const noexcept { return token_.stop_requested(); }

    // Runtime waits (channel/sleep) register stop_callback via this handle.
    [[nodiscard]] std::stop_token native_token() const noexcept { return token_; }

    // The callback runs on request(), including when the token is already requested.
    // It must not resume a coroutine. unsubscribe removes it.
    [[nodiscard]] StopSubscription subscribe(std::function<void()> callback) const {
        auto registered = std::make_unique<std::stop_callback<std::function<void()>>>(
            token_, std::move(callback));
        return StopSubscription(std::move(registered));
    }
};

class StopSource {
    std::stop_source source_;

public:
    StopSource() = default;

    [[nodiscard]] StopToken token() const noexcept { return StopToken(source_.get_token()); }

    void request() noexcept { source_.request_stop(); }

    [[nodiscard]] bool requested() const noexcept { return source_.stop_requested(); }
};

} // namespace mlc::concurrency
