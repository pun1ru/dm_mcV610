#ifndef SYSTEM_DEFINE_H
#define SYSTEM_DEFINE_H

#include <stdint.h>

#define SYSTEM_CPU_FREQUENCY_MHZ      480U
#define TASK_MONITOR_PERIOD_MS        100U
#define TASK_MONITOR_LOOP_PERIOD_MS   2U
#define TASK_CAN_MONITOR_PERIOD_MS    6U
//任务创建定义
//任务周期
#define TASK_TRANSMIT_PERIOD_MS       1U
#define TASK_UART_TRANSMIT_PERIOD_MS  2U
#define TASK_INS_PERIOD_MS            2U
#define TASK_GIMBAL_PERIOD_MS         2U
#define TASK_SHOOTER_PERIOD_MS        2U
#define TASK_STATE_PERIOD_MS          10U
//任务的栈空间
#define TASK_MONITOR_STACK_SIZE       512U
#define TASK_INIT_STACK_SIZE          512U
#define TASK_TRANSMIT_STACK_SIZE      512U
#define TASK_UART_TRANSMIT_STACK_SIZE 256U
#define TASK_INS_STACK_SIZE           512U
#define TASK_GIMBAL_STACK_SIZE        384U
#define TASK_SHOOTER_STACK_SIZE       256U
#define TASK_STATE_STACK_SIZE         384U
//任务优先级
#define TASK_MONITOR_PRIORITY         1U
#define TASK_INIT_PRIORITY            6U
#define TASK_GIMBAL_PRIORITY          3U
#define TASK_SHOOTER_PRIORITY         3U
#define TASK_STATE_PRIORITY           3U
#define TASK_INS_PRIORITY             4U
#define TASK_TRANSMIT_PRIORITY        5U
/* UART 发送是调试输出，优先级放在控制任务之下，不挤占控制 */
#define TASK_UART_TRANSMIT_PRIORITY   2U

//任务间通信队列长度
#define IPC_GIMBAL_CMD_QUEUE_LENGTH   1U
#define IPC_INS_MSG_QUEUE_LENGTH      1U
#define IPC_SHOOTER_CMD_QUEUE_LENGTH  1U
/* 调试数据队列：各模块一个周期发一条，发送任务一个周期取空，留够几个模块同周期发布的余量 */
#define IPC_DEBUG_MSG_QUEUE_LENGTH    8U

//调试输出：UART 发送任务把通道数组拼成 VOFA JustFloat 帧发出去
//每个模块占连续的几个通道，新增模块往后排，并同步加大 DEBUG_CHANNEL_COUNT
#define DEBUG_CHANNEL_SHOOTER_SPEED   0U    /* 摩擦轮转速，占 3 个通道：左 右 上 */
#define DEBUG_CHANNEL_COUNT           3U    /* 每帧发多少个通道 */
/* 云台任务收到的命令超过这个时间未更新就视为状态任务失联，进入保护 */
#define GIMBAL_CMD_TIMEOUT_MS         50U
/* 摩擦轮同理 */
#define SHOOTER_CMD_TIMEOUT_MS        50U
/* 遥控器超过这个时间没收到数据按离线处理 */

//有点杂
#define SYSTEM_GIMBAL_FORCE_ZERO_OUTPUT   0U
/* 云台启动后等 INS 完成初始化、电机上电回传反馈再进入控制循环 */
#define TASK_GIMBAL_STARTUP_DELAY_MS  400U
//调试用的 宏
#define REMOTE_TIMEOUT_MS             100U
/* 调试开关：1 时云台电机收到的指令强制为 0（电机照常使能，只是零力矩/零电流），双环 PID 照常计算
 * 用来在不让电机出力的情况下观察 PID 输出，置 0 恢复正常输出 */


#endif

