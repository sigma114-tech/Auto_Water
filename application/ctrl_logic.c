#include "ctrl_logic.h"
#include "app_state.h"
#include "bsp_key.h"
#include "bsp_output.h"
#include "bsp_tm7705.h"
#include "measure_weight.h"

#define SET_VALUE_MIN   0    // SV最小值
#define SET_VALUE_MAX   59   // SV最大值

#define LOW_LEVEL_KG        (-0.5f)   // 容忍负值
#define LOW_LEVEL_CNT_MAX   25        // 25 * 20ms = 500ms

#define OL_ON_KG   60       // 过载报警阈值
#define OL_OFF_KG  59       // 过载报警解除阈值

static uint16_t s_low_level_cnt = 0;


static void Control_UpdateMeasure(void)// 更新测量值
{
    if (!Weight_UpdateFromTM7705())
        return;

    // 绝对重量
    float abs_kg = g_weight.abs_slow_kg;

    // 净重
    float net_kg = Weight_NetFastKg();
    if (net_kg < 0) net_kg = 0;

    // 按 1kg 量化写回（四舍五入）
    g_sys.weight = abs_kg ;
}

void Control_Run(void)
{
    Control_UpdateMeasure();// 更新测量值

    Control_HandleKeys();// 处理按键事件（改写 g_sys 中的模式、速度、方向、操作意图）

    if (g_sys.mode == MODE_AUTO)// 自动模式状态机
        Control_AutoMode();

    if (g_sys.mode == MODE_MANUAL)// 手动模式状态机
        Control_ManualMode();

    Control_Safety();// 安全检查
    
    Control_UpdateOutputs();// 刷新输出

    Control_UpdateLEDs();// 刷新 LED 显示

    // 更新 process_value（由独立函数维护，保持 Control_Run 简洁）
    Control_UpdateProcessValue();
}

// 处理 process_value：根据运行态决定显示内容
void Control_UpdateProcessValue(void)
{
    bool now_running = is_system_running();

    if (!now_running)
    {
        // 非运行态：PV 显示为当前绝对重量
        g_sys.process_value = g_sys.weight;
    }
    else
    {
        // 已在运行态：直接使用测量层的净重（已减去测量层的 tare）
        // 去皮与显示清零由状态机在 Weight_TareNow() 后执行
        float pv = Weight_NetFastKg();
        if (pv < 0) pv = 0;
        g_sys.process_value =pv;
    }
}

void clamp_set_value(void)
{
#if (SET_VALUE_MIN > 0)
    if (g_sys.set_value < SET_VALUE_MIN)
        g_sys.set_value = SET_VALUE_MIN;
#endif
    if (g_sys.set_value > SET_VALUE_MAX)
        g_sys.set_value = SET_VALUE_MAX;
}

