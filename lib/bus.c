#include <bus.h>
#include <stdint.h>
#include <sys/types.h>
#include <ram.h>
#include <cpu.h>
#include <io.h>
#include <ppu.h>
#include <dma.h>

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
    if (address < 0x8000)                                   // ROM Data
    {
        return cart_read(address);
    }
    else if (address < 0xA000)                              // VRAM
    {
        return ppu_vram_read(address);
    }
    else if (address < 0xC000)                              // 8 KiB External RAM, Cartridge RAM
    {
        return cart_read(address);
    }
    else if (address < 0xD000)                              // 4 KiB Work RAM WRAM
    {
        return wram_read(address);
    }
    else if (address < 0xE000)                              // 4 KiB Work RAM WRA, bank 1-7
    {
        return wram_read(address);
    }
    else if (address < 0xFE00)                              // Echo RAM, mirror of C000-DDFF, use of this area is prohibited
    {
        return 0;
    }
    else if (address < 0xFEA0)                              // Object attribute memory OAM
    {
        if (IsTransferring()) {
            return 0xFF;
        }

        return ppu_oam_read(address);
    }
    else if (address < 0xFF00)                              // Not switchable, use of this area is prohibited
    {
        return 0;
    }
    else if (address < 0xFF80)                              // I/O Registers
    {
        return io_read(address);
    }
    else if (address < 0xFFFF)                              // High RAM HRAM
    {

    }
    else if (address == 0xFFFF)                            // Interrupt Enable register IE
    {
        return cpu_get_ie_register();
    }

    return hram_read(address);
}

void bus_write(uint16_t address, uint8_t value) {
    if (address < 0x8000)
    {
        cart_write(address, value);
        return ;
    }
    else if (address < 0xA000)                              // VRAM
    {
        ppu_vram_write(address, value);
        return ;
    }
    else if (address < 0xC000)                              // 8 KiB External RAM, Cartridge RAM
    {
        cart_write(address, value);
        return ;
    }
    else if (address < 0xD000)                              // 4 KiB Work RAM WRAM
    {
        wram_write(address, value);
        return ;
    }
    else if (address < 0xE000)                              // 4 KiB Work RAM WRA, bank 1-7
    {
        wram_write(address, value);
        return ;
    }
    else if (address < 0xFE00)                              // Echo RAM, mirror of C000-DDFF, use of this area is prohibited
    {
        return ;
    }
    else if (address < 0xFEA0)                              // Object attribute memory OAM
    {
        if (IsTransferring()) {
            return ;
        }

        ppu_oam_write(address, value);
        return ;
    }
    else if (address < 0xFF00)                              // Not switchable, use of this area is prohibited
    {
        return ;
    }
    else if (address < 0xFF80)                              // I/O Registers
    {
        io_write(address, value);
        return ;
    }
    else if (address == 0xFFFF)                            // Interrupt Enable register IE
    {
        cpu_set_ie_register(value);
        return ;
    }
    else
    {
        hram_write(address, value);
        return ;
    }
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
