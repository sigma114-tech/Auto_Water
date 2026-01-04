#include "bsp_output.h"
#include "gpio.h"
#include "main.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} OutputMap_t;

static const OutputMap_t stateled_map[LED_COUNT] = {
    [LED_Quantitative]     = {GPIOC, State_LED1_Pin},
    [LED_NonQuantitative]  = {GPIOC, State_LED2_Pin},
    [LED_Drain]            = {GPIOC, State_LED3_Pin},
    [LED_Speed1]           = {GPIOC, State_LED4_Pin},
    [LED_Speed2]           = {GPIOC, State_LED5_Pin},
    [LED_Speed3]           = {GPIOA, State_LED6_Pin},
    [LED_InternalDrain]    = {GPIOB, State_LED7_Pin},
    [LED_ExternalDrain]    = {GPIOB, State_LED8_Pin},
    [LED_LowAlarm]         = {GPIOA, Low_Alarm_Pin},
};

static const OutputMap_t output_map[OUT_COUNT] = {
    [OUT_ExternalDrain]       = {GPIOB, Control_Supply_Pin},
    [OUT_InternalDrain]       = {GPIOB, Control_Drainage_Pin},
    [OUT_Speed1]              = {GPIOB, Control_Speed1_Pin},
    [OUT_Speed2]              = {GPIOB, Control_Speed2_Pin},
    [OUT_Speed3]              = {GPIOB, Control_Speed3_Pin},
    [OUT_Motor]               = {GPIOB, Control_Motor_ON_OFF_Pin},
    [OUT_Buzzer]              = {GPIOB, Control_FM_Pin},
};

void Output_Init(void)// 输出初始化
{
    for (int i = 0; i < LED_COUNT; i++)
        LED_Off((StateLed_t)i);

    for (int i = 0; i < OUT_COUNT; i++)
        Out_Reset((Output_t)i);

    RS485_SetRx();
}


void LED_On(StateLed_t led)// 状态灯亮
{
    HAL_GPIO_WritePin(stateled_map[led].port, stateled_map[led].pin, GPIO_PIN_RESET);
}

void LED_Off(StateLed_t led)// 状态灯灭
{
    HAL_GPIO_WritePin(stateled_map[led].port, stateled_map[led].pin, GPIO_PIN_SET);
}

void LED_Toggle(StateLed_t led)// 状态灯切换
{
    HAL_GPIO_TogglePin(stateled_map[led].port, stateled_map[led].pin);
}

void Out_Set(Output_t out)// 输出置位
{
    HAL_GPIO_WritePin(output_map[out].port, output_map[out].pin, GPIO_PIN_RESET);
}

void Out_Reset(Output_t out)// 输出复位
{
    HAL_GPIO_WritePin(output_map[out].port, output_map[out].pin, GPIO_PIN_SET);
}

void Out_Toggle(Output_t out)// 输出切换
{
    HAL_GPIO_TogglePin(output_map[out].port, output_map[out].pin);
}

void RS485_SetTx(void)// RS485设置为发送模式
{
    HAL_GPIO_WritePin(GPIOB, USART3_485_RE_DE_Pin, GPIO_PIN_SET);
}

void RS485_SetRx(void)// RS485设置为接收模式
{
    HAL_GPIO_WritePin(GPIOB, USART3_485_RE_DE_Pin, GPIO_PIN_RESET);
}
