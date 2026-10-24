#ifndef SYSTEM_INSTANCE_H
#define SYSTEM_INSTANCE_H

#include "CAN_driver.h"
#include "DMMotor.h"
#include "arm.h"
#include "LKMotor.h"
#include "RMMotor.h"
#include "monitor.h"
#include "queue.h"
#include "referee.h"
#include "remote_control.h"
#include "task.h"
#include "uart_driver.h"

/* 实例按层分段，从底到顶：通信 -> ipc -> 电机 -> 模块 -> task
 * 各层的创建与绑定在同目录的 comm_init、ipc_init、motor_init、module_init、task_init */

/*--------------------------------------通信实例--------------------------------------*/
#define SYSTEM_CAN_INSTANCE_COUNT     3U
#define SYSTEM_UART_INSTANCE_COUNT    6U

enum {
    SYSTEM_CAN1_INDEX = 0U,
    SYSTEM_CAN2_INDEX,
    SYSTEM_CAN3_INDEX
};

enum {
    SYSTEM_UART5_INDEX = 0U,
    SYSTEM_UART7_INDEX,
    SYSTEM_UART1_INDEX,
    SYSTEM_UART2_INDEX,
    SYSTEM_UART3_INDEX,
    SYSTEM_UART10_INDEX
};

extern CanInstance_t can_instances[SYSTEM_CAN_INSTANCE_COUNT];
extern QueueSetHandle_t transmit_can_queues;
extern UartInstance_t uart_instances[SYSTEM_UART_INSTANCE_COUNT];
/* 协议解析器挂在 UART 口上，属于通信层 */
extern Referee_t referee_receiver;
extern RC_Remote_t rc_receiver;

/*--------------------------------------ipc 实例--------------------------------------*/

/*--------------------------------------电机实例--------------------------------------*/
/* 七轴臂 J0~J6：J0~J3 在 CAN1，J4~J6 在 CAN2；GM6020 按 ID 段分组，1~4 一组、5~7 一组 */
extern RMMotorGroup_t armRmGroupLow;
extern RMMotorGroup_t armRmGroupHigh;
extern RMMotor_t armJ0Motor;    /* GM6020 */
extern DMMotor_t armJ1Motor;    /* DM-J4310 */
extern RMMotor_t armJ2Motor;    /* GM6020 */
extern DMMotor_t armJ3Motor;    /* DM-J4310 */
extern LKMotor_t armJ4Motor;    /* MF4005 */
extern LKMotor_t armJ5Motor;    /* MF4015 */
extern LKMotor_t armJ6Motor;    /* MF4015 */

/*--------------------------------------模块实例--------------------------------------*/
extern Monitor_t systemMonitor;
extern Arm_t armInstance;

/*--------------------------------------task 实例--------------------------------------*/
/* 参与心跳监控的任务，值即 Monitor_Beat 的任务号，也是 systemTaskPeriodMs 的下标
 * 新增模块任务加在 SYSTEM_TASK_COUNT 之前，并同步 systemTaskPeriodMs */
typedef enum {
    SYSTEM_TASK_TRANSMIT = 0U,
    SYSTEM_TASK_UART_TRANSMIT,
    SYSTEM_TASK_ARM,
    SYSTEM_TASK_COUNT
} SystemTaskId_t;

extern const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT];

extern TaskHandle_t MonitorTaskHandle;
extern TaskHandle_t TransmitTaskHandle;
extern TaskHandle_t UartTransmitTaskHandle;
extern TaskHandle_t ArmTaskHandle;

#endif
