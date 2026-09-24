#include "mlc/gl/image_preview.hpp"

#include "mlc/gl/glad_gl.hpp"

#if __has_include(<GLFW/glfw3.h>)
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#define MLC_PREVIEW_HAS_GLFW 1
#else
#define MLC_PREVIEW_HAS_GLFW 0
#endif

#include <png.h>
#include <jpeglib.h>
#include <webp/decode.h>

#include <csetjmp>
#include <cstdint>
#include <new>
#include <cstring>
#include <filesystem>
#include <string>
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
  bool failed = false;
  bool used = false;
};

Slot slots[k_slots];
int32_t current_slot = -1;
int32_t clock_hand = 0;

GLuint picture = 0;
GLuint backup = 0;
GLuint program = 0;
GLuint buffer = 0;
GLint picture_size[2] = {0, 0};
int32_t shown_left = 0;
int32_t shown_bottom = 0;
int32_t shown_right = 0;
int32_t shown_top = 0;
int32_t shown_pointer_x = -1;
int32_t shown_pointer_y = -1;
bool shown = false;

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
    "uniform float u_plain;\n"
    "uniform float u_mode;\n"
    "uniform float u_time;\n"
    "void main() {\n"
    "  if (u_plain > 0.5) {\n"
    "    gl_FragColor = vec4(texture2D(u_picture, v_uv).rgb, 1.0);\n"
    "    return;\n"
    "  }\n"
    "  vec2 centered = v_uv - vec2(0.5);\n"
    "  float radius = length(centered);\n"
    "  float mask = clamp(1.0 - radius * radius * 4.0, 0.0, 1.0);\n"
    "  mask = mask * mask;\n"
    "  vec3 color = texture2D(u_picture, v_uv).rgb;\n"
    "  if (u_mode > 0.5 && u_mode < 1.5) {\n"
    "    vec2 sample_uv = v_uv - centered * mask * 0.18;\n"
    "    color = texture2D(u_picture, sample_uv).rgb;\n"
    "    float pulse = 0.72 + 0.28 * sin(u_time * 1.7);\n"
    "    color += vec3(0.22, 0.4, 0.85) * mask * pulse;\n"
    "  }\n"
    "  if (u_mode > 2.5) {\n"
    "    vec2 seed = vec2(0.3 * sin(u_time * 0.4), 0.3 * cos(u_time * 0.33));\n"
    "    vec2 z = (v_uv - vec2(0.5)) * 2.2;\n"
    "    float shade = 0.0;\n"
    "    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + seed;\n"
    "    shade += step(dot(z, z), 4.0);\n"
    "    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + seed;\n"
    "    shade += step(dot(z, z), 4.0);\n"
    "    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + seed;\n"
    "    shade += step(dot(z, z), 4.0);\n"
    "    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + seed;\n"
    "    shade += step(dot(z, z), 4.0);\n"
    "    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + seed;\n"
    "    shade += step(dot(z, z), 4.0);\n"
    "    vec3 fractal = mix(vec3(0.04, 0.07, 0.14), vec3(0.4, 0.65, 1.0), shade * 0.2);\n"
    "    fractal += vec3(0.8, 0.22, 0.5) * shade * 0.08;\n"
    "    color = mix(color, fractal, 0.78);\n"
    "  }\n"
    "  if (u_mode > 1.5 && u_mode < 2.5) {\n"
    "    float band = floor(v_uv.y * 80.0);\n"
    "    float glitch = step(0.97, fract(sin(band + u_time * 9.0) * 43758.5)) * 0.045;\n"
    "    float scan = 0.72 + 0.28 * step(0.5, fract(v_uv.y * 48.0));\n"
    "    color = texture2D(u_picture, v_uv - vec2(glitch, 0.0)).rgb * scan;\n"
    "    color.r += 0.28 * mask;\n"
    "    color.b += 0.55 * mask;\n"
    "  }\n"
    "  float round_mask = smoothstep(0.56, 0.42, max(abs(centered.x), abs(centered.y)));\n"
    "  gl_FragColor = vec4(color, 0.84 * round_mask);\n"
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
  glGenTextures(1, &backup);
  glBindTexture(GL_TEXTURE_2D, picture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, backup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, k_box, k_box, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glGenBuffers(1, &buffer);
}

