#include "task_init.h"

#include "FreeRTOS.h"
#include "base_init.h"
#include "bsp_buzzer.h"
#include "comm_init.h"
#include "ipc_init.h"
#include "module_init.h"
#include "task.h"
#include "task_define.h"
#include "task_gimbal.h"
#include "task_ins.h"
#include "task_monitor.h"
#include "task_receive.h"
#include "task_state.h"
#include "task_system.h"
#include "task_transmit.h"

/* 初始化分层，顺序不能换：
 * 基础 -> 任务间队列 -> 登记通信口（只登记不启动）-> 模块创建并绑定 -> 创建任务 -> 最后启动通信
 * 回调和绑定表在 CAN 中断、UART 接收打开之前就位，不会有帧落进空回调
 * 各层的实现在同目录的 base_init、ipc_init、comm_init、module_init */

/*--------------------------------------任务--------------------------------------*/
static void Board_TaskCreate(void)
{
    xTaskCreate(MonitorTask, "MonitorTask", TASK_MONITOR_STACK_SIZE, NULL,
                TASK_MONITOR_PRIORITY, &MonitorTaskHandle);
    xTaskCreate(ReceiveTask, "ReceiveTask", TASK_RECEIVE_STACK_SIZE, NULL,
                TASK_RECEIVE_PRIORITY, &ReceiveTaskHandle);
    xTaskCreate(TransmitTask, "TransmitTask", TASK_TRANSMIT_STACK_SIZE, NULL,
                TASK_TRANSMIT_PRIORITY, &TransmitTaskHandle);
    xTaskCreate(INS_Task, "INSTask", TASK_INS_STACK_SIZE, NULL,
                TASK_INS_PRIORITY, &INSTaskHandle);
    xTaskCreate(GimbalTask, "GimbalTask", TASK_GIMBAL_STACK_SIZE, NULL,
                TASK_GIMBAL_PRIORITY, &GimbalTaskHandle);
    xTaskCreate(StateTask, "StateTask", TASK_STATE_STACK_SIZE, NULL,
                TASK_STATE_PRIORITY, &StateTaskHandle);
}

/*--------------------------------------入口--------------------------------------*/
void Board_Init(void)
{
    Base_Init();
    Ipc_Init();
    Comm_Register();
    Module_Init();
    Board_TaskCreate();
    Comm_Start();
}

void InitTask(void *argument)
{
    (void)argument;
    Board_Init();
    /* 蜂鸣含 vTaskDelay，不能放进临界区 */
    Buzzer_BeepOnce();
    vTaskDelete(NULL);
}
