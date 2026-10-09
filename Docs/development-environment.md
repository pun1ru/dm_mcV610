# 开发环境

## 目标与构建配置

- 目标芯片：STM32H723VGTx，Cortex-M7 内核，启用硬件双精度 FPU。
- CMSIS 解决方案：`MDK-ARM/dm_mcV610.csolution.yml`。
- 解决方案指定的编译器：`AC6@6.19.0`。
- 解决方案由 `uv2csolution@1.6.0` 转换生成。其中的
  `created-for: CMSIS-Toolbox@2.7.0` 是转换时写入的目标版本；本次实际用于构建的
  CMSIS-Toolbox 版本为 2.12.0。
- `MDK-ARM/vcpkg-configuration.json` 声明了构建工具，没有声明 Arm 编译器包。
  AC6 6.19 来自本机 Keil 安装，并通过 `AC6_TOOLCHAIN_6_19_0` 注册。

## 已验证的本机工具

| 工具 | 本次使用的版本 | 路径或配置 |
| --- | --- | --- |
| CMSIS-Toolbox（`cbuild`、`csolution`） | 2.12.0 | `C:\Users\liaoz\Desktop\VStools\cmsis-toolbox-windows-amd64\bin` |
| Arm Compiler for Embedded（`armclang`） | 6.19 | `C:\Keil_v5\ARM\ARMCLANG\bin` |
| CMake | 4.1.2 | `C:\Program Files\CMake\bin` |
| Ninja | 1.10.2 | `C:\Users\liaoz\Desktop\VStools\ninja.exe` |

本次构建使用了以下环境变量：

```text
AC6_TOOLCHAIN_6_19_0=C:\Keil_v5\ARM\ARMCLANG\bin
CMSIS_PACK_ROOT=C:\Keil_v5\ARM\PACK
```

实际解析并使用的 Pack 为 `ARM::CMSIS@5.9.0` 和 `Keil::STM32H7xx_DFP@4.1.3`。
工程还声明了 CMSIS-DSP 和 CMSIS-NN，但本次构建没有使用它们。

Ninja 1.10.2 可以完成构建，但 `cbuild` 提示使用 1.11.1 或更新版本可减少冗长输出。

在 `MDK-ARM` 目录中检查工具链并构建：

```powershell
cbuild list toolchains
cbuild .\dm_mcV610.csolution.yml --update-rte -j 4
```

`--update-rte` 会在需要时生成缺失的 RTE 或调试配置文件。输出文件为
`MDK-ARM/out/dm_mcV610/dm_mcV610/dm_mcV610.axf`。

## FreeRTOS 移植层变更

工程使用 FreeRTOS Kernel 10.3.1。本次从同级目录的 `serialleg2027` 工程复制以下两个文件，
替换当前工程中的同路径文件：

```text
Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c
Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/portmacro.h
```

来源目录相对于当前工程根目录的路径为：

```text
../serialleg2027/Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/
```

两个工程均使用 STM32H723VGTx 和 FreeRTOS Kernel 10.3.1。比较整个 FreeRTOS
`Source` 目录后，只有上述两个移植层文件存在差异。

原移植层使用 Arm Compiler 5 风格的 `__asm { ... }`、`__forceinline`、`__dsb` 和
`__isb` 等写法，无法使用当前 ARMClang 6.19 编译。复制的版本改用 ARMClang 可编译的
内联汇编和属性。目录名 `RVDS/ARM_CM4F` 保持原样，因此 CMSIS 和 uVision 工程现有的
源文件路径、头文件搜索路径仍然有效；`Core/Inc/FreeRTOSConfig.h` 未修改。

## 验证结果与范围

2026-10-09 使用 AC6 6.19 执行
`cbuild .\dm_mcV610.csolution.yml --update-rte -j 4`，结果为
`1 succeeded, 0 failed`。链接映射显示 `SVC_Handler`、`PendSV_Handler` 和
`SysTick_Handler` 均来自移植后的 `port.o`。

目前已验证编译和链接；调度器启动、系统节拍、任务切换及中断行为仍需在目标板上验证。
