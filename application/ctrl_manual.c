#include "ctrl_logic.h"
#include "app_state.h"
#include "bsp_key.h"
#include "bsp_output.h"
#include "measure_weight.h"


static void Manual_Idle(void);
static void Manual_Run(void);
static void Manual_Alarm(void);

void Control_ManualMode(void)// 主状态
{
    switch (g_sys.man_state)
    {
        case MAN_IDLE:  Manual_Idle(); break;
        case MAN_RUN:   Manual_Run();  break;
        case MAN_ALARM: Manual_Alarm(); break;
    }
}


static void Manual_Idle(void)// 手动待机状态
{
    g_sys.is_running_supply = false;
    g_sys.is_running_drain  = false;// 清除标志位

    if (g_sys.req_start && (g_sys.manual_mode==MAN_QUANT || g_sys.manual_mode==MAN_NONQUANT)) // 加水模式
    {
        g_sys.req_start = false;// 清除标志位

        if (g_sys.manual_mode==MAN_QUANT) 
        {
            Weight_TareNow();
            g_sys.process_value = 0;  // 去皮后清零显示
            g_sys.speed = Speed_AutoAdjust(g_sys.process_value, g_sys.set_value);
        }
        g_sys.is_running_supply = true;
        g_sys.man_state = MAN_RUN;
        return;
    }

    if (g_sys.req_start && g_sys.manual_mode==MAN_DRAIN) // 排水模式
    {
        g_sys.req_start = false;// 清除标志位
        g_sys.is_running_drain = true;
        g_sys.man_state = MAN_RUN;
        return;
    }
}

static void Manual_Run(void)// 手动运行状态
{
    // 停止请求
    if (g_sys.req_stop) 
    {
         g_sys.req_stop=false; 
         g_sys.is_running_supply=false;
         g_sys.is_running_drain=false; 
         g_sys.man_state=MAN_IDLE; 
    }

    // 定量运行：变速、到量停
    if (g_sys.manual_mode==MAN_QUANT && g_sys.is_running_supply) 
    {
        g_sys.speed = Speed_AutoAdjust(g_sys.process_value, g_sys.set_value);
        if (Control_IsFillComplete(g_sys.process_value, g_sys.set_value)) { g_sys.is_running_supply=false; g_sys.man_state=MAN_IDLE; }
    }
}

static void Manual_Alarm(void)// 手动报警状态
{
    // 报警态：等报警清除后回待机
    g_sys.is_running_supply = false;
    g_sys.is_running_drain = false;

    if (!g_sys.alarm_low_level && !g_sys.alarm_overload) {
      g_sys.man_state = MAN_IDLE;
    }
}
