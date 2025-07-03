#include <bus.h>
#include <stdint.h>
#include <sys/types.h>

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

uint8_t bus_read(uint16_t address) {
    if (address < 0x8000) {
        return cart_read(address);
    }

    printf("UNSUPPORTED bus_read(%04X)\n", address);

    // NO_IMPL
}

void bus_write(uint16_t address, uint8_t value) {
    if (address < 0x8000) {
        cart_write(address, value);
        return;
    }

    printf("UNSUPPORTED bus_write(%04X, %02X)\n", address, value);
    
    // NO_IMPL
}

uint16_t bus_read16(uint16_t address) {
    uint16_t lo = bus_read(address);
    uint16_t hi = bus_read(address + 1);

    return lo | (hi << 8);
}

void bus_write16(uint16_t address, uint16_t value) {
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}
