#include <ppu.h>
#include <ppu_sm.h>
#include <lcd.h>
#include <cpu.h>
#include <interrupts.h>

void IncrementLy() {
  LcdGetContext()->ly++;

  if (LcdGetContext()->ly == LcdGetContext()->ly_compare) {
    LCDS_SET_LYC(1);

    if (LCD_STAT_INT(SS_LYC)) {
      cpu_request_interrupt(IT_LCD_STAT);
    }

  } else {
    LCDS_SET_LYC(0);
  }
}

void ppu_mode_oam() {
  if (PpuGetContext()->line_ticks >= 80) {
    LCDS_SET_MODE(MODE_DRAW_PIXEL);

    PpuGetContext()->pixel_fifo_ctx.line_x = 0;
    PpuGetContext()->pixel_fifo_ctx.pushed_x = 0;
    PpuGetContext()->pixel_fifo_ctx.fetch_x = 0;
    PpuGetContext()->pixel_fifo_ctx.fetch_x = 0;
    PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_TILE;
  }
}

void ppu_mode_xfer() {
  PipelineProcess();
  if (PpuGetContext()->pixel_fifo_ctx.pushed_x >= XRES) {
    PipelineFifoReset();

    LCDS_SET_MODE(MODE_HBLANK);

    if (LCD_STAT_INT(SS_HBLANK)) {
      cpu_request_interrupt(IT_LCD_STAT);
    }
  }
}

static uint32_t target_frame_time = 1000 / 60;
static long prev_frame_time = 0;
static long start_timer = 0;
static long frame_count = 0;

void ppu_mode_hblank() {
  if (PpuGetContext()->line_ticks >= TICKS_PER_LINE) {
    IncrementLy();
    if (LcdGetContext()->ly >= YRES) {
      LCDS_SET_MODE(MODE_VBLANK);

      cpu_request_interrupt(IT_VBLANK);

      if (LCD_STAT_INT(SS_VBLANK)) {
        cpu_request_interrupt(IT_LCD_STAT);
      }

      PpuGetContext()->current_frame++;


      uint32_t end = GetTicks();
      uint32_t frame_time = end - prev_frame_time;

      if (frame_time < target_frame_time) {
        delay(target_frame_time - frame_time);
      }

      if (end - start_timer >= 1000) {
        uint32_t fps = frame_count;
        start_timer = end;
        frame_count = 0;

        printf("FPS: %d\n", fps);
      }

      frame_count++;
      prev_frame_time = GetTicks();

    } else {
      LCDS_SET_MODE(MODE_OAM);
    }

    PpuGetContext()->line_ticks = 0;
  }
}

void ppu_mode_vblank() {
  if (PpuGetContext()->line_ticks >= TICKS_PER_LINE) {
    IncrementLy();

    if (LcdGetContext()->ly >= LINES_PER_FRAME) {
      LCDS_SET_MODE(MODE_OAM);
      LcdGetContext()->ly = 0;
    }

    PpuGetContext()->line_ticks = 0;
  }

}
