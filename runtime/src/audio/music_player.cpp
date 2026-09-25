#include "mlc/audio/music_player.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <pulse/simple.h>
}

#include <chrono>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace mlc {
namespace music_player {
namespace {

constexpr int32_t k_rate = 44100;
constexpr int32_t k_channels = 2;
constexpr int32_t k_bands = 128;
constexpr int32_t k_transform_length = 256;

std::atomic<int32_t> band_levels[k_bands];
std::atomic<uint32_t> band_serial{0};

void note_levels(const int16_t* samples, int32_t frame_count) {
  if (frame_count < 2) {
    return;
  }
  float real_part[k_transform_length];
  float imag_part[k_transform_length];
  const int32_t used = frame_count < k_transform_length ? frame_count : k_transform_length;
  for (int32_t index = 0; index < k_transform_length; index += 1) {
    real_part[index] = 0.0f;
    imag_part[index] = 0.0f;
  }
  for (int32_t index = 0; index < used; index += 1) {
    const float window = 0.5f - 0.5f * std::cos(6.2831853f * static_cast<float>(index) / static_cast<float>(used));
    const int32_t left = samples[static_cast<size_t>(index) * k_channels];
    const int32_t right = samples[static_cast<size_t>(index) * k_channels + 1];
    real_part[index] = window * static_cast<float>(left + right) / 65536.0f;
  }
  for (int32_t index = 0; index < k_transform_length; index += 1) {
    int32_t reversed = 0;
    int32_t value = index;
    for (int32_t bit = 0; bit < 8; bit += 1) {
      reversed = (reversed << 1) | (value & 1);
      value >>= 1;
    }
    if (reversed > index) {
      const float swap_real = real_part[index];
      real_part[index] = real_part[reversed];
      real_part[reversed] = swap_real;
    }
  }
  for (int32_t span = 1; span < k_transform_length; span <<= 1) {
    const float angle = -3.14159265f / static_cast<float>(span);
    const float turn_real = std::cos(angle);
    const float turn_imag = std::sin(angle);
    for (int32_t start = 0; start < k_transform_length; start += span * 2) {
      float factor_real = 1.0f;
      float factor_imag = 0.0f;
      for (int32_t offset = 0; offset < span; offset += 1) {
        const int32_t even = start + offset;
        const int32_t odd = even + span;
        const float mixed_real = factor_real * real_part[odd] - factor_imag * imag_part[odd];
        const float mixed_imag = factor_real * imag_part[odd] + factor_imag * real_part[odd];
        real_part[odd] = real_part[even] - mixed_real;
        imag_part[odd] = imag_part[even] - mixed_imag;
        real_part[even] += mixed_real;
        imag_part[even] += mixed_imag;
        const float next_real = factor_real * turn_real - factor_imag * turn_imag;
        factor_imag = factor_real * turn_imag + factor_imag * turn_real;
        factor_real = next_real;
      }
    }
  }
  for (int32_t band = 0; band < k_bands; band += 1) {
    const int32_t bin = band + 1;
    const float magnitude = std::sqrt(real_part[bin] * real_part[bin] + imag_part[bin] * imag_part[bin]);
    float scaled = std::log10(1.0f + magnitude) / 2.0f;
    if (scaled > 1.0f) {
      scaled = 1.0f;
    }
    const int32_t next = static_cast<int32_t>(scaled * 255.0f);
    const int32_t previous = band_levels[band].load();
    band_levels[band].store(next > previous * 3 / 4 ? next : previous * 3 / 4);
  }
  band_serial.fetch_add(1);
}

struct SharedMusic {
  std::mutex mutex;
  std::condition_variable condition;
  std::string path;
  bool stopping = false;
  bool path_changed = false;
  bool paused = false;
  bool active = false;
  std::vector<std::string> queue;
  int32_t queue_index = 0;
  std::atomic<int32_t> seek_milliseconds{-1};
  std::atomic<int32_t> position_milliseconds{0};
  std::atomic<int32_t> duration_milliseconds{0};
};

SharedMusic& shared_music() {
  static SharedMusic music;
  return music;
}

std::thread& player_thread() {
  static std::thread thread;
  return thread;
}

bool same_request(const std::string& path) {
  SharedMusic& music = shared_music();
  std::lock_guard<std::mutex> guard(music.mutex);
  return music.stopping == false && music.path == path && music.path_changed == false;
}

bool wait_if_paused(const std::string& path) {
  SharedMusic& music = shared_music();
  std::unique_lock<std::mutex> guard(music.mutex);
  while (music.paused && music.stopping == false && music.path == path && music.path_changed == false) {
    music.condition.wait_for(guard, std::chrono::milliseconds(40));
  }
  return music.stopping == false && music.path == path && music.path_changed == false;
}

void play_path(const std::string& path) {
  SharedMusic& music = shared_music();
  const int32_t pending_seek = music.seek_milliseconds.load();
  if (pending_seek >= 0) {
    music.position_milliseconds.store(pending_seek);
  } else {
    music.position_milliseconds.store(0);
  }
  music.duration_milliseconds.store(0);
  AVFormatContext* format = nullptr;
  if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0) {
    return;
  }
  if (avformat_find_stream_info(format, nullptr) < 0) {
    avformat_close_input(&format);
    return;
  }
  const int stream_index = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
  if (stream_index < 0) {
    avformat_close_input(&format);
    return;
  }
  AVStream* stream = format->streams[stream_index];
  if (stream->duration > 0) {
    const int64_t milliseconds = av_rescale_q(stream->duration, stream->time_base, AVRational{1, 1000});
    if (milliseconds > 0 && milliseconds < 2147483647) {
      music.duration_milliseconds.store(static_cast<int32_t>(milliseconds));
    }
  }
  const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
  AVCodecContext* codec = avcodec_alloc_context3(decoder);
  if (decoder == nullptr || codec == nullptr || avcodec_parameters_to_context(codec, stream->codecpar) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  codec->thread_count = 1;
  if (avcodec_open2(codec, decoder, nullptr) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  SwrContext* resampler = swr_alloc();
  if (resampler == nullptr) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  av_opt_set_chlayout(resampler, "in_chlayout", &codec->ch_layout, 0);
  av_opt_set_int(resampler, "in_sample_rate", codec->sample_rate, 0);
  av_opt_set_sample_fmt(resampler, "in_sample_fmt", codec->sample_fmt, 0);
  AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
  av_opt_set_chlayout(resampler, "out_chlayout", &stereo, 0);
  av_opt_set_int(resampler, "out_sample_rate", k_rate, 0);
  av_opt_set_sample_fmt(resampler, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
  if (swr_init(resampler) < 0) {
    swr_free(&resampler);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  pa_sample_spec specification;
  specification.format = PA_SAMPLE_S16LE;
  specification.rate = k_rate;
  specification.channels = k_channels;
  int pulse_error = 0;
  pa_simple* pulse = pa_simple_new(
    nullptr, "mlc", PA_STREAM_PLAYBACK, nullptr, "music", &specification, nullptr, nullptr, &pulse_error
  );
  if (pulse == nullptr) {
    swr_free(&resampler);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  AVPacket* packet = av_packet_alloc();
  AVFrame* frame = av_frame_alloc();
  int64_t samples_written = 0;
  while (same_request(path) && packet != nullptr && frame != nullptr && wait_if_paused(path)) {
    const int32_t seek_to = music.seek_milliseconds.exchange(-1);
    if (seek_to >= 0) {
      const int64_t stamp = av_rescale_q(seek_to, AVRational{1, 1000}, stream->time_base);
      av_seek_frame(format, stream_index, stamp, AVSEEK_FLAG_BACKWARD);
      avcodec_flush_buffers(codec);
      pa_simple_flush(pulse, &pulse_error);
      samples_written = static_cast<int64_t>(seek_to) * k_rate / 1000;
      music.position_milliseconds.store(seek_to);
    }
    if (av_read_frame(format, packet) < 0) {
      break;
    }
    if (packet->stream_index == stream_index && avcodec_send_packet(codec, packet) == 0) {
      while (avcodec_receive_frame(codec, frame) == 0 && same_request(path)) {
        if (wait_if_paused(path) == false) {
          break;
        }
        const int output_count = swr_get_out_samples(resampler, frame->nb_samples) + 32;
        std::vector<int16_t> output(static_cast<size_t>(output_count) * k_channels);
        uint8_t* destination = reinterpret_cast<uint8_t*>(output.data());
        const uint8_t** source_planes = const_cast<const uint8_t**>(frame->extended_data);
        const int converted = swr_convert(resampler, &destination, output_count, source_planes, frame->nb_samples);
        if (converted > 0) {
          const size_t bytes = static_cast<size_t>(converted) * k_channels * sizeof(int16_t);
          if (pa_simple_write(pulse, output.data(), bytes, &pulse_error) < 0) {
            av_frame_unref(frame);
            av_packet_unref(packet);
            goto playback_finished;
          }
          samples_written += converted;
          music.position_milliseconds.store(static_cast<int32_t>(samples_written * 1000 / k_rate));
          note_levels(output.data(), converted);
        }
        av_frame_unref(frame);
      }
    }
    av_packet_unref(packet);
  }
playback_finished:
  pa_simple_drain(pulse, &pulse_error);
  pa_simple_free(pulse);
  av_frame_free(&frame);
  av_packet_free(&packet);
  swr_free(&resampler);
  avcodec_free_context(&codec);
  avformat_close_input(&format);
}

void player_main() {
  SharedMusic& music = shared_music();
  while (true) {
    std::string path;
    {
      std::unique_lock<std::mutex> guard(music.mutex);
      music.condition.wait(guard, [&] { return music.stopping || music.path_changed; });
      if (music.stopping && music.path.empty()) {
        return;
      }
      path = music.path;
      music.path_changed = false;
    }
    if (path.empty()) {
      continue;
    }
    play_path(path);
    std::lock_guard<std::mutex> guard(music.mutex);
    if (music.path == path && music.path_changed == false) {
      const int32_t next_index = music.queue_index + 1;
      if (next_index >= 0 && next_index < static_cast<int32_t>(music.queue.size())) {
        music.queue_index = next_index;
        music.path = music.queue[static_cast<size_t>(next_index)];
        music.path_changed = true;
        music.active = true;
        music.paused = false;
      } else {
        music.active = false;
        music.paused = false;
      }
    }
  }
}

void ensure_player() {
  std::thread& thread = player_thread();
  if (thread.joinable()) {
    return;
  }
  thread = std::thread(player_main);
}

}  // namespace

void play_value(mlc::String path) {
  SharedMusic& music = shared_music();
  {
    std::lock_guard<std::mutex> guard(music.mutex);
    music.path = path.as_std_string();
    music.path_changed = true;
    music.paused = false;
    music.active = music.path.empty() == false;
    music.position_milliseconds.store(0);
  }
  music.condition.notify_all();
  ensure_player();
}

std::vector<std::string> split_paths(const std::string& joined) {
  std::vector<std::string> paths;
  std::string current;
  for (char character : joined) {
    if (character == '\n') {
      if (current.empty() == false) {
        paths.push_back(current);
      }
      current.clear();
    } else {
      current.push_back(character);
    }
  }
  if (current.empty() == false) {
    paths.push_back(current);
  }
  return paths;
}

void play_list_value(mlc::String paths, int32_t start_index) {
  SharedMusic& music = shared_music();
  const std::vector<std::string> queue = split_paths(paths.as_std_string());
  if (queue.empty()) {
    return;
  }
  int32_t index = start_index;
  if (index < 0 || index >= static_cast<int32_t>(queue.size())) {
    index = 0;
  }
  {
    std::lock_guard<std::mutex> guard(music.mutex);
    music.queue = queue;
    music.queue_index = index;
  }
  play_value(mlc::String(queue[static_cast<size_t>(index)]));
}

void restore_value(mlc::String paths, int32_t start_index, int32_t position_milliseconds, int32_t paused) {
  if (position_milliseconds < 0) {
    position_milliseconds = 0;
  }
  play_list_value(paths, start_index);
  SharedMusic& music = shared_music();
  music.seek_milliseconds.store(position_milliseconds);
  music.position_milliseconds.store(position_milliseconds);
  if (paused != 0) {
    std::lock_guard<std::mutex> guard(music.mutex);
    music.paused = true;
  }
}

void step_queue(int32_t delta) {
  SharedMusic& music = shared_music();
  std::string path;
  {
    std::lock_guard<std::mutex> guard(music.mutex);
    if (music.queue.empty()) {
      return;
    }
    int32_t index = music.queue_index + delta;
    if (index < 0) {
      index = 0;
    }
    if (index >= static_cast<int32_t>(music.queue.size())) {
      index = static_cast<int32_t>(music.queue.size()) - 1;
    }
    music.queue_index = index;
    path = music.queue[static_cast<size_t>(index)];
  }
  play_value(mlc::String(path));
}

void next_value() {
  step_queue(1);
}

void previous_value() {
  step_queue(-1);
}

void seek_value(int32_t milliseconds) {
  if (milliseconds < 0) {
    milliseconds = 0;
  }
  shared_music().seek_milliseconds.store(milliseconds);
  shared_music().position_milliseconds.store(milliseconds);
}

void toggle_value() {
  SharedMusic& music = shared_music();
  {
    std::lock_guard<std::mutex> guard(music.mutex);
    if (music.active == false) {
      return;
    }
    music.paused = music.paused == false;
  }
  music.condition.notify_all();
}

void release_value() {
  SharedMusic& music = shared_music();
  {
    std::lock_guard<std::mutex> guard(music.mutex);
    music.stopping = true;
    music.active = false;
    music.path.clear();
    music.path_changed = true;
    music.paused = false;
  }
  music.condition.notify_all();
  std::thread& thread = player_thread();
  if (thread.joinable()) {
    thread.join();
  }
}

int32_t active_value() {
  SharedMusic& music = shared_music();
  std::lock_guard<std::mutex> guard(music.mutex);
  return music.active ? 1 : 0;
}

int32_t paused_value() {
  SharedMusic& music = shared_music();
  std::lock_guard<std::mutex> guard(music.mutex);
  return music.paused ? 1 : 0;
}

int32_t position_value() {
  return shared_music().position_milliseconds.load();
}

int32_t duration_value() {
  return shared_music().duration_milliseconds.load();
}

int32_t band_level_value(int32_t band) {
  if (band < 0 || band >= k_bands) {
    return 0;
  }
  return band_levels[band].load();
}

mlc::String path_value() {
  SharedMusic& music = shared_music();
  std::lock_guard<std::mutex> guard(music.mutex);
  if (music.active == false) {
    return mlc::String("");
  }
  return mlc::String(music.path);
}

mlc::String queue_value() {
  SharedMusic& music = shared_music();
  std::lock_guard<std::mutex> guard(music.mutex);
  if (music.active == false) {
    return mlc::String("");
  }
  std::string joined;
  for (size_t index = 0; index < music.queue.size(); index += 1) {
    if (index > 0) {
      joined.push_back('\n');
    }
    joined += music.queue[index];
  }
  return mlc::String(std::move(joined));
}

int32_t queue_index_value() {
  SharedMusic& music = shared_music();
  std::lock_guard<std::mutex> guard(music.mutex);
  return music.queue_index;
}

mlc::String title_value() {
  SharedMusic& music = shared_music();
  std::string path;
  {
    std::lock_guard<std::mutex> guard(music.mutex);
    if (music.active == false) {
      return mlc::String("");
    }
    path = music.path;
  }
  const size_t slash = path.find_last_of('/');
  if (slash != std::string::npos) {
    path.erase(0, slash + 1);
  }
  return mlc::String(std::move(path));
}

}  // namespace music_player
}  // namespace mlc
