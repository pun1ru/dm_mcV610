#ifndef SYSTEM_INSTANCE_H
#define SYSTEM_INSTANCE_H

#include "CAN_driver.h"
#include "DMMotor.h"
#include "debug.h"
#include "LKMotor.h"
#include "RMMotor.h"
#include "gimbal.h"
#include "gimbal_ammo_state.h"
#include "ins.h"
#include "monitor.h"
#include "queue.h"
#include "referee.h"
#include "remote_control.h"
#include "shooter.h"
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
/* 协议解析器和调试输出挂在 UART 口上，属于通信层 */
extern Referee_t referee_receiver;
extern RC_Remote_t rc_receiver;
extern Debug_t debug_sender;

/*--------------------------------------ipc 实例--------------------------------------*/
extern QueueHandle_t gimbalCmdQueue;    /* 状态机 -> 云台，元素 GimbalCmdMail_t */
extern QueueHandle_t insMsgQueue;       /* INS -> 云台，元素 INSMsg_t */
extern QueueHandle_t shooterCmdQueue;   /* 状态机 -> 摩擦轮，元素 ShooterCmdMail_t */
extern QueueHandle_t debugMsgQueue;     /* 各模块 -> UART 发送任务，元素 DebugMsg_t */

/*--------------------------------------电机实例--------------------------------------*/
extern DMMotor_t gimbalYawMotor;
extern LKMotor_t gimbalPitchMotor;

/* 摩擦轮：3 个 C620，同 hero_up 排布 左 右 上，ID 1~3，共用 0x200 一帧 */
typedef enum {
    SHOOTER_FRIC_LEFT = 0U,
    SHOOTER_FRIC_RIGHT,
    SHOOTER_FRIC_UP,
    SHOOTER_FRIC_COUNT
} ShooterFricId_t;

extern RMMotorGroup_t shooterFricGroup;        /* tx 0x200 */
extern RMMotor_t shooterFricMotor[SHOOTER_FRIC_COUNT];

/*--------------------------------------模块实例--------------------------------------*/
extern Monitor_t systemMonitor;
extern INS_t board_ins;
extern Gimbal_t gimbalInstance;
extern Shooter_t shooterInstance;
extern GimbalAmmoState_t gimbalAmmoState;

/*--------------------------------------task 实例--------------------------------------*/
/* 参与心跳监控的任务，值即 Monitor_Beat 的任务号，也是 systemTaskPeriodMs 的下标 */
typedef enum {
    SYSTEM_TASK_TRANSMIT = 0U,
    SYSTEM_TASK_UART_TRANSMIT,
    SYSTEM_TASK_INS,
    SYSTEM_TASK_GIMBAL,
    SYSTEM_TASK_STATE,
    SYSTEM_TASK_SHOOTER,
    SYSTEM_TASK_COUNT
} SystemTaskId_t;

extern const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT];

extern TaskHandle_t MonitorTaskHandle;
extern TaskHandle_t TransmitTaskHandle;
extern TaskHandle_t UartTransmitTaskHandle;
extern TaskHandle_t INSTaskHandle;
extern TaskHandle_t GimbalTaskHandle;
extern TaskHandle_t StateTaskHandle;
extern TaskHandle_t ShooterTaskHandle;

#endif
