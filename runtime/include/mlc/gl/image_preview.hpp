#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc {
namespace image_preview {

int32_t load_value(mlc::String path);
void arm_value(
  int32_t dest_x,
  int32_t dest_y,
  int32_t dest_width,
  int32_t dest_height,
  int32_t effect_mode,
  int32_t scene_framebuffer
);
void draw_background_value(int32_t window_width, int32_t window_height, int32_t full_paint);
int32_t needs_paint_value();
int32_t background_value();
int32_t picture_width_value();
int32_t picture_height_value();
int32_t picture_fit_width_value();
int32_t picture_fit_height_value();
void discard();
int32_t playing_value();
bool write_sample_png_value(mlc::String path, int32_t seed);

}  // namespace image_preview
}  // namespace mlc
