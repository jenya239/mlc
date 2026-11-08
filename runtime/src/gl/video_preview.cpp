#include "mlc/gl/video_preview.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <pulse/simple.h>
}

#include <chrono>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace mlc {
namespace image_preview {
namespace {

constexpr int32_t k_long_side = 480;

struct SharedVideo {
  std::mutex mutex;
  std::condition_variable condition;
  std::string path;
  bool stopping = false;
  bool path_changed = false;
  bool active = false;
  int quality = 0;
  uint64_t playback_request = 0;
  bool decode_again = false;
  bool paused = false;
  bool seek_pending = false;
  int64_t seek_milliseconds = 0;
  int64_t duration_milliseconds = 0;
  int64_t position_milliseconds = 0;
  std::atomic<uint32_t> seek_serial{0};
  std::atomic<uint32_t> playback_loop{0};
  std::atomic<bool> soundtrack_open{false};
  std::atomic<bool> audio_cancel{false};
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t height = 0;
  uint64_t serial = 0;
  std::atomic<bool> cancel{false};
};

SharedVideo& shared_video() {
  static SharedVideo video;
  return video;
}

std::thread& decoder_thread() {
  static std::thread thread;
  return thread;
}

int interrupt_read(void* opaque) {
  auto* cancel = static_cast<std::atomic<bool>*>(opaque);
  return cancel->load() ? 1 : 0;
}

bool same_request(const std::string& path, uint64_t request) {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.stopping == false && video.path == path && video.playback_request == request && video.path_changed == false;
}

constexpr int32_t full_long_side = 1920;

struct FramePace {
  bool realtime = false;
  AVRational time_base{1, 1};
  int64_t origin_pts = AV_NOPTS_VALUE;
  int64_t frame_interval_micros = 33333;
  std::chrono::steady_clock::time_point origin{};
  bool origin_set = false;
};

bool soundtrack_current(const std::string& path, uint64_t request) {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.stopping == false && video.quality == 1 && video.path == path && video.playback_request == request && video.soundtrack_open.load();
}

void ensure_channel_layout(AVCodecContext* codec) {
  if (codec->ch_layout.nb_channels > 0 && codec->ch_layout.order != AV_CHANNEL_ORDER_UNSPEC) {
    return;
  }
  int channels = codec->ch_layout.nb_channels;
  if (channels <= 0) {
    channels = 2;
  }
  av_channel_layout_uninit(&codec->ch_layout);
  av_channel_layout_default(&codec->ch_layout, channels);
}

pa_simple* open_playback(int* pulse_error) {
  pa_sample_spec specification;
  specification.format = PA_SAMPLE_S16LE;
  specification.rate = 44100;
  specification.channels = 2;
  pa_buffer_attr attribute;
  attribute.maxlength = static_cast<uint32_t>(-1);
  attribute.tlength = static_cast<uint32_t>(44100 / 20 * 4);
  attribute.prebuf = static_cast<uint32_t>(-1);
  attribute.minreq = static_cast<uint32_t>(-1);
  attribute.fragsize = static_cast<uint32_t>(-1);
  pa_simple* pulse = pa_simple_new(
    nullptr, "mlc", PA_STREAM_PLAYBACK, nullptr, "video", &specification, nullptr, &attribute, pulse_error
  );
  if (pulse != nullptr) {
    return pulse;
  }
  *pulse_error = 0;
  return pa_simple_new(
    nullptr, "mlc", PA_STREAM_PLAYBACK, nullptr, "video", &specification, nullptr, nullptr, pulse_error
  );
}

void play_soundtrack(std::string path, uint64_t request) {
  SharedVideo& video = shared_video();
  const uint32_t started_loop = video.playback_loop.load();
  AVFormatContext* format = nullptr;
  if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0) {
    return;
  }
  format->interrupt_callback.callback = interrupt_read;
  format->interrupt_callback.opaque = &video.audio_cancel;
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
  const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
  AVCodecContext* codec = avcodec_alloc_context3(decoder);
  if (decoder == nullptr || codec == nullptr || avcodec_parameters_to_context(codec, stream->codecpar) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  if (avcodec_open2(codec, decoder, nullptr) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  ensure_channel_layout(codec);
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
  av_opt_set_int(resampler, "out_sample_rate", 44100, 0);
  av_opt_set_sample_fmt(resampler, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
  if (swr_init(resampler) < 0) {
    swr_free(&resampler);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  int pulse_error = 0;
  pa_simple* pulse = open_playback(&pulse_error);
  if (pulse == nullptr) {
    swr_free(&resampler);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  AVPacket* packet = av_packet_alloc();
  AVFrame* frame = av_frame_alloc();
  uint32_t heard_loop = started_loop;
  uint32_t heard_seek = video.seek_serial.load();
  bool finished = false;
  while (finished == false && soundtrack_current(path, request) && packet != nullptr && frame != nullptr) {
    {
      std::unique_lock<std::mutex> guard(video.mutex);
      video.condition.wait(guard, [&] {
        return video.soundtrack_open.load() == false || video.playback_request != request || video.stopping || video.paused == false || video.seek_serial.load() != heard_seek;
      });
    }
    if (soundtrack_current(path, request) == false) {
      finished = true;
      continue;
    }
    const uint32_t seek_now = video.seek_serial.load();
    if (seek_now != heard_seek) {
      int64_t milliseconds = 0;
      bool still_paused = false;
      {
        std::lock_guard<std::mutex> guard(video.mutex);
        milliseconds = video.seek_milliseconds;
        still_paused = video.paused;
      }
      const int64_t target = av_rescale(milliseconds, AV_TIME_BASE, 1000);
      av_seek_frame(format, -1, target, AVSEEK_FLAG_BACKWARD);
      avcodec_flush_buffers(codec);
      pa_simple_flush(pulse, &pulse_error);
      heard_seek = seek_now;
      if (still_paused) {
        continue;
      }
    }
    const uint32_t loop = video.playback_loop.load();
    if (loop != heard_loop) {
      av_seek_frame(format, stream_index, 0, AVSEEK_FLAG_BACKWARD);
      avcodec_flush_buffers(codec);
      pa_simple_flush(pulse, &pulse_error);
      heard_loop = loop;
    }
    const int read = av_read_frame(format, packet);
    if (read == AVERROR_EOF) {
      std::unique_lock<std::mutex> guard(video.mutex);
      video.condition.wait_for(guard, std::chrono::milliseconds(40), [&] {
        return video.playback_loop.load() != heard_loop || video.soundtrack_open.load() == false || video.playback_request != request || video.quality != 1 || video.path != path || video.stopping;
      });
      continue;
    }
    if (read < 0) {
      finished = true;
      continue;
    }
    if (packet->stream_index == stream_index && avcodec_send_packet(codec, packet) == 0) {
      while (finished == false && avcodec_receive_frame(codec, frame) == 0 && soundtrack_current(path, request)) {
        const int output_count = swr_get_out_samples(resampler, frame->nb_samples);
        if (output_count < 0 || output_count > 1000000 || frame->extended_data == nullptr) {
          av_frame_unref(frame);
          continue;
        }
        const int output_capacity = output_count + 32;
        std::vector<int16_t> output(static_cast<size_t>(output_capacity) * 2);
        uint8_t* destination = reinterpret_cast<uint8_t*>(output.data());
        const uint8_t** source_planes = const_cast<const uint8_t**>(frame->extended_data);
        const int converted = swr_convert(resampler, &destination, output_capacity, source_planes, frame->nb_samples);
        if (converted > 0 && converted <= output_capacity && soundtrack_current(path, request)) {
          const size_t bytes = static_cast<size_t>(converted) * 2 * sizeof(int16_t);
          if (pa_simple_write(pulse, output.data(), bytes, &pulse_error) < 0) {
            finished = true;
          }
        }
        av_frame_unref(frame);
      }
    }
    av_packet_unref(packet);
  }
  pa_simple_flush(pulse, &pulse_error);
  pa_simple_free(pulse);
  av_frame_free(&frame);
  av_packet_free(&packet);
  swr_free(&resampler);
  avcodec_free_context(&codec);
  avformat_close_input(&format);
}

void fit_side(int32_t source_width, int32_t source_height, int32_t long_side, int32_t* width, int32_t* height) {
  int32_t next_width = source_width;
  int32_t next_height = source_height;
  if (next_width > long_side || next_height > long_side) {
    if (next_width >= next_height && next_width > 0) {
      next_height = next_height * long_side / next_width;
      next_width = long_side;
    } else if (next_height > 0) {
      next_width = next_width * long_side / next_height;
      next_height = long_side;
    }
  }
  if (next_width < 1) {
    next_width = 1;
  }
  if (next_height < 1) {
    next_height = 1;
  }
  *width = next_width;
  *height = next_height;
}

struct ScalerSlot {
  SwsContext* context = nullptr;
  int32_t source_width = 0;
  int32_t source_height = 0;
  int32_t target_width = 0;
  int32_t target_height = 0;
  int format = -1;
};

bool publish_scaled(AVFrame* frame, ScalerSlot* scaler, int32_t long_side, FramePace* pace, uint64_t request) {
  av_frame_apply_cropping(frame, 0);
  if (frame->width < 1 || frame->height < 1 || frame->data[0] == nullptr) {
    return false;
  }
  int32_t display_width = frame->width;
  const AVRational sample_aspect = frame->sample_aspect_ratio;
  if (sample_aspect.num > 0 && sample_aspect.den > 0) {
    display_width = frame->width * sample_aspect.num / sample_aspect.den;
  }
  if (display_width < 1) {
    display_width = frame->width;
  }
  int32_t target_width = 0;
  int32_t target_height = 0;
  fit_side(display_width, frame->height, long_side, &target_width, &target_height);
  const int frame_format = frame->format;
  if (scaler->context == nullptr || scaler->source_width != frame->width || scaler->source_height != frame->height || scaler->target_width != target_width || scaler->target_height != target_height || scaler->format != frame_format) {
    if (scaler->context != nullptr) {
      sws_freeContext(scaler->context);
    }
    const int scale_flags = pace->realtime ? SWS_BILINEAR : SWS_FAST_BILINEAR;
    scaler->context = sws_getContext(
      frame->width,
      frame->height,
      static_cast<AVPixelFormat>(frame_format),
      target_width,
      target_height,
      AV_PIX_FMT_RGBA,
      scale_flags,
      nullptr,
      nullptr,
      nullptr
    );
    scaler->source_width = frame->width;
    scaler->source_height = frame->height;
    scaler->target_width = target_width;
    scaler->target_height = target_height;
    scaler->format = frame_format;
  }
  if (scaler->context == nullptr) {
    return false;
  }
  const int32_t row_bytes = target_width * 4;
  const int32_t stride = (row_bytes + 31) & ~31;
  std::vector<uint8_t> scaled(static_cast<size_t>(stride) * static_cast<size_t>(target_height) + static_cast<size_t>(stride));
  uint8_t* destination_data[1] = {scaled.data()};
  int destination_stride[1] = {stride};
  const int produced = sws_scale(scaler->context, frame->data, frame->linesize, 0, frame->height, destination_data, destination_stride);
  if (produced < 1) {
    return false;
  }
  const int32_t rows = produced < target_height ? produced : target_height;
  std::vector<uint8_t> pixels(static_cast<size_t>(row_bytes) * static_cast<size_t>(target_height));
  for (int32_t row = 0; row < rows; row += 1) {
    std::memcpy(pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(row_bytes), scaled.data() + static_cast<size_t>(row) * static_cast<size_t>(stride), static_cast<size_t>(row_bytes));
  }
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    if (video.path_changed || video.stopping || video.playback_request != request) {
      return false;
    }
    video.pixels.swap(pixels);
    video.width = target_width;
    video.height = target_height;
    video.serial += 1;
    const int64_t stamp = frame->best_effort_timestamp != AV_NOPTS_VALUE ? frame->best_effort_timestamp : frame->pts;
    if (stamp != AV_NOPTS_VALUE) {
      const int64_t millis = av_rescale_q(stamp, pace->time_base, AVRational{1, 1000});
      if (millis >= 0) {
        video.position_milliseconds = millis;
      }
    }
  }
  auto delay = std::chrono::milliseconds(80);
  if (pace->realtime) {
    delay = std::chrono::milliseconds(0);
    const auto now = std::chrono::steady_clock::now();
    if (pace->origin_set == false) {
      pace->origin_set = true;
      pace->origin = now;
      pace->origin_pts = frame->pts;
    } else if (frame->pts != AV_NOPTS_VALUE && pace->origin_pts != AV_NOPTS_VALUE) {
      const int64_t micros = av_rescale_q(frame->pts - pace->origin_pts, pace->time_base, AVRational{1, 1000000});
      if (micros > 0 && micros <= 200000) {
        const auto due = pace->origin + std::chrono::microseconds(micros);
        if (due > now) {
          delay = std::chrono::duration_cast<std::chrono::milliseconds>(due - now);
        }
      } else {
        delay = std::chrono::milliseconds(pace->frame_interval_micros / 1000);
        pace->origin = now;
        pace->origin_pts = frame->pts;
      }
    } else {
      delay = std::chrono::milliseconds(pace->frame_interval_micros / 1000);
    }
  }
  if (delay.count() > 0) {
    std::unique_lock<std::mutex> pause(video.mutex);
    video.condition.wait_for(pause, delay, [&] {
      return video.path_changed || video.stopping || video.playback_request != request || video.seek_pending;
    });
  }
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    if (video.path_changed || video.stopping || video.playback_request != request) {
      return false;
    }
  }
  return true;
}

bool receive_frames(AVCodecContext* codec, AVFrame* frame, ScalerSlot* scaler, const std::string& path, int32_t long_side, FramePace* pace, uint64_t request) {
  while (same_request(path, request)) {
    const int received = avcodec_receive_frame(codec, frame);
    if (received == AVERROR(EAGAIN) || received == AVERROR_EOF) {
      return received != AVERROR_EOF;
    }
    if (received < 0) {
      return false;
    }
    if (publish_scaled(frame, scaler, long_side, pace, request) == false) {
      av_frame_unref(frame);
      return false;
    }
    av_frame_unref(frame);
  }
  return false;
}

void decode_path(const std::string& path, int quality, uint64_t request) {
  SharedVideo& video = shared_video();
  if (same_request(path, request) == false) {
    return;
  }
  const int32_t long_side = quality == 1 ? full_long_side : k_long_side;
  video.cancel.store(false);
  AVFormatContext* format = nullptr;
  if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0) {
    return;
  }
  format->interrupt_callback.callback = interrupt_read;
  format->interrupt_callback.opaque = &video.cancel;
  if (avformat_find_stream_info(format, nullptr) < 0 || same_request(path, request) == false) {
    avformat_close_input(&format);
    return;
  }
  const int stream_index = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (stream_index < 0) {
    avformat_close_input(&format);
    return;
  }
  AVStream* stream = format->streams[stream_index];
  const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
  AVCodecContext* codec = avcodec_alloc_context3(decoder);
  if (decoder == nullptr || codec == nullptr || avcodec_parameters_to_context(codec, stream->codecpar) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  codec->thread_count = quality == 1 ? 2 : 1;
  if (avcodec_open2(codec, decoder, nullptr) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  AVPacket* packet = av_packet_alloc();
  AVFrame* frame = av_frame_alloc();
  ScalerSlot scaler;
  FramePace pace;
  pace.realtime = quality == 1;
  pace.time_base = stream->time_base;
  if (stream->avg_frame_rate.num > 0 && stream->avg_frame_rate.den > 0) {
    const int64_t interval = av_rescale(
      static_cast<int64_t>(stream->avg_frame_rate.den),
      1000000,
      static_cast<int64_t>(stream->avg_frame_rate.num)
    );
    if (interval >= 1000 && interval <= 200000) {
      pace.frame_interval_micros = interval;
    }
  }
  uint64_t serial_before = 0;
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    serial_before = video.serial;
  }
  if (format->duration > 0) {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.duration_milliseconds = format->duration / (AV_TIME_BASE / 1000);
  }
  std::thread soundtrack;
  if (quality == 1 && same_request(path, request)) {
    video.audio_cancel.store(false);
    video.soundtrack_open.store(true);
    soundtrack = std::thread(play_soundtrack, path, request);
  }
  while (same_request(path, request) && packet != nullptr && frame != nullptr) {
    bool have_seek = false;
    int64_t milliseconds = 0;
    {
      std::lock_guard<std::mutex> guard(video.mutex);
      if (video.seek_pending) {
        have_seek = true;
        milliseconds = video.seek_milliseconds;
        video.seek_pending = false;
      }
    }
    if (have_seek) {
      const int64_t target = av_rescale(milliseconds, AV_TIME_BASE, 1000);
      if (av_seek_frame(format, -1, target, AVSEEK_FLAG_BACKWARD) < 0) {
        avformat_seek_file(format, -1, 0, target, target, AVSEEK_FLAG_BACKWARD);
      }
      avcodec_flush_buffers(codec);
      pace.origin_set = false;
      video.condition.notify_all();
    }
    bool seek_waiting = false;
    {
      std::unique_lock<std::mutex> guard(video.mutex);
      video.condition.wait(guard, [&] {
        return video.stopping || video.path != path || video.playback_request != request || video.path_changed || video.paused == false || video.seek_pending;
      });
      seek_waiting = video.seek_pending;
    }
    if (same_request(path, request) == false) {
      break;
    }
    if (seek_waiting) {
      continue;
    }
    const int read = av_read_frame(format, packet);
    if (read == AVERROR_EOF) {
      avcodec_send_packet(codec, nullptr);
      receive_frames(codec, frame, &scaler, path, long_side, &pace, request);
      avcodec_flush_buffers(codec);
      {
        std::lock_guard<std::mutex> guard(video.mutex);
        if (video.serial != serial_before && video.stopping == false && video.path == path && video.playback_request == request && video.path_changed == false) {
          video.decode_again = true;
        }
      }
      video.condition.notify_all();
      break;
    }
    if (read < 0) {
      break;
    }
    if (packet->stream_index == stream_index) {
      if (avcodec_send_packet(codec, packet) == 0) {
        if (receive_frames(codec, frame, &scaler, path, long_side, &pace, request) == false) {
          av_packet_unref(packet);
          break;
        }
      }
    }
    av_packet_unref(packet);
  }
  video.audio_cancel.store(true);
  video.soundtrack_open.store(false);
  video.condition.notify_all();
  if (soundtrack.joinable()) {
    soundtrack.join();
  }
  if (scaler.context != nullptr) {
    sws_freeContext(scaler.context);
  }
  av_frame_free(&frame);
  av_packet_free(&packet);
  avcodec_free_context(&codec);
  avformat_close_input(&format);
}

void decoder_main() {
  SharedVideo& video = shared_video();
  while (true) {
    std::string path;
    int quality = 0;
    uint64_t request = 0;
    {
      std::unique_lock<std::mutex> guard(video.mutex);
      video.condition.wait(guard, [&] {
        return video.stopping || video.path_changed || video.decode_again;
      });
      if (video.stopping && video.path.empty()) {
        return;
      }
      path = video.path;
      quality = video.quality;
      request = video.playback_request;
      video.path_changed = false;
      video.decode_again = false;
      video.cancel.store(true);
    }
    if (path.empty()) {
      continue;
    }
    video.cancel.store(false);
    decode_path(path, quality, request);
  }
}

void ensure_decoder() {
  std::thread& thread = decoder_thread();
  if (thread.joinable()) {
    return;
  }
  thread = std::thread(decoder_main);
}

}  // namespace

void video_replay(const std::string& path) {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.active = true;
    video.paused = false;
    video.seek_pending = false;
    video.position_milliseconds = 0;
    video.duration_milliseconds = 0;
    video.playback_request += 1;
    video.path = path;
    video.path_changed = true;
    video.pixels.clear();
    video.width = 0;
    video.height = 0;
    video.serial += 1;
    video.cancel.store(true);
    video.audio_cancel.store(true);
  }
  video.condition.notify_all();
  ensure_decoder();
}

void video_watch(const std::string& path) {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.active = true;
    if (video.path == path) {
      return;
    }
    video.quality = 0;
    video.paused = false;
    video.seek_pending = false;
    video.position_milliseconds = 0;
    video.duration_milliseconds = 0;
    video.playback_request += 1;
    video.path = path;
    video.path_changed = true;
    video.pixels.clear();
    video.width = 0;
    video.height = 0;
    video.serial += 1;
    video.cancel.store(true);
    video.audio_cancel.store(true);
  }
  video.condition.notify_all();
  ensure_decoder();
}

void video_play_full(const std::string& path) {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.active = true;
    video.quality = 1;
    video.paused = false;
    video.seek_pending = false;
    video.position_milliseconds = 0;
    video.duration_milliseconds = 0;
    video.playback_request += 1;
    video.path = path;
    video.path_changed = true;
    video.pixels.clear();
    video.width = 0;
    video.height = 0;
    video.serial += 1;
    video.cancel.store(true);
    video.audio_cancel.store(true);
  }
  video.condition.notify_all();
  ensure_decoder();
}

