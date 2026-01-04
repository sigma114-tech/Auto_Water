#pragma once
#include "main.h"
#include <stdbool.h>
#include <stdint.h>

void TM7705_Init(void);
bool TM7705_WaitReady(uint32_t timeout_ms);
bool TM7705_ReadCode(uint16_t* out_code);
void TM7705_DrdyIRQ(void);
