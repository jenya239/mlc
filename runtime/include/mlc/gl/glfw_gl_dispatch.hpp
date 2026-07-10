#pragma once

#include <cstdint>

namespace mlc {
namespace gl {

// Thin GL dispatch via glfwGetProcAddress (TRACK_TEXT_RENDERING_NATIVE STEP=3).
// Call glfw_gl_load() only after glfwMakeContextCurrent.
// Returns 0 on success; negative if any locked entry point is missing.
int32_t glfw_gl_load();

// True after a successful glfw_gl_load(); false after unload or failed load.
bool glfw_gl_is_loaded();

// Clear function pointers (does not touch the GL context).
void glfw_gl_unload();

// GLFW window + load locked set + clear/poll/swap smoke (hidden by default).
// Returns 0 on success; negative on failure. MLC_GLFW_VISIBLE=1 for interactive.
int32_t glfw_gl_dispatch_clear_smoke();

// --- Locked GL set (plain C ABI for MLC extern fn) ---

void gl_clear(uint32_t mask);
void gl_clear_color(float red, float green, float blue, float alpha);
void gl_viewport(int32_t x, int32_t y, int32_t width, int32_t height);
void gl_enable(uint32_t capability);
void gl_blend_func(uint32_t source_factor, uint32_t destination_factor);

void gl_gen_textures(int32_t count, uint32_t* textures);
void gl_bind_texture(uint32_t target, uint32_t texture);
void gl_tex_image_2d(
  uint32_t target,
  int32_t level,
  int32_t internal_format,
  int32_t width,
  int32_t height,
  int32_t border,
  uint32_t format,
  uint32_t type,
  const void* data
);
void gl_tex_parameter_i(uint32_t target, uint32_t parameter_name, int32_t parameter_value);
void gl_active_texture(uint32_t texture_unit);
void gl_delete_textures(int32_t count, const uint32_t* textures);

void gl_gen_buffers(int32_t count, uint32_t* buffers);
void gl_bind_buffer(uint32_t target, uint32_t buffer);
void gl_buffer_data(uint32_t target, int64_t size, const void* data, uint32_t usage);
void gl_delete_buffers(int32_t count, const uint32_t* buffers);
void gl_enable_vertex_attrib_array(uint32_t index);
void gl_vertex_attrib_pointer(
  uint32_t index,
  int32_t size,
  uint32_t type,
  uint8_t normalized,
  int32_t stride,
  const void* pointer
);

uint32_t gl_create_shader(uint32_t shader_type);
void gl_shader_source(uint32_t shader, const char* source);
void gl_compile_shader(uint32_t shader);
void gl_get_shader_iv(uint32_t shader, uint32_t parameter_name, int32_t* out_value);
uint32_t gl_create_program();
void gl_attach_shader(uint32_t program, uint32_t shader);
void gl_link_program(uint32_t program);
void gl_get_program_iv(uint32_t program, uint32_t parameter_name, int32_t* out_value);
void gl_use_program(uint32_t program);
void gl_delete_shader(uint32_t shader);
void gl_delete_program(uint32_t program);
int32_t gl_get_attrib_location(uint32_t program, const char* name);
int32_t gl_get_uniform_location(uint32_t program, const char* name);
void gl_uniform_1i(int32_t location, int32_t value);

void gl_draw_arrays(uint32_t mode, int32_t first, int32_t count);

} // namespace gl
} // namespace mlc
