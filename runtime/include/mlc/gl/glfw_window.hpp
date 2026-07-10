#pragma once

#include <cstdint>

namespace mlc {
namespace gl {

// GLFW window + empty clear/poll/swap loop (TRACK_TEXT_RENDERING_NATIVE STEP=2).
// Creates 800x600 window, clears a solid color, runs a few frames (or until Esc
// with a timeout). Returns 0 on success; negative on failure.
// Set MLC_GLFW_VISIBLE=1 to show the window; default is hidden for automation.
int32_t glfw_window_empty_loop_smoke();

} // namespace gl
} // namespace mlc
