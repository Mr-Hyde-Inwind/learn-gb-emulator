#include <cpu.h>

cpu_context ctx = {0};

void cpu_init() {
  ctx.regs.pc = 0x100;
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
    fetch_data();

    printf("%04X: %7s (%02X %02X %02X) A: %02X BC: %02X%02X DE: %02X%02X HL: %02X%02X\n",
      pc, inst_name(ctx.cur_inst->type),
      ctx.cur_opcode, bus_read(pc+1), bus_read(pc+2),
      ctx.regs.a, ctx.regs.b, ctx.regs.c,
      ctx.regs.d, ctx.regs.e,
      ctx.regs.h, ctx.regs.l
    );

    // printf("Executing Instruction: %02X    PC: %04X\n", ctx.cur_opcode, pc);
    
    execute();
  }
  return true;
}

void emu_cycles(int cpu_cycles) {

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