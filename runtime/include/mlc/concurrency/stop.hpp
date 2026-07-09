#pragma once

// Cooperative cancellation (TRACK_CONCURRENCY_V2 STEP=5).
// Wraps C++20 stop_source/stop_token; do not expose std types in the public API.

#include <stop_token>

namespace mlc::concurrency {

class StopToken {
    std::stop_token token_;

public:
    explicit StopToken(std::stop_token token) noexcept : token_(std::move(token)) {}

    [[nodiscard]] bool requested() const noexcept { return token_.stop_requested(); }
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
