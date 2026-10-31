#include "system_task.h"

#include "FreeRTOS.h"
#include "WS2812.h"
#include "bsp_buzzer.h"
#include "bsp_dwt.h"
#include "comm_init.h"
#include "system_define.h"
#include "system_instance.h"
#include "task.h"

#define TASK_MONITOR_EVALUATE_DIVIDER   (TASK_MONITOR_PERIOD_MS / TASK_MONITOR_LOOP_PERIOD_MS)
#define TASK_CAN_MONITOR_DIVIDER        (TASK_CAN_MONITOR_PERIOD_MS / TASK_MONITOR_LOOP_PERIOD_MS)
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
    TickType_t loop_period = pdMS_TO_TICKS(TASK_MONITOR_LOOP_PERIOD_MS);
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

/* CAN、UART 接收在中断里直接分发，没有接收任务 */
void TransmitTask(void *argument)
{
    (void)argument;

    for (;;)
    {
        Comm_ProcessTransmit();
        Task_Beat(SYSTEM_TASK_TRANSMIT);
    }
}

/* UART 发送和 CAN 发送分开：CAN 是队列里有帧就发，UART 发的是调试数据，按固定周期整理一帧发一次 */
void UartTransmitTask(void *argument)
{
    TickType_t last_wake;

    (void)argument;
    last_wake = xTaskGetTickCount();

    for (;;)
    {
        Comm_ProcessUartTransmit();
        Task_Beat(SYSTEM_TASK_UART_TRANSMIT);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_UART_TRANSMIT_PERIOD_MS));
    }
}

/*--------------------------------------模块任务--------------------------------------*/
/* 遥控状态机：
 * 双上拨（左右拨杆都在上挡）-> 保护，七个关节全部发零指令；
 * 右拨杆中挡 -> 使能，关节角度环工作，目标默认对齐当前关节角，PID 参数全 0 时不出力；
 * 其余拨杆组合、遥控掉线或没有有效帧 -> 保护。 */
static void Arm_RcStateSync(Arm_t *arm)
{
    DT7RecData raw;
    DT7CmdData command;
    uint32_t now_ms = (uint32_t)(xTaskGetTickCount() * (TickType_t)portTICK_PERIOD_MS);
    uint8_t rc_online = RC_Remote_IsOnline(&rc_receiver, now_ms, RC_DBUS_ONLINE_TIMEOUT_MS);

    Arm_SetRcOnline(arm, rc_online != 0U);

    /* 没有有效帧、掉线，或者两个拨杆都在上挡，都按保护处理 */
    if (rc_online == 0U || RC_Remote_Read(&rc_receiver, &raw, &command) == 0U ||
        (command.switch_left == DT7_RC_SW_UP && command.switch_right == DT7_RC_SW_UP))
    {
        (void)Arm_SetState(arm, ARM_PROTECTED);
        return;
    }

    if (command.switch_left == DT7_RC_SW_UP && command.switch_right == DT7_RC_SW_DOWN)
    {
        (void)Arm_SetState(arm, ARM_HOST);
    }
    else
    {
        (void)Arm_SetState(arm, command.switch_right == DT7_RC_SW_MID ? ARM_ENABLE : ARM_PROTECTED);
    }
}

/* 模块任务往这里加：循环里调用模块运行函数，末尾 Task_Beat，再 vTaskDelayUntil */
void ArmTask(void *argument)
{
    TickType_t last_wake;

    (void)argument;
    last_wake = xTaskGetTickCount();

    for (;;)
    {
        Arm_RcStateSync(&armInstance);
        Arm_Run(&armInstance);
        Task_Beat(SYSTEM_TASK_ARM);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_ARM_PERIOD_MS));
    }
}
