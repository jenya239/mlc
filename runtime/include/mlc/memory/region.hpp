#pragma once

// Scoped arena (TRACK_LANG_REGION_ARENA STEP=4).
// RegionHandle owns a monotonic_buffer_resource; alloc placement-news into it.
// Phantom Tag from the checker is erased at runtime.

#include <memory_resource>
#include <new>
#include <utility>

namespace mlc::memory {

template<typename Value>
class RegionPtr {
    Value* pointer_ = nullptr;

public:
    RegionPtr() = default;
    explicit RegionPtr(Value* pointer) noexcept : pointer_(pointer) {}

    [[nodiscard]] Value& operator*() const noexcept { return *pointer_; }
    [[nodiscard]] Value* operator->() const noexcept { return pointer_; }
    [[nodiscard]] Value* get() const noexcept { return pointer_; }
};

class RegionHandle {
    std::pmr::monotonic_buffer_resource resource_;

public:
    RegionHandle() = default;
    RegionHandle(const RegionHandle&) = delete;
    RegionHandle& operator=(const RegionHandle&) = delete;
    RegionHandle(RegionHandle&&) = delete;
    RegionHandle& operator=(RegionHandle&&) = delete;

    // Destructors of allocated values are not run; the arena frees storage at once.
    template<typename Value>
    RegionPtr<Value> alloc(Value value) {
        void* memory = resource_.allocate(sizeof(Value), alignof(Value));
        Value* pointer = new (memory) Value(std::move(value));
        return RegionPtr<Value>(pointer);
    }
};

} // namespace mlc::memory
