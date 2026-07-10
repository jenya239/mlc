#include "mlc/gl/text_renderer_shim.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
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
      && gen_buffers && bind_buffer && buffer_data && enable_vertex_attrib_array
      && vertex_attrib_pointer && draw_arrays && clear && clear_color && viewport
      && read_pixels && active_texture && uniform_1i && enable && blend_func
      && delete_textures && delete_framebuffers && delete_buffers && delete_program
      && delete_shader;
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

bool load_glyph_a8(
  const char* font_path,
  int32_t codepoint,
  int32_t pixel_size,
  std::vector<uint8_t>& pixels,
  int32_t& width,
  int32_t& rows
) {
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return false;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path, 0, &face) != 0) {
    FT_Done_FreeType(library);
    return false;
  }
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  const FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(codepoint));
  if (glyph_index == 0
      || FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0
      || FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  width = static_cast<int32_t>(face->glyph->bitmap.width);
  rows = static_cast<int32_t>(face->glyph->bitmap.rows);
  const int pitch = face->glyph->bitmap.pitch;
  pixels.assign(static_cast<size_t>(width * rows), 0);
  for (int32_t row = 0; row < rows; row += 1) {
    std::memcpy(
      pixels.data() + static_cast<size_t>(row * width),
      face->glyph->bitmap.buffer + static_cast<size_t>(row * pitch),
      static_cast<size_t>(width)
    );
  }
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return width > 0 && rows > 0;
}

void blit_a8_centered(
  const std::vector<uint8_t>& glyph,
  int32_t glyph_width,
  int32_t glyph_rows,
  int32_t frame_width,
  int32_t frame_height,
  std::vector<uint8_t>& rgba_out
) {
  rgba_out.assign(static_cast<size_t>(frame_width * frame_height * 4), 0);
  const int32_t origin_x = (frame_width - glyph_width) / 2;
  const int32_t origin_y = (frame_height - glyph_rows) / 2;
  for (int32_t row = 0; row < glyph_rows; row += 1) {
    for (int32_t column = 0; column < glyph_width; column += 1) {
      const int32_t destination_x = origin_x + column;
      const int32_t destination_y = origin_y + row;
      if (destination_x < 0 || destination_y < 0
          || destination_x >= frame_width || destination_y >= frame_height) {
        continue;
      }
      const uint8_t alpha =
        glyph[static_cast<size_t>(row * glyph_width + column)];
      const size_t index =
        static_cast<size_t>((destination_y * frame_width + destination_x) * 4);
      rgba_out[index + 0] = alpha;
      rgba_out[index + 1] = alpha;
      rgba_out[index + 2] = alpha;
      rgba_out[index + 3] = 255;
    }
  }
}

double mean_absolute_error(
  const std::vector<uint8_t>& expected_rgba,
  const std::vector<uint8_t>& actual_rgba
) {
  if (expected_rgba.size() != actual_rgba.size() || expected_rgba.empty()) {
    return 1.0e9;
  }
  double sum = 0.0;
  // Compare RGB only (alpha fixed to 255 in both paths).
  for (size_t index = 0; index < expected_rgba.size(); index += 4) {
    sum += std::fabs(
      static_cast<double>(expected_rgba[index]) - static_cast<double>(actual_rgba[index])
    );
    sum += std::fabs(
      static_cast<double>(expected_rgba[index + 1])
      - static_cast<double>(actual_rgba[index + 1])
    );
    sum += std::fabs(
      static_cast<double>(expected_rgba[index + 2])
      - static_cast<double>(actual_rgba[index + 2])
    );
  }
  return sum / static_cast<double>(expected_rgba.size() / 4 * 3);
}

} // namespace

