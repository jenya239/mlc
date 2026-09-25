#include "mlc/gl/image_preview.hpp"
#include "mlc/gl/video_preview.hpp"
#include "mlc/audio/music_player.hpp"

#include "mlc/gl/glad_gl.hpp"

#include <png.h>
#include <jpeglib.h>
#include <webp/decode.h>

#include <csetjmp>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <new>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace mlc {
namespace image_preview {
namespace {

constexpr int32_t k_box = 1024;
constexpr int32_t k_slots = 8;
constexpr std::uintmax_t k_byte_limit = 8 * 1024 * 1024;

struct Slot {
  std::string path;
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t height = 0;
  int32_t source_width = 0;
  int32_t source_height = 0;
  bool failed = false;
  bool used = false;
};

int32_t decoded_source_width = 0;
int32_t decoded_source_height = 0;

Slot slots[k_slots];
int32_t current_slot = -1;
int32_t clock_hand = 0;

GLuint picture = 0;
GLuint program = 0;
GLuint buffer = 0;
bool paint_needed = false;
bool picture_armed = false;
int32_t armed_x = 0;
int32_t armed_y = 0;
int32_t armed_width = 0;
int32_t armed_height = 0;
int32_t armed_effect = 0;
int32_t armed_framebuffer = 0;
std::string shown_key;
bool video_mode = false;
std::string video_path;
std::vector<uint8_t> live_pixels;
int32_t live_width = 0;
int32_t live_height = 0;
uint64_t live_serial = 0;
uint64_t live_uploaded = 0;

bool video_extension(const std::string& path) {
  const auto slash = path.find_last_of('/');
  const std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
  std::string lower = name;
  for (char& character : lower) {
    if (character >= 'A' && character <= 'Z') {
      character = static_cast<char>(character - 'A' + 'a');
    }
  }
  const std::string suffixes[] = {".mp4", ".m4v", ".mov", ".mkv", ".webm", ".avi", ".ogv"};
  for (const std::string& suffix : suffixes) {
    if (lower.size() >= suffix.size() && lower.compare(lower.size() - suffix.size(), suffix.size(), suffix) == 0) {
      return true;
    }
  }
  return false;
}

int32_t fit_size(int32_t width, int32_t height, int32_t* out_width, int32_t* out_height) {
  if (width < 1 || height < 1) {
    return 0;
  }
  int32_t next_width = width;
  int32_t next_height = height;
  if (next_width > k_box || next_height > k_box) {
    if (next_width >= next_height) {
      next_height = next_height * k_box / next_width;
      next_width = k_box;
    } else {
      next_width = next_width * k_box / next_height;
      next_height = k_box;
    }
  }
  if (next_width < 1) {
    next_width = 1;
  }
  if (next_height < 1) {
    next_height = 1;
  }
  *out_width = next_width;
  *out_height = next_height;
  return 1;
}

std::vector<uint8_t> scale_rgba(
  const uint8_t* source,
  int32_t source_width,
  int32_t source_height,
  int32_t width,
  int32_t height
) {
  std::vector<uint8_t> pixels(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
  for (int32_t y = 0; y < height; y += 1) {
    const int32_t source_y = y * source_height / height;
    for (int32_t x = 0; x < width; x += 1) {
      const int32_t source_x = x * source_width / width;
      const size_t from = (static_cast<size_t>(source_y) * static_cast<size_t>(source_width) + static_cast<size_t>(source_x)) * 4;
      const size_t to = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)) * 4;
      pixels[to] = source[from];
      pixels[to + 1] = source[from + 1];
      pixels[to + 2] = source[from + 2];
      pixels[to + 3] = source[from + 3];
    }
  }
  return pixels;
}

bool decode_png(const std::string& path, std::vector<uint8_t>* pixels, int32_t* width, int32_t* height) {
  FILE* file = std::fopen(path.c_str(), "rb");
  if (file == nullptr) {
    return false;
  }
  png_structp reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (reader == nullptr) {
    std::fclose(file);
    return false;
  }
  png_infop info = png_create_info_struct(reader);
  if (info == nullptr) {
    png_destroy_read_struct(&reader, nullptr, nullptr);
    std::fclose(file);
    return false;
  }
  if (setjmp(png_jmpbuf(reader))) {
    png_destroy_read_struct(&reader, &info, nullptr);
    std::fclose(file);
    return false;
  }
  png_init_io(reader, file);
  png_read_info(reader, info);
  const png_uint_32 image_width = png_get_image_width(reader, info);
  const png_uint_32 image_height = png_get_image_height(reader, info);
  png_set_expand(reader);
  png_set_strip_16(reader);
  png_set_gray_to_rgb(reader);
  png_set_filler(reader, 0xff, PNG_FILLER_AFTER);
  png_read_update_info(reader, info);
  if (image_width == 0 || image_height == 0 || image_width > 16000 || image_height > 16000) {
    png_destroy_read_struct(&reader, &info, nullptr);
    std::fclose(file);
    return false;
  }
  std::vector<uint8_t> rgba(static_cast<size_t>(image_width) * static_cast<size_t>(image_height) * 4);
  std::vector<png_bytep> rows(image_height);
  for (png_uint_32 row = 0; row < image_height; row += 1) {
    rows[row] = rgba.data() + static_cast<size_t>(row) * static_cast<size_t>(image_width) * 4;
  }
  png_read_image(reader, rows.data());
  png_destroy_read_struct(&reader, &info, nullptr);
  std::fclose(file);
  decoded_source_width = static_cast<int32_t>(image_width);
  decoded_source_height = static_cast<int32_t>(image_height);
  *width = static_cast<int32_t>(image_width);
  *height = static_cast<int32_t>(image_height);
  *pixels = std::move(rgba);
  return true;
}

struct JpegGuard {
  jpeg_error_mgr base;
  jmp_buf jump;
};

void jpeg_guard_exit(j_common_ptr info) {
  JpegGuard* guard = reinterpret_cast<JpegGuard*>(info->err);
  std::longjmp(guard->jump, 1);
}

void jpeg_guard_quiet(j_common_ptr) {}

bool decode_jpeg(const std::string& path, std::vector<uint8_t>* pixels, int32_t* width, int32_t* height) {
  FILE* file = std::fopen(path.c_str(), "rb");
  if (file == nullptr) {
    return false;
  }
  jpeg_decompress_struct info;
  JpegGuard guard;
  info.err = jpeg_std_error(&guard.base);
  guard.base.error_exit = jpeg_guard_exit;
  guard.base.output_message = jpeg_guard_quiet;
  if (setjmp(guard.jump)) {
    jpeg_destroy_decompress(&info);
    std::fclose(file);
    return false;
  }
  jpeg_create_decompress(&info);
  jpeg_stdio_src(&info, file);
  jpeg_read_header(&info, TRUE);
  decoded_source_width = static_cast<int32_t>(info.image_width);
  decoded_source_height = static_cast<int32_t>(info.image_height);
  info.out_color_space = JCS_RGB;
  info.scale_num = 1;
  info.scale_denom = 1;
  while (
    info.scale_denom < 8 &&
    (info.image_width / info.scale_denom > 1024 || info.image_height / info.scale_denom > 1024)
  ) {
    info.scale_denom *= 2;
  }
  jpeg_start_decompress(&info);
  if (
    info.output_width == 0 || info.output_height == 0 ||
    info.output_width > 4096 || info.output_height > 4096 ||
    info.output_components != 3
  ) {
    jpeg_abort_decompress(&info);
    jpeg_destroy_decompress(&info);
    std::fclose(file);
    return false;
  }
  const int32_t image_width = static_cast<int32_t>(info.output_width);
  const int32_t image_height = static_cast<int32_t>(info.output_height);
  std::vector<uint8_t> rgb(static_cast<size_t>(image_width) * static_cast<size_t>(image_height) * 3);
  while (info.output_scanline < info.output_height) {
    uint8_t* row = rgb.data() + static_cast<size_t>(info.output_scanline) * static_cast<size_t>(image_width) * 3;
    jpeg_read_scanlines(&info, &row, 1);
  }
  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);
  std::fclose(file);
  std::vector<uint8_t> rgba(static_cast<size_t>(image_width) * static_cast<size_t>(image_height) * 4);
  for (size_t pixel = 0; pixel < static_cast<size_t>(image_width) * static_cast<size_t>(image_height); pixel += 1) {
    rgba[pixel * 4] = rgb[pixel * 3];
    rgba[pixel * 4 + 1] = rgb[pixel * 3 + 1];
    rgba[pixel * 4 + 2] = rgb[pixel * 3 + 2];
    rgba[pixel * 4 + 3] = 255;
  }
  *width = image_width;
  *height = image_height;
  *pixels = std::move(rgba);
  return true;
}

