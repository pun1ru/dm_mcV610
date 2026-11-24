#ifndef SYSTEM_INSTANCE_H
#define SYSTEM_INSTANCE_H

#include "CAN_driver.h"
#include "monitor.h"
#include "queue.h"
#include "referee.h"
#include "remote_control.h"
#include "task.h"
#include "uart_driver.h"

#define SYSTEM_CAN_INSTANCE_COUNT     3U
#define SYSTEM_UART_INSTANCE_COUNT    8U

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
    SYSTEM_UART10_INDEX,
    SYSTEM_UART8_INDEX,
    SYSTEM_UART9_INDEX
};

extern CanInstance_t can_instances[SYSTEM_CAN_INSTANCE_COUNT];
extern QueueSetHandle_t transmit_can_queues;
extern UartInstance_t uart_instances[SYSTEM_UART_INSTANCE_COUNT];
extern Referee_t referee_receiver;
extern RC_Remote_t rc_receiver;

extern Monitor_t systemMonitor;

typedef enum {
    SYSTEM_TASK_CAN_TRANSMIT = 0U,
    SYSTEM_TASK_UART_TRANSMIT,
    SYSTEM_TASK_COUNT
} SystemTaskId_t;

extern const uint16_t systemTaskPeriodMs[SYSTEM_TASK_COUNT];
extern TaskHandle_t MonitorTaskHandle;
extern TaskHandle_t CanTransmitTaskHandle;
extern TaskHandle_t UartTransmitTaskHandle;

#endif
