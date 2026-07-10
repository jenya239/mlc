#include "mlc/gl/glfw_gl_dispatch.hpp"

#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#define MLC_HAS_GLFW 1
#else
#define MLC_HAS_GLFW 0
#endif

#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace mlc {
namespace gl {

#if MLC_HAS_GLFW

namespace {

template <typename FunctionPointer>
FunctionPointer load_gl(const char* name) {
  return reinterpret_cast<FunctionPointer>(glfwGetProcAddress(name));
}

struct GlDispatch {
  void (*clear)(unsigned int) = nullptr;
  void (*clear_color)(float, float, float, float) = nullptr;
  void (*viewport)(int, int, int, int) = nullptr;
  void (*enable)(unsigned int) = nullptr;
  void (*blend_func)(unsigned int, unsigned int) = nullptr;

  void (*gen_textures)(int, unsigned int*) = nullptr;
  void (*bind_texture)(unsigned int, unsigned int) = nullptr;
  void (*tex_image_2d)(
    unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void*
  ) = nullptr;
  void (*tex_parameter_i)(unsigned int, unsigned int, int) = nullptr;
  void (*active_texture)(unsigned int) = nullptr;
  void (*delete_textures)(int, const unsigned int*) = nullptr;

  void (*gen_buffers)(int, unsigned int*) = nullptr;
  void (*bind_buffer)(unsigned int, unsigned int) = nullptr;
  void (*buffer_data)(unsigned int, std::ptrdiff_t, const void*, unsigned int) = nullptr;
  void (*delete_buffers)(int, const unsigned int*) = nullptr;
  void (*enable_vertex_attrib_array)(unsigned int) = nullptr;
  void (*vertex_attrib_pointer)(
    unsigned int, int, unsigned int, unsigned char, int, const void*
  ) = nullptr;

  unsigned int (*create_shader)(unsigned int) = nullptr;
  void (*shader_source)(unsigned int, int, const char* const*, const int*) = nullptr;
  void (*compile_shader)(unsigned int) = nullptr;
  void (*get_shader_iv)(unsigned int, unsigned int, int*) = nullptr;
  unsigned int (*create_program)() = nullptr;
  void (*attach_shader)(unsigned int, unsigned int) = nullptr;
  void (*link_program)(unsigned int) = nullptr;
  void (*get_program_iv)(unsigned int, unsigned int, int*) = nullptr;
  void (*use_program)(unsigned int) = nullptr;
  void (*delete_shader)(unsigned int) = nullptr;
  void (*delete_program)(unsigned int) = nullptr;
  int (*get_attrib_location)(unsigned int, const char*) = nullptr;
  int (*get_uniform_location)(unsigned int, const char*) = nullptr;
  void (*uniform_1i)(int, int) = nullptr;

  void (*draw_arrays)(unsigned int, int, int) = nullptr;

