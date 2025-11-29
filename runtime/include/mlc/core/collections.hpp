#ifndef AURORA_COLLECTIONS_HPP
#define AURORA_COLLECTIONS_HPP

#include <vector>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cstdlib>
#include "mlc/core/string.hpp"

namespace mlc::collections {

template <typename T, typename Func>
auto map(const std::vector<T>& items, Func&& func) {
    using Result = std::decay_t<std::invoke_result_t<Func, const T&>>;
    std::vector<Result> result;
    result.reserve(items.size());
    for (const auto& item : items) {
        result.push_back(func(item));
    }
    return result;
}

template <typename T, typename Func>
auto filter(const std::vector<T>& items, Func&& predicate) {
    std::vector<T> result;
    for (const auto& item : items) {
        if (predicate(item)) {
            result.push_back(item);
        }
    }
    return result;
}

template <typename T, typename Acc, typename Func>
Acc fold(const std::vector<T>& items, Acc acc, Func&& reducer) {
    for (const auto& item : items) {
        acc = reducer(acc, item);
    }
    return acc;
}

template <typename T>
bool is_empty(const std::vector<T>& items) {
    return items.empty();
}

template <typename T>
mlc::String join(const std::vector<T>& items, const mlc::String& separator) {
    if (items.empty()) {
        return mlc::String("");
    }

    mlc::String result = mlc::to_string(items.front());
    for (size_t i = 1; i < items.size(); ++i) {
        result += separator;
        result += mlc::to_string(items[i]);
    }
    return result;
}

// Specialized version for string arrays
inline mlc::String join_strings(const std::vector<mlc::String>& items, const mlc::String& separator) {
    return join(items, separator);
}

// Reverse array (generic)
template <typename T>
std::vector<T> reverse(const std::vector<T>& items) {
    std::vector<T> result(items.rbegin(), items.rend());
    return result;
}

// Specialized reverse functions
inline std::vector<int32_t> reverse_i32(const std::vector<int32_t>& items) {
    return reverse(items);
}

inline std::vector<float> reverse_f32(const std::vector<float>& items) {
    return reverse(items);
}

inline std::vector<mlc::String> reverse_str(const std::vector<mlc::String>& items) {
    return reverse(items);
}

// Take first n elements (generic)
template <typename T>
std::vector<T> take(const std::vector<T>& items, int32_t n) {
    if (n <= 0) return std::vector<T>{};
    size_t count = std::min(static_cast<size_t>(n), items.size());
    return std::vector<T>(items.begin(), items.begin() + count);
}

// Specialized take functions
inline std::vector<int32_t> take_i32(const std::vector<int32_t>& items, int32_t n) {
    return take(items, n);
}

inline std::vector<float> take_f32(const std::vector<float>& items, int32_t n) {
    return take(items, n);
}

inline std::vector<mlc::String> take_str(const std::vector<mlc::String>& items, int32_t n) {
    return take(items, n);
}

// Drop first n elements (generic)
template <typename T>
std::vector<T> drop(const std::vector<T>& items, int32_t n) {
    if (n <= 0) return items;
    size_t skip = std::min(static_cast<size_t>(n), items.size());
    return std::vector<T>(items.begin() + skip, items.end());
}

// Specialized drop functions
inline std::vector<int32_t> drop_i32(const std::vector<int32_t>& items, int32_t n) {
    return drop(items, n);
}

inline std::vector<float> drop_f32(const std::vector<float>& items, int32_t n) {
    return drop(items, n);
}

inline std::vector<mlc::String> drop_str(const std::vector<mlc::String>& items, int32_t n) {
    return drop(items, n);
}

// Slice array [start, end) (generic)
template <typename T>
std::vector<T> slice(const std::vector<T>& items, int32_t start, int32_t end) {
    if (start < 0) start = 0;
    if (end > static_cast<int32_t>(items.size())) end = items.size();
    if (start >= end) return std::vector<T>{};
    return std::vector<T>(items.begin() + start, items.begin() + end);
}

// Specialized slice functions
inline std::vector<int32_t> slice_i32(const std::vector<int32_t>& items, int32_t start, int32_t end) {
    return slice(items, start, end);
}

inline std::vector<float> slice_f32(const std::vector<float>& items, int32_t start, int32_t end) {
    return slice(items, start, end);
}

inline std::vector<mlc::String> slice_str(const std::vector<mlc::String>& items, int32_t start, int32_t end) {
    return slice(items, start, end);
}

// Check if array contains element (generic)
template <typename T>
bool contains(const std::vector<T>& items, const T& element) {
    for (const auto& item : items) {
        if (item == element) return true;
    }
    return false;
}

// Specialized contains functions
inline bool contains_i32(const std::vector<int32_t>& items, int32_t element) {
    return contains(items, element);
}

inline bool contains_f32(const std::vector<float>& items, float element) {
    return contains(items, element);
}

inline bool contains_str(const std::vector<mlc::String>& items, const mlc::String& element) {
    return contains(items, element);
}

// Sum for integers
inline int32_t sum_i32(const std::vector<int32_t>& items) {
    int32_t result = 0;
    for (auto item : items) {
        result += item;
    }
    return result;
}

// Sum for floats
inline float sum_f32(const std::vector<float>& items) {
    float result = 0.0f;
    for (auto item : items) {
        result += item;
    }
    return result;
}

// Min for integers
inline int32_t min_i32(const std::vector<int32_t>& items) {
    if (items.empty()) return 0;
    int32_t result = items[0];
    for (auto item : items) {
        if (item < result) result = item;
    }
    return result;
}

// Max for integers
inline int32_t max_i32(const std::vector<int32_t>& items) {
    if (items.empty()) return 0;
    int32_t result = items[0];
    for (auto item : items) {
        if (item > result) result = item;
    }
    return result;
}

// Min for floats
inline float min_f32(const std::vector<float>& items) {
    if (items.empty()) return 0.0f;
    float result = items[0];
    for (auto item : items) {
        if (item < result) result = item;
    }
    return result;
}

// Max for floats
inline float max_f32(const std::vector<float>& items) {
    if (items.empty()) return 0.0f;
    float result = items[0];
    for (auto item : items) {
        if (item > result) result = item;
    }
    return result;
}

// Generate range [start, end)
inline std::vector<int32_t> range(int32_t start, int32_t end) {
    std::vector<int32_t> result;
    if (start < end) {
        result.reserve(end - start);
        for (int32_t i = start; i < end; ++i) {
            result.push_back(i);
        }
    }
    return result;
}

// any - check if any element satisfies predicate
template <typename T, typename Func>
bool any(const std::vector<T>& items, Func&& predicate) {
    for (const auto& item : items) {
        if (predicate(item)) return true;
    }
    return false;
}

// all - check if all elements satisfy predicate
template <typename T, typename Func>
bool all(const std::vector<T>& items, Func&& predicate) {
    for (const auto& item : items) {
        if (!predicate(item)) return false;
    }
    return true;
}

// none - check if no element satisfies predicate
template <typename T, typename Func>
bool none(const std::vector<T>& items, Func&& predicate) {
    for (const auto& item : items) {
        if (predicate(item)) return false;
    }
    return true;
}

// find - find first element satisfying predicate (returns optional-like: default value if not found)
template <typename T, typename Func>
T find(const std::vector<T>& items, Func&& predicate) {
    for (const auto& item : items) {
        if (predicate(item)) return item;
    }
    return T{};  // Return default value if not found
}

// find_index - find index of first element satisfying predicate (-1 if not found)
template <typename T, typename Func>
int32_t find_index(const std::vector<T>& items, Func&& predicate) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (predicate(items[i])) return static_cast<int32_t>(i);
    }
    return -1;
}