bool decode_webp(const std::string& path, std::vector<uint8_t>* pixels, int32_t* width, int32_t* height) {
  FILE* file = std::fopen(path.c_str(), "rb");
  if (file == nullptr) {
    return false;
  }
  std::fseek(file, 0, SEEK_END);
  const long length = std::ftell(file);
  std::fseek(file, 0, SEEK_SET);
  if (length <= 0 || static_cast<std::uintmax_t>(length) > k_byte_limit) {
    std::fclose(file);
    return false;
  }
  std::vector<uint8_t> encoded(static_cast<size_t>(length));
  if (std::fread(encoded.data(), 1, encoded.size(), file) != encoded.size()) {
    std::fclose(file);
    return false;
  }
  std::fclose(file);
  int image_width = 0;
  int image_height = 0;
  uint8_t* decoded = WebPDecodeRGBA(encoded.data(), encoded.size(), &image_width, &image_height);
  if (decoded == nullptr) {
    return false;
  }
  decoded_source_width = image_width;
  decoded_source_height = image_height;
  *width = image_width;
  *height = image_height;
  pixels->assign(decoded, decoded + static_cast<size_t>(image_width) * static_cast<size_t>(image_height) * 4);
  WebPFree(decoded);
  return true;
}

bool path_ends_with(const std::string& path, const char* suffix) {
  const size_t suffix_length = std::strlen(suffix);
  if (path.size() < suffix_length) {
    return false;
  }
  for (size_t index = 0; index < suffix_length; index += 1) {
    unsigned char left = static_cast<unsigned char>(path[path.size() - suffix_length + index]);
    unsigned char right = static_cast<unsigned char>(suffix[index]);
    if (left >= 'A' && left <= 'Z') {
      left = static_cast<unsigned char>(left - 'A' + 'a');
    }
    if (right >= 'A' && right <= 'Z') {
      right = static_cast<unsigned char>(right - 'A' + 'a');
    }
    if (left != right) {
      return false;
    }
  }
  return true;
}