int32_t text_renderer_a8_glyph_smoke(
  mlc::String font_path,
  int32_t codepoint,
  int32_t pixel_size
) {
  if (pixel_size <= 0) {
    return -1;
  }

  std::vector<uint8_t> glyph_pixels;
  int32_t glyph_width = 0;
  int32_t glyph_rows = 0;
  if (!load_glyph_a8(
        font_path.c_str(), codepoint, pixel_size, glyph_pixels, glyph_width, glyph_rows
      )) {
    return -2;
  }

  constexpr int32_t frame_size = 64;
  if (glyph_width > frame_size || glyph_rows > frame_size) {
    return -3;
  }

  std::vector<uint8_t> expected_rgba;
  blit_a8_centered(
    glyph_pixels, glyph_width, glyph_rows, frame_size, frame_size, expected_rgba
  );

  EGLDisplay display = open_display();
  if (display == EGL_NO_DISPLAY) {
    return -4;
  }
  if (!eglInitialize(display, nullptr, nullptr)) {
    return -5;
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
      return -6;
    }
  }

  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
  if (context == EGL_NO_CONTEXT) {
    eglTerminate(display);
    return -7;
  }

  EGLint pbuffer_attribs[] = {EGL_WIDTH, frame_size, EGL_HEIGHT, frame_size, EGL_NONE};
  EGLSurface surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);
  if (surface == EGL_NO_SURFACE) {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
      eglDestroyContext(display, context);
      eglTerminate(display);
      return -8;
    }
  } else if (!eglMakeCurrent(display, surface, surface, context)) {
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -9;
  }

  GlApi gl;
  if (!gl.load_all()) {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (surface != EGL_NO_SURFACE) {
      eglDestroySurface(display, surface);
    }
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -10;
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
    gl.delete_framebuffers(1, &framebuffer);
    gl.delete_textures(1, &color_texture);
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (surface != EGL_NO_SURFACE) {
      eglDestroySurface(display, surface);
    }
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -11;
  }

  GLuint glyph_texture = 0;
  gl.gen_textures(1, &glyph_texture);
  gl.bind_texture(GL_TEXTURE_2D, glyph_texture);
  std::vector<uint8_t> glyph_rgba(static_cast<size_t>(glyph_width * glyph_rows * 4), 0);
  for (int32_t index = 0; index < glyph_width * glyph_rows; index += 1) {
    const uint8_t alpha = glyph_pixels[static_cast<size_t>(index)];
    const size_t base = static_cast<size_t>(index * 4);
    glyph_rgba[base + 0] = alpha;
    glyph_rgba[base + 1] = alpha;
    glyph_rgba[base + 2] = alpha;
    glyph_rgba[base + 3] = 255;
  }
  gl.tex_image_2d(
    GL_TEXTURE_2D, 0, GL_RGBA, glyph_width, glyph_rows, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    glyph_rgba.data()
  );
  gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  const char* vertex_source =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "  v_uv = a_uv;\n"
    "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";
  const char* fragment_source =
    "precision mediump float;\n"
    "varying vec2 v_uv;\n"
    "uniform sampler2D u_tex;\n"
    "void main() {\n"
    "  float alpha = texture2D(u_tex, v_uv).r;\n"
    "  gl_FragColor = vec4(alpha, alpha, alpha, 1.0);\n"
    "}\n";

  GLuint vertex_shader = gl.create_shader(GL_VERTEX_SHADER);
  gl.shader_source(vertex_shader, 1, &vertex_source, nullptr);
  gl.compile_shader(vertex_shader);
  GLint compile_ok = 0;
  gl.get_shader_iv(vertex_shader, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    return -12;
  }
  GLuint fragment_shader = gl.create_shader(GL_FRAGMENT_SHADER);
  gl.shader_source(fragment_shader, 1, &fragment_source, nullptr);
  gl.compile_shader(fragment_shader);
  gl.get_shader_iv(fragment_shader, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    return -13;
  }
  GLuint program = gl.create_program();
  gl.attach_shader(program, vertex_shader);
  gl.attach_shader(program, fragment_shader);
  gl.link_program(program);
  GLint link_ok = 0;
  gl.get_program_iv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    return -14;
  }
  gl.use_program(program);

  const float left = -static_cast<float>(glyph_width) / static_cast<float>(frame_size);
  const float right = static_cast<float>(glyph_width) / static_cast<float>(frame_size);
  const float bottom = -static_cast<float>(glyph_rows) / static_cast<float>(frame_size);
  const float top = static_cast<float>(glyph_rows) / static_cast<float>(frame_size);
  // FreeType row 0 is glyph top; map texture v=0 to quad top.
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

  gl.active_texture(GL_TEXTURE0);
  gl.bind_texture(GL_TEXTURE_2D, glyph_texture);
  gl.uniform_1i(gl.get_uniform_location(program, "u_tex"), 0);
  gl.viewport(0, 0, frame_size, frame_size);
  gl.clear_color(0.0f, 0.0f, 0.0f, 1.0f);
  gl.clear(GL_COLOR_BUFFER_BIT);
  gl.draw_arrays(GL_TRIANGLES, 0, 6);

  std::vector<uint8_t> actual_rgba(static_cast<size_t>(frame_size * frame_size * 4), 0);
  gl.read_pixels(0, 0, frame_size, frame_size, GL_RGBA, GL_UNSIGNED_BYTE, actual_rgba.data());

  // GL origin is bottom-left; CPU blit used top-left row 0. Flip actual for compare.
  std::vector<uint8_t> actual_flipped(actual_rgba.size(), 0);
  for (int32_t row = 0; row < frame_size; row += 1) {
    const int32_t source_row = frame_size - 1 - row;
    std::memcpy(
      actual_flipped.data() + static_cast<size_t>(row * frame_size * 4),
      actual_rgba.data() + static_cast<size_t>(source_row * frame_size * 4),
      static_cast<size_t>(frame_size * 4)
    );
  }

  constexpr double mae_tolerance = 8.0; // /255 units; Mesa soft raster tolerance
  const double mae = mean_absolute_error(expected_rgba, actual_flipped);

  gl.delete_buffers(1, &vertex_buffer);
  gl.delete_program(program);
  gl.delete_shader(vertex_shader);
  gl.delete_shader(fragment_shader);
  gl.delete_textures(1, &glyph_texture);
  gl.delete_framebuffers(1, &framebuffer);
  gl.delete_textures(1, &color_texture);
  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  if (surface != EGL_NO_SURFACE) {
    eglDestroySurface(display, surface);
  }
  eglDestroyContext(display, context);
  eglTerminate(display);

  if (mae > mae_tolerance) {
    return -15;
  }
  return 0;
}

