#include "app_display.h"
#include <string.h>

static Disp_t disp;

// 初始化显示设备
void AppDisplay_Init(void)
{
	Disp_Init(&disp);
}

// 将pv显示在前四位（保留两位小数，左对齐），sv显示在后两位（整数）
void AppDisplay_SetValues(float pv, float sv)
{
	Disp_t tmp;
	// 清空目标缓存
	for (int i = 0; i < DISP_DIGITS; i++)
		disp.buf[i] = 0x00;

	// 1) 生成pv的右对齐表示（保留 2 位小数）
	Disp_Init(&tmp);
	Disp_SetFloat(&tmp, pv, 2);
	// 将右对齐的pv部分移动到前四位
	// 偏移量 = DISP_DIGITS - 4
	const int shift = DISP_DIGITS - 4;
	for (int i = 0; i < 4; i++) {
		int src_idx = i + shift;
		if (src_idx >= 0 && src_idx < DISP_DIGITS)
			disp.buf[i] = tmp.buf[src_idx];
		else
			disp.buf[i] = 0x00;
	}

	// 2) 生成sv的表示（整数），并放到后两位
	Disp_Init(&tmp);
	Disp_SetFloat(&tmp, sv, 0);
	// 复制最后两位
	if (DISP_DIGITS >= 2) {
		disp.buf[DISP_DIGITS-2] = tmp.buf[DISP_DIGITS-2];
		disp.buf[DISP_DIGITS-1] = tmp.buf[DISP_DIGITS-1];
	}
}

// 数码管扫描中断调用
void AppDisplay_ScanISR(void)
{
	Disp_ScanISR(&disp);
}

