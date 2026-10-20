#include "ipc_init.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "system_define.h"
#include "system_instance.h"

void Ipc_Init(void)
{
    gimbalCmdQueue = xQueueCreate(TASK_GIMBAL_CMD_QUEUE_LENGTH, sizeof(GimbalCmdMail_t));
    insMsgQueue = xQueueCreate(TASK_INS_MSG_QUEUE_LENGTH, sizeof(INSMsg_t));
}
