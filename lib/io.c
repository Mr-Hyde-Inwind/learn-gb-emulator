#include <io.h>
#include <cpu.h>
#include <timer.h>
#include <dma.h>


static char serial_data[2];

uint8_t io_read(uint16_t address) {
  if (address == 0xFF01) {
    return serial_data[0];
  }

  if (address == 0xFF02) {
    return serial_data[1];
  }

  if (address >= 0xFF04 && address <= 0xFF07) {
    return timer_read(address);
  }

  if (address == 0xFF0F) {
    return cpu_get_int_flags();
  }

  if (address == 0xFF44) {
    return 0x94;
  }

  printf("UNSUPPORTED bus_read(%04X)\n", address);
  return 0x00;
}

void io_write(uint16_t address, uint8_t value) {
  if (address == 0xFF01) {
    serial_data[0] = value;
    return;
  }

  if (address == 0xFF02) {
    serial_data[1] = value;
    return ;
  }

  if (address >= 0xFF04 && address <= 0xFF07) {
    timer_write(address, value);
    return ;
  }

  if (address == 0xFF0F) {
    cpu_set_int_flags(value);
    return ;
  }

  if (address == 0xFF46) {
    DmaStart(value);
    printf("DMA START!\n");
  }

  printf("UNSUPPORTED bus_write(%04X)\n", address);
}