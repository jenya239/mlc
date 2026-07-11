#include "mlc/gl/glfw_gl_dispatch.hpp"

#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#define MLC_HAS_GLFW 1
#else
#define MLC_HAS_GLFW 0
#endif

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <vector>

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

  void clear_all() { *this = GlDispatch{}; }
};

GlDispatch& dispatch() {
  static GlDispatch instance;
  return instance;
}

std::vector<float>& scratch_f32() {
  static std::vector<float> values;
  return values;
}

std::vector<uint8_t>& scratch_u8() {
  static std::vector<uint8_t> values;
  return values;
}

GLFWwindow*& context_window() {
  static GLFWwindow* window = nullptr;
  return window;
}

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr int kAutomatedFrameCount = 8;
constexpr double kInteractiveTimeoutSeconds = 5.0;
constexpr int32_t kGlColorBufferBit = 0x00004000;

bool env_flag_enabled(const char* name) {
  const char* value = std::getenv(name);
  return value != nullptr && value[0] != '\0' && std::strcmp(value, "0") != 0;
}

unsigned int as_gl(int32_t value) {
  return static_cast<unsigned int>(value);
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

bool glfw_gl_is_loaded() { return dispatch().clear != nullptr; }

void glfw_gl_unload() { dispatch().clear_all(); }

void gl_clear(int32_t mask) {
  if (dispatch().clear != nullptr) {
    dispatch().clear(as_gl(mask));
  }
}

void gl_clear_color(float red, float green, float blue, float alpha) {
  if (dispatch().clear_color != nullptr) {
    dispatch().clear_color(red, green, blue, alpha);
  }
}

void gl_clear_color_f64(double red, double green, double blue, double alpha) {
  gl_clear_color(
    static_cast<float>(red),
    static_cast<float>(green),
    static_cast<float>(blue),
    static_cast<float>(alpha)
  );
}

void gl_viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
  if (dispatch().viewport != nullptr) {
    dispatch().viewport(x, y, width, height);
  }
}

void gl_enable(int32_t capability) {
  if (dispatch().enable != nullptr) {
    dispatch().enable(as_gl(capability));
  }
}

void gl_blend_func(int32_t source_factor, int32_t destination_factor) {
  if (dispatch().blend_func != nullptr) {
    dispatch().blend_func(as_gl(source_factor), as_gl(destination_factor));
  }
}

void gl_gen_textures(int32_t count, uint32_t* textures) {
  if (dispatch().gen_textures != nullptr) {
    dispatch().gen_textures(count, textures);
  }
}

void gl_bind_texture(int32_t target, int32_t texture) {
  if (dispatch().bind_texture != nullptr) {
    dispatch().bind_texture(as_gl(target), as_gl(texture));
  }
}

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
) {
  if (dispatch().tex_image_2d != nullptr) {
    dispatch().tex_image_2d(
      as_gl(target), level, internal_format, width, height, border,
      as_gl(format), as_gl(type), data
    );
  }
}

void gl_tex_parameter_i(int32_t target, int32_t parameter_name, int32_t parameter_value) {
  if (dispatch().tex_parameter_i != nullptr) {
    dispatch().tex_parameter_i(as_gl(target), as_gl(parameter_name), parameter_value);
  }
}

void gl_active_texture(int32_t texture_unit) {
  if (dispatch().active_texture != nullptr) {
    dispatch().active_texture(as_gl(texture_unit));
  }
}

void gl_delete_textures(int32_t count, const uint32_t* textures) {
  if (dispatch().delete_textures != nullptr) {
    dispatch().delete_textures(count, textures);
  }
}

int32_t gl_create_shader(int32_t shader_type) {
  if (dispatch().create_shader == nullptr) {
    return 0;
  }
  return static_cast<int32_t>(dispatch().create_shader(as_gl(shader_type)));
}

void gl_shader_source_string(int32_t shader, String source) {
  if (dispatch().shader_source == nullptr) {
    return;
  }
  const char* text = source.c_str();
  dispatch().shader_source(as_gl(shader), 1, &text, nullptr);
}

void gl_compile_shader(int32_t shader) {
  if (dispatch().compile_shader != nullptr) {
    dispatch().compile_shader(as_gl(shader));
  }
}

int32_t gl_get_shader_iv_value(int32_t shader, int32_t parameter_name) {
  int32_t value = 0;
  if (dispatch().get_shader_iv != nullptr) {
    dispatch().get_shader_iv(as_gl(shader), as_gl(parameter_name), &value);
  }
  return value;
}

