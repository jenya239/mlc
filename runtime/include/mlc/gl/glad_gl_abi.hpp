#pragma once

// MLC-facing ABI over vendored GLAD2 desktop GL (+ scratch helpers).
// mlcc takes &symbol; GLAD macros make raw gl* unusable as addresses.

#include "mlc/core/string.hpp"
#include "mlc/gl/glad_gl.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

namespace mlc {
namespace gl {
namespace glad_abi {

inline int loader_load_gl() { return gladLoaderLoadGL(); }
inline void loader_unload_gl() { gladLoaderUnloadGL(); }

inline void clear(int32_t mask) { glClear(static_cast<GLbitfield>(mask)); }
inline void clear_color_f64(double red, double green, double blue, double alpha) {
  glClearColor(
    static_cast<GLfloat>(red),
    static_cast<GLfloat>(green),
    static_cast<GLfloat>(blue),
    static_cast<GLfloat>(alpha)
  );
}
inline void viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
  glViewport(x, y, width, height);
}
inline void enable(int32_t capability) { glEnable(static_cast<GLenum>(capability)); }
inline void blend_func(int32_t source_factor, int32_t destination_factor) {
  glBlendFunc(static_cast<GLenum>(source_factor), static_cast<GLenum>(destination_factor));
}

inline int32_t create_shader(int32_t shader_type) {
  return static_cast<int32_t>(glCreateShader(static_cast<GLenum>(shader_type)));
}
inline void shader_source_string(int32_t shader, String source) {
  const char* text = source.c_str();
  glShaderSource(static_cast<GLuint>(shader), 1, &text, nullptr);
}
inline void compile_shader(int32_t shader) { glCompileShader(static_cast<GLuint>(shader)); }
inline int32_t get_shader_iv_value(int32_t shader, int32_t parameter_name) {
  GLint value = 0;
  glGetShaderiv(static_cast<GLuint>(shader), static_cast<GLenum>(parameter_name), &value);
  return static_cast<int32_t>(value);
}
inline int32_t create_program() { return static_cast<int32_t>(glCreateProgram()); }
inline void attach_shader(int32_t program, int32_t shader) {
  glAttachShader(static_cast<GLuint>(program), static_cast<GLuint>(shader));
}
inline void link_program(int32_t program) { glLinkProgram(static_cast<GLuint>(program)); }
inline int32_t get_program_iv_value(int32_t program, int32_t parameter_name) {
  GLint value = 0;
  glGetProgramiv(static_cast<GLuint>(program), static_cast<GLenum>(parameter_name), &value);
  return static_cast<int32_t>(value);
}
inline void use_program(int32_t program) { glUseProgram(static_cast<GLuint>(program)); }
inline void delete_shader(int32_t shader) { glDeleteShader(static_cast<GLuint>(shader)); }
inline void delete_program(int32_t program) { glDeleteProgram(static_cast<GLuint>(program)); }
inline int32_t get_attrib_location_string(int32_t program, String name) {
  return glGetAttribLocation(static_cast<GLuint>(program), name.c_str());
}
inline int32_t get_uniform_location_string(int32_t program, String name) {
  return glGetUniformLocation(static_cast<GLuint>(program), name.c_str());
}
inline void uniform_1i(int32_t location, int32_t value) { glUniform1i(location, value); }

inline int32_t gen_buffer() {
  GLuint buffer = 0;
  glGenBuffers(1, &buffer);
  return static_cast<int32_t>(buffer);
}
inline void delete_buffer(int32_t buffer) {
  const GLuint handle = static_cast<GLuint>(buffer);
  glDeleteBuffers(1, &handle);
}
inline void bind_buffer(int32_t target, int32_t buffer) {
  glBindBuffer(static_cast<GLenum>(target), static_cast<GLuint>(buffer));
}
inline void enable_vertex_attrib_array(int32_t index) {
  glEnableVertexAttribArray(static_cast<GLuint>(index));
}
inline void vertex_attrib_pointer_offset(
  int32_t index,
  int32_t size,
  int32_t attribute_type,
  int32_t normalized,
  int32_t stride,
  int32_t byte_offset
) {
  glVertexAttribPointer(
    static_cast<GLuint>(index),
    size,
    static_cast<GLenum>(attribute_type),
    static_cast<GLboolean>(normalized),
    stride,
    reinterpret_cast<const void*>(static_cast<std::ptrdiff_t>(byte_offset))
  );
}
inline void draw_arrays(int32_t mode, int32_t first, int32_t count) {
  glDrawArrays(static_cast<GLenum>(mode), first, count);
}

inline int32_t gen_texture() {
  GLuint texture = 0;
  glGenTextures(1, &texture);
  return static_cast<int32_t>(texture);
}
inline void delete_texture(int32_t texture) {
  const GLuint handle = static_cast<GLuint>(texture);
  glDeleteTextures(1, &handle);
}
inline void bind_texture(int32_t target, int32_t texture) {
  glBindTexture(static_cast<GLenum>(target), static_cast<GLuint>(texture));
}
inline void active_texture(int32_t texture_unit) {
  glActiveTexture(static_cast<GLenum>(texture_unit));
}
inline void tex_parameter_i(int32_t target, int32_t parameter_name, int32_t parameter_value) {
  glTexParameteri(
    static_cast<GLenum>(target),
    static_cast<GLenum>(parameter_name),
    parameter_value
  );
}

inline std::vector<float>& scratch_f32() {
  static std::vector<float> values;
  return values;
}
inline std::vector<uint8_t>& scratch_u8() {
  static std::vector<uint8_t> values;
  return values;
}

inline void scratch_f32_clear() { scratch_f32().clear(); }
inline void scratch_f32_push(double value) {
  scratch_f32().push_back(static_cast<float>(value));
}
inline int32_t buffer_data_scratch(int32_t target, int32_t usage) {
  const std::vector<float>& values = scratch_f32();
  if (values.empty()) {
    return -1;
  }
  glBufferData(
    static_cast<GLenum>(target),
    static_cast<GLsizeiptr>(values.size() * sizeof(float)),
    values.data(),
    static_cast<GLenum>(usage)
  );
  return 0;
}

inline void scratch_u8_clear() { scratch_u8().clear(); }
inline void scratch_u8_resize_zero(int32_t byte_count) {
  if (byte_count < 0) {
    scratch_u8().clear();
    return;
  }
  scratch_u8().assign(static_cast<size_t>(byte_count), 0);
}
inline void scratch_u8_fill_rect(
  int32_t atlas_width,
  int32_t x,
  int32_t y,
  int32_t width,
  int32_t height,
  int32_t value
) {
  if (atlas_width <= 0 || width <= 0 || height <= 0) {
    return;
  }
  std::vector<uint8_t>& pixels = scratch_u8();
  const uint8_t byte_value = static_cast<uint8_t>(value & 0xff);
  for (int32_t row = 0; row < height; row += 1) {
    const int32_t atlas_y = y + row;
    for (int32_t column = 0; column < width; column += 1) {
      const size_t index =
        static_cast<size_t>(atlas_y * atlas_width + (x + column));
      if (index < pixels.size()) {
        pixels[index] = byte_value;
      }
    }
  }
}
inline int32_t tex_image_2d_scratch_luminance(int32_t width, int32_t height) {
  constexpr GLenum k_texture_2d = 0x0DE1;
  constexpr GLenum k_luminance = 0x1909;
  constexpr GLenum k_unsigned_byte = 0x1401;
  constexpr GLenum k_texture_min_filter = 0x2801;
  constexpr GLenum k_texture_mag_filter = 0x2800;
  constexpr GLint k_nearest = 0x2600;
  const std::vector<uint8_t>& pixels = scratch_u8();
  const size_t expected = static_cast<size_t>(width) * static_cast<size_t>(height);
  if (width <= 0 || height <= 0 || pixels.size() < expected) {
    return -1;
  }
  glTexImage2D(
    k_texture_2d, 0, static_cast<GLint>(k_luminance), width, height, 0,
    k_luminance, k_unsigned_byte, pixels.data()
  );
  glTexParameteri(k_texture_2d, k_texture_min_filter, k_nearest);
  glTexParameteri(k_texture_2d, k_texture_mag_filter, k_nearest);
  return 0;
}
inline void scratch_push_glyph_quad(
  double left,
  double bottom,
  double right,
  double top,
  int32_t atlas_size,
  int32_t slot_x,
  int32_t slot_y,
  int32_t slot_width,
  int32_t slot_height
) {
  if (atlas_size <= 0) {
    return;
  }
  const double atlas = static_cast<double>(atlas_size);
  const double u0 = static_cast<double>(slot_x) / atlas;
  const double v0 = static_cast<double>(slot_y) / atlas;
  const double u1 = static_cast<double>(slot_x + slot_width) / atlas;
  const double v1 = static_cast<double>(slot_y + slot_height) / atlas;
  auto& values = scratch_f32();
  const float verts[] = {
    static_cast<float>(left), static_cast<float>(bottom), static_cast<float>(u0), static_cast<float>(v1),
    static_cast<float>(right), static_cast<float>(bottom), static_cast<float>(u1), static_cast<float>(v1),
    static_cast<float>(left), static_cast<float>(top), static_cast<float>(u0), static_cast<float>(v0),
    static_cast<float>(right), static_cast<float>(bottom), static_cast<float>(u1), static_cast<float>(v1),
    static_cast<float>(right), static_cast<float>(top), static_cast<float>(u1), static_cast<float>(v0),
    static_cast<float>(left), static_cast<float>(top), static_cast<float>(u0), static_cast<float>(v0),
  };
  values.insert(values.end(), std::begin(verts), std::end(verts));
}

} // namespace glad_abi
} // namespace gl
} // namespace mlc
