#include "motor_init.h"

#include "Motor.h"
#include "system_instance.h"
//宏定义，电机绑定的can总线，电机型号，电机ID，电机反馈ID，电机零点，电机方向，控制模式
//inverse 只影响力矩/电流指令方向（各驱动内部取反），不改变反馈，也不影响观测角
//目前只有 J2 和 J6 置 true：测力矩方向时它们的 +指令方向与实际关节 +q 相反
//关节观测角的正负由 module_init.c 里 ArmConfig.param.direction 决定，见 arm_kinematics 的 Arm_UpdateJointAngle
//零点按 2026-10-28 上板标定的机械零位填写；注意 Arm_Init 会用 ArmConfig.param.zero_point 覆盖这里，两处必须一致
//J0~J3 在 CAN1，J4~J6 在 CAN2
#define ARM_CAN1_INDEX                SYSTEM_CAN1_INDEX
#define ARM_CAN2_INDEX                SYSTEM_CAN2_INDEX
//GM6020 电流环力矩控制：ID 1~4 发 0x1FE，ID 5~7 发 0x2FE，反馈 ID 都是 0x204 + ID
//注意：0x1FE/0x2FE 只有在电机的"电流环"开关打开后才生效（调参软件里设，固件 >= 1.0.11.2），
//开关关着时电机只认电压帧 0x1FF/0x2FF，电流帧会被直接忽略
#define ARM_RM_TX_ID_LOW              0x1FEU
#define ARM_RM_TX_ID_HIGH             0x2FEU
#define ARM_RM_MOTOR_TYPE             RM_GM6020
#define ARM_RM_MOTOR_MODE             RM_CURRENT
#define ARM_RM_CONTROL_MODE           MODE_TORQUE
//DM 的 zero_point 是 MIT 位置映射下的原始值（0~65535 线性映射到 P_MIN~P_MAX），各关节不同，见下面 J1/J3
#define ARM_DM_CONTROL_MODE           MODE_MIT
//LK 力矩模式，N*m 由驱动换成 iq
#define ARM_LK_CONTROL_MODE           MODE_TORQUE
//MF4005 有 25T 和 40T 两种绕组，扭矩常数不同，装的是哪种需要确认
#define ARM_J4_MOTOR_MODEL            LK_MF4005_25T
//J0 GM6020，拨码 ID 3，发送 0x1FE，反馈 0x207
#define ARM_J0_MOTOR_ID               3U
#define ARM_J0_ZERO_POINT             4108U     // 实测机械零位，编码器 0~8191
#define ARM_J0_INVERSE                false
//J1 DM-J4310，MIT 模式发送 ID 即电机 CAN ID，反馈 ID 是电机的 MST_ID
#define ARM_J1_MOTOR_ID               0x01U
#define ARM_J1_FEEDBACK_ID            0x11U
#define ARM_J1_ZERO_POINT             19268U    // 实测机械零位，MIT 位置原始值 0~65535
#define ARM_J1_INVERSE                false
//J2 GM6020，拨码 ID 7，发送 0x2FE，反馈 0x20B
#define ARM_J2_MOTOR_ID               7U
#define ARM_J2_ZERO_POINT             6740U     // 实测机械零位，编码器 0~8191
#define ARM_J2_INVERSE                true
//J3 DM-J4310，反馈 ID 是电机的 MST_ID
#define ARM_J3_MOTOR_ID               0x02U
#define ARM_J3_FEEDBACK_ID            0x12U
#define ARM_J3_ZERO_POINT             21635U    // 实测机械零位，MIT 位置原始值 0~65535
#define ARM_J3_INVERSE                false
//J4 MF4005，LK 指令 ID 为 0x140 + ID，反馈 ID 同值，这里是 0x142
#define ARM_J4_MOTOR_ID               2U
#define ARM_J4_FEEDBACK_ID            (LK_CAN_COMMAND_ID_BASE + ARM_J4_MOTOR_ID)
#define ARM_J4_ZERO_POINT             26560U    // 实测机械零位，编码器 0~65535
#define ARM_J4_INVERSE                false
//J5 MF4015，0x141
#define ARM_J5_MOTOR_ID               1U
#define ARM_J5_FEEDBACK_ID            (LK_CAN_COMMAND_ID_BASE + ARM_J5_MOTOR_ID)
#define ARM_J5_ZERO_POINT             32676U    // 实测机械零位，编码器 0~65535
#define ARM_J5_INVERSE                false
//J6 MF4015，0x146
#define ARM_J6_MOTOR_ID               6U
#define ARM_J6_FEEDBACK_ID            (LK_CAN_COMMAND_ID_BASE + ARM_J6_MOTOR_ID)
#define ARM_J6_ZERO_POINT             65112U    // 实测机械零位，编码器 0~65535
#define ARM_J6_INVERSE                true

