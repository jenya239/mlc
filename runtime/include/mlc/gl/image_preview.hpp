#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc {
namespace image_preview {

int32_t load_value(mlc::String path);
int32_t present_value(
  int32_t pointer_x,
  int32_t pointer_y,
  int32_t window_width,
  int32_t window_height,
  int32_t clean_window,
  int32_t effect_mode,
  int32_t scene_framebuffer
);
void discard();
bool write_sample_png_value(mlc::String path, int32_t seed);

}  // namespace image_preview
}  // namespace mlc