int32_t gl_create_program() {
  if (dispatch().create_program == nullptr) {
    return 0;
  }
  return static_cast<int32_t>(dispatch().create_program());
}

void gl_attach_shader(int32_t program, int32_t shader) {
  if (dispatch().attach_shader != nullptr) {
    dispatch().attach_shader(as_gl(program), as_gl(shader));
  }
}

void gl_link_program(int32_t program) {
  if (dispatch().link_program != nullptr) {
    dispatch().link_program(as_gl(program));
  }
}

int32_t gl_get_program_iv_value(int32_t program, int32_t parameter_name) {
  int32_t value = 0;
  if (dispatch().get_program_iv != nullptr) {
    dispatch().get_program_iv(as_gl(program), as_gl(parameter_name), &value);
  }
  return value;
}

void gl_use_program(int32_t program) {
  if (dispatch().use_program != nullptr) {
    dispatch().use_program(as_gl(program));
  }
}

void gl_delete_shader(int32_t shader) {
  if (dispatch().delete_shader != nullptr) {
    dispatch().delete_shader(as_gl(shader));
  }
}

void gl_delete_program(int32_t program) {
  if (dispatch().delete_program != nullptr) {
    dispatch().delete_program(as_gl(program));
  }
}

int32_t gl_get_attrib_location_string(int32_t program, String name) {
  if (dispatch().get_attrib_location == nullptr) {
    return -1;
  }
  return dispatch().get_attrib_location(as_gl(program), name.c_str());
}

void gl_uniform_1i(int32_t location, int32_t value) {
  if (dispatch().uniform_1i != nullptr) {
    dispatch().uniform_1i(location, value);
  }
}

int32_t gl_gen_buffer() {
  unsigned int buffer = 0;
  if (dispatch().gen_buffers != nullptr) {
    dispatch().gen_buffers(1, &buffer);
  }
  return static_cast<int32_t>(buffer);
}

void gl_delete_buffer(int32_t buffer) {
  if (dispatch().delete_buffers != nullptr) {
    const unsigned int handle = as_gl(buffer);
    dispatch().delete_buffers(1, &handle);
  }
}

void gl_bind_buffer(int32_t target, int32_t buffer) {
  if (dispatch().bind_buffer != nullptr) {
    dispatch().bind_buffer(as_gl(target), as_gl(buffer));
  }
}

void gl_scratch_f32_clear() { scratch_f32().clear(); }

void gl_scratch_f32_push(double value) {
  scratch_f32().push_back(static_cast<float>(value));
}

int32_t gl_buffer_data_scratch(int32_t target, int32_t usage) {
  const std::vector<float>& values = scratch_f32();
  if (values.empty() || dispatch().buffer_data == nullptr) {
    return -1;
  }
  dispatch().buffer_data(
    as_gl(target),
    static_cast<std::ptrdiff_t>(values.size() * sizeof(float)),
    values.data(),
    as_gl(usage)
  );
  return 0;
}

void gl_enable_vertex_attrib_array(int32_t index) {
  if (dispatch().enable_vertex_attrib_array != nullptr) {
    dispatch().enable_vertex_attrib_array(as_gl(index));
  }
}

void gl_vertex_attrib_pointer_offset(
  int32_t index,
  int32_t size,
  int32_t attribute_type,
  int32_t normalized,
  int32_t stride,
  int32_t byte_offset
) {
  if (dispatch().vertex_attrib_pointer != nullptr) {
    dispatch().vertex_attrib_pointer(
      as_gl(index),
      size,
      as_gl(attribute_type),
      static_cast<unsigned char>(normalized),
      stride,
      reinterpret_cast<const void*>(static_cast<std::ptrdiff_t>(byte_offset))
    );
  }
}

void gl_draw_arrays(int32_t mode, int32_t first, int32_t count) {
  if (dispatch().draw_arrays != nullptr) {
    dispatch().draw_arrays(as_gl(mode), first, count);
  }
}

int32_t gl_gen_texture() {
  unsigned int texture = 0;
  if (dispatch().gen_textures != nullptr) {
    dispatch().gen_textures(1, &texture);
  }
  return static_cast<int32_t>(texture);
}

void gl_delete_texture(int32_t texture) {
  if (dispatch().delete_textures != nullptr) {
    const unsigned int handle = as_gl(texture);
    dispatch().delete_textures(1, &handle);
  }
}

