# J6（0x146）收发一帧不回：FDCAN 关闭了自动重发

- 日期：2026-10-28
- 分支：`7axisarm`
- 固件：`MDK-ARM/out/dm_mcV610/dm_mcV610/dm_mcV610.axf`（cbuild 流程产出）
- 相关文档：[USB核复位卡死导致整机死机-排查记录.md](USB核复位卡死导致整机死机-排查记录.md)（另一个独立问题，已修复）

---

## 1. 现象

- J4（MF4005，`0x142`，CAN2）收发正常，1:1 回包。
- **J5（MF4015，`0x141`）从来没有回过一帧**，反馈状态一直是 `MOTOR_NO_FEEDBACK`。
- **J6（MF4015，`0x146`）只回过极少数帧，之后就再也不回**：
  - `canDebug` 显示对 `0x146` 发过 13453 帧，`hw_fail = 0`、`enqueue_fail = 0`；
  - 收到的回包 = 0；
  - `armJ6Motor.feedback.timestamp`（收到反馈帧的计数）冻结在很小的数值上不再增长。

一开始怀疑的方向（TX FIFO 满、总线负载、ID 冲突、过滤器、接收 FIFO 溢出、中断栈、任务栈、电机型号参数）**全部被实测排除**，见第 4 节。

---

## 2. 关键观测手段

`LibSE/BSP/CAN/CAN_driver.c` 里的 `canDebug` 逐 ID 统计表（`#define CAN_DEBUG_TRACE 1`）是这次的主要工具，每个 ID 记：

```
enqueue_req  请求入队次数
enqueue_fail 队列满被丢
hw_ok        成功写进 FDCAN TX FIFO
hw_fail      HAL 拒绝
rx_count     中断里收到的帧数
last_tx_tick / last_rx_tick
```

配套的 `CanDebug_ResetIds()` / `CanDebug_RxCount(id)` 用来做「发 N 帧、数回包」的对照实验。

用调试器直接读内存（独立 OpenOCD，不 reset，保留 RAM）：

```
openocd -f interface/cmsis-dap.cfg -f target/stm32h7x.cfg \
        -c "adapter speed 500" -c "init" -c "halt" \
        -c "mdw <addr> <count>" -c "resume" -c "shutdown"
```

符号地址用 `fromelf --text -s ...axf` 取，**注意每次改代码 `.bss` 会移位，地址要重新取**。

---

## 3. 决定性实验：把变量一个个减掉

思路：`ArmTask` 每 2 ms 调用 `Arm_SendZeroCommand()`，对七个关节各发一帧零指令。
先让臂停发（腾空 CAN2），再由测试代码自己发帧，把「帧内容 / 速率 / 总线流量 / 下发对象」逐个剥离。

### 3.1 臂停发，自己按不同速率发 0x9A / 0xA1

| 测试 | 发出 | J6 回包 |
|---|---|---|
| 100 ms × 10 | 10 | **10** |
| 10 ms × 50 | 50 | **50** |
| **2 ms × 100**（与臂同速） | 100 | **100** |
| 10 ms × 50（0xA1 力矩指令） | 50 | **50** |
| 2 ms × 500（1 秒） | 500 | **500** |
| 对照：0x142 10 ms × 50（J4） | 50 | 50 |
| 对照：0x141 10 ms × 50（J5） | 50 | **0** |

**结论：电机本身完全正常，帧内容、速率、句柄、总线都没问题。J5 是另一回事（见第 7 节）。**

### 3.2 臂正常跑，只改「下发哪几个关节」

`Arm_SendZeroCommand` 上加了一个临时掩码 `g_arm_joint_mask`，bit i 为 0 的关节跳过。窗口 2 ms 周期、1 秒。

| 掩码 | 下发内容 | CAN2 指令帧 | J6 回包 |
|---|---|---|---|
| 0x40 | 只有 J6 | 1 帧 / 2 ms | **500** |
| 0x50 | J4 + J6 | 2 帧 / 2 ms | **500** |
| 0x4F | J0~J3(CAN1) + J6 | 2 帧 / 2 ms | **500** |
| 0x60 | J5 + J6 | 2 帧 / 2 ms | **500** |
| 0x5F | J0~J4 + J6 | 2 帧 / 2 ms | **500** |
| **0x70** | **J4 + J5 + J6** | **3 帧 / 2 ms** | **0** |
| **0x7F** | **全部** | **3 帧 / 2 ms** | **0** |

