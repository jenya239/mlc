#pragma once

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace gl {

// GLFW window/context/input only (TRACK_GL_GLAD_MIGRATION STEP=5).
// GL entry points live in glad_gl_abi / vendored glad.

int32_t glfw_gl_context_begin(int32_t width, int32_t height);
int32_t glfw_gl_context_should_close();
void glfw_gl_context_swap_poll();
// TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU STEP=2 — idle path (WaitEventsTimeout).
void glfw_gl_context_wait_events_timeout(double timeout_seconds);
void glfw_gl_context_end();
double glfw_gl_get_time();
double glfw_gl_anim_unit();
int32_t glfw_gl_window_width();
int32_t glfw_gl_window_height();
void glfw_gl_window_set_size(int32_t width, int32_t height);
// TRACK_GLFW_CONTENT_SCALE STEP=1
double glfw_gl_window_content_scale_x();
double glfw_gl_window_content_scale_y();

int32_t glfw_gl_mouse_x();
int32_t glfw_gl_mouse_y();
int32_t glfw_gl_mouse_left_down();
int32_t glfw_gl_mouse_right_down();
double glfw_gl_take_scroll_y();
int32_t glfw_gl_key_escape_down();
int32_t glfw_gl_key_backspace_down();
int32_t glfw_gl_key_enter_down();
String glfw_gl_take_text();
void glfw_gl_text_test_push(String text);
void glfw_gl_keys_test_set(int32_t backspace_down, int32_t enter_down);
void glfw_gl_input_test_set(
  int32_t mouse_x,
  int32_t mouse_y,
  int32_t mouse_left_down,
  int32_t escape_down
);
void glfw_gl_input_test_clear();

// shape: 0 arrow, 1 ew-resize, 2 ibeam (TRACK_EDITOR_LIVE_POLISH STEP=3)
void glfw_gl_cursor_set(int32_t shape);
int32_t glfw_gl_cursor_shape_get();

// TRACK_EDITOR_LIVE_POLISH STEP=4
String glfw_gl_clipboard_get();
void glfw_gl_clipboard_set(String text);
void glfw_gl_clipboard_test_set(String text);

// TRACK_EDITOR_LIVE_POLISH STEP=5 — command-bus chords
int32_t glfw_gl_mod_ctrl_down();
int32_t glfw_gl_mod_shift_down();
int32_t glfw_gl_mod_alt_down();
String glfw_gl_take_binding_key();
void glfw_gl_mods_test_set(int32_t ctrl_down, int32_t shift_down, int32_t alt_down);
void glfw_gl_binding_key_test_push(String key);

// TRACK_EDITOR_DROP_FILE STEP=2 — OS drag-drop path queue
String glfw_gl_take_drop_path();
void glfw_gl_drop_path_test_push(String path);

} // namespace gl
} // namespace mlc
