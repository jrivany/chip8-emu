#pragma once

#include "common.h"

void display_init();
void display_quit();
void display_clear();
void display_tick();
int display_sprite(const uint8_t x, const uint8_t  y, const uint8_t *loc, const uint8_t count);