  bool load_all() {
    clear = load_gl<decltype(clear)>("glClear");
    clear_color = load_gl<decltype(clear_color)>("glClearColor");
    viewport = load_gl<decltype(viewport)>("glViewport");
    enable = load_gl<decltype(enable)>("glEnable");
    blend_func = load_gl<decltype(blend_func)>("glBlendFunc");

    gen_textures = load_gl<decltype(gen_textures)>("glGenTextures");
    bind_texture = load_gl<decltype(bind_texture)>("glBindTexture");
    tex_image_2d = load_gl<decltype(tex_image_2d)>("glTexImage2D");
    tex_parameter_i = load_gl<decltype(tex_parameter_i)>("glTexParameteri");
    active_texture = load_gl<decltype(active_texture)>("glActiveTexture");
    delete_textures = load_gl<decltype(delete_textures)>("glDeleteTextures");

    gen_buffers = load_gl<decltype(gen_buffers)>("glGenBuffers");
    bind_buffer = load_gl<decltype(bind_buffer)>("glBindBuffer");
    buffer_data = load_gl<decltype(buffer_data)>("glBufferData");
    delete_buffers = load_gl<decltype(delete_buffers)>("glDeleteBuffers");
    enable_vertex_attrib_array =
      load_gl<decltype(enable_vertex_attrib_array)>("glEnableVertexAttribArray");
    vertex_attrib_pointer =
      load_gl<decltype(vertex_attrib_pointer)>("glVertexAttribPointer");

    create_shader = load_gl<decltype(create_shader)>("glCreateShader");
    shader_source = load_gl<decltype(shader_source)>("glShaderSource");
    compile_shader = load_gl<decltype(compile_shader)>("glCompileShader");
    get_shader_iv = load_gl<decltype(get_shader_iv)>("glGetShaderiv");
    create_program = load_gl<decltype(create_program)>("glCreateProgram");
    attach_shader = load_gl<decltype(attach_shader)>("glAttachShader");
    link_program = load_gl<decltype(link_program)>("glLinkProgram");
    get_program_iv = load_gl<decltype(get_program_iv)>("glGetProgramiv");
    use_program = load_gl<decltype(use_program)>("glUseProgram");
    delete_shader = load_gl<decltype(delete_shader)>("glDeleteShader");
    delete_program = load_gl<decltype(delete_program)>("glDeleteProgram");
    get_attrib_location = load_gl<decltype(get_attrib_location)>("glGetAttribLocation");
    get_uniform_location =
      load_gl<decltype(get_uniform_location)>("glGetUniformLocation");
    uniform_1i = load_gl<decltype(uniform_1i)>("glUniform1i");

    draw_arrays = load_gl<decltype(draw_arrays)>("glDrawArrays");

    return clear && clear_color && viewport && enable && blend_func
      && gen_textures && bind_texture && tex_image_2d && tex_parameter_i
      && active_texture && delete_textures && gen_buffers && bind_buffer
      && buffer_data && delete_buffers && enable_vertex_attrib_array
      && vertex_attrib_pointer && create_shader && shader_source && compile_shader
      && get_shader_iv && create_program && attach_shader && link_program
      && get_program_iv && use_program && delete_shader && delete_program
      && get_attrib_location && get_uniform_location && uniform_1i && draw_arrays;
  }

  void clear_all() {
    *this = GlDispatch{};
  }
};

GlDispatch& dispatch() {
  static GlDispatch instance;
  return instance;
}

constexpr unsigned int kGlColorBufferBit = 0x00004000u;
constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr int kAutomatedFrameCount = 8;
constexpr double kInteractiveTimeoutSeconds = 5.0;

bool env_flag_enabled(const char* name) {
  const char* value = std::getenv(name);
  return value != nullptr && value[0] != '\0' && std::strcmp(value, "0") != 0;
}

} // namespace

int32_t glfw_gl_load() {
  GlDispatch& api = dispatch();
  api.clear_all();
  if (!api.load_all()) {
    api.clear_all();
    return -1;
  }
  return 0;
}

bool glfw_gl_is_loaded() {
  return dispatch().clear != nullptr;
}

void glfw_gl_unload() {
  dispatch().clear_all();
}

void gl_clear(uint32_t mask) {
  if (dispatch().clear != nullptr) {
    dispatch().clear(mask);
  }
}

void gl_clear_color(float red, float green, float blue, float alpha) {
  if (dispatch().clear_color != nullptr) {
    dispatch().clear_color(red, green, blue, alpha);
  }
}

void gl_viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
  if (dispatch().viewport != nullptr) {
    dispatch().viewport(x, y, width, height);
  }
}

void gl_enable(uint32_t capability) {
  if (dispatch().enable != nullptr) {
    dispatch().enable(capability);
  }
}

void gl_blend_func(uint32_t source_factor, uint32_t destination_factor) {
  if (dispatch().blend_func != nullptr) {
    dispatch().blend_func(source_factor, destination_factor);
  }
}

void gl_gen_textures(int32_t count, uint32_t* textures) {
  if (dispatch().gen_textures != nullptr) {
    dispatch().gen_textures(count, textures);
  }
}

void gl_bind_texture(uint32_t target, uint32_t texture) {
  if (dispatch().bind_texture != nullptr) {
    dispatch().bind_texture(target, texture);
  }
}

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
) {
  if (dispatch().tex_image_2d != nullptr) {
    dispatch().tex_image_2d(
      target, level, internal_format, width, height, border, format, type, data
    );
  }
}

