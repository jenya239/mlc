#include "mlc/text/harfbuzz_abi.hpp"

#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <vector>

namespace mlc {
namespace text {
namespace {

struct ShapedGlyphEntry {
  std::int32_t glyph_id = 0;
  std::int32_t x_advance = 0;
};

std::vector<ShapedGlyphEntry>& last_shape_slot() {
  static thread_local std::vector<ShapedGlyphEntry> slot;
  return slot;
}

FT_Face i64_to_face(std::int64_t face) {
  return reinterpret_cast<FT_Face>(face);
}

hb_font_t* i64_to_hb_font(std::int64_t font) {
  return reinterpret_cast<hb_font_t*>(font);
}

std::int64_t pointer_to_i64(void* pointer) {
  return reinterpret_cast<std::int64_t>(pointer);
}

} // namespace

std::int64_t hb_font_create_from_face(std::int64_t face) {
  FT_Face free_type_face = i64_to_face(face);
  if (free_type_face == nullptr) {
    return 0;
  }
  hb_font_t* harfbuzz_font = hb_ft_font_create(free_type_face, nullptr);
  if (harfbuzz_font == nullptr) {
    return 0;
  }
  return pointer_to_i64(harfbuzz_font);
}

std::int32_t hb_font_destroy(std::int64_t font) {
  hb_font_t* harfbuzz_font = i64_to_hb_font(font);
  if (harfbuzz_font == nullptr) {
    return -1;
  }
  ::hb_font_destroy(harfbuzz_font);
  return 0;
}

std::int32_t hb_shape_text(std::int64_t font, String text) {
  last_shape_slot().clear();
  hb_font_t* harfbuzz_font = i64_to_hb_font(font);
  if (harfbuzz_font == nullptr) {
    return -1;
  }
  hb_buffer_t* buffer = hb_buffer_create();
  if (buffer == nullptr) {
    return -2;
  }
  const char* utf8 = text.c_str();
  const int byte_length = static_cast<int>(text.view().size());
  hb_buffer_add_utf8(buffer, utf8, byte_length, 0, byte_length);
  hb_buffer_guess_segment_properties(buffer);
  hb_shape(harfbuzz_font, buffer, nullptr, 0);
  unsigned int glyph_count = 0;
  hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buffer, &glyph_count);
  hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buffer, &glyph_count);
  last_shape_slot().reserve(glyph_count);
  for (unsigned int index = 0; index < glyph_count; ++index) {
    ShapedGlyphEntry entry;
    entry.glyph_id = static_cast<std::int32_t>(infos[index].codepoint);
    entry.x_advance = positions[index].x_advance;
    last_shape_slot().push_back(entry);
  }
  hb_buffer_destroy(buffer);
  return static_cast<std::int32_t>(last_shape_slot().size());
}

std::int32_t hb_shape_glyph_id_at(std::int32_t index) {
  const std::vector<ShapedGlyphEntry>& slot = last_shape_slot();
  if (index < 0 || index >= static_cast<std::int32_t>(slot.size())) {
    return -1;
  }
  return slot[static_cast<std::size_t>(index)].glyph_id;
}

std::int32_t hb_shape_glyph_advance_at(std::int32_t index) {
  const std::vector<ShapedGlyphEntry>& slot = last_shape_slot();
  if (index < 0 || index >= static_cast<std::int32_t>(slot.size())) {
    return -1;
  }
  return slot[static_cast<std::size_t>(index)].x_advance;
}

} // namespace text
} // namespace mlc
