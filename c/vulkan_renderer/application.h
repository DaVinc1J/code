#ifndef APPLICATION_H
#define APPLICATION_H

#include "define.h"
#include "utilities.h"
#include "context.h"
#include "window.h"
#include "renderer.h"

/////////////////
// APPLICATION //
/////////////////

void initialise(_state *state);

void clean_and_exit(_state *state);

void loop(_state *state);

#endif