bool decode_file(const std::string& path, std::vector<uint8_t>* pixels, int32_t* width, int32_t* height) {
  std::error_code error;
  const auto size = std::filesystem::file_size(path, error);
  if (error || size > k_byte_limit) {
    return false;
  }
  try {
    if (path_ends_with(path, ".png")) {
      return decode_png(path, pixels, width, height);
    }
    if (path_ends_with(path, ".jpg") || path_ends_with(path, ".jpeg")) {
      return decode_jpeg(path, pixels, width, height);
    }
    if (path_ends_with(path, ".webp")) {
      return decode_webp(path, pixels, width, height);
    }
    return false;
  } catch (const std::bad_alloc&) {
    return false;
  }
}

int32_t find_slot(const std::string& path) {
  for (int32_t index = 0; index < k_slots; index += 1) {
    if (slots[index].used && slots[index].path == path) {
      return index;
    }
  }
  return -1;
}

int32_t claim_slot() {
  for (int32_t index = 0; index < k_slots; index += 1) {
    if (slots[index].used == false) {
      return index;
    }
  }
  const int32_t index = clock_hand;
  clock_hand = (clock_hand + 1) % k_slots;
  return index;
}

GLuint compile_shader(GLenum kind, const char* source) {
  const GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  GLint status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

void ensure_gl() {
  if (program != 0) {
    return;
  }
  const char* vertex =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "  v_uv = a_uv;\n"
    "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";
  const char* fragment =
    "varying vec2 v_uv;\n"
    "uniform sampler2D u_picture;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(texture2D(u_picture, v_uv).rgb, 1.0);\n"
    "}\n";
  const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex);
  const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment);
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glGenTextures(1, &picture);
  glBindTexture(GL_TEXTURE_2D, picture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glGenBuffers(1, &buffer);
}

