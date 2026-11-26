#include "module_init.h"

#include "system_define.h"
#include "system_instance.h"

//每个模块的配置按外设、通信、电机、状态、参数五层写,没有就不写
/*--------------------------------------监控--------------------------------------*/
static void Module_MonitorInit(void)
{
    const MonitorConfig_t config =
    {
        .comm = {.monitor_task = &MonitorTaskHandle},
        .param =
        {
            .sample_period_ms = TASK_MONITOR_LOOP_PERIOD_MS,
            .task_count = SYSTEM_TASK_COUNT,
            .task_period_ms = systemTaskPeriodMs,
        },
    };
    Monitor_Init(&systemMonitor, &config);
}

/*--------------------------------------七轴臂--------------------------------------*/
//自定义控制器，做主机
#define ARM_ROLE                      ARM_ROLE_MASTER
//改进DH参数表
#define ARM_DH_J0                     {0.0f, 0.0f, 0.0626f, 0.0f}
#define ARM_DH_J1                     {90.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J2                     {90.0f, 0.0f, -0.148f, 0.0f}
#define ARM_DH_J3                     {90.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J4                     {90.0f, 0.0f, 0.1f, 0.0f}
#define ARM_DH_J5                     {90.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J6                     {90.0f, 0.0f, 0.0f, 0.0f}
//根据你定义的坐标系的编码器零点，默认为x轴朝向一致的编码器值
#define ARM_ZERO_J0                   4108U
#define ARM_ZERO_J1                   19268U
#define ARM_ZERO_J2                   6740U
#define ARM_ZERO_J3                   21635U
#define ARM_ZERO_J4                   36500U    // J4/J6 对调后重新标定（2026-11-01）
#define ARM_ZERO_J5                   32676U
#define ARM_ZERO_J6                   52365U    // J4/J6 对调后重新标定（2026-11-01）
//关节 +q 方向相对编码器计数方向的符号：+1 正向转编码器增大
//只影响观测角的正负
#define ARM_DIRECTION_J0              1
#define ARM_DIRECTION_J1              1
#define ARM_DIRECTION_J2              (-1)
#define ARM_DIRECTION_J3              1
#define ARM_DIRECTION_J4              1         // 对调后沿用原 J4 关节的方向，实机未复核
#define ARM_DIRECTION_J5              1
#define ARM_DIRECTION_J6              (-1)      // 对调后沿用原 J6 关节的方向，实机未复核
//关节工作区间
#define ARM_LIMIT_J0                  {(-90.0f), 90.0f}
#define ARM_LIMIT_J1                  {(-20.0f), 175.0f}
#define ARM_LIMIT_J2                  {(-175.0f), 175.0f}
#define ARM_LIMIT_J3                  {(-110.0f), 110.0f}
#define ARM_LIMIT_J4                  {(-175.0f), 175.0f}
#define ARM_LIMIT_J5                  {(-120.0f), 70.0f}
#define ARM_LIMIT_J6                  {(-175.0f), 175.0f}
//关节速度限制 待加

//关节 PID 参数，单位 deg -> N*m，阻抗用PD
//每行：kp, ki, kd, d_alpha, sum_error_max, output_max, total_output_max，单位 deg -> N*m
#define ARM_GAIN_J0                   {0.2f, 0.0f, 2.0f, 0.90f, 0.0f, 1.5f, 1.5f}
#define ARM_GAIN_J1                   {1.0f, 0.0f, 5.0f, 0.0f, 0.0f, 2.5f, 2.5f}
#define ARM_GAIN_J2                   {0.06f, 0.0f, 0.6f, 0.0f, 0.0f, 0.5f, 0.5f}
#define ARM_GAIN_J3                   {0.3f, 0.0f, 3.0f, 0.0f, 0.0f, 0.6f, 0.6f}
#define ARM_GAIN_J4                   {0.01f, 0.0f, 0.1f, 0.0f, 0.0f, 0.25f, 0.25f}
#define ARM_GAIN_J5                   {0.01f, 0.0f, 0.1f, 0.0f, 0.0f, 0.2f, 0.2f}
#define ARM_GAIN_J6                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.05f, 0.05f}

