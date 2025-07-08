#include <dma.h>
#include <bus.h>
#include <ppu.h>

typedef struct {
  bool active;
  uint8_t byte;
  uint8_t value;
  uint8_t start_delay;
} dma_context;

static dma_context ctx;

void DmaStart(uint8_t start) {
  ctx.active = true;
  ctx.byte = 0;
  ctx.start_delay = 2;
  ctx.value = start;
}

void DmaTick() {
  if (!ctx.active) {
    return ;
  }

  if (ctx.start_delay) {
    ctx.start_delay--;
    return ;
  }

  ppu_oam_write(ctx.byte, bus_read((ctx.value * 0x100) + ctx.byte));

  ctx.byte++;

  ctx.active = ctx.byte < 0xA0;
}

bool IsTransferring() {
  return ctx.active;
}