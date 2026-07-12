#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc {
namespace text {

// Load font at path, render one Unicode codepoint at pixel_size (A8).
// On success: returns (width << 16) | rows; pixels cached for glyph_byte_*.
// On failure: returns a negative error code.
//
// FreeType faces are cached process-locally by (font_path, pixel_size) and
// never torn down until process exit. That cache is process-global mutable
// state with no mutex: safe only when all glyph_* / shaping calls run on the
// same thread as today's demos (main/GL render loop). If a caller ever moves
// rasterization off that thread, add a mutex around the face cache first —
// do not treat this API as thread-safe without that change.
int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size);

// Same as glyph_bitmap_packed but FreeType glyph index (HarfBuzz shape id).
int32_t glyph_bitmap_by_index(mlc::String font_path, int32_t glyph_index, int32_t pixel_size);

int32_t glyph_width();
int32_t glyph_rows();
// FreeType bearing of the last successful glyph_bitmap_* call
// (bitmap_left / bitmap_top). Same cache slot as glyph_width/glyph_rows.
int32_t glyph_bearing_x();
int32_t glyph_bearing_y();
int32_t glyph_byte_count();
int32_t glyph_byte_at(int32_t index);
const uint8_t* glyph_a8_data();

} // namespace text
} // namespace mlc
