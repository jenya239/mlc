#ifndef MLC_CORE_ARRAY_HPP
#define MLC_CORE_ARRAY_HPP

#include <vector>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>
#include <memory>
#include <functional>
#include <optional>
#include <utility>

namespace mlc {

// COW (Copy-on-Write) Array over std::vector<T>.
// Copying is O(1) via shared_ptr. Mutation triggers detach only when shared.
template<typename T>
class Array {
    std::shared_ptr<std::vector<T>> data_;

public:
    using value_type = T;

private:

    void detach() {
        if (data_.use_count() > 1)
            data_ = std::make_shared<std::vector<T>>(*data_);
    }

public:
    Array() : data_(std::make_shared<std::vector<T>>()) {}
    Array(std::initializer_list<T> init) : data_(std::make_shared<std::vector<T>>(init)) {}
    explicit Array(std::vector<T> vec) : data_(std::make_shared<std::vector<T>>(std::move(vec))) {}

    template<typename Iter>
    Array(Iter first, Iter last) : data_(std::make_shared<std::vector<T>>(first, last)) {}

    // Copy/move/dtor — all default via shared_ptr
    Array(const Array&) = default;
    Array& operator=(const Array&) = default;
    Array(Array&&) noexcept = default;
    Array& operator=(Array&&) noexcept = default;
    ~Array() = default;

    size_t size() const   { return data_->size(); }
    int length() const { return static_cast<int>(data_->size()); }
    bool empty() const    { return data_->empty(); }

    template<typename Sep>
    T join(const Sep& sep) const {
        if (data_->empty()) return T{};
        T result = (*data_)[0];
        for (size_t i = 1; i < data_->size(); ++i)
            result = result + sep + (*data_)[i];
        return result;
    }

    const T& operator[](size_t i)   const { return (*data_)[i]; }
    const T& operator[](int32_t i)  const { return (*data_)[static_cast<size_t>(i)]; }
    const T& at(size_t i)           const { return data_->at(i); }

    auto begin()  const { return data_->begin(); }
    auto end()    const { return data_->end(); }
    auto cbegin() const { return data_->cbegin(); }
    auto cend()   const { return data_->cend(); }

    void push_back(const T& value) { detach(); data_->push_back(value); }
    void push_back(T&& value)      { detach(); data_->push_back(std::move(value)); }
    void pop_back()                { detach(); data_->pop_back(); }
    void set(size_t i, const T& v) { detach(); (*data_)[i] = v; }
    void reserve(size_t n)         { detach(); data_->reserve(n); }

    const std::vector<T>& data() const { return *data_; }

    // ── Higher-order methods ──────────────────────────────────────────────────

    template<typename F>
    auto map(F&& fn) const -> Array<std::decay_t<std::invoke_result_t<F, const T&>>> {
        using R = std::decay_t<std::invoke_result_t<F, const T&>>;
        Array<R> result;
        result.reserve(data_->size());
        for (const auto& item : *data_) result.push_back(fn(item));
        return result;
    }

    template<typename F>
    Array<T> filter(F&& pred) const {
        Array<T> result;
        for (const auto& item : *data_)
            if (pred(item)) result.push_back(item);
        return result;
    }

    template<typename Acc, typename F>
    Acc fold(Acc init, F&& fn) const {
        for (const auto& item : *data_) init = fn(init, item);
        return init;
    }

    template<typename F>
    void each(F&& fn) const {
        for (const auto& item : *data_) fn(item);
    }

    template<typename F>
    bool any(F&& pred) const {
        for (const auto& item : *data_) if (pred(item)) return true;
        return false;
    }

    template<typename F>
    bool all(F&& pred) const {
        for (const auto& item : *data_) if (!pred(item)) return false;
        return true;
    }

    template<typename F>
    bool none(F&& pred) const {
        for (const auto& item : *data_) if (pred(item)) return false;
        return true;
    }

    template<typename F>
    std::optional<T> find(F&& pred) const {
        for (const auto& item : *data_) if (pred(item)) return item;
        return std::nullopt;
    }

    template<typename F>
    int32_t find_index(F&& pred) const {
        for (int32_t i = 0; i < static_cast<int32_t>(data_->size()); ++i)
            if (pred((*data_)[i])) return i;
        return -1;
    }

    bool contains(const T& v) const {
        for (const auto& item : *data_) if (item == v) return true;
        return false;
    }

    int32_t index_of(const T& v) const {
        for (int32_t i = 0; i < static_cast<int32_t>(data_->size()); ++i)
            if ((*data_)[i] == v) return i;
        return -1;
    }

    Array<T> reverse() const {
        Array<T> result(*data_);
        std::reverse(result.data_->begin(), result.data_->end());
        return result;
    }

    Array<T> sort() const {
        Array<T> result(*data_);
        std::sort(result.data_->begin(), result.data_->end());
        return result;
    }

    template<typename F>
    Array<T> sort_by(F&& key_fn) const {
        Array<T> result(*data_);
        std::sort(result.data_->begin(), result.data_->end(),
            [&](const T& a, const T& b) { return key_fn(a) < key_fn(b); });
        return result;
    }

    Array<T> take(int32_t n) const {
        int32_t count = std::min(n, static_cast<int32_t>(data_->size()));
        return Array<T>(data_->begin(), data_->begin() + count);
    }

    Array<T> drop(int32_t n) const {
        int32_t skip = std::min(n, static_cast<int32_t>(data_->size()));
        return Array<T>(data_->begin() + skip, data_->end());
    }

    Array<T> first_n(int32_t n) const { return take(n); }
    Array<T> last_n(int32_t n) const {
        int32_t count = std::min(n, static_cast<int32_t>(data_->size()));
        return Array<T>(data_->end() - count, data_->end());
    }

    template<typename F>
    auto flat_map(F&& fn) const -> Array<typename std::decay_t<std::invoke_result_t<F, const T&>>::value_type> {
        using R = typename std::decay_t<std::invoke_result_t<F, const T&>>::value_type;
        Array<R> result;
        for (const auto& item : *data_) {
            auto sub = fn(item);
            for (const auto& r : sub) result.push_back(r);
        }
        return result;
    }

    template<typename B>
    Array<std::pair<T, B>> zip(const Array<B>& other) const {
        Array<std::pair<T, B>> result;
        size_t n = std::min(data_->size(), other.size());
        for (size_t i = 0; i < n; ++i)
            result.push_back({(*data_)[i], other[i]});
        return result;
    }

    Array<std::pair<int32_t, T>> enumerate() const {
        Array<std::pair<int32_t, T>> result;
        for (int32_t i = 0; i < static_cast<int32_t>(data_->size()); ++i)
            result.push_back({i, (*data_)[i]});
        return result;
    }

    Array<T> concat(const Array<T>& other) const {
        Array<T> result(*data_);
        for (const auto& item : other) result.push_back(item);
        return result;
    }

    T first() const { return (*data_)[0]; }
    T last()  const { return (*data_)[data_->size() - 1]; }
};

} // namespace mlc

#endif // MLC_CORE_ARRAY_HPP
