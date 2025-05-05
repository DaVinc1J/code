#include "renderer.h"

void create_renderer(_state *state) {
  vkCreateGraphicsPipelines(state->context.device, NULL, 1, &(VkGraphicsPipelineCreateInfo) {
    
  }, state->context.alloc, &state->renderer.graphics_pipeline);
}

void destroy_renderer(_state *state) {}
