#ifndef MLC_STRING_HPP
#define MLC_STRING_HPP

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <memory>
#include "mlc/core/array.hpp"

namespace mlc {

// Forward declarations
class Bytes;

// mlc::String — UTF-8 string.
// Strings ≤ 22 bytes are stored inline (SSO), no heap allocation.
// Strings > 22 bytes are stored via shared_ptr for O(1) copy.
// UTF-8 is not re-encoded; SSO only affects storage.
class String {
    static constexpr size_t SSO_CAPACITY = 22;

    std::shared_ptr<std::string> heap_;  // null in SSO mode
    char    sso_buf_[SSO_CAPACITY + 1];  // inline buffer (+1 for '\0'); valid in SSO mode
    uint8_t sso_len_;                    // byte length in SSO mode (0..22)
    bool    is_ascii_;                   // cached: all bytes < 0x80

    // ── internal helpers ──────────────────────────────────────────────────────

    static bool check_ascii(const char* data, size_t len) noexcept {
        for (size_t i = 0; i < len; ++i)
            if (static_cast<unsigned char>(data[i]) >= 0x80) return false;
        return true;
    }

    void init(const char* data, size_t len, bool ascii) {
        is_ascii_ = ascii;
        if (len <= SSO_CAPACITY) {
            std::memcpy(sso_buf_, data, len);
            sso_buf_[len] = '\0';
            sso_len_ = static_cast<uint8_t>(len);
        } else {
            heap_ = std::make_shared<std::string>(data, len);
            sso_len_ = 0;
        }
    }

    // Private constructors for known-ascii strings (avoid re-scanning)
    String(const char* data, size_t len, bool ascii) : sso_len_(0), is_ascii_(ascii) {
        init(data, len, ascii);
    }

    String(std::string&& s, bool ascii) : sso_len_(0), is_ascii_(ascii) {
        if (s.size() <= SSO_CAPACITY) {
            std::memcpy(sso_buf_, s.data(), s.size());
            sso_buf_[s.size()] = '\0';
            sso_len_ = static_cast<uint8_t>(s.size());
        } else {
            heap_ = std::make_shared<std::string>(std::move(s));
        }
    }

    String(std::shared_ptr<std::string> h, bool ascii)
        : heap_(std::move(h)), sso_len_(0), is_ascii_(ascii) {}

    // UTF-8 helpers (implemented in string.cpp)
    static size_t      utf8_length(std::string_view str) noexcept;
    static size_t      utf8_char_index(std::string_view str, size_t char_pos);
    static std::string utf8_char_at(std::string_view str, size_t char_pos);

public:
    // ── constructors ──────────────────────────────────────────────────────────

    String() noexcept : sso_len_(0), is_ascii_(true) { sso_buf_[0] = '\0'; }

    String(const char* str) : sso_len_(0), is_ascii_(true) {
        if (!str) { sso_buf_[0] = '\0'; return; }
        size_t len = std::strlen(str);
        init(str, len, check_ascii(str, len));
    }

    String(const char* s, size_t len) : sso_len_(0), is_ascii_(true) {
        init(s, len, check_ascii(s, len));
    }

    String(const std::string& str) : sso_len_(0), is_ascii_(true) {
        init(str.data(), str.size(), check_ascii(str.data(), str.size()));
    }

    String(std::string&& str) : sso_len_(0), is_ascii_(true) {
        bool ascii = check_ascii(str.data(), str.size());
        if (str.size() <= SSO_CAPACITY) {
            std::memcpy(sso_buf_, str.data(), str.size());
            sso_buf_[str.size()] = '\0';
            sso_len_ = static_cast<uint8_t>(str.size());
            is_ascii_ = ascii;
        } else {
            heap_ = std::make_shared<std::string>(std::move(str));
            is_ascii_ = ascii;
        }
    }

    // Copy: SSO copies 48 bytes inline (no heap); heap increments refcount — both O(1)
    String(const String&) = default;
    String(String&&) noexcept = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) noexcept = default;

    // ── raw access ────────────────────────────────────────────────────────────

    bool    is_sso()    const noexcept { return !heap_; }
    const char* raw_data() const noexcept { return is_sso() ? sso_buf_ : heap_->data(); }
    size_t      raw_size() const noexcept { return is_sso() ? sso_len_ : heap_->size(); }

    // Zero-copy view — preferred read-only accessor
    std::string_view view() const noexcept { return {raw_data(), raw_size()}; }

    // For C++ interop. Constructs std::string for SSO strings (copy of ≤22 bytes).
    std::string as_std_string() const {
        return is_sso() ? std::string(sso_buf_, sso_len_) : *heap_;
    }

