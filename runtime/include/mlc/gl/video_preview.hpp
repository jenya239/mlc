#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace mlc {
namespace image_preview {

void video_watch(const std::string& path);
void video_clear();
void video_shutdown();
bool video_is_active();
bool video_frame_pending(uint64_t serial);
bool video_take_frame(std::vector<uint8_t>* pixels, int32_t* width, int32_t* height, uint64_t* serial);

}  // namespace image_preview
}  // namespace mlc
