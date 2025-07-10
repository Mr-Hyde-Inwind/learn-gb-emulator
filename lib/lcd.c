#include <lcd.h>
#include <ppu.h>
#include <dma.h>

static lcd_context ctx;

// Map Game Boy tile color code to SDL2 color code
// SDL2 Color encode                     White    light gray   dark gray   black
static unsigned long colors_default[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void LcdInit() {
  ctx.lcdc            = 0x91;
  ctx.lcds            = 0x81;
  ctx.scroll_y        = 0x00;
  ctx.scroll_x        = 0x00;
  ctx.ly              = 0x00;
  ctx.ly_compare      = 0x00;
  ctx.dma             = 0xFF;
  ctx.bg_palette      = 0xFC;
  ctx.obj_palette[0]  = 0xFF;
  ctx.obj_palette[1]  = 0xFF;
  ctx.win_y           = 0x00;
  ctx.win_x           = 0x00;

  for (int i = 0; i < 4; i++) {
    ctx.bg_colors[i] = colors_default[i];
    ctx.sp1_colors[i] = colors_default[i];
    ctx.sp2_colors[i] = colors_default[i];
  }
}

lcd_context* LcdGetContext() {
  return &ctx;
}

uint8_t LcdRead(uint16_t address) {
  uint8_t offset = (address - 0xFF40);
  uint8_t* p = (uint8_t*)&ctx;

  return p[offset];
}

void UpdatePalette(uint8_t palette, uint8_t palette_value) {
  uint32_t* p_colors = ctx.bg_colors;

  if (palette == 1)
  {
    p_colors = ctx.sp1_colors;
  }
  if (palette == 2)
  {
    p_colors == ctx.sp2_colors;
  }

  p_colors[0] = colors_default[palette_value & 0b11];
  p_colors[1] = colors_default[(palette_value >> 2) & 0b11];
  p_colors[2] = colors_default[(palette_value >> 4) & 0b11];
  p_colors[3] = colors_default[(palette_value >> 6) & 0b11];
}

void LcdWrite(uint16_t address, uint8_t value) {
  uint8_t offset = (address - 0xFF40);
  uint8_t* p = (uint8_t*)&ctx;

  p[offset] = value;

  if (offset == 0x06)
  {
    // 0xFF46 -- DMA
    DmaStart(value);
  }

  // bg_palette
  if (address == 0xFF47)
  {
    UpdatePalette(0, value);
  }
  // obj palette 0
  else if (address == 0xFF48)
  {
    UpdatePalette(1, value & 0b11111100);
  }
  // obj palette 1
  else if (address == 0xFF49)
  {
    UpdatePalette(2, value & 0b11111100);
  }

}