**发现一：失败的都是「CAN2 上同时下发 3 个关节」的组合。**

### 3.3 排除「速率」

改臂控周期，全部下发，同时数 J6 和 J4（J4 是「臂确实在按这个周期发」的活基准）：

| 周期 | J6 (0x146) | J4 (0x142) |
|---|---|---|
| 2 ms | **0** | 500 |
| 10 ms | **0** | 100 |
| 20 ms | **0** | 50 |

J4 的回包数随周期精确变化，说明臂和测量都正常；而 J6 在 20 ms 下照样一帧不回。

**结论：不是速率、不是负载。**

### 3.4 找出到底是哪一帧在捣鬼

臂只下发 J4+J6（这组已知全回），测试代码再用 2 ms 插一条背景帧：

| 背景帧 | J6 回包 |
|---|---|
| 不加 | 550 |
| `0x143`（总线上没有这个电机） | 550 |
| `0x141`（J5） | 550 |
| **`0x142`（J4）** | **50** |

**发现二：只要额外加一条 `0x142`，J6 立刻哑掉。加没有电机应答的 `0x143` 毫无影响。**

也就是说，**真正压死 J6 的是 J4 的「回包」，不是任何一条指令帧。**

---

## 4. 被排除的假设

| 假设 | 实测数据 | 结论 |
|---|---|---|
| 发送失败 / 队列满 | `enqueue_fail_total = 0`、`hw_fail_total = 0`、`tx_ok` 与请求数一致 | ❌ |
| FDCAN TX FIFO 满 | `tx_fifo_free = 29/32`、`min_fifo_free = 29`、`tx_fifo_full_count = 0` | ❌ |
| 接收 FIFO 溢出 | `rx_fifo1_capacity = 32`、`peak_fill = 0`、`fifo_full_events = 0` | ❌ |
| 总线错误 | `FDCAN2->ECR = 0`（TX/RX 错误计数都是 0）、`PSR.ACT = 1`（error active） | ❌ |
| 被配成回环自收 | `CCCR.TEST = 0`、`TEST.LBCK(bit4) = 0` | ❌ |
| 接收绑定漏了 J6 | CAN2 绑定表：`0x142 / 0x141 / 0x146` 三条都在，同一个 handler | ❌ |
| 电机句柄错了 | `armJ6Motor.base.can = 0x24002bf0`，与 J4/J5 **同一个** `hfdcan2` | ❌ |
| 电机 ID / 模式配置错 | `model = LK_MF4015`、`motor_id = 6`、`feedback_id = 0x146`、`control_mode = MODE_TORQUE` | ❌ |
| 帧内容不对 | `LKMotor_SendCommand` 产出 `{0xA1,0,0,0,0,0,0,0}`，与手工发的帧逐字节相同 | ❌ |
| 刷新率太快 | 20 ms 周期下 J6 仍然 0 回包 | ❌ |
| 任务卡死 / 整机停摆 | `g_crash` 全 0、`uwTick` 持续递增 | ❌ |

---

## 5. 根因

### 5.1 直接原因

`Core/Src/fdcan.c` 里 CubeMX 生成的默认配置是：

```c
hfdcan2.Init.AutoRetransmission = DISABLE;      /* → CCCR.DAR = 1 */
```

读寄存器确认：

```
FDCAN2->CCCR = 0x00001040      /* bit6 DAR = 1，自动重发关闭 */
```

**`CCCR.DAR = 1` 时，M_CAN 在「丢失仲裁」或「出错」后不会重发，这一帧直接被静默丢弃。**
关键点在于：**这种丢弃不产生任何错误中断，TX 错误计数不涨，`ECR` 保持 0，`HAL_FDCAN_AddMessageToTxFifoQ` 也已经返回过 OK**——所以从软件侧看，发送「完全成功」。

### 5.2 为什么偏偏是 J6

LK（MyActuator）协议的约定是 **指令 ID 和回包 ID 相同**，都是 `0x140 + motor_id`：

| 关节 | 指令 ID | 回包 ID |
|---|---|---|
| J4 | `0x142` | `0x142` |
| J5 | `0x141` | `0x141` |
| J6 | `0x146` | `0x146` |

CAN 仲裁是「ID 小的赢」。板子每 2 ms 连续发 `0x142`、`0x141`、`0x146` 三帧，J4 收到 `0x142` 后几乎立刻回一帧 `0x142`：

