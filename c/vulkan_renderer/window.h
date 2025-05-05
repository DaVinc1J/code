#ifndef WINDOW_H
#define WINDOW_H

#include "define.h"
#include "utilities.h"

////////////
// WINDOW //
////////////

void poll_events(const _state *state);

unsigned char should_close_window(const _state *state);

VkSurfaceCapabilitiesKHR get_capabilities(const _window *window, const _context *context);

VkSurfaceFormatKHR get_surface_format(const _window *window, const _context *context);

VkPresentModeKHR get_present_mode(const _window *window, const _context *context);

void get_swapchain_images(_window *window, _context *context);

void create_swapchain_image_views(_state *state);

void create_swapchain(_state *state);

void create_surface(_state *state);

void create_window(_state *state);

void destroy_window(_state *state);

#endif
