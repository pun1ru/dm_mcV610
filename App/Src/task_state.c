#include "task_state.h"

#include "FreeRTOS.h"
#include "monitor.h"
#include "queue.h"
#include "remote_control.h"
#include "task.h"
#include "task_define.h"
#include "task_system.h"

/* 摇杆映射参照 hero_down 的 GimbalInputUpdate（10ms 周期）：
 * 正常 yaw 每周期 ch2 * 1.0 deg、pitch 每周期 ch3 * 1.0 deg，换算成打满约 100 deg/s
 * 吊射 yaw 每周期 ch2 * 0.1 deg、pitch 每周期 ch3 * 0.06 deg，换算成打满约 10 / 6 deg/s */
#define STATE_NORMAL_YAW_RATE_DPS     100.0f
#define STATE_NORMAL_PITCH_RATE_DPS   100.0f
#define STATE_SNIPER_YAW_RATE_DPS     10.0f
#define STATE_SNIPER_PITCH_RATE_DPS   6.0f

/*--------------------------------------初始化--------------------------------------*/
static void StateTask_Init(void)
{
    const GimbalAmmoStateConfig_t config =
    {
        .normal =
        {
            .yaw_rate_dps = STATE_NORMAL_YAW_RATE_DPS,
            .pitch_rate_dps = STATE_NORMAL_PITCH_RATE_DPS,
        },
        .sniper =
        {
            .yaw_rate_dps = STATE_SNIPER_YAW_RATE_DPS,
            .pitch_rate_dps = STATE_SNIPER_PITCH_RATE_DPS,
        },
    };
    GimbalAmmoState_Init(&gimbalAmmoState, &config);
}

/*--------------------------------------输入--------------------------------------*/
/* 还没收到有效帧或已超时返回 NULL，状态机据此进保护 */
static const DT7CmdData *StateTask_ReadRemote(DT7CmdData *command)
{
    DT7RecData raw;
    uint32_t now_ms = (uint32_t)xTaskGetTickCount() * portTICK_PERIOD_MS;

    if (RC_Remote_Read(&rc_receiver, &raw, command) == 0U ||
        RC_Remote_IsOnline(&rc_receiver, now_ms, RC_DBUS_ONLINE_TIMEOUT_MS) == 0U)
    {
        return NULL;
    }
    return command;
}

/*--------------------------------------任务主体--------------------------------------*/
void StateTask(void *argument)
{
    (void)argument;
    StateTask_Init();
    TickType_t last_wake = xTaskGetTickCount();

    while (1)
    {
        DT7CmdData command;
        TaskGimbalCmdMail_t mail;

        GimbalAmmoState_Update(&gimbalAmmoState, StateTask_ReadRemote(&command), &mail.cmd);
        mail.publish_tick = xTaskGetTickCount();
        (void)xQueueOverwrite(gimbalCmdQueue, &mail);

        Monitor_TaskBeat(MONITOR_TASK_STATE);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_STATE_PERIOD_MS));
    }
}
