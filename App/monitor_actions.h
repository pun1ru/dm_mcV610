#ifndef MONITOR_ACTIONS_H
#define MONITOR_ACTIONS_H

#include "monitor.h"

void MonitorAction_OnTaskEvent(
    MonitorEvent_t event,
    const MonitorTaskConfig_t *config,
    const MonitorTaskStatus_t *status,
    void *context);
void MonitorAction_Poll(void);

#endif
