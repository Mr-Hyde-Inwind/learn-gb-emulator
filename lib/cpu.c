#include <cpu.h>
#include <bus.h>
#include <emu.h>
#include <interrupts.h>
#include <dbg.h>
#include <timer.h>

cpu_context ctx = {0};

void cpu_init() {
  ctx.regs.pc = 0x100;
  ctx.regs.sp = 0xFFFE;
  /** 
   *  On little-endian mahcines, assignment statements store data in memory starting from
   *  the least significant byte to the most significant byte, following the increasing
   *  address order
   * 
   *  typedef struct {
   *    uint8_t a;
   *    uint8_t f;
   *    uint8_t b;
   *    uint8_t c;
   *    uint8_t d;
   *    uint8_t e;
   *    uint8_t h;
   *    uint8_t l;
   *    uint16_t sp;
   *    uint16_t pc;
   *  } cpu_registers;
   * 
   *  When the statement '*((short *)&ctx.regs.a) = 0xB001;' is executed, the value 0x01
   *  is written to the register a, and 0xB0 is written to the adjacent register f.
   *  
  **/

  *((short *)&ctx.regs.a) = 0xB001;
  *((short *)&ctx.regs.b) = 0x1300;
  *((short *)&ctx.regs.d) = 0xD800;
  *((short *)&ctx.regs.h) = 0x4D01;

  ctx.ie_register = 0;
  ctx.int_flags = 0;
  ctx.int_master_enabled = false;
  ctx.enabling_ime = false;

  timer_get_context()->div = 0xABCC;

}

static void fetch_instruction() {
  ctx.cur_opcode = bus_read(ctx.regs.pc++);
  ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);

  // if (ctx.cur_inst == NULL) {
  //   printf("Unknown Instruction! %02X\n", ctx.cur_opcode);
  //   exit(-7);
  // }
}

static void execute() {
  IN_PROC proc = inst_get_processor(ctx.cur_inst->type);

  if (!proc) {
    // NO_IMPL
    // exit(-7);
    return ;
  }

  proc(&ctx);
}

bool cpu_step() {
  if (!ctx.halted) {
    uint16_t pc = ctx.regs.pc;

    fetch_instruction();
    emu_cycles(1);
    fetch_data();

    char flags[16];
    sprintf(flags, "%c%c%c%c",
      ctx.regs.f & (1<<7) ? 'Z' : '-',
      ctx.regs.f & (1<<6) ? 'N' : '-',
      ctx.regs.f & (1<<5) ? 'H' : '-',
      ctx.regs.f & (1<<4) ? 'C' : '-'
    );

    char inst[16];
    inst_to_str(&ctx, inst);

    printf("%08lX - %04X: %-12s (%02X %02X %02X %04X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X SP: %04X\n",
      emu_get_context()->ticks,
      pc,
      inst,
      ctx.cur_opcode,
      bus_read(pc+1),
      bus_read(pc+2),
      ctx.fetched_data,
      ctx.regs.a,
      flags,
      ctx.regs.b,
      ctx.regs.c,
      ctx.regs.d,
      ctx.regs.e,
      ctx.regs.h,
      ctx.regs.l,
      ctx.regs.sp
    );

    // printf("Executing Instruction: %02X    PC: %04X\n", ctx.cur_opcode, pc);
    dbg_update();
    dbg_print();
    
    
    execute();
  } else {
    // halted
    emu_cycles(1);

    if (ctx.int_flags) {
      ctx.halted = false;
    }
  }

  if (ctx.int_master_enabled) {
    cpu_handle_interrupts(&ctx);
    ctx.enabling_ime = false;
  }

  if (ctx.enabling_ime) {
    ctx.int_master_enabled = true;
  }
  return true;
}

uint8_t cpu_get_ie_register() {
  return ctx.ie_register;
}
void cpu_set_ie_register(uint8_t n) {
  ctx.ie_register = n;
}

cpu_registers* cpu_get_regs() {
  return &ctx.regs;
}

void cpu_request_interrupt(interrupt_type t) {
  ctx.int_flags |= t;
}