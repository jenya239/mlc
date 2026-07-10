#pragma once

// Structured JSON-line logging to stderr (TRACK_STDLIB_ENV_LOGGING STEP=2).
// Format: {"level":"<level>","msg":"<escaped>"}\n

#include "mlc/core/string.hpp"

#include <cstdio>
#include <string>
#include <string_view>

namespace mlc {
namespace log {

namespace detail {

inline void append_json_escaped(std::string& output, std::string_view text) {
  for (unsigned char byte : text) {
    switch (byte) {
      case '"':
        output += "\\\"";
        break;
      case '\\':
        output += "\\\\";
        break;
      case '\b':
        output += "\\b";
        break;
      case '\f':
        output += "\\f";
        break;
      case '\n':
        output += "\\n";
        break;
      case '\r':
        output += "\\r";
        break;
      case '\t':
        output += "\\t";
        break;
      default:
        if (byte < 0x20) {
          char buffer[7];
          std::snprintf(buffer, sizeof(buffer), "\\u%04x", byte);
          output += buffer;
        } else {
          output.push_back(static_cast<char>(byte));
        }
        break;
    }
  }
}

inline void write_line(const char* level, const String& message) {
  std::string line;
  line.reserve(32 + message.raw_size() * 2);
  line += "{\"level\":\"";
  line += level;
  line += "\",\"msg\":\"";
  append_json_escaped(line, message.view());
  line += "\"}\n";
  std::fwrite(line.data(), 1, line.size(), stderr);
  std::fflush(stderr);
}

} // namespace detail

inline void error(const String& message) {
  detail::write_line("error", message);
}

inline void warn(const String& message) {
  detail::write_line("warn", message);
}

inline void info(const String& message) {
  detail::write_line("info", message);
}

inline void debug(const String& message) {
  detail::write_line("debug", message);
}

} // namespace log
} // namespace mlc
