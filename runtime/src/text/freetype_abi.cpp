#include "mlc/text/freetype_abi.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <vector>

namespace mlc {
namespace text {
namespace {

struct LastGlyphSlot {
  std::vector<std::uint8_t> pixels;
  std::int32_t width = 0;
  std::int32_t rows = 0;
  std::int32_t pitch = 0;
  std::int32_t bearing_x = 0;
  std::int32_t bearing_y = 0;
};

LastGlyphSlot& last_glyph_slot() {
  static thread_local LastGlyphSlot slot;
  return slot;
}

FT_Library i64_to_library(std::int64_t library) {
  return reinterpret_cast<FT_Library>(library);
}

FT_Face i64_to_face(std::int64_t face) {
  return reinterpret_cast<FT_Face>(face);
}

std::int64_t pointer_to_i64(void* pointer) {
  return reinterpret_cast<std::int64_t>(pointer);
}

} // namespace

std::int64_t ft_library_get() {
  static FT_Library library = nullptr;
  if (library == nullptr) {
    if (FT_Init_FreeType(&library) != 0) {
      return 0;
    }
  }
  return pointer_to_i64(library);
}

std::int64_t ft_face_open(std::int64_t library, String path) {
  FT_Library free_type_library = i64_to_library(library);
  if (free_type_library == nullptr) {
    return 0;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(free_type_library, path.c_str(), 0, &face) != 0) {
    return 0;
  }
  return pointer_to_i64(face);
}

std::int32_t ft_face_set_pixel_size(std::int64_t face, std::int32_t pixel_size) {
  FT_Face free_type_face = i64_to_face(face);
  if (free_type_face == nullptr || pixel_size <= 0) {
    return -1;
  }
  if (FT_Set_Pixel_Sizes(free_type_face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    return -2;
  }
  return 0;
}

std::int32_t ft_face_close(std::int64_t face) {
  FT_Face free_type_face = i64_to_face(face);
  if (free_type_face == nullptr) {
    return -1;
  }
  FT_Done_Face(free_type_face);
  return 0;
}

std::int32_t ft_face_load_glyph(std::int64_t face, std::int32_t glyph_index) {
  FT_Face free_type_face = i64_to_face(face);
  if (free_type_face == nullptr || glyph_index <= 0) {
    return -1;
  }
  if (FT_Load_Glyph(free_type_face, static_cast<FT_UInt>(glyph_index), FT_LOAD_DEFAULT) != 0) {
    return -2;
  }
  return 0;
}

std::int32_t ft_face_render_glyph(std::int64_t face) {
  FT_Face free_type_face = i64_to_face(face);
  if (free_type_face == nullptr || free_type_face->glyph == nullptr) {
    return -1;
  }
  if (FT_Render_Glyph(free_type_face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
    return -2;
  }
  const FT_Bitmap& bitmap = free_type_face->glyph->bitmap;
  const std::int32_t width = static_cast<std::int32_t>(bitmap.width);
  const std::int32_t rows = static_cast<std::int32_t>(bitmap.rows);
  const std::int32_t pitch = bitmap.pitch;
  if (width < 0 || rows < 0 || width > 0xffff || rows > 0xffff) {
    return -3;
  }
  if (pitch <= 0 && width > 0 && rows > 0) {
    return -4;
  }
  LastGlyphSlot& slot = last_glyph_slot();
  slot.width = width;
  slot.rows = rows;
  slot.pitch = pitch;
  slot.bearing_x = free_type_face->glyph->bitmap_left;
  slot.bearing_y = free_type_face->glyph->bitmap_top;
  if (width == 0 || rows == 0 || bitmap.buffer == nullptr) {
    slot.pixels.clear();
    return (width << 16) | rows;
  }
  const std::size_t byte_count = static_cast<std::size_t>(pitch) * static_cast<std::size_t>(rows);
  slot.pixels.assign(bitmap.buffer, bitmap.buffer + byte_count);
  return (width << 16) | rows;
}

std::int32_t ft_glyph_width() { return last_glyph_slot().width; }
std::int32_t ft_glyph_rows() { return last_glyph_slot().rows; }
std::int32_t ft_glyph_pitch() { return last_glyph_slot().pitch; }
std::int32_t ft_glyph_bearing_x() { return last_glyph_slot().bearing_x; }
std::int32_t ft_glyph_bearing_y() { return last_glyph_slot().bearing_y; }

std::int32_t ft_glyph_byte_count() {
  return static_cast<std::int32_t>(last_glyph_slot().pixels.size());
}

std::int32_t ft_glyph_byte_at(std::int32_t index) {
  const LastGlyphSlot& slot = last_glyph_slot();
  if (index < 0 || static_cast<std::size_t>(index) >= slot.pixels.size()) {
    return -1;
  }
  return slot.pixels[static_cast<std::size_t>(index)];
}

} // namespace text
} // namespace mlc
