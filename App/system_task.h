#ifndef SYSTEM_TASK_H
#define SYSTEM_TASK_H

/* 所有任务都在 system_task.c，每个任务只有循环：取数据、调用处理函数、发心跳、延时
 * 初始化、回调定义和数据处理都在对应的 init 文件里 */

/* 基础任务 */
void MonitorTask(void *argument);
void CanTransmitTask(void *argument);
void UartTransmitTask(void *argument);

/* 模块任务 */
void ArmTask(void *argument);

#endif
