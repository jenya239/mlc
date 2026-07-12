#include "mlc/text/freetype_shim.hpp"

#include "mlc/text/freetype_abi.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <vector>

namespace mlc {
namespace text {
namespace {

// Flat A8 copy of last glyph_bitmap_* (legacy accessors). Deprecated path —
// prefer freetype_abi + MLC text_shaping (TRACK_TEXT_SHIM_TO_MLC STEP=8).
struct GlyphPixelBuffer {
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t rows = 0;
  int32_t bearing_x = 0;
  int32_t bearing_y = 0;
};

GlyphPixelBuffer& glyph_pixel_buffer() {
  static thread_local GlyphPixelBuffer buffer;
  return buffer;
}

void sync_shim_buffer_from_abi() {
  GlyphPixelBuffer& buffer = glyph_pixel_buffer();
  buffer.width = ft_glyph_width();
  buffer.rows = ft_glyph_rows();
  buffer.bearing_x = ft_glyph_bearing_x();
  buffer.bearing_y = ft_glyph_bearing_y();
  const uint8_t* flat = ft_glyph_a8_data();
  const std::size_t count =
    static_cast<std::size_t>(buffer.width) * static_cast<std::size_t>(buffer.rows);
  if (flat == nullptr || count == 0) {
    buffer.pixels.clear();
    return;
  }
  buffer.pixels.assign(flat, flat + count);
}

int32_t render_glyph_index(String font_path, int32_t glyph_index, int32_t pixel_size) {
  if (pixel_size <= 0) {
    return -1;
  }
  if (glyph_index <= 0) {
    return -5;
  }
  const std::int64_t library = ft_library_get();
  if (library == 0) {
    return -2;
  }
  const std::int64_t face = ft_face_open(library, font_path);
  if (face == 0) {
    return -3;
  }
  if (ft_face_set_pixel_size(face, pixel_size) != 0) {
    ft_face_close(face);
    return -4;
  }
  if (ft_face_load_glyph(face, glyph_index) != 0) {
    ft_face_close(face);
    return -6;
  }
  const int32_t packed = ft_face_render_glyph(face);
  sync_shim_buffer_from_abi();
  ft_face_close(face);
  return packed;
}

} // namespace

int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size) {
  if (pixel_size <= 0) {
    return -1;
  }
  const std::int64_t library = ft_library_get();
  if (library == 0) {
    return -2;
  }
  const std::int64_t face = ft_face_open(library, font_path);
  if (face == 0) {
    return -3;
  }
  if (ft_face_set_pixel_size(face, pixel_size) != 0) {
    ft_face_close(face);
    return -4;
  }
  FT_Face free_type_face = reinterpret_cast<FT_Face>(face);
  const FT_UInt glyph_index =
    FT_Get_Char_Index(free_type_face, static_cast<FT_ULong>(codepoint));
  ft_face_close(face);
  if (glyph_index == 0) {
    return -5;
  }
  return render_glyph_index(font_path, static_cast<int32_t>(glyph_index), pixel_size);
}

int32_t glyph_bitmap_by_index(mlc::String font_path, int32_t glyph_index, int32_t pixel_size) {
  return render_glyph_index(font_path, glyph_index, pixel_size);
}

int32_t glyph_width() { return glyph_pixel_buffer().width; }
int32_t glyph_rows() { return glyph_pixel_buffer().rows; }
int32_t glyph_bearing_x() { return glyph_pixel_buffer().bearing_x; }
int32_t glyph_bearing_y() { return glyph_pixel_buffer().bearing_y; }

int32_t glyph_byte_count() {
  return static_cast<int32_t>(glyph_pixel_buffer().pixels.size());
}

int32_t glyph_byte_at(int32_t index) {
  const GlyphPixelBuffer& buffer = glyph_pixel_buffer();
  if (index < 0 || static_cast<size_t>(index) >= buffer.pixels.size()) {
    return -1;
  }
  return buffer.pixels[static_cast<size_t>(index)];
}

const uint8_t* glyph_a8_data() {
  const GlyphPixelBuffer& buffer = glyph_pixel_buffer();
  if (buffer.pixels.empty()) {
    return nullptr;
  }
  return buffer.pixels.data();
}

} // namespace text
} // namespace mlc
