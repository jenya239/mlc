#include "mlc/text/harfbuzz_shim.hpp"

#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>

namespace mlc::text {
namespace {

struct ShapedGlyphCacheEntry {
  int32_t glyph_id = 0;
  int32_t x_advance = 0;
};

std::vector<ShapedGlyphCacheEntry> g_shape_cache;

} // namespace

int32_t shape_glyph_count(mlc::String font_path, mlc::String text, int32_t pixel_size) {
  g_shape_cache.clear();
  if (pixel_size <= 0) {
    return -1;
  }
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return -2;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path.c_str(), 0, &face) != 0) {
    FT_Done_FreeType(library);
    return -3;
  }
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -4;
  }
  hb_font_t* font = hb_ft_font_create(face, nullptr);
  if (font == nullptr) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -5;
  }
  hb_buffer_t* buffer = hb_buffer_create();
  if (buffer == nullptr) {
    hb_font_destroy(font);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -6;
  }
  const char* utf8 = text.c_str();
  const int byte_length = static_cast<int>(text.view().size());
  hb_buffer_add_utf8(buffer, utf8, byte_length, 0, byte_length);
  hb_buffer_guess_segment_properties(buffer);
  hb_shape(font, buffer, nullptr, 0);
  unsigned int glyph_count = 0;
  hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buffer, &glyph_count);
  hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buffer, &glyph_count);
  g_shape_cache.reserve(glyph_count);
  for (unsigned int index = 0; index < glyph_count; ++index) {
    ShapedGlyphCacheEntry entry;
    entry.glyph_id = static_cast<int32_t>(infos[index].codepoint);
    entry.x_advance = positions[index].x_advance;
    g_shape_cache.push_back(entry);
  }
  hb_buffer_destroy(buffer);
  hb_font_destroy(font);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return static_cast<int32_t>(g_shape_cache.size());
}

int32_t shape_glyph_id_at(int32_t index) {
  if (index < 0 || index >= static_cast<int32_t>(g_shape_cache.size())) {
    return -1;
  }
  return g_shape_cache[static_cast<size_t>(index)].glyph_id;
}

int32_t shape_glyph_advance_at(int32_t index) {
  if (index < 0 || index >= static_cast<int32_t>(g_shape_cache.size())) {
    return -1;
  }
  return g_shape_cache[static_cast<size_t>(index)].x_advance;
}

} // namespace mlc::text
