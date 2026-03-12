#ifndef MLC_CORE_HASHMAP_HPP
#define MLC_CORE_HASHMAP_HPP

#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>
#include <memory>
#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"

namespace mlc {

// COW (Copy-on-Write) HashMap over std::unordered_map<K,V>.
// Copying is O(1) via shared_ptr. Mutation triggers detach only when shared.
template<typename K, typename V>
class HashMap {
    std::shared_ptr<std::unordered_map<K, V>> data_;

    void detach() {
        if (data_.use_count() > 1)
            data_ = std::make_shared<std::unordered_map<K, V>>(*data_);
    }

public:
    HashMap() : data_(std::make_shared<std::unordered_map<K, V>>()) {}

    HashMap(const HashMap&) = default;
    HashMap& operator=(const HashMap&) = default;
    HashMap(HashMap&&) noexcept = default;
    HashMap& operator=(HashMap&&) noexcept = default;
    ~HashMap() = default;

    size_t size() const { return data_->size(); }
    bool empty() const  { return data_->empty(); }

    bool has(const K& key) const {
        return data_->find(key) != data_->end();
    }

    V get(const K& key) const {
        auto it = data_->find(key);
        if (it != data_->end()) return it->second;
        return V{};
    }

    void set(const K& key, const V& value) {
        detach();
        (*data_)[key] = value;
    }

    void remove(const K& key) {
        detach();
        data_->erase(key);
    }

    mlc::Array<K> keys() const {
        mlc::Array<K> result;
        for (const auto& [k, v] : *data_)
            result.push_back(k);
        return result;
    }

    mlc::Array<V> values() const {
        mlc::Array<V> result;
        for (const auto& [k, v] : *data_)
            result.push_back(v);
        return result;
    }

    auto begin() const { return data_->begin(); }
    auto end()   const { return data_->end(); }
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
