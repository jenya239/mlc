#ifndef MLC_CORE_HASHMAP_HPP
#define MLC_CORE_HASHMAP_HPP

#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>
#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"

namespace mlc {

// COW (Copy-on-Write) HashMap wrapper over std::unordered_map<K,V>.
template<typename K, typename V>
class HashMap {
    struct Buffer {
        std::unordered_map<K, V> data;
        size_t refcount;

        Buffer() : data(), refcount(1) {}
        explicit Buffer(std::unordered_map<K, V> d) : data(std::move(d)), refcount(1) {}
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
    HashMap() : buf_(new Buffer()) {}

    HashMap(const HashMap& other) : buf_(other.buf_) { buf_->refcount++; }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            release();
            buf_ = other.buf_;
            buf_->refcount++;
        }
        return *this;
    }

    HashMap(HashMap&& other) noexcept : buf_(other.buf_) { other.buf_ = nullptr; }

    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            release();
            buf_ = other.buf_;
            other.buf_ = nullptr;
        }
        return *this;
    }

    ~HashMap() { release(); }

    // Immutable operations
    size_t size() const { return buf_->data.size(); }
    bool empty() const { return buf_->data.empty(); }

    bool has(const K& key) const {
        return buf_->data.find(key) != buf_->data.end();
    }

    V get(const K& key) const {
        auto it = buf_->data.find(key);
        if (it != buf_->data.end()) return it->second;
        return V{};
    }

    // Mutable operations (trigger COW)
    void set(const K& key, const V& value) {
        detach();
        buf_->data[key] = value;
    }

    void remove(const K& key) {
        detach();
        buf_->data.erase(key);
    }

    // Get keys as Array
    mlc::Array<K> keys() const {
        mlc::Array<K> result;
        for (const auto& [k, v] : buf_->data) {
            result.push_back(k);
        }
        return result;
    }

    // Get values as Array
    mlc::Array<V> values() const {
        mlc::Array<V> result;
        for (const auto& [k, v] : buf_->data) {
            result.push_back(v);
        }
        return result;
    }

    // Iteration
    auto begin() const { return buf_->data.begin(); }
    auto end() const { return buf_->data.end(); }
};

} // namespace mlc

// Hash support for mlc::String
namespace std {
template<>
struct hash<mlc::String> {
    size_t operator()(const mlc::String& s) const {
        return std::hash<std::string>{}(s.as_std_string());
    }
};
}

#endif // MLC_CORE_HASHMAP_HPP
