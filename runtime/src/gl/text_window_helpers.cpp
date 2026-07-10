#include "mlc/gl/text_window_helpers.hpp"

#include "mlc/gl/glfw_gl_dispatch.hpp"
#include "mlc/text/freetype_shim.hpp"
#include "mlc/text/msdf_shim.hpp"

#include <cstdint>
#include <vector>

namespace mlc {
namespace gl {
namespace {

std::vector<uint8_t>& scratch_rgb() {
  static std::vector<uint8_t> values;
  return values;
}

} // namespace

int32_t gl_scratch_u8_blit_glyph_a8(int32_t atlas_width, int32_t dest_x, int32_t dest_y) {
  const uint8_t* source = text::glyph_a8_data();
  const int32_t glyph_width = text::glyph_width();
  const int32_t glyph_rows = text::glyph_rows();
  uint8_t* destination = gl_scratch_u8_mutable_data();
  const int32_t destination_size = gl_scratch_u8_size();
  if (source == nullptr || destination == nullptr || atlas_width <= 0
      || glyph_width <= 0 || glyph_rows <= 0) {
    return -1;
  }
  for (int32_t row = 0; row < glyph_rows; row += 1) {
    for (int32_t column = 0; column < glyph_width; column += 1) {
      const int32_t dest_index = (dest_y + row) * atlas_width + (dest_x + column);
      if (dest_index < 0 || dest_index >= destination_size) {
        continue;
      }
      destination[dest_index] = source[row * glyph_width + column];
    }
  }
  return 0;
}

int32_t gl_scratch_rgb_resize_zero(int32_t width, int32_t height) {
  if (width <= 0 || height <= 0) {
    scratch_rgb().clear();
    return -1;
  }
  scratch_rgb().assign(static_cast<size_t>(width * height * 3), 0);
  return 0;
}

int32_t gl_scratch_rgb_blit_msdf(int32_t atlas_width, int32_t dest_x, int32_t dest_y) {
  const uint8_t* source = text::msdf_rgb_data();
  const int32_t field_width = text::msdf_width();
  const int32_t field_height = text::msdf_height();
  if (source == nullptr || atlas_width <= 0 || field_width <= 0 || field_height <= 0) {
    return -1;
  }
  std::vector<uint8_t>& destination = scratch_rgb();
  const int32_t atlas_height = static_cast<int32_t>(destination.size() / 3 / atlas_width);
  if (atlas_height <= 0) {
    return -2;
  }
  for (int32_t row = 0; row < field_height; row += 1) {
    for (int32_t column = 0; column < field_width; column += 1) {
      const int32_t dest_x_pixel = dest_x + column;
      const int32_t dest_y_pixel = dest_y + row;
      if (dest_x_pixel < 0 || dest_y_pixel < 0
          || dest_x_pixel >= atlas_width || dest_y_pixel >= atlas_height) {
        continue;
      }
      const size_t source_index =
        static_cast<size_t>((row * field_width + column) * 3);
      const size_t dest_index =
        static_cast<size_t>((dest_y_pixel * atlas_width + dest_x_pixel) * 3);
      destination[dest_index] = source[source_index];
      destination[dest_index + 1] = source[source_index + 1];
      destination[dest_index + 2] = source[source_index + 2];
    }
  }
  return 0;
}

int32_t gl_tex_image_2d_scratch_rgb(int32_t width, int32_t height) {
  constexpr int32_t kGlTexture2d = 0x0DE1;
  constexpr int32_t kGlRgb = 0x1907;
  constexpr int32_t kGlUnsignedByte = 0x1401;
  constexpr int32_t kGlTextureMinFilter = 0x2801;
  constexpr int32_t kGlTextureMagFilter = 0x2800;
  constexpr int32_t kGlLinear = 0x2601;
  const std::vector<uint8_t>& pixels = scratch_rgb();
  const size_t expected = static_cast<size_t>(width) * static_cast<size_t>(height) * 3;
  if (width <= 0 || height <= 0 || pixels.size() < expected) {
    return -1;
  }
  gl_tex_image_2d(
    kGlTexture2d, 0, kGlRgb, width, height, 0, kGlRgb, kGlUnsignedByte, pixels.data()
  );
  gl_tex_parameter_i(kGlTexture2d, kGlTextureMinFilter, kGlLinear);
  gl_tex_parameter_i(kGlTexture2d, kGlTextureMagFilter, kGlLinear);
  return 0;
}

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
) {
  if (window_width <= 0 || window_height <= 0) {
    return;
  }
  const double width = static_cast<double>(window_width);
  const double height = static_cast<double>(window_height);
  const double left = (static_cast<double>(pixel_x) / width) * 2.0 - 1.0;
  const double right =
    (static_cast<double>(pixel_x + pixel_width) / width) * 2.0 - 1.0;
  const double top = 1.0 - (static_cast<double>(pixel_y) / height) * 2.0;
  const double bottom =
    1.0 - (static_cast<double>(pixel_y + pixel_height) / height) * 2.0;
  gl_scratch_push_glyph_quad(
    left, bottom, right, top, atlas_size, slot_x, slot_y, slot_width, slot_height
  );
}

} // namespace gl
} // namespace mlc
