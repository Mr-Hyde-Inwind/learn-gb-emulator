#pragma once
#include <common.h>

//               7	      6	      5	        4	       3	    2	1	0
// Attributes	Priority	Y flip	X flip	DMG palette	Bank	CGB palette
// Priority: 0 = No, 1 = BG and Window color indices 1–3 are drawn over this OBJ
// Y flip: 0 = Normal, 1 = Entire OBJ is vertically mirrored
// X flip: 0 = Normal, 1 = Entire OBJ is horizontally mirrored
// DMG palette [Non CGB Mode only]: 0 = OBP0, 1 = OBP1
// Bank [CGB Mode Only]: 0 = Fetch tile from VRAM bank 0, 1 = Fetch tile from VRAM bank 1
// CGB palette [CGB Mode Only]: Which of OBP0–7 to use

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t tile;

  unsigned f_cgb_pn     : 3;
  unsigned f_vram_bank  : 1;
  unsigned f_pn         : 1;
  unsigned f_x_flip     : 1;
  unsigned f_y_flip     : 1;
  unsigned f_priority   : 1;
} oam_entry;

typedef enum {
  FS_TILE,
  FS_DATA_HI,
  FS_DATA_LO,
  FS_IDLE,
  FS_PUSH
} FetchState;

typedef struct FifoEntry {
  struct FifoEntry* next;
  uint32_t color;
} FifoEntry;

typedef struct {
  FifoEntry* head;
  FifoEntry* tail;
  uint32_t size;
} Fifo;

typedef struct {
  FetchState current_fetch_state;
  Fifo pixel_fifo;
  uint8_t line_x;
  uint8_t pushed_x;
  uint8_t fetch_x;
  uint8_t bgw_fetch_data[3];
  uint8_t fetch_entry_data[6];
  uint8_t map_x;
  uint8_t map_y;
  uint8_t tile_y;
  uint8_t fifo_x;
} PixelFifoContext;

typedef struct {
  oam_entry oam_ram[40];
  uint8_t vram[0x2000];

  PixelFifoContext pixel_fifo_ctx;

  uint32_t current_frame;
  uint32_t line_ticks;
  uint32_t* video_buffer;
} ppu_context;


ppu_context* PpuGetContext();

void ppu_init();
void ppu_tick();

void ppu_oam_write(uint16_t address, uint8_t value);
uint8_t ppu_oam_read(uint16_t address);

void ppu_vram_write(uint16_t address, uint8_t value);
uint8_t ppu_vram_read(uint16_t address);

void PipelineFifoReset();
void PipelineProcess();