void Control_HandleKeys(void)
{
    if (Key_ON(Key_Mode_Simulation))// 模拟模式切换
    {
        g_sys.sim_mode = true;
    }
    else 
    {
        g_sys.sim_mode = false;
    }

    Mode_t target_mode = Key_ON(Key_Mode_AutoHand) ? MODE_AUTO : MODE_MANUAL;
    if (target_mode != g_sys.mode)
    {
        g_sys.mode = target_mode;
        g_sys.req_start = false;        // 清除请求标志
        g_sys.req_stop  = false;        // 清除请求标志

        if (g_sys.mode == MODE_AUTO)
        {
            g_sys.auto_state = AUTO_IDLE;
        }
        else
        {
            g_sys.man_state  = MAN_IDLE;
        }
    }

    
    if (g_sys.mode == MODE_AUTO)// 自动模式下只处理自动相关的按键
    {
        if (g_sys.auto_state == AUTO_IDLE)// 自动待机时只处理设置相关的按键
        {
            // 设定给定值
            if (Key_WasPressed(Key_Adjust_Up) || Key_WasRepeated(Key_Adjust_Up))
            {
                g_sys.set_value++;
                clamp_set_value();
            }

            if (Key_WasPressed(Key_Adjust_Down) || Key_WasRepeated(Key_Adjust_Down))
            {
                if (g_sys.set_value > 0) g_sys.set_value--;
                clamp_set_value();
            }

            // 上水启动,产生“开始请求”
            if (Key_WasPressed(Key_Supply_ON))
            {
                g_sys.req_start = true;
            }
        }

        if (Key_WasPressed(Key_Supply_OFF))// 自动运行时只允许上水停止
        {
            g_sys.req_stop = true;
        }
        return; // 自动模式下不处理手动按键
    }

    if (g_sys.mode == MODE_MANUAL)// 手动模式下只处理手动相关的按键
    {
        if (g_sys.man_state == MAN_IDLE)
        {
            // 手动方式循环
            if (Key_WasPressed(Key_Manual_SetWay))
            {
                if (g_sys.manual_mode == MAN_DRAIN) g_sys.manual_mode = MAN_QUANT;
                else g_sys.manual_mode++;
            }

            // 速度循环
            if (Key_WasPressed(Key_Manual_SetSpeed))
            {
                if (g_sys.speed == SPEED_3) g_sys.speed = SPEED_1;
                else g_sys.speed++;
            }

            // 排水方向
            if (Key_WasPressed(Key_Manual_SetDrainDir))
            {
                g_sys.drain_dir = (g_sys.drain_dir == DRAIN_IN) ? DRAIN_OUT : DRAIN_IN;
            }

            // 设定给定值
            if (Key_WasPressed(Key_Adjust_Up) || Key_WasRepeated(Key_Adjust_Up))
            {
                g_sys.set_value++;
                clamp_set_value();
            }

            if (Key_WasPressed(Key_Adjust_Down) || Key_WasRepeated(Key_Adjust_Down))
            {
                if (g_sys.set_value > 0) g_sys.set_value--;
                clamp_set_value();
            }

            // 启动请求
            if (Key_WasPressed(Key_Supply_ON))
            {
                if (g_sys.manual_mode == MAN_QUANT || g_sys.manual_mode == MAN_NONQUANT)
                    g_sys.req_start = true;
            }

            if (Key_WasPressed(Key_Drainage_ON))
            {
                if (g_sys.manual_mode == MAN_DRAIN)
                    g_sys.req_start = true;
            }
        }
        // 手动运行时只允许停止
        if (Key_WasPressed(Key_Supply_OFF) || Key_WasPressed(Key_Drainage_OFF))
        {
            g_sys.req_stop = true;
        }
    }
}

bool Control_IsFillComplete(uint16_t pv, uint16_t sv)// 判断给定条件下是否停止加水
{
    //if (sv == 0) return false;
    
    const uint16_t THRESH = 1; // 接近阈值，需要调试
    
    return (pv + THRESH >= sv);
}

bool is_system_running(void)
{
    if (g_sys.mode == MODE_AUTO)
        return (g_sys.auto_state != AUTO_IDLE);
    else
        return (g_sys.man_state == MAN_RUN);
}

void Control_Safety(void)
{
    float net = Weight_NetFastKg();

    // 低水位报警
    if (is_system_running())
    {
        if (net <= LOW_LEVEL_KG)
        {
            if (s_low_level_cnt < LOW_LEVEL_CNT_MAX)
                s_low_level_cnt++;
        }
        else
        {
            s_low_level_cnt = 0;
        }

        if (s_low_level_cnt >= LOW_LEVEL_CNT_MAX)
            g_sys.alarm_low_level = true;
    }
    else
    {
        s_low_level_cnt = 0;
        g_sys.alarm_low_level = false;
    }

    // 过载报警
    if (!g_sys.alarm_overload)
    {
        if (g_sys.weight >= OL_ON_KG)
            g_sys.alarm_overload = true;
    }
    else
    {
        if (g_sys.weight <= OL_OFF_KG)
            g_sys.alarm_overload = false;
    }

    if (g_sys.alarm_low_level || g_sys.alarm_overload)// 报警时强制进入报警态
    {
        g_sys.req_start = false;
        g_sys.req_stop  = true;   // 让状态机尽快回待机
        g_sys.man_state = MAN_ALARM;
        g_sys.auto_state = AUTO_ALARM;
    }
}
