#pragma once

// Thin FreeType mask + RGB cache (TRACK_TEXT_MSDF_TO_MLC STEP=5).
// EDT/SDF math lives in misc/gui/msdf.mlc; MLC fills the RGB cache after mask render.

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc::text {

// FreeType → binary coverage mask (field_size²). Returns 0 on success.
int32_t msdf_mask_render(
  mlc::String font_path,
  int32_t codepoint,
  int32_t field_size
);
int32_t msdf_mask_size();
int32_t msdf_mask_byte_at(int32_t index);

// RGB cache filled by MLC after msdf_sdf_rgb_from_mask.
void msdf_cache_begin(int32_t width, int32_t height, int32_t px_range);
void msdf_cache_push_byte(int32_t value);
void msdf_cache_finish();

int32_t msdf_width();
int32_t msdf_height();
int32_t msdf_px_range();
int32_t msdf_byte_count();
int32_t msdf_byte_at(int32_t index);
const uint8_t* msdf_rgb_data();

} // namespace mlc::text
