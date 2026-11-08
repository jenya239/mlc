#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace mlc {
namespace image_preview {

void video_watch(const std::string& path);
void video_replay(const std::string& path);
void video_play_full(const std::string& path);
void video_clear();
void video_shutdown();
void video_toggle_paused();
void video_seek_milliseconds(int32_t milliseconds);
int32_t video_position_milliseconds();
int32_t video_duration_milliseconds();
bool video_is_paused();
void video_transport_set_borrowed(int32_t borrowed);
int32_t video_transport_borrowed();
void video_transport_request_step(int32_t step);
void video_transport_request_toggle();
void video_transport_request_seek(int32_t milliseconds);
int32_t video_transport_take_step();
int32_t video_transport_take_toggle();
int32_t video_transport_take_seek();
int32_t video_transport_seek_target();
bool video_is_active();
bool video_is_full();
bool video_frame_pending(uint64_t serial);
void video_displayed_size(int32_t* width, int32_t* height);
bool video_take_frame(std::vector<uint8_t>* pixels, int32_t* width, int32_t* height, uint64_t* serial);

}  // namespace image_preview
}  // namespace mlc