void upload_rgba(const uint8_t* pixels, int32_t width, int32_t height) {
  glBindTexture(GL_TEXTURE_2D, picture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, pixels
  );
}

void upload_picture() {
  if (current_slot < 0) {
    return;
  }
  const Slot& slot = slots[current_slot];
  upload_rgba(slot.pixels.data(), slot.width, slot.height);
}

void draw_quad(int32_t left, int32_t bottom, int32_t right, int32_t top, int32_t window_width, int32_t window_height) {
  const float x0 = static_cast<float>(left) / static_cast<float>(window_width) * 2.0f - 1.0f;
  const float x1 = static_cast<float>(right) / static_cast<float>(window_width) * 2.0f - 1.0f;
  const float y0 = static_cast<float>(bottom) / static_cast<float>(window_height) * 2.0f - 1.0f;
  const float y1 = static_cast<float>(top) / static_cast<float>(window_height) * 2.0f - 1.0f;
  const float vertices[] = {
    x0, y0, 0.0f, 1.0f,
    x1, y0, 1.0f, 1.0f,
    x0, y1, 0.0f, 0.0f,
    x0, y1, 0.0f, 0.0f,
    x1, y0, 1.0f, 1.0f,
    x1, y1, 1.0f, 0.0f
  };
  glUseProgram(program);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
  const GLint position = glGetAttribLocation(program, "a_pos");
  const GLint uv = glGetAttribLocation(program, "a_uv");
  glEnableVertexAttribArray(static_cast<GLuint>(position));
  glEnableVertexAttribArray(static_cast<GLuint>(uv));
  glVertexAttribPointer(static_cast<GLuint>(position), 2, GL_FLOAT, GL_FALSE, 16, reinterpret_cast<void*>(0));
  glVertexAttribPointer(static_cast<GLuint>(uv), 2, GL_FLOAT, GL_FALSE, 16, reinterpret_cast<void*>(8));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, picture);
  glUniform1i(glGetUniformLocation(program, "u_picture"), 0);
  glDisable(GL_BLEND);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void bind_draw(int32_t framebuffer) {
  if (framebuffer > 0) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(framebuffer));
    return;
  }
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

struct SharedStill {
  std::mutex mutex;
  std::condition_variable condition;
  std::string path;
  bool stopping = false;
  bool path_changed = false;
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t height = 0;
  int32_t source_width = 0;
  int32_t source_height = 0;
  bool failed = false;
  uint64_t serial = 0;
};

SharedStill& shared_still() {
  static SharedStill still;
  return still;
}

std::thread& still_thread() {
  static std::thread thread;
  return thread;
}

uint64_t still_seen = 0;

