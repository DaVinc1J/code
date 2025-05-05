#ifndef DEFINE_H
#define DEFINE_H

///////////////
// LIBRARIES //
///////////////

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

/////////////////
// ERROR CATCH //
/////////////////

#define CHECK(ERROR, FORMAT, ...)                                            \
if (ERROR) {                                                                 \
  fprintf(stderr, "%s -> %s -> %i -> ERROR(%i):\n\t" FORMAT, __FILE_NAME__,  \
          __func__, __LINE__, ERROR, ##__VA_ARGS__);                     \
  raise(SIGABRT);                                                            \
}

/////////////
// STRUCTS //
/////////////

typedef struct {
  VkSwapchainKHR handle;

  uint32_t image_count;
  VkImage *images;
  VkImageView *images_views;

  VkSurfaceFormatKHR surface_format;
  VkExtent2D image_extent;
} _swapchain;


typedef struct {
  unsigned int quit : 1;
} _keymap;

typedef struct {
  const char *win_title;
  int win_width, win_height;
  char *application_name;
  char *engine_name;
  unsigned int win_is_fullscreen : 1;
  uint32_t api_version;
  VkComponentMapping swapchain_component_mapping;
  uint32_t swapchain_buffering;
} _config;

typedef struct {
  GLFWwindow *handle;
  VkSurfaceKHR surface;

  _swapchain swapchain;
} _window;

typedef struct {
  uint32_t queue_family;
  VkAllocationCallbacks *alloc;
  VkInstance inst;
  VkPhysicalDevice physical_device;
  VkDevice device;
  VkQueue queue;
} _context;

typedef struct {
  VkPipeline graphics_pipeline;
} _renderer;

typedef struct {
  _config config;
  _window window;
  _context context;
  _renderer renderer;
} _state;

enum swapchain_buffering {
  SWAPCHAIN_BUFFERING_DOUBLE = 2,
  SWAPCHAIN_BUFFERING_TRIPLE = 3,
};

#endif
