#pragma once

#include <common.h>
#include <bus.h>
#include <instructions.h>

#define CPU_FLAG_Z BIT(ctx->regs.f, 7)
#define CPU_FLAG_N BIT(ctx->regs.f, 6)
#define CPU_FLAG_H BIT(ctx->regs.f, 5)
#define CPU_FLAG_C BIT(ctx->regs.f, 4)

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

  bool int_master_enabled;
  bool enabling_ime;

  bool halted;
  bool stepping;

  uint8_t ie_register;
  uint8_t int_flags;
} cpu_context;

void cpu_init();

bool cpu_step();

void emu_cycles(int cpu_cycles);

void fetch_data();

typedef void (*IN_PROC)(cpu_context* );

uint16_t cpu_read_reg(reg_type rt);

void cpu_set_reg(reg_type rt, uint16_t value);

uint8_t cpu_get_ie_register();
void cpu_set_ie_register(uint8_t n);

uint8_t cpu_read_reg8(reg_type rt);
void cpu_set_reg8(reg_type rt, uint8_t val);

cpu_registers* cpu_get_regs();


IN_PROC inst_get_processor(in_type);