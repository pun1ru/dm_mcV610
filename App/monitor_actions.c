#include "monitor_actions.h"

#include "FreeRTOS.h"
#include "bsp_buzzer.h"
#include "task.h"

#define MONITOR_ACTION_ALERT_PITCH       69U
#define MONITOR_ACTION_ALERT_DURATION_MS 100U
#define MONITOR_ACTION_TICK_HALF_RANGE   0x80000000UL

static TickType_t alert_deadline;
static uint8_t alert_active;

void MonitorAction_OnTaskEvent(
    MonitorEvent_t event,
    const MonitorTaskConfig_t *config,
    const MonitorTaskStatus_t *status,
    void *context)
{
    (void)config;
    (void)status;
    (void)context;

    if (event == MONITOR_EVENT_LOST) {
        alert_deadline = xTaskGetTickCount() +
                         pdMS_TO_TICKS(MONITOR_ACTION_ALERT_DURATION_MS);
        alert_active = 1U;
        Buzzer_PlayPitch(MONITOR_ACTION_ALERT_PITCH);
    }
}

void MonitorAction_Poll(void)
{
    TickType_t now;

    if (alert_active == 0U) {
        return;
    }

    now = xTaskGetTickCount();
    if ((TickType_t)(now - alert_deadline) < MONITOR_ACTION_TICK_HALF_RANGE) {
        Buzzer_PlayPitch(0U);
        alert_active = 0U;
    }
}
