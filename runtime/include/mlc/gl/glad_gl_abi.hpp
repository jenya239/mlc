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
inline void scissor(int32_t x, int32_t y, int32_t width, int32_t height) {
  glScissor(x, y, width, height);
}
inline int32_t is_enabled(int32_t capability) {
  return glIsEnabled(static_cast<GLenum>(capability)) ? 1 : 0;
}
// For 4-component queries such as GL_SCISSOR_BOX (index 0..3).
inline int32_t get_integer_at(int32_t parameter_name, int32_t index) {
  GLint values[4] = {0, 0, 0, 0};
  glGetIntegerv(static_cast<GLenum>(parameter_name), values);
  if (index < 0 || index > 3) {
    return 0;
  }
  return static_cast<int32_t>(values[index]);
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
inline uint8_t* scratch_u8_mutable_data() {
  if (scratch_u8().empty()) {
    return nullptr;
  }
  return scratch_u8().data();
}
inline int32_t scratch_u8_size() {
  return static_cast<int32_t>(scratch_u8().size());
}
inline void tex_image_2d(
  int32_t target,
  int32_t level,
  int32_t internal_format,
  int32_t width,
  int32_t height,
  int32_t border,
  int32_t format,
  int32_t type,
  const void* data
) {
  glTexImage2D(
    static_cast<GLenum>(target),
    level,
    internal_format,
    width,
    height,
    border,
    static_cast<GLenum>(format),
    static_cast<GLenum>(type),
    data
  );
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

// Compatibility names for demos / helpers still using mlc::gl::gl_*.
inline void gl_clear(int32_t mask) { glad_abi::clear(mask); }
inline void gl_clear_color_f64(double red, double green, double blue, double alpha) {
  glad_abi::clear_color_f64(red, green, blue, alpha);
}
inline void gl_viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
  glad_abi::viewport(x, y, width, height);
}
inline void gl_enable(int32_t capability) { glad_abi::enable(capability); }
inline void gl_blend_func(int32_t source_factor, int32_t destination_factor) {
  glad_abi::blend_func(source_factor, destination_factor);
}
inline void gl_scissor(int32_t x, int32_t y, int32_t width, int32_t height) {
  glad_abi::scissor(x, y, width, height);
}
inline int32_t gl_is_enabled(int32_t capability) { return glad_abi::is_enabled(capability); }
inline int32_t gl_get_integer_at(int32_t parameter_name, int32_t index) {
  return glad_abi::get_integer_at(parameter_name, index);
}
inline int32_t gl_create_shader(int32_t shader_type) {
  return glad_abi::create_shader(shader_type);
}
inline void gl_shader_source_string(int32_t shader, String source) {
  glad_abi::shader_source_string(shader, source);
}
inline void gl_compile_shader(int32_t shader) { glad_abi::compile_shader(shader); }
inline int32_t gl_get_shader_iv_value(int32_t shader, int32_t parameter_name) {
  return glad_abi::get_shader_iv_value(shader, parameter_name);
}
inline int32_t gl_create_program() { return glad_abi::create_program(); }
inline void gl_attach_shader(int32_t program, int32_t shader) {
  glad_abi::attach_shader(program, shader);
}
inline void gl_link_program(int32_t program) { glad_abi::link_program(program); }
inline int32_t gl_get_program_iv_value(int32_t program, int32_t parameter_name) {
  return glad_abi::get_program_iv_value(program, parameter_name);
}
inline void gl_use_program(int32_t program) { glad_abi::use_program(program); }
inline void gl_delete_shader(int32_t shader) { glad_abi::delete_shader(shader); }
inline void gl_delete_program(int32_t program) { glad_abi::delete_program(program); }
inline int32_t gl_get_attrib_location_string(int32_t program, String name) {
  return glad_abi::get_attrib_location_string(program, name);
}
inline int32_t gl_get_uniform_location_string(int32_t program, String name) {
  return glad_abi::get_uniform_location_string(program, name);
}
inline void gl_uniform_1i(int32_t location, int32_t value) {
  glad_abi::uniform_1i(location, value);
}
inline int32_t gl_gen_buffer() { return glad_abi::gen_buffer(); }
inline void gl_delete_buffer(int32_t buffer) { glad_abi::delete_buffer(buffer); }
inline void gl_bind_buffer(int32_t target, int32_t buffer) {
  glad_abi::bind_buffer(target, buffer);
}
inline void gl_scratch_f32_clear() { glad_abi::scratch_f32_clear(); }
inline void gl_scratch_f32_push(double value) { glad_abi::scratch_f32_push(value); }
inline int32_t gl_buffer_data_scratch(int32_t target, int32_t usage) {
  return glad_abi::buffer_data_scratch(target, usage);
}
inline void gl_enable_vertex_attrib_array(int32_t index) {
  glad_abi::enable_vertex_attrib_array(index);
}
inline void gl_vertex_attrib_pointer_offset(
  int32_t index,
  int32_t size,
  int32_t attribute_type,
  int32_t normalized,
  int32_t stride,
  int32_t byte_offset
) {
  glad_abi::vertex_attrib_pointer_offset(
    index, size, attribute_type, normalized, stride, byte_offset
  );
}
inline void gl_draw_arrays(int32_t mode, int32_t first, int32_t count) {
  glad_abi::draw_arrays(mode, first, count);
}
inline int32_t gl_gen_texture() { return glad_abi::gen_texture(); }
inline void gl_delete_texture(int32_t texture) { glad_abi::delete_texture(texture); }
inline void gl_bind_texture(int32_t target, int32_t texture) {
  glad_abi::bind_texture(target, texture);
}
inline void gl_active_texture(int32_t texture_unit) {
  glad_abi::active_texture(texture_unit);
}
inline void gl_tex_parameter_i(int32_t target, int32_t parameter_name, int32_t parameter_value) {
  glad_abi::tex_parameter_i(target, parameter_name, parameter_value);
}
inline void gl_scratch_u8_clear() { glad_abi::scratch_u8_clear(); }
inline void gl_scratch_u8_resize_zero(int32_t byte_count) {
  glad_abi::scratch_u8_resize_zero(byte_count);
}
inline uint8_t* gl_scratch_u8_mutable_data() { return glad_abi::scratch_u8_mutable_data(); }
inline int32_t gl_scratch_u8_size() { return glad_abi::scratch_u8_size(); }
inline void gl_scratch_u8_fill_rect(
  int32_t atlas_width,
  int32_t x,
  int32_t y,
  int32_t width,
  int32_t height,
  int32_t value
) {
  glad_abi::scratch_u8_fill_rect(atlas_width, x, y, width, height, value);
}
inline int32_t gl_tex_image_2d_scratch_luminance(int32_t width, int32_t height) {
  return glad_abi::tex_image_2d_scratch_luminance(width, height);
}
inline void gl_scratch_push_glyph_quad(
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
  glad_abi::scratch_push_glyph_quad(
    left, bottom, right, top, atlas_size, slot_x, slot_y, slot_width, slot_height
  );
}
inline void gl_tex_image_2d(
  int32_t target,
  int32_t level,
  int32_t internal_format,
  int32_t width,
  int32_t height,
  int32_t border,
  int32_t format,
  int32_t type,
  const void* data
) {
  glad_abi::tex_image_2d(
    target, level, internal_format, width, height, border, format, type, data
  );
}

} // namespace gl
} // namespace mlc
