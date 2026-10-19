#ifndef IPC_INIT_H
#define IPC_INIT_H

/* 任务间通信层：任务之间传数据的队列，必须在创建任务之前调用
 * CAN、UART 自带的队列和事件组归 comm_init 管 */
void Ipc_Init(void);

#endif
