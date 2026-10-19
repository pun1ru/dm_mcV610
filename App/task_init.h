#ifndef TASK_INIT_H
#define TASK_INIT_H

/* 整板初始化入口，按层依次：基础、队列、通信口登记、模块、任务、通信启动 */
void Board_Init(void);
void InitTask(void *argument);

#endif
