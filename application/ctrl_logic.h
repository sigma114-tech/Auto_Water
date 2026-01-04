#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "app_state.h"

void Control_Run(void);                 // 整个控制系统主逻辑
void clamp_set_value(void);             // 水位设定值的限制
void Control_UpdateProcessValue(void);  // 更新 process_value
void Control_HandleKeys(void);          // 处理按键事件，更新 g_sys

void Control_AutoMode(void);            // 自动模式状态机
void Control_ManualMode(void);          // 手动模式状态机

void Control_Safety(void);              // 过载/低水位 等安全检查
void Control_UpdateOutputs(void);       // 根据 g_sys 刷新继电器输出
void Control_UpdateLEDs(void);          // 根据 g_sys 刷新指示灯

bool Control_IsFillComplete(uint16_t pv, uint16_t sv);  // 判断给定条件下是否停止加水
bool is_system_running(void);                           // 判断系统当前是否处于运行态
Speed_t Speed_AutoAdjust(uint16_t pv, uint16_t sv);     // 自动调节给定条件下水位的加水速度
