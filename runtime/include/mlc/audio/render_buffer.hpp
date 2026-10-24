#pragma once

#include <cstdint>
#include <malloc.h>

namespace mlc {
namespace audio {

inline constexpr std::int32_t audio_render_capacity() {
  return 64;
}

struct AudioRenderBuffer {
  std::int32_t samples[64];
  std::int32_t length;
};

inline AudioRenderBuffer& audio_render_buffer() {
  static AudioRenderBuffer buffer{{}, 0};
  return buffer;
}

inline std::int32_t audio_render_prepare(std::int32_t length) {
  if (length <= 0 || length > audio_render_capacity()) {
    return 0;
  }
  AudioRenderBuffer& buffer = audio_render_buffer();
  std::int32_t index = 0;
  while (index < length) {
    buffer.samples[index] = 0;
    index = index + 1;
  }
  buffer.length = length;
  return 1;
}

inline std::int32_t audio_render_length() {
  return audio_render_buffer().length;
}

inline std::int32_t audio_render_occupied(std::int32_t index) {
  if (index < 0 || index >= audio_render_buffer().length) {
    return 0;
  }
  return 1;
}

inline std::int32_t audio_render_store(std::int32_t index, std::int32_t sample) {
  AudioRenderBuffer& buffer = audio_render_buffer();
  if (index < 0 || index >= buffer.length) {
    return 0;
  }
  buffer.samples[index] = sample;
  return 1;
}

inline std::int32_t audio_render_load(std::int32_t index) {
  const AudioRenderBuffer& buffer = audio_render_buffer();
  if (index < 0 || index >= buffer.length) {
    return 0;
  }
  return buffer.samples[index];
}

inline std::int32_t audio_render_heap_bytes() {
  const struct mallinfo2 info = ::mallinfo2();
  if (info.uordblks > static_cast<std::size_t>(INT32_MAX)) {
    return -1;
  }
  return static_cast<std::int32_t>(info.uordblks);
}

}  // namespace audio
}  // namespace mlc
