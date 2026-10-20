#ifndef SYSTEM_INSTANCE_H
#define SYSTEM_INSTANCE_H

#include "CAN_driver.h"
#include "DMMotor.h"
#include "LKMotor.h"
#include "event_groups.h"
#include "gimbal.h"
#include "gimbal_ammo_state.h"
#include "ins.h"
#include "monitor.h"
#include "queue.h"
#include "referee.h"
#include "remote_control.h"
#include "task.h"
#include "uart_driver.h"

/* 实例按层分段，从底到顶：通信 -> ipc -> 电机 -> 模块 -> task
 * 各层的创建与绑定在同目录的 comm_init、ipc_init、motor_init、module_init、task_init */

/*--------------------------------------通信实例--------------------------------------*/
#define TASK_CAN_INSTANCE_COUNT       3U
#define TASK_UART_INSTANCE_COUNT      6U

enum {
    SYSTEM_CAN1_INDEX = 0U,
    SYSTEM_CAN2_INDEX,
    SYSTEM_CAN3_INDEX
};

enum {
    TASK_UART_HUART5_INDEX = 0U,
    TASK_UART_HUART7_INDEX,
    TASK_UART_HUART1_INDEX,
    TASK_UART_HUART2_INDEX,
    TASK_UART_HUART3_INDEX,
    TASK_UART_HUART10_INDEX
};

#define TASK_UART_EVENT_HUART5_RX     ((EventBits_t)(1UL << 0U))
#define TASK_UART_EVENT_HUART5_ERROR  ((EventBits_t)(1UL << 1U))
#define TASK_UART_EVENT_HUART7_RX     ((EventBits_t)(1UL << 2U))
#define TASK_UART_EVENT_HUART7_ERROR  ((EventBits_t)(1UL << 3U))
#define TASK_UART_EVENT_HUART1_RX     ((EventBits_t)(1UL << 4U))
#define TASK_UART_EVENT_HUART1_ERROR  ((EventBits_t)(1UL << 5U))
#define TASK_UART_EVENT_HUART2_RX     ((EventBits_t)(1UL << 6U))
#define TASK_UART_EVENT_HUART2_ERROR  ((EventBits_t)(1UL << 7U))
#define TASK_UART_EVENT_HUART3_RX     ((EventBits_t)(1UL << 8U))
#define TASK_UART_EVENT_HUART3_ERROR  ((EventBits_t)(1UL << 9U))
#define TASK_UART_EVENT_HUART10_RX    ((EventBits_t)(1UL << 10U))
#define TASK_UART_EVENT_HUART10_ERROR ((EventBits_t)(1UL << 11U))

extern CanInstance_t can_instances[TASK_CAN_INSTANCE_COUNT];
extern QueueSetHandle_t receive_can_queues;
extern QueueSetHandle_t transmit_can_queues;
extern UartInstance_t uart_instances[TASK_UART_INSTANCE_COUNT];
extern EventGroupHandle_t UartEventGroup;
/* 协议解析器挂在 UART 口上，属于通信层 */
extern Referee_t referee_receiver;
extern RC_Remote_t rc_receiver;

/*--------------------------------------ipc 实例--------------------------------------*/
extern QueueHandle_t gimbalCmdQueue;    /* 状态机 -> 云台，元素 GimbalCmdMail_t */
extern QueueHandle_t insMsgQueue;       /* INS -> 云台，元素 INSMsg_t */

/*--------------------------------------电机实例--------------------------------------*/
extern DMMotor_t gimbalYawMotor;
extern LKMotor_t gimbalPitchMotor;

/*--------------------------------------模块实例--------------------------------------*/
extern Monitor_t systemMonitor;
extern INS_t board_ins;
extern Gimbal_t gimbalInstance;
extern GimbalAmmoState_t gimbalAmmoState;

/*--------------------------------------task 实例--------------------------------------*/
/* 参与心跳监控的任务，值即 Monitor_Beat 的任务号，也是 systemTaskPeriodMs 的下标 */
typedef enum {
    SYSTEM_TASK_RECEIVE = 0U,
    SYSTEM_TASK_TRANSMIT,
    SYSTEM_TASK_INS,
    SYSTEM_TASK_GIMBAL,
    SYSTEM_TASK_STATE,
    SYSTEM_TASK_COUNT
} SystemTaskId_t;

extern const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT];

extern TaskHandle_t MonitorTaskHandle;
extern TaskHandle_t ReceiveTaskHandle;
extern TaskHandle_t TransmitTaskHandle;
extern TaskHandle_t INSTaskHandle;
extern TaskHandle_t GimbalTaskHandle;
extern TaskHandle_t StateTaskHandle;

#endif
