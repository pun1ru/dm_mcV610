#include "task_ins.h"
#include "bsp_dwt.h"
#include "FreeRTOS.h"
#include "task_define.h"
#include "task.h"
#include "monitor.h"
#include "task_system.h"

#define INS_MADGWICK_BETA 0.05f
#define INS_RETRY_PERIOD_MS 1000U

/* 传感器系 -> 云台 IMU 系（x 前、y 右、z 下），v_gimbal = R * v_sensor
 * R = Rx(180°) * Rz(顺时针 90°)：先绕 z 轴顺时针（从上往下看）转 90°，再绕 x 轴转 180°
 * 即云台 x = 传感器 -y，云台 y = 传感器 -x，云台 z = 传感器 -z；装机验证后方向反了再改这里 */
static const float insMountMatrix[3][3] = {
    { 0.0f, -1.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    { 0.0f,  0.0f, -1.0f},
};

void INS_Task(void *argument)
{
    TickType_t last_wake;
    uint32_t last_cycle;
    (void)argument;
    last_wake = xTaskGetTickCount();
    last_cycle = DWT->CYCCNT;

    while(1) {
        if (board_ins.ready == 0U) {
            INS_Init(&board_ins, INS_MADGWICK_BETA, insMountMatrix);
            last_wake = xTaskGetTickCount();
            last_cycle = DWT->CYCCNT;
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(TASK_INS_PERIOD_MS));
        INS_Update(&board_ins, DWT_GetDeltaT(&last_cycle));
        Monitor_TaskBeat(MONITOR_TASK_INS);
    }
}