```
板子:  ... 0x142 | 0x141 | 0x146 ............... 0x142 | 0x141 | 0x146 ...
J4  :                    ← 回 0x142 ──┐
                                      ↓
                    板子的 0x146 和 J4 的 0x142 撞在同一时刻
                    0x142 < 0x146 → J4 赢 → 板子的 0x146 输掉仲裁
                    DAR = 1 → 0x146 不重发，直接丢弃
                    → J6 永远收不到指令 → 永远不回包
```

而 `0x142` 自己是总线上 ID 最小的（`0x141` 上没人应答），它**永远赢仲裁**，所以 J4 一切正常。

这也解释了所有实验现象：

| 现象 | 解释 |
|---|---|
| 只有 J6 不回，J4 正常 | `0x142` 是最小 ID，永远赢；`0x146` 永远输 |
| 只发 J6 时全回 | 没有 `0x142` 回包来抢 |
| 加一条 `0x142` 就死 | 多出来的 J4 回包正好撞上 `0x146` |
| 加 `0x141` / `0x143` 没事 | 这两个 ID 上没有电机回包 |
| 降周期（20 ms）也没用 | 周期变长，碰撞只是变稀疏，仍然每次必撞 |
| `ECR`、`hw_fail` 全是 0 | 仲裁丢失不算错误，不计数 |

---

## 6. 修改

只改一处，写在 CubeMX 的 **USER CODE 区**（重新生成不会被覆盖）：

`Core/Src/fdcan.c` —— `MX_FDCAN1/2/3_Init()` 的 `USER CODE BEGIN FDCANx_Init 2` 里各加一行：

```c
/* 打开自动重发（CCCR.DAR=0） */
CLEAR_BIT(FDCAN1->CCCR, FDCAN_CCCR_DAR);
CLEAR_BIT(FDCAN2->CCCR, FDCAN_CCCR_DAR);
CLEAR_BIT(FDCAN3->CCCR, FDCAN_CCCR_DAR);
```

打开后，丢仲裁的帧会一直重试到发出去（CAN 控制器本来就应该这样工作），问题消失。

**建议同时在 CubeMX 的 `.ioc` 里把三个 FDCAN 的 `Auto Retransmission` 勾上**，这样生成的代码本身就是对的，两处保持一致。

> 注意：`Start/Retransmit` 只影响「输掉仲裁/出错」的帧，不会影响正常发送，也不会引入额外延迟风险——总线上的节点都会正常 ACK。

---

## 7. 验证

改完后重新编译、烧录（同一份干净固件，临时调试代码已全部移除），运行约 10 秒后读内存：

| 项 | 修前 | 修后 |
|---|---|---|
| `FDCAN2->CCCR` | `0x00001040`（DAR=1） | **`0x00001000`（DAR=0）** |
| J6 回包（1 秒窗口，全部关节 2 ms） | 0 | **500** |
| J6 回包（第 2 秒） | 0 | **500**（持续，不是只回一阵） |
| `armJ6Motor.feedback.timestamp` | 冻结（400 左右） | 与 J4 **完全相同**（各 4574 帧，1:1） |
| `armJ4Motor.feedback.timestamp` | 正常 | 正常，未受影响 |

七个关节的反馈计数（运行约 30 秒）：

| 关节 | 电机 | 反馈帧数 | 状态 |
|---|---|---|---|
| J0 | GM6020 | 32109 | ✅ |
| J1 | DM4310 | 16047 | ✅ |
| J2 | GM6020 | 32109 | ✅ |
| J3 | DM4310 | 16046 | ✅ |
| J4 | MF4005 | 16047 | ✅ |
| J5 | MF4015 | **0** | ❌ `MOTOR_NO_FEEDBACK` |
| J6 | MF4015 | **16046** | ✅ |

---

## 8. 遗留问题

### 8.1 J5 是硬件问题，与本 bug 无关

J5（`0x141`）一帧都没回过。修好自动重发后重新扫描了 **CAN1 / CAN2 / CAN3 三条总线上的 `0x141~0x14A`**，每条 ID 连发 3 帧「读错误状态(`0x9A`)」：

```
CAN2 0x142 (J4) → 3/3    ✅
CAN2 0x146 (J6) → 3/3    ✅
其余全部        → 0
```

**总线上只有 J4 和 J6 两个 LK 电机应答，`0x141` 没有任何节点。** 需要现场检查 J5：

