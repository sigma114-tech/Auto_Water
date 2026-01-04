#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 运行模式
typedef enum {
    MODE_MANUAL = 0,
    MODE_AUTO
} Mode_t;

// 速度档
typedef enum {
		SPEED_0 = 0,
    SPEED_1,
    SPEED_2,
    SPEED_3
} Speed_t;

// 排水方向
typedef enum {
    DRAIN_IN = 0,
    DRAIN_OUT
} DrainDir_t;

typedef enum {
    MAN_QUANT = 0,  // 手动定量加水
    MAN_NONQUANT,   // 手动不定量
    MAN_DRAIN,      // 手动排水
}ManualMode_t;// 手动方式

typedef enum {
    MAN_IDLE = 0,
    MAN_RUN,     
    MAN_ALARM
} ManualState_t;// 手动模式状态

typedef enum {
    AUTO_IDLE = 0,         
    AUTO_TARE,             // 去皮
    AUTO_START_PUMP,       // 启动水泵
    AUTO_SPEED1,           // 速度档1
    AUTO_SPEED2,           // 速度档2
    AUTO_SPEED3,           // 速度档3
    AUTO_STOP_PUMP,        // 停止水泵
    AUTO_DRAIN_START,      // 开始排水
    AUTO_DRAIN_WAIT_ZERO,  // 等待排水至零
    AUTO_FINISH,           // 完成
    AUTO_ALARM             
} AutoState_t;// 自动模式状态

typedef struct
{
    // 顶层模式
    Mode_t mode;             // 自动/手动
    bool sim_mode;           // 模拟模式
    
    // 称重数据
    float weight;          // 当前称重数据
    float process_value;   // PV
    uint16_t set_value;       // SV

    // 手动模式设置
    ManualMode_t manual_mode;    // 手动方式
    Speed_t speed;               // 当前速度档位
    DrainDir_t drain_dir;        // 内排/外排选择
    
    // 状态机状态
    AutoState_t auto_state;
    ManualState_t man_state;

    // 输出状态（供输出层使用）
    bool is_running_supply;   // 水泵状态
    bool is_running_drain;    // 排水阀状态
    
    // 用户请求（由按键产生，状态机消费后清零）
    bool req_start;   // 请求开始（上水启动 / 排水启动）
    bool req_stop;    // 请求停止（上水停止 / 排水停止）

    // 报警状态
    bool alarm_low_level;     // 低水位
    bool alarm_overload;      // 过载 > 60kg
} SystemState_t;// 系统状态结构体

extern SystemState_t g_sys;
extern volatile uint8_t g_blink_on;

// 初始化系统状态
void SystemState_Init(void);
 
