# DM610 通用通信模板

分支：`originModel`

本模板只保留板级基础能力，不创建机械臂、电机、云台、Host 或供弹机构等具体业务实例。

任务心跳架构及硬件验证记录见 `Docs/monitor-task-heartbeat-validation.md`。

## 已包含

- FDCAN1/2/3 实例注册、接收中断分发、发送队列和自动恢复。
- UART5/7、USART1/2/3/10、UART8/9 实例注册和 DMA/中断收发。
- UART5 默认绑定 DT7 遥控器接收。
- USART1 默认绑定裁判系统接收。
- LibSE 的 Algorithm、Drivers、Module 和 MinimalState 源码全部加入 CMSIS 构建。
- 模板不创建电机、机械臂、云台、Host 或供弹机构等具体业务实例。
- Monitor 模块初始化、任务心跳注册及 CAN 监测量轮询。
- MonitorTask、CanTransmitTask、UartTransmitTask。

当前 DM610 板载 WS2812 使用 SPI6 和 `ws2812_board.c`。旧版 TIM2 PWM
灯带实现依赖当前 CubeMX 未配置的 `htim2`，因此在模板构建中不编入。

## 初始化顺序

`Base_Init -> Ipc_Init -> Comm_Register -> Module_Init -> Board_TaskCreate -> Comm_Start`

应用模块应在 `Module_Init` 中创建实例，在协议层通过 `CAN_BindRx`、`UART_BindRx`、`UART_BindTx` 绑定回调。

## 任务心跳监控

监控任务由 `Monitor_Init` 统一注册以下信息：

- 任务名称和 `TaskHandle_t` 地址
- 正常执行周期
- 心跳超时时间
- 丢失和恢复事件处理函数及上下文
- 任务持有的 `MonitorHeartbeat_t` 令牌

业务任务不直接访问 `systemMonitor`，也不决定 LED 或蜂鸣器策略。每轮业务完成后只调用：

```c
Monitor_Heartbeat(can_tx_heartbeat);
```

Monitor 每 2 ms 收集心跳，每 100 ms 判断一次在线状态。任务超过注册的 `timeout_ms` 没有心跳时触发 `MONITOR_EVENT_LOST`，恢复后触发 `MONITOR_EVENT_RECOVERED`。默认报警策略位于 `App/monitor_actions.c`。

## 硬件映射

| 接口 | 引脚 | 用途 |
|---|---|---|
| FDCAN1 | PD0/PD1 | 1 Mbps Classic CAN |
| FDCAN2 | PB5/PB6 | 1 Mbps Classic CAN |
| FDCAN3 | PD12/PD13 | 1 Mbps Classic CAN |
| UART5 | PD2 | DT7 DBUS，100 kbps，9E2，仅接收 |
| USART1 | PA9/PA10 | 裁判系统，115200 8N1 |
| UART8 | PE0/PE1 | 921600 8N1，RX DMA + TX 中断 |
| UART9 | PD14/PD15 | 921600 8N1，RX DMA + TX 中断 |

## 已知问题

- USB CDC 暂未启用。板级 USB OTG HS core reset 会卡在 `GRSTCTL.CSRST`，加入 USB 通信前必须先复核该硬件问题。
- 当前没有 IWDG，只有任务心跳、CAN 恢复和 UART 自动重挂。
- CAN 发送队列满时仍可能丢帧，业务层必须检查入队结果并定义控制帧的优先级或最新值策略。
- CAN 驱动目前只支持标准 ID、Classic CAN 和最多 8 字节数据。
- UART 自动重试恢复通信通道，但不会恢复已经丢失或发送超时的业务数据。