//重力补偿
#define ARM_GRAVITY_ENABLE            1U        //0：不计算，也不会有任何前馈
#define ARM_GRAVITY_USE_BETA          1U        //1：用 12 维 beta（正式控制）；0：用 28 维 pi（核对用）
//前馈符号相对 dU/dq：+1 同号；若实测悬停时关节往重力方向塌，把它改成 -1（只在 Arm_GravityUpdate 里乘一次）
#define ARM_GRAVITY_SIGN              1
#define ARM_GRAVITY_G                 9.81f
//各轴单独开关：1 = 该轴叠加重力前馈
#define ARM_GRAVITY_EN_J0             0U
#define ARM_GRAVITY_EN_J1             1U
#define ARM_GRAVITY_EN_J2             1U
#define ARM_GRAVITY_EN_J3             1U
#define ARM_GRAVITY_EN_J4             1U
#define ARM_GRAVITY_EN_J5             1U
#define ARM_GRAVITY_EN_J6             1U
#define ARM_GRAVITY_JOINT_MASK        ((ARM_GRAVITY_EN_J0 << 0) | (ARM_GRAVITY_EN_J1 << 1) | (ARM_GRAVITY_EN_J2 << 2) | \
                                       (ARM_GRAVITY_EN_J3 << 3) | (ARM_GRAVITY_EN_J4 << 4) | (ARM_GRAVITY_EN_J5 << 5) | \
                                       (ARM_GRAVITY_EN_J6 << 6))

//28 维 pi
static const float armGravityPi[ARM_GRAVITY_PI_DIM] =
{
    +0.0000000000f, +0.0000000000f, +0.0000000000f, +0.0000000000f,  //link0 (J0)
    -0.0004752478f, +0.0819734718f, +0.0000000000f, +0.0000000000f,  //link1 (J1)
    -0.0054564876f, +0.0000241475f, -0.0819734718f, +0.0121320738f,  //link2 (J2)
    +0.0006196353f, -0.0130415714f, -0.0000241475f, +0.0121320738f,  //link3 (J3)
    -0.0003754912f, +0.0002746489f, +0.0130415714f, +0.0134362310f,  //link4 (J4)
    +0.0004932583f, +0.0030319493f, -0.0002746489f, +0.0134362310f,  //link5 (J5)
    -0.0001107794f, +0.0001979887f, -0.0030319493f, +0.0134362310f,  //link6 (J6)
};
//12 维基参数 beta = [h2x h2y h3x h3y h4x h4y h5x h5y h6x h6y h7x h7y]
//来源：gravity_20261126_210025.csv，按上方嵌入式 DH 和保存的逐关节噪声做加权拟合后投影
static const float armGravityBeta[ARM_GRAVITY_BETA_DIM] =
{
    -0.0004752478f, +0.1735037239f,  //h2x, h2y
    -0.0054564876f, +0.0000482951f,  //h3x, h3y
    +0.0006196353f, -0.0301140122f,  //h4x, h4y
    -0.0003754912f, +0.0005492978f,  //h5x, h5y
    +0.0004932583f, +0.0060638986f,  //h6x, h6y
    -0.0001107794f, +0.0001979887f,  //h7x, h7y
};

//数值逆解（含义见 ik_numeric.h，与 Simulation/ArmSim 的 NumericIkParam 一一对应）
//首次上板取保守值：步长和领先量都比仿真默认（0.2 / 5.0）小一半以上；跟踪正常后再逐步放大
#define ARM_IK_DAMPING                0.03      //λ
#define ARM_IK_ORIENTATION_LENGTH_M   0.1       //姿态误差折算成位置误差的等效长度
#define ARM_IK_NULLSPACE_GAIN         0.02      //零空间向参考构型拉回的比例，0 = 关闭
#define ARM_IK_MAX_STEP_DEG           0.1       //每关节每周期最大转角，2 ms 周期下约 50 °/s
#define ARM_IK_MAX_POS_ERR_M          0.01
#define ARM_IK_MAX_ROT_ERR_RAD        0.05
#define ARM_IK_WEIGHT_FLOOR           1e-3
#define ARM_IK_CONVERGED_POS_M        5e-4
#define ARM_IK_CONVERGED_ROT_RAD      2e-3
#define ARM_IK_MAX_LIMIT_PASSES       4
#define ARM_IK_MAX_LEAD_DEG           2.0       //指令领先实测角的上限
#define ARM_IK_WEIGHT                 {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}  //关节权重，越大越不愿动

