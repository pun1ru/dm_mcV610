#include "system_instance.h"

#include "system_define.h"

/*--------------------------------------通信实例--------------------------------------*/
CanInstance_t can_instances[SYSTEM_CAN_INSTANCE_COUNT];
QueueSetHandle_t transmit_can_queues;

UartInstance_t uart_instances[SYSTEM_UART_INSTANCE_COUNT];

Referee_t referee_receiver;
RC_Remote_t rc_receiver;
Debug_t debug_sender;

/*--------------------------------------ipc 实例--------------------------------------*/
QueueHandle_t gimbalCmdQueue;
QueueHandle_t insMsgQueue;
QueueHandle_t shooterCmdQueue;
QueueHandle_t debugMsgQueue;

/*--------------------------------------motor实例--------------------------------------*/
DMMotor_t gimbalYawMotor;
LKMotor_t gimbalPitchMotor;
RMMotorGroup_t shooterFricGroup;
RMMotor_t shooterFricMotor[SHOOTER_FRIC_COUNT];

/*--------------------------------------module实例--------------------------------------*/
Monitor_t systemMonitor;
INS_t board_ins;
Gimbal_t gimbalInstance;
Shooter_t shooterInstance;
GimbalAmmoState_t gimbalAmmoState;

/*--------------------------------------task 实例--------------------------------------*/
/* 顺序与 SystemTaskId_t 一致 */
const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT] = {
    TASK_TRANSMIT_PERIOD_MS,
    TASK_UART_TRANSMIT_PERIOD_MS,
    TASK_INS_PERIOD_MS,
    TASK_GIMBAL_PERIOD_MS,
    TASK_STATE_PERIOD_MS,
    TASK_SHOOTER_PERIOD_MS
};

TaskHandle_t MonitorTaskHandle;
TaskHandle_t TransmitTaskHandle;
TaskHandle_t UartTransmitTaskHandle;
TaskHandle_t INSTaskHandle;
TaskHandle_t GimbalTaskHandle;
TaskHandle_t StateTaskHandle;
TaskHandle_t ShooterTaskHandle;
