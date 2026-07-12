#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc {
namespace text {

// DEPRECATED (TRACK_TEXT_SHIM_TO_MLC STEP=8): path+size face cache removed.
// Prefer freetype_abi.hpp + misc/gui/text_shaping.mlc. Kept as thin wrappers
// for smokes / golden gate (text_shaping_vs_shim_gate).

int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size);
int32_t glyph_bitmap_by_index(mlc::String font_path, int32_t glyph_index, int32_t pixel_size);

int32_t glyph_width();
int32_t glyph_rows();
int32_t glyph_bearing_x();
int32_t glyph_bearing_y();
int32_t glyph_byte_count();
int32_t glyph_byte_at(int32_t index);
const uint8_t* glyph_a8_data();

} // namespace text
} // namespace mlc