void still_main() {
  SharedStill& still = shared_still();
  while (true) {
    std::string path;
    {
      std::unique_lock<std::mutex> guard(still.mutex);
      still.condition.wait(guard, [&] { return still.stopping || still.path_changed; });
      if (still.stopping && still.path.empty()) {
        return;
      }
      path = still.path;
      still.width = 0;
      still.height = 0;
      still.source_width = 0;
      still.source_height = 0;
      still.path_changed = false;
    }
    if (path.empty()) {
      continue;
    }
    std::vector<uint8_t> decoded;
    int32_t decoded_width = 0;
    int32_t decoded_height = 0;
    int32_t fitted_width = 0;
    int32_t fitted_height = 0;
    std::vector<uint8_t> fitted;
    const bool decoded_ok = decode_file(path, &decoded, &decoded_width, &decoded_height)
      && fit_size(decoded_width, decoded_height, &fitted_width, &fitted_height) != 0;
    if (decoded_ok) {
      fitted = scale_rgba(decoded.data(), decoded_width, decoded_height, fitted_width, fitted_height);
    }
    std::lock_guard<std::mutex> guard(still.mutex);
    if (still.path != path || still.path_changed) {
      continue;
    }
    still.failed = decoded_ok == false || fitted.empty();
    still.width = still.failed ? 0 : fitted_width;
    still.height = still.failed ? 0 : fitted_height;
    still.source_width = still.failed ? 0 : decoded_source_width;
    still.source_height = still.failed ? 0 : decoded_source_height;
    still.pixels = std::move(fitted);
    still.serial += 1;
  }
}

void ensure_still() {
  std::thread& thread = still_thread();
  if (thread.joinable()) {
    return;
  }
  thread = std::thread(still_main);
}

void still_watch(const std::string& path) {
  SharedStill& still = shared_still();
  {
    std::lock_guard<std::mutex> guard(still.mutex);
    if (still.path == path) {
      return;
    }
    still.path = path;
    still.path_changed = true;
    still.failed = false;
  }
  still.condition.notify_all();
  ensure_still();
}

int32_t picture_edge(bool width_edge, bool source_edge) {
  if (video_mode) {
    int32_t displayed_width = 0;
    int32_t displayed_height = 0;
    video_displayed_size(&displayed_width, &displayed_height);
    if (width_edge) {
      return displayed_width;
    }
    return displayed_height;
  }
  if (current_slot >= 0 && slots[current_slot].failed == false && slots[current_slot].width > 0) {
    if (source_edge && slots[current_slot].source_width > 0) {
      if (width_edge) {
        return slots[current_slot].source_width;
      }
      return slots[current_slot].source_height;
    }
    if (width_edge) {
      return slots[current_slot].width;
    }
    return slots[current_slot].height;
  }
  SharedStill& still = shared_still();
  std::lock_guard<std::mutex> guard(still.mutex);
  if (still.path != shown_key || still.path_changed || still.failed) {
    return 0;
  }
  if (source_edge && still.source_width > 0) {
    if (width_edge) {
      return still.source_width;
    }
    return still.source_height;
  }
  if (width_edge) {
    return still.width;
  }
  return still.height;
}

bool still_pending() {
  SharedStill& still = shared_still();
  std::lock_guard<std::mutex> guard(still.mutex);
  return still.serial != still_seen && still.path_changed == false;
}

void still_publish() {
  SharedStill& still = shared_still();
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t height = 0;
  int32_t source_width = 0;
  int32_t source_height = 0;
  bool failed = false;
  std::string path;
  {
    std::lock_guard<std::mutex> guard(still.mutex);
    if (still.serial == still_seen || still.path_changed) {
      return;
    }
    pixels = still.pixels;
    width = still.width;
    height = still.height;
    source_width = still.source_width;
    source_height = still.source_height;
    failed = still.failed;
    path = still.path;
    still_seen = still.serial;
  }
  if (path != shown_key) {
    return;
  }
  const int32_t index = claim_slot();
  slots[index].used = true;
  slots[index].path = path;
  slots[index].pixels = std::move(pixels);
  slots[index].width = width;
  slots[index].height = height;
  slots[index].source_width = source_width;
  slots[index].source_height = source_height;
  slots[index].failed = failed;
  current_slot = index;
  if (failed == false && width > 0) {
    paint_needed = true;
  }
}

