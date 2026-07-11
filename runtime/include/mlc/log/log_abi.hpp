#pragma once

// Thin stderr line writer (TRACK_STDLIB_LOGIC_TO_MLC STEP=3).
// JSON escape/format stays in MLC; this is fwrite-only (Decision: eprintln/fwrite OK).

#include "mlc/core/string.hpp"

#include <cstdio>

namespace mlc {
namespace log {

inline void write_stderr_line(String line) {
  if (!line.is_empty()) {
    std::fwrite(line.raw_data(), 1, line.raw_size(), stderr);
  }
  std::fputc('\n', stderr);
  std::fflush(stderr);
}

} // namespace log
} // namespace mlc
