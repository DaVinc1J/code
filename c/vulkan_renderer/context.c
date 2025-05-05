#include "context.h"

/////////////
// CONTEXT //
/////////////

void create_instance(_state *state) {
  uint32_t required_extensions_count;
  const char **required_extensions = glfwGetRequiredInstanceExtensions(&required_extensions_count);

  CHECK(vkCreateInstance(&(VkInstanceCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &(VkApplicationInfo) {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = state->config.application_name,
      .pEngineName = state->config.engine_name,
      .apiVersion = state->config.api_version,
    },
    .enabledExtensionCount = required_extensions_count,
    .ppEnabledExtensionNames = required_extensions,
  }, state->context.alloc, &state->context.inst), "Failed to create instance")
}

void select_physical_device(_state *state) {
  uint32_t count;
  CHECK(vkEnumeratePhysicalDevices(state->context.inst, &count, NULL), "Failed to enumerate physical devices");

  CHECK(count == 0, "Failed to find Vulkan-supported physical devices");

  VkPhysicalDevice *devices = malloc(count * sizeof(VkPhysicalDevice));
  CHECK(devices == NULL, "Failed to allocate memory for physical devices");

  CHECK(vkEnumeratePhysicalDevices(state->context.inst, &count, devices), "Failed to enumerate physical devices");

  state->context.physical_device = VK_NULL_HANDLE;
  for (uint32_t i = 0; i < count; ++i) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(devices[i], &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
      properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      state->context.physical_device = devices[i];
      break;
    }
  }

  CHECK(state->context.physical_device == VK_NULL_HANDLE, "Failed to select a suitable physical device");

  free(devices);
}

void select_queue_family(_state *state) {
  state->context.queue_family = UINT32_MAX;
  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(state->context.physical_device, &count, NULL);
  VkQueueFamilyProperties *queue_families = malloc(count * sizeof(VkQueueFamilyProperties));
  CHECK(queue_families == NULL, "Failed to allocate memory")
  vkGetPhysicalDeviceQueueFamilyProperties(state->context.physical_device, &count, queue_families);

  for (uint32_t queue_family_index = 0; queue_family_index < count; ++queue_family_index) {
    VkQueueFamilyProperties properties = queue_families[queue_family_index];
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT && glfwGetPhysicalDevicePresentationSupport(state->context.inst, state->context.physical_device, queue_family_index)) {
      state->context.queue_family = queue_family_index; 
      break;
    }
  }

  CHECK(state->context.queue_family == UINT32_MAX, "Failed to find suitable queue family")
  free(queue_families);

}

void create_device(_state *state) {
  CHECK(vkCreateDevice(state->context.physical_device, &(VkDeviceCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = state->context.queue_family,
      .queueCount = 1,
      .pQueuePriorities = &(float){1.0}
    },
    .queueCreateInfoCount = 1,
    .enabledExtensionCount = 1,
    .ppEnabledExtensionNames = &(const char *) {VK_KHR_SWAPCHAIN_EXTENSION_NAME}
  }, state->context.alloc, &state->context.device), "Failed to create device and queues")

}

void get_queue(_state *state) {
  vkGetDeviceQueue(state->context.device, state->context.queue_family, 0, &state->context.queue);
}

void create_context(_state *state) {
  create_instance(state);
  select_physical_device(state);
  select_queue_family(state);
  create_device(state);
  get_queue(state);
}

void destroy_context(_state *state) {
  vkDestroyDevice(state->context.device, state->context.alloc);
  vkDestroyInstance(state->context.inst, state->context.alloc);
}
