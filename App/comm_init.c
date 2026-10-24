#include "comm_init.h"

#include "CAN_driver.h"
#include "FreeRTOS.h"
#include "fdcan.h"
#include "referee.h"
#include "remote_control.h"
#include "system_define.h"
#include "system_instance.h"
#include "usart.h"

#include <string.h>

/*--------------------------------------登记--------------------------------------*/
/* 只有发送走队列集；接收在 CAN 中断里直接分发，没有接收队列 */
static void CANQueueSetsInit(void)
{
    transmit_can_queues = xQueueCreateSet((UBaseType_t)SYSTEM_CAN_INSTANCE_COUNT * (UBaseType_t)CAN_TX_QUEUE_LENGTH);

    for (uint8_t i = 0U; i < SYSTEM_CAN_INSTANCE_COUNT; ++i) {
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

    CAN_RegisterInstances(can_instances, SYSTEM_CAN_INSTANCE_COUNT);
    CANTransmit_QueueInit();
    CANQueueSetsInit();
}

static void UARTRegister(UartInstance_t *uart, UART_HandleTypeDef *huart)
{
    memset(uart, 0, sizeof(*uart));
    uart->huart = huart;
    UART_Register(uart);
}

static void Comm_UartRegister(void)
{
    UARTRegister(&uart_instances[SYSTEM_UART5_INDEX], &huart5);
    UARTRegister(&uart_instances[SYSTEM_UART7_INDEX], &huart7);
    UARTRegister(&uart_instances[SYSTEM_UART1_INDEX], &huart1);
    UARTRegister(&uart_instances[SYSTEM_UART2_INDEX], &huart2);
    UARTRegister(&uart_instances[SYSTEM_UART3_INDEX], &huart3);
    UARTRegister(&uart_instances[SYSTEM_UART10_INDEX], &huart10);

    /* 协议解析器挂在 UART 口上，接收在串口中断里直接解析；没绑解析器的口收到的包直接丢弃 */
    Referee_Init(&referee_receiver);
    RC_Init(&rc_receiver);
    //这种全局用的数据直接在这里绑定，各自模块用的自己绑定，类似can电机回调
    (void)RC_Remote_BindUart(&rc_receiver, &uart_instances[SYSTEM_UART5_INDEX]);
    (void)Referee_BindUart(&referee_receiver, &uart_instances[SYSTEM_UART1_INDEX]);
}

void Comm_Register(void)
{
    Comm_CanRegister();
    Comm_UartRegister();
}

void Comm_Start(void)
{
    can_bsp_init();
    for (uint8_t i = 0U; i < SYSTEM_UART_INSTANCE_COUNT; ++i) {
        UART_StartReceiveEx(&uart_instances[i]);
    }
}

/*--------------------------------------数据处理--------------------------------------*/
/* CAN、UART 接收都没有数据处理函数：CAN 中断（CAN_driver.c 的 CANReceive_FromIsr）读空 FIFO 后、
 * UART 中断（uart_driver.c 的 HAL_UARTEx_RxEventCallback）切换缓冲后，直接调绑定的回调 */

/* UART 发送：每个口绑定的发送回调在任务里整理数据并发出，没绑的口跳过 */
void Comm_ProcessUartTransmit(void)
{
    for (uint8_t i = 0U; i < SYSTEM_UART_INSTANCE_COUNT; ++i) {
        UART_DispatchTx(&uart_instances[i]);
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
