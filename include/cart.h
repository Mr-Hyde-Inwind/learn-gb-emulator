#pragma once
#include <common.h>

static const char* CART_TYPE[0x100] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM",
    "ROM+RAM+BATTERY",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 10",
    "MBC3",
    "MBC3+RAM 10",
    "MBC3+RAM+BATTERY 10",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
    "POCKET CAMERA",                        // $FC POCKET CAMERA
    "BANDAI TAMA5",                         // $FD BANDAI TAMA5
    "HuC3",                                 // $FE HuC3
    "HuC1+RAM+BATTERY"                      // $FF HuC1+RAM+BATTERY
};

static const char* OLD_LIC_CODE[0x100] = {
  [0x00] = "None",
  [0x01] = "Nintendo",
  [0x08] = "Capcom",
  [0x09] = "HOT-B",
  [0x0A] = "Jaleco",
  [0x0B] = "Coconuts Japan",
  [0x0C] = "Elite Systems",
  [0x13] = "EA (Electronic Arts)",
  [0x18] = "Hudson Soft",
  [0x19] = "ITC Entertainment",
  [0x1A] = "Yanoman",
  [0x1D] = "Japan Clary",
  [0x1F] = "Virgin Games Ltd.",
  [0x24] = "PCM Complete",
  [0x25] = "San-X",
  [0x28] = "Kemco",
  [0x29] = "SETA Corporation",
  [0x30] = "Infogrames",
  [0x31] = "Nintendo",
  [0x32] = "Bandai",
  [0x33] = "Indicates that the New licensee code should be used instead.",
  [0x34] = "Konami",
  [0x35] = "HectorSoft",
  [0x38] = "Capcom",
  [0x39] = "Banpresto",
  [0x3C] = "Entertainment Interactive (stub)",
  [0x3E] = "Gremlin",
  [0x41] = "Ubi Soft",
  [0x42] = "Atlus",
  [0x44] = "Malibu Interactive",
  [0x46] = "Angel",
  [0x47] = "Spectrum HoloByte",
  [0x49] = "Irem",
  [0x4A] = "Virgin Games Ltd.",
  [0x4D] = "Malibu Interactive",
  [0x4F] = "U.S. Gold",
  [0x50] = "Absolute",
  [0x51] = "Acclaim Entertainment",
  [0x52] = "Activision",
  [0x53] = "Sammy USA Corporation",
  [0x54] = "GameTek",
  [0x55] = "Park Place",
  [0x56] = "LJN",
  [0x57] = "Matchbox",
  [0x59] = "Milton Bradley Company",
  [0x5A] = "Mindscape",
  [0x5B] = "Romstar",
  [0x5C] = "Naxat Soft",
  [0x5D] = "Tradewest",
  [0x60] = "Titus Interactive",
  [0x61] = "Virgin Games Ltd.",
  [0x67] = "Ocean Software",
  [0x69] = "EA (Electronic Arts)",
  [0x6E] = "Elite Systems",
  [0x6F] = "Electro Brain",
  [0x70] = "Infogrames",
  [0x71] = "Interplay Entertainment",
  [0x72] = "Broderbund",
  [0x73] = "Sculptured Software",
  [0x75] = "The Sales Curve Limited",
  [0x78] = "THQ",
  [0x79] = "Accolade",
  [0x7A] = "Triffix Entertainment",
  [0x7C] = "MicroProse",
  [0x7F] = "Kemco",
  [0x80] = "Misawa Entertainment",
  [0x83] = "LOZC G.",
  [0x86] = "Tokuma Shoten",
  [0x8B] = "Bullet-Proof Software",
  [0x8C] = "Vic Tokai Corp.",
  [0x8E] = "Ape Inc.",
  [0x8F] = "I’Max",
  [0x91] = "Chunsoft Co.",
  [0x92] = "Video System",
  [0x93] = "Tsubaraya Productions",
  [0x95] = "Varie",
  [0x96] = "Yonezawa/S’Pal",
  [0x97] = "Kemco",
  [0x99] = "Arc",
  [0x9A] = "Nihon Bussan",
  [0x9B] = "Tecmo",
  [0x9C] = "Imagineer",
  [0x9D] = "Banpresto",
  [0x9F] = "Nova",
  [0xA1] = "Hori Electric",
  [0xA2] = "Bandai",
  [0xA4] = "Konami",
  [0xA6] = "Kawada",
  [0xA7] = "Takara",
  [0xA9] = "Technos Japan",
  [0xAA] = "Broderbund",
  [0xAC] = "Toei Animation",
  [0xAD] = "Toho",
  [0xAF] = "Namco",
  [0xB0] = "Acclaim Entertainment",
  [0xB1] = "ASCII Corporation or Nexsoft",
  [0xB2] = "Bandai",
  [0xB4] = "Square Enix",
  [0xB6] = "HAL Laboratory",
  [0xB7] = "SNK",
  [0xB9] = "Pony Canyon",
  [0xBA] = "Culture Brain",
  [0xBB] = "Sunsoft",
  [0xBD] = "Sony Imagesoft",
  [0xBF] = "Sammy Corporation",
  [0xC0] = "Taito",
  [0xC2] = "Kemco",
  [0xC3] = "Square",
  [0xC4] = "Tokuma Shoten",
  [0xC5] = "Data East",
  [0xC6] = "Tonkin House",
  [0xC8] = "Koei",
  [0xC9] = "UFL",
  [0xCA] = "Ultra Games",
  [0xCB] = "VAP, Inc.",
  [0xCC] = "Use Corporation",
  [0xCD] = "Meldac",
  [0xCE] = "Pony Canyon",
  [0xCF] = "Angel",
  [0xD0] = "Taito",
  [0xD1] = "SOFEL (Software Engineering Lab)",
  [0xD2] = "Quest",
  [0xD3] = "Sigma Enterprises",
  [0xD4] = "ASK Kodansha Co.",
  [0xD6] = "Naxat Soft",
  [0xD7] = "Copya System",
  [0xD9] = "Banpresto",
  [0xDA] = "Tomy",
  [0xDB] = "LJN",
  [0xDD] = "Nippon Computer Systems",
  [0xDE] = "Human Ent.",
  [0xDF] = "Altron",
  [0xE0] = "Jaleco",
  [0xE1] = "Towa Chiki",
  [0xE2] = "Yutaka # Needs more info",
  [0xE3] = "Varie",
  [0xE5] = "Epoch",
  [0xE7] = "Athena",
  [0xE8] = "Asmik Ace Entertainment",
  [0xE9] = "Natsume",
  [0xEA] = "King Records",
  [0xEB] = "Atlus",
  [0xEC] = "Epic/Sony Records",
  [0xEE] = "IGS",
  [0xF0] = "A Wave",
  [0xF3] = "Extreme Entertainment",
  [0xFF] = "LJN"
};

typedef struct {
  /*
        https://gbdev.io/pandocs/The_Cartridge_Header.htmlhttps://gbdev.io/pandocs/The_Cartridge_Header.html
        0100 - 0103         -- Entry point
        0104 - 0133         -- Nintendo logo
        0134 - 0143         -- Title
          |- 013F - 0142    -- Manufacturer Code (Purpose of this code is unkown)
          |- 
        
        0144 - 0145         -- New licensee code
        0146                -- SGB flag
        0147                -- Cartridge type
        0148                -- ROM size


  */

  uint8_t   entry_point[4];
  uint8_t   logo[0x30];
  char      title[0x10];
  uint16_t  new_lic_code;
  uint8_t   sgb_flag;
  uint8_t   cart_type;
  uint8_t   rom_size;           // ROM size is given by 32KiB * (1 << <value>)
  uint8_t   ram_size;
  uint8_t   dest_code;
  uint8_t   old_lic_code;
  uint8_t   mask_rom_version;
  uint8_t   checksum;
  uint16_t  global_checksum;
} cart_header;

bool cart_load(char* cart);

uint8_t cart_read(uint16_t address);
uint8_t cart_write(uint16_t address, uint8_t value);
