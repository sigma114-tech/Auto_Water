#include "ctrl_logic.h"
#include "app_state.h"
#include "measure_weight.h"

#define AUTO_ZERO_THRESH   1  // 接近零的阈值，根据实际需求调整

void Control_AutoMode(void)
{
    // 任何报警：进入 ALARM（输出层也会强制停机）
    if (g_sys.alarm_low_level || g_sys.alarm_overload)
    {
        g_sys.auto_state = AUTO_ALARM;
    }

    if (g_sys.req_stop) // 停止请求：直接回待机
    {
        g_sys.req_stop = false;
        g_sys.auto_state = AUTO_IDLE;
    }

    switch (g_sys.auto_state)
    {
        case AUTO_IDLE:
        {
            // 待机：确保不动作
            g_sys.is_running_supply = false;
            g_sys.is_running_drain  = false;
            if (g_sys.req_start)
            {
                g_sys.req_start = false; // 清除请求标志
                g_sys.auto_state = AUTO_TARE;
            }
        } break;

        case AUTO_TARE:
        {
            // 去皮
            Weight_TareNow();
            g_sys.process_value = 0;  // 去皮后清零显示

            // 初始速度
            g_sys.speed = SPEED_1;

            g_sys.auto_state = AUTO_START_PUMP;
        } break;

        case AUTO_START_PUMP:
        {
            // 开泵前确保不排水
            g_sys.is_running_drain  = false;
            g_sys.is_running_supply = true;

            // 进入速度状态（也可以直接进 SPEED1）
            g_sys.auto_state = AUTO_SPEED1;
        } break;

        case AUTO_SPEED1:
        case AUTO_SPEED2:
        case AUTO_SPEED3:
        {
            // 加水过程：一直开泵、不开排水
            g_sys.is_running_drain  = false;
            g_sys.is_running_supply = true;

            // 动态变速
            Speed_t target = Speed_AutoAdjust(g_sys.process_value, g_sys.set_value);
            g_sys.speed = target;

            // 接近设定值停泵
            if (Control_IsFillComplete(g_sys.process_value, g_sys.set_value))
            {
                g_sys.auto_state = AUTO_STOP_PUMP;
                break;
            }

            // 状态跟随速度
            if (target == SPEED_1) g_sys.auto_state = AUTO_SPEED1;
            else if (target == SPEED_2) g_sys.auto_state = AUTO_SPEED2;
            else g_sys.auto_state = AUTO_SPEED3;

        } break;

        case AUTO_STOP_PUMP:
        {
            // 停泵
            g_sys.is_running_supply = false;
            g_sys.is_running_drain  = false;

            // 开始内排
            g_sys.drain_dir = DRAIN_IN;

            g_sys.auto_state = AUTO_DRAIN_START;
        } break;

        case AUTO_DRAIN_START:
        {
            // 开始排水（泵关、阀开）
            g_sys.is_running_supply = false;
            g_sys.is_running_drain  = true;

            g_sys.auto_state = AUTO_DRAIN_WAIT_ZERO;
        } break;

        case AUTO_DRAIN_WAIT_ZERO:
        {
            // 持续排水，直到“接近零”
            g_sys.is_running_supply = false;
            g_sys.is_running_drain  = true;

            if (g_sys.process_value <= AUTO_ZERO_THRESH)// 接近零的阈值
            {
                g_sys.auto_state = AUTO_FINISH;
            }
        } break;

        case AUTO_FINISH:
        {
            // 完成：全停，回待机
            g_sys.is_running_supply = false;
            g_sys.is_running_drain  = false;

            g_sys.auto_state = AUTO_IDLE;
        } break;

        case AUTO_ALARM:
        default:
        {
            // 报警态：等报警清除后回待机
            g_sys.is_running_supply = false;
            g_sys.is_running_drain  = false;

            if (!g_sys.alarm_low_level && !g_sys.alarm_overload)
            {
                g_sys.auto_state = AUTO_IDLE;
            }
        } break;
    }
}
