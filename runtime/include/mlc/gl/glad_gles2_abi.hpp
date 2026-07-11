#pragma once

// MLC-facing ABI over vendored GLAD2 GLES2 (headless).

#include "mlc/gl/glad_gles2.hpp"

#include <cstdint>

namespace mlc {
namespace gl {
namespace glad_gles2_abi {

inline int loader_load_gles2() { return gladLoaderLoadGLES2(); }
inline void loader_unload_gles2() { gladLoaderUnloadGLES2(); }

inline void clear(int32_t mask) { glClear(static_cast<GLbitfield>(mask)); }
inline void viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
  glViewport(x, y, width, height);
}
inline void bind_buffer(int32_t target, int32_t buffer) {
  glBindBuffer(static_cast<GLenum>(target), static_cast<GLuint>(buffer));
}
inline void draw_arrays(int32_t mode, int32_t first, int32_t count) {
  glDrawArrays(static_cast<GLenum>(mode), first, count);
}

} // namespace glad_gles2_abi
} // namespace gl
} // namespace mlc
