#ifndef MOTOR_INIT_H
#define MOTOR_INIT_H

/* 电机层：创建电机实例，电机型号、ID、零点、方向都在 motor_init.c
 * 必须在 Comm_Register 之后调用；电机反馈由使用电机的模块在自己的初始化里绑到 CAN 实例上 */
void Motor_Init(void);

#endif
