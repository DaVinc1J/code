#include "application.h"

void initialise(_state *state) {
  setup_error_handling();
  glfwInit();

  create_context(state);
  create_window(state);
  create_renderer(state);
}

void clean_and_exit(_state *state) { 
  destroy_renderer(state);
  destroy_window(state);
  destroy_context(state);
}

void loop(_state *state) {
  while (!should_close_window(state)) {
    poll_events(state);
  }
}
