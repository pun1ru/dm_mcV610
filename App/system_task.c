#include "system_task.h"

#include "FreeRTOS.h"
#include "WS2812.h"
#include "comm_init.h"
#include "monitor_actions.h"
#include "system_define.h"
#include "system_instance.h"
#include "task.h"

#define TASK_MONITOR_EVALUATE_DIVIDER   (TASK_MONITOR_PERIOD_MS / TASK_MONITOR_LOOP_PERIOD_MS)
#define TASK_CAN_MONITOR_DIVIDER        (TASK_CAN_MONITOR_PERIOD_MS / TASK_MONITOR_LOOP_PERIOD_MS)
#define TASK_MONITOR_BREATHE_PERIOD_MS  2000U
#define TASK_MONITOR_BREATHE_GREEN      255U

void MonitorTask(void *argument)
{
    TickType_t last_wake;
    TickType_t loop_period = pdMS_TO_TICKS(TASK_MONITOR_LOOP_PERIOD_MS);
    uint8_t evaluate_divider = 0U;
    uint8_t can_monitor_divider = 0U;

    (void)argument;
    last_wake = xTaskGetTickCount();
    WS2812_Breathe_Start(0U, TASK_MONITOR_BREATHE_GREEN, 0U,
                         TASK_MONITOR_BREATHE_PERIOD_MS);

    for (;;)
    {
        Monitor_Collect(&systemMonitor);
        //can检查，恢复
        if (++can_monitor_divider >= TASK_CAN_MONITOR_DIVIDER)
        {
            can_monitor_divider = 0U;
            CAN_MonitorPoll();
        }

        if (++evaluate_divider >= TASK_MONITOR_EVALUATE_DIVIDER)
        {
            evaluate_divider = 0U;
            WS2812_Breathe_Update();
            (void)Monitor_Evaluate(&systemMonitor);
            MonitorAction_Poll();
        }

        vTaskDelayUntil(&last_wake, loop_period);
        if ((TickType_t)(xTaskGetTickCount() - last_wake) >= loop_period)
        {
            last_wake = xTaskGetTickCount();
        }
    }
}

void CanTransmitTask(void *argument)
{
    (void)argument;

    for (;;)
    {
        Comm_ProcessCanTransmit();
        Monitor_Heartbeat(can_tx_heartbeat);
    }
}

void UartTransmitTask(void *argument)
{
    TickType_t last_wake;

    (void)argument;
    last_wake = xTaskGetTickCount();

    for (;;)
    {
        UART_MonitorPoll();
        Comm_ProcessUartTransmit();
        Monitor_Heartbeat(uart_tx_heartbeat);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_UART_TRANSMIT_PERIOD_MS));
    }
}
