#include "mlc/gl/video_preview.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
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

bool same_request(const std::string& path) {
  SharedVideo& video = shared_video();
  std::lock_guard<std::mutex> guard(video.mutex);
  return video.stopping == false && video.path == path;
}

void fit_side(int32_t source_width, int32_t source_height, int32_t* width, int32_t* height) {
  int32_t next_width = source_width;
  int32_t next_height = source_height;
  if (next_width > k_long_side || next_height > k_long_side) {
    if (next_width >= next_height && next_width > 0) {
      next_height = next_height * k_long_side / next_width;
      next_width = k_long_side;
    } else if (next_height > 0) {
      next_width = next_width * k_long_side / next_height;
      next_height = k_long_side;
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

bool publish_scaled(AVFrame* frame, SwsContext** scaler, int32_t* scaler_width, int32_t* scaler_height) {
  av_frame_apply_cropping(frame, 0);
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
  fit_side(display_width, frame->height, &target_width, &target_height);
  if (*scaler == nullptr || *scaler_width != frame->width || *scaler_height != frame->height) {
    if (*scaler != nullptr) {
      sws_freeContext(*scaler);
    }
    *scaler = sws_getContext(
      frame->width,
      frame->height,
      static_cast<AVPixelFormat>(frame->format),
      target_width,
      target_height,
      AV_PIX_FMT_RGBA,
      SWS_FAST_BILINEAR,
      nullptr,
      nullptr,
      nullptr
    );
    *scaler_width = frame->width;
    *scaler_height = frame->height;
  }
  if (*scaler == nullptr) {
    return false;
  }
  std::vector<uint8_t> pixels(static_cast<size_t>(target_width) * static_cast<size_t>(target_height) * 4);
  uint8_t* destination_data[1] = {pixels.data()};
  int destination_stride[1] = {target_width * 4};
  sws_scale(*scaler, frame->data, frame->linesize, 0, frame->height, destination_data, destination_stride);
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    if (video.path_changed || video.stopping) {
      return false;
    }
    video.pixels.swap(pixels);
    video.width = target_width;
    video.height = target_height;
    video.serial += 1;
  }
  std::unique_lock<std::mutex> pause(video.mutex);
  video.condition.wait_for(pause, std::chrono::milliseconds(80), [&] {
    return video.path_changed || video.stopping;
  });
  return true;
}

bool receive_frames(AVCodecContext* codec, AVFrame* frame, SwsContext** scaler, int32_t* scaler_width, int32_t* scaler_height, const std::string& path) {
  while (same_request(path)) {
    const int received = avcodec_receive_frame(codec, frame);
    if (received == AVERROR(EAGAIN) || received == AVERROR_EOF) {
      return received != AVERROR_EOF;
    }
    if (received < 0) {
      return false;
    }
    publish_scaled(frame, scaler, scaler_width, scaler_height);
    av_frame_unref(frame);
  }
  return false;
}

void decode_path(const std::string& path) {
  SharedVideo& video = shared_video();
  video.cancel.store(false);
  AVFormatContext* format = nullptr;
  if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0) {
    return;
  }
  format->interrupt_callback.callback = interrupt_read;
  format->interrupt_callback.opaque = &video.cancel;
  if (avformat_find_stream_info(format, nullptr) < 0) {
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
  codec->thread_count = 1;
  if (avcodec_open2(codec, decoder, nullptr) < 0) {
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return;
  }
  AVPacket* packet = av_packet_alloc();
  AVFrame* frame = av_frame_alloc();
  SwsContext* scaler = nullptr;
  int32_t scaler_width = 0;
  int32_t scaler_height = 0;
  while (same_request(path) && packet != nullptr && frame != nullptr) {
    const int read = av_read_frame(format, packet);
    if (read == AVERROR_EOF) {
      avcodec_send_packet(codec, nullptr);
      receive_frames(codec, frame, &scaler, &scaler_width, &scaler_height, path);
      avcodec_flush_buffers(codec);
      if (av_seek_frame(format, stream_index, 0, AVSEEK_FLAG_BACKWARD) < 0) {
        break;
      }
      continue;
    }
    if (read < 0) {
      break;
    }
    if (packet->stream_index == stream_index) {
      if (avcodec_send_packet(codec, packet) == 0) {
        if (receive_frames(codec, frame, &scaler, &scaler_width, &scaler_height, path) == false) {
          av_packet_unref(packet);
          break;
        }
      }
    }
    av_packet_unref(packet);
  }
  if (scaler != nullptr) {
    sws_freeContext(scaler);
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
    {
      std::unique_lock<std::mutex> guard(video.mutex);
      video.condition.wait(guard, [&] { return video.stopping || video.path_changed; });
      if (video.stopping && video.path.empty()) {
        return;
      }
      path = video.path;
      video.path_changed = false;
      video.cancel.store(true);
    }
    if (path.empty()) {
      continue;
    }
    video.cancel.store(false);
    decode_path(path);
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

void video_watch(const std::string& path) {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.active = true;
    if (video.path == path && video.path_changed == false) {
      return;
    }
    video.path = path;
    video.path_changed = true;
    video.pixels.clear();
    video.width = 0;
    video.height = 0;
    video.serial += 1;
    video.cancel.store(true);
  }
  video.condition.notify_all();
  ensure_decoder();
}

void video_clear() {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.active = false;
    video.path.clear();
    video.path_changed = true;
    video.pixels.clear();
    video.width = 0;
    video.height = 0;
    video.serial += 1;
    video.cancel.store(true);
  }
  video.condition.notify_all();
}

void video_shutdown() {
  SharedVideo& video = shared_video();
  {
    std::lock_guard<std::mutex> guard(video.mutex);
    video.stopping = true;
    video.active = false;
    video.path.clear();
    video.path_changed = true;
    video.cancel.store(true);
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

}  // namespace image_preview
}  // namespace mlc
