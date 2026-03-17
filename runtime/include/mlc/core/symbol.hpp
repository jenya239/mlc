#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace mlc {

// Symbol — interned string. Equality is O(1) integer comparison.
// Symbols with the same name are always identical objects (same id).
class Symbol {
public:
    using id_t = uint32_t;

    explicit Symbol(const char* name) : id_(intern(name)) {}
    explicit Symbol(const std::string& name) : id_(intern(name.c_str())) {}
    explicit Symbol(std::string_view name) : id_(intern(std::string(name).c_str())) {}

    [[nodiscard]] const std::string& name() const { return table()[id_]; }
    [[nodiscard]] id_t id() const { return id_; }

    bool operator==(const Symbol& o) const { return id_ == o.id_; }
    bool operator!=(const Symbol& o) const { return id_ != o.id_; }
    bool operator<(const Symbol& o) const { return id_ < o.id_; }

    [[nodiscard]] std::string to_string() const { return name(); }
    [[nodiscard]] std::string inspect() const { return ":" + name(); }

    friend std::ostream& operator<<(std::ostream& os, const Symbol& s) {
        return os << s.name();
    }

private:
    id_t id_;

    static std::vector<std::string>& table() {
        static std::vector<std::string> t;
        return t;
    }

    static std::unordered_map<std::string, id_t>& index() {
        static std::unordered_map<std::string, id_t> m;
        return m;
    }

    static std::mutex& mtx() {
        static std::mutex m;
        return m;
    }

    static id_t intern(const char* name) {
        std::lock_guard<std::mutex> g(mtx());
        auto& idx = index();
        auto it = idx.find(name);
        if (it != idx.end()) return it->second;
        auto id = static_cast<id_t>(table().size());
        table().emplace_back(name);
        idx.emplace(name, id);
        return id;
    }
};

} // namespace mlc

template <>
struct std::hash<mlc::Symbol> {
    size_t operator()(const mlc::Symbol& s) const noexcept {
        return std::hash<uint32_t>{}(s.id());
    }
};
