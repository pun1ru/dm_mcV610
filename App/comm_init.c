#include "comm_init.h"

#include "CAN_driver.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "fdcan.h"
#include "task_define.h"
#include "task_receive.h"
#include "task_system.h"
#include "usart.h"

#include <string.h>

/*--------------------------------------CAN--------------------------------------*/
static void CANQueueSetsInit(void)
{
    receive_can_queues = xQueueCreateSet((UBaseType_t)TASK_CAN_INSTANCE_COUNT * (UBaseType_t)CAN_RX_QUEUE_LENGTH);
    transmit_can_queues = xQueueCreateSet((UBaseType_t)TASK_CAN_INSTANCE_COUNT * (UBaseType_t)CAN_TX_QUEUE_LENGTH);

    for (uint8_t i = 0U; i < TASK_CAN_INSTANCE_COUNT; ++i) {
        xQueueAddToSet(can_instances[i].rx_queue, receive_can_queues);
        xQueueAddToSet(can_instances[i].tx_queue, transmit_can_queues);
    }
}

static void Comm_CanRegister(void)
{
    memset(can_instances, 0, sizeof(can_instances));

    can_instances[0].can = &hfdcan1;
    can_instances[0].rx_fifo = FDCAN_RX_FIFO0;
    can_instances[0].on_receive = TaskReceive_CAN1Callback;

    can_instances[1].can = &hfdcan2;
    can_instances[1].rx_fifo = FDCAN_RX_FIFO1;
    can_instances[1].on_receive = TaskReceive_CAN2Callback;

    can_instances[2].can = &hfdcan3;
    can_instances[2].rx_fifo = FDCAN_RX_FIFO1;
    can_instances[2].on_receive = TaskReceive_CAN3Callback;

    CAN_RegisterInstances(can_instances, TASK_CAN_INSTANCE_COUNT);
    CANTransmit_QueueInit();
    CANReceive_QueueInit();
    CANQueueSetsInit();
}

/*--------------------------------------UART--------------------------------------*/
static void UARTRegister(UartInstance_t *uart, UART_HandleTypeDef *huart,
                         EventBits_t rx_bit, EventBits_t error_bit)
{
    memset(uart, 0, sizeof(*uart));
    uart->huart = huart;
    UART_Register(uart);
    UART_SetEventGroup(uart, UartEventGroup, rx_bit, error_bit);
}

static void Comm_UartRegister(void)
{
    UartEventGroup = xEventGroupCreate();
    UART_EventGroup = UartEventGroup;

    UARTRegister(&uart_instances[TASK_UART_HUART5_INDEX], &huart5,
                 TASK_UART_EVENT_HUART5_RX, TASK_UART_EVENT_HUART5_ERROR);
    UARTRegister(&uart_instances[TASK_UART_HUART7_INDEX], &huart7,
                 TASK_UART_EVENT_HUART7_RX, TASK_UART_EVENT_HUART7_ERROR);
    UARTRegister(&uart_instances[TASK_UART_HUART1_INDEX], &huart1,
                 TASK_UART_EVENT_HUART1_RX, TASK_UART_EVENT_HUART1_ERROR);
    UARTRegister(&uart_instances[TASK_UART_HUART2_INDEX], &huart2,
                 TASK_UART_EVENT_HUART2_RX, TASK_UART_EVENT_HUART2_ERROR);
    UARTRegister(&uart_instances[TASK_UART_HUART3_INDEX], &huart3,
                 TASK_UART_EVENT_HUART3_RX, TASK_UART_EVENT_HUART3_ERROR);
    UARTRegister(&uart_instances[TASK_UART_HUART10_INDEX], &huart10,
                 TASK_UART_EVENT_HUART10_RX, TASK_UART_EVENT_HUART10_ERROR);

    /* 没配回调的口走驱动里的默认处理 */
    uart_instances[TASK_UART_HUART5_INDEX].on_packet = TaskReceive_RCInput;
    uart_instances[TASK_UART_HUART5_INDEX].on_error = TaskReceive_RCError;
    uart_instances[TASK_UART_HUART1_INDEX].on_packet = TaskReceive_RefereeInput;
    uart_instances[TASK_UART_HUART1_INDEX].on_error = TaskReceive_RefereeError;
}

/*--------------------------------------入口--------------------------------------*/
void Comm_Register(void)
{
    Comm_CanRegister();
    Comm_UartRegister();
}

void Comm_Start(void)
{
    can_bsp_init();
    for (uint8_t i = 0U; i < TASK_UART_INSTANCE_COUNT; ++i) {
        UART_StartReceiveEx(&uart_instances[i]);
    }
}
