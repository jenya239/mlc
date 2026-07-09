#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::text {

// Load font at path, render one Unicode codepoint at pixel_size (A8).
// On success: returns (width << 16) | rows (both in low 16 bits each).
// On failure: returns a negative error code.
int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size);

} // namespace mlc::text
