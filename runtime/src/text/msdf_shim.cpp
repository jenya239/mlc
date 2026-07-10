#include "mlc/text/msdf_shim.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace mlc::text {
namespace {

struct MsdfCache {
  int32_t width = 0;
  int32_t height = 0;
  int32_t px_range = 0;
  std::vector<uint8_t> rgb;
};

MsdfCache g_msdf_cache;

void clear_cache() {
  g_msdf_cache.width = 0;
  g_msdf_cache.height = 0;
  g_msdf_cache.px_range = 0;
  g_msdf_cache.rgb.clear();
}

// 1D squared EDT (Felzenszwalb & Huttenlocher).
void distance_transform_1d(
  const std::vector<float>& input,
  std::vector<float>& output,
  int length
) {
  const float infinity = std::numeric_limits<float>::infinity();
  std::vector<int> parabola_index(static_cast<size_t>(length));
  std::vector<float> parabola_boundary(static_cast<size_t>(length + 1));
  int envelope_size = 0;
  parabola_index[0] = 0;
  parabola_boundary[0] = -infinity;
  parabola_boundary[1] = infinity;
  for (int query = 1; query < length; query += 1) {
    float intersection = infinity;
    while (true) {
      const int envelope = parabola_index[envelope_size];
      intersection = (
        (input[static_cast<size_t>(query)] + static_cast<float>(query * query))
        - (input[static_cast<size_t>(envelope)] + static_cast<float>(envelope * envelope))
      ) / static_cast<float>(2 * query - 2 * envelope);
      if (intersection > parabola_boundary[envelope_size]) {
        break;
      }
      envelope_size -= 1;
    }
    envelope_size += 1;
    parabola_index[static_cast<size_t>(envelope_size)] = query;
    parabola_boundary[static_cast<size_t>(envelope_size)] = intersection;
    parabola_boundary[static_cast<size_t>(envelope_size + 1)] = infinity;
  }
  envelope_size = 0;
  for (int query = 0; query < length; query += 1) {
    while (parabola_boundary[static_cast<size_t>(envelope_size + 1)] < static_cast<float>(query)) {
      envelope_size += 1;
    }
    const int envelope = parabola_index[static_cast<size_t>(envelope_size)];
    const float delta = static_cast<float>(query - envelope);
    output[static_cast<size_t>(query)] =
      delta * delta + input[static_cast<size_t>(envelope)];
  }
}

void distance_transform_2d(
  std::vector<float>& field,
  int32_t width,
  int32_t height
) {
  std::vector<float> column_input(static_cast<size_t>(height));
  std::vector<float> column_output(static_cast<size_t>(height));
  for (int32_t column = 0; column < width; column += 1) {
    for (int32_t row = 0; row < height; row += 1) {
      column_input[static_cast<size_t>(row)] =
        field[static_cast<size_t>(row * width + column)];
    }
    distance_transform_1d(column_input, column_output, height);
    for (int32_t row = 0; row < height; row += 1) {
      field[static_cast<size_t>(row * width + column)] =
        column_output[static_cast<size_t>(row)];
    }
  }
  std::vector<float> row_input(static_cast<size_t>(width));
  std::vector<float> row_output(static_cast<size_t>(width));
  for (int32_t row = 0; row < height; row += 1) {
    for (int32_t column = 0; column < width; column += 1) {
      row_input[static_cast<size_t>(column)] =
        field[static_cast<size_t>(row * width + column)];
    }
    distance_transform_1d(row_input, row_output, width);
    for (int32_t column = 0; column < width; column += 1) {
      field[static_cast<size_t>(row * width + column)] =
        row_output[static_cast<size_t>(column)];
    }
  }
}

bool render_glyph_mask(
  const char* font_path,
  int32_t codepoint,
  int32_t field_size,
  std::vector<uint8_t>& mask
) {
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return false;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path, 0, &face) != 0) {
    FT_Done_FreeType(library);
    return false;
  }
  // Pad so the SDF has room for px_range around the glyph.
  const int32_t pixel_size = std::max(8, field_size - 8);
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  const FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(codepoint));
  if (glyph_index == 0
      || FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0
      || FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  const int32_t glyph_width = static_cast<int32_t>(face->glyph->bitmap.width);
  const int32_t glyph_rows = static_cast<int32_t>(face->glyph->bitmap.rows);
  const int pitch = face->glyph->bitmap.pitch;
  mask.assign(static_cast<size_t>(field_size * field_size), 0);
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
      mask[static_cast<size_t>(y * field_size + x)] = 1;
    }
  }
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return true;
}

} // namespace

