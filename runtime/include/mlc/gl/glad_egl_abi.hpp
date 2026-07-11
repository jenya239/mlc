#pragma once

// MLC-facing ABI over vendored GLAD2 EGL (scalar loaders only this STEP).

#include "mlc/gl/glad_egl.hpp"

namespace mlc {
namespace gl {
namespace glad_egl_abi {

inline void loader_unload_egl() { gladLoaderUnloadEGL(); }

} // namespace glad_egl_abi
} // namespace gl
} // namespace mlc
