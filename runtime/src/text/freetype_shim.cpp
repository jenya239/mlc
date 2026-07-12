#include "mlc/text/freetype_shim.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <string>
#include <vector>

namespace mlc {
namespace text {
namespace {

// Raster pixel buffer for the last successful glyph_bitmap_* call.
struct GlyphPixelBuffer {
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t rows = 0;
};

GlyphPixelBuffer& glyph_pixel_buffer() {
  static thread_local GlyphPixelBuffer buffer;
  return buffer;
}

// Process-local FreeType face cache keyed by (font_path, pixel_size).
// Single-threaded render loop only — not safe for concurrent callers (STEP=2).
struct CachedFontFace {
  std::string font_path;
  int32_t pixel_size = 0;
  FT_Face face = nullptr;
};

FT_Library shared_free_type_library() {
  static FT_Library library = nullptr;
  if (library == nullptr) {
    if (FT_Init_FreeType(&library) != 0) {
      return nullptr;
    }
  }
  return library;
}

CachedFontFace* cached_font_face(const char* font_path, int32_t pixel_size) {
  if (font_path == nullptr || pixel_size <= 0) {
    return nullptr;
  }
  static std::vector<CachedFontFace> faces;
  for (CachedFontFace& entry : faces) {
    if (entry.pixel_size == pixel_size && entry.font_path == font_path) {
      return &entry;
    }
  }
  FT_Library library = shared_free_type_library();
  if (library == nullptr) {
    return nullptr;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path, 0, &face) != 0) {
    return nullptr;
  }
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    return nullptr;
  }
  CachedFontFace entry;
  entry.font_path = font_path;
  entry.pixel_size = pixel_size;
  entry.face = face;
  faces.push_back(entry);
  return &faces.back();
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
  GlyphPixelBuffer& buffer = glyph_pixel_buffer();
  buffer.width = width;
  buffer.rows = rows;
  buffer.pixels.assign(static_cast<size_t>(width * rows), 0);
  const int pitch = face->glyph->bitmap.pitch;
  for (int32_t row = 0; row < rows; row += 1) {
    for (int32_t column = 0; column < width; column += 1) {
      buffer.pixels[static_cast<size_t>(row * width + column)] =
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
  if (shared_free_type_library() == nullptr) {
    return -2;
  }
  CachedFontFace* cached = cached_font_face(font_path, pixel_size);
  if (cached == nullptr || cached->face == nullptr) {
    return -3;
  }
  if (glyph_index == 0) {
    return -5;
  }
  FT_Face face = cached->face;
  if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0) {
    return -6;
  }
  return render_face_glyph(face);
}

} // namespace

int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size) {
  if (pixel_size <= 0) {
    return -1;
  }
  if (shared_free_type_library() == nullptr) {
    return -2;
  }
  CachedFontFace* cached = cached_font_face(font_path.c_str(), pixel_size);
  if (cached == nullptr || cached->face == nullptr) {
    return -3;
  }
  const FT_UInt glyph_index =
    FT_Get_Char_Index(cached->face, static_cast<FT_ULong>(codepoint));
  return with_face(font_path.c_str(), pixel_size, glyph_index);
}

int32_t glyph_bitmap_by_index(mlc::String font_path, int32_t glyph_index, int32_t pixel_size) {
  if (glyph_index <= 0) {
    return -5;
  }
  return with_face(font_path.c_str(), pixel_size, static_cast<FT_UInt>(glyph_index));
}

int32_t glyph_width() { return glyph_pixel_buffer().width; }
int32_t glyph_rows() { return glyph_pixel_buffer().rows; }

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
