#include "mlc/gl/glfw_window_gl.hpp"

#if __has_include(<GLFW/glfw3.h>)
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#define MLC_HAS_GLFW 1
#else
#define MLC_HAS_GLFW 0
#endif

#if MLC_HAS_GLFW
#include "mlc/gl/glad_gl.hpp"
#endif

#include <cmath>
#include <cstdlib>
#include <cstring>

namespace mlc {
namespace gl {

#if MLC_HAS_GLFW

namespace {

GLFWwindow*& context_window() {
  static GLFWwindow* window = nullptr;
  return window;
}

constexpr int k_window_width = 800;
constexpr int k_window_height = 600;

bool env_flag_enabled(const char* name) {
  const char* value = std::getenv(name);
  return value != nullptr && value[0] != '\0' && std::strcmp(value, "0") != 0;
}

struct InputTestOverride {
  bool active = false;
  int32_t mouse_x = 0;
  int32_t mouse_y = 0;
  int32_t mouse_left_down = 0;
  int32_t escape_down = 0;
};

InputTestOverride& input_test_override() {
  static InputTestOverride override_state;
  return override_state;
}

} // namespace

int32_t glfw_gl_context_begin(int32_t width, int32_t height) {
  if (context_window() != nullptr) {
    return -10;
  }
  if (glfwInit() != GLFW_TRUE) {
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  if (!env_flag_enabled("MLC_GLFW_VISIBLE")) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }
  const int window_width = width > 0 ? width : k_window_width;
  const int window_height = height > 0 ? height : k_window_height;
  GLFWwindow* window = glfwCreateWindow(
    window_width, window_height, "mlc gl renderer", nullptr, nullptr
  );
  if (window == nullptr) {
    glfwTerminate();
    return -2;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return -3;
  }
  context_window() = window;
  return 0;
}

int32_t glfw_gl_context_should_close() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 1;
  }
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  return glfwWindowShouldClose(window) ? 1 : 0;
}

void glfw_gl_context_swap_poll() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void glfw_gl_context_end() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  glfw_gl_input_test_clear();
  glfwDestroyWindow(window);
  context_window() = nullptr;
  glfwTerminate();
}

double glfw_gl_get_time() { return glfwGetTime(); }

double glfw_gl_anim_unit() {
  return 0.5 + 0.5 * std::sin(glfwGetTime() * 2.0);
}

int32_t glfw_gl_mouse_x() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.active) {
    return override_state.mouse_x;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  double cursor_x = 0.0;
  double cursor_y = 0.0;
  glfwGetCursorPos(window, &cursor_x, &cursor_y);
  return static_cast<int32_t>(cursor_x);
}

int32_t glfw_gl_mouse_y() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.active) {
    return override_state.mouse_y;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  double cursor_x = 0.0;
  double cursor_y = 0.0;
  glfwGetCursorPos(window, &cursor_x, &cursor_y);
  return static_cast<int32_t>(cursor_y);
}

int32_t glfw_gl_mouse_left_down() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.active) {
    return override_state.mouse_left_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? 1 : 0;
}

int32_t glfw_gl_key_escape_down() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.active) {
    return override_state.escape_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  return glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ? 1 : 0;
}

void glfw_gl_input_test_set(
  int32_t mouse_x,
  int32_t mouse_y,
  int32_t mouse_left_down,
  int32_t escape_down
) {
  InputTestOverride& override_state = input_test_override();
  override_state.active = true;
  override_state.mouse_x = mouse_x;
  override_state.mouse_y = mouse_y;
  override_state.mouse_left_down = mouse_left_down;
  override_state.escape_down = escape_down;
}

void glfw_gl_input_test_clear() {
  input_test_override() = InputTestOverride{};
}

#else

int32_t glfw_gl_context_begin(int32_t, int32_t) { return -100; }
int32_t glfw_gl_context_should_close() { return 1; }
void glfw_gl_context_swap_poll() {}
void glfw_gl_context_end() {}
double glfw_gl_get_time() { return 0.0; }
double glfw_gl_anim_unit() { return 0.0; }
int32_t glfw_gl_mouse_x() { return 0; }
int32_t glfw_gl_mouse_y() { return 0; }
int32_t glfw_gl_mouse_left_down() { return 0; }
int32_t glfw_gl_key_escape_down() { return 0; }
void glfw_gl_input_test_set(int32_t, int32_t, int32_t, int32_t) {}
void glfw_gl_input_test_clear() {}

#endif

} // namespace gl
} // namespace mlc