namespace {

struct ShapedBitmap {
  std::vector<uint8_t> pixels;
  int32_t width = 0;
  int32_t rows = 0;
  int32_t left = 0;
  int32_t top = 0;
  int32_t pen_x = 0;
  int32_t pen_y = 0;
};

bool shape_and_rasterize(
  const char* font_path,
  const char* text,
  int text_byte_length,
  int32_t pixel_size,
  std::vector<ShapedBitmap>& out_glyphs
) {
  out_glyphs.clear();
  FT_Library library = nullptr;
  if (FT_Init_FreeType(&library) != 0) {
    return false;
  }
  FT_Face face = nullptr;
  if (FT_New_Face(library, font_path, 0, &face) != 0) {
    FT_Done_FreeType(library);
    return false;
  }
  if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_size)) != 0) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  hb_font_t* font = hb_ft_font_create(face, nullptr);
  if (font == nullptr) {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  hb_buffer_t* buffer = hb_buffer_create();
  if (buffer == nullptr) {
    hb_font_destroy(font);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }
  hb_buffer_add_utf8(buffer, text, text_byte_length, 0, text_byte_length);
  hb_buffer_guess_segment_properties(buffer);
  hb_shape(font, buffer, nullptr, 0);
  unsigned int glyph_count = 0;
  hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buffer, &glyph_count);
  hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buffer, &glyph_count);
  int32_t pen_x = 0;
  int32_t pen_y = 0;
  for (unsigned int index = 0; index < glyph_count; index += 1) {
    const int32_t glyph_id = static_cast<int32_t>(infos[index].codepoint);
    if (FT_Load_Glyph(face, static_cast<FT_UInt>(glyph_id), FT_LOAD_DEFAULT) != 0
        || FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) {
      hb_buffer_destroy(buffer);
      hb_font_destroy(font);
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return false;
    }
    ShapedBitmap shaped;
    shaped.width = static_cast<int32_t>(face->glyph->bitmap.width);
    shaped.rows = static_cast<int32_t>(face->glyph->bitmap.rows);
    shaped.left = face->glyph->bitmap_left
      + static_cast<int32_t>(positions[index].x_offset / 64);
    shaped.top = face->glyph->bitmap_top
      + static_cast<int32_t>(positions[index].y_offset / 64);
    shaped.pen_x = pen_x;
    shaped.pen_y = pen_y;
    shaped.pixels.assign(static_cast<size_t>(shaped.width * shaped.rows), 0);
    const int pitch = face->glyph->bitmap.pitch;
    for (int32_t row = 0; row < shaped.rows; row += 1) {
      if (shaped.width > 0) {
        std::memcpy(
          shaped.pixels.data() + static_cast<size_t>(row * shaped.width),
          face->glyph->bitmap.buffer + static_cast<size_t>(row * pitch),
          static_cast<size_t>(shaped.width)
        );
      }
    }
    out_glyphs.push_back(std::move(shaped));
    pen_x += static_cast<int32_t>(positions[index].x_advance / 64);
    pen_y += static_cast<int32_t>(positions[index].y_advance / 64);
  }
  hb_buffer_destroy(buffer);
  hb_font_destroy(font);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return !out_glyphs.empty();
}