void upload_picture() {
  if (current_slot < 0) {
    return;
  }
  const Slot& slot = slots[current_slot];
  glBindTexture(GL_TEXTURE_2D, picture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA8, slot.width, slot.height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, slot.pixels.data()
  );
  picture_size[0] = slot.width;
  picture_size[1] = slot.height;
}

void draw_quad(GLuint texture, int32_t left, int32_t bottom, int32_t right, int32_t top, int32_t window_width, int32_t window_height, bool image, int32_t effect_mode, float time_seconds) {
  const float x0 = static_cast<float>(left) / static_cast<float>(window_width) * 2.0f - 1.0f;
  const float x1 = static_cast<float>(right) / static_cast<float>(window_width) * 2.0f - 1.0f;
  const float y0 = static_cast<float>(bottom) / static_cast<float>(window_height) * 2.0f - 1.0f;
  const float y1 = static_cast<float>(top) / static_cast<float>(window_height) * 2.0f - 1.0f;
  const float u1 = image ? 1.0f : static_cast<float>(right - left) / static_cast<float>(k_box);
  const float v1 = image ? 1.0f : static_cast<float>(top - bottom) / static_cast<float>(k_box);
  const float v_bottom = image ? v1 : 0.0f;
  const float v_top = image ? 0.0f : v1;
  const float vertices[] = {
    x0, y0, 0.0f, v_bottom,
    x1, y0, u1, v_bottom,
    x0, y1, 0.0f, v_top,
    x0, y1, 0.0f, v_top,
    x1, y0, u1, v_bottom,
    x1, y1, u1, v_top
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
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(program, "u_picture"), 0);
  glUniform1f(glGetUniformLocation(program, "u_plain"), image ? 0.0f : 1.0f);
  glUniform1f(glGetUniformLocation(program, "u_mode"), static_cast<float>(effect_mode));
  glUniform1f(glGetUniformLocation(program, "u_time"), time_seconds);
  if (image) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else {
    glDisable(GL_BLEND);
  }
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisable(GL_BLEND);
}

void restore_shown(int32_t window_width, int32_t window_height, int32_t scene_framebuffer) {
  if (shown == false) {
    return;
  }
  if (scene_framebuffer > 0) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(scene_framebuffer));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
      shown_left, shown_bottom, shown_right, shown_top,
      shown_left, shown_bottom, shown_right, shown_top,
      GL_COLOR_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  } else {
    draw_quad(backup, shown_left, shown_bottom, shown_right, shown_top, window_width, window_height, false, 0, 0.0f);
  }
  shown = false;
}

void capture_rect(int32_t left, int32_t bottom, int32_t right, int32_t top) {
  glBindTexture(GL_TEXTURE_2D, backup);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, left, bottom, right - left, top - bottom);
}

}  // namespace

int32_t load_value(mlc::String path) {
  const std::string key = path.as_std_string();
  if (key.empty()) {
    current_slot = -1;
    return 0;
  }
  const int32_t existing = find_slot(key);
  if (existing >= 0) {
    current_slot = existing;
    if (slots[existing].failed) {
      return 0;
    }
    return 1;
  }
  const int32_t index = claim_slot();
  slots[index].used = true;
  slots[index].path = key;
  slots[index].pixels.clear();
  slots[index].width = 0;
  slots[index].height = 0;
  slots[index].failed = true;
  current_slot = index;
  std::vector<uint8_t> decoded;
  int32_t decoded_width = 0;
  int32_t decoded_height = 0;
  if (decode_file(key, &decoded, &decoded_width, &decoded_height) == false) {
    return 0;
  }
  int32_t fitted_width = 0;
  int32_t fitted_height = 0;
  if (fit_size(decoded_width, decoded_height, &fitted_width, &fitted_height) == 0) {
    return 0;
  }
  slots[index].pixels = scale_rgba(decoded.data(), decoded_width, decoded_height, fitted_width, fitted_height);
  slots[index].width = fitted_width;
  slots[index].height = fitted_height;
  slots[index].failed = false;
  return 1;
}

