#pragma once
#include <common.h>


typedef struct {
  // registers
  // Registers are ordered in the struct by address,
  // allowing easy access to each register later via offset-based addressing.
  uint8_t lcdc;                       // $FF40
  uint8_t lcds;                       // $FF41
  uint8_t scroll_y;                   // $FF42
  uint8_t scroll_x;                   // $FF43
  uint8_t ly;                         // $FF44
  uint8_t ly_compare;                 // $FF45
  uint8_t dma;                        // $FF46
  uint8_t bg_palette;                 // $FF47
  uint8_t obj_palette[2];             // $FF48 - $FF49
  uint8_t win_y;                      // $FF4A
  uint8_t win_x;                      // $FF4B

  // other data
  uint32_t bg_colors[4];
  uint32_t sp1_colors[4];
  uint32_t sp2_colors[4];
} lcd_context;

lcd_context* LcdGetContext();

typedef enum {
  MODE_HBLANK,
  MODE_VBLANK,
  MODE_OAM,
  MODE_DRAW_PIXEL
} lcd_mode;

// 7    - NONE
// 6    - LYC int select (Read/Write): If set, selects the LYC == LY condition for the STAT interrupt.
// 5    - Mode 2 int select (Read/Write): If set, selects the Mode 2 (OAM SCAN) condition for the STAT interrupt.
// 4    - Mode 1 int select (Read/Write): If set, selects the Mode 1 (VBLANK) condition for the STAT interrupt.
// 3    - Mode 0 int select (Read/Write): If set, selects the Mode 0 (HBLANK) condition for the STAT interrupt.
// 2    - LYC == LY (Read-only): Set when LY contains the same value as LYC; it is constantly updated.
// 1, 0 - PPU mode (Read-only): Indicates the PPU’s current status. Reports 0 instead when the PPU is disabled.
//                 - 0: Mode 0, HBLANK
//                 - 1: Mode 1, VBLANK
//                 - 2: Mode 2, OAM SCAN
//                 - 3: Mode 3, DRAWING PIXEL

// 7 - LCD & PPU enable: 0 = Off; 1 = On
// 6 - Window tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
// 5 - Window enable: 0 = Off; 1 = On
// 4 - BG & Window tile data area: 0 = 8800–97FF; 1 = 8000–8FFF
// 3 - BG tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
// 2 - OBJ size: 0 = 8×8; 1 = 8×16
// 1 - OBJ enable: 0 = Off; 1 = On
// 0 - BG & Window enable / priority [Different meaning in CGB Mode]: 0 = Off; 1 = On

#define LCDC_LCD_ENABLE         (BIT(LcdGetContext()->lcdc, 7))
#define LCDC_WINDOW_TILE_MAP    (BIT(LcdGetContext()->lcdc, 6) ? 0x9C00 : 0x9800)
#define LCDC_WINDOW_ENABLE      (BIT(LcdGetContext()->lcdc, 5))
#define LCDC_BGW_TILE_DATA      (BIT(LcdGetContext()->lcdc, 4) ? 0x8000 : 0x8800)
#define LCDC_BG_TILE_MAP        (BIT(LcdGetContext()->lcdc, 3) ? 0x9C00 : 0x9800)
#define LCDC_OBJ_HEIGHT         (BIT(LcdGetContext()->lcdc, 2) ? 16 : 8)
#define LCDC_OBJ_ENABLE         (BIT(LcdGetContext()->lcdc, 1))
#define LCDC_BGW_ENABLE         (BIT(LcdGetContext()->lcdc, 0))


#define LCDS_LYC                (BIT(LcdGetContext()->lcds, 2))
#define LCDS_SET_LYC(b)         (BIT_SET(LcdGetContext()->lcds, 2, b))
#define LCDS_MODE               (LcdGetContext()->lcds & 0b11)
#define LCDS_SET_MODE(mode)     { LcdGetContext()->lcds &= ~0b11; LcdGetContext()->lcds |= mode; }

typedef enum {
  SS_HBLANK = (1 << 3),
  SS_VBLANK = (1 << 4),
  SS_OAM    = (1 << 5),
  SS_LYC    = (1 << 6)
} stat_src;

#define LCD_STAT_INT(src)     (LcdGetContext()->lcds & src)


void LcdInit();

uint8_t LcdRead(uint16_t address);
void LcdWrite(uint16_t address, uint8_t value);