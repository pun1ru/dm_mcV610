# 通信硬件联调记录（2026-11-25）

测试分支：`originModelTest`

## 测试连接

- CAN1、CAN2、CAN3 的 CAN_H/CAN_L 并联到同一 1 Mbps 总线。
- 总线上接入 CAN 分析仪，实际数据端口为 `COM17`。
- 无线调试器串口先接 USART10，后改接 UART7，实际无线端口为 `COM8`。
- USB_OTG_HS 的 CDC 设备连接到电脑。
- 烧录器为 ATK-HS-V3-CMSIS-DAP，使用 pyOCD 烧录。

## 测试固件

- CAN1、CAN2、CAN3 分别每 100 ms 发送：
  - CAN1：`0x111`
  - CAN2：`0x222`
  - CAN3：`0x333`
- 每个控制器接收另外两路的帧并累计计数。
- CAN 负载测试由 CAN1 发送 `0x100`，8 字节标准帧。
- 压测持续 5 秒。
- UART7 每秒发送 CAN 测试统计，并记录接收字节、事件次数和最后接收内容。
- USB CDC 每秒发送 `DM610 USB CDC TEST`，并对接收数据回显。

## CAN 互通结果

低负载阶段，三路各以 100 ms 周期互发：

| 计数项 | 10 秒增量 |
|---|---:|
| CAN2 发送成功 | 119 |
| CAN3 发送成功 | 119 |
| CAN1 收到 CAN2/CAN3 | 240 |
| CAN2/CAN3 新增发送失败 | 0 |

结论：CAN1、CAN2、CAN3 双向互通正常。CAN1 的接收数量等于 CAN2、CAN3 发送数量之和。

## CAN 负载结果

### 8,000 帧/秒阶段

| 指标 | 结果 |
|---|---:|
| 5 秒发送请求 | 40,000 帧 |
| CAN1 硬件接受增量 | 40,087 帧 |
| 硬件入队失败增量 | 0 |
| CAN2 收到的负载帧 | 40,000 帧 |
| CAN3 收到的负载帧 | 40,000 帧 |
| 名义总线负载 | 约 88.8% |

### 饱和压测阶段

发送侧每秒最多提交 16,000 帧请求：

| 指标 | 结果 |
|---|---:|
| 5 秒发送请求 | 80,000 帧 |
| CAN1 实际接受 | 42,322 帧 |
| CAN1 实际接受速率 | 约 8,464 帧/秒 |
| 硬件入队失败 | 37,725 帧 |
| CAN2 收到 | 42,275 帧 |
| CAN3 收到 | 42,275 帧 |
| 按 111 bit/帧估算负载 | 约 94.0% |
| 加入 3 bit 帧间隔估算 | 约 96.5% |

结论：

- 此硬件和当前驱动在 1 Mbps、标准帧、8 字节数据下的稳定发送能力约为 **8,450～8,470 帧/秒**。
- 总线进入饱和后，继续提高软件请求速率只会增加硬件 TX FIFO 入队失败，不会增加总线吞吐。
- 饱和测试期间，CAN2、CAN3 每路约有 18 个低频互通帧因硬件 FIFO 满而发送失败，说明高负载下普通 FIFO 没有业务优先级。
- CAN2、CAN3 的 RX FIFO message-lost 计数为 0，未观察到接收 FIFO 溢出。

COM17 CAN 分析仪使用二进制逐帧封装。原始流中可以识别 `AA 11 08 <ID> ... A5 55` 格式的 CAN 记录，但缺少厂商上位机协议说明，不能仅靠原始串口准确计算 UI 中的总线占用百分比。因此饱和吞吐以 MCU 内 `tx_ok`、CAN2/CAN3 接收计数和 TX FIFO 失败计数为准，分析仪流用于辅助确认总线确实存在活动。

## UART7 无线串口

- 无线串口端口：`COM8`。
- 参数：`921600 8N1`。
- UART7 发送方向持续收到 ASCII 统计：

```text
CAN_FPS=10 LOAD=1 R1=1002 R2=970 R3=970 L2=41100 L3=41103 QF=0 ACT=0
```

- 固件内 `g_uart7_tx_status` 为 `HAL_OK`。
- UART7 接收方向从 COM8 写入：

```text
UART7_RX_TEST_12345
SECOND_UART7_RX
```

- 接收计数：
  - `g_uart7_rx_events = 2`
  - `g_uart7_rx_bytes = 38`
  - `g_uart7_last_rx_size = 17`
- 最后接收缓冲区保存的 ASCII 为 `SECOND_UART7_RX\r`，与发送内容一致。

结论：UART7 的发送和接收双向通信均正常。

## USB CDC

- 设备成功枚举为 STM32 USB CDC：
  - VID：`0x0483`
  - PID：`0x5740`
  - Windows 端口：`COM6`
- 电脑持续收到：

```text
DM610 USB CDC TEST
```

- 从 COM6 发送：

```text
ECHO_TEST_12345
```

- 成功收到相同回显，同时继续收到周期测试消息。

结论：USB CDC 双向通信正常。

## 烧录说明

本次 CMSIS-DAP 在较高 SWD 时钟下偶发 `DAP_Info` 超时。可靠烧录参数为：

```powershell
$env:PYOCD_USB_BACKEND = "pyusb"
pyocd flash --probe ATK --target stm32h723xx --frequency 50k `
  --connect under-reset --erase chip -O cmsis_dap.limit_packets=true `
  .\out\dm_mcV610\dm_mcV610\dm_mcV610.axf
```

烧录后建议使用普通 attach reset，而不是 under-reset，避免 USB 设备未重新枚举：

```powershell
pyocd cmd --probe ATK --target stm32h723xx --frequency 50k `
  --connect attach -O cmsis_dap.limit_packets=true -c "reset" -c "go"
```

## 已知问题

- CAN 饱和时会丢低频业务帧，需要增加发送优先级、同 ID 最新值合并或限流策略。
- USB CDC 可以正常枚举和双向通信，但复位方式会影响枚举，仍需继续验证板级 USB core reset 稳定性。
- UART10 的早期测试只确认过无线链路有数据，当前默认无线调试口已改为 UART7。
