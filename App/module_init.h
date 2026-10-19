#ifndef MODULE_INIT_H
#define MODULE_INIT_H

/* 模块层：创建并绑定电机、云台、状态机、协议解析器，配置常量都在 module_init.c
 * 必须在 Comm_Register 之后、Comm_Start 和任务创建之前调用 */
void Module_Init(void);

#endif
