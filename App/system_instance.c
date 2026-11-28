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
RMMotorGroup_t armRmGroupLow;
RMMotorGroup_t armRmGroupHigh;
RMMotor_t armJ0Motor;
DMMotor_t armJ1Motor;
RMMotor_t armJ2Motor;
DMMotor_t armJ3Motor;
LKMotor_t armJ4Motor;
LKMotor_t armJ5Motor;
LKMotor_t armJ6Motor;

/*--------------------------------------module实例--------------------------------------*/
Monitor_t systemMonitor;
Arm_t armInstance;
Host_t hostInstance;

/*--------------------------------------task 实例--------------------------------------*/
/* 顺序与 SystemTaskId_t 一致 */
MonitorHeartbeat_t can_tx_heartbeat;
MonitorHeartbeat_t uart_tx_heartbeat;
MonitorHeartbeat_t arm_heartbeat;

TaskHandle_t MonitorTaskHandle;
TaskHandle_t CanTransmitTaskHandle;
TaskHandle_t UartTransmitTaskHandle;
TaskHandle_t ArmTaskHandle;
