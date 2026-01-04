#include "bsp_rs485.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"

// 配置大小
#define RS485_RX_DMA_BUF_SIZE   128
#define RS485_RB_SIZE           256   // 环形缓冲，够用就行

// 硬件句柄
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

// DMA接收缓冲
static uint8_t s_rx_dma_buf[RS485_RX_DMA_BUF_SIZE];
static volatile uint16_t s_last_pos = 0;

// 环形缓冲
static uint8_t  s_rb[RS485_RB_SIZE];
static volatile uint16_t s_rb_w = 0;
static volatile uint16_t s_rb_r = 0;

static volatile bool s_frame_flag = false;

// 方向控制
static inline void rs485_set_tx(void)
{
    HAL_GPIO_WritePin(USART3_485_RE_DE_GPIO_Port, USART3_485_RE_DE_Pin, GPIO_PIN_SET);
}
static inline void rs485_set_rx(void)
{
    HAL_GPIO_WritePin(USART3_485_RE_DE_GPIO_Port, USART3_485_RE_DE_Pin, GPIO_PIN_RESET);
}

// 环形缓冲写入
static void rb_push(const uint8_t *p, uint16_t n)
{
    for (uint16_t i = 0; i < n; i++)
    {
        uint16_t next = (uint16_t)((s_rb_w + 1u) % RS485_RB_SIZE);
        if (next == s_rb_r)
        {
            // 满了直接丢
            break;
        }
        s_rb[s_rb_w] = p[i];
        s_rb_w = next;
    }
}

size_t RS485_Read(uint8_t *out, size_t maxlen)
{
    size_t cnt = 0;
    while (cnt < maxlen && s_rb_r != s_rb_w)
    {
        out[cnt++] = s_rb[s_rb_r];
        s_rb_r = (uint16_t)((s_rb_r + 1u) % RS485_RB_SIZE);
    }
    return cnt;
}

bool RS485_FrameAvailable(void) { return s_frame_flag; }
void RS485_ClearFrameFlag(void) { s_frame_flag = false; }

// DMA新数据搬运
static void rs485_rx_check_dma_and_push(void)
{
    // DMA写入位置 = BUF_SIZE - NDTR
    uint16_t pos = (uint16_t)(RS485_RX_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart3.hdmarx));

    if (pos == s_last_pos) return;

    if (pos > s_last_pos)
    {
        rb_push(&s_rx_dma_buf[s_last_pos], (uint16_t)(pos - s_last_pos));
    }
    else
    {
        // 回绕
        rb_push(&s_rx_dma_buf[s_last_pos], (uint16_t)(RS485_RX_DMA_BUF_SIZE - s_last_pos));
        rb_push(&s_rx_dma_buf[0], pos);
    }

    s_last_pos = pos;
    s_frame_flag = true;
}

// 初始化
void RS485_Init(void)
{
    // 设置为接收模式
    rs485_set_rx();

    // 启动 DMA 接收
    s_last_pos = 0;
    s_frame_flag = false;

    HAL_UART_Receive_DMA(&huart3, s_rx_dma_buf, RS485_RX_DMA_BUF_SIZE);

    // 使能 IDLE 中断（作为帧结束提示）
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
}

// 发送,先阻塞跑通
bool RS485_SendBlocking(const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    if (data == NULL || len == 0) return false;

    rs485_set_tx();

    if (HAL_UART_Transmit(&huart3, (uint8_t *)data, (uint16_t)len, timeout_ms) != HAL_OK)
    {
        rs485_set_rx();
        return false;
    }

    // 确保最后一个字节完全发完
    while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) { }
    __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_TC);

    rs485_set_rx();
    return true;
}

// ISR Hook
void RS485_OnUartIrq(void)
{
    // IDLE：认为“帧间隔到来”，把DMA新数据推进 ring buffer
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);

        rs485_rx_check_dma_and_push();
    }
}