void composite_shaped_string(
  const std::vector<ShapedBitmap>& glyphs,
  int32_t frame_width,
  int32_t frame_height,
  int32_t baseline_y,
  int32_t origin_x,
  std::vector<uint8_t>& rgba_out
) {
  rgba_out.assign(static_cast<size_t>(frame_width * frame_height * 4), 0);
  for (size_t index = 0; index < rgba_out.size(); index += 4) {
    rgba_out[index + 3] = 255;
  }
  for (const ShapedBitmap& glyph : glyphs) {
    const int32_t destination_x = origin_x + glyph.pen_x + glyph.left;
    const int32_t destination_y = baseline_y - glyph.pen_y - glyph.top;
    for (int32_t row = 0; row < glyph.rows; row += 1) {
      for (int32_t column = 0; column < glyph.width; column += 1) {
        const int32_t x = destination_x + column;
        const int32_t y = destination_y + row;
        if (x < 0 || y < 0 || x >= frame_width || y >= frame_height) {
          continue;
        }
        const uint8_t alpha =
          glyph.pixels[static_cast<size_t>(row * glyph.width + column)];
        const size_t pixel = static_cast<size_t>((y * frame_width + x) * 4);
        if (alpha > rgba_out[pixel]) {
          rgba_out[pixel + 0] = alpha;
          rgba_out[pixel + 1] = alpha;
          rgba_out[pixel + 2] = alpha;
        }
      }
    }
  }
}

bool read_golden_rgba(const char* path, std::vector<uint8_t>& out_bytes) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    return false;
  }
  input.seekg(0, std::ios::end);
  const std::streamoff size = input.tellg();
  if (size <= 0) {
    return false;
  }
  input.seekg(0, std::ios::beg);
  out_bytes.resize(static_cast<size_t>(size));
  input.read(reinterpret_cast<char*>(out_bytes.data()), size);
  return static_cast<std::streamoff>(input.gcount()) == size;
}

bool write_golden_rgba(const char* path, const std::vector<uint8_t>& bytes) {
  std::ofstream output(path, std::ios::binary | std::ios::trunc);
  if (!output) {
    return false;
  }
  output.write(
    reinterpret_cast<const char*>(bytes.data()),
    static_cast<std::streamsize>(bytes.size())
  );
  return static_cast<bool>(output);
}

} // namespace