void gl_tex_parameter_i(uint32_t target, uint32_t parameter_name, int32_t parameter_value) {
  if (dispatch().tex_parameter_i != nullptr) {
    dispatch().tex_parameter_i(target, parameter_name, parameter_value);
  }
}

void gl_active_texture(uint32_t texture_unit) {
  if (dispatch().active_texture != nullptr) {
    dispatch().active_texture(texture_unit);
  }
}

void gl_delete_textures(int32_t count, const uint32_t* textures) {
  if (dispatch().delete_textures != nullptr) {
    dispatch().delete_textures(count, textures);
  }
}

void gl_gen_buffers(int32_t count, uint32_t* buffers) {
  if (dispatch().gen_buffers != nullptr) {
    dispatch().gen_buffers(count, buffers);
  }
}

void gl_bind_buffer(uint32_t target, uint32_t buffer) {
  if (dispatch().bind_buffer != nullptr) {
    dispatch().bind_buffer(target, buffer);
  }
}

void gl_buffer_data(uint32_t target, int64_t size, const void* data, uint32_t usage) {
  if (dispatch().buffer_data != nullptr) {
    dispatch().buffer_data(target, static_cast<std::ptrdiff_t>(size), data, usage);
  }
}

void gl_delete_buffers(int32_t count, const uint32_t* buffers) {
  if (dispatch().delete_buffers != nullptr) {
    dispatch().delete_buffers(count, buffers);
  }
}

void gl_enable_vertex_attrib_array(uint32_t index) {
  if (dispatch().enable_vertex_attrib_array != nullptr) {
    dispatch().enable_vertex_attrib_array(index);
  }
}

void gl_vertex_attrib_pointer(
  uint32_t index,
  int32_t size,
  uint32_t type,
  uint8_t normalized,
  int32_t stride,
  const void* pointer
) {
  if (dispatch().vertex_attrib_pointer != nullptr) {
    dispatch().vertex_attrib_pointer(index, size, type, normalized, stride, pointer);
  }
}

uint32_t gl_create_shader(uint32_t shader_type) {
  if (dispatch().create_shader == nullptr) {
    return 0;
  }
  return dispatch().create_shader(shader_type);
}

void gl_shader_source(uint32_t shader, const char* source) {
  if (dispatch().shader_source != nullptr && source != nullptr) {
    dispatch().shader_source(shader, 1, &source, nullptr);
  }
}

void gl_compile_shader(uint32_t shader) {
  if (dispatch().compile_shader != nullptr) {
    dispatch().compile_shader(shader);
  }
}

void gl_get_shader_iv(uint32_t shader, uint32_t parameter_name, int32_t* out_value) {
  if (dispatch().get_shader_iv != nullptr && out_value != nullptr) {
    dispatch().get_shader_iv(shader, parameter_name, out_value);
  }
}

uint32_t gl_create_program() {
  if (dispatch().create_program == nullptr) {
    return 0;
  }
  return dispatch().create_program();
}

void gl_attach_shader(uint32_t program, uint32_t shader) {
  if (dispatch().attach_shader != nullptr) {
    dispatch().attach_shader(program, shader);
  }
}

void gl_link_program(uint32_t program) {
  if (dispatch().link_program != nullptr) {
    dispatch().link_program(program);
  }
}

void gl_get_program_iv(uint32_t program, uint32_t parameter_name, int32_t* out_value) {
  if (dispatch().get_program_iv != nullptr && out_value != nullptr) {
    dispatch().get_program_iv(program, parameter_name, out_value);
  }
}

void gl_use_program(uint32_t program) {
  if (dispatch().use_program != nullptr) {
    dispatch().use_program(program);
  }
}

void gl_delete_shader(uint32_t shader) {
  if (dispatch().delete_shader != nullptr) {
    dispatch().delete_shader(shader);
  }
}

void gl_delete_program(uint32_t program) {
  if (dispatch().delete_program != nullptr) {
    dispatch().delete_program(program);
  }
}

int32_t gl_get_attrib_location(uint32_t program, const char* name) {
  if (dispatch().get_attrib_location == nullptr || name == nullptr) {
    return -1;
  }
  return dispatch().get_attrib_location(program, name);
}

int32_t gl_get_uniform_location(uint32_t program, const char* name) {
  if (dispatch().get_uniform_location == nullptr || name == nullptr) {
    return -1;
  }
  return dispatch().get_uniform_location(program, name);
}

