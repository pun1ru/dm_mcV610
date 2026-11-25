#include "module_init.h"

#include "monitor_actions.h"
#include "system_define.h"
#include "system_instance.h"

#include <stddef.h>

/* 通用模板只实例化基础监控模块，具体业务模块在应用工程中继续添加。 */
static const MonitorTaskConfig_t monitorTasks[] =
{
    {
        .name = "CAN TX",
        .task_handle = &CanTransmitTaskHandle,
        .period_ms = TASK_CAN_TRANSMIT_PERIOD_MS,
        .timeout_ms = MONITOR_CAN_TX_TIMEOUT_MS,
        .on_event = MonitorAction_OnTaskEvent,
        .context = NULL,
        .heartbeat = &can_tx_heartbeat,
    },
    {
        .name = "UART TX",
        .task_handle = &UartTransmitTaskHandle,
        .period_ms = TASK_UART_TRANSMIT_PERIOD_MS,
        .timeout_ms = MONITOR_UART_TX_TIMEOUT_MS,
        .on_event = MonitorAction_OnTaskEvent,
        .context = NULL,
        .heartbeat = &uart_tx_heartbeat,
    },
};

static void Module_MonitorInit(void)
{
    const MonitorConfig_t config =
    {
        .comm = {.monitor_task = &MonitorTaskHandle},
        .param =
        {
            .sample_period_ms = TASK_MONITOR_LOOP_PERIOD_MS,
            .task_count =
                (uint8_t)(sizeof(monitorTasks) / sizeof(monitorTasks[0])),
            .tasks = monitorTasks,
        },
    };
    configASSERT(Monitor_Init(&systemMonitor, &config) == pdPASS);
}

void Module_Init(void)
{
    Module_MonitorInit();
}