static RMMotorConfig_t Motor_RmConfig(uint8_t motor_id, uint16_t zero_point, bool inverse)
{
    const RMMotorConfig_t config =
    {
        .motor_type = ARM_RM_MOTOR_TYPE,
        .mode = ARM_RM_MOTOR_MODE,
        .motor_id = motor_id,
        .zero_point = zero_point,
        .inverse = inverse,
        .control_mode = ARM_RM_CONTROL_MODE,
    };
    return config;
}

static DMMotorConfig_t Motor_DmConfig(uint16_t motor_id, uint16_t feedback_id,
                                      uint16_t zero_point, bool inverse)
{
    const DMMotorConfig_t config =
    {
        .motor_type = DM_4310,
        .motor_id = motor_id,
        .feedback_id = feedback_id,
        .zero_point = zero_point,
        .inverse = inverse,
        .control_mode = ARM_DM_CONTROL_MODE,
    };
    return config;
}

static LKMotorConfig_t Motor_LkConfig(LKMotorModel_t model, uint8_t motor_id,
                                      uint16_t feedback_id, uint16_t zero_point, bool inverse)
{
    const LKMotorConfig_t config =
    {
        .model = model,
        .motor_id = motor_id,
        .feedback_id = feedback_id,
        .zero_point = zero_point,
        .inverse = inverse,
        .control_mode = ARM_LK_CONTROL_MODE,
    };
    return config;
}

void Motor_Init(void)
{
    CanInstance_t *can1 = &can_instances[ARM_CAN1_INDEX];
    CanInstance_t *can2 = &can_instances[ARM_CAN2_INDEX];

    const RMMotorConfig_t j0_config = Motor_RmConfig(ARM_J0_MOTOR_ID, ARM_J0_ZERO_POINT,
                                                     ARM_J0_INVERSE);
    const DMMotorConfig_t j1_config = Motor_DmConfig(ARM_J1_MOTOR_ID, ARM_J1_FEEDBACK_ID,
                                                     ARM_J1_ZERO_POINT, ARM_J1_INVERSE);
    const RMMotorConfig_t j2_config = Motor_RmConfig(ARM_J2_MOTOR_ID, ARM_J2_ZERO_POINT,
                                                     ARM_J2_INVERSE);
    const DMMotorConfig_t j3_config = Motor_DmConfig(ARM_J3_MOTOR_ID, ARM_J3_FEEDBACK_ID,
                                                     ARM_J3_ZERO_POINT, ARM_J3_INVERSE);
    const LKMotorConfig_t j4_config = Motor_LkConfig(ARM_J4_MOTOR_MODEL, ARM_J4_MOTOR_ID,
                                                     ARM_J4_FEEDBACK_ID, ARM_J4_ZERO_POINT, ARM_J4_INVERSE);
    const LKMotorConfig_t j5_config = Motor_LkConfig(LK_MF4015, ARM_J5_MOTOR_ID,
                                                     ARM_J5_FEEDBACK_ID, ARM_J5_ZERO_POINT, ARM_J5_INVERSE);
    const LKMotorConfig_t j6_config = Motor_LkConfig(LK_MF4015, ARM_J6_MOTOR_ID,
                                                     ARM_J6_FEEDBACK_ID, ARM_J6_ZERO_POINT, ARM_J6_INVERSE);

    //RM电机组声明，J0 和 J2 的 ID 不在同一段，各占一个发送帧
    RMMotorGroup_Init(&armRmGroupLow, can1->can, ARM_RM_TX_ID_LOW);
    RMMotorGroup_Init(&armRmGroupHigh, can1->can, ARM_RM_TX_ID_HIGH);
    RMMotor_Init(&armJ0Motor, &armRmGroupLow, &j0_config);
    DMMotor_Init(&armJ1Motor, can1->can, &j1_config);
    RMMotor_Init(&armJ2Motor, &armRmGroupHigh, &j2_config);
    DMMotor_Init(&armJ3Motor, can1->can, &j3_config);
    LKMotor_Init(&armJ4Motor, can2->can, &j4_config);
    LKMotor_Init(&armJ5Motor, can2->can, &j5_config);
    LKMotor_Init(&armJ6Motor, can2->can, &j6_config);
    //反馈回调由七轴臂模块在 Arm_Init 里绑到 CAN 实例，这里只创建电机
}
