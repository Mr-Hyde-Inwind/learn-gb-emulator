#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include <ui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
  Cart
  CPU
  Address Bus
  PPU
  Timer
*/

static emu_context ctx;

emu_context* emu_get_context() {
  return &ctx;
}

void delay(uint32_t ms) {
  SDL_Delay(ms);
}

void* cpu_run(void *p) {
  cpu_init();

  ctx.running = true;
  ctx.paused = false;
  ctx.ticks = 0;

  while (ctx.running) {
    if (ctx.paused) {
      delay(10);
      continue;
    }

    if (!cpu_step()) {
      printf("CPU Stopped\n");
      return 0;
    }

    ctx.ticks++;
  }
}

int emu_run(int argc, char **argv) {
  if (argc < 2) {
    printf("No rom file supplied.\n");
    return -1;
  }

  if (!cart_load(argv[1])) {
    printf("Failed to load ROM file: %s\n", argv[1]);
    return -2;
  }

  printf("Cart loaded..\n");

  ui_init();

  pthread_t cpu_thread;
  if (pthread_create(&cpu_thread, NULL, cpu_run, NULL)) {
    fprintf(stderr, "FAILED TO START MAIN CPU THREAD.\n");
  }

  while (!ctx.die) {
    usleep(1000);
    ui_handle_events();
  }

  return 0;
}