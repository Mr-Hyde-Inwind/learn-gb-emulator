#include <cart.h>

typedef struct {
    char filename[1024];
    uint32_t rom_size;
    uint8_t* rom_data;
    cart_header* header;
} cart_ctx;

static cart_ctx ctx;

const char* cart_type_name() {
    if (ctx.header->cart_type <= 0x22) {
        return CART_TYPE[ctx.header->cart_type];
    } else {
        return "UNKNOWN";
    }
}

const char* lic_code_name() {
    if (ctx.header->old_lic_code <= 0xFF) {
        return OLD_LIC_CODE[ctx.header->old_lic_code];
    } else {
        return "UNKNOWN";
    }
}

bool cart_load(char* cart) {
    FILE *fp = fopen(cart, "r");

    if (!fp) {
        printf("Failed to open %s\n", cart);
        return false;
    }

    snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart);
    printf("Opened: %s\n", cart);

    fseek(fp, 0, SEEK_END);
    ctx.rom_size = ftell(fp);

    rewind(fp);
    ctx.rom_data = malloc(ctx.rom_size);
    fread(ctx.rom_data, ctx.rom_size, 1, fp);
    fclose(fp);

    ctx.header = (cart_header*)(ctx.rom_data + 0x100);
    ctx.header->title[15] = 0;

    printf("Cartridge Loaded:\n");
    printf("\t Title        : %s\n", ctx.header->title);
    printf("\t Type         : %2.2X (%s)\n", ctx.header->cart_type, cart_type_name());
    printf("\t ROM Size     : %d KB\n", (1 << ctx.header->rom_size)*32);
    printf("\t RAM Size     : %2.2X\n", ctx.header->ram_size);
    printf("\t Lic Code     : %2.2X (%s)\n", ctx.header->old_lic_code, lic_code_name());
    printf("\t ROM Version  : %2.2X\n", ctx.header->mask_rom_version);

    uint16_t checksum = 0;
    for (uint16_t i = 0x0134; i <= 0x014C; i++) {
        checksum = checksum - ctx.rom_data[i] - 1;
    }

    printf("\t Checksum     : %2.2X (%s)\n",
        ctx.header->checksum,
        ((checksum & 0xFF) == ctx.header->checksum) ? "PASSED" : "FAILED");

    return true;
}

uint8_t cart_read(uint16_t address) {
    return ctx.rom_data[address];
}

uint8_t cart_write(uint16_t address, uint8_t value) {
    printf("cart_write\n");
    NO_IMPL
}