void gl_uniform_1i(int32_t location, int32_t value) {
  if (dispatch().uniform_1i != nullptr) {
    dispatch().uniform_1i(location, value);
  }
}

void gl_draw_arrays(uint32_t mode, int32_t first, int32_t count) {
  if (dispatch().draw_arrays != nullptr) {
    dispatch().draw_arrays(mode, first, count);
  }
}

int32_t glfw_gl_dispatch_clear_smoke() {
  if (glfwInit() != GLFW_TRUE) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  if (!env_flag_enabled("MLC_GLFW_VISIBLE")) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }

  GLFWwindow* window = glfwCreateWindow(
      kWindowWidth, kWindowHeight, "mlc glfw gl dispatch", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return -2;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  if (glfw_gl_load() != 0) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return -3;
  }

  // Exercise a few non-clear entry points so load is not clear-only.
  uint32_t buffer = 0;
  gl_gen_buffers(1, &buffer);
  if (buffer == 0) {
    glfw_gl_unload();
    glfwDestroyWindow(window);
    glfwTerminate();
    return -4;
  }
  gl_delete_buffers(1, &buffer);

  const bool interactive = env_flag_enabled("MLC_GLFW_VISIBLE");
  const double start_time = glfwGetTime();
  int frame_index = 0;

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    gl_viewport(0, 0, kWindowWidth, kWindowHeight);
    gl_clear_color(0.15f, 0.25f, 0.4f, 1.0f);
    gl_clear(kGlColorBufferBit);
    glfwSwapBuffers(window);
    glfwPollEvents();

    ++frame_index;
    if (!interactive && frame_index >= kAutomatedFrameCount) {
      break;
    }
    if (interactive && (glfwGetTime() - start_time) >= kInteractiveTimeoutSeconds) {
      break;
    }
  }

  glfw_gl_unload();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

#else

int32_t glfw_gl_load() { return -100; }
bool glfw_gl_is_loaded() { return false; }
void glfw_gl_unload() {}
int32_t glfw_gl_dispatch_clear_smoke() { return -100; }

void gl_clear(uint32_t) {}
void gl_clear_color(float, float, float, float) {}
void gl_viewport(int32_t, int32_t, int32_t, int32_t) {}
void gl_enable(uint32_t) {}
void gl_blend_func(uint32_t, uint32_t) {}
void gl_gen_textures(int32_t, uint32_t*) {}
void gl_bind_texture(uint32_t, uint32_t) {}
void gl_tex_image_2d(
  uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, const void*
) {}
void gl_tex_parameter_i(uint32_t, uint32_t, int32_t) {}
void gl_active_texture(uint32_t) {}
void gl_delete_textures(int32_t, const uint32_t*) {}
void gl_gen_buffers(int32_t, uint32_t*) {}
void gl_bind_buffer(uint32_t, uint32_t) {}
void gl_buffer_data(uint32_t, int64_t, const void*, uint32_t) {}
void gl_delete_buffers(int32_t, const uint32_t*) {}
void gl_enable_vertex_attrib_array(uint32_t) {}
void gl_vertex_attrib_pointer(
  uint32_t, int32_t, uint32_t, uint8_t, int32_t, const void*
) {}
uint32_t gl_create_shader(uint32_t) { return 0; }
void gl_shader_source(uint32_t, const char*) {}
void gl_compile_shader(uint32_t) {}
void gl_get_shader_iv(uint32_t, uint32_t, int32_t*) {}
uint32_t gl_create_program() { return 0; }
void gl_attach_shader(uint32_t, uint32_t) {}
void gl_link_program(uint32_t) {}
void gl_get_program_iv(uint32_t, uint32_t, int32_t*) {}
void gl_use_program(uint32_t) {}
void gl_delete_shader(uint32_t) {}
void gl_delete_program(uint32_t) {}
int32_t gl_get_attrib_location(uint32_t, const char*) { return -1; }
int32_t gl_get_uniform_location(uint32_t, const char*) { return -1; }
void gl_uniform_1i(int32_t, int32_t) {}
void gl_draw_arrays(uint32_t, int32_t, int32_t) {}

#endif

} // namespace gl
} // namespace mlc
