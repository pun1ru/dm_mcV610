# USB_OTG_HS 核复位卡死，导致整机开机 9.6 秒后停死

- 日期：2026-10-28
- 分支：`7axisarm`
- 固件：`MDK-ARM/out/dm_mcV610/dm_mcV610/dm_mcV610.axf`（cbuild / CMSIS-Toolbox 流程产出，**不是** UV4 的 `MDK-ARM/dm_mcV610/` 那份）

---

## 1. 现象

- 上电后 CAN 正常工作一段时间，然后**所有 CAN 帧在同一瞬间全部消失**（不是逐条丢，是整条总线一起没反应）。
- 关节 J6（MF4015）相关操作期间，现象不规律：

  | 情况 | 表现 |
  |---|---|
  | J6 有接收 | 接收帧数停在 1205 左右，之后所有 CAN 帧死掉 |
  | J6 没接收 | 看起来"正常运行"（实际也在约 9.6 秒后死，只是没人盯着看） |

- 一度怀疑：TX FIFO 满、总线负载高、ID 冲突、收发队列满、中断栈溢出、任务栈溢出。**以上全部被实测排除**（见第 3 节）。

---

## 2. 定位思路

「所有 CAN 帧在**同一时刻**一起停」这个特征，指向的不是 CAN 本身的错误，而是**整个系统停摆**。

能造成整机停摆的只有几处：

1. 进 `HardFault` / `Error_Handler()`（`__disable_irq()` + `while(1)`）—— 会同时冻住 TIM4 时基、FreeRTOS 调度、全部 FDCAN 中断。
2. 关总中断的临界区没退出。
3. 调度器被某个死循环任务饿死。

于是加了一个"黑匣子"：在 `Error_Handler()` 里把现场（返回地址、MSP/PSP、IPSR、tick、当前任务名）存进 `g_crash`，之后用调试器直接读内存。

---

## 3. 排查过程与被排除的假设

### 3.1 黑匣子抓到现场

读 `g_crash`：

```
magic = 0x43524153 ('CRAS')   // 记录有效
hits  = 1                     // 只进过一次
lr    = 0x0800EAAB            // = USBD_LL_Init + 0x42
psp   = 0x24005B40
msp   = 0x2400E520
ipsr  = 0                     // 线程模式（任务上下文），不是中断里
tick  = 9645                  // HAL 时基 9645 ms
tcb   = 0x24005B60
task  = "defaultTask"
```

`lr = 0x0800EAAB` 反汇编对应：

```
0x0800EAA0  BL  HAL_PCD_Init
0x0800EAA4  CBZ r0, ...
0x0800EAA6  BL  Error_Handler      <-- 调进来的
0x0800EAAA  ...                    <-- lr 指向这里（+0x42）
```

**结论：`HAL_PCD_Init()` 返回失败，`USBD_LL_Init()` 里直接调了 `Error_Handler()`。**

三次烧录的 `tick` 分别是 9636 / 9638 / 9645 —— 高度一致。这说明它不是调度上的偶发，而是一段**固定长度**的耗时。

### 3.2 为什么恰好是 9.6 秒

`stm32h7xx_ll_usb.h`：

```c
#define HAL_USB_TIMEOUT   0xF000000U     /* = 251,658,240 */
```

`USB_CoreReset()` 的第二个循环：

```c
USBx->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
do {
  count++;
  if (count > HAL_USB_TIMEOUT) return HAL_TIMEOUT;
} while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
```

硬件永远不清 `CSRST` → 空转 2.5 亿次 → 每次迭代含一次 APB 读 + 自增 + 比较，约 20 周期 @550 MHz ≈ **9.5 秒**。

9.5 秒的忙等 + 之后的 `Error_Handler()`，正好落在实测的 9.6 秒。**时间对上了，链条闭合。**

### 3.3 已排除的假设

