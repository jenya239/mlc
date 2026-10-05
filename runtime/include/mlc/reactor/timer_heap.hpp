#pragma once

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <optional>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

namespace mlc::reactor {

class TimerHeap {
    struct TimerEntry {
        std::chrono::steady_clock::time_point deadline;
        std::uint64_t identifier;
        std::coroutine_handle<> continuation;
    };

    struct EarlierDeadlineFirst {
        bool operator()(const TimerEntry& left, const TimerEntry& right) const {
            return left.deadline > right.deadline;
        }
    };

    std::priority_queue<TimerEntry, std::vector<TimerEntry>, EarlierDeadlineFirst> entries_;
    std::unordered_set<std::uint64_t> cancelled_identifiers_;
    std::uint64_t next_identifier_ = 1;

    void discard_cancelled() {
        while (!entries_.empty() &&
               cancelled_identifiers_.contains(entries_.top().identifier)) {
            cancelled_identifiers_.erase(entries_.top().identifier);
            entries_.pop();
        }
    }

public:
    std::uint64_t push(
        std::chrono::steady_clock::time_point deadline,
        std::coroutine_handle<> continuation) {
        const std::uint64_t identifier = next_identifier_;
        next_identifier_ += 1;
        if (next_identifier_ == 0) {
            next_identifier_ = 1;
        }
        entries_.push(TimerEntry{deadline, identifier, continuation});
        return identifier;
    }

    void cancel(std::uint64_t identifier) { cancelled_identifiers_.insert(identifier); }

    std::optional<std::chrono::steady_clock::time_point> next_deadline() {
        discard_cancelled();
        if (entries_.empty()) {
            return std::nullopt;
        }
        return entries_.top().deadline;
    }

    std::vector<std::coroutine_handle<>> pop_expired(std::chrono::steady_clock::time_point now) {
        std::vector<std::coroutine_handle<>> ready;
        while (!entries_.empty()) {
            discard_cancelled();
            if (entries_.empty() || entries_.top().deadline > now) {
                break;
            }
            ready.push_back(entries_.top().continuation);
            entries_.pop();
        }
        return ready;
    }
};

}  // namespace mlc::reactor
