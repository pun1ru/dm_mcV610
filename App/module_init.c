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
//DH 表由 Simulation 里的 robot.urdf 反推（改进 DH），只对应这台自定义控制器，已和 MuJoCo 加载的模型连乘核对，J0 坐标系取在 J0 轴原点
//零位是 x 轴对齐的竖直伸直姿态，各行 theta_offset 均为 0，q=0 即该姿态
//每行：alpha_deg, a_m, d_m, theta_offset_deg
#define ARM_DH_J0                     {0.0f, 0.0f, 0.0f, 0.0f}
#define ARM_DH_J1                     {90.0f, 0.0f, -0.004f, 0.0f}
#define ARM_DH_J2                     {90.0f, 0.0f, -0.148f, 0.0f}
#define ARM_DH_J3                     {90.0f, 0.0f, -0.0025f, 0.0f}
#define ARM_DH_J4                     {90.0f, 0.0f, 0.1f, 0.0f}
#define ARM_DH_J5                     {90.0f, 0.0f, -0.001f, 0.0f}
#define ARM_DH_J6                     {90.0f, 0.0f, 0.0f, 0.0f}
//各关节机械零位对应的原始编码器值，暂定值，上板用 Arm_CalibrateZero 标定后抄回这里
//DM 的零点是 MIT 位置映射下的原始值，32768 约对应 0 rad
#define ARM_ZERO_DM_DEFAULT           32768U
#define ARM_ZERO_J0                   0U
#define ARM_ZERO_J1                   ARM_ZERO_DM_DEFAULT
#define ARM_ZERO_J2                   0U
#define ARM_ZERO_J3                   ARM_ZERO_DM_DEFAULT
#define ARM_ZERO_J4                   0U
#define ARM_ZERO_J5                   0U
#define ARM_ZERO_J6                   0U

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