| 假设 | 实测数据 | 结论 |
|---|---|---|
| CAN TX FIFO 满 | `hw_fail_total = 0`、`min_fifo_free = 18` | ❌ 排除 |
| 发送队列满 | `enqueue_fail_total = 0`（队列长 16） | ❌ 排除 |
| 总线负载高 | 发送周期降到 2 ms 现象不变 | ❌ 排除 |
| ID 冲突 | 直接转 J6 关节角，角度照常变化 | ❌ 排除 |
| 看错总线 | CAN2 上确实有 0x142 等帧 | ❌ 排除 |
| 中断栈（MSP）溢出 | `g_isr_msp_min = 0x2400E3E0`，栈顶 `0x2400E540`，只用了 352 字节 / 4 KB | ❌ 排除 |
| 任务栈溢出 | `g_stack_overflow_count = 0`（任务栈已从 512 B 加到 2 KB） | ❌ 排除 |

> 注：`CAN_driver.c` 里加了 `CAN_ISR_STACK_SAMPLE()` 和 `CAN_DEBUG_TRACE` 统计，就是这一步的产物，排查完应关闭/删除。

---

## 4. 根因

`StartDefaultTask` 一启动就调用 `MX_USB_DEVICE_Init()`，而**本板 USB_OTG_HS 的核软复位永远不完成**：

```
写 GRSTCTL.CSRST
      ↓ 硬件永远不清 CSRST
USB_CoreReset 空转 HAL_USB_TIMEOUT 次 ≈ 9.5 秒
      ↓
返回 HAL_TIMEOUT → HAL_PCD_Init 返回 HAL_ERROR
      ↓
USBD_LL_Init 里 Error_Handler()
      ↓
__disable_irq() + while(1)
      ↓
TIM4 时基 / FreeRTOS 调度 / 全部 FDCAN 收发 —— 同一瞬间全停
```

本项目**根本没有用到 USB**，这个初始化纯粹是 CubeMX 生成时的默认产物，却成了整机的唯一死因。

---

## 5. 为什么 CSRST 清不掉

把 USB 相关的所有配置寄存器都读了一遍，**软件侧没有任何配错**：

| 寄存器 | 地址 | 值 | 解读 |
|---|---|---|---|
| `RCC_CR` | `0x58024400` | `0x3303F025` | HSI48ON(bit12)=1、HSI48RDY(bit13)=1 ✅ |
| `RCC_AHB1ENR` | `0x580244D8` | `0x02000023` | USB1OTGHSEN(bit25)=1 ✅ |
| `RCC_D2CCIP2R` | `0x58024454` | `0x00300000` | USBSEL[21:20]=`0b11` = HSI48 ✅ |
| `PWR_CR3` | `0x5802480C` | `0x05000042` | USB33DEN(bit24)=1、USB33RDY(bit26)=1 ✅ |
| `GUSBCFG` | `0x4004000C` | `0x00001440` | PHYSEL(bit6)=1，已选嵌入 FS PHY ✅ |
| **`GRSTCTL`** | **`0x40040010`** | **`0x80000001`** | **AHBIDL=1，CSRST=1 卡死** ❌ |

时钟源、使能位、电压检测器、PHY 选择全部正确，唯独核软复位不返回。

**判断：这属于 USB 核自身/板级硬件问题，不是 MCU 配置问题。** 以后如果要启用 USB，需要从这一条查起（例如：芯片版本 errata、USB 供电、OTG_HS 引脚上是否接了外部 ULPI PHY 等）。

---

## 6. 修改

| 文件 | 修改 | 说明 |
|---|---|---|
| `Core/Src/freertos.c` | `StartDefaultTask` 里注释掉 `MX_USB_DEVICE_Init()` | **根治**。写在 USER CODE 区，CubeMX 重新生成不会覆盖 |
| `USB_DEVICE/Target/usbd_conf.c` | `HAL_PCD_Init` 失败不再 `Error_Handler()`，改为记录 `GRSTCTL`/`State` 到 `g_usb_init_*` 后 `return USBD_FAIL` | 防御：以后就算启用 USB 起不来，也不会锁死 CPU |
| `USB_DEVICE/App/usb_device.c` | 4 处 `Error_Handler()` 改为直接 `return` | 防御，同上。**注意此文件 CubeMX 重新生成会被覆盖** |

