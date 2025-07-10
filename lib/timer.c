#include <timer.h>
#include <interrupts.h>

static timer_context ctx = {0};

void timer_init() {
  ctx.div = 0xAC00;
}

timer_context* timer_get_context() {
  return &ctx;
}

void timer_tick() {
  uint16_t prev_div = ctx.div;

  ctx.div++;

  bool timer_update = false;

  switch (ctx.tac & 0b11)
  {
    case 0b00:
      timer_update = ((prev_div & (1 << 10)) ^ (ctx.div & (1 << 10)));
      // timer_update = (prev_div & (1 << 9)) && (!(ctx.div & (1 << 9)));
      break;
    case 0b01:
      timer_update = ((prev_div & (1 << 4)) ^ (ctx.div & (1 << 4)));
      // timer_update = (prev_div & (1 << 3) && (!(ctx.div & (1 << 3))));
      break;
    case 0b10:
      timer_update = ((prev_div & (1 << 6)) ^ (ctx.div & (1 << 6)));
      // timer_update = (prev_div & (1 << 5) && (!(ctx.div & (1 << 5))));
      break;
    case 0b11:
      timer_update = ((prev_div & (1 << 8)) ^ (ctx.div & (1 << 8)));
      // timer_update = (prev_div & (1 << 7) && (!(ctx.div & (1 << 7))));
      break;
  }

  if (timer_update && ctx.tac & (1 << 2)) {
    ctx.tima++;

    if (ctx.tima == 0xFF) {
      ctx.tima = ctx.tma;

      cpu_request_interrupt(IT_TIMER);
    }
  }
}

void timer_write(uint16_t address, uint8_t value) {
  switch (address)
  {
    case 0xFF04:
      ctx.div = 0;
      break;

    case 0xFF05:
      ctx.tima = value;
      break;

    case 0xFF06:
      ctx.tma = value;
      break;

    case 0xFF07:
      ctx.tac = value;
      break;
  }
}

uint8_t timer_read(uint16_t address) {
  switch (address)
  {
    case 0xFF04:
      return ctx.div >> 8;

    case 0xFF05:
      return ctx.tima;
      break;

    case 0xFF06:
      return ctx.tma;
      break;

    case 0xFF07:
      return ctx.tac;
      break;
  }
}