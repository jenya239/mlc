#ifndef MLC_CORE_ARRAY_HPP
#define MLC_CORE_ARRAY_HPP

#include <vector>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>

namespace mlc {

// COW (Copy-on-Write) Array wrapper over std::vector<T>.
// Copying is O(1) via reference counting.
// Mutation triggers detach (deep copy) only when shared.
template<typename T>
class Array {
    struct Buffer {
        std::vector<T> data;
        size_t refcount;

        Buffer() : data(), refcount(1) {}
        explicit Buffer(std::vector<T> d) : data(std::move(d)), refcount(1) {}
        Buffer(std::initializer_list<T> init) : data(init), refcount(1) {}
    };

    Buffer* buf_;

    void detach() {
        if (buf_->refcount > 1) {
            buf_->refcount--;
            buf_ = new Buffer(buf_->data);
        }
    }

    void release() {
        if (buf_ && --buf_->refcount == 0) {
            delete buf_;
        }
    }

public:
    // Constructors
    Array() : buf_(new Buffer()) {}
    Array(std::initializer_list<T> init) : buf_(new Buffer(init)) {}
    explicit Array(std::vector<T> vec) : buf_(new Buffer(std::move(vec))) {}

    // Iterator-based constructor (for slicing)
    template<typename Iter>
    Array(Iter first, Iter last) : buf_(new Buffer(std::vector<T>(first, last))) {}

    // Copy: RC++
    Array(const Array& other) : buf_(other.buf_) {
        buf_->refcount++;
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            release();
            buf_ = other.buf_;
            buf_->refcount++;
        }
        return *this;
    }

    // Move
    Array(Array&& other) noexcept : buf_(other.buf_) {
        other.buf_ = nullptr;
    }

    Array& operator=(Array&& other) noexcept {
        if (this != &other) {
            release();
            buf_ = other.buf_;
            other.buf_ = nullptr;
        }
        return *this;
    }

    ~Array() { release(); }

    // Immutable operations (no detach needed)
    size_t size() const { return buf_->data.size(); }
    bool empty() const { return buf_->data.empty(); }

    const T& operator[](size_t i) const { return buf_->data[i]; }
    const T& operator[](int32_t i) const { return buf_->data[static_cast<size_t>(i)]; }

    const T& at(size_t i) const { return buf_->data.at(i); }

    // Iterators (immutable)
    auto begin() const { return buf_->data.begin(); }
    auto end() const { return buf_->data.end(); }
    auto cbegin() const { return buf_->data.cbegin(); }
    auto cend() const { return buf_->data.cend(); }

    // Mutable operations (trigger COW detach)
    void push_back(const T& value) { detach(); buf_->data.push_back(value); }
    void push_back(T&& value) { detach(); buf_->data.push_back(std::move(value)); }

    void pop_back() { detach(); buf_->data.pop_back(); }

    // Explicit mutation methods (prefer over operator[] for write)
    void set(size_t i, const T& value) { detach(); buf_->data[i] = value; }

    // Compatibility with std::vector interface used by collections.hpp
    void reserve(size_t n) { detach(); buf_->data.reserve(n); }

    // Access underlying data (read-only, for interop)
    const std::vector<T>& data() const { return buf_->data; }
};

} // namespace mlc

#endif // MLC_CORE_ARRAY_HPP