- 是否上电（MF4015 的电源指示灯）
- CAN_H / CAN_L 是否接上、终端电阻是否正常
- 电机拨码 ID 是否真的设成了 `1`（若设成了别的值，用上面的扫描能扫出来——但现在全范围都没有，更像没上电/没接线）

### 8.2 临时调试代码待清理

| 位置 | 内容 | 建议 |
|---|---|---|
| `LibSE/BSP/CAN/CAN_driver.c` | `#define CAN_DEBUG_TRACE 1` 及整套 `canDebug` / `CanDebug_*` | 确认完可以关掉，减少中断里的开销 |
| `LibSE/BSP/CAN/CAN_driver.h` | `g_isr_msp_min`、`CAN_ISR_STACK_SAMPLE()` | 删除 |
| `Core/Src/main.c` | `CrashRecord_t` / `g_crash` 黑匣子 | 建议保留，成本极低 |
| `Core/Src/freertos.c` | `g_stack_overflow_count` / `g_stack_overflow_task` | 建议保留 |
| `MDK-ARM/startup_stm32h723xx.s` | `Stack_Size` 已由 `0x400` 改为 `0x1000` | 建议保留 |

本次排查加的临时变量（`g_arm_hold`、`g_arm_period_ms`、`g_arm_joint_mask`、`g_scanResult`、`CAN_SCAN_DEBUG`）已全部移除。

### 8.3 臂的状态机还没打通

`Arm_SetState()` 定义了但**全项目没有任何调用点**，所以 `armInstance.state` 永远停在 `ARM_PROTECTED`，只会发零力矩指令，`ARM_ENABLE` 分支（发 `0x88` 使能帧）永远走不到。要让关节真正出力，需要先把状态切换的入口补上。

---

## 9. 复现要点

- 编译用 **cbuild**（不是 UV4，两者输出路径不同）：
  ```
  cd MDK-ARM
  cbuild dm_mcV610.csolution.yml --toolchain AC6@6.19.0
  ```
- 符号偏移每次改代码都会变，读内存前先用 `fromelf` 重新取地址：
  ```
  fromelf --text -s MDK-ARM/out/dm_mcV610/dm_mcV610/dm_mcV610.axf | grep <symbol>
  ```
- 烧录（按 ELF 地址，不要跟 `0x08000000` 或 `reset run` 混在一个 `program` 里）：
  ```
  openocd -f interface/cmsis-dap.cfg -f target/stm32h7x.cfg \
          -c "adapter speed 1000" -c "init" -c "halt" \
          -c "program <abs path>/dm_mcV610.axf verify" \
          -c "reset run" -c "shutdown"
  ```
- CMSIS-DAP 偶发 `transfer count mismatch`：重试一次即可；读内存时把 `adapter speed` 降到 500。
- 判断 J6 是否正常，最快的办法是直接读 `armJ6Motor` 的 `feedback.timestamp`（`&armJ6Motor + 0`），它应该和 `armJ4Motor` 同步增长。

---

## 10. 2026-10-31 复发：CAN 自动恢复把 DAR 又打开了

启动时 `USER CODE 2` 里的 `CLEAR_BIT(FDCANx->CCCR, FDCAN_CCCR_DAR)` 仍然有效，但运行中
`CAN_MonitorPoll()` 检测到 bus-off / TX FIFO 卡死 / 控制器不在 BUSY 状态时会调用
`CAN_Recover()`。该函数重新执行 `HAL_FDCAN_Init()`，而 `hfdcanx.Init.AutoRetransmission`
仍为 `DISABLE`，于是 `CCCR.DAR` 被重新置 1，`0x142` 再次压掉 `0x146`，J6 无回包复发。

修复需要同时覆盖三条路径，避免只在冷启动时正确：

1. `dm_mcV610.ioc`：FDCAN1/2/3 的 `AutoRetransmission` 改为 `ENABLE`；
2. `Core/Src/fdcan.c`：生成配置改为 `ENABLE`，并保留 USER CODE 区的清 DAR 作为防御；
3. `LibSE/BSP/CAN/CAN_driver.c`：`CAN_Recover()` 在 `HAL_FDCAN_Init()` 前强制
   `hcan->Init.AutoRetransmission = ENABLE`，保证运行中的自动恢复不会再关闭重发。

验证时不能只看上电后的 `FDCAN2->CCCR`；还应人为制造一次恢复或读 `systemMonitor` 的
`recover_success` / `err_reinit`，确认恢复后的 `CCCR.DAR` 仍为 0，J4 与 J6 的反馈计数继续同步增长。
