#include "base_init.h"

#include "WS2812.h"
#include "bsp_dwt.h"
#include "monitor.h"
#include "spi.h"
#include "task_define.h"

void Base_Init(void)
{
    DWT_Init(TASK_CPU_FREQUENCY_MHZ);
    Monitor_Init();
    WS2812_SPI_Init(&hspi6);
}
