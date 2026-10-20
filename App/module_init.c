#include "module_init.h"

#include "gimbal.h"
#include "system_define.h"
#include "system_instance.h"

/* 每个模块的配置按外设、通信、电机、状态、参数五层写，没有的层省略，层间用分隔线隔开 */

/*--------------------------------------监控--------------------------------------*/
static void Module_MonitorInit(void)
{
    const MonitorConfig_t config =
    {
        /*--------------------------------------通信层--------------------------------------*/
        .comm = {.monitor_task = &MonitorTaskHandle},
        /*--------------------------------------参数层--------------------------------------*/
        .param =
        {
            .sample_period_ms = TASK_MOTOR_SEND_PERIOD_MS,
            .task_count = SYSTEM_TASK_COUNT,
            .task_period_ms = systemTaskPeriodMs,
        },
    };
    Monitor_Init(&systemMonitor, &config);
}

/*--------------------------------------INS--------------------------------------*/
#define INS_MADGWICK_BETA 0.05f

static void Module_InsInit(void)
{
    const INSConfig_t config =
    {
        /*--------------------------------------通信层--------------------------------------*/
        .comm = {.msg_queue = insMsgQueue},
        /*--------------------------------------参数层--------------------------------------*/
        .param =
        {
            .beta = INS_MADGWICK_BETA,
            /* 传感器系 -> 云台 IMU 系（x 前、y 右、z 下），v_gimbal = R * v_sensor
             * R = Rx(180°) * Rz(顺时针 90°)：先绕 z 轴顺时针（从上往下看）转 90°，再绕 x 轴转 180°
             * 即云台 x = 传感器 -y，云台 y = 传感器 -x，云台 z = 传感器 -z；装机验证后方向反了再改这里 */
            .mount =
            {
                { 0.0f, -1.0f,  0.0f},
                {-1.0f,  0.0f,  0.0f},
                { 0.0f,  0.0f, -1.0f},
            },
        },
    };
    INS_Init(&board_ins, &config);
}

/*--------------------------------------云台--------------------------------------*/
#define GIMBAL_PITCH_MIN_DEG          (-15.0f)
#define GIMBAL_PITCH_MAX_DEG          36.0f
#define GIMBAL_ANGLE_LOOP_DIV         2U
#define GIMBAL_MS_PER_S               1000.0f

/* 连杆长度只影响动力学前馈，指向控制只用旋转部分，待测量 */
#define GIMBAL_LINK_YAW_PITCH_M       0.0f
#define GIMBAL_LINK_PITCH_END_M       0.0f
#define GIMBAL_DH_ALPHA_PITCH_DEG     90.0f
#define GIMBAL_DH_ALPHA_END_DEG       90.0f

/* PID 未整定，全 0 时输出恒为 0，云台不会动，整定后再填 */
static const GimbalGainSet_t gimbalGain = {0};

static void Module_GimbalInit(void)
{
    const GimbalConfig_t config =
    {
        /*--------------------------------------外设层--------------------------------------*/
        .periph =
        {
            .yaw_can = &can_instances[SYSTEM_CAN3_INDEX],
            .pitch_can = &can_instances[SYSTEM_CAN1_INDEX],
        },
        /*--------------------------------------通信层--------------------------------------*/
        .comm =
        {
            .cmd_queue = gimbalCmdQueue,
            .ins_queue = insMsgQueue,
        },
        /*--------------------------------------电机层--------------------------------------*/
        .motor =
        {
            .yaw = &gimbalYawMotor.base,
            .pitch = &gimbalPitchMotor.base,
        },
        /*--------------------------------------状态层--------------------------------------*/
        .state = {.init_state = GIMBAL_PROTECTED},
        /*--------------------------------------参数层--------------------------------------*/
        .param =
        {
            .model =
            {
                .dh =
                {
                    {0.0f, 0.0f, 0.0f, 0.0f},
                    {GIMBAL_DH_ALPHA_PITCH_DEG, -GIMBAL_LINK_YAW_PITCH_M, 0.0f, 0.0f},
                    {GIMBAL_DH_ALPHA_END_DEG, GIMBAL_LINK_PITCH_END_M, 0.0f, 0.0f},
                },
            },
            .normal_gain = gimbalGain,
            .sniper_gain = gimbalGain,
            .pitch_min_deg = GIMBAL_PITCH_MIN_DEG,
            .pitch_max_deg = GIMBAL_PITCH_MAX_DEG,
            .angle_loop_div = GIMBAL_ANGLE_LOOP_DIV,
            .period_s = (float)TASK_GIMBAL_PERIOD_MS / GIMBAL_MS_PER_S,
            .cmd_timeout_ms = TASK_GIMBAL_CMD_TIMEOUT_MS,
        },
    };
    Gimbal_Init(&gimbalInstance, &config);
}

/*--------------------------------------状态机--------------------------------------*/
/* 摇杆映射参照 hero_down 的 GimbalInputUpdate（10ms 周期）：
 * 正常 yaw 每周期 ch2 * 1.0 deg、pitch 每周期 ch3 * 1.0 deg，换算成打满约 100 deg/s
 * 吊射 yaw 每周期 ch2 * 0.1 deg、pitch 每周期 ch3 * 0.06 deg，换算成打满约 10 / 6 deg/s */
#define STATE_NORMAL_YAW_RATE_DPS     100.0f
#define STATE_NORMAL_PITCH_RATE_DPS   100.0f
#define STATE_SNIPER_YAW_RATE_DPS     10.0f
#define STATE_SNIPER_PITCH_RATE_DPS   6.0f

static void Module_StateInit(void)
{
    const GimbalAmmoStateConfig_t config =
    {
        /*--------------------------------------外设层--------------------------------------*/
        .periph = {.remote = &rc_receiver},
        /*--------------------------------------通信层--------------------------------------*/
        .comm = {.cmd_queue = gimbalCmdQueue},
        /*--------------------------------------状态层--------------------------------------*/
        .state = {.init_state = GIMBAL_PROTECTED},
        /*--------------------------------------参数层--------------------------------------*/
        .param =
        {
            .normal = {STATE_NORMAL_YAW_RATE_DPS, STATE_NORMAL_PITCH_RATE_DPS},
            .sniper = {STATE_SNIPER_YAW_RATE_DPS, STATE_SNIPER_PITCH_RATE_DPS},
            .remote_timeout_ms = TASK_REMOTE_TIMEOUT_MS,
        },
    };
    GimbalAmmoState_Init(&gimbalAmmoState, &config);
}

/*--------------------------------------入口--------------------------------------*/
void Module_Init(void)
{
    Module_MonitorInit();
    Module_InsInit();
    Module_GimbalInit();
    Module_StateInit();
}