void still_shutdown() {
  SharedStill& still = shared_still();
  {
    std::lock_guard<std::mutex> guard(still.mutex);
    still.stopping = true;
    still.path.clear();
    still.path_changed = true;
  }
  still.condition.notify_all();
  std::thread& thread = still_thread();
  if (thread.joinable()) {
    thread.join();
  }
}

GLuint bars_program = 0;
GLuint bars_buffer = 0;
int32_t bars_x = 0;
int32_t bars_y = 0;
int32_t bars_width = 0;
int32_t bars_height = 0;
int32_t bars_framebuffer = 0;

void ensure_bars() {
  if (bars_program != 0) {
    return;
  }
  const char* vertex =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "  v_uv = a_uv;\n"
    "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";
  const char* fragment =
    "varying vec2 v_uv;\n"
    "uniform float u_level[128];\n"
    "void main() {\n"
    "  float index = floor(min(v_uv.x, 0.999) * 128.0);\n"
    "  float level = u_level[int(index)];\n"
    "  float column = fract(v_uv.x * 128.0);\n"
    "  float inside = step(0.12, column) * step(column, 0.88);\n"
    "  float on = step(v_uv.y, level) * inside;\n"
    "  vec3 color = mix(vec3(0.07, 0.08, 0.10), vec3(0.35, 0.62, 0.95), on);\n"
    "  gl_FragColor = vec4(color, 1.0);\n"
    "}\n";
  const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex);
  const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment);
  bars_program = glCreateProgram();
  glAttachShader(bars_program, vertex_shader);
  glAttachShader(bars_program, fragment_shader);
  glLinkProgram(bars_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glGenBuffers(1, &bars_buffer);
}

}  // namespace

int32_t load_value(mlc::String path) {
  const std::string key = path.as_std_string();
  if (key != shown_key) {
    shown_key = key;
    paint_needed = true;
  }
  if (key.empty()) {
    video_clear();
    video_mode = false;
    video_path.clear();
    live_width = 0;
    current_slot = -1;
    return 0;
  }
  if (video_extension(key)) {
    current_slot = -1;
    video_mode = true;
    if (video_path != key) {
      video_path = key;
      live_width = 0;
      live_height = 0;
      live_uploaded = 0;
    }
    video_watch(key);
    return 1;
  }
  video_clear();
  video_mode = false;
  video_path.clear();
  live_width = 0;
  live_height = 0;
  const int32_t existing = find_slot(key);
  if (existing >= 0) {
    current_slot = existing;
    if (slots[existing].failed) {
      return 0;
    }
    return 1;
  }
  current_slot = -1;
  still_watch(key);
  return 0;
}