附带效果：因为没有人再引用 USB，链接器把整条 USB 链（`USBD_LL_Init` 等）全部回收，镜像里已经没有这段代码。

---

## 7. 验证结果

用 OpenOCD 烧录 `MDK-ARM/out/dm_mcV610/dm_mcV610/dm_mcV610.axf`，`verify` 通过，复位运行后读内存：

| 项 | 修改前 | 修改后 |
|---|---|---|
| `g_crash.magic` | `0x43524153`（进过 Error_Handler） | `0x00000000`（一次都没进） |
| 系统存活时间 | 9.6 秒后停死 | 跑过 50 秒仍在运行 |
| `uwTick` / `xTickCount` | 冻结在 `0x25AD` / `0x25AB` | 持续递增（`0x96E3` → `0xC611`） |
| `g_stack_overflow_count` | 0 | 0 |
| `g_isr_msp_min` | `0x2400E3E0` | `0x2400CBD8`（栈顶 `0x2400CD48`，用 368 B） |

**整机不再 9.6 秒停死。**

---

## 8. 遗留问题

1. ~~**J6（MF4015）收发仍未解决**~~ —— **已解决**，根因与本问题无关：FDCAN 的 `CCCR.DAR=1`（自动重发关闭）导致板子的 `0x146` 指令在仲裁中输给 J4 的 `0x142` 回包后被静默丢弃。详见 [J6无回包-FDCAN关闭自动重发.md](J6无回包-FDCAN关闭自动重发.md)。J5 仍无应答，属硬件接线/供电问题。
2. **临时调试代码待清理**：

   | 位置 | 内容 |
   |---|---|
   | `LibSE/BSP/CAN/CAN_driver.c` | `#define CAN_DEBUG_TRACE 1`，整套 `canDebug` / `CanDebug_*` 统计 |
   | `LibSE/BSP/CAN/CAN_driver.h` / `.c` | `g_isr_msp_min`、`CAN_ISR_STACK_SAMPLE()` |
   | `Core/Src/main.c` | `CrashRecord_t` / `g_crash` 黑匣子（建议保留一段时间，成本极低） |
   | `Core/Src/freertos.c` | `g_stack_overflow_count` / `g_stack_overflow_task` |
   | `MDK-ARM/startup_stm32h723xx.s` | `Stack_Size` 已从 `0x400` 改为 `0x1000`（建议保留） |

3. **`LibSE` 子模块目前是脏的**，收尾时要一并处理。
4. `MDK-ARM/build_dbg.log` 等构建中间文件未纳入版本管理。

---

## 9. 复现要点（给以后的人）

- 编译要用 **cbuild**，不是 UV4：两者输出路径不同，`launch.json` 指的是 `out/` 那份。
  ```
  cd MDK-ARM
  cbuild dm_mcV610.csolution.yml --toolchain AC6@6.19.0
  ```
- 读内存走独立 OpenOCD（不 reset，保 RAM）：
  ```
  openocd -f interface/cmsis-dap.cfg -f target/stm32h7x.cfg \
          -c "init" -c "halt" -c "mdw <addr> <count>" -c "resume" -c "shutdown"
  ```
- 烧录（按 ELF 地址，**不要**跟 `0x08000000` 和 `reset run` 混在一起，会报 `Invalid command argument`）：
  ```
  openocd -f interface/cmsis-dap.cfg -f target/stm32h7x.cfg \
          -c "init" -c "halt" \
          -c "program <abs path>/dm_mcV610.axf verify" \
          -c "reset run" -c "shutdown"
  ```
