#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#define CHECK(ERROR, FORMAT, ...)                                            \
if (ERROR) {                                                                 \
  fprintf(stderr, "%s -> %s -> %i -> ERROR(%i):\n\t" FORMAT, __FILE_NAME__,  \
          __FUNCTION__, __LINE__, ERROR, ##__VA_ARGS__);                     \
  raise(SIGABRT);                                                            \
}

typedef struct {
  /* window variables */
  const char *win_title;
  int win_width, win_height;

  /* bit states */
  unsigned int win_is_fullscreen : 1;

  /* uint32_t values */
  uint32_t api_version;
  uint32_t queue_family;
  uint32_t swapchain_image_count;

  /* GLFW */
  int frame_buffer_width, frame_buffer_height;
  GLFWwindow *win;
  GLFWmonitor *win_monitor;

  /* Vulkan */
  VkAllocationCallbacks *alloc;
  VkInstance inst;

  VkPhysicalDevice physical_device;
  VkSurfaceKHR surface;
  VkDevice device;
  VkQueue queue;

  VkSwapchainKHR swapchain;
  VkImage *swapchain_images;
  VkImageView *swapchain_image_views;

} graphics;

typedef struct {
  unsigned int quit : 1;
} keymap;

void glfw_error_callback(int error_code, const char *description) {
  CHECK(error_code, "GFLW: %s", description)
}

void exit_callback() { 
  glfwTerminate();
}

void setup_error_handling() {
  glfwSetErrorCallback(glfw_error_callback);
  atexit(exit_callback);
}

void create_window(graphics *gfx) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  if (gfx->win_is_fullscreen) {
    gfx->win_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(gfx->win_monitor);
    gfx->win_width = mode->width;
    gfx->win_height = mode->height;
  }

  gfx->win = glfwCreateWindow(
    gfx->win_width, gfx->win_height, gfx->win_title,
    gfx->win_monitor, NULL);

}

void create_instance(graphics *gfx) {

  uint32_t required_extensions_count;

  const char **required_extensions = glfwGetRequiredInstanceExtensions(&required_extensions_count);

  VkApplicationInfo application_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion = gfx->api_version,
  };

  VkInstanceCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &application_info,
    .enabledExtensionCount = required_extensions_count,
    .ppEnabledExtensionNames = required_extensions
  };

  CHECK(vkCreateInstance(&create_info, gfx->alloc, &gfx->inst), "Failed to create instance")
}

void select_physical_device(graphics *gfx) {
  uint32_t count;
  CHECK(vkEnumeratePhysicalDevices(gfx->inst, &count, NULL), "Failed to enumerate physical devices");

  CHECK(count == 0, "Failed to find Vulkan-supported physical devices");

  VkPhysicalDevice *devices = malloc(count * sizeof(VkPhysicalDevice));
  CHECK(devices == NULL, "Failed to allocate memory for physical devices");

  CHECK(vkEnumeratePhysicalDevices(gfx->inst, &count, devices), "Failed to enumerate physical devices");

  gfx->physical_device = VK_NULL_HANDLE;
  for (uint32_t i = 0; i < count; ++i) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(devices[i], &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
      properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      gfx->physical_device = devices[i];
      break;
    }
  }

  CHECK(gfx->physical_device == VK_NULL_HANDLE, "Failed to select a suitable physical device");

  free(devices);
}

void create_surface(graphics *gfx) {
  CHECK(glfwCreateWindowSurface(gfx->inst, gfx->win, gfx->alloc, &gfx->surface), "Failed to create window surface")
}

void select_queue_family(graphics *gfx) {
  gfx->queue_family = UINT32_MAX;
  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(gfx->physical_device, &count, NULL);
  VkQueueFamilyProperties *queue_families = malloc(count * sizeof(VkQueueFamilyProperties));
  CHECK(queue_families == NULL, "Failed to allocate memory")
  vkGetPhysicalDeviceQueueFamilyProperties(gfx->physical_device, &count, queue_families);

  for (uint32_t queue_family_index = 0; queue_family_index < count; ++queue_family_index) {
    VkQueueFamilyProperties properties = queue_families[queue_family_index];
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT && glfwGetPhysicalDevicePresentationSupport(gfx->inst, gfx->physical_device, queue_family_index)) {
      gfx->queue_family = queue_family_index; 
      break;
    }
  }

  CHECK(gfx->queue_family == UINT32_MAX, "Failed to find suitable queue family")
  free(queue_families);

}

void create_device(graphics *gfx) {
  CHECK(vkCreateDevice(gfx->physical_device, &(VkDeviceCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = gfx->queue_family,
      .queueCount = 1,
      .pQueuePriorities = &(float){1.0}
    },
    .queueCreateInfoCount = 1,
    .enabledExtensionCount = 1,
    .ppEnabledExtensionNames = &(const char *) {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
  }, gfx->alloc, &gfx->device), "Failed to create device and queues")

}

