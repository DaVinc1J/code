#ifndef CONTEXT_H
#define CONTEXT_H

#include "define.h"
#include "utilities.h"

/////////////
// CONTEXT //
/////////////

void create_instance(_state *state);

void select_physical_device(_state *state);

void select_queue_family(_state *state);

void create_device(_state *state);

void get_queue(_state *state);

void create_context(_state *state);

void destroy_context(_state *state);

#endif