//J0~J3 在 CAN1，J4~J6 在 CAN2，反馈在 Arm_Init 里绑到对应 CAN
static void Module_ArmInit(void)
{
    CanInstance_t *can1 = &can_instances[SYSTEM_CAN1_INDEX];
    CanInstance_t *can2 = &can_instances[SYSTEM_CAN2_INDEX];

    const ArmConfig_t config =
    {
        .periph =
        {
            .can =
            {
                [ARM_J0] = can1,
                [ARM_J1] = can1,
                [ARM_J2] = can1,
                [ARM_J3] = can1,
                [ARM_J4] = can2,
                [ARM_J5] = can2,
                [ARM_J6] = can2,
            },
        },
        .motor =
        {
            .joint =
            {
                [ARM_J0] = &armJ0Motor.base,
                [ARM_J1] = &armJ1Motor.base,
                [ARM_J2] = &armJ2Motor.base,
                [ARM_J3] = &armJ3Motor.base,
                [ARM_J4] = &armJ4Motor.base,
                [ARM_J5] = &armJ5Motor.base,
                [ARM_J6] = &armJ6Motor.base,
            },
        },
        .state = {.role = ARM_ROLE, .init_state = ARM_PROTECTED},
        .param =
        {
            .dh =
            {
                [ARM_J0] = ARM_DH_J0,
                [ARM_J1] = ARM_DH_J1,
                [ARM_J2] = ARM_DH_J2,
                [ARM_J3] = ARM_DH_J3,
                [ARM_J4] = ARM_DH_J4,
                [ARM_J5] = ARM_DH_J5,
                [ARM_J6] = ARM_DH_J6,
            },
            .zero_point =
            {
                [ARM_J0] = ARM_ZERO_J0,
                [ARM_J1] = ARM_ZERO_J1,
                [ARM_J2] = ARM_ZERO_J2,
                [ARM_J3] = ARM_ZERO_J3,
                [ARM_J4] = ARM_ZERO_J4,
                [ARM_J5] = ARM_ZERO_J5,
                [ARM_J6] = ARM_ZERO_J6,
            },
            .direction =
            {
                [ARM_J0] = ARM_DIRECTION_J0,
                [ARM_J1] = ARM_DIRECTION_J1,
                [ARM_J2] = ARM_DIRECTION_J2,
                [ARM_J3] = ARM_DIRECTION_J3,
                [ARM_J4] = ARM_DIRECTION_J4,
                [ARM_J5] = ARM_DIRECTION_J5,
                [ARM_J6] = ARM_DIRECTION_J6,
            },
            .limit =
            {
                [ARM_J0] = ARM_LIMIT_J0,
                [ARM_J1] = ARM_LIMIT_J1,
                [ARM_J2] = ARM_LIMIT_J2,
                [ARM_J3] = ARM_LIMIT_J3,
                [ARM_J4] = ARM_LIMIT_J4,
                [ARM_J5] = ARM_LIMIT_J5,
                [ARM_J6] = ARM_LIMIT_J6,
            },
            .gain =
            {
                [ARM_J0] = ARM_GAIN_J0,
                [ARM_J1] = ARM_GAIN_J1,
                [ARM_J2] = ARM_GAIN_J2,
                [ARM_J3] = ARM_GAIN_J3,
                [ARM_J4] = ARM_GAIN_J4,
                [ARM_J5] = ARM_GAIN_J5,
                [ARM_J6] = ARM_GAIN_J6,
            },
            .gravity =
            {
                .enable = ARM_GRAVITY_ENABLE,
                .use_beta = ARM_GRAVITY_USE_BETA,
                .sign = ARM_GRAVITY_SIGN,
                .joint_mask = ARM_GRAVITY_JOINT_MASK,
                .g = ARM_GRAVITY_G,
                .pi = armGravityPi,
                .beta = armGravityBeta,
            },
            .ik =
            {
                .numeric =
                {
                    .damping = ARM_IK_DAMPING,
                    .orientation_length_m = ARM_IK_ORIENTATION_LENGTH_M,
                    .nullspace_gain = ARM_IK_NULLSPACE_GAIN,
                    .max_step_deg = ARM_IK_MAX_STEP_DEG,
                    .max_pos_err_m = ARM_IK_MAX_POS_ERR_M,
                    .max_rot_err_rad = ARM_IK_MAX_ROT_ERR_RAD,
                    .weight_floor = ARM_IK_WEIGHT_FLOOR,
                    .converged_pos_m = ARM_IK_CONVERGED_POS_M,
                    .converged_rot_rad = ARM_IK_CONVERGED_ROT_RAD,
                    .max_limit_passes = ARM_IK_MAX_LIMIT_PASSES,
                    .max_lead_deg = ARM_IK_MAX_LEAD_DEG,
                },
                .weight = ARM_IK_WEIGHT,
            },
        },
    };
    Arm_Init(&armInstance, &config);
}

/*--------------------------------------上位机--------------------------------------*/
//收发共用 UART10（921600）：发送任务周期调 Host_Transmit，接收中断只校验并入队，
//命令执行和应答都在任务上下文完成；Host 直接访问 armInstance 的上位机目标接口
static void Module_HostInit(void)
{
    const HostConfig_t config =
    {
        .periph = {.uart = &uart_instances[SYSTEM_UART10_INDEX]},
        .param = {.state_divider = HOST_STATE_DIVIDER},
    };

    Host_Init(&hostInstance, &config, &armInstance);
    (void)Host_BindUart(&hostInstance);
}

/*--------------------------------------入口--------------------------------------*/
void Module_Init(void)
{
    Module_MonitorInit();
    Module_ArmInit();
    Module_HostInit();
}
