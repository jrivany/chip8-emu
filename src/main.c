#include <stdio.h>
#include <SDL.h>
#include <unistd.h>
#include <time.h>
#include "display.h"
#include "rom.h"
#include "cpu.h"
#include "input.h"

static void print_usage(const char *program_name) {
  printf("Usage: %s [-d] <romfile>\n", program_name);
}

const clock_t DISP_HZ = CLOCKS_PER_SEC / 60;
const clock_t CPU_HZ = CLOCKS_PER_SEC / 2000;

bool debugger = false;

int main(int argc, char **argv) {
  uint8_t *rom;
  if (argc < 2 || argc > 3) {
    print_usage(argv[0]);
    return 1;
  }

  if (argc == 3) {
    char *opt = argv[1];
    if (opt[0] == '-' && opt[1] == 'd' && opt[2] == '\0'){
      debugger = true;
    } else {
      print_usage(argv[0]);
      return 1;
    }
  }
  char *romfile = (argc == 3) ? argv[2] : argv[1];
  size_t sz = rom_load(romfile, &rom);
  cpu_load_rom(rom, sz);
  SDL_Init(SDL_INIT_VIDEO);
  display_init();

  bool is_running = true;
  bool tick = true;
  SDL_Event event;
  clock_t last_draw = 0;
  clock_t last_tick = 0;
  clock_t dt;
  while (is_running) {
      dt = clock() - last_tick;
      if (dt < CPU_HZ) {
        // TODO use a better sleep pattern
        // usleep(10000);
        continue;
      }
      while (SDL_PollEvent(&event)) {
          switch (event.type) {
          case SDL_QUIT:
              is_running = false;
              break;
          case SDL_KEYUP:
              key_up(event.key.keysym.sym);
              if (debugger && event.key.keysym.sym == SDLK_RETURN) {
                tick = true;
              }
              break;
          case SDL_KEYDOWN:
              key_down(event.key.keysym.sym);
              break;
          default: break;
          }
      }
      if (tick) {
        tick = cpu_tick();
        if (debugger) {
          puts("Press return to continue...\n");
          tick = false;
          cpu_mem_dump();
        }
      }
      if (clock() - last_draw > DISP_HZ) {
        cpu_timer_tick();
        display_tick();
        last_draw = clock();
      }
      last_tick = clock();
  }
  return 0;
}

