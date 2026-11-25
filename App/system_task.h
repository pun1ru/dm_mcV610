#ifndef SYSTEM_TASK_H
#define SYSTEM_TASK_H

/* 通用模板只创建监控、CAN 发送和 UART 发送三个内核任务。
 * 应用模块任务应在自己的应用中显式创建并注册 monitor 配置，
 * 每轮业务完成后用对应的 MonitorHeartbeat_t 令牌上报心跳。 */
void MonitorTask(void *argument);
void CanTransmitTask(void *argument);
void UartTransmitTask(void *argument);

#endif