void video_clear() {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.active = false;
    video.quality = 0;
    video.paused = false;
    video.seek_pending = false;
    video.position_milliseconds = 0;
    video.duration_milliseconds = 0;
    video.playback_request += 1;
    video.path.clear();
    video.path_changed = true;
    video.pixels.clear();
    video.width = 0;
    video.height = 0;
    video.serial += 1;
    video.cancel.store(true);
    video.audio_cancel.store(true);
  }
  video.condition.notify_all();
}

void video_shutdown() {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.stopping = true;
    video.active = false;
    video.playback_request += 1;
    video.path.clear();
    video.path_changed = true;
    video.cancel.store(true);
    video.audio_cancel.store(true);
  }
  video.condition.notify_all();
  std::thread& thread = decoder_thread();
  if (thread.joinable()) {
    thread.join();
  }
}

bool video_is_active() {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.active;
}

bool video_is_full() {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.active && video.quality == 1;
}

void video_displayed_size(int32_t* width, int32_t* height) {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  if (video.active == false || video.path_changed || video.width < 1 || video.height < 1) {
    *width = 0;
    *height = 0;
    return;
  }
  *width = video.width;
  *height = video.height;
}

bool video_frame_pending(uint64_t serial) {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.active && video.serial != serial && video.width > 0;
}