// index_of - find index of element (-1 if not found)
template <typename T>
int32_t index_of(const std::vector<T>& items, const T& element) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i] == element) return static_cast<int32_t>(i);
    }
    return -1;
}

// concat - concatenate two arrays (returns new array)
template <typename T>
std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size() + b.size());
    result.insert(result.end(), a.begin(), a.end());
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

// append - in-place append (modifies first array, returns reference)
template <typename T>
std::vector<T>& append(std::vector<T>& a, const std::vector<T>& b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

// flatten - flatten nested array
template <typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& items) {
    std::vector<T> result;
    for (const auto& inner : items) {
        result.insert(result.end(), inner.begin(), inner.end());
    }
    return result;
}

// zip - combine two arrays into array of pairs
template <typename T, typename U>
std::vector<std::pair<T, U>> zip(const std::vector<T>& a, const std::vector<U>& b) {
    std::vector<std::pair<T, U>> result;
    size_t len = std::min(a.size(), b.size());
    result.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        result.push_back({a[i], b[i]});
    }
    return result;
}

// enumerate - add index to each element
template <typename T>
std::vector<std::pair<int32_t, T>> enumerate(const std::vector<T>& items) {
    std::vector<std::pair<int32_t, T>> result;
    result.reserve(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        result.push_back({static_cast<int32_t>(i), items[i]});
    }
    return result;
}

