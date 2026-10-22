#include "ipc_init.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "system_define.h"
#include "system_instance.h"
//模块间使用队列通信,都是单个元素
void Ipc_Init(void)
{
    gimbalCmdQueue = xQueueCreate(TASK_GIMBAL_CMD_QUEUE_LENGTH, sizeof(GimbalCmdMail_t));
    insMsgQueue = xQueueCreate(TASK_INS_MSG_QUEUE_LENGTH, sizeof(INSMsg_t));
    shooterCmdQueue = xQueueCreate(TASK_SHOOTER_CMD_QUEUE_LENGTH, sizeof(ShooterCmdMail_t));
}
