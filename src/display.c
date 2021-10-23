#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

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

void display_loop() {
  bool is_running = true;
  SDL_Event event;
  uint32_t screen_pixels[64*32];

  while (is_running) {
      while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT) {
              is_running = false;
          }
      }
      translate_bit_pixels(screen_pixels, pixels);
      // dump_pixels(screen_pixels);
      // is_running = false;
      SDL_UpdateTexture(texture, NULL, screen_pixels, 64 * 4);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      SDL_Delay(16);
  }
}

void display_init() {
  SDL_Init(SDL_INIT_VIDEO);

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

  display_loop();
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
