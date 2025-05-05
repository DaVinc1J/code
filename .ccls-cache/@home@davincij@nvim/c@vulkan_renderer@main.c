#include "define.h"

#include "utilities.h"
#include "renderer.h"
#include "window.h"
#include "context.h"
#include "application.h"

//////////
// MAIN //
//////////

int main() {
  _state state = {0};
  _keymap keymap = {0};

  state.config.win_is_fullscreen = 0;
  state.config.application_name = "davincij_renderer";
  state.config.engine_name = "davincij_engine";
  state.config.win_title = "davincij_renderer";
  state.config.win_width = 1440;
  state.config.win_height = 1280;
  state.config.api_version = VK_API_VERSION_1_3;
  state.config.swapchain_component_mapping = (VkComponentMapping) {};
  state.config.swapchain_buffering = SWAPCHAIN_BUFFERING_DOUBLE;

  initialise(&state);

  loop(&state);

  clean_and_exit(&state);

  return 0;
}