    // c_str() is valid: sso_buf_ is always null-terminated at sso_len_
    const char* c_str() const noexcept { return raw_data(); }

    // ── basic properties ──────────────────────────────────────────────────────

    int    length()    const { return static_cast<int>(is_ascii_ ? raw_size() : utf8_length(view())); }
    int    byte_size() const noexcept { return static_cast<int>(raw_size()); }
    size_t size()      const noexcept { return raw_size(); }
    bool   is_empty()  const noexcept { return raw_size() == 0; }

    // O(1) byte access — for lexer / byte-level processing
    String byte_at(int index) const {
        size_t i = static_cast<size_t>(index);
        if (i >= raw_size()) return String();
        return String(raw_data() + i, 1, true);
    }

    // O(1) byte-indexed substring — always uses raw byte offsets
    String byte_substring(int start, int length) const {
        size_t s = static_cast<size_t>(start);
        size_t n = static_cast<size_t>(length);
        if (s >= raw_size()) return String();
        n = std::min(n, raw_size() - s);
        return String(raw_data() + s, n);
    }

    String byte_substring(int start) const {
        size_t s = static_cast<size_t>(start);
        if (s >= raw_size()) return String();
        return String(raw_data() + s, raw_size() - s);
    }

    // ── character access ──────────────────────────────────────────────────────

    String char_at(size_t index) const {
        if (is_ascii_) return String(raw_data() + index, 1, true);
        return String(utf8_char_at(view(), index));
    }

    char operator[](size_t index) const {
        size_t bi = is_ascii_ ? index : utf8_char_index(view(), index);
        return bi < raw_size() ? raw_data()[bi] : '\0';
    }

    // ── substrings ────────────────────────────────────────────────────────────

    String substring(size_t start) const;
    String substring(size_t start, size_t length) const;

    // ── numeric conversion ────────────────────────────────────────────────────

    int to_i() const { return std::stoi(std::string(view())); }

    // ── case ──────────────────────────────────────────────────────────────────

    String upper()    const;
    String lower()    const;
    String to_lower() const { return lower(); }
    String to_upper() const { return upper(); }

    // ── trimming ──────────────────────────────────────────────────────────────

    String trim()       const;
    String trim_start() const;
    String trim_end()   const;

    // ── splitting ─────────────────────────────────────────────────────────────

    mlc::Array<String> split(const String& delimiter) const;
    mlc::Array<String> chars() const;
    mlc::Array<String> lines() const;

    // ── searching ─────────────────────────────────────────────────────────────

    bool contains(const String& s) const noexcept {
        return view().find(s.view()) != std::string_view::npos;
    }

    bool starts_with(const String& prefix) const noexcept {
        auto v = view(), pv = prefix.view();
        return v.size() >= pv.size() && v.substr(0, pv.size()) == pv;
    }

    bool ends_with(const String& suffix) const noexcept {
        auto v = view(), sv = suffix.view();
        return v.size() >= sv.size() && v.substr(v.size() - sv.size()) == sv;
    }

    int32_t index_of(const String& sub) const {
        auto v = view(), sv = sub.view();
        size_t pos = v.find(sv);
        if (pos == std::string_view::npos) return -1;
        return static_cast<int32_t>(utf8_length(v.substr(0, pos)));
    }

    int32_t last_index_of(const String& sub) const {
        auto v = view(), sv = sub.view();
        size_t pos = v.rfind(sv);
        if (pos == std::string_view::npos) return -1;
        return static_cast<int32_t>(utf8_length(v.substr(0, pos)));
    }

    String replace(const String& old_str, const String& new_str) const {
        std::string result(view());
        auto ov = old_str.view(), nv = new_str.view();
        size_t pos = 0;
        while ((pos = result.find(ov, pos)) != std::string::npos) {
            result.replace(pos, ov.size(), nv);
            pos += nv.size();
        }
        return String(std::move(result));
    }

    String repeat(int32_t n) const {
        if (n <= 0) return String("");
        auto v = view();
        std::string result;
        result.reserve(v.size() * static_cast<size_t>(n));
        for (int32_t i = 0; i < n; ++i) result.append(v);
        return String(std::move(result));
    }

    String reverse() const {
        auto v = view();
        std::string result;
        result.reserve(v.size());
        size_t len = utf8_length(v);
        for (size_t i = len; i > 0; --i) result += utf8_char_at(v, i - 1);
        return String(std::move(result));
    }

    bool is_blank() const noexcept {
        for (unsigned char c : view())
            if (!std::isspace(c)) return false;
        return true;
    }

    bool is_present() const noexcept { return !is_blank(); }

