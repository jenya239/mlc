#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace mlc {

// ── UTF-8 helpers ─────────────────────────────────────────────────────────────

size_t String::utf8_length(std::string_view str) noexcept {
    size_t count = 0;
    for (size_t i = 0; i < str.size(); ) {
        uint8_t byte = static_cast<uint8_t>(str[i]);
        if      ((byte & 0x80) == 0x00) i += 1;
        else if ((byte & 0xE0) == 0xC0) i += 2;
        else if ((byte & 0xF0) == 0xE0) i += 3;
        else if ((byte & 0xF8) == 0xF0) i += 4;
        else                            i += 1;  // invalid byte
        ++count;
    }
    return count;
}

size_t String::utf8_char_index(std::string_view str, size_t char_pos) {
    size_t cur = 0, bi = 0;
    while (bi < str.size() && cur < char_pos) {
        uint8_t byte = static_cast<uint8_t>(str[bi]);
        if      ((byte & 0x80) == 0x00) bi += 1;
        else if ((byte & 0xE0) == 0xC0) bi += 2;
        else if ((byte & 0xF0) == 0xE0) bi += 3;
        else if ((byte & 0xF8) == 0xF0) bi += 4;
        else                            bi += 1;
        ++cur;
    }
    return bi;
}

std::string String::utf8_char_at(std::string_view str, size_t char_pos) {
    size_t bi = utf8_char_index(str, char_pos);
    if (bi >= str.size()) throw std::out_of_range("String character index out of range");

    uint8_t byte = static_cast<uint8_t>(str[bi]);
    size_t char_bytes = 1;
    if      ((byte & 0x80) == 0x00) char_bytes = 1;
    else if ((byte & 0xE0) == 0xC0) char_bytes = 2;
    else if ((byte & 0xF0) == 0xE0) char_bytes = 3;
    else if ((byte & 0xF8) == 0xF0) char_bytes = 4;

    return std::string(str.substr(bi, char_bytes));
}

// ── substring ─────────────────────────────────────────────────────────────────

String String::substring(size_t start) const {
    auto v = view();
    if (is_ascii_) {
        if (start >= v.size()) return String();
        return String(v.data() + start, v.size() - start, true);
    }
    size_t bi = utf8_char_index(v, start);
    return String(v.data() + bi, v.size() - bi, false);
}

String String::substring(size_t start, size_t length) const {
    auto v = view();
    if (is_ascii_) {
        if (start >= v.size()) return String();
        size_t len = std::min(length, v.size() - start);
        return String(v.data() + start, len, true);
    }
    size_t byte_start = utf8_char_index(v, start);
    size_t byte_end   = utf8_char_index(v, start + length);
    return String(v.data() + byte_start, byte_end - byte_start, false);
}

// ── case ──────────────────────────────────────────────────────────────────────

String String::upper() const {
    auto v = view();
    std::string result(v);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return String(std::move(result));
}

String String::lower() const {
    auto v = view();
    std::string result(v);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return String(std::move(result));
}

// ── trim ──────────────────────────────────────────────────────────────────────

String String::trim() const {
    auto v = view();
    size_t s = 0, e = v.size();
    while (s < e && std::isspace(static_cast<unsigned char>(v[s]))) ++s;
    while (e > s && std::isspace(static_cast<unsigned char>(v[e-1]))) --e;
    return String(v.data() + s, e - s);
}

String String::trim_start() const {
    auto v = view();
    size_t s = 0;
    while (s < v.size() && std::isspace(static_cast<unsigned char>(v[s]))) ++s;
    return String(v.data() + s, v.size() - s);
}

String String::trim_end() const {
    auto v = view();
    size_t e = v.size();
    while (e > 0 && std::isspace(static_cast<unsigned char>(v[e-1]))) --e;
    return String(v.data(), e);
}

// ── split ─────────────────────────────────────────────────────────────────────

mlc::Array<String> String::split(const String& delimiter) const {
    mlc::Array<String> result;
    auto v  = view();
    auto dv = delimiter.view();

    if (dv.empty()) {
        size_t char_count = static_cast<size_t>(length());
        for (size_t i = 0; i < char_count; ++i)
            result.push_back(char_at(i));
        return result;
    }

    size_t start = 0;
    size_t pos   = v.find(dv);
    while (pos != std::string_view::npos) {
        result.push_back(String(v.data() + start, pos - start));
        start = pos + dv.size();
        pos   = v.find(dv, start);
    }
    result.push_back(String(v.data() + start, v.size() - start));
    return result;
}

mlc::Array<String> String::chars() const {
    mlc::Array<String> result;
    size_t char_count = static_cast<size_t>(length());
    for (size_t i = 0; i < char_count; ++i)
        result.push_back(char_at(i));
    return result;
}

mlc::Array<String> String::lines() const {
    return split(String("\n"));
}

} // namespace mlc