void get_queue(graphics *gfx) {
  vkGetDeviceQueue(gfx->device, gfx->queue_family, 0, &gfx->queue);
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

void create_swapchain(graphics *gfx) {
  VkSurfaceCapabilitiesKHR capabilities;
  CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gfx->physical_device, gfx->surface, &capabilities), "Failed to get surface capabilities");

  uint32_t surface_format_count;
  CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gfx->physical_device, gfx->surface, &surface_format_count, NULL), "Failed to get surface format count");
  VkSurfaceFormatKHR *surface_formats = malloc(surface_format_count * sizeof(VkSurfaceFormatKHR));
  CHECK(!surface_formats, "Failed to allocate memory for surface formats");
  CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gfx->physical_device, gfx->surface, &surface_format_count, surface_formats), "Failed to get surface formats");

  VkSurfaceFormatKHR surface_format = surface_formats[0];
  for (uint32_t i = 0; i < surface_format_count; ++i) {
    if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surface_formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      surface_format = surface_formats[i];
      break;
    }
  }
  free(surface_formats);

  uint32_t present_mode_count;
  VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
  CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gfx->physical_device, gfx->surface, &present_mode_count, NULL), "Failed to get present mode count");
  VkPresentModeKHR *present_modes = malloc(present_mode_count * sizeof(VkPresentModeKHR));
  CHECK(!present_modes, "Failed to allocate memory for present modes");
  CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gfx->physical_device, gfx->surface, &present_mode_count, present_modes), "Failed to get present modes");

  for (uint32_t i = 0; i < present_mode_count; ++i) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
  }
  free(present_modes);

  CHECK(vkCreateSwapchainKHR(gfx->device, &(VkSwapchainCreateInfoKHR){
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = gfx->surface,
    .minImageCount = clamp(capabilities.minImageCount + 1, capabilities.minImageCount, capabilities.maxImageCount ? capabilities.maxImageCount : UINT32_MAX),
    .imageFormat = surface_format.format,
    .imageColorSpace = surface_format.colorSpace,
    .imageExtent = capabilities.currentExtent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = &gfx->queue_family,
    .preTransform = capabilities.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = present_mode,
    .clipped = VK_TRUE,
  }, gfx->alloc, &gfx->swapchain), "Failed to create swapchain");

  CHECK(vkGetSwapchainImagesKHR(gfx->device, gfx->swapchain, &gfx->swapchain_image_count, NULL), "Failed to get swapchain images count");
  gfx->swapchain_images = malloc(gfx->swapchain_image_count * sizeof(VkImage));
  CHECK(!gfx->swapchain_images, "Failed to allocate memory for swapchain images");
  CHECK(vkGetSwapchainImagesKHR(gfx->device, gfx->swapchain, &gfx->swapchain_image_count, gfx->swapchain_images), "Failed to get swapchain images");
  gfx->swapchain_image_views = malloc(gfx->swapchain_image_count * sizeof(VkImageView));
  CHECK(!gfx->swapchain_image_views, "Failed to allocate memory for image views");

  for (uint32_t i = 0; i < gfx->swapchain_image_count; ++i) {
    CHECK(vkCreateImageView(gfx->device, &(VkImageViewCreateInfo){
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = gfx->swapchain_images[i],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = surface_format.format,
      .components = {0},
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    }, gfx->alloc, &gfx->swapchain_image_views[i]), "Failed to create image view");
  }
}

void clean_and_exit(graphics *gfx) { 
  for (uint32_t swapchain_image_count_index = 0; swapchain_image_count_index < gfx->swapchain_image_count; ++swapchain_image_count_index) {
    vkDestroyImageView(gfx->device, gfx->swapchain_image_views[swapchain_image_count_index] ,gfx->alloc);
  }

  free(gfx->swapchain_image_views);
  free(gfx->swapchain_images);

  vkDestroySwapchainKHR(gfx->device, gfx->swapchain, gfx->alloc);
  vkDestroyDevice(gfx->device, gfx->alloc);
  vkDestroySurfaceKHR(gfx->inst, gfx->surface, gfx->alloc);
  vkDestroyInstance(gfx->inst, gfx->alloc);
}

int main() {

  graphics gfx = {0};
  keymap keys = {0};

  gfx.win_is_fullscreen = 0;
  gfx.win_title = "";
  gfx.win_width = 800;
  gfx.win_height = 600;
  gfx.api_version = VK_API_VERSION_1_3;

  setup_error_handling();

  create_window(&gfx);
  create_instance(&gfx);

  select_physical_device(&gfx);
  create_surface(&gfx);
  select_queue_family(&gfx);
  create_device(&gfx);
  get_queue(&gfx);

  create_swapchain(&gfx);

  while (!glfwWindowShouldClose(gfx.win)) {
    glfwPollEvents();

  }

  clean_and_exit(&gfx);

  return 0;
}
