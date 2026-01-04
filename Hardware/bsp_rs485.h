#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 初始化
void RS485_Init(void);

// 阻塞发送
// 内部会：切到发送->发完等TC->切回接收
bool RS485_SendBlocking(const uint8_t *data, size_t len, uint32_t timeout_ms);

// 从接收缓冲取出字节（非阻塞）
size_t RS485_Read(uint8_t *out, size_t maxlen);

// 是否检测到“帧结束”（IDLE触发）
bool RS485_FrameAvailable(void);
void RS485_ClearFrameFlag(void);

// ===== ISR Hook：在 USART3_IRQHandler 的 USER CODE 0 里调用 =====
void RS485_OnUartIrq(void);
