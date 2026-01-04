#pragma once
#include "main.h"

// 你的硬件一共几位数码管
#define DISP_DIGITS 6

typedef struct {
    uint8_t buf[DISP_DIGITS];  // 每位的段码，bit0~6=a~g, bit7=dp
    uint8_t scan_idx;          // 当前扫描位 [0..DISP_DIGITS-1]
} Disp_t;

void Disp_Init(Disp_t *d);
void Disp_SetFloat(Disp_t *d, float value, uint8_t decimals);
void Disp_ScanISR(Disp_t *d);  // 放在 TIM3 的中断回调里调用