// Generic min for any comparable type
template <typename T>
T min(const std::vector<T>& items) {
    if (items.empty()) return T{};
    T result = items[0];
    for (const auto& item : items) {
        if (item < result) result = item;
    }
    return result;
}

// Generic max for any comparable type
template <typename T>
T max(const std::vector<T>& items) {
    if (items.empty()) return T{};
    T result = items[0];
    for (const auto& item : items) {
        if (item > result) result = item;
    }
    return result;
}

// Sort array (ascending order)
template <typename T>
std::vector<T> sort(const std::vector<T>& items) {
    std::vector<T> result = items;
    std::sort(result.begin(), result.end());
    return result;
}

// Sort array by key function
template <typename T, typename Func>
std::vector<T> sort_by(const std::vector<T>& items, Func&& key_fn) {
    std::vector<T> result = items;
    std::sort(result.begin(), result.end(), [&key_fn](const T& a, const T& b) {
        return key_fn(a) < key_fn(b);
    });
    return result;
}

// Remove duplicates (keeps first occurrence)
template <typename T>
std::vector<T> uniq(const std::vector<T>& items) {
    std::vector<T> result;
    std::unordered_set<T> seen;
    for (const auto& item : items) {
        if (seen.find(item) == seen.end()) {
            seen.insert(item);
            result.push_back(item);
        }
    }
    return result;
}

// Remove duplicates by key function
template <typename T, typename Func>
std::vector<T> uniq_by(const std::vector<T>& items, Func&& key_fn) {
    std::vector<T> result;
    std::unordered_set<decltype(key_fn(items[0]))> seen;
    for (const auto& item : items) {
        auto key = key_fn(item);
        if (seen.find(key) == seen.end()) {
            seen.insert(key);
            result.push_back(item);
        }
    }
    return result;
}

// Group array by key function - returns Map<K, T[]>
template <typename T, typename Func>
auto group_by(const std::vector<T>& items, Func&& key_fn) {
    using KeyType = std::decay_t<decltype(key_fn(items[0]))>;
    std::unordered_map<KeyType, std::vector<T>> result;
    for (const auto& item : items) {
        auto key = key_fn(item);
        result[key].push_back(item);
    }
    return result;
}

// Partition array into two: matching and non-matching
template <typename T, typename Func>
std::pair<std::vector<T>, std::vector<T>> partition(const std::vector<T>& items, Func&& predicate) {
    std::vector<T> matching;
    std::vector<T> non_matching;
    for (const auto& item : items) {
        if (predicate(item)) {
            matching.push_back(item);
        } else {
            non_matching.push_back(item);
        }
    }
    return {matching, non_matching};
}

// Take elements while predicate is true
template <typename T, typename Func>
std::vector<T> take_while(const std::vector<T>& items, Func&& predicate) {
    std::vector<T> result;
    for (const auto& item : items) {
        if (!predicate(item)) break;
        result.push_back(item);
    }
    return result;
}