    String squish() const {
        std::string result;
        bool in_space = true;
        for (unsigned char c : view()) {
            if (std::isspace(c)) {
                if (!in_space) { result += ' '; in_space = true; }
            } else {
                result += static_cast<char>(c);
                in_space = false;
            }
        }
        if (!result.empty() && result.back() == ' ') result.pop_back();
        return String(std::move(result));
    }

    String truncate(int32_t max_len) const {
        if (max_len <= 0) return String("");
        size_t char_len = utf8_length(view());
        if (char_len <= static_cast<size_t>(max_len)) return *this;
        if (max_len <= 3) return String("...");
        return substring(0, max_len - 3) + String("...");
    }

    String titleize() const {
        std::string result;
        bool cap_next = true;
        for (unsigned char c : view()) {
            if (std::isspace(c) || c == '_' || c == '-') {
                result += ' '; cap_next = true;
            } else if (cap_next) {
                result += static_cast<char>(std::toupper(c)); cap_next = false;
            } else {
                result += static_cast<char>(std::tolower(c));
            }
        }
        return String(std::move(result));
    }

    String camelize() const {
        std::string result;
        bool cap_next = false;
        for (unsigned char c : view()) {
            if (c == '_' || c == '-' || c == ' ') {
                cap_next = true;
            } else if (cap_next) {
                result += static_cast<char>(std::toupper(c)); cap_next = false;
            } else {
                result += static_cast<char>(std::tolower(c));
            }
        }
        return String(std::move(result));
    }

    String underscore() const {
        auto v = view();
        std::string result;
        for (size_t i = 0; i < v.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(v[i]);
            if (std::isupper(c)) {
                if (i > 0) result += '_';
                result += static_cast<char>(std::tolower(c));
            } else if (c == '-' || c == ' ') {
                result += '_';
            } else {
                result += static_cast<char>(c);
            }
        }
        return String(std::move(result));
    }

    String pad_start(int32_t len, const String& pad_char) const {
        size_t char_len = utf8_length(view());
        if (char_len >= static_cast<size_t>(len)) return *this;
        auto pv = pad_char.view();
        std::string pc = pv.empty() ? " " : std::string(pv.substr(0, 1));
        std::string result;
        for (size_t i = 0, n = static_cast<size_t>(len) - char_len; i < n; ++i) result += pc;
        result.append(view());
        return String(std::move(result));
    }

    String pad_end(int32_t len, const String& pad_char) const {
        size_t char_len = utf8_length(view());
        if (char_len >= static_cast<size_t>(len)) return *this;
        auto pv = pad_char.view();
        std::string pc = pv.empty() ? " " : std::string(pv.substr(0, 1));
        std::string result(view());
        for (size_t i = 0, n = static_cast<size_t>(len) - char_len; i < n; ++i) result += pc;
        return String(std::move(result));
    }

    // ── concatenation ─────────────────────────────────────────────────────────

    String operator+(const String& other) const {
        auto av = view(), bv = other.view();
        bool ascii = is_ascii_ && other.is_ascii_;
        size_t total = av.size() + bv.size();
        if (total <= SSO_CAPACITY) {
            String result;
            std::memcpy(result.sso_buf_, av.data(), av.size());
            std::memcpy(result.sso_buf_ + av.size(), bv.data(), bv.size());
            result.sso_buf_[total] = '\0';
            result.sso_len_ = static_cast<uint8_t>(total);
            result.is_ascii_ = ascii;
            return result;
        }
        std::string s;
        s.reserve(total);
        s.append(av);
        s.append(bv);
        return String(std::move(s), ascii);
    }

    String& operator+=(const String& other) {
        *this = *this + other;
        return *this;
    }

    // ── comparison ────────────────────────────────────────────────────────────

    bool operator==(const String& o) const noexcept { return view() == o.view(); }
    bool operator!=(const String& o) const noexcept { return view() != o.view(); }
    bool operator< (const String& o) const noexcept { return view() <  o.view(); }
    bool operator> (const String& o) const noexcept { return view() >  o.view(); }
    bool operator<=(const String& o) const noexcept { return view() <= o.view(); }
    bool operator>=(const String& o) const noexcept { return view() >= o.view(); }

    // ── conversion ────────────────────────────────────────────────────────────

    Bytes to_bytes() const;
    static String from_bytes(const Bytes& bytes);
};

// ── Bytes ─────────────────────────────────────────────────────────────────────

class Bytes {
    std::vector<uint8_t> data_;
public:
    Bytes() : data_() {}
    Bytes(const std::vector<uint8_t>& bytes) : data_(bytes) {}
    Bytes(std::vector<uint8_t>&& bytes) : data_(std::move(bytes)) {}
    Bytes(const uint8_t* ptr, size_t size) : data_(ptr, ptr + size) {}

