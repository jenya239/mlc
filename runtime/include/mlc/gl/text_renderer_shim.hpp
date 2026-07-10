#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::gl {

// FreeType A8 glyph → GLES2 textured quad → offscreen FBO → readback.
// Pixel-diff vs CPU blit of the same A8 (MAE tolerance). Returns 0 on success.
int32_t text_renderer_a8_glyph_smoke(
  mlc::String font_path,
  int32_t codepoint,
  int32_t pixel_size
);

} // namespace mlc::gl