float effect_time() {
#if MLC_PREVIEW_HAS_GLFW
  return static_cast<float>(glfwGetTime());
#else
  return 0.0f;
#endif
}

int32_t present_value(
  int32_t pointer_x,
  int32_t pointer_y,
  int32_t window_width,
  int32_t window_height,
  int32_t clean_window,
  int32_t effect_mode,
  int32_t scene_framebuffer
) {
  if (effect_mode == 0) {
    scene_framebuffer = 0;
  }
  if (window_width < 1 || window_height < 1) {
    return 0;
  }
  ensure_gl();
  if (program == 0) {
    return 0;
  }
  const bool ready = current_slot >= 0 && slots[current_slot].failed == false && slots[current_slot].width > 0;
  if (ready == false) {
    if (shown == false || clean_window != 0) {
      shown = false;
      return 0;
    }
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, window_width, window_height);
    restore_shown(window_width, window_height, scene_framebuffer);
    return 1;
  }
  glDisable(GL_BLEND);
  glDisable(GL_SCISSOR_TEST);
  glViewport(0, 0, window_width, window_height);
  if (clean_window == 0) {
    restore_shown(window_width, window_height, scene_framebuffer);
  }
  shown = false;
  const int32_t source_width = slots[current_slot].width;
  const int32_t source_height = slots[current_slot].height;
  const int32_t shorter = window_width < window_height ? window_width : window_height;
  int32_t budget = shorter * 42 / 100;
  if (budget > k_box) {
    budget = k_box;
  }
  if (budget > shorter - 32) {
    budget = shorter - 32;
  }
  if (budget < 1) {
    budget = 1;
  }
  int32_t card_width = budget;
  int32_t card_height = budget;
  if (source_width >= source_height && source_width > 0) {
    card_height = budget * source_height / source_width;
  }
  if (source_height > source_width && source_height > 0) {
    card_width = budget * source_width / source_height;
  }
  if (card_width < 1) {
    card_width = 1;
  }
  if (card_height < 1) {
    card_height = 1;
  }
  int32_t left = pointer_x + 24;
  if (left + card_width > window_width) {
    left = pointer_x - card_width - 24;
  }
  if (left < 0) {
    left = 0;
  }
  if (left + card_width > window_width) {
    left = window_width - card_width;
  }
  if (left < 0) {
    left = 0;
  }
  int32_t top = window_height - pointer_y + card_height + 16;
  if (top > window_height) {
    top = window_height;
  }
  int32_t bottom = top - card_height;
  if (bottom < 0) {
    bottom = 0;
    top = card_height;
    if (top > window_height) {
      top = window_height;
      bottom = 0;
    }
  }
  const int32_t right = left + card_width > window_width ? window_width : left + card_width;
  if (right - left < 1 || top - bottom < 1) {
    return 0;
  }
  if (scene_framebuffer <= 0) {
    capture_rect(left, bottom, right, top);
  }
  upload_picture();
  draw_quad(picture, left, bottom, right, top, window_width, window_height, true, effect_mode, effect_time());
  shown_left = left;
  shown_bottom = bottom;
  shown_right = right;
  shown_top = top;
  shown_pointer_x = pointer_x;
  shown_pointer_y = pointer_y;
  shown = true;
  return 1;
}

void discard() {
  if (program != 0) {
    glDeleteProgram(program);
    glDeleteTextures(1, &picture);
    glDeleteTextures(1, &backup);
    glDeleteBuffers(1, &buffer);
  }
  program = 0;
  picture = 0;
  backup = 0;
  buffer = 0;
  shown = false;
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
