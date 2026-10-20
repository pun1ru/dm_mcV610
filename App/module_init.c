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
#define GIMBAL_ANGLE_LOOP_DIV         5U
/* 速度环分频，1 即每个控制周期都跑；外环（角度环）慢、内环（速度环）快
 * 离散速度环的稳定条件看 kp * 周期 / 转动惯量，改分频要同步按比例调速度环 kp */
#define GIMBAL_SPEED_LOOP_DIV         1U
/* 关节角速度来源：0 用电机回传速度；N>0 用关节角差分，窗口 N 个控制周期，期间保持
 * 窗口取速度环周期的一半以内，并与速度环分频互质；速度环每周期都跑时只能取 1，每周期都差分一次
 * yaw 关节角量化约 0.0055 deg，2 ms 窗口约 2.7 dps，乘速度环 kp 才零点几 N*m */
#define GIMBAL_YAW_SPEED_DIFF_DIV     1U
#define GIMBAL_PITCH_SPEED_DIFF_DIV   0U
/* yaw 速度环反馈：1 用 IMU 陀螺转到 yaw 关节系的 z 分量（参照 hero_up），0 用关节角速度（上面的差分或电机回传）
 * 陀螺含底盘自身转动；pitch 暂不支持陀螺反馈，仍用关节角速度 */
#define GIMBAL_YAW_SPEED_USE_GYRO     1U
#define GIMBAL_MS_PER_S               1000.0f

/* 连杆长度只影响动力学前馈，指向控制只用旋转部分，待测量 */
#define GIMBAL_LINK_YAW_PITCH_M       0.0f
#define GIMBAL_LINK_PITCH_END_M       0.0f
#define GIMBAL_DH_ALPHA_PITCH_DEG     90.0f
#define GIMBAL_DH_ALPHA_END_DEG       90.0f

/* yaw 参照 hero_up 的 YAW_DUAL_PID（角度环 6.0 / 0.004 / 0，限幅 400；速度环 0.08，限幅 5）
 * 角度环每 GIMBAL_ANGLE_LOOP_DIV 个控制周期才跑一次，积分按次累加，所以 ki 乘分频数、积分限幅除以分频数
 * pitch 未整定，全 0 时输出恒为 0，pitch 不会动，整定后再填 */
static const GimbalGainSet_t gimbalGain =
{
    .axis[GIMBAL_YAW] =
    {
        .angle =
        {
            .kp = 5.0f,
            .ki = 0.004f * GIMBAL_ANGLE_LOOP_DIV,
            .kd = 0.0f,
            .d_alpha = 0.0f,
            .sum_error_max = 1000.0f / GIMBAL_ANGLE_LOOP_DIV,
            .output_max = 200.0f,
        },
        .speed =
        {
            .kp = 0.02f,
            .ki = 0.0f,
            .kd = 0.0f,
            .d_alpha = 0.4f,
            .sum_error_max = 0.0f,
            .output_max = 2.0f,
        },
    },
};

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
            .speed_loop_div = GIMBAL_SPEED_LOOP_DIV,
            .yaw_speed_use_gyro = GIMBAL_YAW_SPEED_USE_GYRO,
            .speed_diff_div =
            {
                [GIMBAL_YAW] = GIMBAL_YAW_SPEED_DIFF_DIV,
                [GIMBAL_PITCH] = GIMBAL_PITCH_SPEED_DIFF_DIV,
            },
            .period_s = (float)TASK_GIMBAL_PERIOD_MS / GIMBAL_MS_PER_S,
            .cmd_timeout_ms = TASK_GIMBAL_CMD_TIMEOUT_MS,
            .force_zero_output = SYSTEM_GIMBAL_FORCE_ZERO_OUTPUT,
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
