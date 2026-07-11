#pragma once

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace gl {

// Thin GL dispatch via glfwGetProcAddress (TRACK_TEXT_RENDERING_NATIVE STEP=3/4).
// Call glfw_gl_load() only after glfwMakeContextCurrent.
// MLC-facing ABI uses int32_t / String-by-value (mlcc extern binding).

int32_t glfw_gl_load();
bool glfw_gl_is_loaded();
void glfw_gl_unload();
int32_t glfw_gl_dispatch_clear_smoke();

int32_t glfw_gl_context_begin(int32_t width, int32_t height);
int32_t glfw_gl_context_should_close();
void glfw_gl_context_swap_poll();
void glfw_gl_context_end();
double glfw_gl_get_time();
// 0..1 oscillating phase from sin(time); for demo color animation.
double glfw_gl_anim_unit();

// Window content coords (origin top-left, y down). Requires active context.
int32_t glfw_gl_mouse_x();
int32_t glfw_gl_mouse_y();
// 1 if left mouse button currently pressed.
int32_t glfw_gl_mouse_left_down();
// 1 if Escape currently pressed.
int32_t glfw_gl_key_escape_down();
// Automated smoke: override polled input until cleared (1=on).
void glfw_gl_input_test_set(
  int32_t mouse_x,
  int32_t mouse_y,
  int32_t mouse_left_down,
  int32_t escape_down
);
void glfw_gl_input_test_clear();

// --- MLC-facing GL surface (int32_t / String) ---

void gl_clear(int32_t mask);
void gl_clear_color(float red, float green, float blue, float alpha);
void gl_clear_color_f64(double red, double green, double blue, double alpha);
void gl_viewport(int32_t x, int32_t y, int32_t width, int32_t height);
void gl_enable(int32_t capability);
void gl_blend_func(int32_t source_factor, int32_t destination_factor);

int32_t gl_create_shader(int32_t shader_type);
void gl_shader_source_string(int32_t shader, String source);
void gl_compile_shader(int32_t shader);
int32_t gl_get_shader_iv_value(int32_t shader, int32_t parameter_name);
int32_t gl_create_program();
void gl_attach_shader(int32_t program, int32_t shader);
void gl_link_program(int32_t program);
int32_t gl_get_program_iv_value(int32_t program, int32_t parameter_name);
void gl_use_program(int32_t program);
void gl_delete_shader(int32_t shader);
void gl_delete_program(int32_t program);
int32_t gl_get_attrib_location_string(int32_t program, String name);
void gl_uniform_1i(int32_t location, int32_t value);

int32_t gl_gen_buffer();
void gl_delete_buffer(int32_t buffer);
void gl_bind_buffer(int32_t target, int32_t buffer);
void gl_scratch_f32_clear();
void gl_scratch_f32_push(double value);
int32_t gl_buffer_data_scratch(int32_t target, int32_t usage);
void gl_enable_vertex_attrib_array(int32_t index);
void gl_vertex_attrib_pointer_offset(
  int32_t index,
  int32_t size,
  int32_t attribute_type,
  int32_t normalized,
  int32_t stride,
  int32_t byte_offset
);
void gl_draw_arrays(int32_t mode, int32_t first, int32_t count);

int32_t gl_gen_texture();
void gl_delete_texture(int32_t texture);
int32_t gl_get_uniform_location_string(int32_t program, String name);
void gl_scratch_u8_clear();
void gl_scratch_u8_resize_zero(int32_t byte_count);
uint8_t* gl_scratch_u8_mutable_data();
int32_t gl_scratch_u8_size();
void gl_scratch_u8_fill_rect(
  int32_t atlas_width,
  int32_t x,
  int32_t y,
  int32_t width,
  int32_t height,
  int32_t value
);
int32_t gl_tex_image_2d_scratch_luminance(int32_t width, int32_t height);
void gl_scratch_push_glyph_quad(
  double left,
  double bottom,
  double right,
  double top,
  int32_t atlas_size,
  int32_t slot_x,
  int32_t slot_y,
  int32_t slot_width,
  int32_t slot_height
);

// Texture helpers (int32_t ids; pointer out-params for multi-gen).
void gl_gen_textures(int32_t count, uint32_t* textures);
void gl_bind_texture(int32_t target, int32_t texture);
void gl_tex_image_2d(
  int32_t target,
  int32_t level,
  int32_t internal_format,
  int32_t width,
  int32_t height,
  int32_t border,
  int32_t format,
  int32_t type,
  const void* data
);
void gl_tex_parameter_i(int32_t target, int32_t parameter_name, int32_t parameter_value);
void gl_active_texture(int32_t texture_unit);
void gl_delete_textures(int32_t count, const uint32_t* textures);

} // namespace gl
} // namespace mlc
