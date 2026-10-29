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
//DH 表取 Simulation/mujoco/dh_params.md 第 7.1 节的「理想版」改进 DH：
//原表第 2/4/6 行的 4.000 / 2.500 / 1.000 mm 小偏置按 0 处理，只剩 62.6 / -148.0 / +100.0 mm 三段轴向长度
//每行：alpha_deg, a_m, d_m, theta_offset_deg；θ 偏置全为 0，q = 0 即全伸直位形
//基座系取 O_0（J0 轴原点下方 62.6 mm），与 robot.xml 世界系一致，所以 J0 行带 d = 0.0626
#define ARM_DH_J0                     {0.0f, 0.0f, 0.0626f, 0.0f}
#define ARM_DH_J1                     {90.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J2                     {90.0f, 0.0f, -0.148f, 0.0f}
#define ARM_DH_J3                     {90.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J4                     {90.0f, 0.0f, 0.1f, 0.0f}
#define ARM_DH_J5                     {90.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J6                     {90.0f, 0.0f, 0.0f, 0.0f}
//各关节机械零位对应的原始编码器值，2026-10-28 上板实测；Arm_Init 用它覆盖电机初始化里的零点
//DM 的零点是 MIT 位置映射下的原始值，GM6020 是 0~8191，LK 是 0~65535
#define ARM_ZERO_J0                   4108U
#define ARM_ZERO_J1                   19268U
#define ARM_ZERO_J2                   6740U
#define ARM_ZERO_J3                   21635U
#define ARM_ZERO_J4                   26560U
#define ARM_ZERO_J5                   32676U
#define ARM_ZERO_J6                   65112U
//关节 +q 方向相对编码器计数方向的符号：+1 正向转编码器增大，-1 正向转编码器减小（2026-10-28 实测）
//只影响观测角的正负，不影响力矩指令方向
#define ARM_DIRECTION_J0              1
#define ARM_DIRECTION_J1              1
#define ARM_DIRECTION_J2              (-1)
#define ARM_DIRECTION_J3              1
#define ARM_DIRECTION_J4              1
#define ARM_DIRECTION_J5              1
#define ARM_DIRECTION_J6              (-1)
//关节工作区间，观测角口径 deg（-180, 180]，min 必须小于 max
//只在非保护模式生效：使能后目标超出区间就截断到边界，保护模式不读限位、一律发零
//当前先给整圈占位，等逐关节实测机械行程后再收紧
#define ARM_LIMIT_J0                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J1                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J2                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J3                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J4                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J5                  {(-180.0f), 180.0f}
#define ARM_LIMIT_J6                  {(-180.0f), 180.0f}
//角度环增益，暂时全填 0：PID 输出恒为 0，使能后电机不出力
//每行：kp, ki, kd, d_alpha, sum_error_max, output_max，单位 deg -> N*m
//注意 output_max 是力矩限幅，调参时忘了填就一直把输出截成 0
#define ARM_GAIN_J0                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_GAIN_J1                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_GAIN_J2                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_GAIN_J3                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_GAIN_J4                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_GAIN_J5                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_GAIN_J6                   {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}

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
        },
    };
    Arm_Init(&armInstance, &config);
}

/*--------------------------------------入口--------------------------------------*/
void Module_Init(void)
{
    Module_MonitorInit();
    Module_ArmInit();
}
