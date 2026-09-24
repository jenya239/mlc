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
#include <deque>
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

std::deque<std::string>& pending_drop_paths() {
  static std::deque<std::string> paths;
  return paths;
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
  int32_t key_d = 0;
  int32_t key_x = 0;
  int32_t key_v = 0;
  int32_t key_a = 0;
  int32_t key_w = 0;
  int32_t key_tab = 0;
  int32_t key_left = 0;
  int32_t key_right = 0;
  int32_t key_up = 0;
  int32_t key_down = 0;
  int32_t key_home = 0;
  int32_t key_end = 0;
  int32_t key_page_up = 0;
  int32_t key_page_down = 0;
  int32_t key_delete = 0;
  int32_t key_f = 0;
  int32_t key_f3 = 0;
  int32_t key_g = 0;
  int32_t key_slash = 0;
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

struct PlacementRequest {
  bool active = false;
  int x = 0;
  int y = 0;
};

PlacementRequest& placement_request() {
  static PlacementRequest request;
  return request;
}

bool rect_overlaps_work_area(int x, int y, int width, int height) {
  int monitor_count = 0;
  GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
  if (monitors == nullptr || monitor_count <= 0) {
    return true;
  }
  const int need_x = width < 32 ? 1 : 32;
  const int need_y = height < 32 ? 1 : 32;
  for (int index = 0; index < monitor_count; ++index) {
    int area_x = 0;
    int area_y = 0;
    int area_width = 0;
    int area_height = 0;
    glfwGetMonitorWorkarea(monitors[index], &area_x, &area_y, &area_width, &area_height);
    const int left = x > area_x ? x : area_x;
    const int top = y > area_y ? y : area_y;
    const int right_edge = x + width;
    const int area_right = area_x + area_width;
    const int right = right_edge < area_right ? right_edge : area_right;
    const int bottom_edge = y + height;
    const int area_bottom = area_y + area_height;
    const int bottom = bottom_edge < area_bottom ? bottom_edge : area_bottom;
    if (right - left >= need_x && bottom - top >= need_y) {
      return true;
    }
  }
  return false;
}

struct StandardCursors {
  GLFWcursor* arrow = nullptr;
  GLFWcursor* ew_resize = nullptr;
  GLFWcursor* ibeam = nullptr;
  GLFWcursor* hand = nullptr;
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
  if (cursors.hand != nullptr) {
    glfwDestroyCursor(cursors.hand);
    cursors.hand = nullptr;
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
  if (cursors.hand == nullptr) {
    cursors.hand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
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

std::deque<std::string>& pending_edit_repeats() {
  static std::deque<std::string> repeats;
  return repeats;
}

void discard_one_edit_repeat(const char* name) {
  std::deque<std::string>& repeats = pending_edit_repeats();
  for (std::deque<std::string>::iterator item = repeats.begin(); item != repeats.end(); ++item) {
    if (*item == name) {
      repeats.erase(item);
      return;
    }
  }
}

const char* edit_repeat_name(int key, int mods) {
  switch (key) {
    case GLFW_KEY_BACKSPACE:
      return "backspace";
    case GLFW_KEY_DELETE:
      return "delete";
    case GLFW_KEY_LEFT:
      return "left";
    case GLFW_KEY_RIGHT:
      return "right";
    case GLFW_KEY_HOME:
      return "home";
    case GLFW_KEY_END:
      return "end";
    case GLFW_KEY_KP_7:
      // Numpad 7 is Home while Num Lock is off. GLFW reports the physical key.
      if ((mods & GLFW_MOD_NUM_LOCK) == 0) {
        return "home";
      }
      return nullptr;
    case GLFW_KEY_KP_1:
      if ((mods & GLFW_MOD_NUM_LOCK) == 0) {
        return "end";
      }
      return nullptr;
    default:
      return nullptr;
  }
}

void on_key(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods) {
  // glfwGetKey stays pressed and hides GLFW_REPEAT. Home and End are queued
  // on press as well: a tap is often released before the next poll, and the
  // numpad pair is a physical key (KP_7 / KP_1) rather than GLFW_KEY_HOME.
  if (action != GLFW_PRESS && action != GLFW_REPEAT) {
    return;
  }
  const char* name = edit_repeat_name(key, mods);
  if (name == nullptr) {
    return;
  }
  const bool navigation_jump = key == GLFW_KEY_HOME || key == GLFW_KEY_END ||
    key == GLFW_KEY_KP_7 || key == GLFW_KEY_KP_1;
  if (navigation_jump || action == GLFW_REPEAT) {
    pending_edit_repeats().push_back(name);
  }
}

void on_scroll(GLFWwindow* /*window*/, double /*offset_x*/, double offset_y) {
  pending_scroll_y() += offset_y;
}

void on_window_size(GLFWwindow* /*window*/, int width, int height) {
  cached_window_width() = width;
  cached_window_height() = height;
}

void on_drop(GLFWwindow* /*window*/, int path_count, const char** paths) {
  if (paths == nullptr || path_count <= 0) {
    return;
  }
  for (int index = 0; index < path_count; ++index) {
    if (paths[index] == nullptr) {
      continue;
    }
    pending_drop_paths().push_back(std::string(paths[index]));
  }
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
  const bool want_visible = env_flag_enabled("MLC_GLFW_VISIBLE");
  const bool place = placement_request().active;
  const int place_x = placement_request().x;
  const int place_y = placement_request().y;
  placement_request().active = false;
  if (!want_visible || place) {
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
  // Default vsync. textui overrides to 0: a frame is painted only when the
  // signature changes, so an uncapped swap does not spin on a still pointer.
  if (env_flag_enabled("MLC_GLFW_VISIBLE")) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return -3;
  }
  pending_text().clear();
  pending_drop_paths().clear();
  pending_edit_repeats().clear();
  pending_scroll_y() = 0.0;
  cached_window_width() = window_width;
  cached_window_height() = window_height;
  glfwSetCharCallback(window, on_char);
  glfwSetKeyCallback(window, on_key);
  glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
  glfwSetScrollCallback(window, on_scroll);
  glfwSetWindowSizeCallback(window, on_window_size);
  glfwSetDropCallback(window, on_drop);
  context_window() = window;
  if (place && rect_overlaps_work_area(place_x, place_y, window_width, window_height)) {
    glfwSetWindowPos(window, place_x, place_y);
  }
  if (want_visible && place) {
    glfwShowWindow(window);
  }
  return 0;
}

void glfw_gl_swap_interval(int32_t interval) {
  if (context_window() == nullptr) {
    return;
  }
  glfwSwapInterval(interval);
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

void glfw_gl_context_wait_events_timeout(double timeout_seconds) {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  // Negative timeout waits until a GLFW event. Zero polls already-queued events.
  if (timeout_seconds < 0.0) {
    glfwWaitEvents();
    return;
  }
  glfwWaitEventsTimeout(timeout_seconds);
}

void glfw_gl_context_end() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return;
  }
  glfw_gl_input_test_clear();
  pending_text().clear();
  pending_drop_paths().clear();
  pending_edit_repeats().clear();
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

int32_t glfw_gl_window_x() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  int x = 0;
  int y = 0;
  glfwGetWindowPos(window, &x, &y);
  return x;
}

int32_t glfw_gl_window_y() {
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  int x = 0;
  int y = 0;
  glfwGetWindowPos(window, &x, &y);
  return y;
}

void glfw_gl_window_request_position(int32_t x, int32_t y) {
  placement_request().active = true;
  placement_request().x = x;
  placement_request().y = y;
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

int32_t glfw_gl_mouse_right_down() {
  const InputTestOverride& override_state = input_test_override();
  if (override_state.active) {
    // Test override has no right button; live GLFW path is the product gate.
    return 0;
  }
  GLFWwindow* window = context_window();
  if (window == nullptr) {
    return 0;
  }
  return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? 1 : 0;
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
  pending_drop_paths().clear();
  pending_edit_repeats().clear();
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
  } else if (shape == 3) {
    cursor = cursors.hand;
    resolved = 3;
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
  // TRACK_EDITOR_TERMINAL §102d — Ctrl+D (EOF signal), no prior binding used D.
  if (edge_key_down(GLFW_KEY_D, edges.key_d) != 0) {
    return String("d");
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
  if (edge_key_down(GLFW_KEY_LEFT, edges.key_left) != 0) {
    return String("left");
  }
  if (edge_key_down(GLFW_KEY_RIGHT, edges.key_right) != 0) {
    return String("right");
  }
  if (edge_key_down(GLFW_KEY_UP, edges.key_up) != 0) {
    return String("up");
  }
  if (edge_key_down(GLFW_KEY_DOWN, edges.key_down) != 0) {
    return String("down");
  }
  if (edge_key_down(GLFW_KEY_HOME, edges.key_home) != 0) {
    discard_one_edit_repeat("home");
    return String("home");
  }
  if (edge_key_down(GLFW_KEY_END, edges.key_end) != 0) {
    discard_one_edit_repeat("end");
    return String("end");
  }
  if (edge_key_down(GLFW_KEY_PAGE_UP, edges.key_page_up) != 0) {
    return String("page_up");
  }
  if (edge_key_down(GLFW_KEY_PAGE_DOWN, edges.key_page_down) != 0) {
    return String("page_down");
  }
  if (edge_key_down(GLFW_KEY_DELETE, edges.key_delete) != 0) {
    return String("delete");
  }
  if (edge_key_down(GLFW_KEY_F, edges.key_f) != 0) {
    return String("f");
  }
  if (edge_key_down(GLFW_KEY_F3, edges.key_f3) != 0) {
    return String("f3");
  }
  if (edge_key_down(GLFW_KEY_G, edges.key_g) != 0) {
    return String("g");
  }
  if (edge_key_down(GLFW_KEY_SLASH, edges.key_slash) != 0) {
    return String("/");
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

String glfw_gl_take_edit_repeat() {
  std::deque<std::string>& repeats = pending_edit_repeats();
  if (repeats.empty()) {
    return String();
  }
  String taken(repeats.front());
  repeats.pop_front();
  return taken;
}

String glfw_gl_take_drop_path() {
  std::deque<std::string>& paths = pending_drop_paths();
  if (paths.empty()) {
    return String();
  }
  String taken(paths.front());
  paths.pop_front();
  return taken;
}

void glfw_gl_drop_path_test_push(String path) {
  pending_drop_paths().push_back(std::string(path.raw_data(), path.size()));
}

#else

int32_t glfw_gl_context_begin(int32_t, int32_t) { return -100; }
void glfw_gl_swap_interval(int32_t) {}
int32_t glfw_gl_context_should_close() { return 1; }
void glfw_gl_context_swap_poll() {}
void glfw_gl_context_wait_events_timeout(double) {}
void glfw_gl_context_end() {}
double glfw_gl_get_time() { return 0.0; }
double glfw_gl_anim_unit() { return 0.0; }
int32_t glfw_gl_window_width() { return 0; }
int32_t glfw_gl_window_height() { return 0; }
int32_t glfw_gl_window_x() { return 0; }
int32_t glfw_gl_window_y() { return 0; }
void glfw_gl_window_request_position(int32_t, int32_t) {}
void glfw_gl_window_set_size(int32_t, int32_t) {}
double glfw_gl_window_content_scale_x() { return 1.0; }
double glfw_gl_window_content_scale_y() { return 1.0; }
int32_t glfw_gl_mouse_x() { return 0; }
int32_t glfw_gl_mouse_y() { return 0; }
int32_t glfw_gl_mouse_left_down() { return 0; }
int32_t glfw_gl_mouse_right_down() { return 0; }
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
String glfw_gl_take_edit_repeat() { return String(); }
void glfw_gl_mods_test_set(int32_t, int32_t, int32_t) {}
void glfw_gl_binding_key_test_push(String) {}

namespace {
std::deque<std::string>& stub_pending_drop_paths() {
  static std::deque<std::string> paths;
  return paths;
}
} // namespace

String glfw_gl_take_drop_path() {
  std::deque<std::string>& paths = stub_pending_drop_paths();
  if (paths.empty()) {
    return String();
  }
  String taken(paths.front());
  paths.pop_front();
  return taken;
}

void glfw_gl_drop_path_test_push(String path) {
  stub_pending_drop_paths().push_back(std::string(path.raw_data(), path.size()));
}

#endif

} // namespace gl
} // namespace mlc
