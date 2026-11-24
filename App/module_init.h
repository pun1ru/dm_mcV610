#ifndef MODULE_INIT_H
#define MODULE_INIT_H

/* 模块层入口：通用模板默认只初始化 Monitor。
 * 应用工程在这里添加自己的模块实例，但仍需保持实例创建与任务创建分离。 */
void Module_Init(void);

#endif
