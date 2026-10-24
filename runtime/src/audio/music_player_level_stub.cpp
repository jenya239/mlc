#include "mlc/audio/music_player.hpp"

namespace mlc {
namespace music_player {

void play_value(String) {}

void play_list_value(String, int32_t) {}

void restore_value(String, int32_t, int32_t, int32_t) {}

void next_value() {}

void previous_value() {}

void seek_value(int32_t) {}

void toggle_value() {}

void release_value() {}

int32_t active_value() {
  return 0;
}

int32_t paused_value() {
  return 0;
}

int32_t position_value() {
  return 0;
}

int32_t duration_value() {
  return 0;
}

String title_value() {
  return String();
}

String path_value() {
  return String();
}

String queue_value() {
  return String();
}

int32_t queue_index_value() {
  return 0;
}

int32_t band_level_value(int32_t) {
  return 0;
}

}  // namespace music_player
}  // namespace mlc
