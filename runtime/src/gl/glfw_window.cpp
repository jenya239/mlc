#include "mlc/gl/glfw_window.hpp"

#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#define MLC_HAS_GLFW 1
#else
#define MLC_HAS_GLFW 0
#endif

#include <cstdlib>
#include <cstring>

namespace mlc {
namespace gl {

#if MLC_HAS_GLFW

namespace {

using GlClearColorFn = void (*)(float, float, float, float);
using GlClearFn = void (*)(unsigned int);

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

int32_t glfw_window_empty_loop_smoke() {
  if (glfwInit() != GLFW_TRUE) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  if (!env_flag_enabled("MLC_GLFW_VISIBLE")) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }

  GLFWwindow* window = glfwCreateWindow(
      kWindowWidth, kWindowHeight, "mlc glfw empty loop", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return -2;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  auto clear_color = reinterpret_cast<GlClearColorFn>(
      glfwGetProcAddress("glClearColor"));
  auto clear = reinterpret_cast<GlClearFn>(glfwGetProcAddress("glClear"));
  if (clear_color == nullptr || clear == nullptr) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return -3;
  }

  const bool interactive = env_flag_enabled("MLC_GLFW_VISIBLE");
  const double start_time = glfwGetTime();
  int frame_index = 0;

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    clear_color(0.1f, 0.2f, 0.35f, 1.0f);
    clear(kGlColorBufferBit);
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

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

#else

int32_t glfw_window_empty_loop_smoke() {
  return -100; // GLFW headers not available at compile time
}

#endif

} // namespace gl
} // namespace mlc
