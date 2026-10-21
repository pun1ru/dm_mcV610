#include "system_instance.h"

#include "system_define.h"

/*--------------------------------------通信实例--------------------------------------*/
CanInstance_t can_instances[TASK_CAN_INSTANCE_COUNT];
QueueSetHandle_t receive_can_queues;
QueueSetHandle_t transmit_can_queues;

UartInstance_t uart_instances[TASK_UART_INSTANCE_COUNT];
EventGroupHandle_t UartEventGroup;

Referee_t referee_receiver;
RC_Remote_t rc_receiver;

/*--------------------------------------ipc 实例--------------------------------------*/
QueueHandle_t gimbalCmdQueue;
QueueHandle_t insMsgQueue;

/*--------------------------------------电机实例--------------------------------------*/
DMMotor_t gimbalYawMotor;
LKMotor_t gimbalPitchMotor;

/*--------------------------------------模块实例--------------------------------------*/
Monitor_t systemMonitor;
INS_t board_ins;
Gimbal_t gimbalInstance;
GimbalAmmoState_t gimbalAmmoState;

/*--------------------------------------task 实例--------------------------------------*/
/* 顺序与 SystemTaskId_t 一致 */
const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT] = {
    TASK_CAN_RECEIVE_PERIOD_MS,
    TASK_UART_RECEIVE_PERIOD_MS,
    TASK_TRANSMIT_PERIOD_MS,
    TASK_INS_PERIOD_MS,
    TASK_GIMBAL_PERIOD_MS,
    TASK_STATE_PERIOD_MS
};

TaskHandle_t MonitorTaskHandle;
TaskHandle_t CanReceiveTaskHandle;
TaskHandle_t UartReceiveTaskHandle;
TaskHandle_t TransmitTaskHandle;
TaskHandle_t INSTaskHandle;
TaskHandle_t GimbalTaskHandle;
TaskHandle_t StateTaskHandle;
