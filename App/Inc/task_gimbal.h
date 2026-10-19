#ifndef GIMBAL_TASK_H
#define GIMBAL_TASK_H

/* 云台电机、绑定表和云台对象由 module_init 创建，这里只有任务本体 */
void GimbalTask(void *argument);

#endif
