#pragma once

// MLC-facing ABI over vendored GLAD2 desktop GL.
// mlcc extern codegen takes &symbol; GLAD `#define glClear glad_glClear` makes
// &glClear a pointer-to-pointer — real functions here avoid that.

#include "mlc/gl/glad_gl.hpp"

#include <cstdint>

namespace mlc {
namespace gl {
namespace glad_abi {

inline int loader_load_gl() { return gladLoaderLoadGL(); }
inline void loader_unload_gl() { gladLoaderUnloadGL(); }

inline void clear(int32_t mask) { glClear(static_cast<GLbitfield>(mask)); }
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
inline void compile_shader(int32_t shader) { glCompileShader(static_cast<GLuint>(shader)); }
inline int32_t create_program() { return static_cast<int32_t>(glCreateProgram()); }
inline void attach_shader(int32_t program, int32_t shader) {
  glAttachShader(static_cast<GLuint>(program), static_cast<GLuint>(shader));
}
inline void link_program(int32_t program) { glLinkProgram(static_cast<GLuint>(program)); }
inline void use_program(int32_t program) { glUseProgram(static_cast<GLuint>(program)); }
inline void delete_shader(int32_t shader) { glDeleteShader(static_cast<GLuint>(shader)); }
inline void delete_program(int32_t program) { glDeleteProgram(static_cast<GLuint>(program)); }
inline void uniform_1i(int32_t location, int32_t value) { glUniform1i(location, value); }

inline void bind_buffer(int32_t target, int32_t buffer) {
  glBindBuffer(static_cast<GLenum>(target), static_cast<GLuint>(buffer));
}
inline void enable_vertex_attrib_array(int32_t index) {
  glEnableVertexAttribArray(static_cast<GLuint>(index));
}
inline void draw_arrays(int32_t mode, int32_t first, int32_t count) {
  glDrawArrays(static_cast<GLenum>(mode), first, count);
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

} // namespace glad_abi
} // namespace gl
} // namespace mlc
