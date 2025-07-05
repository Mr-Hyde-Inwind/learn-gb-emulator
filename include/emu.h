#pragma once

#include <common.h>

typedef struct {
  bool paused;
  bool running;
  bool die;
  uint64_t ticks;
} emu_context;

int emu_run(int argc, char **argv);

emu_context* emu_get_context();

void emu_cycles(int cpu_cycles);