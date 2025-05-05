#include "window.h"

void poll_events(const _state *state) {
  return glfwPollEvents();
}

unsigned char should_close_window(const _state *state) {
  return glfwWindowShouldClose(state->window.handle);
}

VkSurfaceCapabilitiesKHR get_capabilities(const _window *window, const _context *context) {
  VkSurfaceCapabilitiesKHR capabilities;
  CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device, window->surface, &capabilities), "Failed to get surface capabilities")
  return capabilities;
}

VkSurfaceFormatKHR get_surface_format(const _window *window, const _context *context) {
  uint32_t surface_format_count;
  CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, window->surface, &surface_format_count, NULL), "Failed to get surface format count")
  VkSurfaceFormatKHR *surface_formats = malloc(surface_format_count * sizeof(VkSurfaceFormatKHR));
  CHECK(!surface_formats, "Failed to allocate memory for surface formats")
  CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, window->surface, &surface_format_count, surface_formats), "Failed to get surface formats")

  VkSurfaceFormatKHR surface_format = surface_formats[0];
  for (uint32_t i = 0; i < surface_format_count; ++i) {
    if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surface_formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      surface_format = surface_formats[i];
      break;
    }
  }
  free(surface_formats);

  return surface_format;
}

VkPresentModeKHR get_present_mode(const _window *window, const _context *context) {
  uint32_t present_mode_count;
  VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
  CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, window->surface, &present_mode_count, NULL), "Failed to get present mode count")
  VkPresentModeKHR *present_modes = malloc(present_mode_count * sizeof(VkPresentModeKHR));
  CHECK(!present_modes, "Failed to allocate memory for present modes")
  CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, window->surface, &present_mode_count, present_modes), "Failed to get present modes")

  for (uint32_t i = 0; i < present_mode_count; ++i) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
  }
  free(present_modes);
  return present_mode;
}

void get_swapchain_images(_window *window, _context *context) {
  CHECK(vkGetSwapchainImagesKHR(context->device, window->swapchain.handle, &window->swapchain.image_count, NULL), "Failed to get swapchain images count");
  window->swapchain.images = malloc(window->swapchain.image_count * sizeof(VkImage));
  CHECK(!window->swapchain.images, "Failed to allocate memory for swapchain images");
  CHECK(vkGetSwapchainImagesKHR(context->device, window->swapchain.handle, &window->swapchain.image_count, window->swapchain.images), "Failed to get swapchain images");
}

void create_swapchain_image_views(_state *state) {
  state->window.swapchain.images_views = malloc(state->window.swapchain.image_count * sizeof(VkImageView));
  CHECK(!state->window.swapchain.images_views, "Failed to allocate memory for image views");

  for (uint32_t i = 0; i < state->window.swapchain.image_count; ++i) {
    CHECK(vkCreateImageView(state->context.device, &(VkImageViewCreateInfo){
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = state->window.swapchain.images[i],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = state->window.swapchain.surface_format.format,
      .components = state->config.swapchain_component_mapping,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    }, state->context.alloc, &state->window.swapchain.images_views[i]), "Failed to create image view");
  }
}

void create_swapchain(_state *state) {

  VkSurfaceCapabilitiesKHR capabilities = get_capabilities(&state->window, &state->context);
  state->window.swapchain.surface_format = get_surface_format(&state->window, &state->context);
  VkPresentModeKHR present_mode = get_present_mode(&state->window, &state->context);

  CHECK(vkCreateSwapchainKHR(state->context.device, &(VkSwapchainCreateInfoKHR){
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = state->window.surface,
    .minImageCount = clamp(state->config.swapchain_buffering, capabilities.minImageCount, capabilities.maxImageCount ? capabilities.maxImageCount : UINT32_MAX),
    .imageFormat = state->window.swapchain.surface_format.format,
    .imageColorSpace = state->window.swapchain.surface_format.colorSpace,
    .imageExtent = capabilities.currentExtent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = &state->context.queue_family,
    .preTransform = capabilities.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = present_mode,
    .clipped = VK_TRUE,
  }, state->context.alloc, &state->window.swapchain.handle), "Failed to create swapchain");

  get_swapchain_images(&state->window, &state->context);

  create_swapchain_image_views(state);

}

void create_surface(_state *state) {
  CHECK(glfwCreateWindowSurface(state->context.inst, state->window.handle, state->context.alloc, &state->window.surface), "Failed to create window surface")
}

void create_window(_state *state) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  int width = state->config.win_width;
  int height = state->config.win_height;

  GLFWmonitor *monitor = NULL;

  if (state->config.win_is_fullscreen) {
    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  }

  state->window.handle = glfwCreateWindow(
    width, height, state->config.win_title,
    monitor, NULL);

  create_surface(state);
  create_swapchain(state);
}

void destroy_window(_state *state) {
  for (uint32_t image_count_index = 0; image_count_index < state->window.swapchain.image_count; ++image_count_index) {
    vkDestroyImageView(state->context.device, state->window.swapchain.images_views[image_count_index] ,state->context.alloc);
  }

  free(state->window.swapchain.images_views);
  free(state->window.swapchain.images);

  vkDestroySwapchainKHR(state->context.device, state->window.swapchain.handle, state->context.alloc);
  vkDestroySurfaceKHR(state->context.inst, state->window.surface, state->context.alloc);
  glfwDestroyWindow(state->window.handle);
}
