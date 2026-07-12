#include "mlc/text/harfbuzz_shim.hpp"

#include "mlc/text/freetype_abi.hpp"
#include "mlc/text/harfbuzz_abi.hpp"

#include <cstdint>

namespace mlc::text {

// Deprecated thin wrapper over freetype_abi + harfbuzz_abi (no path/size cache).
// Prefer misc/gui/text_shaping.mlc (TRACK_TEXT_SHIM_TO_MLC STEP=8).

int32_t shape_glyph_count(mlc::String font_path, mlc::String text, int32_t pixel_size) {
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
  const std::int64_t harfbuzz_font = hb_font_create_from_face(face);
  if (harfbuzz_font == 0) {
    ft_face_close(face);
    return -5;
  }
  const int32_t count = hb_shape_text(harfbuzz_font, text);
  hb_font_destroy(harfbuzz_font);
  ft_face_close(face);
  return count;
}

int32_t shape_glyph_id_at(int32_t index) {
  return hb_shape_glyph_id_at(index);
}

int32_t shape_glyph_advance_at(int32_t index) {
  return hb_shape_glyph_advance_at(index);
}

} // namespace mlc::text
