#include "measure_weight.h"
#include "bsp_tm7705.h"

#define FULL_SCALE_KG  100.0f// 满量程100kg

static uint16_t s_code_at_0kg   = 0;      // 空载时的code
static uint16_t s_code_at_100kg = 65535;  // 满量程时的code

// 滤波参数
#define FAST_ALPHA   0.20f
#define SLOW_WIN     32

WeightState_t g_weight;

static float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static float iir(float x, float y_prev, float a)// 一阶IIR滤波
{
    return y_prev + a * (x - y_prev);
}

static float slow_avg(float x)// 简单滑动平均滤波
{
    static float buf[SLOW_WIN];
    static uint16_t idx = 0, filled = 0;
    static float sum = 0;

    sum -= buf[idx];
    buf[idx] = x;
    sum += x;

    idx = (idx + 1) % SLOW_WIN;
    if (filled < SLOW_WIN) filled++;

    return sum / (float)filled;
}


static float code_to_abs_kg(uint16_t code)// 线性映射
{
    // 防止参数异常
    if (s_code_at_100kg <= s_code_at_0kg + 10) {
        return 0.0f;
    }

    float t = (float)(code - s_code_at_0kg) / (float)(s_code_at_100kg - s_code_at_0kg);
    float kg = t * FULL_SCALE_KG;
    return clampf(kg, 0.0f, FULL_SCALE_KG);
}

void Weight_Init(void)
{
    g_weight.abs_fast_kg = 0;
    g_weight.abs_slow_kg = 0;
    g_weight.tare_abs_kg = 0;
    g_weight.valid = false;
}

bool Weight_UpdateFromTM7705(void)// 从TM7705读取数据并更新滤波值
{
    uint16_t code;
    if (!TM7705_ReadCode(&code)) 
    {
        g_weight.valid = false;
        return false;
    }

    float abs_kg = code_to_abs_kg(code);

    g_weight.abs_fast_kg = iir(abs_kg, g_weight.abs_fast_kg, FAST_ALPHA);
    g_weight.abs_slow_kg = slow_avg(abs_kg);

    g_weight.valid = true;
    return true;
}

void Weight_TareNow(void)
{
    g_weight.tare_abs_kg = g_weight.abs_fast_kg;
}