bool video_take_frame(std::vector<uint8_t>* pixels, int32_t* width, int32_t* height, uint64_t* serial) {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  if (video.active == false || video.serial == *serial || video.width < 1) {
    return false;
  }
  *pixels = video.pixels;
  *width = video.width;
  *height = video.height;
  *serial = video.serial;
  return true;
}

namespace {

struct TransportMailbox {
  std::atomic<int32_t> borrowed{0};
  std::atomic<int32_t> step{0};
  std::atomic<int32_t> toggle{0};
  std::atomic<int32_t> seek_armed{0};
  std::atomic<int32_t> seek_milliseconds{0};
};

TransportMailbox& transport_mailbox() {
  static TransportMailbox mailbox;
  return mailbox;
}

int32_t clamp_milliseconds(int64_t value) {
  if (value < 0) {
    return 0;
  }
  if (value > 2147483647) {
    return 2147483647;
  }
  return static_cast<int32_t>(value);
}

}  // namespace

void video_toggle_paused() {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    if (video.active == false) {
      return;
    }
    video.paused = video.paused == false;
  }
  video.condition.notify_all();
}

void video_seek_milliseconds(int32_t milliseconds) {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    if (video.active == false) {
      return;
    }
    int64_t target = milliseconds;
    if (target < 0) {
      target = 0;
    }
    if (video.duration_milliseconds > 0 && target > video.duration_milliseconds) {
      target = video.duration_milliseconds;
    }
    video.seek_milliseconds = target;
    video.position_milliseconds = target;
    video.seek_pending = true;
    video.seek_serial.fetch_add(1);
  }
  video.condition.notify_all();
}

