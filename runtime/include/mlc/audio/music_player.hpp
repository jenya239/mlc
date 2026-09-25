#pragma once

#include "mlc/core/string.hpp"
#include <cstdint>

namespace mlc {
namespace music_player {

void play_value(mlc::String path);
void play_list_value(mlc::String paths, int32_t start_index);
void restore_value(mlc::String paths, int32_t start_index, int32_t position_milliseconds, int32_t paused);
void next_value();
void previous_value();
void seek_value(int32_t milliseconds);
void toggle_value();
void release_value();
int32_t active_value();
int32_t paused_value();
int32_t position_value();
int32_t duration_value();
mlc::String title_value();
mlc::String path_value();
mlc::String queue_value();
int32_t queue_index_value();
int32_t band_level_value(int32_t band);

}  // namespace music_player
}  // namespace mlc
