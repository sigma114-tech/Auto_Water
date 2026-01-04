#include "ctrl_logic.h"
#include "app_state.h"
#include "bsp_output.h"
#include "bsp_key.h"
#include <stdbool.h>

void Control_UpdateOutputs(void)// 输出更新
{
    // 停机
    if (g_sys.alarm_low_level || g_sys.alarm_overload)// 出现任何报警都停机
    {
        g_sys.is_running_supply = false;
        g_sys.is_running_drain  = false;
    }

    // 加水
    if (g_sys.is_running_supply)
        Out_Set(OUT_Motor);
    else
        Out_Reset(OUT_Motor);

    // 排水
    if (g_sys.is_running_drain)
    {
        if (g_sys.drain_dir == DRAIN_IN)// 内排
        {
            Out_Set(OUT_InternalDrain);
            Out_Reset(OUT_ExternalDrain);
        }
        else// 外排
        {
            Out_Set(OUT_ExternalDrain);
            Out_Reset(OUT_InternalDrain);
        }
    }
    else// 不排
    {
        Out_Reset(OUT_InternalDrain);
        Out_Reset(OUT_ExternalDrain);
    }

    // 速度
    Out_Reset(OUT_Speed1);
    Out_Reset(OUT_Speed2);
    Out_Reset(OUT_Speed3);

    if (g_sys.is_running_supply)// 只有加水运行才需要速度档位
    {
        switch (g_sys.speed)
        {
            case SPEED_1: Out_Set(OUT_Speed1); break;
            case SPEED_2: Out_Set(OUT_Speed2); break;
            case SPEED_3: Out_Set(OUT_Speed3); break;
        }
    }

    // 报警
    if (g_sys.alarm_low_level || g_sys.alarm_overload)
        Out_Set(OUT_Buzzer);
    else
        Out_Reset(OUT_Buzzer);
}

static inline void LedWrite(StateLed_t led, bool on)// LED灯控制
{
    if (on) LED_On(led);
    else    LED_Off(led);
}

static inline void AllLedsOff(void)// 所有LED灭
{
    for (int i = 0; i < LED_COUNT; i++)
        LED_Off((StateLed_t)i);
}

static inline void ShowSpeed(bool on)// 速度灯控制
{
    LedWrite(LED_Speed1, on && (g_sys.speed == SPEED_1));
    LedWrite(LED_Speed2, on && (g_sys.speed == SPEED_2));
    LedWrite(LED_Speed3, on && (g_sys.speed == SPEED_3));
}

static inline void ShowManualMode(bool on)// 手动模式灯控制
{
    LedWrite(LED_Quantitative,    on && (g_sys.manual_mode == MAN_QUANT));
    LedWrite(LED_NonQuantitative, on && (g_sys.manual_mode == MAN_NONQUANT));
    LedWrite(LED_Drain,           on && (g_sys.manual_mode == MAN_DRAIN));
}

static inline void ShowDrainDir(bool on)// 排水方向灯控制
{
    // 只有在“排水方式”时显示方向
    if (g_sys.manual_mode != MAN_DRAIN) {
        LedWrite(LED_InternalDrain, false);
        LedWrite(LED_ExternalDrain, false);
        return;
    }
    LedWrite(LED_InternalDrain, on && (g_sys.drain_dir == DRAIN_IN));
    LedWrite(LED_ExternalDrain, on && (g_sys.drain_dir == DRAIN_OUT));
}

void Control_UpdateLEDs(void)// LED灯更新
{
    bool blink_on = (g_blink_on != 0);

    AllLedsOff();

    LedWrite(LED_LowAlarm, g_sys.alarm_low_level);

    // 任意报警：其余灯全灭
    if (g_sys.alarm_low_level || g_sys.alarm_overload)
        return;
    
    // 无报警：水位灯常亮
    if (g_sys.alarm_low_level==false){LED_On(LED_LowAlarm);}

    // 自动模式
    if (g_sys.mode == MODE_AUTO)
    {
        // 加水过程：定量灯 + 速度灯常亮
        switch (g_sys.auto_state)
        {
            case AUTO_SPEED1:
            case AUTO_SPEED2:
            case AUTO_SPEED3:
            case AUTO_START_PUMP:
            case AUTO_TARE:
                LedWrite(LED_Quantitative, true);
                ShowSpeed(true);
                break;

            case AUTO_DRAIN_START:
            case AUTO_DRAIN_WAIT_ZERO:
                // 自动排水
                LedWrite(LED_InternalDrain, true);
                break;

            default:
                break;
        }
        return;
    }

    // 手动模式
    if (g_sys.mode == MODE_MANUAL)
    {
        if (g_sys.man_state == MAN_IDLE)
        {
            // 待机，常亮
            ShowManualMode(true);
            ShowSpeed(true);
            ShowDrainDir(true);
        }
        else if (g_sys.man_state == MAN_RUN)
        {
            // 运行，闪烁
            ShowManualMode(blink_on);
            ShowSpeed(blink_on);

            // 排水方向灯常亮，方便辨认
            ShowDrainDir(true);
        }
    }
}
