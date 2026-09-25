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
  music.position_milliseconds.store(0);
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
