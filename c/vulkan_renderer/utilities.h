#ifndef UTILITIES_H
#define UTILITIES_H

#include "define.h"

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

void glfw_error_callback(int error_code, const char *description);

void exit_callback();

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max);

void setup_error_handling();

#endif
