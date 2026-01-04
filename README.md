# 自动加水控制器（下位机程序）

这是 **华北理工大学 测控技术与仪器专业 生产实习** 中  
“自动加水控制器” 的 **下位机程序实现**。

主要用于：
- 跑得起来
- 结构清楚
- 方便同学参考和理解

---

## 用到的东西

- MCU：STM32F103 系列  
- 开发环境：Keil uVision5（ARMCC 5）
- 配置工具：STM32CubeMX
- 调试方式：SWD（J-Link / ST-Link）

---

## 主要功能（简要）

- 采集重量/水量相关信号（TM7705）
- 自动 / 手动加水控制逻辑
- 按键输入
- 数码管显示
- RS485 通信

---

## 工程结构说明

```text
├── Core
│   ├── Src / Inc         // HAL、启动文件、中断
│
├── Hardware
│   ├── bsp_xxx.c/.h      // 硬件驱动（显示、按键、ADC、通信等）
│
├── Application
│   ├── app_xxx.c/.h      // 系统状态、控制逻辑
│   ├── ctrl_xxx.c/.h     // 自动/手动控制策略
│
├── Middlewares
│   └── FreeRTOS          // 已集成，但当前未启用调度器

简单理解就是：

Hardware：只管“怎么驱硬件”

Application：只管“系统在干什么”

关于 FreeRTOS（重要说明）

工程中 包含 FreeRTOS 文件，
但 当前版本未启动调度器（osKernelStart / vTaskStartScheduler 已注释）。

原因：

裸机逻辑已满足本次实习需求

开启 RTOS 后需要额外处理中断优先级、栈、堆等问题

如果你要用 RTOS：

需要重新检查中断优先级配置

确保在 ISR 中只调用 FromISR 接口

编译 / 下载注意事项

调试接口请使用 SWD

下载后如果板子没有立即运行：

按一次复位

或断电重启（这是正常现象）

本工程仅用于 学习与交流，
不保证适配所有硬件环境。

如果你只是想：

看结构

抄思路

对照自己工程

直接从 Application 和 Hardware 两层看起就够了。
