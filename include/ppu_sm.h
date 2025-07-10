#pragma once

#include <common.h>

static const int LINES_PER_FRAME = 154;
static const int TICKS_PER_LINE = 456;
static const int YRES = 144;
static const int XRES = 160;

void ppu_mode_oam();
void ppu_mode_xfer();
void ppu_mode_vblank();
void ppu_mode_hblank();