int32_t present_value(
  int32_t dest_x,
  int32_t dest_y,
  int32_t dest_width,
  int32_t dest_height,
  int32_t window_width,
  int32_t window_height,
  int32_t effect_mode,
  int32_t scene_framebuffer
) {
  if (effect_mode == 0 || scene_framebuffer <= 0) {
    scene_framebuffer = 0;
  }
  if (window_width < 1 || window_height < 1) {
    return 0;
  }
  ensure_gl();
  if (program == 0) {
    return 0;
  }
  if (video_mode == false) {
    still_publish();
  }
  const bool fresh_video = video_mode && video_take_frame(&live_pixels, &live_width, &live_height, &live_serial);
  const bool ready = video_mode
    ? live_width > 0
    : current_slot >= 0 && slots[current_slot].failed == false && slots[current_slot].width > 0;
  if (ready == false || dest_width < 1 || dest_height < 1) {
    if (fresh_video || paint_needed) {
      paint_needed = true;
    }
    return 0;
  }
  const int32_t shown_width = video_mode ? live_width : slots[current_slot].source_width > 0 ? slots[current_slot].source_width : slots[current_slot].width;
  const int32_t shown_height = video_mode ? live_height : slots[current_slot].source_height > 0 ? slots[current_slot].source_height : slots[current_slot].height;
  const int64_t width_span = static_cast<int64_t>(dest_width) * shown_height;
  const int64_t height_span = static_cast<int64_t>(dest_height) * shown_width;
  const int64_t span_difference = width_span > height_span ? width_span - height_span : height_span - width_span;
  const int64_t span_tolerance = shown_width > shown_height ? shown_width : shown_height;
  if (shown_width < 1 || shown_height < 1 || span_difference > span_tolerance) {
    paint_needed = true;
    return 0;
  }
  bind_draw(scene_framebuffer);
  glDisable(GL_BLEND);
  glDisable(GL_SCISSOR_TEST);
  glViewport(0, 0, window_width, window_height);
  const int32_t left = dest_x;
  const int32_t right = dest_x + dest_width;
  const int32_t top = window_height - dest_y;
  const int32_t bottom = top - dest_height;
  if (right - left < 1 || top - bottom < 1) {
    return 0;
  }
  bind_draw(scene_framebuffer);
  if (video_mode) {
    if (live_serial != live_uploaded && live_width > 0 && live_pixels.empty() == false) {
      upload_rgba(live_pixels.data(), live_width, live_height);
      live_uploaded = live_serial;
    }
  } else {
    upload_picture();
  }
  const int32_t scissor_bottom = window_height - (dest_y + dest_height);
  glEnable(GL_SCISSOR_TEST);
  glScissor(dest_x, scissor_bottom, dest_width, dest_height);
  draw_quad(left, bottom, right, top, window_width, window_height);
  glDisable(GL_SCISSOR_TEST);
  paint_needed = false;
  return 1;
}

void arm_value(
  int32_t dest_x,
  int32_t dest_y,
  int32_t dest_width,
  int32_t dest_height,
  int32_t effect_mode,
  int32_t scene_framebuffer
) {
  if (dest_width < 1 || dest_height < 1) {
    if (picture_armed) {
      paint_needed = true;
    }
    picture_armed = false;
    return;
  }
  picture_armed = true;
  armed_x = dest_x;
  armed_y = dest_y;
  armed_width = dest_width;
  armed_height = dest_height;
  armed_effect = effect_mode;
  armed_framebuffer = scene_framebuffer;
}

void bars_arm_value(
  int32_t dest_x,
  int32_t dest_y,
  int32_t dest_width,
  int32_t dest_height,
  int32_t scene_framebuffer
) {
  bars_x = dest_x;
  bars_y = dest_y;
  bars_width = dest_width;
  bars_height = dest_height;
  bars_framebuffer = scene_framebuffer;
}

