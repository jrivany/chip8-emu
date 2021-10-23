#include <stdio.h>
#include <SDL.h>
#include <unistd.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static unsigned int pixels[640 * 480 * 4];

#define CHECK_RESOURCE(res) \
  if (!res) { \
    printf("Could not create %s. Error: %s", #res, SDL_GetError()); \
    goto quit; \
  }

void init_pixels() {
  for (int i = 0; i < 100; ++i) {
    pixels[i] = 0xFF0000FF;
  }
}

int main() {
  printf("Hello world!\n");
  SDL_Init(SDL_INIT_VIDEO);

  init_pixels();

  window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  CHECK_RESOURCE(window);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  CHECK_RESOURCE(renderer);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
  CHECK_RESOURCE(texture);

  int is_running = 1;
  SDL_Event event;
  while (is_running) {
      while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT) {
              is_running = 0;
          }
      }
      SDL_UpdateTexture(texture, NULL, pixels, 640 * 4);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      SDL_Delay(16);
  }

quit:
  SDL_DestroyWindow(window);

  window = NULL;
  renderer = NULL;
  texture = NULL;

  SDL_Quit();
  return 0;
}
#undef CHECK_RESOURCE
