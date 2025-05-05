#include "utilities.h"

void glfw_error_callback(int error_code, const char *description) {
  CHECK(error_code, "GFLW: %s", description)
}

void exit_callback() { 
  glfwTerminate();
}

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  } else {
    return value;
  }
}

void setup_error_handling() {
  glfwSetErrorCallback(glfw_error_callback);
  atexit(exit_callback);
}

