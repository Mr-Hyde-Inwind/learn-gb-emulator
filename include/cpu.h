#pragma once

#include <common.h>
#include <instructions.h>

typedef struct {
  uint8_t a;
  uint8_t f;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint16_t sp;
  uint16_t pc;
} cpu_registers;

typedef struct {
  cpu_registers regs;

  uint16_t fetched_data;
  bool dest_is_mem;
  uint16_t mem_dest;
  uint8_t cur_opcode;
  instruction* cur_inst;

  bool halted;
  bool stepping;
} cpu_context;

void cpu_init();

bool cpu_step();

void emu_cycles(int cpu_cycles);