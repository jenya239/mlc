#pragma once

#include <cstdint>

namespace mlc {
namespace gl {

// Blit FreeType A8 cache into luminance scratch at atlas slot (STEP=6).
// Requires prior glyph_bitmap_* call. Returns 0 on success.
int32_t gl_scratch_u8_blit_glyph_a8(int32_t atlas_width, int32_t dest_x, int32_t dest_y);

// RGB scratch for MSDF atlas pages.
int32_t gl_scratch_rgb_resize_zero(int32_t width, int32_t height);
int32_t gl_scratch_rgb_blit_msdf(int32_t atlas_width, int32_t dest_x, int32_t dest_y);
int32_t gl_tex_image_2d_scratch_rgb(int32_t width, int32_t height);

// Screen-space quad → NDC into f32 scratch (pixel origin top-left).
void gl_scratch_push_screen_quad(
  int32_t window_width,
  int32_t window_height,
  int32_t pixel_x,
  int32_t pixel_y,
  int32_t pixel_width,
  int32_t pixel_height,
  int32_t atlas_size,
  int32_t slot_x,
  int32_t slot_y,
  int32_t slot_width,
  int32_t slot_height
);

} // namespace gl
} // namespace mlc
