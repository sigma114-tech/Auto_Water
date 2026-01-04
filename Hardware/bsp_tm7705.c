#include "bsp_tm7705.h"
#include "gpio.h"
#include "spi.h"
#include "stm32f1xx_hal_spi.h"
#include <stdbool.h>

static volatile uint8_t s_drdy_flag = 0;// DRDY 标志位

void TM7705_DrdyIRQ(void) // 由 EXTI 回调中调用
{     
    s_drdy_flag = 1;
}

static uint8_t spi_txrx(uint8_t tx)// SPI读写一个字节
{
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

static void soft_reset(void)
{
    uint8_t data = 0xFF;
    for (int i = 0; i < 4; i++)
    {
        spi_txrx(data);
    }
}

static void write_comm(uint8_t v)// 写通信寄存器
{
    spi_txrx(v);
}

static void write_setup(uint8_t v)// 写设置寄存器
{
    write_comm(0x10);
    spi_txrx(v);
}

static void write_clock(uint8_t v)// 写时钟寄存器
{
    write_comm(0x20);
    spi_txrx(v);
}

static inline bool drdy_is_low(void) // 读取标志位
{
    return HAL_GPIO_ReadPin(SPI2_DRDY_GPIO_Port, SPI2_DRDY_Pin) == GPIO_PIN_RESET;
}

bool TM7705_WaitReady(uint32_t timeout_ms) // 判断等待是否就绪
{
    uint32_t t0 = HAL_GetTick();
    if (timeout_ms == 0) timeout_ms = 0xFFFFFFFF;  // 0表示一直等
    s_drdy_flag = s_drdy_flag && drdy_is_low() ? 0 : s_drdy_flag;// 清标志位残留
    while (!drdy_is_low()) {
        if (s_drdy_flag) {// 若已触发中断则结束轮询
            s_drdy_flag = 0;
            break;
        }
        if ((HAL_GetTick() - t0) >= timeout_ms) return false;
    }
    return true;
}

void TM7705_Init(void) 
{
    soft_reset();
    HAL_Delay(1);
    
    write_clock(0x0C);// 时钟：50 Hz 输出率
    
    write_setup(0x56);// AIN1，单极性，BUF=ON，增益=4，并触发自校准

    TM7705_WaitReady(500);// 等自校准完成
}

bool TM7705_ReadCode(uint16_t* out_code) 
{
    if (!out_code) return false;// 参数检查

    write_comm(0x38);// 下次访问数据寄存器（通道1，读）
    if (!TM7705_WaitReady(100)) return false;

    uint8_t msb = spi_txrx(0x00);
    uint8_t lsb = spi_txrx(0x00);
    *out_code = ((uint16_t)msb << 8) | lsb;// 返回值
    return true;// 返回状态
}
