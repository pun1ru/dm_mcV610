# Monitor 任务心跳重构验证（2026-11-25）

分支：`originModel`

## 修改目标

- `Monitor_Init` 统一注册任务名称、`TaskHandle_t` 地址、周期、超时和事件处理函数。
- 业务任务只持有自己的 `MonitorHeartbeat_t` 令牌并上报心跳。
- Monitor 内部统一处理超时判断和 Lost/Recovered 事件。
- 蜂鸣器策略从 Monitor 核心移到 App 层 `monitor_actions.c`。

## 构建

使用 CMSIS Toolbox 命令行完整重建：

```powershell
$env:AC6_TOOLCHAIN_6_22_0 = "C:\Keil_v5\ARM\ARMCLANG\bin"
$env:CMSIS_PACK_ROOT = "C:\Keil_v5\ARM\PACK"
cbuild .\dm_mcV610.csolution.yml -r -j 4
```

结果：`1 succeeded, 0 failed`。

本次同时把 LibSE `Algorithm/Drivers/Module/minimal_state` 下的通用源码加入
DM610 CMSIS 工程。当前板级 WS2812 使用 SPI6 实现，未编入依赖未配置 TIM2 的
旧版 PWM 灯带实现。

## pyOCD 烧录

```powershell
$env:PYOCD_USB_BACKEND = "pyusb"
pyocd flash --probe ATK --target stm32h723xx --frequency 50k `
  --connect under-reset --erase chip -O cmsis_dap.limit_packets=true `
  .\out\dm_mcV610\dm_mcV610\dm_mcV610.axf
```

烧录成功，共写入 72,704 字节。

加入全部 LibSE 源码后的镜像重新烧录成功，复位后 CAN/UART 心跳均为在线。

## 注册信息

从目标内存读取的配置表确认：

- CAN TX：周期 1 ms，超时 20 ms，任务句柄有效。
- UART TX：周期 2 ms，超时 20 ms，任务句柄有效。
- 两个任务均注册 `MonitorAction_OnTaskEvent`。
- 两个 heartbeat token 都指向 `systemMonitor`。

## 正常运行

复位运行后读取状态：

| 任务 | online | offline_count |
|---|---:|---:|
| CAN TX | 1 | 0 |
| UART TX | 1 | 0 |

## 故障注入与恢复

临时清空 UART heartbeat token 的 monitor 指针，使 UART 任务停止上报心跳。

失联阶段观测到：

- `UART TX online = 0`
- `offline_count` 持续增长
- `alert_active = 1`
- `alert_deadline` 被事件处理函数更新

随后恢复 UART heartbeat token：

- `UART TX online = 1`
- 新心跳恢复
- `alert_active = 0`

对 CAN heartbeat token 执行相同注入和恢复，也观测到 `online: 1 -> 0 -> 1`。

将全部 LibSE 源码加入构建后再次执行 UART 故障注入：

- 失联阶段：`online = 0`、`alert_active = 1`
- 恢复阶段：`online = 1`、`alert_active = 0`

## 结论

- 注册表、任务句柄、heartbeat token 和事件回调均正常运行。
- 任务超时能够触发 `MONITOR_EVENT_LOST`。
- heartbeat 恢复后能够触发 `MONITOR_EVENT_RECOVERED`。
- 默认蜂鸣器处理函数已从 Monitor 核心解耦到 App 层。
- 测试结束后两个 token 均已恢复，CAN/UART 任务均回到在线状态。
