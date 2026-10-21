#include "motor_init.h"

#include "system_instance.h"

#define GIMBAL_YAW_CAN_INDEX          SYSTEM_CAN3_INDEX
#define GIMBAL_YAW_MOTOR_TYPE         DM_6006
#define GIMBAL_YAW_MOTOR_ID           0x07U
#define GIMBAL_YAW_FEEDBACK_ID        0x017U
#define GIMBAL_YAW_ZERO_POINT         59274U
#define GIMBAL_PITCH_CAN_INDEX        SYSTEM_CAN1_INDEX
#define GIMBAL_PITCH_MOTOR_MODEL      LK_MG6012E_I8
#define GIMBAL_PITCH_MOTOR_ID         0x01U
#define GIMBAL_PITCH_FEEDBACK_ID      0x141U
#define GIMBAL_PITCH_ZERO_POINT       56183U

/* 电机方向在电机初始化配置里给：驱动取反指令，云台估计层把反馈折算到关节方向 */
#define GIMBAL_YAW_INVERSE            false
#define GIMBAL_PITCH_INVERSE          true

void Motor_Init(void)
{
    const MotorConfig_t yaw_config =
    {
        .motor_type = GIMBAL_YAW_MOTOR_TYPE,
        .motor_id = GIMBAL_YAW_MOTOR_ID,
        .feedback_id = GIMBAL_YAW_FEEDBACK_ID,
        .zero_point = GIMBAL_YAW_ZERO_POINT,
        .inverse = GIMBAL_YAW_INVERSE,
    };
    const LKMotorConfig_t pitch_config =
    {
        .model = GIMBAL_PITCH_MOTOR_MODEL,
        .motor_id = GIMBAL_PITCH_MOTOR_ID,
        .feedback_id = GIMBAL_PITCH_FEEDBACK_ID,
        .zero_point = GIMBAL_PITCH_ZERO_POINT,
        .inverse = GIMBAL_PITCH_INVERSE,
    };

    DMMotor_Init(&gimbalYawMotor, can_instances[GIMBAL_YAW_CAN_INDEX].can, &yaw_config);
    LKMotor_Init(&gimbalPitchMotor, can_instances[GIMBAL_PITCH_CAN_INDEX].can, &pitch_config);

    /* 控制模式零值是位置模式：yaw 走 MIT 力矩字段，pitch 同 hero_up 走 iq 电流 */
    gimbalYawMotor.base.motorcmd.control_mode = MODE_MIT;
    gimbalPitchMotor.base.motorcmd.control_mode = MODE_CURRENT;
}
