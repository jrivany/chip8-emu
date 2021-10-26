#include <SDL.h>
#include "common.h"
#include "display.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static uint64_t pixels[32];

#define BG 0xBAB86CFF
#define FG 0x000000FF

void translate_bit_pixels(uint32_t *dest, const uint64_t *src) {
  for (unsigned int row = 0; row < 32; ++row) {
    for (unsigned int col = 0; col < 64; ++ col) {
      const uint32_t sample = 1 & (uint32_t)(src[row] >> (63 - col));
      dest[(row * 64) + col] = (sample) ? FG : BG;
    }
  }
}

void dump_pixels(const uint32_t *px) {
  for (int row = 0; row < 32; ++row) {
    for (int col = 0; col < 64; ++col) {
      printf(" %04X", px[row * 64 + col]);
    }
    printf("\n");
  }
}

void display_tick() {
  uint32_t screen_pixels[64*32];

  translate_bit_pixels(screen_pixels, pixels);
  SDL_UpdateTexture(texture, NULL, screen_pixels, 64 * 4);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
  SDL_Delay(16);
}

void display_init() {

#define CHECK_RESOURCE(res) \
  if (!res) { \
    printf("Could not create %s. Error: %s", #res, SDL_GetError()); \
    display_quit(); \
  }

  window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
  CHECK_RESOURCE(window);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  CHECK_RESOURCE(renderer);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
  CHECK_RESOURCE(texture);
#undef CHECK_RESOURCE

  SDL_RenderSetLogicalSize(renderer, 64, 32);
  SDL_RenderSetIntegerScale(renderer, 1);
}

void display_quit() {
  if (texture) {
    SDL_DestroyTexture(texture);
    texture = NULL;
  }

  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
  }

  if (window) {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  SDL_Quit();
}

void display_clear() {
  memset(pixels, 0, 32 * sizeof(uint64_t));
}

int display_sprite(const uint8_t x, const uint8_t  y, const uint8_t *loc, const uint8_t count) {
  int ret = 0;
  for (uint8_t i = 0; i < count; ++i) {
    const uint64_t sample = loc[i];
    uint64_t *row = &pixels[(y + i) % 32];
    const uint8_t rot = (63 - x) - 7;
    const uint64_t mask = sample << rot | sample >> (64 - rot);
    const uint64_t result = mask ^ *row;
    if (debugger) {
      printf("Display sample: %02llX row %04X, %04X mask: %016llX, row: %016llX\n",
              sample, x, y + i, mask, *row);
    }
    if (result != *row) {
      ret = 1;
      *row = result;
    }
  }

  return ret;
}

