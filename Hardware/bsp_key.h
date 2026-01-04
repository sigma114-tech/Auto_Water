#pragma once
#include "main.h"
#include <stdbool.h>

typedef enum 
{
    Key_Mode_Simulation,Key_Mode_AutoHand,
    Key_Adjust_Up,Key_Adjust_Down,
    Key_Supply_ON,Key_Supply_OFF,
    Key_Drainage_ON,Key_Drainage_OFF,
    Key_Manual_SetWay,Key_Manual_SetSpeed,Key_Manual_SetDrainDir,
}Key_t;

typedef enum {
    KEY_ACTION_SIMPLE,   // 瞬时按键
    KEY_ACTION_TOGGLE,   // 自锁按键
    KEY_ACTION_REPEAT    // 连续动作按键
} KeyAction_t;

typedef enum {
    KEY_EVENT_NONE = 0,     // 无事件
    KEY_EVENT_PRESS,        // 按下事件
    KEY_EVENT_RELEASE,      // 松开事件
    KEY_EVENT_REPEAT        // 长按后重复触发
} KeyEvent_t;

void Key_Init(void);
void Key_ScanTick(void);
bool Key_ON(Key_t key);
bool Key_WasPressed(Key_t key);
bool Key_WasReleased(Key_t key);
bool Key_WasRepeated(Key_t key);
