#pragma once
#include <common.h>

void DmaStart(uint8_t start);

void DmaTick();

bool IsTransferring();