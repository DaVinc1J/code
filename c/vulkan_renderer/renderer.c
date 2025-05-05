#include "renderer.h"

void create_renderer(_state *state) {
  vkCreateGraphicsPipelines(state->context.device, NULL, 1, &(VkGraphicsPipelineCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

  }, state->context.alloc, &state->renderer.graphics_pipeline);
}

void destroy_renderer(_state *state) {}
