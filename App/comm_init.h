#ifndef COMM_INIT_H
#define COMM_INIT_H

/* 通信层分两步，中间夹着模块初始化：
 * Comm_Register：登记 CAN、UART，建它们的队列和事件组，并把接收回调绑上去；只登记，不启动
 * Comm_Start：打开 CAN 和 UART 接收，必须在回调、绑定表、任务都就位之后调用 */
void Comm_Register(void);
void Comm_Start(void);

#endif
