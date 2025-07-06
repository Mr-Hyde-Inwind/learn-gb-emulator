#include <cpu.h>

extern cpu_context ctx;

uint16_t reverse(uint16_t n) {
  return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

uint16_t cpu_read_reg(reg_type rt) {
  switch (rt)
  {
    case RT_A: return ctx.regs.a;
    case RT_F: return ctx.regs.f;
    case RT_B: return ctx.regs.b;
    case RT_C: return ctx.regs.c;
    case RT_D: return ctx.regs.d;
    case RT_E: return ctx.regs.e;
    case RT_H: return ctx.regs.h;
    case RT_L: return ctx.regs.l;

    case RT_AF: return reverse(*((uint16_t*)&ctx.regs.a));
    case RT_BC: return reverse(*((uint16_t*)&ctx.regs.b));
    case RT_DE: return reverse(*((uint16_t*)&ctx.regs.d));
    case RT_HL: return reverse(*((uint16_t*)&ctx.regs.h));

    case RT_PC: return ctx.regs.pc;
    case RT_SP: return ctx.regs.sp;
  
    default: return 0;
  }
}

void cpu_set_reg(reg_type rt, uint16_t value) {
  switch (rt)
  {
    case RT_A: ctx.regs.a = value & 0xFF; break;
    case RT_F: ctx.regs.f = value & 0xFF; break;
    case RT_B: ctx.regs.b = value & 0xFF; break;
    case RT_C: ctx.regs.c = value & 0xFF; break;
    case RT_D: ctx.regs.d = value & 0xFF; break;
    case RT_E: ctx.regs.e = value & 0xFF; break;
    case RT_H: ctx.regs.h = value & 0xFF; break;
    case RT_L: ctx.regs.l = value & 0xFF; break;

    case RT_AF: *((uint16_t*)&ctx.regs.a) = reverse(value); break;
    case RT_BC: *((uint16_t*)&ctx.regs.b) = reverse(value); break;
    case RT_DE: *((uint16_t*)&ctx.regs.d) = reverse(value); break;
    case RT_HL: *((uint16_t*)&ctx.regs.h) = reverse(value); break;


    case RT_PC: ctx.regs.pc = value; break;
    case RT_SP: ctx.regs.sp = value; break;
    case RT_NONE: break;
  }
}

uint8_t cpu_read_reg8(reg_type rt) {
   switch (rt)
  {
    case RT_A: return ctx.regs.a;
    case RT_F: return ctx.regs.f;
    case RT_B: return ctx.regs.b;
    case RT_C: return ctx.regs.c;
    case RT_D: return ctx.regs.d;
    case RT_E: return ctx.regs.e;
    case RT_H: return ctx.regs.h;
    case RT_L: return ctx.regs.l;
    
    case RT_HL: {
      return bus_read(cpu_read_reg(RT_HL));
    }

    default: {
      printf("**ERR INVALID REG8 %d\n", rt);
      NO_IMPL
    }
  }
}

void cpu_set_reg8(reg_type rt, uint8_t val) {
   switch (rt)
  {
    case RT_A: ctx.regs.a = val & 0xFF; break;
    case RT_F: ctx.regs.f = val & 0xFF; break;
    case RT_B: ctx.regs.b = val & 0xFF; break;
    case RT_C: ctx.regs.c = val & 0xFF; break;
    case RT_D: ctx.regs.d = val & 0xFF; break;
    case RT_E: ctx.regs.e = val & 0xFF; break;
    case RT_H: ctx.regs.h = val & 0xFF; break;
    case RT_L: ctx.regs.l = val & 0xFF; break;

    case RT_HL: bus_write(cpu_read_reg(RT_HL), val); break;

    default: {
      printf("**ERR INVALID REG8 %d\n", rt);
      NO_IMPL
    }
  }
}

uint8_t cpu_get_int_flags() {
    return ctx.int_flags;
}

void cpu_set_int_flags(uint8_t value) {
    ctx.int_flags = value;
}