void bars_draw_value(int32_t window_width, int32_t window_height) {
  if (bars_width < 8 || bars_height < 4 || window_width < 1 || window_height < 1) {
    return;
  }
  ensure_bars();
  if (bars_program == 0) {
    return;
  }
  bind_draw(bars_framebuffer);
  glViewport(0, 0, window_width, window_height);
  const float x0 = static_cast<float>(bars_x) / static_cast<float>(window_width) * 2.0f - 1.0f;
  const float x1 = static_cast<float>(bars_x + bars_width) / static_cast<float>(window_width) * 2.0f - 1.0f;
  const float y1 = static_cast<float>(window_height - bars_y) / static_cast<float>(window_height) * 2.0f - 1.0f;
  const float y0 = static_cast<float>(window_height - (bars_y + bars_height)) / static_cast<float>(window_height) * 2.0f - 1.0f;
  const float vertices[] = {
    x0, y0, 0.0f, 0.0f, x1, y0, 1.0f, 0.0f, x0, y1, 0.0f, 1.0f,
    x0, y1, 0.0f, 1.0f, x1, y0, 1.0f, 0.0f, x1, y1, 1.0f, 1.0f
  };
  glUseProgram(bars_program);
  glBindBuffer(GL_ARRAY_BUFFER, bars_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
  const GLint position = glGetAttribLocation(bars_program, "a_pos");
  const GLint uv = glGetAttribLocation(bars_program, "a_uv");
  glEnableVertexAttribArray(static_cast<GLuint>(position));
  glEnableVertexAttribArray(static_cast<GLuint>(uv));
  glVertexAttribPointer(static_cast<GLuint>(position), 2, GL_FLOAT, GL_FALSE, 16, reinterpret_cast<void*>(0));
  glVertexAttribPointer(static_cast<GLuint>(uv), 2, GL_FLOAT, GL_FALSE, 16, reinterpret_cast<void*>(8));
  float levels[128];
  for (int32_t band = 0; band < 128; band += 1) {
    levels[band] = static_cast<float>(mlc::music_player::band_level_value(band)) / 255.0f;
  }
  glUniform1fv(glGetUniformLocation(bars_program, "u_level"), 128, levels);
  const int32_t scissor_bottom = window_height - (bars_y + bars_height);
  glEnable(GL_SCISSOR_TEST);
  glScissor(bars_x, scissor_bottom, bars_width, bars_height);
  glDisable(GL_BLEND);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisable(GL_SCISSOR_TEST);
}

void draw_background_value(int32_t window_width, int32_t window_height, int32_t full_paint) {
  if (full_paint == 0 || picture_armed == false) {
    return;
  }
  present_value(
    armed_x, armed_y, armed_width, armed_height,
    window_width, window_height, armed_effect, armed_framebuffer
  );
}

int32_t needs_paint_value() {
  if (video_mode && video_frame_pending(live_serial)) {
    return 1;
  }
  if (video_mode == false && still_pending()) {
    return 1;
  }
  return paint_needed ? 1 : 0;
}

int32_t background_value() {
  return picture_armed ? 1 : 0;
}

int32_t picture_width_value() {
  return picture_edge(true, true);
}

int32_t picture_height_value() {
  return picture_edge(false, true);
}

int32_t picture_fit_width_value() {
  return picture_edge(true, false);
}

int32_t picture_fit_height_value() {
  return picture_edge(false, false);
}

int32_t playing_value() {
  return video_is_active() ? 1 : 0;
}

void discard() {
  still_shutdown();
  video_shutdown();
  video_mode = false;
  if (program != 0) {
    glDeleteProgram(program);
    glDeleteTextures(1, &picture);
    glDeleteBuffers(1, &buffer);
  }
  program = 0;
  picture = 0;
  buffer = 0;
  current_slot = -1;
}

bool write_sample_png_value(mlc::String path, int32_t seed) {
  FILE* file = std::fopen(path.as_std_string().c_str(), "wb");
  if (file == nullptr) {
    return false;
  }
  png_structp writer = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (writer == nullptr) {
    std::fclose(file);
    return false;
  }
  png_infop info = png_create_info_struct(writer);
  if (info == nullptr || setjmp(png_jmpbuf(writer))) {
    png_destroy_write_struct(&writer, info == nullptr ? nullptr : &info);
    std::fclose(file);
    return false;
  }
  constexpr int32_t width = 160;
  constexpr int32_t height = 120;
  png_init_io(writer, file);
  png_set_IHDR(
    writer, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
  );
  png_write_info(writer, info);
  std::vector<uint8_t> row(static_cast<size_t>(width) * 3);
  for (int32_t y = 0; y < height; y += 1) {
    for (int32_t x = 0; x < width; x += 1) {
      row[static_cast<size_t>(x) * 3] = static_cast<uint8_t>((x * (2 + seed) + seed * 40) & 255);
      row[static_cast<size_t>(x) * 3 + 1] = static_cast<uint8_t>((y * (3 + seed)) & 255);
      row[static_cast<size_t>(x) * 3 + 2] = static_cast<uint8_t>((x + y * seed) & 255);
    }
    png_write_row(writer, row.data());
  }
  png_write_end(writer, nullptr);
  png_destroy_write_struct(&writer, &info);
  std::fclose(file);
  return true;
}

}  // namespace image_preview
}  // namespace mlc
