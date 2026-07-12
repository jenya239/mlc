#pragma once

// Thin FreeType ABI for TRACK_TEXT_SHIM_TO_MLC STEP=2.
// Handles cross the MLC boundary as i64 (postgres_abi pattern).
// No face/path cache here — bookkeeping belongs in MLC (STEP=4).

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace text {

// Lazy FT_Init_FreeType singleton. 0 = failure.
std::int64_t ft_library_get();

// FT_New_Face. 0 = error.
std::int64_t ft_face_open(std::int64_t library, String path);

// FT_Set_Pixel_Sizes. 0 = ok, negative = error.
std::int32_t ft_face_set_pixel_size(std::int64_t face, std::int32_t pixel_size);

// FT_Done_Face. 0 = ok, negative = error.
std::int32_t ft_face_close(std::int64_t face);

// FT_Load_Glyph. 0 = ok, negative = error.
std::int32_t ft_face_load_glyph(std::int64_t face, std::int32_t glyph_index);

// FT_Render_Glyph + fill last-glyph slot (raw pitch buffer, no flatten).
// Success: (width << 16) | rows. Negative = error.
std::int32_t ft_face_render_glyph(std::int64_t face);

std::int32_t ft_glyph_width();
std::int32_t ft_glyph_rows();
std::int32_t ft_glyph_pitch();
std::int32_t ft_glyph_bearing_x();
std::int32_t ft_glyph_bearing_y();
std::int32_t ft_glyph_byte_count();
// Index into raw pitch buffer (pitch * rows). -1 out of range.
std::int32_t ft_glyph_byte_at(std::int32_t index);
// Flattened width*rows A8 (for GL atlas blit). Null if empty.
const std::uint8_t* ft_glyph_a8_data();

} // namespace text
} // namespace mlc
