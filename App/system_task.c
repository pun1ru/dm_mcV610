#include "system_task.h"

#include "FreeRTOS.h"
#include "WS2812.h"
#include "bsp_buzzer.h"
#include "bsp_dwt.h"
#include "comm_init.h"
#include "system_define.h"
#include "system_instance.h"
#include "task.h"

#define TASK_MONITOR_EVALUATE_DIVIDER   (TASK_MONITOR_PERIOD_MS / TASK_MOTOR_SEND_PERIOD_MS)
#define TASK_CAN_MONITOR_DIVIDER        (TASK_CAN_MONITOR_PERIOD_MS / TASK_MOTOR_SEND_PERIOD_MS)
#define TASK_MONITOR_ALERT_PITCH        69U
#define TASK_MONITOR_BREATHE_PERIOD_MS  2000U
#define TASK_MONITOR_BREATHE_GREEN      255U

/* 每个任务循环末尾发一次心跳 */
static void Task_Beat(SystemTaskId_t task)
{
    Monitor_Beat(&systemMonitor, (uint8_t)task);
}

/*--------------------------------------基础任务--------------------------------------*/
void MonitorTask(void *argument)
{
    TickType_t last_wake;
    TickType_t loop_period = pdMS_TO_TICKS(TASK_MOTOR_SEND_PERIOD_MS);
    uint8_t evaluate_divider = 0U;
    uint8_t can_monitor_divider = 0U;
    uint8_t buzzer_active = 0U;

    (void)argument;
    last_wake = xTaskGetTickCount();
    WS2812_Breathe_Start(0U, TASK_MONITOR_BREATHE_GREEN, 0U, TASK_MONITOR_BREATHE_PERIOD_MS);

    for (;;)
    {
        Monitor_Collect(&systemMonitor);

        if (++can_monitor_divider >= TASK_CAN_MONITOR_DIVIDER)
        {
            can_monitor_divider = 0U;
            CAN_MonitorPoll();
        }

        if (++evaluate_divider >= TASK_MONITOR_EVALUATE_DIVIDER)
        {
            evaluate_divider = 0U;
            WS2812_Breathe_Update();
            if (buzzer_active != 0U)
            {
                Buzzer_PlayPitch(0U);
                buzzer_active = 0U;
            }
            if (Monitor_Evaluate(&systemMonitor) != 0U)
            {
                Buzzer_PlayPitch(TASK_MONITOR_ALERT_PITCH);
                buzzer_active = 1U;
            }
        }
        vTaskDelayUntil(&last_wake, loop_period);
        if ((TickType_t)(xTaskGetTickCount() - last_wake) >= loop_period)
        {
            last_wake = xTaskGetTickCount();
        }
    }
}

void ReceiveTask(void *argument)
{
    uint32_t notification_value;

    (void)argument;

    for (;;)
    {
        Comm_ProcessCan();
        Comm_ProcessUart();

        Task_Beat(SYSTEM_TASK_RECEIVE);
        notification_value = 0U;
        (void)xTaskNotifyWait(0U, (uint32_t)UART_EVENT_ALL, &notification_value,
                              pdMS_TO_TICKS(TASK_RECEIVE_PERIOD_MS));
    }
}

void TransmitTask(void *argument)
{
    (void)argument;

    for (;;)
    {
        Comm_ProcessTransmit();
        Task_Beat(SYSTEM_TASK_TRANSMIT);
    }
}

/*--------------------------------------模块任务--------------------------------------*/
void INS_Task(void *argument)
{
    TickType_t last_wake;
    uint32_t last_cycle;

    (void)argument;
    last_wake = xTaskGetTickCount();
    last_cycle = DWT->CYCCNT;

    for (;;)
    {
        if (board_ins.ready == 0U && INS_Connect(&board_ins) != 0U)
        {
            last_wake = xTaskGetTickCount();
            last_cycle = DWT->CYCCNT;
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_INS_PERIOD_MS));
        INS_Run(&board_ins, DWT_GetDeltaT(&last_cycle));
        Task_Beat(SYSTEM_TASK_INS);
    }
}

void GimbalTask(void *argument)
{
    TickType_t last_wake;

    (void)argument;
    /* 等 INS 完成初始化、电机上电回传反馈后再进入控制循环 */
    vTaskDelay(pdMS_TO_TICKS(TASK_GIMBAL_STARTUP_DELAY_MS));
    last_wake = xTaskGetTickCount();

    for (;;)
    {
        Gimbal_Run(&gimbalInstance);
        Task_Beat(SYSTEM_TASK_GIMBAL);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_GIMBAL_PERIOD_MS));
    }
}

void StateTask(void *argument)
{
    TickType_t last_wake;

    (void)argument;
    last_wake = xTaskGetTickCount();

    for (;;)
    {
        GimbalAmmoState_Run(&gimbalAmmoState);
        Task_Beat(SYSTEM_TASK_STATE);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_STATE_PERIOD_MS));
    }
}
