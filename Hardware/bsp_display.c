#include "bsp_display.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>
#include <math.h>

// 共阴极段码表（a~g），dp 用 bit7
static const uint8_t segLUT[10] = {
    0x3F,/*0*/ 0x06,/*1*/ 0x5B,/*2*/ 0x4F,/*3*/
    0x66,/*4*/ 0x6D,/*5*/ 0x7D,/*6*/ 0x07,/*7*/
    0x7F,/*8*/ 0x6F /*9*/
};

void Disp_Init(Disp_t *d)//初始化
{
    for (int i = 0; i < DISP_DIGITS; i++)
        d->buf[i] = 0x00;
    d->scan_idx = 0;
}

static inline void write_segments(uint8_t bits)
{
    HAL_GPIO_WritePin(Display_SA_GPIO_Port, Display_SA_Pin, (bits & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SB_GPIO_Port, Display_SB_Pin, (bits & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SC_GPIO_Port, Display_SC_Pin, (bits & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SD_GPIO_Port, Display_SD_Pin, (bits & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SE_GPIO_Port, Display_SE_Pin, (bits & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SF_GPIO_Port, Display_SF_Pin, (bits & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SG_GPIO_Port, Display_SG_Pin, (bits & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Display_SDP_GPIO_Port, Display_SDP_Pin, (bits & 0x80) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static inline void select_digit(uint8_t idx)
{
    // 禁能译码器
    HAL_GPIO_WritePin(Display_238D_EN_GPIO_Port, Display_238D_EN_Pin, GPIO_PIN_RESET);

    // 译码输入
    HAL_GPIO_WritePin(Display_238D_A0_GPIO_Port, Display_238D_A0_Pin, (idx & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Display_238D_A1_GPIO_Port, Display_238D_A1_Pin, (idx & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Display_238D_A2_GPIO_Port, Display_238D_A2_Pin, (idx & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // 使能译码器
    HAL_GPIO_WritePin(Display_238D_EN_GPIO_Port, Display_238D_EN_Pin, GPIO_PIN_SET);
}

void Disp_SetFloat(Disp_t *d, float value, uint8_t decimals)
{
    if (value > (float)(powf(10, DISP_DIGITS)-1)) value = (float)(powf(10, DISP_DIGITS)-1);
    if (value < 0.0f) value = 0.0f;

    // 放大对应倍数
    int scaled = (int)(value * powf(10, decimals));

    // 清空显示缓存
    for (int i = 0; i < DISP_DIGITS; i++)
        d->buf[i] = 0x00;
    // 填充各位数字
    for (int i = DISP_DIGITS-1; i >= 0; i--)
    {
        int digit = scaled % 10;
        d->buf[i] = segLUT[digit];
        scaled /= 10;
        // 小数点位置
        if (decimals < DISP_DIGITS)
            d->buf[DISP_DIGITS-1-decimals] |= 0x80;
        // 个位前的数为0则结束
        if (i<=DISP_DIGITS-1-decimals && scaled == 0)
            break;
    }
}

void Disp_ScanISR(Disp_t *d)
{
    // 关闭上一个位选
    HAL_GPIO_WritePin(Display_238D_EN_GPIO_Port, Display_238D_EN_Pin, GPIO_PIN_RESET);

    // 下一个位
    d->scan_idx++;
    if (d->scan_idx >= DISP_DIGITS)
        d->scan_idx = 0;
    // 输出段码
    write_segments(d->buf[d->scan_idx]);
    // 选通当前位
    select_digit(d->scan_idx);
}
