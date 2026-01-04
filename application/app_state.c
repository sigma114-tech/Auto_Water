#include "app_state.h"

SystemState_t g_sys = 
{
    .mode = MODE_MANUAL,
    .sim_mode = false,

    .weight = 0,
    .process_value = 0,
    .set_value = 0,

    .manual_mode = MAN_DRAIN,
    .speed = SPEED_1,
    .drain_dir = DRAIN_IN,

    .auto_state = AUTO_IDLE,
    .man_state = MAN_IDLE,

    .is_running_supply = false,
    .is_running_drain = false,

    .req_start = false,
    .req_stop  = false,

    .alarm_low_level = false,
    .alarm_overload = false
};// 声明系统全局变量

void SystemState_Init(void)// 系统状态初始化
{
    // 顶层模式
    g_sys.mode = MODE_MANUAL;
    g_sys.sim_mode = false;
    
    // 称重数据
    g_sys.weight = 0;
    g_sys.process_value = 0;
    g_sys.set_value = 0;

    // 手动模式设置
    g_sys.manual_mode = MAN_DRAIN;
    g_sys.speed = SPEED_0;
    g_sys.drain_dir = DRAIN_IN;
    
    // 状态机状态
    g_sys.auto_state = AUTO_IDLE;
    g_sys.man_state = MAN_IDLE;

    // 输出状态
    g_sys.is_running_supply = false;
    g_sys.is_running_drain = false;
    
    // 用户请求
    g_sys.req_start = false;
    g_sys.req_stop = false;

    // 报警状态
    g_sys.alarm_low_level = false;
    g_sys.alarm_overload = false;
}
