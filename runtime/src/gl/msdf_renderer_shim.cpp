#include "mlc/gl/msdf_renderer_shim.hpp"
#include "mlc/text/msdf_bridge.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#include <cstdint>
#include <cstring>
#include <vector>

namespace mlc::gl {
namespace {

template <typename FunctionPointer>
FunctionPointer load_gl(const char* name) {
  return reinterpret_cast<FunctionPointer>(eglGetProcAddress(name));
}

struct GlApi {
  void (*gen_textures)(GLsizei, GLuint*) = nullptr;
  void (*bind_texture)(GLenum, GLuint) = nullptr;
  void (*tex_image_2d)(
    GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*
  ) = nullptr;
  void (*tex_parameter_i)(GLenum, GLenum, GLint) = nullptr;
  void (*gen_framebuffers)(GLsizei, GLuint*) = nullptr;
  void (*bind_framebuffer)(GLenum, GLuint) = nullptr;
  void (*framebuffer_texture_2d)(GLenum, GLenum, GLenum, GLuint, GLint) = nullptr;
  GLenum (*check_framebuffer_status)(GLenum) = nullptr;
  GLuint (*create_shader)(GLenum) = nullptr;
  void (*shader_source)(GLuint, GLsizei, const char* const*, const GLint*) = nullptr;
  void (*compile_shader)(GLuint) = nullptr;
  void (*get_shader_iv)(GLuint, GLenum, GLint*) = nullptr;
  GLuint (*create_program)() = nullptr;
  void (*attach_shader)(GLuint, GLuint) = nullptr;
  void (*link_program)(GLuint) = nullptr;
  void (*get_program_iv)(GLuint, GLenum, GLint*) = nullptr;
  void (*use_program)(GLuint) = nullptr;
  GLint (*get_attrib_location)(GLuint, const char*) = nullptr;
  GLint (*get_uniform_location)(GLuint, const char*) = nullptr;
  void (*uniform_1f)(GLint, GLfloat) = nullptr;
  void (*gen_buffers)(GLsizei, GLuint*) = nullptr;
  void (*bind_buffer)(GLenum, GLuint) = nullptr;
  void (*buffer_data)(GLenum, GLsizeiptr, const void*, GLenum) = nullptr;
  void (*enable_vertex_attrib_array)(GLuint) = nullptr;
  void (*vertex_attrib_pointer)(
    GLuint, GLint, GLenum, GLboolean, GLsizei, const void*
  ) = nullptr;
  void (*draw_arrays)(GLenum, GLint, GLsizei) = nullptr;
  void (*clear)(GLbitfield) = nullptr;
  void (*clear_color)(GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
  void (*viewport)(GLint, GLint, GLsizei, GLsizei) = nullptr;
  void (*read_pixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*) = nullptr;
  void (*active_texture)(GLenum) = nullptr;
  void (*uniform_1i)(GLint, GLint) = nullptr;
  void (*enable)(GLenum) = nullptr;
  void (*blend_func)(GLenum, GLenum) = nullptr;
  void (*delete_textures)(GLsizei, const GLuint*) = nullptr;
  void (*delete_framebuffers)(GLsizei, const GLuint*) = nullptr;
  void (*delete_buffers)(GLsizei, const GLuint*) = nullptr;
  void (*delete_program)(GLuint) = nullptr;
  void (*delete_shader)(GLuint) = nullptr;

  bool load_all() {
    gen_textures = load_gl<decltype(gen_textures)>("glGenTextures");
    bind_texture = load_gl<decltype(bind_texture)>("glBindTexture");
    tex_image_2d = load_gl<decltype(tex_image_2d)>("glTexImage2D");
    tex_parameter_i = load_gl<decltype(tex_parameter_i)>("glTexParameteri");
    gen_framebuffers = load_gl<decltype(gen_framebuffers)>("glGenFramebuffers");
    bind_framebuffer = load_gl<decltype(bind_framebuffer)>("glBindFramebuffer");
    framebuffer_texture_2d =
      load_gl<decltype(framebuffer_texture_2d)>("glFramebufferTexture2D");
    check_framebuffer_status =
      load_gl<decltype(check_framebuffer_status)>("glCheckFramebufferStatus");
    create_shader = load_gl<decltype(create_shader)>("glCreateShader");
    shader_source = load_gl<decltype(shader_source)>("glShaderSource");
    compile_shader = load_gl<decltype(compile_shader)>("glCompileShader");
    get_shader_iv = load_gl<decltype(get_shader_iv)>("glGetShaderiv");
    create_program = load_gl<decltype(create_program)>("glCreateProgram");
    attach_shader = load_gl<decltype(attach_shader)>("glAttachShader");
    link_program = load_gl<decltype(link_program)>("glLinkProgram");
    get_program_iv = load_gl<decltype(get_program_iv)>("glGetProgramiv");
    use_program = load_gl<decltype(use_program)>("glUseProgram");
    get_attrib_location = load_gl<decltype(get_attrib_location)>("glGetAttribLocation");
    get_uniform_location =
      load_gl<decltype(get_uniform_location)>("glGetUniformLocation");
    uniform_1f = load_gl<decltype(uniform_1f)>("glUniform1f");
    gen_buffers = load_gl<decltype(gen_buffers)>("glGenBuffers");
    bind_buffer = load_gl<decltype(bind_buffer)>("glBindBuffer");
    buffer_data = load_gl<decltype(buffer_data)>("glBufferData");
    enable_vertex_attrib_array =
      load_gl<decltype(enable_vertex_attrib_array)>("glEnableVertexAttribArray");
    vertex_attrib_pointer =
      load_gl<decltype(vertex_attrib_pointer)>("glVertexAttribPointer");
    draw_arrays = load_gl<decltype(draw_arrays)>("glDrawArrays");
    clear = load_gl<decltype(clear)>("glClear");
    clear_color = load_gl<decltype(clear_color)>("glClearColor");
    viewport = load_gl<decltype(viewport)>("glViewport");
    read_pixels = load_gl<decltype(read_pixels)>("glReadPixels");
    active_texture = load_gl<decltype(active_texture)>("glActiveTexture");
    uniform_1i = load_gl<decltype(uniform_1i)>("glUniform1i");
    enable = load_gl<decltype(enable)>("glEnable");
    blend_func = load_gl<decltype(blend_func)>("glBlendFunc");
    delete_textures = load_gl<decltype(delete_textures)>("glDeleteTextures");
    delete_framebuffers = load_gl<decltype(delete_framebuffers)>("glDeleteFramebuffers");
    delete_buffers = load_gl<decltype(delete_buffers)>("glDeleteBuffers");
    delete_program = load_gl<decltype(delete_program)>("glDeleteProgram");
    delete_shader = load_gl<decltype(delete_shader)>("glDeleteShader");
    return gen_textures && bind_texture && tex_image_2d && tex_parameter_i
      && gen_framebuffers && bind_framebuffer && framebuffer_texture_2d
      && check_framebuffer_status && create_shader && shader_source && compile_shader
      && get_shader_iv && create_program && attach_shader && link_program
      && get_program_iv && use_program && get_attrib_location && get_uniform_location
      && uniform_1f && gen_buffers && bind_buffer && buffer_data
      && enable_vertex_attrib_array && vertex_attrib_pointer && draw_arrays && clear
      && clear_color && viewport && read_pixels && active_texture && uniform_1i
      && enable && blend_func && delete_textures && delete_framebuffers
      && delete_buffers && delete_program && delete_shader;
  }
};

EGLDisplay open_display() {
#if defined(EGL_MESA_platform_surfaceless)
  auto get_platform_display =
    reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
      eglGetProcAddress("eglGetPlatformDisplayEXT"));
  if (get_platform_display != nullptr) {
    EGLDisplay display = get_platform_display(
      EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
    if (display != EGL_NO_DISPLAY) {
      return display;
    }
  }
#endif
  return eglGetDisplay(EGL_DEFAULT_DISPLAY);
}

} // namespace

int32_t msdf_renderer_fbo_smoke(
  mlc::String font_path,
  int32_t codepoint,
  int32_t field_size,
  int32_t px_range
) {
  (void)font_path;
  (void)codepoint;
  (void)field_size;
  (void)px_range;
  // RGB cache must already be filled by MLC msdf_generate (misc/gui/msdf.mlc).
  const int32_t width = mlc::text::msdf_width();
  const int32_t height = mlc::text::msdf_height();
  const uint8_t* rgb = mlc::text::msdf_rgb_data();
  if (rgb == nullptr || width <= 0 || height <= 0) {
    return -2;
  }

  constexpr int32_t frame_size = 64;
  EGLDisplay display = open_display();
  if (display == EGL_NO_DISPLAY) {
    return -3;
  }
  if (!eglInitialize(display, nullptr, nullptr)) {
    return -4;
  }
  EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };
  EGLConfig config = nullptr;
  EGLint config_count = 0;
  if (!eglChooseConfig(display, config_attribs, &config, 1, &config_count)
      || config_count < 1) {
    EGLint config_attribs_any[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE};
    if (!eglChooseConfig(display, config_attribs_any, &config, 1, &config_count)
        || config_count < 1) {
      eglTerminate(display);
      return -5;
    }
  }
  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
  if (context == EGL_NO_CONTEXT) {
    eglTerminate(display);
    return -6;
  }
  EGLint pbuffer_attribs[] = {EGL_WIDTH, frame_size, EGL_HEIGHT, frame_size, EGL_NONE};
  EGLSurface surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);
  if (surface == EGL_NO_SURFACE) {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
      eglDestroyContext(display, context);
      eglTerminate(display);
      return -7;
    }
  } else if (!eglMakeCurrent(display, surface, surface, context)) {
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -8;
  }

  GlApi gl;
  if (!gl.load_all()) {
    return -9;
  }

  GLuint color_texture = 0;
  gl.gen_textures(1, &color_texture);
  gl.bind_texture(GL_TEXTURE_2D, color_texture);
  gl.tex_image_2d(
    GL_TEXTURE_2D, 0, GL_RGBA, frame_size, frame_size, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    nullptr
  );
  gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GLuint framebuffer = 0;
  gl.gen_framebuffers(1, &framebuffer);
  gl.bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
  gl.framebuffer_texture_2d(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0
  );
  if (gl.check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    return -10;
  }

  GLuint field_texture = 0;
  gl.gen_textures(1, &field_texture);
  gl.bind_texture(GL_TEXTURE_2D, field_texture);
  gl.tex_image_2d(
    GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb
  );
  gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const char* vertex_source =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "  v_uv = a_uv;\n"
    "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";
  // Median MSDF decode (works for SDF-in-RGB from MLC msdf_generate).
  const char* fragment_source =
    "precision mediump float;\n"
    "varying vec2 v_uv;\n"
    "uniform sampler2D u_tex;\n"
    "uniform float u_screen_px_range;\n"
    "float median(float red, float green, float blue) {\n"
    "  return max(min(red, green), min(max(red, green), blue));\n"
    "}\n"
    "void main() {\n"
    "  vec3 sample_color = texture2D(u_tex, v_uv).rgb;\n"
    "  float signed_distance = median(sample_color.r, sample_color.g, sample_color.b);\n"
    "  float alpha = clamp((0.5 - signed_distance) * u_screen_px_range + 0.5, 0.0, 1.0);\n"
    "  gl_FragColor = vec4(1.0, 1.0, 1.0, alpha);\n"
    "}\n";

  GLuint vertex_shader = gl.create_shader(GL_VERTEX_SHADER);
  gl.shader_source(vertex_shader, 1, &vertex_source, nullptr);
  gl.compile_shader(vertex_shader);
  GLint compile_ok = 0;
  gl.get_shader_iv(vertex_shader, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    return -11;
  }
  GLuint fragment_shader = gl.create_shader(GL_FRAGMENT_SHADER);
  gl.shader_source(fragment_shader, 1, &fragment_source, nullptr);
  gl.compile_shader(fragment_shader);
  gl.get_shader_iv(fragment_shader, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    return -12;
  }
  GLuint program = gl.create_program();
  gl.attach_shader(program, vertex_shader);
  gl.attach_shader(program, fragment_shader);
  gl.link_program(program);
  GLint link_ok = 0;
  gl.get_program_iv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    return -13;
  }
  gl.use_program(program);

  // Centered quad covering most of the FBO.
  const float left = -0.75f;
  const float right = 0.75f;
  const float bottom = -0.75f;
  const float top = 0.75f;
  const float vertices[] = {
    left, bottom, 0.0f, 1.0f,
    right, bottom, 1.0f, 1.0f,
    left, top, 0.0f, 0.0f,
    right, bottom, 1.0f, 1.0f,
    right, top, 1.0f, 0.0f,
    left, top, 0.0f, 0.0f,
  };
  GLuint vertex_buffer = 0;
  gl.gen_buffers(1, &vertex_buffer);
  gl.bind_buffer(GL_ARRAY_BUFFER, vertex_buffer);
  gl.buffer_data(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  const GLint position_location = gl.get_attrib_location(program, "a_pos");
  const GLint uv_location = gl.get_attrib_location(program, "a_uv");
  gl.enable_vertex_attrib_array(static_cast<GLuint>(position_location));
  gl.vertex_attrib_pointer(
    static_cast<GLuint>(position_location), 2, GL_FLOAT, GL_FALSE, 16,
    reinterpret_cast<const void*>(0)
  );
  gl.enable_vertex_attrib_array(static_cast<GLuint>(uv_location));
  gl.vertex_attrib_pointer(
    static_cast<GLuint>(uv_location), 2, GL_FLOAT, GL_FALSE, 16,
    reinterpret_cast<const void*>(8)
  );

  const float screen_px_range =
    static_cast<float>(px_range) * (static_cast<float>(frame_size) * 0.75f * 2.0f)
    / static_cast<float>(field_size);
  gl.uniform_1f(gl.get_uniform_location(program, "u_screen_px_range"), screen_px_range);
  gl.active_texture(GL_TEXTURE0);
  gl.bind_texture(GL_TEXTURE_2D, field_texture);
  gl.uniform_1i(gl.get_uniform_location(program, "u_tex"), 0);
  gl.enable(GL_BLEND);
  gl.blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl.viewport(0, 0, frame_size, frame_size);
  gl.clear_color(0.0f, 0.0f, 0.0f, 1.0f);
  gl.clear(GL_COLOR_BUFFER_BIT);
  gl.draw_arrays(GL_TRIANGLES, 0, 6);

  std::vector<uint8_t> rgba(static_cast<size_t>(frame_size * frame_size * 4), 0);
  gl.read_pixels(0, 0, frame_size, frame_size, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
  int32_t lit_count = 0;
  for (int32_t index = 0; index < frame_size * frame_size; index += 1) {
    if (rgba[static_cast<size_t>(index * 4)] > 32) {
      lit_count += 1;
    }
  }

  gl.delete_buffers(1, &vertex_buffer);
  gl.delete_program(program);
  gl.delete_shader(vertex_shader);
  gl.delete_shader(fragment_shader);
  gl.delete_textures(1, &field_texture);
  gl.delete_framebuffers(1, &framebuffer);
  gl.delete_textures(1, &color_texture);
  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  if (surface != EGL_NO_SURFACE) {
    eglDestroySurface(display, surface);
  }
  eglDestroyContext(display, context);
  eglTerminate(display);

  if (lit_count < 80) {
    return -14;
  }
  return 0;
}

} // namespace mlc::gl