int32_t msdf_generate(
  mlc::String font_path,
  int32_t codepoint,
  int32_t field_size,
  int32_t px_range
) {
  clear_cache();
  if (field_size < 8 || field_size > 256 || px_range < 1 || px_range > 64) {
    return -1;
  }
  std::vector<uint8_t> mask;
  if (!render_glyph_mask(font_path.c_str(), codepoint, field_size, mask)) {
    return -2;
  }

  const float infinity = 1.0e20f;
  std::vector<float> outside(static_cast<size_t>(field_size * field_size));
  std::vector<float> inside(static_cast<size_t>(field_size * field_size));
  for (int32_t index = 0; index < field_size * field_size; index += 1) {
    if (mask[static_cast<size_t>(index)] != 0) {
      outside[static_cast<size_t>(index)] = 0.0f;
      inside[static_cast<size_t>(index)] = infinity;
    } else {
      outside[static_cast<size_t>(index)] = infinity;
      inside[static_cast<size_t>(index)] = 0.0f;
    }
  }
  distance_transform_2d(outside, field_size, field_size);
  distance_transform_2d(inside, field_size, field_size);

  g_msdf_cache.width = field_size;
  g_msdf_cache.height = field_size;
  g_msdf_cache.px_range = px_range;
  g_msdf_cache.rgb.assign(static_cast<size_t>(field_size * field_size * 3), 0);
  const float range = static_cast<float>(px_range);
  for (int32_t index = 0; index < field_size * field_size; index += 1) {
    const float outside_distance = std::sqrt(outside[static_cast<size_t>(index)]);
    const float inside_distance = std::sqrt(inside[static_cast<size_t>(index)]);
    // Signed: positive outside, negative inside; 0.5 at the edge.
    const float signed_distance = outside_distance - inside_distance;
    float mapped = signed_distance / range * 0.5f + 0.5f;
    if (mapped < 0.0f) {
      mapped = 0.0f;
    }
    if (mapped > 1.0f) {
      mapped = 1.0f;
    }
    const uint8_t byte = static_cast<uint8_t>(mapped * 255.0f + 0.5f);
    // True multi-channel edge coloring needs msdfgen; RGB channels share SDF
    // so median() MSDF shaders still recover a usable distance.
    const size_t base = static_cast<size_t>(index * 3);
    g_msdf_cache.rgb[base + 0] = byte;
    g_msdf_cache.rgb[base + 1] = byte;
    g_msdf_cache.rgb[base + 2] = byte;
  }
  return 0;
}

int32_t msdf_width() {
  return g_msdf_cache.width;
}

int32_t msdf_height() {
  return g_msdf_cache.height;
}

int32_t msdf_px_range() {
  return g_msdf_cache.px_range;
}

int32_t msdf_byte_count() {
  return static_cast<int32_t>(g_msdf_cache.rgb.size());
}

int32_t msdf_byte_at(int32_t index) {
  if (index < 0 || index >= static_cast<int32_t>(g_msdf_cache.rgb.size())) {
    return -1;
  }
  return static_cast<int32_t>(g_msdf_cache.rgb[static_cast<size_t>(index)]);
}

const uint8_t* msdf_rgb_data() {
  if (g_msdf_cache.rgb.empty()) {
    return nullptr;
  }
  return g_msdf_cache.rgb.data();
}

} // namespace mlc::text
