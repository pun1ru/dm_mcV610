#include "module_init.h"

#include "gimbal.h"
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

/*--------------------------------------INS--------------------------------------*/
#define INS_MADGWICK_BETA 0.05f

static void Module_InsInit(void)
{
    const INSConfig_t config =
    {
        .comm = {.msg_queue = insMsgQueue},
        .param =
        {
            .beta = INS_MADGWICK_BETA,
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
//pitch限幅（关节角不是imu）
#define GIMBAL_PITCH_MIN_DEG          (-15.0f)
#define GIMBAL_PITCH_MAX_DEG          36.0f
//速度环角度环分频
#define GIMBAL_ANGLE_LOOP_DIV         5U
#define GIMBAL_SPEED_LOOP_DIV         1U
//用imu旋转到yaw系的角速度做闭环，pitch还没调，暂时留着吧啊
#define GIMBAL_YAW_SPEED_USE_GYRO     1U
#define GIMBAL_MS_PER_S               1000.0f

//连杆长度未测定没有使用用来动力学前馈的，dh参数表
#define GIMBAL_LINK_YAW_PITCH_M       0.0f
#define GIMBAL_LINK_PITCH_END_M       0.0f
#define GIMBAL_DH_ALPHA_PITCH_DEG     90.0f
#define GIMBAL_DH_ALPHA_END_DEG       90.0f
//PID参数，只有yaw
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
        .periph =
        {
            .yaw_can = &can_instances[SYSTEM_CAN3_INDEX],
            .pitch_can = &can_instances[SYSTEM_CAN1_INDEX],
        },
        .comm =
        {
            .cmd_queue = gimbalCmdQueue,
            .ins_queue = insMsgQueue,
        },
        .motor =
        {
            .yaw = &gimbalYawMotor.base,
            .pitch = &gimbalPitchMotor.base,
        },
        .state = {.init_state = GIMBAL_PROTECTED},
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
            .period_s = (float)TASK_GIMBAL_PERIOD_MS / GIMBAL_MS_PER_S,
            .cmd_timeout_ms = GIMBAL_CMD_TIMEOUT_MS,
            .force_zero_output = SYSTEM_GIMBAL_FORCE_ZERO_OUTPUT,
        },
    };
    Gimbal_Init(&gimbalInstance, &config);
}

/*--------------------------------------摩擦轮--------------------------------------*/
//转速
#define SHOOTER_FRIC_RPM              3110.0f
#define SHOOTER_STANDBY_RPM           (-200.0f)
/* 反馈连续 50 ms 没更新按失联 */
#define SHOOTER_FEEDBACK_TIMEOUT_CYCLES   (50U / TASK_SHOOTER_PERIOD_MS)

static void Module_ShooterInit(void)
{
    const ShooterConfig_t config =
    {
        .periph =
        {
            .can =
            {
                [SHOOTER_FRIC_LEFT] = &can_instances[SYSTEM_CAN2_INDEX],
                [SHOOTER_FRIC_RIGHT] = &can_instances[SYSTEM_CAN2_INDEX],
                [SHOOTER_FRIC_UP] = &can_instances[SYSTEM_CAN2_INDEX],
            },
        },
        .comm =
        {
            .cmd_queue = shooterCmdQueue,
            .debug_queue = debugMsgQueue,
        },
        .motor =
        {
            .wheel =
            {
                [SHOOTER_FRIC_LEFT] = &shooterFricMotor[SHOOTER_FRIC_LEFT].base,
                [SHOOTER_FRIC_RIGHT] = &shooterFricMotor[SHOOTER_FRIC_RIGHT].base,
                [SHOOTER_FRIC_UP] = &shooterFricMotor[SHOOTER_FRIC_UP].base,
            },
            .wheel_count = SHOOTER_FRIC_COUNT,
        },
        .state = {.init_state = SHOOTER_PROTECTED},
        .param =
        {
            .speed =
            {
                .kp = 17.5f,
                .ki = 0.0f,
                .kd = 12.5f,
                .d_alpha = 0.0f,
                .sum_error_max = 0.0f,
                .output_max = 8000.0f,
            },
            .fric_rpm = SHOOTER_FRIC_RPM,
            .standby_rpm = SHOOTER_STANDBY_RPM,
            .cmd_timeout_ms = SHOOTER_CMD_TIMEOUT_MS,
            .feedback_timeout_cycles = SHOOTER_FEEDBACK_TIMEOUT_CYCLES,
            .debug_channel = DEBUG_CHANNEL_SHOOTER_SPEED,
        },
    };
    Shooter_Init(&shooterInstance, &config);
}

/*--------------------------------------状态机--------------------------------------*/
//暂时用来测云台的最小状态机
//正常 yaw 每周期 ch2 * 1.0 deg、pitch 每周期 ch3 * 1.0 deg，换算成打满约 100 deg/s
//吊射 yaw 每周期 ch2 * 0.1 deg、pitch 每周期 ch3 * 0.06 deg，换算成打满约 10 / 6 deg/s 
#define STATE_NORMAL_YAW_RATE_DPS     100.0f
#define STATE_NORMAL_PITCH_RATE_DPS   100.0f
#define STATE_SNIPER_YAW_RATE_DPS     10.0f
#define STATE_SNIPER_PITCH_RATE_DPS   6.0f

static void Module_StateInit(void)
{
    const GimbalAmmoStateConfig_t config =
    {
        .periph = {.remote = &rc_receiver},
        .comm =
        {
            .cmd_queue = gimbalCmdQueue,
            .shooter_queue = shooterCmdQueue,
        },
        .state = {.init_state = GIMBAL_PROTECTED},
        .param =
        {
            .normal = {STATE_NORMAL_YAW_RATE_DPS, STATE_NORMAL_PITCH_RATE_DPS},
            .sniper = {STATE_SNIPER_YAW_RATE_DPS, STATE_SNIPER_PITCH_RATE_DPS},
            .remote_timeout_ms = REMOTE_TIMEOUT_MS,
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
    Module_ShooterInit();
    Module_StateInit();
}
