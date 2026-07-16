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

double& pending_scroll_y() {
  static double scroll_y = 0.0;
  return scroll_y;
}

struct ClipboardTestOverride {
  bool active = false;
  std::string text;
};

ClipboardTestOverride& clipboard_test_override() {
  static ClipboardTestOverride override_state;
  return override_state;
}

struct ModsTestOverride {
  bool active = false;
  int32_t ctrl_down = 0;
  int32_t shift_down = 0;
  int32_t alt_down = 0;
};

ModsTestOverride& mods_test_override() {
  static ModsTestOverride override_state;
  return override_state;
}

std::string& pending_binding_key() {
  static std::string key;
  return key;
}

struct BindingKeyEdges {
  int32_t key_s = 0;
  int32_t key_z = 0;
  int32_t key_y = 0;
  int32_t key_c = 0;
  int32_t key_x = 0;
  int32_t key_v = 0;
  int32_t key_a = 0;
  int32_t key_w = 0;
  int32_t key_tab = 0;
};

BindingKeyEdges& binding_key_edges() {
  static BindingKeyEdges edges;
  return edges;
}

int32_t& cached_window_width() {
  static int32_t width = 0;
  return width;
}

int32_t& cached_window_height() {
  static int32_t height = 0;
  return height;
}

struct StandardCursors {
  GLFWcursor* arrow = nullptr;
  GLFWcursor* ew_resize = nullptr;
  GLFWcursor* ibeam = nullptr;
  int32_t last_shape = 0;
};

StandardCursors& standard_cursors() {
  static StandardCursors cursors;
  return cursors;
}

void destroy_standard_cursors() {
  StandardCursors& cursors = standard_cursors();
  if (cursors.arrow != nullptr) {
    glfwDestroyCursor(cursors.arrow);
    cursors.arrow = nullptr;
  }
  if (cursors.ew_resize != nullptr) {
    glfwDestroyCursor(cursors.ew_resize);
    cursors.ew_resize = nullptr;
  }
  if (cursors.ibeam != nullptr) {
    glfwDestroyCursor(cursors.ibeam);
    cursors.ibeam = nullptr;
  }
  cursors.last_shape = 0;
}

void ensure_standard_cursors() {
  StandardCursors& cursors = standard_cursors();
  if (cursors.arrow == nullptr) {
    cursors.arrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  }
  if (cursors.ew_resize == nullptr) {
    cursors.ew_resize = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  }
  if (cursors.ibeam == nullptr) {
    cursors.ibeam = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  }
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

void on_scroll(GLFWwindow* /*window*/, double /*offset_x*/, double offset_y) {
  pending_scroll_y() += offset_y;
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
  pending_scroll_y() = 0.0;
  cached_window_width() = window_width;
  cached_window_height() = window_height;
  glfwSetCharCallback(window, on_char);
  glfwSetScrollCallback(window, on_scroll);
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
  pending_scroll_y() = 0.0;
  cached_window_width() = 0;
  cached_window_height() = 0;
  destroy_standard_cursors();
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

// TRACK_GLFW_CONTENT_SCALE STEP=1 — missing window → 1.0
double glfw_gl_window_content_scale_x() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 1.0;
  }
  float scale_x = 1.0f;
  float scale_y = 1.0f;
  glfwGetWindowContentScale(window, &scale_x, &scale_y);
  return static_cast<double>(scale_x);
}

double glfw_gl_window_content_scale_y() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 1.0;
  }
  float scale_x = 1.0f;
  float scale_y = 1.0f;
  glfwGetWindowContentScale(window, &scale_x, &scale_y);
  return static_cast<double>(scale_y);
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

double glfw_gl_take_scroll_y() {
  const double value = pending_scroll_y();
  pending_scroll_y() = 0.0;
  return value;
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
  pending_scroll_y() = 0.0;
  clipboard_test_override() = ClipboardTestOverride{};
  mods_test_override() = ModsTestOverride{};
  pending_binding_key().clear();
  binding_key_edges() = BindingKeyEdges{};
}

void glfw_gl_cursor_set(int32_t shape) {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  ensure_standard_cursors();
  StandardCursors& cursors = standard_cursors();
  GLFWcursor* cursor = cursors.arrow;
  int32_t resolved = 0;
  if (shape == 1) {
    cursor = cursors.ew_resize;
    resolved = 1;
  } else if (shape == 2) {
    cursor = cursors.ibeam;
    resolved = 2;
  }
  if (cursor != nullptr) {
    glfwSetCursor(window, cursor);
  }
  cursors.last_shape = resolved;
}

int32_t glfw_gl_cursor_shape_get() {
  return standard_cursors().last_shape;
}

String glfw_gl_clipboard_get() {
  const ClipboardTestOverride& override_state = clipboard_test_override();
  if (override_state.active) {
    return String(override_state.text);
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return String();
  }
  const char* text = glfwGetClipboardString(window);
  if (text == nullptr) {
    return String();
  }
  return String(text);
}

