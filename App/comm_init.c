#include "comm_init.h"

#include "CAN_driver.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "fdcan.h"
#include "referee.h"
#include "remote_control.h"
#include "system_define.h"
#include "system_instance.h"
#include "usart.h"

#include <string.h>

/*--------------------------------------UART 回调--------------------------------------*/
static void Comm_RCInput(const uint8_t *data, uint16_t size)
{
    RC_Remote_Input(&rc_receiver, data, size);
}

static void Comm_RCError(void)
{
    RC_Remote_OnError(&rc_receiver);
}

static void Comm_RefereeInput(const uint8_t *data, uint16_t size)
{
    Referee_Input(&referee_receiver, data, size);
}

static void Comm_RefereeError(void)
{
    Referee_Reset(&referee_receiver);
}

/*--------------------------------------登记--------------------------------------*/
/* 只有发送走队列集；接收在 CAN 中断里直接分发，没有接收队列 */
static void CANQueueSetsInit(void)
{
    transmit_can_queues = xQueueCreateSet((UBaseType_t)TASK_CAN_INSTANCE_COUNT * (UBaseType_t)CAN_TX_QUEUE_LENGTH);

    for (uint8_t i = 0U; i < TASK_CAN_INSTANCE_COUNT; ++i) {
        xQueueAddToSet(can_instances[i].tx_queue, transmit_can_queues);
    }
}

static void Comm_CanRegister(void)
{
    memset(can_instances, 0, sizeof(can_instances));

    can_instances[SYSTEM_CAN1_INDEX].can = &hfdcan1;
    can_instances[SYSTEM_CAN1_INDEX].rx_fifo = FDCAN_RX_FIFO0;

    can_instances[SYSTEM_CAN2_INDEX].can = &hfdcan2;
    can_instances[SYSTEM_CAN2_INDEX].rx_fifo = FDCAN_RX_FIFO1;

    can_instances[SYSTEM_CAN3_INDEX].can = &hfdcan3;
    can_instances[SYSTEM_CAN3_INDEX].rx_fifo = FDCAN_RX_FIFO1;

    CAN_RegisterInstances(can_instances, TASK_CAN_INSTANCE_COUNT);
    CANTransmit_QueueInit();
    CANQueueSetsInit();
}

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

    /* 协议解析器挂在 UART 口上，没配回调的口收到的包直接丢弃 */
    Referee_Init(&referee_receiver);
    RC_Init(&rc_receiver);
    uart_instances[TASK_UART_HUART5_INDEX].on_packet = Comm_RCInput;
    uart_instances[TASK_UART_HUART5_INDEX].on_error = Comm_RCError;
    uart_instances[TASK_UART_HUART1_INDEX].on_packet = Comm_RefereeInput;
    uart_instances[TASK_UART_HUART1_INDEX].on_error = Comm_RefereeError;
}

void Comm_Register(void)
{
    Comm_CanRegister();
    Comm_UartRegister();
}

void Comm_BindUartTask(void)
{
    for (uint8_t i = 0U; i < TASK_UART_INSTANCE_COUNT; ++i) {
        (void)UART_SetNotifyTask(&uart_instances[i], UartReceiveTaskHandle);
    }
}

void Comm_Start(void)
{
    can_bsp_init();
    for (uint8_t i = 0U; i < TASK_UART_INSTANCE_COUNT; ++i) {
        UART_StartReceiveEx(&uart_instances[i]);
    }
}

/*--------------------------------------数据处理--------------------------------------*/
/* CAN 接收没有数据处理函数：中断（CAN_driver.c 的 CANReceive_FromIsr）读空 FIFO 后直接调绑定的回调 */
static void Comm_ProcessUartInstance(UartInstance_t *uart)
{
    uint8_t packet[UART_DMA_BUFFER_SIZE];
    uint16_t packet_size;

    while (UART_RxPop(uart, packet, sizeof(packet), &packet_size) == pdTRUE) {
        if (uart->on_packet != NULL) {
            uart->on_packet(packet, packet_size);
        }
    }
}

void Comm_ProcessUart(void)
{
    for (uint8_t i = 0U; i < TASK_UART_INSTANCE_COUNT; ++i) {
        UartInstance_t *uart = &uart_instances[i];
        EventBits_t events = UART_GetEvents(uart, pdTRUE);

        if ((events & UART_EVENT_RX_READY) != 0U || UART_RxAvailable(uart) != 0U) {
            Comm_ProcessUartInstance(uart);
        }
        if ((events & (UART_EVENT_RX_ERROR | UART_EVENT_RX_OVERFLOW)) != 0U &&
            uart->on_error != NULL) {
            uart->on_error();
        }
    }
}

/* 各模块把 CAN 帧放进发送队列，这里取出来发出去；发送失败由 CAN 驱动自己计数 */
void Comm_ProcessTransmit(void)
{
    CanTxFrame_t frame;
    QueueSetMemberHandle_t ready = xQueueSelectFromSet(
        transmit_can_queues, pdMS_TO_TICKS(TASK_TRANSMIT_PERIOD_MS));

    while (ready != NULL) {
        if (xQueueReceive(ready, &frame, 0U) == pdPASS) {
            (void)fdcanx_send_data(frame.can, frame.id, frame.data, frame.length);
        }
        ready = xQueueSelectFromSet(transmit_can_queues, 0U);
    }
}
