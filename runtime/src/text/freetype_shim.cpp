#include "mlc/text/freetype_shim.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace mlc::text {

int32_t glyph_bitmap_packed(mlc::String font_path, int32_t codepoint, int32_t pixel_size) {
  if (pixel_size <= 0) {
    return -1;
  }
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return -2;
  }
  FT_Face face = nullptr;
  const int new_face_status = FT_New_Face(library, font_path.c_str(), 0, &face);
  if (new_face_status != 0) {
    FT_Done_FreeType(library);
    return -3;
  }
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -4;
  }
  const FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(codepoint));
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
  if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return -7;
  }
  const int32_t width = static_cast<int32_t>(face->glyph->bitmap.width);
  const int32_t rows = static_cast<int32_t>(face->glyph->bitmap.rows);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  if (width < 0 || rows < 0 || width > 0xffff || rows > 0xffff) {
    return -8;
  }
  return (width << 16) | rows;
}

} // namespace mlc::text
