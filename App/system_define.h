#ifndef SYSTEM_DEFINE_H
#define SYSTEM_DEFINE_H

#include <stdint.h>

#define SYSTEM_CPU_FREQUENCY_MHZ      480U
#define TASK_MONITOR_PERIOD_MS        100U
#define TASK_MONITOR_LOOP_PERIOD_MS   2U
#define TASK_CAN_MONITOR_PERIOD_MS    6U
//任务创建定义，新增模块任务往后加
//任务周期
#define TASK_TRANSMIT_PERIOD_MS       1U
#define TASK_UART_TRANSMIT_PERIOD_MS  2U
#define TASK_ARM_PERIOD_MS            2U
//任务的栈空间
#define TASK_MONITOR_STACK_SIZE       512U
#define TASK_INIT_STACK_SIZE          512U
#define TASK_TRANSMIT_STACK_SIZE      512U
#define TASK_UART_TRANSMIT_STACK_SIZE 256U
#define TASK_ARM_STACK_SIZE           256U
//任务优先级
#define TASK_MONITOR_PRIORITY         1U
#define TASK_INIT_PRIORITY            6U
#define TASK_TRANSMIT_PRIORITY        5U
#define TASK_ARM_PRIORITY             3U
/* UART 发送是调试输出，优先级放在控制任务之下，不挤占控制 */
#define TASK_UART_TRANSMIT_PRIORITY   2U

//上位机上报：UART 发送任务周期 2 ms，取 5 即每 10 ms 一帧 JOINT_STATE（100 Hz）
#define HOST_STATE_DIVIDER            5U

//任务间通信队列长度，新增队列往后加

#endif
