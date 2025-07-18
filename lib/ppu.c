#include <ppu.h>
#include <lcd.h>
#include <ppu_sm.h>
#include <string.h>


static ppu_context ctx;

ppu_context* PpuGetContext() {
  return &ctx;
}

void ppu_init() {
  ctx.current_frame = 0;
  ctx.line_ticks = 0;
  ctx.video_buffer = malloc(YRES * XRES * sizeof(uint32_t));

  ctx.pixel_fifo_ctx.line_x = 0;
  ctx.pixel_fifo_ctx.pushed_x = 0;
  ctx.pixel_fifo_ctx.fetch_x = 0;
  ctx.pixel_fifo_ctx.pixel_fifo.size = 0;
  ctx.pixel_fifo_ctx.pixel_fifo.head = NULL;
  ctx.pixel_fifo_ctx.pixel_fifo.tail = NULL;
  ctx.pixel_fifo_ctx.current_fetch_state = FS_TILE;

  ctx.line_sprites = 0;
  ctx.line_sprite_count = 0;
  ctx.window_line = 0;

  LcdInit();
  LCDS_SET_MODE(MODE_OAM);

  memset(ctx.oam_ram, 0, sizeof(ctx.oam_ram));
  memset(ctx.video_buffer, 0, YRES * XRES * sizeof(uint32_t));
}

void ppu_tick() {
  ctx.line_ticks++;
  switch (LCDS_MODE)
  {
    case MODE_OAM:
      ppu_mode_oam();
      break;
    case MODE_DRAW_PIXEL:
      ppu_mode_xfer();
      break;
    case MODE_HBLANK:
      ppu_mode_hblank();
      break;
    case MODE_VBLANK:
      ppu_mode_vblank();
      break;
  }
}

void ppu_oam_write(uint16_t address, uint8_t value) {
  if (address >= 0xFE00) {
    address -= 0xFE00;
  }

  uint8_t* p = (uint8_t*)ctx.oam_ram;
  p[address] = value;
}

uint8_t ppu_oam_read(uint16_t address) {
  if (address >= 0xFE00) {
    address -= 0xFE00;
  }

  uint8_t* p = (uint8_t*)ctx.oam_ram;
  return p[address];
}

void ppu_vram_write(uint16_t address, uint8_t value) {
  ctx.vram[address - 0x8000] = value;
}

uint8_t ppu_vram_read(uint16_t address) {
  return ctx.vram[address - 0x8000];
}