#include "input.h"
#include <stdint.h>

static uint16_t key_state;
// Whether a key was pressed this tick
static int8_t next = -1;

static bool map_key(uint8_t *dst, const SDL_KeyCode code) {
  switch (code) {
    case SDLK_0: *dst = 0x0; return true;
    case SDLK_1: *dst = 0x1; return true;
    case SDLK_2: *dst = 0x2; return true;
    case SDLK_3: *dst = 0x3; return true;
    case SDLK_4: *dst = 0x4; return true;
    case SDLK_5: *dst = 0x5; return true;
    case SDLK_6: *dst = 0x6; return true;
    case SDLK_7: *dst = 0x7; return true;
    case SDLK_8: *dst = 0x8; return true;
    case SDLK_9: *dst = 0x9; return true;
    case SDLK_a: *dst = 0xA; return true;
    case SDLK_b: *dst = 0xB; return true;
    case SDLK_c: *dst = 0xC; return true;
    case SDLK_d: *dst = 0xD; return true;
    case SDLK_e: *dst = 0xE; return true;
    case SDLK_f: *dst = 0xF; return true;
    default: return false;
  }
}

bool key_check(const uint8_t key) {
  return key_state & (0x1 << key);
}

int8_t key_next() {
  return next;
}

void key_tick() {
  next = -1;
}

void key_down(const SDL_KeyCode code) {
  uint8_t key;
  if (map_key(&key, code)) {
    key_state |= 0x1 << key;
    if (next == -1)
      next = key;
  }
}

void key_up(const SDL_KeyCode code) {
  uint8_t key;
  if (map_key(&key, code)) {
    key_state &= ~(0x1 << key);
  }
}
