#ifndef COMM_INIT_H
#define COMM_INIT_H

/* 通信层：CAN、UART 的登记、启动、回调绑定和发送数据处理都在 comm_init.c
 * 初始化分两步，中间夹着电机、模块和任务创建：
 * Comm_Register：登记 CAN、UART，建 CAN 发送队列，初始化协议解析器并绑定 UART 接收回调；只登记，不启动
 * Comm_Start：打开 CAN 和 UART 接收，必须在电机和模块把 CAN 反馈绑好之后调用 */
void Comm_Register(void);
void Comm_Start(void);

/* 由任务循环调用，不含循环和心跳
 * Comm_ProcessTransmit 自己阻塞等发送队列
 * CAN、UART 接收不经任务，在中断里直接分发给绑定的回调 */
void Comm_ProcessTransmit(void);

#endif
