#include "motor_init.h"

#include "Motor.h"
#include "system_instance.h"
//宏定义，电机绑定的can总线，电机型号，电机ID，电机反馈ID，电机零点，电机方向，控制模式
//注意这里的inverse只翻了力矩方向，就是你定义的正力矩对应的正确方向，观测方向由模块自己决定是不是使用
//gimbal yaw
#define GIMBAL_YAW_CAN_INDEX          SYSTEM_CAN3_INDEX
#define GIMBAL_YAW_MOTOR_TYPE         DM_6006
#define GIMBAL_YAW_MOTOR_ID           0x07U
#define GIMBAL_YAW_FEEDBACK_ID        0x017U
#define GIMBAL_YAW_ZERO_POINT         59274U
#define GIMBAL_YAW_INVERSE            false
#define GIMBAL_YAW_CONTROL_MODE       MODE_MIT
//gimbal pitch
#define GIMBAL_PITCH_CAN_INDEX        SYSTEM_CAN1_INDEX
#define GIMBAL_PITCH_MOTOR_MODEL      LK_MG6012E_I8
#define GIMBAL_PITCH_MOTOR_ID         0x01U
#define GIMBAL_PITCH_FEEDBACK_ID      0x141U
#define GIMBAL_PITCH_ZERO_POINT       56183U
#define GIMBAL_PITCH_INVERSE          false
#define GIMBAL_PITCH_CONTROL_MODE     MODE_TORQUE
//shooter 摩擦轮，3个C620共用0x200一帧
#define SHOOTER_FRIC_CAN_INDEX        SYSTEM_CAN2_INDEX
#define SHOOTER_FRIC_TX_ID            0x200U
#define SHOOTER_FRIC_MOTOR_TYPE       RM_C620
#define SHOOTER_FRIC_MOTOR_MODE       RM_CURRENT
#define SHOOTER_FRIC_CONTROL_MODE     MODE_CURRENT
#define SHOOTER_FRIC_LEFT_MOTOR_ID    1U
#define SHOOTER_FRIC_LEFT_INVERSE     false
#define SHOOTER_FRIC_RIGHT_MOTOR_ID   2U
#define SHOOTER_FRIC_RIGHT_INVERSE    true
#define SHOOTER_FRIC_UP_MOTOR_ID      3U
#define SHOOTER_FRIC_UP_INVERSE       false

void Motor_Init(void)
{
    const DMMotorConfig_t gimbal_yaw_config =
    {
        .motor_type = GIMBAL_YAW_MOTOR_TYPE,
        .motor_id = GIMBAL_YAW_MOTOR_ID,
        .feedback_id = GIMBAL_YAW_FEEDBACK_ID,
        .zero_point = GIMBAL_YAW_ZERO_POINT,
        .inverse = GIMBAL_YAW_INVERSE,
        .control_mode = GIMBAL_YAW_CONTROL_MODE,
    };
    const LKMotorConfig_t gimbal_pitch_config =
    {
        .model = GIMBAL_PITCH_MOTOR_MODEL,
        .motor_id = GIMBAL_PITCH_MOTOR_ID,
        .feedback_id = GIMBAL_PITCH_FEEDBACK_ID,
        .zero_point = GIMBAL_PITCH_ZERO_POINT,
        .inverse = GIMBAL_PITCH_INVERSE,
        .control_mode = GIMBAL_PITCH_CONTROL_MODE,
    };
    const RMMotorConfig_t shooter_fric_config[SHOOTER_FRIC_COUNT] =
    {
        [SHOOTER_FRIC_LEFT] =
        {
            .motor_type = SHOOTER_FRIC_MOTOR_TYPE,
            .mode = SHOOTER_FRIC_MOTOR_MODE,
            .motor_id = SHOOTER_FRIC_LEFT_MOTOR_ID,
            .inverse = SHOOTER_FRIC_LEFT_INVERSE,
            .control_mode = SHOOTER_FRIC_CONTROL_MODE,
        },
        [SHOOTER_FRIC_RIGHT] =
        {
            .motor_type = SHOOTER_FRIC_MOTOR_TYPE,
            .mode = SHOOTER_FRIC_MOTOR_MODE,
            .motor_id = SHOOTER_FRIC_RIGHT_MOTOR_ID,
            .inverse = SHOOTER_FRIC_RIGHT_INVERSE,
            .control_mode = SHOOTER_FRIC_CONTROL_MODE,
        },
        [SHOOTER_FRIC_UP] =
        {
            .motor_type = SHOOTER_FRIC_MOTOR_TYPE,
            .mode = SHOOTER_FRIC_MOTOR_MODE,
            .motor_id = SHOOTER_FRIC_UP_MOTOR_ID,
            .inverse = SHOOTER_FRIC_UP_INVERSE,
            .control_mode = SHOOTER_FRIC_CONTROL_MODE,
        },
    };

    DMMotor_Init(&gimbalYawMotor, can_instances[GIMBAL_YAW_CAN_INDEX].can, &gimbal_yaw_config);
    LKMotor_Init(&gimbalPitchMotor, can_instances[GIMBAL_PITCH_CAN_INDEX].can, &gimbal_pitch_config);
    //RM电机组声明
    RMMotorGroup_Init(&shooterFricGroup, can_instances[SHOOTER_FRIC_CAN_INDEX].can, SHOOTER_FRIC_TX_ID);
    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        RMMotor_Init(&shooterFricMotor[i], &shooterFricGroup, &shooter_fric_config[i]);
    }
}
