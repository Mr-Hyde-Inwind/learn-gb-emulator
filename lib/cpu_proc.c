#include <cpu.h>
#include <stack.h>

static bool check_cond(cpu_context* ctx) {
  bool z = CPU_FLAG_Z;
  bool c = CPU_FLAG_C;

  switch (ctx->cur_inst->cond)
  {
    case CT_NONE: return true;
    case CT_C: return c;
    case CT_NC: return !c;
    case CT_Z: return z;
    case CT_NZ: return !z;
  }
  return false;
}

static void cpu_set_flags(cpu_context* ctx, char z, char n, char h, char c) {
  if (z != -1) {
    BIT_SET(ctx->regs.f, 7, z);
  }
  if (n != -1) {
    BIT_SET(ctx->regs.f, 6, n);
  }
  if (h != -1) {
    BIT_SET(ctx->regs.f, 5, h);
  }
  if (c != -1) {
    BIT_SET(ctx->regs.f, 4, c);
  }
}

static void proc_nop(cpu_context* ctx) {

}

static void proc_none(cpu_context* ctx) {
  printf("INVALID INSTRUCTION.\n");
  exit(-7);
}

static void proc_ld(cpu_context* ctx) {
  if (ctx->dest_is_mem) {
    if (ctx->cur_inst->reg_2 >= RT_AF) {
      // use 16 bits register
      emu_cycles(1);
      bus_write16(ctx->mem_dest, ctx->fetched_data);
    } else {
      emu_cycles(1);
      bus_write(ctx->mem_dest, ctx->fetched_data);
    }
    return ;
  }

  if (ctx->cur_inst->mode == AM_HL_SPR) {
    uint8_t hflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;

    uint8_t cflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x10;

    cpu_set_flags(ctx, 0, 0, hflag, cflag);
    cpu_set_reg(ctx->cur_inst->reg_1,
      cpu_read_reg(ctx->cur_inst->reg_2) + (char)ctx->fetched_data);

  }

  cpu_set_reg(ctx->cur_inst->reg_1, ctx->fetched_data);

  return ;
}

static void goto_addr(cpu_context* ctx, uint16_t addr, bool pushpc) {
  if (check_cond(ctx)) {
    if (pushpc) {
      stack_push16(ctx->regs.pc);
      emu_cycles(2);
    }

    ctx->regs.pc = addr;
    emu_cycles(1);
  }
}

static void proc_jp(cpu_context* ctx) {
  goto_addr(ctx, ctx->fetched_data, false);
}

static void proc_jr(cpu_context* ctx) {
  char rel = (char)(ctx->fetched_data & 0xFF);
  uint16_t addr = ctx->regs.pc + rel;
  goto_addr(ctx, addr, false);
}

static void proc_call(cpu_context* ctx) {
  goto_addr(ctx, ctx->fetched_data, true);
}

static void proc_ret(cpu_context* ctx) {
  if (ctx->cur_inst->cond != CT_NONE) {
    emu_cycles(1);
  }

  if (check_cond(ctx)) {
    uint16_t lo = stack_pop();
    emu_cycles(1);

    uint16_t hi = stack_pop();
    emu_cycles(1);

    uint16_t addr = (hi << 8) | lo;
    ctx->regs.pc = addr;
    emu_cycles(1);
  }
}

static void proc_reti(cpu_context* ctx) {
  ctx->int_master_enabled = true;
  proc_ret(ctx);
}

static void proc_rst(cpu_context* ctx) {
  goto_addr(ctx, ctx->cur_inst->param, true);
}

static void proc_di(cpu_context* ctx) {
  ctx->int_master_enabled = false;
}

static void proc_xor(cpu_context* ctx) {
  ctx->regs.a ^= ctx->fetched_data & 0xFF;
  cpu_set_flags(ctx, ctx->regs.a, 0, 0, 0);
}

static void proc_ldh(cpu_context* ctx) {
  if (ctx->dest_is_mem) {
    bus_write(ctx->mem_dest, ctx->fetched_data);
  } else {
    cpu_set_reg(ctx->cur_inst->reg_1, bus_read(0xFF00 | ctx->fetched_data));
  }
  emu_cycles(1);
}

static void proc_pop(cpu_context* ctx) {
  uint16_t lo = stack_pop();
  emu_cycles(1);
  uint16_t hi = stack_pop();
  emu_cycles(1);

  uint16_t val = (hi << 8) | lo;
  cpu_set_reg(ctx->cur_inst->reg_1, val);

  if (ctx->cur_inst->reg_1 == RT_AF) {
    cpu_set_reg(ctx->cur_inst->reg_1, val & 0xFFF0);
  }
}

static void proc_push(cpu_context* ctx) {
  uint16_t hi = (cpu_read_reg(ctx->cur_inst->reg_1) >> 8) & 0xFF;
  emu_cycles(1);
  stack_push(hi);

  uint16_t lo = cpu_read_reg(ctx->cur_inst->reg_2) &0xFF;
  emu_cycles(1);
  stack_push(lo);

  emu_cycles(1);
}

IN_PROC processors[] = {
  [IN_NONE] = proc_none,
  [IN_NOP] = proc_nop,
  [IN_LD] = proc_ld,
  [IN_JP] = proc_jp,
  [IN_DI] = proc_di,
  [IN_XOR] = proc_xor,
  [IN_LDH] = proc_ldh,
  [IN_POP] = proc_pop,
  [IN_JR] = proc_jr,
  [IN_CALL] = proc_call,
  [IN_PUSH] = proc_push,
  [IN_RET] = proc_ret,
  [IN_RETI] = proc_reti,
  [IN_RST] = proc_rst
};

IN_PROC inst_get_processor(in_type type) {
  return processors[type];
}