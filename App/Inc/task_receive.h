#ifndef TASK_RECEIVE_H
#define TASK_RECEIVE_H

#include "uart_driver.h"
#include "fdcan.h"

/* 接收回调，由 comm_init 在登记通信口时绑定 */
void TaskReceive_RCInput(const uint8_t *data, uint16_t size);
void TaskReceive_RCError(void);
void TaskReceive_RefereeInput(const uint8_t *data, uint16_t size);
void TaskReceive_RefereeError(void);
void TaskReceive_CAN1Callback(FDCAN_HandleTypeDef *can, uint16_t id,
                              const uint8_t *data, uint8_t length);
void TaskReceive_CAN2Callback(FDCAN_HandleTypeDef *can, uint16_t id,
                              const uint8_t *data, uint8_t length);
void TaskReceive_CAN3Callback(FDCAN_HandleTypeDef *can, uint16_t id,
                              const uint8_t *data, uint8_t length);
void ReceiveTask(void *argument);
void CANReceive_Callback(FDCAN_HandleTypeDef *can, uint16_t id,
                         const uint8_t *data, uint8_t length);
void UARTReceive_Callback(UartInstance_t *uart, const uint8_t *data,
                          uint16_t length);

#endif
