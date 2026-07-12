#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc {
namespace text {

// Load font at path, render one Unicode codepoint at pixel_size (A8).
// On success: returns (width << 16) | rows; pixels cached for glyph_byte_*.
// On failure: returns a negative error code.
//
// FreeType faces are cached process-locally by (font_path, pixel_size).
// Single-threaded render loop only — not safe for concurrent callers.
int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size);

// Same as glyph_bitmap_packed but FreeType glyph index (HarfBuzz shape id).
int32_t glyph_bitmap_by_index(mlc::String font_path, int32_t glyph_index, int32_t pixel_size);

int32_t glyph_width();
int32_t glyph_rows();
int32_t glyph_byte_count();
int32_t glyph_byte_at(int32_t index);
const uint8_t* glyph_a8_data();

} // namespace text
} // namespace mlc
