#include "ipc_init.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task_define.h"
#include "task_system.h"

void Ipc_Init(void)
{
    gimbalCmdQueue = xQueueCreate(TASK_GIMBAL_CMD_QUEUE_LENGTH, sizeof(TaskGimbalCmdMail_t));
}
