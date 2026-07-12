#pragma once

// Thin HarfBuzz ABI for TRACK_TEXT_SHIM_TO_MLC STEP=3.
// Handles are i64 (postgres_abi pattern). No path/size cache — MLC STEP=4.
// Shape result accessors use hb_shape_glyph_* names to avoid clashing with
// legacy harfbuzz_shim.hpp shape_glyph_* until STEP=8.

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace text {

// hb_ft_font_create from FT_Face handle. 0 = error.
std::int64_t hb_font_create_from_face(std::int64_t face);

// hb_font_destroy. 0 = ok, negative = error.
std::int32_t hb_font_destroy(std::int64_t font);

// Shape UTF-8 into last-shape slot. Returns glyph count, or negative error.
std::int32_t hb_shape_text(std::int64_t font, String text);

std::int32_t hb_shape_glyph_id_at(std::int32_t index);
std::int32_t hb_shape_glyph_advance_at(std::int32_t index);

} // namespace text
} // namespace mlc
