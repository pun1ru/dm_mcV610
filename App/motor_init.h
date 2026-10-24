#ifndef MOTOR_INIT_H
#define MOTOR_INIT_H

/* 电机层：创建电机实例，电机型号、ID、零点、方向都在 motor_init.c
 * 必须在 Comm_Register 之后、Comm_Start 之前调用；这里只创建电机，反馈回调由各模块在自己的 Init 里绑到 CAN 实例上 */
void Motor_Init(void);

#endif
