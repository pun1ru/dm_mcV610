#include "system_instance.h"

#include "system_define.h"

/*--------------------------------------通信实例--------------------------------------*/
CanInstance_t can_instances[SYSTEM_CAN_INSTANCE_COUNT];
QueueSetHandle_t transmit_can_queues;

UartInstance_t uart_instances[SYSTEM_UART_INSTANCE_COUNT];

Referee_t referee_receiver;
RC_Remote_t rc_receiver;

/*--------------------------------------ipc 实例--------------------------------------*/

/*--------------------------------------motor实例--------------------------------------*/

/*--------------------------------------module实例--------------------------------------*/
Monitor_t systemMonitor;

/*--------------------------------------task 实例--------------------------------------*/
/* 顺序与 SystemTaskId_t 一致 */
const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT] = {
    TASK_TRANSMIT_PERIOD_MS,
    TASK_UART_TRANSMIT_PERIOD_MS
};

TaskHandle_t MonitorTaskHandle;
TaskHandle_t TransmitTaskHandle;
TaskHandle_t UartTransmitTaskHandle;
