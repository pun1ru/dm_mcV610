#include "task_state.h"

#include "FreeRTOS.h"
#include "monitor.h"
#include "queue.h"
#include "remote_control.h"
#include "task.h"
#include "task_define.h"
#include "task_system.h"

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