// Drop elements while predicate is true
template <typename T, typename Func>
std::vector<T> drop_while(const std::vector<T>& items, Func&& predicate) {
    std::vector<T> result;
    bool taking = false;
    for (const auto& item : items) {
        if (!taking && !predicate(item)) {
            taking = true;
        }
        if (taking) {
            result.push_back(item);
        }
    }
    return result;
}

// Get first n elements
template <typename T>
std::vector<T> first_n(const std::vector<T>& items, int32_t n) {
    if (n <= 0) return std::vector<T>{};
    size_t count = std::min(static_cast<size_t>(n), items.size());
    return std::vector<T>(items.begin(), items.begin() + count);
}

// Get last n elements
template <typename T>
std::vector<T> last_n(const std::vector<T>& items, int32_t n) {
    if (n <= 0) return std::vector<T>{};
    size_t count = std::min(static_cast<size_t>(n), items.size());
    return std::vector<T>(items.end() - count, items.end());
}

// Get second element (index 1)
template <typename T>
T second(const std::vector<T>& items) {
    if (items.size() < 2) return T{};
    return items[1];
}

// Get third element (index 2)
template <typename T>
T third(const std::vector<T>& items) {
    if (items.size() < 3) return T{};
    return items[2];
}

// Count elements matching predicate
template <typename T, typename Func>
int32_t count(const std::vector<T>& items, Func&& predicate) {
    int32_t result = 0;
    for (const auto& item : items) {
        if (predicate(item)) ++result;
    }
    return result;
}

// Sum with custom selector function
template <typename T, typename Func>
auto sum_by(const std::vector<T>& items, Func&& selector) {
    using Result = std::decay_t<decltype(selector(items[0]))>;
    Result result{};
    for (const auto& item : items) {
        result += selector(item);
    }
    return result;
}

// Product of numeric array
template <typename T>
T product(const std::vector<T>& items) {
    if (items.empty()) return T{1};
    T result = items[0];
    for (size_t i = 1; i < items.size(); ++i) {
        result *= items[i];
    }
    return result;
}

// Compact - remove default/zero values (useful for Option unwrapping)
template <typename T>
std::vector<T> compact(const std::vector<T>& items) {
    std::vector<T> result;
    for (const auto& item : items) {
        if (item != T{}) {
            result.push_back(item);
        }
    }
    return result;
}

// Rotate array by n positions (positive = left, negative = right)
template <typename T>
std::vector<T> rotate(const std::vector<T>& items, int32_t n) {
    if (items.empty()) return items;
    std::vector<T> result = items;
    int32_t size = static_cast<int32_t>(items.size());
    n = ((n % size) + size) % size;  // Normalize negative indices
    std::rotate(result.begin(), result.begin() + n, result.end());
    return result;
}

// Sample random element
template <typename T>
T sample(const std::vector<T>& items) {
    if (items.empty()) return T{};
    return items[rand() % items.size()];
}

// Chunk array into groups of n
template <typename T>
std::vector<std::vector<T>> in_groups_of(const std::vector<T>& items, int32_t n) {
    std::vector<std::vector<T>> result;
    if (n <= 0) return result;
    for (size_t i = 0; i < items.size(); i += n) {
        size_t end = std::min(i + n, items.size());
        result.push_back(std::vector<T>(items.begin() + i, items.begin() + end));
    }
    return result;
}

// Split array into n groups
template <typename T>
std::vector<std::vector<T>> in_groups(const std::vector<T>& items, int32_t n) {
    std::vector<std::vector<T>> result(n);
    if (n <= 0) return result;
    for (size_t i = 0; i < items.size(); ++i) {
        result[i % n].push_back(items[i]);
    }
    return result;
}

// Flat map - map then flatten one level
template <typename T, typename Func>
auto flat_map(const std::vector<T>& items, Func&& func) {
    using Inner = std::decay_t<decltype(func(items[0]))>;
    using Element = typename Inner::value_type;
    std::vector<Element> result;
    for (const auto& item : items) {
        auto inner = func(item);
        result.insert(result.end(), inner.begin(), inner.end());
    }
    return result;
}

} // namespace mlc::collections

#endif // AURORA_COLLECTIONS_HPP
