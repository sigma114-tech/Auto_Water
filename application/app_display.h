#pragma once
#include "bsp_display.h"

// 初始化显示模块
void AppDisplay_Init(void);

// 设置 PV 与 SV 的显示值
// - pv: 显示在数码管前四位，保留两位小数（左对齐到前四位）
// - sv: 显示在数码管后两位（整数，0~99）
void AppDisplay_SetValues(float pv, float sv);

// 在定时器中断回调里调用，用于数码管扫描
void AppDisplay_ScanISR(void);
