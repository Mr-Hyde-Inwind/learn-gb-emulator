#include <ram.h>

typedef struct {
  uint8_t wram[0x2000];
  uint8_t hram[0x0080];
} ram_context;

static ram_context ram_ctx;

// 0000 - 3FFF 16 KiB ROM bank 00               From cartridge, usually a fixed bank
// 4000 - 7FFF 16 KiB ROM Bank 01–NN            From cartridge, switchable bank via mapper (if any)
// 8000 - 9FFF 8 KiB Video RAM (VRAM)           In CGB mode, switchable bank 0/1
// A000 - BFFF 8 KiB External RAM               From cartridge, switchable bank if any
// C000 - CFFF 4 KiB Work RAM (WRAM)
// D000 - DFFF 4 KiB Work RAM (WRA)            In CGB mode, switchable bank 1–7
// E000 - FDFF Echo RAM (mirror of C000–DDFF)   Nintendo says use of this area is prohibited.
// FE00 - FE9F Object attribute memory (OAM)
// FEA0 - FEFF Not switchable                   Nintendo says use of this area is prohibited.
// FF00 - FF7F I/O Registers
// FF80 - FFFE High RAM (HRAM)
// FFFF - FFFF Interrupt Enable register (IE)

uint8_t wram_read(uint16_t address) {
  address -= 0xC000;

  if (address >= 0x2000 || address < 0) {
    printf("INVALID WRAM ADDR %08X\n", address + 0xC000);
    exit(-1);
  }

  return ram_ctx.wram[address];
}

void wram_write(uint16_t address, uint8_t value) {
  address -= 0xC000;

  if (address >= 0x2000 || address < 0) {
    printf("INVALID WRAM ADDR %08X\n", address + 0xC000);
    exit(-1);
  }

  ram_ctx.wram[address] = value;
}

uint8_t hram_read(uint16_t address) {
  address -= 0xFF80;

  if (address >= 0x0080 || address < 0) {
    printf("INVALID HRAM ADDR %08X\n", address + 0xFF80);
    exit(-1);
  }

  return ram_ctx.hram[address];
}

void hram_write(uint16_t address, uint8_t value) {
  address -= 0xFF80;

  if (address >= 0x0080 || address < 0) {
    printf("INVALID HRAM ADDR %08X\n", address + 0xFF80);
    exit(-1);
  }

  ram_ctx.hram[address] = value;
}
