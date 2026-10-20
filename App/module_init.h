#ifndef MODULE_INIT_H
#define MODULE_INIT_H

/* 模块层：监控、INS、云台、状态机，配置分层写在 module_init.c
 * 必须在 Comm_Register、Motor_Init 之后，Comm_Start 和任务创建之前调用；
 * 云台在这里把电机反馈绑到 CAN 实例上 */
void Module_Init(void);

#endif
