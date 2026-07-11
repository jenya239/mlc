#pragma once

#include <cstdint>

namespace mlc {
namespace gl {

// GLFW window/context/input only (TRACK_GL_GLAD_MIGRATION STEP=5).
// GL entry points live in glad_gl_abi / vendored glad.

int32_t glfw_gl_context_begin(int32_t width, int32_t height);
int32_t glfw_gl_context_should_close();
void glfw_gl_context_swap_poll();
void glfw_gl_context_end();
double glfw_gl_get_time();
double glfw_gl_anim_unit();

int32_t glfw_gl_mouse_x();
int32_t glfw_gl_mouse_y();
int32_t glfw_gl_mouse_left_down();
int32_t glfw_gl_key_escape_down();
void glfw_gl_input_test_set(
  int32_t mouse_x,
  int32_t mouse_y,
  int32_t mouse_left_down,
  int32_t escape_down
);
void glfw_gl_input_test_clear();

} // namespace gl
} // namespace mlc
