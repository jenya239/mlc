#pragma once

#include <cstdint>

namespace mlc::gl {

// Headless EGL context + eglGetProcAddress("glGenBuffers") smoke.
// Returns 0 on success; negative error code on failure.
// Does not require X11/Wayland (pbuffer or surfaceless Mesa).
int32_t gl_loader_gen_buffers_smoke();

} // namespace mlc::gl
