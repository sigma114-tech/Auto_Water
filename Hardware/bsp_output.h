#pragma once
#include "main.h"

typedef enum {
    LED_Quantitative,     // 定量模式
    LED_NonQuantitative,  // 不定量模式
    LED_Drain,            // 排水状态
    LED_Speed1,           // 速度1
    LED_Speed2,           // 速度2
    LED_Speed3,           // 速度3
    LED_InternalDrain,    // 内排
    LED_ExternalDrain,    // 外排
    LED_LowAlarm,         // 低水位报警

    LED_COUNT             // LED数量
} StateLed_t;

typedef enum {
    OUT_InternalDrain,    // 内排
    OUT_ExternalDrain,    // 外排
    OUT_Speed1,           // 速度1
    OUT_Speed2,           // 速度2
    OUT_Speed3,           // 速度3
    OUT_Motor,            // 变频启停
    OUT_Buzzer,           // 蜂鸣器

    OUT_COUNT             // 输出数量
} Output_t;

void Output_Init(void);

void LED_On(StateLed_t led);
void LED_Off(StateLed_t led);
void LED_Toggle(StateLed_t led);

void Out_Set(Output_t out);
void Out_Reset(Output_t out);
void Out_Toggle(Output_t out);

void RS485_SetTx(void);
void RS485_SetRx(void);
