#include <ppu.h>
#include <lcd.h>
#include <bus.h>
#include <ppu_sm.h>

bool WindowVisible() {
  return LCDC_WINDOW_ENABLE
        && LcdGetContext()->win_x >= 0
        && LcdGetContext()->win_x <= 166
        && LcdGetContext()->win_y >= 0
        && LcdGetContext()->win_y < YRES;
}

void PixelFifoPush(uint32_t value) {
  FifoEntry *next = malloc(sizeof(FifoEntry));
  next->next = NULL;
  next->color = value;

  if (!PpuGetContext()->pixel_fifo_ctx.pixel_fifo.head) {
    PpuGetContext()->pixel_fifo_ctx.pixel_fifo.head = next;
    PpuGetContext()->pixel_fifo_ctx.pixel_fifo.tail = next;
  } else {
    PpuGetContext()->pixel_fifo_ctx.pixel_fifo.tail->next = next;
    PpuGetContext()->pixel_fifo_ctx.pixel_fifo.tail = next;
  }

  PpuGetContext()->pixel_fifo_ctx.pixel_fifo.size++;
}

uint32_t PixelFifoPop() {
  if (PpuGetContext()->pixel_fifo_ctx.pixel_fifo.size <= 0) {
    fprintf(stderr, "POP ERROR: NO DATA IN FIFO!\n");
    exit(-8);
  }

  FifoEntry* popped = PpuGetContext()->pixel_fifo_ctx.pixel_fifo.head;
  PpuGetContext()->pixel_fifo_ctx.pixel_fifo.head = popped->next;
  PpuGetContext()->pixel_fifo_ctx.pixel_fifo.size--;

  uint32_t value = popped->color;
  free(popped);

  return value;
}

uint32_t FetchSpritePixels(int bit, uint32_t color, uint8_t bg_color) {
  for (int i = 0; i < PpuGetContext()->fetched_entry_count; i++) {
    int sp_x = (PpuGetContext()->fetched_entries[i].x - 8)
        + (LcdGetContext()->scroll_x % 8);

    if (sp_x + 8 < PpuGetContext()->pixel_fifo_ctx.fifo_x) {
      continue;
    }

    int offset = PpuGetContext()->pixel_fifo_ctx.fifo_x - sp_x;

    if (offset < 0 || offset > 7) {
      continue;
    }

    bit = 7 - offset;

    if (PpuGetContext()->fetched_entries[i].f_x_flip) {
      bit = offset;
    }

    uint8_t hi = !!(PpuGetContext()->pixel_fifo_ctx.fetch_entry_data[i * 2] & (1 << bit));
    uint8_t lo = !!(PpuGetContext()->pixel_fifo_ctx.fetch_entry_data[(i * 2) + 1] & (1 << bit)) << 1;

    bool bg_priority = PpuGetContext()->fetched_entries[i].f_priority;

    if (!(hi | lo)) {
      // Transparent
      continue;
    }

    if (!bg_priority || bg_color == 0) {
      color = (PpuGetContext()->fetched_entries[i].f_pn)
          ? LcdGetContext()->sp2_colors[hi | lo]
          : LcdGetContext()->sp1_colors[hi | lo];

      if (hi | lo) {
        break;
      }
    }
  }

  return color;
}

bool PipelineFifoAdd() {
  if (PpuGetContext()->pixel_fifo_ctx.pixel_fifo.size > 8) {
    return false;
  }

  int x = PpuGetContext()->pixel_fifo_ctx.fetch_x - (8 - (LcdGetContext()->scroll_x % 8));

  for (int i = 0; i < 8; i++) {
    int bit = 7 - i;
    uint8_t hi = !!(PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[1] & (1 << bit));
    uint8_t lo = !!(PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[2] & (1 << bit)) << 1;
    uint32_t color = LcdGetContext()->bg_colors[hi | lo];

    if (!LCDC_BGW_ENABLE) {
      color = LcdGetContext()->bg_colors[0];
    }

    if (LCDC_OBJ_ENABLE) {
      color = FetchSpritePixels(bit, color, hi | lo);
    }

    if (x >= 0) {
      PixelFifoPush(color);
      PpuGetContext()->pixel_fifo_ctx.fifo_x++;
    }
  }

  return true;
}

void PipelineLoadSpriteTile() {
  OamLineEntry* le = PpuGetContext()->line_sprites;

  while (le) {
    int sp_x = (le->entry.x - 8) + (LcdGetContext()->scroll_x % 8);

    if ((sp_x >= PpuGetContext()->pixel_fifo_ctx.fetch_x
                    && sp_x < PpuGetContext()->pixel_fifo_ctx.fetch_x + 8)

        || (sp_x + 8 >= PpuGetContext()->pixel_fifo_ctx.fetch_x
                    && sp_x + 8 < PpuGetContext()->pixel_fifo_ctx.fetch_x + 8)) {
      PpuGetContext()->fetched_entries[PpuGetContext()->fetched_entry_count++] = le->entry;
    }

    le = le->next;

    if (!le || PpuGetContext()->fetched_entry_count >= 3) {
      break;
    }
  }
}