int32_t video_position_milliseconds() {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return clamp_milliseconds(video.position_milliseconds);
}

int32_t video_duration_milliseconds() {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return clamp_milliseconds(video.duration_milliseconds);
}

bool video_is_paused() {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.active && video.paused;
}

void video_transport_set_borrowed(int32_t borrowed) {
  transport_mailbox().borrowed.store(borrowed);
}

int32_t video_transport_borrowed() {
  return transport_mailbox().borrowed.load();
}

void video_transport_request_step(int32_t step) {
  transport_mailbox().step.fetch_add(step);
}

void video_transport_request_toggle() {
  transport_mailbox().toggle.store(1);
}

void video_transport_request_seek(int32_t milliseconds) {
  TransportMailbox& mailbox = transport_mailbox();
  mailbox.seek_milliseconds.store(milliseconds);
  mailbox.seek_armed.store(1);
}

int32_t video_transport_take_step() {
  return transport_mailbox().step.exchange(0);
}

int32_t video_transport_take_toggle() {
  return transport_mailbox().toggle.exchange(0);
}

int32_t video_transport_take_seek() {
  return transport_mailbox().seek_armed.exchange(0);
}

int32_t video_transport_seek_target() {
  return transport_mailbox().seek_milliseconds.load();
}

}  // namespace image_preview
}  // namespace mlc
