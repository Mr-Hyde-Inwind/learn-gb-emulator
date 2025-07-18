#include <ppu.h>
#include <ppu_sm.h>
#include <lcd.h>
#include <cpu.h>
#include <interrupts.h>
#include <string.h>

bool WindowVisible();

void IncrementLy() {
  if (WindowVisible()
      && LcdGetContext()->ly >= LcdGetContext()->win_y
      && LcdGetContext()->ly < LcdGetContext()->win_y + YRES) {
        PpuGetContext()->window_line++;
  }
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

void LoadLineSprites() {
  int cur_y = LcdGetContext()->ly;
  uint8_t sprite_height = LCDC_OBJ_HEIGHT;
  memset(PpuGetContext()->line_entry_array, 0,
      sizeof(PpuGetContext()->line_entry_array));

  for (int i = 0; i < 40; i++) {
    oam_entry e = PpuGetContext()->oam_ram[i];

    if (!e.x) {
      // x == 0 means not visible...
      continue;
    }

    if (PpuGetContext()->line_sprite_count >= 10) {
      // Up to 10 sprites can be rendered per line.
      break;
    }

    if (e.y <= cur_y + 16 && e.y + sprite_height > cur_y + 16) {
      OamLineEntry* entry = &PpuGetContext()->line_entry_array[
        PpuGetContext()->line_sprite_count++
      ];

      entry->entry = e;
      entry->next = NULL;

      if (!PpuGetContext()->line_sprites ||
            PpuGetContext()->line_sprites->entry.x > e.x) {
        entry->next = PpuGetContext()->line_sprites;
        PpuGetContext()->line_sprites = entry;
        continue;
      }

      OamLineEntry* le = PpuGetContext()->line_sprites;
      OamLineEntry* prev = le;
      while (le) {
        if (le->entry.x > e.x) {
          prev->next = entry;
          entry->next = le;
          break;
        }

        if (!le->next) {
          le->next = entry;
          break;
        }
        prev = le;
        le = le->next;
      }
    }
  }
}

void ppu_mode_oam() {
  if (PpuGetContext()->line_ticks >= 80) {
    LCDS_SET_MODE(MODE_DRAW_PIXEL);

    PpuGetContext()->pixel_fifo_ctx.line_x = 0;
    PpuGetContext()->pixel_fifo_ctx.pushed_x = 0;
    PpuGetContext()->pixel_fifo_ctx.fetch_x = 0;
    PpuGetContext()->pixel_fifo_ctx.fifo_x = 0;
    PpuGetContext()->pixel_fifo_ctx.current_fetch_state = FS_TILE;
  }

  if (PpuGetContext()->line_ticks == 1) {
    PpuGetContext()->line_sprites = 0;
    PpuGetContext()->line_sprite_count = 0;

    LoadLineSprites();
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
      PpuGetContext()->window_line = 0;
    }

    PpuGetContext()->line_ticks = 0;
  }

}
