#include "app_comm.h"
#include "bsp_rs485.h"
#include "app_state.h"

#include <string.h>
#include <stdbool.h>

#ifndef COMM_DEV_ADDR
#define COMM_DEV_ADDR 0x02   // Form1.cs 默认 DevAddress=0x02
#endif

// 读出来给上位机的“状态块”长度（Form1.cs 轮询 len=9）
#define STATUS_MAP_SIZE  9

// 接收拼帧缓冲（从 ring buffer 里读出来，可能一次多帧）
#define RX_ACC_SIZE      256

// CRC16
static uint16_t crc16_modbus(const uint8_t *dat, uint16_t len)
{
    uint16_t val = 0xFFFF;
    const uint16_t poly = 0xA001;

    for (uint16_t j = 0; j < len; j++)
    {
        val ^= dat[j];
        for (uint8_t i = 0; i < 8; i++)
        {
            if (val & 0x0001) val = (uint16_t)((val >> 1) ^ poly);
            else              val = (uint16_t)(val >> 1);
        }
    }
    return val;
}

// 发送工具追加CRC
static void send_with_crc(const uint8_t *p, uint16_t len_wo_crc)
{
    uint8_t buf[300];
    if (len_wo_crc + 2 > sizeof(buf)) return;

    memcpy(buf, p, len_wo_crc);
    uint16_t crc = crc16_modbus(buf, len_wo_crc);
    buf[len_wo_crc + 0] = (uint8_t)(crc & 0xFF);
    buf[len_wo_crc + 1] = (uint8_t)((crc >> 8) & 0xFF);

    RS485_SendBlocking(buf, len_wo_crc + 2, 50);
}

// 把g_sys打包成Form1.cs期望的9字节
static void BuildStatus9(uint8_t out9[STATUS_MAP_SIZE])
{
    memset(out9, 0, STATUS_MAP_SIZE);

    // DataBuffer[1] 模式：0x0A=自动 0x0B=手动
    out9[1] = (g_sys.mode == MODE_AUTO) ? 0x0A : 0x0B;

    // DataBuffer[2] SV上位机直接显示
    out9[2] = (uint8_t)g_sys.set_value;

    // DataBuffer[4] 动作：0x0A加水 0x0B内排 0x0C外排 0x0D保持
    if (g_sys.is_running_supply)
    {
        out9[4] = 0x0A;
    }
    else if (g_sys.is_running_drain)
    {
        out9[4] = (g_sys.drain_dir == DRAIN_IN) ? 0x0B : 0x0C;
    }
    else
    {
        out9[4] = 0x0D;
    }

    // DataBuffer[5] 水状态：0x0A 有水 / 0x0B 无水
    // 你前面定义“低水位报警=净重<0”时，这里可以用 alarm_low_level 来表示无水
    out9[5] = (g_sys.alarm_low_level) ? 0x0B : 0x0A;

    // DataBuffer[6,7] PV：上位机按 (hi<<8|lo)/100.0 显示
    uint16_t pv_raw = (uint16_t)(g_sys.process_value * 100);
    out9[6] = (uint8_t)(pv_raw >> 8);
    out9[7] = (uint8_t)(pv_raw & 0xFF);

    // DataBuffer[8] 量程：0x10=50kg 0x11=80kg 0x12=100kg
    out9[8] = 0x12;
}

// 写请求映射
static void ApplyWrite(uint16_t addr, const uint8_t *payload, uint16_t len)
{
    if (payload == NULL || len == 0) return;

    // addr=0x0001 len=1: MODE (0x0A auto, 0x0B manual)
    // addr=0x0002 len=1: SV
    // addr=0x0003 len=1: CMD (0x01 start, 0x00 stop)
    switch (addr)
    {
        case 0x0001:
            if (payload[0] == 0x0A && g_sys.mode != MODE_AUTO)
            {
                g_sys.mode = MODE_AUTO;
                g_sys.auto_state = AUTO_IDLE;
                g_sys.req_start = false;
                g_sys.req_stop  = false;
            }
            else if (payload[0] == 0x0B && g_sys.mode != MODE_MANUAL)
            {
                g_sys.mode = MODE_MANUAL;
                g_sys.man_state = MAN_IDLE;
                g_sys.req_start = false;
                g_sys.req_stop  = false;
            }
            break;

        case 0x0002:
        {
            // SV: 0~99
            uint8_t sv = payload[0];
            if (sv > 99) sv = 99;
            g_sys.set_value = sv;
        } break;

        case 0x0003:
            // CMD
            if (payload[0] == 0x01)
            {
                g_sys.req_start = true;
            }
            else if (payload[0] == 0x00)
            {
                g_sys.req_stop = true;
            }
            break;

        default:
            // 未定义地址：先忽略即可
            break;
    }
}

