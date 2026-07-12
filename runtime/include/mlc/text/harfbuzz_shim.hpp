#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::text {

// Shape UTF-8 text with HarfBuzz + FreeType. Fills an internal cache;
// use shape_glyph_id_at / shape_glyph_advance_at to read results.
// Returns glyph count on success, or a negative error code.
//
// FT_Face + hb_font_t are cached process-locally by (font_path, pixel_size)
// and kept until process exit. The cache is process-global mutable state with
// no mutex: safe only on the single-threaded main/GL render loop used by
// current demos. Off-thread shaping would need a mutex around the shaping-font
// cache before concurrent calls are allowed.
int32_t shape_glyph_count(mlc::String font_path, mlc::String text, int32_t pixel_size);

int32_t shape_glyph_id_at(int32_t index);
int32_t shape_glyph_advance_at(int32_t index);

} // namespace mlc::text
