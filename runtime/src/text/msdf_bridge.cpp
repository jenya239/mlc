#include "mlc/text/msdf_bridge.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <cstdint>
#include <vector>

namespace mlc::text {
namespace {

struct MsdfMaskCache {
  int32_t field_size = 0;
  std::vector<uint8_t> bits;
};

struct MsdfRgbCache {
  int32_t width = 0;
  int32_t height = 0;
  int32_t px_range = 0;
  std::vector<uint8_t> rgb;
};

MsdfMaskCache g_mask_cache;
MsdfRgbCache g_rgb_cache;

} // namespace

int32_t msdf_mask_render(
  mlc::String font_path,
  int32_t codepoint,
  int32_t field_size
) {
  g_mask_cache.field_size = 0;
  g_mask_cache.bits.clear();
  if (field_size < 8 || field_size > 256) {
    return -1;
  }
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return -2;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path.c_str(), 0, &face) != 0) {
    FT_Done_FreeType(library);
    return -2;
  }
  const int32_t pixel_size = std::max(8, field_size - 8);
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -2;
  }
  const FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(codepoint));
  if (glyph_index == 0
      || FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0
      || FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -2;
  }
  const int32_t glyph_width = static_cast<int32_t>(face->glyph->bitmap.width);
  const int32_t glyph_rows = static_cast<int32_t>(face->glyph->bitmap.rows);
  const int pitch = face->glyph->bitmap.pitch;
  g_mask_cache.bits.assign(static_cast<size_t>(field_size * field_size), 0);
  const int32_t origin_x = (field_size - glyph_width) / 2;
  const int32_t origin_y = (field_size - glyph_rows) / 2;
  for (int32_t row = 0; row < glyph_rows; row += 1) {
    for (int32_t column = 0; column < glyph_width; column += 1) {
      const uint8_t coverage =
        face->glyph->bitmap.buffer[static_cast<size_t>(row * pitch + column)];
      if (coverage < 128) {
        continue;
      }
      const int32_t x = origin_x + column;
      const int32_t y = origin_y + row;
      if (x < 0 || y < 0 || x >= field_size || y >= field_size) {
        continue;
      }
      g_mask_cache.bits[static_cast<size_t>(y * field_size + x)] = 1;
    }
  }
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  g_mask_cache.field_size = field_size;
  return 0;
}

int32_t msdf_mask_size() {
  return static_cast<int32_t>(g_mask_cache.bits.size());
}

int32_t msdf_mask_byte_at(int32_t index) {
  if (index < 0 || index >= static_cast<int32_t>(g_mask_cache.bits.size())) {
    return -1;
  }
  return static_cast<int32_t>(g_mask_cache.bits[static_cast<size_t>(index)]);
}

void msdf_cache_begin(int32_t width, int32_t height, int32_t px_range) {
  g_rgb_cache.width = width;
  g_rgb_cache.height = height;
  g_rgb_cache.px_range = px_range;
  g_rgb_cache.rgb.clear();
  if (width > 0 && height > 0) {
    g_rgb_cache.rgb.reserve(static_cast<size_t>(width * height * 3));
  }
}

void msdf_cache_push_byte(int32_t value) {
  if (value < 0) {
    value = 0;
  }
  if (value > 255) {
    value = 255;
  }
  g_rgb_cache.rgb.push_back(static_cast<uint8_t>(value));
}

void msdf_cache_finish() {
  // no-op; bytes already in g_rgb_cache.rgb
}

int32_t msdf_width() {
  return g_rgb_cache.width;
}

int32_t msdf_height() {
  return g_rgb_cache.height;
}

int32_t msdf_px_range() {
  return g_rgb_cache.px_range;
}

int32_t msdf_byte_count() {
  return static_cast<int32_t>(g_rgb_cache.rgb.size());
}

int32_t msdf_byte_at(int32_t index) {
  if (index < 0 || index >= static_cast<int32_t>(g_rgb_cache.rgb.size())) {
    return -1;
  }
  return static_cast<int32_t>(g_rgb_cache.rgb[static_cast<size_t>(index)]);
}

const uint8_t* msdf_rgb_data() {
  if (g_rgb_cache.rgb.empty()) {
    return nullptr;
  }
  return g_rgb_cache.rgb.data();
}

} // namespace mlc::text