int32_t gl_get_uniform_location_string(int32_t program, String name) {
  if (dispatch().get_uniform_location == nullptr) {
    return -1;
  }
  return dispatch().get_uniform_location(as_gl(program), name.c_str());
}

void gl_scratch_u8_clear() { scratch_u8().clear(); }

void gl_scratch_u8_resize_zero(int32_t byte_count) {
  if (byte_count < 0) {
    scratch_u8().clear();
    return;
  }
  scratch_u8().assign(static_cast<size_t>(byte_count), 0);
}

uint8_t* gl_scratch_u8_mutable_data() {
  if (scratch_u8().empty()) {
    return nullptr;
  }
  return scratch_u8().data();
}

int32_t gl_scratch_u8_size() {
  return static_cast<int32_t>(scratch_u8().size());
}

void gl_scratch_u8_fill_rect(
  int32_t atlas_width,
  int32_t x,
  int32_t y,
  int32_t width,
  int32_t height,
  int32_t value
) {
  if (atlas_width <= 0 || width <= 0 || height <= 0) {
    return;
  }
  std::vector<uint8_t>& pixels = scratch_u8();
  const uint8_t byte_value = static_cast<uint8_t>(value & 0xff);
  for (int32_t row = 0; row < height; row += 1) {
    const int32_t atlas_y = y + row;
    for (int32_t column = 0; column < width; column += 1) {
      const size_t index =
        static_cast<size_t>(atlas_y * atlas_width + (x + column));
      if (index < pixels.size()) {
        pixels[index] = byte_value;
      }
    }
  }
}

int32_t gl_tex_image_2d_scratch_luminance(int32_t width, int32_t height) {
  constexpr int32_t kGlTexture2d = 0x0DE1;
  constexpr int32_t kGlLuminance = 0x1909;
  constexpr int32_t kGlUnsignedByte = 0x1401;
  constexpr int32_t kGlTextureMinFilter = 0x2801;
  constexpr int32_t kGlTextureMagFilter = 0x2800;
  constexpr int32_t kGlNearest = 0x2600;
  const std::vector<uint8_t>& pixels = scratch_u8();
  const size_t expected = static_cast<size_t>(width) * static_cast<size_t>(height);
  if (width <= 0 || height <= 0 || pixels.size() < expected) {
    return -1;
  }
  if (dispatch().tex_image_2d == nullptr || dispatch().tex_parameter_i == nullptr) {
    return -2;
  }
  dispatch().tex_image_2d(
    as_gl(kGlTexture2d),
    0,
    kGlLuminance,
    width,
    height,
    0,
    as_gl(kGlLuminance),
    as_gl(kGlUnsignedByte),
    pixels.data()
  );
  dispatch().tex_parameter_i(as_gl(kGlTexture2d), as_gl(kGlTextureMinFilter), kGlNearest);
  dispatch().tex_parameter_i(as_gl(kGlTexture2d), as_gl(kGlTextureMagFilter), kGlNearest);
  return 0;
}

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
) {
  if (atlas_size <= 0) {
    return;
  }
  const double atlas = static_cast<double>(atlas_size);
  const double u0 = static_cast<double>(slot_x) / atlas;
  const double v0 = static_cast<double>(slot_y) / atlas;
  const double u1 = static_cast<double>(slot_x + slot_width) / atlas;
  const double v1 = static_cast<double>(slot_y + slot_height) / atlas;
  auto& values = scratch_f32();
  const float verts[] = {
    static_cast<float>(left), static_cast<float>(bottom), static_cast<float>(u0), static_cast<float>(v1),
    static_cast<float>(right), static_cast<float>(bottom), static_cast<float>(u1), static_cast<float>(v1),
    static_cast<float>(left), static_cast<float>(top), static_cast<float>(u0), static_cast<float>(v0),
    static_cast<float>(right), static_cast<float>(bottom), static_cast<float>(u1), static_cast<float>(v1),
    static_cast<float>(right), static_cast<float>(top), static_cast<float>(u1), static_cast<float>(v0),
    static_cast<float>(left), static_cast<float>(top), static_cast<float>(u0), static_cast<float>(v0),
  };
  values.insert(values.end(), std::begin(verts), std::end(verts));
}

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
  const int window_width = width > 0 ? width : kWindowWidth;
  const int window_height = height > 0 ? height : kWindowHeight;
  GLFWwindow* window = glfwCreateWindow(
      window_width, window_height, "mlc gl renderer", nullptr, nullptr);
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
  glfw_gl_unload();
  glfwDestroyWindow(window);
  context_window() = nullptr;
  glfwTerminate();
}

