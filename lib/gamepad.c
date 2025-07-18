#include <gamepad.h>
#include <string.h>

typedef struct {
  /* data */
  bool button_sel;
  bool dir_sel;
  GamepadState controller;
} GamepadContext;

static GamepadContext ctx = {0};

bool GamepadButtonSelect() {
  return ctx.button_sel;
}

bool GamepadDirSelect() {
  return ctx.dir_sel;
}

void GamepadSetSelect(uint8_t value) {
  ctx.button_sel = value & 0x20;
  ctx.dir_sel = value & 0x10;
}

GamepadState* GamepadGetState() {
  return &ctx.controller;
}

uint8_t GamepadGetOutput() {
  uint8_t output = 0xCF;

  if (!GamepadButtonSelect()) {
    if (GamepadGetState()->start) {
      output &= ~(1 << 3);
    } else if (GamepadGetState()->select) {
      output &= ~(1 << 2);
    } else if (GamepadGetState()->a) {
      output &= ~(1 << 0);
    } else if (GamepadGetState()->b) {
      output &= ~(1 << 1);
    }
  }

  if (!GamepadDirSelect()) {
    if (GamepadGetState()->left) {
      output &= ~(1 << 1);
    } else if (GamepadGetState()->right) {
      output &= ~(1 << 0);
    } else if (GamepadGetState()->up) {
      output &= ~(1 << 2);
    } else if (GamepadGetState()->down) {
      output &= ~(1 << 3);
    }
  }

  return output;
}