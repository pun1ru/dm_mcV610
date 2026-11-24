#include "module_init.h"

#include "system_define.h"
#include "system_instance.h"

/* 通用模板只实例化基础监控模块，具体业务模块在应用工程中继续添加。 */
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

void Module_Init(void)
{
    Module_MonitorInit();
}
