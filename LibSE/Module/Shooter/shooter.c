#include "shooter.h"

#include <stddef.h>
#include <string.h>

static void Shooter_ResetControl(Shooter_t *shooter)
{
    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        PIDReset(&shooter->controller.speed_pid[i]);
        shooter->controller.output[i] = 0.0f;
    }
    shooter->observer.ready_count = 0U;
    shooter->observer.fric_ready = 0U;
}

void Shooter_Init(Shooter_t *shooter, const ShooterConfig_t *config)
{
    if (shooter == NULL || config == NULL)
    {
        return;
    }

    memset(shooter, 0, sizeof(*shooter));
    shooter->state = config->init_state;
    shooter->speed_param = config->speed_param;
    shooter->back_speed_rpm = config->back_speed_rpm;
    shooter->ready_tolerance_rpm = config->ready_tolerance_rpm;
    shooter->ready_hold_tick = config->ready_hold_tick;
    shooter->offline_tick = config->offline_tick;

    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        shooter->motor[i] = config->motor[i];
        shooter->motor[i]->motorcmd.control_mode = MODE_CURRENT;
        PIDInitialize(&shooter->controller.speed_pid[i],
                      config->speed_param.kp, config->speed_param.ki,
                      config->speed_param.kd, config->speed_param.sum_error_max,
                      config->speed_param.output_max);
    }
    Shooter_ResetControl(shooter);
}

/*--------------------------------------输入层--------------------------------------*/
void Shooter_Input(Shooter_t *shooter, const ShooterInput_t *input)
{
    if (shooter == NULL || input == NULL)
    {
        return;
    }

    /* 在线标志来自上一周期的估计层，离线的轮不能带着其余轮空转 */
    ShooterState_t state = input->state;
    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        if (shooter->observer.online[i] == 0U)
        {
            state = SHOOTER_PROTECTED;
        }
    }

    if (state != shooter->state)
    {
        shooter->state = state;
        Shooter_ResetControl(shooter);
    }
    shooter->fric_speed_rpm = input->fric_speed_rpm;

    float target_rpm = 0.0f;
    if (shooter->state == SHOOTER_ISBACK)
    {
        target_rpm = -shooter->back_speed_rpm;
    }
    else if (shooter->state == SHOOTER_ISFRIC)
    {
        target_rpm = shooter->fric_speed_rpm;
    }
    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        shooter->target.speed_rpm[i] = target_rpm;
    }
}

/*--------------------------------------估计层--------------------------------------*/
void Shooter_Estimate(Shooter_t *shooter)
{
    if (shooter == NULL)
    {
        return;
    }

    ShooterObserver_t *obs = &shooter->observer;
    uint8_t all_in_range = 1U;

    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        const Motor_t *motor = shooter->motor[i];

        /* 反馈帧计数在变化即在线；上电后收到第一帧前视为离线 */
        if (motor->feedback.timestamp != obs->last_timestamp[i])
        {
            obs->last_timestamp[i] = motor->feedback.timestamp;
            obs->offline_count[i] = 0U;
            obs->online[i] = 1U;
        }
        else if (obs->offline_count[i] < shooter->offline_tick)
        {
            obs->offline_count[i]++;
        }
        if (obs->offline_count[i] >= shooter->offline_tick)
        {
            obs->online[i] = 0U;
        }

        /* 与目标同号：inverse 的电机指令在驱动里取反，反馈也要折算回来 */
        obs->speed_rpm[i] = motor->inverse ? -motor->feedback.velocity_rpm :
                                             motor->feedback.velocity_rpm;

        float error_rpm = shooter->target.speed_rpm[i] - obs->speed_rpm[i];
        if (error_rpm > shooter->ready_tolerance_rpm ||
            error_rpm < -shooter->ready_tolerance_rpm)
        {
            all_in_range = 0U;
        }
    }

    if (shooter->state == SHOOTER_ISFRIC && all_in_range != 0U)
    {
        if (obs->ready_count < shooter->ready_hold_tick)
        {
            obs->ready_count++;
        }
    }
    else
    {
        obs->ready_count = 0U;
    }
    obs->fric_ready = obs->ready_count >= shooter->ready_hold_tick ? 1U : 0U;
}

uint8_t Shooter_IsFricReady(const Shooter_t *shooter)
{
    return shooter->observer.fric_ready;
}

/*--------------------------------------控制层--------------------------------------*/
/* 同一个 RM 组的电机共用一帧，有组发送接口时只发一次，否则逐个发送 */
static void Shooter_ApplyOutput(Shooter_t *shooter)
{
    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        shooter->motor[i]->motorcmd.current = shooter->controller.output[i];
    }

    Motor_t *first = shooter->motor[0];
    if (first->motorops.sendgroupcommand != NULL)
    {
        first->motorops.sendgroupcommand(first);
        return;
    }
    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        shooter->motor[i]->motorops.sendcommand(shooter->motor[i]);
    }
}

void Shooter_Control(Shooter_t *shooter)
{
    if (shooter == NULL)
    {
        return;
    }

    if (shooter->state == SHOOTER_PROTECTED)
    {
        Shooter_ResetControl(shooter);
        Shooter_ApplyOutput(shooter);
        return;
    }

    for (uint8_t i = 0U; i < SHOOTER_FRIC_COUNT; ++i)
    {
        shooter->controller.output[i] =
            PIDUpdate(&shooter->controller.speed_pid[i],
                      shooter->target.speed_rpm[i] - shooter->observer.speed_rpm[i]);
    }
    Shooter_ApplyOutput(shooter);
}
