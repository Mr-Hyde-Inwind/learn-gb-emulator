#pragma once
#include <common.h>

typedef struct {
  bool start;
  bool select;
  bool a;
  bool b;
  bool up;
  bool down;
  bool left;
  bool right;
} GamepadState;

void GamepadInit();
bool GamepadButtonSelect();
bool GamepadDirSelect();
void GamepadSetSelect(uint8_t value);

GamepadState* GamepadGetState();

uint8_t GamepadGetOutput();