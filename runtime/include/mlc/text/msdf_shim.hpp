#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::text {

// Generate RGB8 distance field for one glyph (MSDF-compatible layout).
// field_size = output width/height; px_range = distance mapped to 0..255.
// Returns 0 on success, negative on error. Results in thread-local cache.
int32_t msdf_generate(
  mlc::String font_path,
  int32_t codepoint,
  int32_t field_size,
  int32_t px_range
);

int32_t msdf_width();
int32_t msdf_height();
int32_t msdf_px_range();
// Interleaved RGB bytes; index in [0, width*height*3).
int32_t msdf_byte_at(int32_t index);
int32_t msdf_byte_count();

// Pointer to interleaved RGB cache (valid until next msdf_generate). C++ shims only.
const uint8_t* msdf_rgb_data();

} // namespace mlc::text
