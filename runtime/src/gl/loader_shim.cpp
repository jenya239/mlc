#include "mlc/gl/loader_shim.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

namespace mlc::gl {
namespace {

using GlGenBuffersFn = void (*)(GLsizei, GLuint*);
using GlDeleteBuffersFn = void (*)(GLsizei, const GLuint*);

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

int32_t gl_loader_gen_buffers_smoke() {
  EGLDisplay display = open_display();
  if (display == EGL_NO_DISPLAY) {
    return -1;
  }
  EGLint major = 0;
  EGLint minor = 0;
  if (!eglInitialize(display, &major, &minor)) {
    return -2;
  }

  EGLint config_attribs_pbuffer[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };
  EGLConfig config = nullptr;
  EGLint config_count = 0;
  if (!eglChooseConfig(display, config_attribs_pbuffer, &config, 1, &config_count)
      || config_count < 1) {
    EGLint config_attribs_any[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
    };
    if (!eglChooseConfig(display, config_attribs_any, &config, 1, &config_count)
        || config_count < 1) {
      eglTerminate(display);
      return -3;
    }
  }

  EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
  if (context == EGL_NO_CONTEXT) {
    eglTerminate(display);
    return -4;
  }

  EGLint pbuffer_attribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
  EGLSurface surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);
  if (surface == EGL_NO_SURFACE) {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
      eglDestroyContext(display, context);
      eglTerminate(display);
      return -5;
    }
  } else if (!eglMakeCurrent(display, surface, surface, context)) {
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -6;
  }

  // Decision §5.2: load GL entry points via eglGetProcAddress (EGL path).
  // glXGetProcAddress is GLX-only; headless CI uses EGL + Mesa llvmpipe.
  auto gen_buffers = reinterpret_cast<GlGenBuffersFn>(eglGetProcAddress("glGenBuffers"));
  auto delete_buffers =
    reinterpret_cast<GlDeleteBuffersFn>(eglGetProcAddress("glDeleteBuffers"));
  if (gen_buffers == nullptr || delete_buffers == nullptr) {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (surface != EGL_NO_SURFACE) {
      eglDestroySurface(display, surface);
    }
    eglDestroyContext(display, context);
    eglTerminate(display);
    return -7;
  }

  GLuint buffer = 0;
  gen_buffers(1, &buffer);
  int32_t result = 0;
  if (buffer == 0) {
    result = -8;
  } else {
    delete_buffers(1, &buffer);
  }

  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  if (surface != EGL_NO_SURFACE) {
    eglDestroySurface(display, surface);
  }
  eglDestroyContext(display, context);
  eglTerminate(display);
  return result;
}

} // namespace mlc::gl
