#pragma once

#include "common.h"

void key_down(const SDL_KeyCode code);
void key_up(const SDL_KeyCode code);
void key_tick();

int8_t key_next();
bool key_check(const uint8_t key);
