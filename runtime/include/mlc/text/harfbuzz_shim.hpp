#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::text {

// DEPRECATED (TRACK_TEXT_SHIM_TO_MLC STEP=8): path+size hb_font cache removed.
// Prefer harfbuzz_abi.hpp + misc/gui/text_shaping.mlc. Kept as thin wrappers
// for smokes / golden gate (text_shaping_vs_shim_gate).

int32_t shape_glyph_count(mlc::String font_path, mlc::String text, int32_t pixel_size);
int32_t shape_glyph_id_at(int32_t index);
int32_t shape_glyph_advance_at(int32_t index);

} // namespace mlc::text
