#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

bool cpu_tick();
void cpu_load_rom(const uint8_t *instructions, const size_t bounds);
void cpu_mem_dump();
void cpu_timer_tick();
