#include "task_gimbal.h"

#include "FreeRTOS.h"
#include "fdcan.h"
#include "gimbal.h"
#include "monitor.h"
#include "queue.h"
#include "task.h"
#include "task_define.h"
#include "task_system.h"

#define GIMBAL_MS_PER_S               1000.0f

/* 启动等待（ms）与电机维护节拍（控制周期数），参照 hero_up 的 ControlTask 和电机发送逻辑 */
#define GIMBAL_STARTUP_DELAY_MS       400U
#define GIMBAL_ENABLE_RESEND_DIV      200U
#define GIMBAL_PROTECT_MAINT_DIV      16U
#define GIMBAL_PROTECT_DISABLE_SLOT   0U
#define GIMBAL_PROTECT_CLEAR_SLOT     8U

/*--------------------------------------输入--------------------------------------*/
/* 取状态任务最近一次发布的命令；邮箱为空或超时未更新时进保护，角速度清零 */
static GimbalAmmoCmd_t GimbalTask_ReadCmd(void)
{
    TaskGimbalCmdMail_t mail;
    GimbalAmmoCmd_t protect_cmd =
    {
        .gimbal_state = GIMBAL_PROTECTED,
    };

    if (xQueuePeek(gimbalCmdQueue, &mail, 0U) != pdPASS ||
        (TickType_t)(xTaskGetTickCount() - mail.publish_tick) >
            pdMS_TO_TICKS(TASK_GIMBAL_CMD_TIMEOUT_MS))
    {
        return protect_cmd;
    }
    return mail.cmd;
}

/*--------------------------------------电机维护--------------------------------------*/
/* 只维护 yaw 的 DM 电机，pitch 的 LK 同 hero_up 不发运行 / 停止命令，保护态只发零电流
 * 保护态：零力矩帧由 Gimbal_Control 发出，这里按节拍补发失能和清错
 * 非保护态：退出保护的首周期立即使能，之后定期重发，电机晚于主控上电时也能使能上 */
static void GimbalTask_MaintainYawMotor(GimbalState_t state)
{
    static uint16_t enable_divider = GIMBAL_ENABLE_RESEND_DIV - 1U;
    static uint8_t protect_slot = 0U;
    Motor_t *yaw_motor = &gimbalYawMotor.base;

    if (state == GIMBAL_PROTECTED)
    {
        if (protect_slot == GIMBAL_PROTECT_DISABLE_SLOT)
        {
            yaw_motor->motorops.disable(yaw_motor);
        }
        else if (protect_slot == GIMBAL_PROTECT_CLEAR_SLOT)
        {
            yaw_motor->motorops.clearerror(yaw_motor);
        }
        protect_slot = (uint8_t)((protect_slot + 1U) % GIMBAL_PROTECT_MAINT_DIV);
        enable_divider = GIMBAL_ENABLE_RESEND_DIV - 1U;
        return;
    }

    protect_slot = 0U;
    if (++enable_divider >= GIMBAL_ENABLE_RESEND_DIV)
    {
        enable_divider = 0U;
        yaw_motor->motorops.enable(yaw_motor);
    }
}

/*--------------------------------------任务主体--------------------------------------*/
void GimbalTask(void *argument)
{
    (void)argument;
    /* 等 INS 完成初始化、电机上电回传反馈后再进入控制循环 */
    vTaskDelay(pdMS_TO_TICKS(GIMBAL_STARTUP_DELAY_MS));
    TickType_t last_wake = xTaskGetTickCount();

    const float period_s = (float)TASK_GIMBAL_PERIOD_MS / GIMBAL_MS_PER_S;

    while (1)
    {
        GimbalAmmoCmd_t cmd = GimbalTask_ReadCmd();
        GimbalAmmoState_Apply(&gimbalInstance, &cmd, period_s);
        Gimbal_Estimate(&gimbalInstance);
        /* 使能帧排在本周期力矩帧之前，退出保护的第一帧力矩就能生效 */
        GimbalTask_MaintainYawMotor(cmd.gimbal_state);
        Gimbal_Control(&gimbalInstance);

        Monitor_TaskBeat(MONITOR_TASK_GIMBAL);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_GIMBAL_PERIOD_MS));
    }
}
