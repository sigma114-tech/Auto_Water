#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float abs_fast_kg;   // 控制用
    float abs_slow_kg;   // 显示用
    float tare_abs_kg;   // 去皮基准（绝对）

    bool  valid;         // 本次数据有效
} WeightState_t;

extern WeightState_t g_weight;

void  Weight_Init(void);
bool  Weight_UpdateFromTM7705(void);  // 直接读 TM7705 并更新滤波输出
void  Weight_TareNow(void);

static inline float Weight_NetFastKg(void){ return g_weight.abs_fast_kg - g_weight.tare_abs_kg; }
static inline float Weight_NetSlowKg(void){ return g_weight.abs_slow_kg - g_weight.tare_abs_kg; }
