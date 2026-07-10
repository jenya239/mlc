#include "mlc/text/freetype_shim.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <vector>

namespace mlc {
namespace text {
namespace {

struct GlyphCache {
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t rows = 0;
};

GlyphCache& glyph_cache() {
  static thread_local GlyphCache cache;
  return cache;
}

int32_t render_face_glyph(FT_Face face) {
  if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
    return -7;
  }
  const int32_t width = static_cast<int32_t>(face->glyph->bitmap.width);
  const int32_t rows = static_cast<int32_t>(face->glyph->bitmap.rows);
  if (width < 0 || rows < 0 || width > 0xffff || rows > 0xffff) {
    return -8;
  }
  GlyphCache& cache = glyph_cache();
  cache.width = width;
  cache.rows = rows;
  cache.pixels.assign(static_cast<size_t>(width * rows), 0);
  const int pitch = face->glyph->bitmap.pitch;
  for (int32_t row = 0; row < rows; row += 1) {
    for (int32_t column = 0; column < width; column += 1) {
      cache.pixels[static_cast<size_t>(row * width + column)] =
        face->glyph->bitmap.buffer[static_cast<size_t>(row * pitch + column)];
    }
  }
  return (width << 16) | rows;
}

int32_t with_face(
  const char* font_path,
  int32_t pixel_size,
  FT_UInt glyph_index
) {
  if (pixel_size <= 0) {
    return -1;
  }
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return -2;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path, 0, &face) != 0) {
    FT_Done_FreeType(library);
    return -3;
  }
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -4;
  }
  if (glyph_index == 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -5;
  }
  if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -6;
  }
  const int32_t packed = render_face_glyph(face);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return packed;
}

} // namespace

int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size) {
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return -2;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path.c_str(), 0, &face) != 0) {
    FT_Done_FreeType(library);
    return -3;
  }
  const FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(codepoint));
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return with_face(font_path.c_str(), pixel_size, glyph_index);
}

int32_t glyph_bitmap_by_index(mlc::String font_path, int32_t glyph_index, int32_t pixel_size) {
  if (glyph_index <= 0) {
    return -5;
  }
  return with_face(font_path.c_str(), pixel_size, static_cast<FT_UInt>(glyph_index));
}

int32_t glyph_width() { return glyph_cache().width; }
int32_t glyph_rows() { return glyph_cache().rows; }

int32_t glyph_byte_count() {
  return static_cast<int32_t>(glyph_cache().pixels.size());
}

int32_t glyph_byte_at(int32_t index) {
  const GlyphCache& cache = glyph_cache();
  if (index < 0 || static_cast<size_t>(index) >= cache.pixels.size()) {
    return -1;
  }
  return cache.pixels[static_cast<size_t>(index)];
}

const uint8_t* glyph_a8_data() {
  const GlyphCache& cache = glyph_cache();
  if (cache.pixels.empty()) {
    return nullptr;
  }
  return cache.pixels.data();
}

} // namespace text
} // namespace mlc
