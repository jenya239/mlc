#pragma once

// Fixed-width atomics, seq_cst only (TRACK_CONCURRENCY_ATOMICS).
// No acquire/release/relaxed knobs in v1.

#include <atomic>
#include <cstdint>

namespace mlc::concurrency {

class AtomicBool {
    mutable std::atomic<bool> value_;

public:
    explicit AtomicBool(bool value = false) noexcept : value_(value) {}

    [[nodiscard]] bool load() const noexcept {
        return value_.load(std::memory_order_seq_cst);
    }

    void store(bool desired) const noexcept {
        value_.store(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] bool exchange(bool desired) const noexcept {
        return value_.exchange(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] bool compare_exchange(bool& expected, bool desired) const noexcept {
        return value_.compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
    }
};

class AtomicI32 {
    mutable std::atomic<int32_t> value_;

public:
    explicit AtomicI32(int32_t value = 0) noexcept : value_(value) {}

    [[nodiscard]] int32_t load() const noexcept {
        return value_.load(std::memory_order_seq_cst);
    }

    void store(int32_t desired) const noexcept {
        value_.store(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] int32_t exchange(int32_t desired) const noexcept {
        return value_.exchange(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] bool compare_exchange(int32_t& expected, int32_t desired) const noexcept {
        return value_.compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] int32_t fetch_add(int32_t argument) const noexcept {
        return value_.fetch_add(argument, std::memory_order_seq_cst);
    }

    [[nodiscard]] int32_t fetch_sub(int32_t argument) const noexcept {
        return value_.fetch_sub(argument, std::memory_order_seq_cst);
    }
};

class AtomicI64 {
    mutable std::atomic<int64_t> value_;

public:
    explicit AtomicI64(int64_t value = 0) noexcept : value_(value) {}

    [[nodiscard]] int64_t load() const noexcept {
        return value_.load(std::memory_order_seq_cst);
    }

    void store(int64_t desired) const noexcept {
        value_.store(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] int64_t exchange(int64_t desired) const noexcept {
        return value_.exchange(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] bool compare_exchange(int64_t& expected, int64_t desired) const noexcept {
        return value_.compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] int64_t fetch_add(int64_t argument) const noexcept {
        return value_.fetch_add(argument, std::memory_order_seq_cst);
    }

    [[nodiscard]] int64_t fetch_sub(int64_t argument) const noexcept {
        return value_.fetch_sub(argument, std::memory_order_seq_cst);
    }
};

class AtomicU64 {
    mutable std::atomic<uint64_t> value_;

public:
    explicit AtomicU64(uint64_t value = 0) noexcept : value_(value) {}

    [[nodiscard]] uint64_t load() const noexcept {
        return value_.load(std::memory_order_seq_cst);
    }

    void store(uint64_t desired) const noexcept {
        value_.store(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] uint64_t exchange(uint64_t desired) const noexcept {
        return value_.exchange(desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] bool compare_exchange(uint64_t& expected, uint64_t desired) const noexcept {
        return value_.compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
    }

    [[nodiscard]] uint64_t fetch_add(uint64_t argument) const noexcept {
        return value_.fetch_add(argument, std::memory_order_seq_cst);
    }

    [[nodiscard]] uint64_t fetch_sub(uint64_t argument) const noexcept {
        return value_.fetch_sub(argument, std::memory_order_seq_cst);
    }
};

} // namespace mlc::concurrency