// 处理读请求,按 addr/len 返回子段
static void HandleRead(uint16_t addr, uint16_t len)
{
    uint8_t status[STATUS_MAP_SIZE];
    BuildStatus9(status);

    // 越界保护
    if (addr >= STATUS_MAP_SIZE) len = 0;
    else if (addr + len > STATUS_MAP_SIZE) len = (uint16_t)(STATUS_MAP_SIZE - addr);

    uint8_t resp[3 + STATUS_MAP_SIZE]; // [dev][0x01][byte_count]+data
    resp[0] = COMM_DEV_ADDR;
    resp[1] = 0x01;
    resp[2] = (uint8_t)len;

    for (uint16_t i = 0; i < len; i++)
    {
        resp[3 + i] = status[addr + i];
    }

    send_with_crc(resp, (uint16_t)(3 + len));
}

// 处理写请求并回ACK
static void HandleWrite(uint16_t addr, uint16_t len, const uint8_t *payload)
{
    ApplyWrite(addr, payload, len);

    // 写成功 ACK：让 PC 的 receive_buf[3] == 0x01
    uint8_t ack[4] = { COMM_DEV_ADDR, 0x02, 0x01, 0x01 };
    send_with_crc(ack, 4);
}

// 接收拼帧状态机
static uint8_t  s_acc[RX_ACC_SIZE];
static uint16_t s_acc_len = 0;

static void acc_drop(uint16_t n)
{
    if (n >= s_acc_len) { s_acc_len = 0; return; }
    memmove(s_acc, s_acc + n, s_acc_len - n);
    s_acc_len -= n;
}

void AppComm_Init(void)
{
    // 这里只做协议层初始化
    s_acc_len = 0;
}

void AppComm_Poll(void)
{
    // 如果 BSP 提供了“帧到来提示”，可以用它做粗略唤醒
    if (RS485_FrameAvailable())
    {
        RS485_ClearFrameFlag();
    }

    // 把 ring buffer 里所有字节取出来
    uint8_t tmp[64];
    size_t n = 0;
    do {
        n = RS485_Read(tmp, sizeof(tmp));
        if (n == 0) break;

        // 追加到 accum
        uint16_t room = (uint16_t)(RX_ACC_SIZE - s_acc_len);
        if (n > room)
        {
            // 太多就丢最旧的
            uint16_t drop = (uint16_t)(n - room);
            acc_drop(drop);
        }
        memcpy(s_acc + s_acc_len, tmp, n);
        s_acc_len += (uint16_t)n;

    } while (n > 0);

    // 可能一次 accum 里有多帧
    while (s_acc_len >= 8)
    {
        // 简单重同步：只接受发给我们的地址
        if (s_acc[0] != COMM_DEV_ADDR)
        {
            acc_drop(1);
            continue;
        }

        uint8_t func = s_acc[1];

        if (func == 0x01)
        {
            // 读请求固定 8 字节
            if (s_acc_len < 8) break;

            uint16_t crc_rx = (uint16_t)(s_acc[6] | (s_acc[7] << 8));
            uint16_t crc_cal = crc16_modbus(s_acc, 6);
            if (crc_rx != crc_cal)
            {
                acc_drop(1);
                continue;
            }

            uint16_t addr = (uint16_t)((s_acc[2] << 8) | s_acc[3]);
            uint16_t len  = (uint16_t)((s_acc[4] << 8) | s_acc[5]);

            HandleRead(addr, len);
            acc_drop(8);
            continue;
        }
        else if (func == 0x02)
        {
            // 写请求长度 = 8 + payload_len
            if (s_acc_len < 8) break;

            uint16_t addr = (uint16_t)((s_acc[2] << 8) | s_acc[3]);
            uint16_t len  = (uint16_t)((s_acc[4] << 8) | s_acc[5]);

            uint16_t total = (uint16_t)(8 + len);
            if (total > RX_ACC_SIZE)
            {
                // 不合理，丢一个字节重同步
                acc_drop(1);
                continue;
            }
            if (s_acc_len < total) break;

            uint16_t crc_rx = (uint16_t)(s_acc[6 + len] | (s_acc[7 + len] << 8));
            uint16_t crc_cal = crc16_modbus(s_acc, (uint16_t)(6 + len));
            if (crc_rx != crc_cal)
            {
                acc_drop(1);
                continue;
            }

            const uint8_t *payload = &s_acc[6];
            HandleWrite(addr, len, payload);
            acc_drop(total);
            continue;
        }
        else
        {
            // 未知功能码，丢一个字节
            acc_drop(1);
        }
    }
}
