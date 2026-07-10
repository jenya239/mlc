#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::gl {

// MSDF/SDF RGB8 → median shader → offscreen FBO. Returns 0 on success.
// Uses mlc::text::msdf_generate then GLES2 median distance decode.
int32_t msdf_renderer_fbo_smoke(
  mlc::String font_path,
  int32_t codepoint,
  int32_t field_size,
  int32_t px_range
);

} // namespace mlc::gl