double glfw_gl_get_time() {
  return glfwGetTime();
}

double glfw_gl_anim_unit() {
  return 0.5 + 0.5 * std::sin(glfwGetTime() * 2.0);
}

namespace {

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

int32_t glfw_gl_dispatch_clear_smoke() {
  const int32_t begin_status = glfw_gl_context_begin(kWindowWidth, kWindowHeight);
  if (begin_status != 0) {
    return begin_status;
  }
  const int32_t buffer = gl_gen_buffer();
  if (buffer == 0) {
    glfw_gl_context_end();
    return -4;
  }
  gl_delete_buffer(buffer);
  const bool interactive = env_flag_enabled("MLC_GLFW_VISIBLE");
  const double start_time = glfwGetTime();
  int frame_index = 0;
  while (glfw_gl_context_should_close() == 0) {
    gl_viewport(0, 0, kWindowWidth, kWindowHeight);
    gl_clear_color(0.15f, 0.25f, 0.4f, 1.0f);
    gl_clear(kGlColorBufferBit);
    glfw_gl_context_swap_poll();
    ++frame_index;
    if (!interactive && frame_index >= kAutomatedFrameCount) {
      break;
    }
    if (interactive && (glfwGetTime() - start_time) >= kInteractiveTimeoutSeconds) {
      break;
    }
  }
  glfw_gl_context_end();
  return 0;
}

#else

int32_t glfw_gl_load() { return -100; }
bool glfw_gl_is_loaded() { return false; }
void glfw_gl_unload() {}
int32_t glfw_gl_dispatch_clear_smoke() { return -100; }
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

void gl_clear(int32_t) {}
void gl_clear_color(float, float, float, float) {}
void gl_clear_color_f64(double, double, double, double) {}
void gl_viewport(int32_t, int32_t, int32_t, int32_t) {}
void gl_enable(int32_t) {}
void gl_blend_func(int32_t, int32_t) {}
int32_t gl_create_shader(int32_t) { return 0; }
void gl_shader_source_string(int32_t, String) {}
void gl_compile_shader(int32_t) {}
int32_t gl_get_shader_iv_value(int32_t, int32_t) { return 0; }
int32_t gl_create_program() { return 0; }
void gl_attach_shader(int32_t, int32_t) {}
void gl_link_program(int32_t) {}
int32_t gl_get_program_iv_value(int32_t, int32_t) { return 0; }
void gl_use_program(int32_t) {}
void gl_delete_shader(int32_t) {}
void gl_delete_program(int32_t) {}
int32_t gl_get_attrib_location_string(int32_t, String) { return -1; }
void gl_uniform_1i(int32_t, int32_t) {}
int32_t gl_gen_buffer() { return 0; }
void gl_delete_buffer(int32_t) {}
void gl_bind_buffer(int32_t, int32_t) {}
void gl_scratch_f32_clear() {}
void gl_scratch_f32_push(double) {}
int32_t gl_buffer_data_scratch(int32_t, int32_t) { return -100; }
void gl_enable_vertex_attrib_array(int32_t) {}
void gl_vertex_attrib_pointer_offset(
  int32_t, int32_t, int32_t, int32_t, int32_t, int32_t
) {}
void gl_draw_arrays(int32_t, int32_t, int32_t) {}
int32_t gl_gen_texture() { return 0; }
void gl_delete_texture(int32_t) {}
int32_t gl_get_uniform_location_string(int32_t, String) { return -1; }
void gl_scratch_u8_clear() {}
void gl_scratch_u8_resize_zero(int32_t) {}
uint8_t* gl_scratch_u8_mutable_data() { return nullptr; }
int32_t gl_scratch_u8_size() { return 0; }
void gl_scratch_u8_fill_rect(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) {}
int32_t gl_tex_image_2d_scratch_luminance(int32_t, int32_t) { return -100; }
void gl_scratch_push_glyph_quad(
  double, double, double, double, int32_t, int32_t, int32_t, int32_t, int32_t
) {}
void gl_gen_textures(int32_t, uint32_t*) {}
void gl_bind_texture(int32_t, int32_t) {}
void gl_tex_image_2d(
  int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, const void*
) {}
void gl_tex_parameter_i(int32_t, int32_t, int32_t) {}
void gl_active_texture(int32_t) {}
void gl_delete_textures(int32_t, const uint32_t*) {}

#endif

} // namespace gl
} // namespace mlc