    template<typename Iterator>
    Bytes(Iterator begin, Iterator end) : data_(begin, end) {}

    Bytes(const Bytes&) = default;
    Bytes(Bytes&&) = default;
    Bytes& operator=(const Bytes&) = default;
    Bytes& operator=(Bytes&&) = default;

    size_t size()     const { return data_.size(); }
    bool   is_empty() const { return data_.empty(); }

    uint8_t operator[](size_t index) const {
        if (index >= data_.size()) throw std::out_of_range("Bytes index out of range");
        return data_[index];
    }
    uint8_t& operator[](size_t index) {
        if (index >= data_.size()) throw std::out_of_range("Bytes index out of range");
        return data_[index];
    }

    Bytes slice(size_t start) const {
        if (start > data_.size()) throw std::out_of_range("Bytes slice start out of range");
        return Bytes(data_.begin() + start, data_.end());
    }
    Bytes slice(size_t start, size_t length) const {
        if (start + length > data_.size()) throw std::out_of_range("Bytes slice out of range");
        return Bytes(data_.begin() + start, data_.begin() + start + length);
    }

    const uint8_t* as_ptr()     const { return data_.data(); }
    uint8_t*       as_mut_ptr()       { return data_.data(); }

    bool operator==(const Bytes& o) const { return data_ == o.data_; }
    bool operator!=(const Bytes& o) const { return data_ != o.data_; }

    String to_string() const {
        return String(std::string(data_.begin(), data_.end()));
    }

    static Bytes from_string(const String& str) {
        auto v = str.view();
        return Bytes(reinterpret_cast<const uint8_t*>(v.data()), v.size());
    }
};

inline Bytes String::to_bytes() const { return Bytes::from_string(*this); }
inline String String::from_bytes(const Bytes& bytes) { return bytes.to_string(); }

// ── free functions ────────────────────────────────────────────────────────────

inline String to_string(int value)                { return String(std::to_string(value)); }
inline String to_string(long value)               { return String(std::to_string(value)); }
inline String to_string(long long value)          { return String(std::to_string(value)); }
inline String to_string(unsigned value)           { return String(std::to_string(value)); }
inline String to_string(unsigned long value)      { return String(std::to_string(value)); }
inline String to_string(unsigned long long value) { return String(std::to_string(value)); }

inline String to_string(float value) {
    std::ostringstream oss; oss << value; return String(oss.str());
}
inline String to_string(double value) {
    std::ostringstream oss; oss << value; return String(oss.str());
}
inline String to_string(long double value) {
    std::ostringstream oss; oss << value; return String(oss.str());
}
inline String to_string(bool value)              { return String(value ? "true" : "false"); }
inline String to_string(const String& value)     { return value; }
inline String to_string(const char* value)       { return String(value); }
inline String to_string(const std::string& value){ return String(value); }

inline int32_t parse_i32(const String& str) {
    try { return std::stoi(std::string(str.view())); } catch (...) { return 0; }
}

inline float parse_f32(const String& str) {
    try { return std::stof(std::string(str.view())); } catch (...) { return 0.0f; }
}

inline bool parse_bool(const String& str) {
    auto s = str.trim().lower();
    auto v = s.view();
    return v == "true" || v == "1" || v == "yes";
}

inline String to_string_i32(int32_t value) { return to_string(value); }
inline String to_string_f32(float value)   { return to_string(value); }
inline String to_string_bool(bool value)   { return to_string(value); }

inline String format(const String& fmt, const std::vector<String>& parts) {
    auto pattern = fmt.view();
    std::string result;
    result.reserve(pattern.size() + parts.size() * 8);
    size_t arg_index = 0;
    for (size_t i = 0; i < pattern.size(); ++i) {
        char ch = pattern[i];
        if (ch == '{') {
            if (i + 1 < pattern.size() && pattern[i + 1] == '{') {
                result.push_back('{'); ++i;
            } else {
                if (arg_index < parts.size()) {
                    result.append(parts[arg_index++].view());
                    if (i + 1 < pattern.size() && pattern[i + 1] == '}') ++i;
                }
            }
        } else if (ch == '}' && i + 1 < pattern.size() && pattern[i + 1] == '}') {
            result.push_back('}'); ++i;
        } else {
            result.push_back(ch);
        }
    }
    return String(std::move(result));
}

template <typename... Args>
inline String format(const String& fmt, Args&&... args) {
    std::vector<String> parts;
    parts.reserve(sizeof...(Args));
    (parts.push_back(to_string(std::forward<Args>(args))), ...);
    return format(fmt, parts);
}

} // namespace mlc

#endif // MLC_STRING_HPP
