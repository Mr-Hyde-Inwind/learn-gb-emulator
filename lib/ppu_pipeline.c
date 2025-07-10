#include <ppu.h>
#include <lcd.h>
#include <bus.h>
#include <ppu_sm.h>


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

    if (x >= 0) {
      PixelFifoPush(color);
      PpuGetContext()->pixel_fifo_ctx.fifo_x++;
    }
  }

  return true;
}

void PipelineFetch() {
  switch (PpuGetContext()->pixel_fifo_ctx.current_fetch_state)
  {
    case FS_TILE: {
      if (LCDC_BGW_ENABLE) {
        PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] = bus_read(LCDC_BG_TILE_MAP
            + (PpuGetContext()->pixel_fifo_ctx.map_x / 8)
            + (PpuGetContext()->pixel_fifo_ctx.map_y / 8) * 32
        );

        if (LCDC_BGW_TILE_DATA == 0x8800) {
          PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] += 128;
        }
      }

      PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_DATA_HI;
      PpuGetContext()->pixel_fifo_ctx.fetch_x += 8;
      break;
    }
    case FS_DATA_HI: {
      PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[1] = bus_read(LCDC_BGW_TILE_DATA
          + (PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] * 16)
          + PpuGetContext()->pixel_fifo_ctx.tile_y
      );

      PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_DATA_LO;
      break;
    }
    case FS_DATA_LO: {
      PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[2] = bus_read(LCDC_BGW_TILE_DATA
          + (PpuGetContext()->pixel_fifo_ctx.bgw_fetch_data[0] * 16)
          + PpuGetContext()->pixel_fifo_ctx.tile_y + 1
      );

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

  // odd line
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