void PipelineLoadSpriteData(uint8_t offset) {
  int cur_y = LcdGetContext()->ly;
  uint8_t sprite_height = LCDC_OBJ_HEIGHT;

  for (int i = 0; i < PpuGetContext()->fetched_entry_count; i++) {
    uint8_t tile_y = ((cur_y + 16) - PpuGetContext()->fetched_entries[i].y) * 2;

    if (PpuGetContext()->fetched_entries[i].f_y_flip) {
      tile_y = (sprite_height * 2) - 2 - tile_y;
    }

    uint8_t tile_idx = PpuGetContext()->fetched_entries[i].tile;

    if (sprite_height == 16) {
      tile_idx &= ~(1);
    }

    PpuGetContext()->pixel_fifo_ctx.fetch_entry_data[(i * 2) + offset] =
        bus_read(0x8000 + (tile_idx * 16) + tile_y + offset);
  }
}

void PipelineLoadWindowTile() {
  if (!WindowVisible()) {
    return ;
  }

  uint8_t window_y = LcdGetContext()->win_y;

  if (PpuGetContext()->pixel_fifo_ctx.fetch_x + 7 >= LcdGetContext()->win_x
        && PpuGetContext()->pixel_fifo_ctx.fetch_x + 7 < LcdGetContext()->win_x + YRES + 14) {
    if (LcdGetContext()->ly >= window_y && LcdGetContext()->ly < window_y + XRES) {
      uint8_t w_tile_y = PpuGetContext()->window_line / 8;

      PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] = bus_read(LCDC_WINDOW_TILE_MAP
         + ((PpuGetContext()->pixel_fifo_ctx.fetch_x + 7 - LcdGetContext()->win_x) / 8)
      + (w_tile_y * 32));

      if (LCDC_BGW_TILE_DATA == 0x8800) {
        PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] += 128;
      }
    }
  }
}

void PipelineFetch() {
  switch (PpuGetContext()->pixel_fifo_ctx.current_fetch_state)
  {
    case FS_TILE: {
      PpuGetContext()->fetched_entry_count = 0;

      if (LCDC_BGW_ENABLE) {
        PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] = bus_read(LCDC_BG_TILE_MAP
            + (PpuGetContext()->pixel_fifo_ctx.map_x / 8)
            + (PpuGetContext()->pixel_fifo_ctx.map_y / 8) * 32
        );

        if (LCDC_BGW_TILE_DATA == 0x8800) {
          PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] += 128;
        }

        PipelineLoadWindowTile();
      }

      if (LCDC_OBJ_ENABLE && PpuGetContext()->line_sprites) {
        PipelineLoadSpriteTile();
      }

      PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_DATA_LO;
      PpuGetContext()->pixel_fifo_ctx.fetch_x += 8;
      break;
    }

    case FS_DATA_LO: {
      PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[1] = bus_read(LCDC_BGW_TILE_DATA
          + (PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] * 16)
          + PpuGetContext()->pixel_fifo_ctx.tile_y
      );

      PipelineLoadSpriteData(0);

      PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_DATA_HI;
      break;
    }

    case FS_DATA_HI: {
      PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[2] = bus_read(LCDC_BGW_TILE_DATA
          + (PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] * 16)
          + PpuGetContext()->pixel_fifo_ctx.tile_y + 1
      );

      PipelineLoadSpriteData(1);

      PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_IDLE;
      break;
    }

    case FS_IDLE: {
      PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_PUSH;
      break;
    }

    case FS_PUSH: {
      if (PipelineFifoAdd()) {
        PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_TILE;
      }
      break;
    }
  }

}

void PipelinePushPixel() {
  if (PpuGetContext()->pixel_fifo_ctx.pixel_fifo.size > 8) {
    uint32_t pixel_data = PixelFifoPop();

    if (PpuGetContext()->pixel_fifo_ctx.line_x >= (LcdGetContext()->scroll_x % 8)) {
      int idx = PpuGetContext()->pixel_fifo_ctx.pushed_x + (LcdGetContext()->ly * XRES);
      PpuGetContext()->video_buffer[idx] = pixel_data;

      PpuGetContext()->pixel_fifo_ctx.pushed_x++;
    }

    PpuGetContext()->pixel_fifo_ctx.line_x++;
  }
}

void PipelineProcess() {
  PpuGetContext()->pixel_fifo_ctx.map_y = (LcdGetContext()->ly
                                            + LcdGetContext()->scroll_y);

  PpuGetContext()->pixel_fifo_ctx.map_x = (PpuGetContext()->pixel_fifo_ctx.fetch_x
                                            + LcdGetContext()->scroll_x);

  PpuGetContext()->pixel_fifo_ctx.tile_y = ((LcdGetContext()->ly
                                            + LcdGetContext()->scroll_y) % 8) * 2;

  // even line
  if (!(PpuGetContext()->line_ticks & 1)) {
    PipelineFetch();
  }

  PipelinePushPixel();

}

void PipelineFifoReset() {
  while (PpuGetContext()->pixel_fifo_ctx.pixel_fifo.size) {
    PixelFifoPop();
  }

  PpuGetContext()->pixel_fifo_ctx.pixel_fifo.head = 0;
}