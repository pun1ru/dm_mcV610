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
//角色在这里定下，初始化后不再改：本板是自定义控制器，做主机
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
#define ARM_ZERO_J4                   26560U
#define ARM_ZERO_J5                   32676U
#define ARM_ZERO_J6                   65112U
//关节 +q 方向相对编码器计数方向的符号：+1 正向转编码器增大
//只影响观测角的正负
#define ARM_DIRECTION_J0              1
#define ARM_DIRECTION_J1              1
#define ARM_DIRECTION_J2              (-1)
#define ARM_DIRECTION_J3              1
#define ARM_DIRECTION_J4              1
#define ARM_DIRECTION_J5              1
#define ARM_DIRECTION_J6              (-1)
//关节工作区间
#define ARM_LIMIT_J0                  {(-90.0f), 90.0f}
#define ARM_LIMIT_J1                  {(-95.0f), 100.0f}
#define ARM_LIMIT_J2                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J3                  {(-120.0f), 0.0f}
#define ARM_LIMIT_J4                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J5                  {(-60.0f), 90.0f}
#define ARM_LIMIT_J6                  {(-180.0f), 180.0f}
//每行：kp, ki, kd, d_alpha, sum_error_max, output_max, total_output_max，单位 deg -> N*m
#define ARM_GAIN_J0                   {0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.2f}
#define ARM_GAIN_J1                   {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 2.5f, 2.5f}
#define ARM_GAIN_J2                   {0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f}
#define ARM_GAIN_J3                   {0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.6f, 0.6f}
#define ARM_GAIN_J4                   {0.002f, 0.0f, 0.000f, 0.0f, 0.0f, 0.25f, 0.25f}
#define ARM_GAIN_J5                   {0.003f, 0.0f, 0.000f, 0.0f, 0.0f, 0.2f, 0.2f}
#define ARM_GAIN_J6                   {0.002f, 0.0f, 0.001f, 0.0f, 0.0f, 0.05f, 0.05f}

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
//28 维 pi：用户实测拟合值（Host 重力拟合 + 最小范数解），link0 与 link1 的质量项在 Y 里恒为 0
static const float armGravityPi[ARM_GRAVITY_PI_DIM] =
{
    +0.00000f, +0.00000f, -0.00000f, +0.00000f,     //link0 (J0)
    -0.00052f, +0.08218f, -0.00000f, +0.00000f,     //link1 (J1)
    -0.00375f, +0.00012f, -0.08218f, +0.01216f,     //link2 (J2)
    +0.00040f, -0.01262f, -0.00012f, +0.01216f,     //link3 (J3)
    +0.00053f, +0.00012f, +0.01262f, +0.01342f,     //link4 (J4)
    +0.00137f, +0.00458f, -0.00012f, +0.01342f,     //link5 (J5)
    -0.00116f, +0.00064f, -0.00458f, +0.01342f,     //link6 (J6)
};
//12 维基参数 beta = [h2x h2y h3x h3y h4x h4y h5x h5y h6x h6y h7x h7y]，对应 pi 的列 4 5 8 9 12 13 16 17 20 21 24 25
static const float armGravityBeta[ARM_GRAVITY_BETA_DIM] =
{
    -0.00052f, +0.1739178400f,      //h2x, h2y
    -0.00375f, +0.00024f,           //h3x, h3y
    +0.00040f, -0.029266f,          //h4x, h4y
    +0.00053f, +0.00024f,           //h5x, h5y
    +0.00137f, +0.00916f,           //h6x, h6y
    -0.00116f, +0.00064f,           //h7x, h7y
};

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
