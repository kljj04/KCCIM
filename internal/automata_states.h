#ifndef AUTOMATA_STATES_H
#define AUTOMATA_STATES_H

#include "../include/kccim.h"
#include "../internal/kccim_internal.h"

int handle_state_empty(kccim_t ctx, int vk_code);
int handle_state_cho(kccim_t ctx, int vk_code);
int handle_state_jung(kccim_t ctx, int vk_code);
int handle_state_jong(kccim_t ctx, int vk_code);

#endif