int32_t text_renderer_a8_string_smoke(
  mlc::String font_path,
  mlc::String text,
  int32_t pixel_size,
  mlc::String golden_path
) {
  if (pixel_size <= 0) {
    return -1;
  }
  const auto text_view = text.view();
  if (text_view.empty()) {
    return -2;
  }

  std::vector<ShapedBitmap> glyphs;
  if (!shape_and_rasterize(
        font_path.c_str(),
        text_view.data(),
        static_cast<int>(text_view.size()),
        pixel_size,
        glyphs
      )) {
    return -3;
  }

  constexpr int32_t frame_width = 256;
  constexpr int32_t frame_height = 64;
  constexpr int32_t baseline_y = 48;
  constexpr int32_t origin_x = 8;
  std::vector<uint8_t> expected_rgba;
  composite_shaped_string(
    glyphs, frame_width, frame_height, baseline_y, origin_x, expected_rgba
  );

  const char* golden_c_path = golden_path.c_str();
  if (golden_c_path != nullptr && golden_c_path[0] != '\0') {
    if (std::getenv("MLC_WRITE_GOLDEN") != nullptr) {
      if (!write_golden_rgba(golden_c_path, expected_rgba)) {
        return -4;
      }
    }
    std::vector<uint8_t> golden_rgba;
    if (!read_golden_rgba(golden_c_path, golden_rgba)) {
      return -5;
    }
    if (golden_rgba.size() != expected_rgba.size()) {
      return -6;
    }
    if (std::memcmp(golden_rgba.data(), expected_rgba.data(), expected_rgba.size()) != 0) {
      return -7;
    }
  }

  EGLDisplay display = open_display();
  if (display == EGL_NO_DISPLAY) {
    return -8;
  }
  if (!eglInitialize(display, nullptr, nullptr)) {
    return -9;
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
      return -10;
    }
  }
  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
  if (context == EGL_NO_CONTEXT) {
    eglTerminate(display);
    return -11;
  }
  EGLint pbuffer_attribs[] = {EGL_WIDTH, frame_width, EGL_HEIGHT, frame_height, EGL_NONE};
  EGLSurface surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);
  if (surface == EGL_NO_SURFACE) {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
      eglDestroyContext(display, context);
      eglTerminate(display);
      return -12;
    }
  } else if (!eglMakeCurrent(display, surface, surface, context)) {
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -13;
  }

  GlApi gl;
  if (!gl.load_all()) {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (surface != EGL_NO_SURFACE) {
      eglDestroySurface(display, surface);
    }
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -14;
  }

  GLuint color_texture = 0;
  gl.gen_textures(1, &color_texture);
  gl.bind_texture(GL_TEXTURE_2D, color_texture);
  gl.tex_image_2d(
    GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
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
    return -15;
  }

  const char* vertex_source =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "  v_uv = a_uv;\n"
    "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";
  const char* fragment_source =
    "precision mediump float;\n"
    "varying vec2 v_uv;\n"
    "uniform sampler2D u_tex;\n"
    "void main() {\n"
    "  float alpha = texture2D(u_tex, v_uv).r;\n"
    "  gl_FragColor = vec4(alpha, alpha, alpha, alpha);\n"
    "}\n";
  GLuint vertex_shader = gl.create_shader(GL_VERTEX_SHADER);
  gl.shader_source(vertex_shader, 1, &vertex_source, nullptr);
  gl.compile_shader(vertex_shader);
  GLint compile_ok = 0;
  gl.get_shader_iv(vertex_shader, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    return -16;
  }
  GLuint fragment_shader = gl.create_shader(GL_FRAGMENT_SHADER);
  gl.shader_source(fragment_shader, 1, &fragment_source, nullptr);
  gl.compile_shader(fragment_shader);
  gl.get_shader_iv(fragment_shader, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    return -17;
  }
  GLuint program = gl.create_program();
  gl.attach_shader(program, vertex_shader);
  gl.attach_shader(program, fragment_shader);
  gl.link_program(program);
  GLint link_ok = 0;
  gl.get_program_iv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    return -18;
  }
  gl.use_program(program);
  const GLint position_location = gl.get_attrib_location(program, "a_pos");
  const GLint uv_location = gl.get_attrib_location(program, "a_uv");
  gl.uniform_1i(gl.get_uniform_location(program, "u_tex"), 0);
  gl.enable(GL_BLEND);
  gl.blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl.viewport(0, 0, frame_width, frame_height);
  gl.clear_color(0.0f, 0.0f, 0.0f, 1.0f);
  gl.clear(GL_COLOR_BUFFER_BIT);

  GLuint vertex_buffer = 0;
  gl.gen_buffers(1, &vertex_buffer);
  for (const ShapedBitmap& glyph : glyphs) {
    if (glyph.width <= 0 || glyph.rows <= 0) {
      continue;
    }
    const int32_t destination_x = origin_x + glyph.pen_x + glyph.left;
    const int32_t destination_y = baseline_y - glyph.pen_y - glyph.top;
    const float left =
      2.0f * static_cast<float>(destination_x) / static_cast<float>(frame_width) - 1.0f;
    const float right = 2.0f
      * static_cast<float>(destination_x + glyph.width) / static_cast<float>(frame_width)
      - 1.0f;
    const float top = 1.0f
      - 2.0f * static_cast<float>(destination_y) / static_cast<float>(frame_height);
    const float bottom = 1.0f
      - 2.0f * static_cast<float>(destination_y + glyph.rows)
        / static_cast<float>(frame_height);
    const float vertices[] = {
      left, bottom, 0.0f, 1.0f,
      right, bottom, 1.0f, 1.0f,
      left, top, 0.0f, 0.0f,
      right, bottom, 1.0f, 1.0f,
      right, top, 1.0f, 0.0f,
      left, top, 0.0f, 0.0f,
    };
    std::vector<uint8_t> glyph_rgba(static_cast<size_t>(glyph.width * glyph.rows * 4), 0);
    for (int32_t pixel_index = 0; pixel_index < glyph.width * glyph.rows; pixel_index += 1) {
      const uint8_t alpha = glyph.pixels[static_cast<size_t>(pixel_index)];
      const size_t base = static_cast<size_t>(pixel_index * 4);
      glyph_rgba[base + 0] = alpha;
      glyph_rgba[base + 1] = alpha;
      glyph_rgba[base + 2] = alpha;
      glyph_rgba[base + 3] = alpha;
    }
    GLuint glyph_texture = 0;
    gl.gen_textures(1, &glyph_texture);
    gl.bind_texture(GL_TEXTURE_2D, glyph_texture);
    gl.tex_image_2d(
      GL_TEXTURE_2D, 0, GL_RGBA, glyph.width, glyph.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      glyph_rgba.data()
    );
    gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl.tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl.bind_buffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl.buffer_data(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
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
    gl.active_texture(GL_TEXTURE0);
    gl.bind_texture(GL_TEXTURE_2D, glyph_texture);
    gl.draw_arrays(GL_TRIANGLES, 0, 6);
    gl.delete_textures(1, &glyph_texture);
  }

  std::vector<uint8_t> actual_rgba(static_cast<size_t>(frame_width * frame_height * 4), 0);
  gl.read_pixels(
    0, 0, frame_width, frame_height, GL_RGBA, GL_UNSIGNED_BYTE, actual_rgba.data()
  );
  std::vector<uint8_t> actual_flipped(actual_rgba.size(), 0);
  for (int32_t row = 0; row < frame_height; row += 1) {
    const int32_t source_row = frame_height - 1 - row;
    std::memcpy(
      actual_flipped.data() + static_cast<size_t>(row * frame_width * 4),
      actual_rgba.data() + static_cast<size_t>(source_row * frame_width * 4),
      static_cast<size_t>(frame_width * 4)
    );
  }

  constexpr double mae_tolerance = 8.0;
  const double mae = mean_absolute_error(expected_rgba, actual_flipped);

  gl.delete_buffers(1, &vertex_buffer);
  gl.delete_program(program);
  gl.delete_shader(vertex_shader);
  gl.delete_shader(fragment_shader);
  gl.delete_framebuffers(1, &framebuffer);
  gl.delete_textures(1, &color_texture);
  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  if (surface != EGL_NO_SURFACE) {
    eglDestroySurface(display, surface);
  }
  eglDestroyContext(display, context);
  eglTerminate(display);

  if (mae > mae_tolerance) {
    return -19;
  }
  return 0;
}

} // namespace mlc::gl