void glfw_gl_clipboard_set(String text) {
  const std::string payload(text.raw_data(), text.size());
  ClipboardTestOverride& override_state = clipboard_test_override();
  if (override_state.active) {
    override_state.text = payload;
    return;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  glfwSetClipboardString(window, payload.c_str());
}

void glfw_gl_clipboard_test_set(String text) {
  ClipboardTestOverride& override_state = clipboard_test_override();
  override_state.active = true;
  override_state.text = std::string(text.raw_data(), text.size());
}

int32_t glfw_gl_mod_ctrl_down() {
  const ModsTestOverride& override_state = mods_test_override();
  if (override_state.active) {
    return override_state.ctrl_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  const int left = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? 1 : 0;
  const int right = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS ? 1 : 0;
  return (left != 0 || right != 0) ? 1 : 0;
}

int32_t glfw_gl_mod_shift_down() {
  const ModsTestOverride& override_state = mods_test_override();
  if (override_state.active) {
    return override_state.shift_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  const int left = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 1 : 0;
  const int right = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ? 1 : 0;
  return (left != 0 || right != 0) ? 1 : 0;
}

int32_t glfw_gl_mod_alt_down() {
  const ModsTestOverride& override_state = mods_test_override();
  if (override_state.active) {
    return override_state.alt_down;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  const int left = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ? 1 : 0;
  const int right = glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS ? 1 : 0;
  return (left != 0 || right != 0) ? 1 : 0;
}

namespace {

int32_t edge_key_down(int32_t glfw_key, int32_t& previous_down) {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    previous_down = 0;
    return 0;
  }
  const int32_t down = glfwGetKey(window, glfw_key) == GLFW_PRESS ? 1 : 0;
  const int32_t edge = (down != 0 && previous_down == 0) ? 1 : 0;
  previous_down = down;
  return edge;
}

} // namespace

String glfw_gl_take_binding_key() {
  if (!pending_binding_key().empty()) {
    String taken(pending_binding_key());
    pending_binding_key().clear();
    return taken;
  }
  BindingKeyEdges& edges = binding_key_edges();
  if (edge_key_down(GLFW_KEY_S, edges.key_s) != 0) {
    return String("s");
  }
  if (edge_key_down(GLFW_KEY_Z, edges.key_z) != 0) {
    return String("z");
  }
  if (edge_key_down(GLFW_KEY_Y, edges.key_y) != 0) {
    return String("y");
  }
  if (edge_key_down(GLFW_KEY_C, edges.key_c) != 0) {
    return String("c");
  }
  if (edge_key_down(GLFW_KEY_X, edges.key_x) != 0) {
    return String("x");
  }
  if (edge_key_down(GLFW_KEY_V, edges.key_v) != 0) {
    return String("v");
  }
  if (edge_key_down(GLFW_KEY_A, edges.key_a) != 0) {
    return String("a");
  }
  if (edge_key_down(GLFW_KEY_W, edges.key_w) != 0) {
    return String("w");
  }
  if (edge_key_down(GLFW_KEY_TAB, edges.key_tab) != 0) {
    return String("tab");
  }
  return String();
}

void glfw_gl_mods_test_set(int32_t ctrl_down, int32_t shift_down, int32_t alt_down) {
  ModsTestOverride& override_state = mods_test_override();
  override_state.active = true;
  override_state.ctrl_down = ctrl_down;
  override_state.shift_down = shift_down;
  override_state.alt_down = alt_down;
}

void glfw_gl_binding_key_test_push(String key) {
  pending_binding_key() = std::string(key.raw_data(), key.size());
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
double glfw_gl_window_content_scale_x() { return 1.0; }
double glfw_gl_window_content_scale_y() { return 1.0; }
int32_t glfw_gl_mouse_x() { return 0; }
int32_t glfw_gl_mouse_y() { return 0; }
int32_t glfw_gl_mouse_left_down() { return 0; }
double glfw_gl_take_scroll_y() { return 0.0; }
int32_t glfw_gl_key_escape_down() { return 0; }
int32_t glfw_gl_key_backspace_down() { return 0; }
int32_t glfw_gl_key_enter_down() { return 0; }
String glfw_gl_take_text() { return String(); }
void glfw_gl_text_test_push(String) {}
void glfw_gl_keys_test_set(int32_t, int32_t) {}
void glfw_gl_input_test_set(int32_t, int32_t, int32_t, int32_t) {}
void glfw_gl_input_test_clear() {}
void glfw_gl_cursor_set(int32_t) {}
int32_t glfw_gl_cursor_shape_get() { return 0; }
String glfw_gl_clipboard_get() { return String(); }
void glfw_gl_clipboard_set(String) {}
void glfw_gl_clipboard_test_set(String) {}
int32_t glfw_gl_mod_ctrl_down() { return 0; }
int32_t glfw_gl_mod_shift_down() { return 0; }
int32_t glfw_gl_mod_alt_down() { return 0; }
String glfw_gl_take_binding_key() { return String(); }
void glfw_gl_mods_test_set(int32_t, int32_t, int32_t) {}
void glfw_gl_binding_key_test_push(String) {}

#endif

} // namespace gl
} // namespace mlc
