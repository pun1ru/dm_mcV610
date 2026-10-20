#include "base_init.h"

#include "WS2812.h"
#include "bsp_dwt.h"
#include "spi.h"
#include "system_define.h"

void Base_Init(void)
{
    DWT_Init(TASK_CPU_FREQUENCY_MHZ);
    WS2812_SPI_Init(&hspi6);
}
