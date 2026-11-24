#ifndef SYSTEM_DEFINE_H
#define SYSTEM_DEFINE_H

#include <stdint.h>

#define SYSTEM_CPU_FREQUENCY_MHZ      480U

/* 监控窗口与通信任务周期 */
#define TASK_MONITOR_PERIOD_MS        100U
#define TASK_MONITOR_LOOP_PERIOD_MS   2U
#define TASK_CAN_MONITOR_PERIOD_MS    6U
#define TASK_CAN_TRANSMIT_PERIOD_MS   1U
#define TASK_UART_TRANSMIT_PERIOD_MS  2U

/* 任务栈按 StackType_t 计，数值乘以 4 为字节数 */
#define TASK_MONITOR_STACK_SIZE       512U
#define TASK_INIT_STACK_SIZE          512U
#define TASK_CAN_TRANSMIT_STACK_SIZE  512U
#define TASK_UART_TRANSMIT_STACK_SIZE 256U

/* 监控和通信服务优先级高于普通应用任务，便于发现卡死并执行恢复 */
#define TASK_MONITOR_PRIORITY         6U
#define TASK_INIT_PRIORITY            7U
#define TASK_CAN_TRANSMIT_PRIORITY    5U
#define TASK_UART_TRANSMIT_PRIORITY   2U

#endif
