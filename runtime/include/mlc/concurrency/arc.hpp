#pragma once

// Thread-safe shared ownership (TRACK_CONCURRENCY STEP=5). Distinct from single-thread Shared<T>.

#include <atomic>
#include <utility>

namespace mlc::concurrency {

template<typename Value>
class Arc {
    struct ControlBlock {
        std::atomic<int> strong_count;
        Value value;

        template<typename... Arguments>
        explicit ControlBlock(Arguments&&... arguments)
            : strong_count(1), value(std::forward<Arguments>(arguments)...) {}
    };

    ControlBlock* control_block_;

    void release() noexcept {
        if (!control_block_) return;
        if (control_block_->strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete control_block_;
        }
    }

    static void retain(ControlBlock* block) noexcept {
        if (block) block->strong_count.fetch_add(1, std::memory_order_relaxed);
    }

public:
    template<typename... Arguments>
    explicit Arc(Arguments&&... arguments)
        : control_block_(new ControlBlock(std::forward<Arguments>(arguments)...)) {}

    Arc(const Arc& other) noexcept : control_block_(other.control_block_) {
        retain(control_block_);
    }

    Arc(Arc&& other) noexcept : control_block_(other.control_block_) {
        other.control_block_ = nullptr;
    }

    Arc& operator=(const Arc& other) noexcept {
        if (this != &other) {
            release();
            control_block_ = other.control_block_;
            retain(control_block_);
        }
        return *this;
    }

    Arc& operator=(Arc&& other) noexcept {
        if (this != &other) {
            release();
            control_block_ = other.control_block_;
            other.control_block_ = nullptr;
        }
        return *this;
    }

    ~Arc() { release(); }

    Value& operator*() noexcept { return control_block_->value; }
    const Value& operator*() const noexcept { return control_block_->value; }

    Value* operator->() noexcept { return &control_block_->value; }
    const Value* operator->() const noexcept { return &control_block_->value; }

    long use_count() const noexcept {
        return control_block_ ? control_block_->strong_count.load(std::memory_order_relaxed) : 0;
    }
};

} // namespace mlc::concurrency
