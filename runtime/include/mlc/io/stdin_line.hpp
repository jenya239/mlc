#pragma once

// Invariants:
// - read_standard_input_line returns 1 for a line, including an empty line, and 0 at end of file
// - take_standard_input_line returns the text stored by the last successful read
// - standard_input_is_terminal is 1 only when standard input is a terminal

#include "mlc/core/string.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>

namespace mlc {
namespace io {

inline String& stored_standard_input_line() {
  static String stored("");
  return stored;
}

inline int32_t standard_input_is_terminal() {
  return ::isatty(::fileno(stdin)) == 1 ? 1 : 0;
}

inline int32_t read_standard_input_line() {
  std::string line;
  if (!std::getline(std::cin, line)) {
    stored_standard_input_line() = String("");
    return 0;
  }
  stored_standard_input_line() = String(line);
  return 1;
}

inline String take_standard_input_line() {
  String taken = stored_standard_input_line();
  stored_standard_input_line() = String("");
  return taken;
}

inline void write_standard_error_text(String text) {
  if (!text.is_empty()) {
    std::fwrite(text.raw_data(), 1, text.raw_size(), stderr);
  }
  std::fflush(stderr);
}

} // namespace io
} // namespace mlc
