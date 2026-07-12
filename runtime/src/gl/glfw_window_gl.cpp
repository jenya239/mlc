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
#include <string>

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
  bool keys_active = false;
  int32_t mouse_x = 0;
  int32_t mouse_y = 0;
  int32_t mouse_left_down = 0;
  int32_t escape_down = 0;
  int32_t backspace_down = 0;
  int32_t enter_down = 0;
};

InputTestOverride& input_test_override() {
  static InputTestOverride override_state;
  return override_state;
}

std::string& pending_text() {
  static std::string buffer;
  return buffer;
}

int32_t& cached_window_width() {
  static int32_t width = 0;
  return width;
}

int32_t& cached_window_height() {
  static int32_t height = 0;
  return height;
}

void append_utf8(std::string& out, unsigned int codepoint) {
  // ASCII / BMP Latin text only (TRACK_GUI_INPUT_ROBUSTNESS out-of-scope: IME/CJK).
  if (codepoint <= 0x7Fu) {
    out.push_back(static_cast<char>(codepoint));
  } else if (codepoint <= 0x7FFu) {
    out.push_back(static_cast<char>(0xC0u | (codepoint >> 6)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  } else if (codepoint <= 0xFFFFu) {
    out.push_back(static_cast<char>(0xE0u | (codepoint >> 12)));
    out.push_back(static_cast<char>(0x80u | ((codepoint >> 6) & 0x3Fu)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
}

void on_char(GLFWwindow* /*window*/, unsigned int codepoint) {
  append_utf8(pending_text(), codepoint);
}

void on_window_size(GLFWwindow* /*window*/, int width, int height) {
  cached_window_width() = width;
  cached_window_height() = height;
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
  pending_text().clear();
  cached_window_width() = window_width;
  cached_window_height() = window_height;
  glfwSetCharCallback(window, on_char);
  glfwSetWindowSizeCallback(window, on_window_size);
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
  pending_text().clear();
  cached_window_width() = 0;
  cached_window_height() = 0;
  glfwDestroyWindow(window);
  context_window() = nullptr;
  glfwTerminate();
}

double glfw_gl_get_time() { return glfwGetTime(); }

double glfw_gl_anim_unit() {
  return 0.5 + 0.5 * std::sin(glfwGetTime() * 2.0);
}

int32_t glfw_gl_window_width() {
  return cached_window_width();
}

int32_t glfw_gl_window_height() {
  return cached_window_height();
}

void glfw_gl_window_set_size(int32_t width, int32_t height) {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  if (width <= 0 || height <= 0) {
    return;
  }
  glfwSetWindowSize(window, width, height);
  glfwPollEvents();
  // Invisible windows may report the old size in the size callback; keep request.
  cached_window_width() = width;
  cached_window_height() = height;
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

int32_t glfw_gl_key_backspace_down() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.keys_active) {
    return override_state.backspace_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  return glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS ? 1 : 0;
}

int32_t glfw_gl_key_enter_down() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.keys_active) {
    return override_state.enter_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  return glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ? 1 : 0;
}

String glfw_gl_take_text() {
  std::string& buffer = pending_text();
  String taken(buffer);
  buffer.clear();
  return taken;
}

void glfw_gl_text_test_push(String text) {
  pending_text().append(text.raw_data(), text.size());
}

void glfw_gl_keys_test_set(int32_t backspace_down, int32_t enter_down) {
  InputTestOverride& override_state = input_test_override();
  override_state.keys_active = true;
  override_state.backspace_down = backspace_down;
  override_state.enter_down = enter_down;
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
  pending_text().clear();
}

#else

int32_t glfw_gl_context_begin(int32_t, int32_t) { return -100; }
int32_t glfw_gl_context_should_close() { return 1; }
void glfw_gl_context_swap_poll() {}
void glfw_gl_context_end() {}
double glfw_gl_get_time() { return 0.0; }
double glfw_gl_anim_unit() { return 0.0; }
int32_t glfw_gl_window_width() { return 0; }
int32_t glfw_gl_window_height() { return 0; }
void glfw_gl_window_set_size(int32_t, int32_t) {}
int32_t glfw_gl_mouse_x() { return 0; }
int32_t glfw_gl_mouse_y() { return 0; }
int32_t glfw_gl_mouse_left_down() { return 0; }
int32_t glfw_gl_key_escape_down() { return 0; }
int32_t glfw_gl_key_backspace_down() { return 0; }
int32_t glfw_gl_key_enter_down() { return 0; }
String glfw_gl_take_text() { return String(); }
void glfw_gl_text_test_push(String) {}
void glfw_gl_keys_test_set(int32_t, int32_t) {}
void glfw_gl_input_test_set(int32_t, int32_t, int32_t, int32_t) {}
void glfw_gl_input_test_clear() {}

#endif

} // namespace gl
} // namespace mlc
