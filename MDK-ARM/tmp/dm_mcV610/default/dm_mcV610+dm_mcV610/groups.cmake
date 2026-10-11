# groups.cmake

# group LibSE/Algorithm/Control/ADRC
add_library(Group_LibSE_Algorithm_Control_ADRC OBJECT
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Control/ADRC/adrc.c"
)
target_include_directories(Group_LibSE_Algorithm_Control_ADRC PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Control/ADRC"
)
target_compile_definitions(Group_LibSE_Algorithm_Control_ADRC PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Algorithm_Control_ADRC_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Algorithm_Control_ADRC_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Algorithm_Control_ADRC PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Algorithm_Control_ADRC PUBLIC
  Group_LibSE_Algorithm_Control_ADRC_ABSTRACTIONS
)

# group LibSE/Algorithm/Control/PID
add_library(Group_LibSE_Algorithm_Control_PID OBJECT
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Control/PID/pid.c"
)
target_include_directories(Group_LibSE_Algorithm_Control_PID PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Control/PID"
)
target_compile_definitions(Group_LibSE_Algorithm_Control_PID PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Algorithm_Control_PID_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Algorithm_Control_PID_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Algorithm_Control_PID PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Algorithm_Control_PID PUBLIC
  Group_LibSE_Algorithm_Control_PID_ABSTRACTIONS
)

# group LibSE/Algorithm/INS/Madgwick
add_library(Group_LibSE_Algorithm_INS_Madgwick OBJECT
  "${SOLUTION_ROOT}/../LibSE/Algorithm/INS/Madgwick/Madgwick.c"
)
target_include_directories(Group_LibSE_Algorithm_INS_Madgwick PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Algorithm/INS/Madgwick"
)
target_compile_definitions(Group_LibSE_Algorithm_INS_Madgwick PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Algorithm_INS_Madgwick_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Algorithm_INS_Madgwick_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Algorithm_INS_Madgwick PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Algorithm_INS_Madgwick PUBLIC
  Group_LibSE_Algorithm_INS_Madgwick_ABSTRACTIONS
)

# group LibSE/Algorithm/Math
add_library(Group_LibSE_Algorithm_Math OBJECT
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Math/algorism.c"
)
target_include_directories(Group_LibSE_Algorithm_Math PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Math"
)
target_compile_definitions(Group_LibSE_Algorithm_Math PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Algorithm_Math_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Algorithm_Math_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Algorithm_Math PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Algorithm_Math PUBLIC
  Group_LibSE_Algorithm_Math_ABSTRACTIONS
)

# group LibSE/Algorithm/Process/ekf
add_library(Group_LibSE_Algorithm_Process_ekf OBJECT
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Process/ekf/ekf_quaternion.c"
)
target_include_directories(Group_LibSE_Algorithm_Process_ekf PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Process/ekf"
)
target_compile_definitions(Group_LibSE_Algorithm_Process_ekf PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Algorithm_Process_ekf_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Algorithm_Process_ekf_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Algorithm_Process_ekf PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Algorithm_Process_ekf PUBLIC
  Group_LibSE_Algorithm_Process_ekf_ABSTRACTIONS
)

# group LibSE/Algorithm/Process/kalman_filter
add_library(Group_LibSE_Algorithm_Process_kalman_filter OBJECT
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Process/kalman_filter/kalman_filter.c"
)
target_include_directories(Group_LibSE_Algorithm_Process_kalman_filter PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Process/kalman_filter"
)
target_compile_definitions(Group_LibSE_Algorithm_Process_kalman_filter PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Algorithm_Process_kalman_filter_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Algorithm_Process_kalman_filter_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Algorithm_Process_kalman_filter PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Algorithm_Process_kalman_filter PUBLIC
  Group_LibSE_Algorithm_Process_kalman_filter_ABSTRACTIONS
)

# group LibSE/BSP/CAN
add_library(Group_LibSE_BSP_CAN OBJECT
  "${SOLUTION_ROOT}/../LibSE/BSP/CAN/CAN_driver.c"
)
target_include_directories(Group_LibSE_BSP_CAN PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/BSP/CAN"
)
target_compile_definitions(Group_LibSE_BSP_CAN PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_BSP_CAN_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_BSP_CAN_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_BSP_CAN PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_BSP_CAN PUBLIC
  Group_LibSE_BSP_CAN_ABSTRACTIONS
)

# group LibSE/BSP/DWT
add_library(Group_LibSE_BSP_DWT OBJECT
  "${SOLUTION_ROOT}/../LibSE/BSP/DWT/bsp_dwt.c"
)
target_include_directories(Group_LibSE_BSP_DWT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/BSP/DWT"
)
target_compile_definitions(Group_LibSE_BSP_DWT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_BSP_DWT_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_BSP_DWT_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_BSP_DWT PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_BSP_DWT PUBLIC
  Group_LibSE_BSP_DWT_ABSTRACTIONS
)

# group LibSE/BSP/PWM
add_library(Group_LibSE_BSP_PWM OBJECT
  "${SOLUTION_ROOT}/../LibSE/BSP/PWM/bsp_pwm.c"
)
target_include_directories(Group_LibSE_BSP_PWM PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/BSP/PWM"
)
target_compile_definitions(Group_LibSE_BSP_PWM PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_BSP_PWM_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_BSP_PWM_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_BSP_PWM PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_BSP_PWM PUBLIC
  Group_LibSE_BSP_PWM_ABSTRACTIONS
)

# group LibSE/BSP/SPI
add_library(Group_LibSE_BSP_SPI OBJECT
  "${SOLUTION_ROOT}/../LibSE/BSP/SPI/bsp_spi.c"
)
target_include_directories(Group_LibSE_BSP_SPI PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/BSP/SPI"
)
target_compile_definitions(Group_LibSE_BSP_SPI PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_BSP_SPI_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_BSP_SPI_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_BSP_SPI PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_BSP_SPI PUBLIC
  Group_LibSE_BSP_SPI_ABSTRACTIONS
)

# group Application/Core
add_library(Group_Application_Core OBJECT
  "${SOLUTION_ROOT}/../Core/Src/gpio.c"
  "${SOLUTION_ROOT}/../Core/Src/adc.c"
  "${SOLUTION_ROOT}/../Core/Src/dma.c"
  "${SOLUTION_ROOT}/../Core/Src/fdcan.c"
  "${SOLUTION_ROOT}/../Core/Src/spi.c"
  "${SOLUTION_ROOT}/../Core/Src/tim.c"
  "${SOLUTION_ROOT}/../Core/Src/usart.c"
  "${SOLUTION_ROOT}/../Core/Src/stm32h7xx_it.c"
  "${SOLUTION_ROOT}/../Core/Src/stm32h7xx_hal_msp.c"
  "${SOLUTION_ROOT}/../Core/Src/stm32h7xx_hal_timebase_tim.c"
)
target_include_directories(Group_Application_Core PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Core/Inc"
)
target_compile_definitions(Group_Application_Core PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_Core_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_Core_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Application_Core PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Application_Core PUBLIC
  Group_Application_Core_ABSTRACTIONS
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/gpio.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/adc.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/dma.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/fdcan.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/spi.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/tim.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/usart.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/stm32h7xx_hal_timebase_tim.c" PROPERTIES
  COMPILE_OPTIONS ""
)

# group LibSE/BSP/UART
add_library(Group_LibSE_BSP_UART OBJECT
  "${SOLUTION_ROOT}/../LibSE/BSP/UART/uart_driver.c"
)
target_include_directories(Group_LibSE_BSP_UART PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/BSP/UART"
)
target_compile_definitions(Group_LibSE_BSP_UART PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_BSP_UART_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_BSP_UART_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_BSP_UART PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_BSP_UART PUBLIC
  Group_LibSE_BSP_UART_ABSTRACTIONS
)

# group Application/USB_Device
add_library(Group_Application_USB_Device OBJECT
  "${SOLUTION_ROOT}/../USB_DEVICE/App/usb_device.c"
  "${SOLUTION_ROOT}/../USB_DEVICE/App/usbd_desc.c"
  "${SOLUTION_ROOT}/../USB_DEVICE/App/usbd_cdc_if.c"
  "${SOLUTION_ROOT}/../USB_DEVICE/Target/usbd_conf.c"
)
target_include_directories(Group_Application_USB_Device PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Application_USB_Device PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_USB_Device_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_USB_Device_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Application_USB_Device PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:-Wno-packed"
    "SHELL:-Wno-missing-variable-declarations"
    "SHELL:-Wno-missing-prototypes"
    "SHELL:-Wno-missing-noreturn"
    "SHELL:-Wno-sign-conversion"
    "SHELL:-Wno-nonportable-include-path"
    "SHELL:-Wno-reserved-id-macro"
    "SHELL:-Wno-unused-macros"
    "SHELL:-Wno-documentation-unknown-command"
    "SHELL:-Wno-documentation"
    "SHELL:-Wno-license-management"
    "SHELL:-Wno-parentheses-equality"
    "SHELL:-Wno-covered-switch-default"
    "SHELL:-Wno-unreachable-code-break"
  >
)
target_link_libraries(Group_Application_USB_Device PUBLIC
  Group_Application_USB_Device_ABSTRACTIONS
)
set_source_files_properties("${SOLUTION_ROOT}/../USB_DEVICE/App/usb_device.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../USB_DEVICE/App/usbd_desc.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../USB_DEVICE/App/usbd_cdc_if.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../USB_DEVICE/Target/usbd_conf.c" PROPERTIES
  COMPILE_OPTIONS ""
)

# group Application/USB_Device/Headers
add_library(Group_Application_USB_Device_Headers INTERFACE)
target_include_directories(Group_Application_USB_Device_Headers INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../USB_DEVICE/App"
)
target_compile_definitions(Group_Application_USB_Device_Headers INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_USB_Device_Headers_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_USB_Device_Headers_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group Application/USB_Device/Target
add_library(Group_Application_USB_Device_Target INTERFACE)
target_include_directories(Group_Application_USB_Device_Target INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../USB_DEVICE/Target"
)
target_compile_definitions(Group_Application_USB_Device_Target INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_USB_Device_Target_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_USB_Device_Target_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group LibSE/Drivers/Bmi088
add_library(Group_LibSE_Drivers_Bmi088 OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Bmi088/bmi088_driver.c"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Bmi088/bmi088_middleware.c"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Bmi088/imu_temp_ctrl.c"
)
target_include_directories(Group_LibSE_Drivers_Bmi088 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Bmi088"
)
target_compile_definitions(Group_LibSE_Drivers_Bmi088 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Bmi088_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Bmi088_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Bmi088 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Bmi088 PUBLIC
  Group_LibSE_Drivers_Bmi088_ABSTRACTIONS
)

# group LibSE/Drivers/Buzzer
add_library(Group_LibSE_Drivers_Buzzer OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Buzzer/bsp_buzzer.c"
)
target_include_directories(Group_LibSE_Drivers_Buzzer PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Buzzer"
)
target_compile_definitions(Group_LibSE_Drivers_Buzzer PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Buzzer_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Buzzer_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Buzzer PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Buzzer PUBLIC
  Group_LibSE_Drivers_Buzzer_ABSTRACTIONS
)

# group Application/System
add_library(Group_Application_System OBJECT
  "${SOLUTION_ROOT}/../Core/Src/system_stm32h7xx.c"
)
target_include_directories(Group_Application_System PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Application_System PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_System_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_System_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Application_System PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Application_System PUBLIC
  Group_Application_System_ABSTRACTIONS
)

# group ThirdParty/CMSIS/Core
add_library(Group_ThirdParty_CMSIS_Core INTERFACE)
target_include_directories(Group_ThirdParty_CMSIS_Core INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Drivers/CMSIS/Include"
)
target_compile_definitions(Group_ThirdParty_CMSIS_Core INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_CMSIS_Core_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_CMSIS_Core_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group ThirdParty/CMSIS/Device
add_library(Group_ThirdParty_CMSIS_Device INTERFACE)
target_include_directories(Group_ThirdParty_CMSIS_Device INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Drivers/CMSIS/Device/ST/STM32H7xx/Include"
)
target_compile_definitions(Group_ThirdParty_CMSIS_Device INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_CMSIS_Device_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_CMSIS_Device_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group ThirdParty/CMSIS/Startup
add_library(Group_ThirdParty_CMSIS_Startup OBJECT
  "${SOLUTION_ROOT}/startup_stm32h723xx.s"
)
target_include_directories(Group_ThirdParty_CMSIS_Startup PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_ThirdParty_CMSIS_Startup PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_CMSIS_Startup_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_CMSIS_Startup_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_ThirdParty_CMSIS_Startup PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_ThirdParty_CMSIS_Startup PUBLIC
  Group_ThirdParty_CMSIS_Startup_ABSTRACTIONS
)
set(COMPILE_DEFINITIONS
  STM32H723xx
  _RTE_
)
cbuild_set_defines(AS_ARM COMPILE_DEFINITIONS)
set_source_files_properties("${SOLUTION_ROOT}/startup_stm32h723xx.s" PROPERTIES
  COMPILE_FLAGS "${COMPILE_DEFINITIONS}"
)

# group ThirdParty/FreeRTOS/Source
add_library(Group_ThirdParty_FreeRTOS_Source OBJECT
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/croutine.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/list.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/queue.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/tasks.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/timers.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c"
)
target_include_directories(Group_ThirdParty_FreeRTOS_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_ThirdParty_FreeRTOS_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_FreeRTOS_Source_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_FreeRTOS_Source_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_ThirdParty_FreeRTOS_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:-Wno-packed"
    "SHELL:-Wno-missing-variable-declarations"
    "SHELL:-Wno-missing-prototypes"
    "SHELL:-Wno-missing-noreturn"
    "SHELL:-Wno-sign-conversion"
    "SHELL:-Wno-nonportable-include-path"
    "SHELL:-Wno-reserved-id-macro"
    "SHELL:-Wno-unused-macros"
    "SHELL:-Wno-documentation-unknown-command"
    "SHELL:-Wno-documentation"
    "SHELL:-Wno-license-management"
    "SHELL:-Wno-parentheses-equality"
    "SHELL:-Wno-covered-switch-default"
    "SHELL:-Wno-unreachable-code-break"
  >
)
target_link_libraries(Group_ThirdParty_FreeRTOS_Source PUBLIC
  Group_ThirdParty_FreeRTOS_Source_ABSTRACTIONS
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/croutine.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/list.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/queue.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/tasks.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/timers.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c" PROPERTIES
  COMPILE_OPTIONS ""
)

# group ThirdParty/FreeRTOS/CMSIS_RTOS
add_library(Group_ThirdParty_FreeRTOS_CMSIS_RTOS INTERFACE)
target_include_directories(Group_ThirdParty_FreeRTOS_CMSIS_RTOS INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS"
)
target_compile_definitions(Group_ThirdParty_FreeRTOS_CMSIS_RTOS INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_FreeRTOS_CMSIS_RTOS_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_FreeRTOS_CMSIS_RTOS_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group ThirdParty/FreeRTOS/Include
add_library(Group_ThirdParty_FreeRTOS_Include INTERFACE)
target_include_directories(Group_ThirdParty_FreeRTOS_Include INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/include"
)
target_compile_definitions(Group_ThirdParty_FreeRTOS_Include INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_FreeRTOS_Include_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_FreeRTOS_Include_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group LibSE/Drivers/Motor
add_library(Group_LibSE_Drivers_Motor OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/Motor.c"
)
target_include_directories(Group_LibSE_Drivers_Motor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor"
)
target_compile_definitions(Group_LibSE_Drivers_Motor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Motor_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Motor_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Motor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Motor PUBLIC
  Group_LibSE_Drivers_Motor_ABSTRACTIONS
)

# group LibSE/Drivers/Motor/DMMotor
add_library(Group_LibSE_Drivers_Motor_DMMotor OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/DMMotor/DMMotor.c"
)
target_include_directories(Group_LibSE_Drivers_Motor_DMMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/DMMotor"
)
target_compile_definitions(Group_LibSE_Drivers_Motor_DMMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Motor_DMMotor_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Motor_DMMotor_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Motor_DMMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Motor_DMMotor PUBLIC
  Group_LibSE_Drivers_Motor_DMMotor_ABSTRACTIONS
)

# group LibSE/Drivers/Motor/LKMotor
add_library(Group_LibSE_Drivers_Motor_LKMotor OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/LKMotor/LKMotor.c"
)
target_include_directories(Group_LibSE_Drivers_Motor_LKMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/LKMotor"
)
target_compile_definitions(Group_LibSE_Drivers_Motor_LKMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Motor_LKMotor_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Motor_LKMotor_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Motor_LKMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Motor_LKMotor PUBLIC
  Group_LibSE_Drivers_Motor_LKMotor_ABSTRACTIONS
)

# group LibSE/Drivers/Motor/RMMotor
add_library(Group_LibSE_Drivers_Motor_RMMotor OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/RMMotor/RMMotor.c"
)
target_include_directories(Group_LibSE_Drivers_Motor_RMMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/RMMotor"
)
target_compile_definitions(Group_LibSE_Drivers_Motor_RMMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Motor_RMMotor_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Motor_RMMotor_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Motor_RMMotor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Motor_RMMotor PUBLIC
  Group_LibSE_Drivers_Motor_RMMotor_ABSTRACTIONS
)

# group LibSE/Drivers/RC/DT7
add_library(Group_LibSE_Drivers_RC_DT7 OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/DT7/dt7_remote_driver.c"
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/DT7/remote_control.c"
)
target_include_directories(Group_LibSE_Drivers_RC_DT7 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/DT7"
)
target_compile_definitions(Group_LibSE_Drivers_RC_DT7 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_RC_DT7_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_RC_DT7_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_RC_DT7 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_RC_DT7 PUBLIC
  Group_LibSE_Drivers_RC_DT7_ABSTRACTIONS
)

# group LibSE/Drivers/RC/VT13
add_library(Group_LibSE_Drivers_RC_VT13 OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/VT13/CRC.c"
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/VT13/VT13_rc_ctrl.c"
)
target_include_directories(Group_LibSE_Drivers_RC_VT13 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/VT13"
)
target_compile_definitions(Group_LibSE_Drivers_RC_VT13 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_RC_VT13_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_RC_VT13_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_RC_VT13 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_RC_VT13 PUBLIC
  Group_LibSE_Drivers_RC_VT13_ABSTRACTIONS
)

# group LibSE/Drivers/Referee
add_library(Group_LibSE_Drivers_Referee OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/Referee/judge_receive.c"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Referee/referee.c"
)
target_include_directories(Group_LibSE_Drivers_Referee PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/Referee"
)
target_compile_definitions(Group_LibSE_Drivers_Referee PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_Referee_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_Referee_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_Referee PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_Referee PUBLIC
  Group_LibSE_Drivers_Referee_ABSTRACTIONS
)

# group ThirdParty/STM32H7xx_HAL_Driver/Source
add_library(Group_ThirdParty_STM32H7xx_HAL_Driver_Source OBJECT
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usb.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_fdcan.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c"
)
target_include_directories(Group_ThirdParty_STM32H7xx_HAL_Driver_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_ThirdParty_STM32H7xx_HAL_Driver_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_STM32H7xx_HAL_Driver_Source_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_STM32H7xx_HAL_Driver_Source_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_ThirdParty_STM32H7xx_HAL_Driver_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_ThirdParty_STM32H7xx_HAL_Driver_Source PUBLIC
  Group_ThirdParty_STM32H7xx_HAL_Driver_Source_ABSTRACTIONS
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usb.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc_ex.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_fdcan.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi_ex.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c" PROPERTIES
  COMPILE_OPTIONS ""
)

# group ThirdParty/STM32H7xx_HAL_Driver/Include
add_library(Group_ThirdParty_STM32H7xx_HAL_Driver_Include INTERFACE)
target_include_directories(Group_ThirdParty_STM32H7xx_HAL_Driver_Include INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Inc"
)
target_compile_definitions(Group_ThirdParty_STM32H7xx_HAL_Driver_Include INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_STM32H7xx_HAL_Driver_Include_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_STM32H7xx_HAL_Driver_Include_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group LibSE/Drivers/UI
add_library(Group_LibSE_Drivers_UI OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/UI/UI_driver.c"
)
target_include_directories(Group_LibSE_Drivers_UI PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/UI"
)
target_compile_definitions(Group_LibSE_Drivers_UI PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_UI_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_UI_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_UI PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_UI PUBLIC
  Group_LibSE_Drivers_UI_ABSTRACTIONS
)

# group ThirdParty/USB_Device_Library/Source
add_library(Group_ThirdParty_USB_Device_Library_Source OBJECT
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c"
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c"
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c"
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c"
)
target_include_directories(Group_ThirdParty_USB_Device_Library_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_ThirdParty_USB_Device_Library_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_USB_Device_Library_Source_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_USB_Device_Library_Source_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_ThirdParty_USB_Device_Library_Source PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:-Wno-packed"
    "SHELL:-Wno-missing-variable-declarations"
    "SHELL:-Wno-missing-prototypes"
    "SHELL:-Wno-missing-noreturn"
    "SHELL:-Wno-sign-conversion"
    "SHELL:-Wno-nonportable-include-path"
    "SHELL:-Wno-reserved-id-macro"
    "SHELL:-Wno-unused-macros"
    "SHELL:-Wno-documentation-unknown-command"
    "SHELL:-Wno-documentation"
    "SHELL:-Wno-license-management"
    "SHELL:-Wno-parentheses-equality"
    "SHELL:-Wno-covered-switch-default"
    "SHELL:-Wno-unreachable-code-break"
  >
)
target_link_libraries(Group_ThirdParty_USB_Device_Library_Source PUBLIC
  Group_ThirdParty_USB_Device_Library_Source_ABSTRACTIONS
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c" PROPERTIES
  COMPILE_OPTIONS ""
)
set_source_files_properties("${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c" PROPERTIES
  COMPILE_OPTIONS ""
)

# group ThirdParty/USB_Device_Library/CDC
add_library(Group_ThirdParty_USB_Device_Library_CDC INTERFACE)
target_include_directories(Group_ThirdParty_USB_Device_Library_CDC INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
)
target_compile_definitions(Group_ThirdParty_USB_Device_Library_CDC INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_USB_Device_Library_CDC_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_USB_Device_Library_CDC_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group ThirdParty/USB_Device_Library/Core
add_library(Group_ThirdParty_USB_Device_Library_Core INTERFACE)
target_include_directories(Group_ThirdParty_USB_Device_Library_Core INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
)
target_compile_definitions(Group_ThirdParty_USB_Device_Library_Core INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ThirdParty_USB_Device_Library_Core_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ThirdParty_USB_Device_Library_Core_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# group LibSE/Drivers/WS2812
add_library(Group_LibSE_Drivers_WS2812 OBJECT
  "${SOLUTION_ROOT}/../LibSE/Drivers/WS2812/WS2812.c"
  "${SOLUTION_ROOT}/../LibSE/Drivers/WS2812/ws2812_board.c"
)
target_include_directories(Group_LibSE_Drivers_WS2812 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Drivers/WS2812"
)
target_compile_definitions(Group_LibSE_Drivers_WS2812 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Drivers_WS2812_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Drivers_WS2812_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Drivers_WS2812 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Drivers_WS2812 PUBLIC
  Group_LibSE_Drivers_WS2812_ABSTRACTIONS
)

# group LibSE/Module/INS
add_library(Group_LibSE_Module_INS OBJECT
  "${SOLUTION_ROOT}/../LibSE/Module/INS/ins.c"
)
target_include_directories(Group_LibSE_Module_INS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Module/INS"
)
target_compile_definitions(Group_LibSE_Module_INS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Module_INS_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Module_INS_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Module_INS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Module_INS PUBLIC
  Group_LibSE_Module_INS_ABSTRACTIONS
)

# group LibSE/Module/Monitor
add_library(Group_LibSE_Module_Monitor OBJECT
  "${SOLUTION_ROOT}/../LibSE/Module/Monitor/monitor.c"
)
target_include_directories(Group_LibSE_Module_Monitor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Module/Monitor"
)
target_compile_definitions(Group_LibSE_Module_Monitor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Module_Monitor_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Module_Monitor_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Module_Monitor PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Module_Monitor PUBLIC
  Group_LibSE_Module_Monitor_ABSTRACTIONS
)

# group LibSE/Task/App
add_library(Group_LibSE_Task_App OBJECT
  "${SOLUTION_ROOT}/../LibSE/Task/Src/task_init.c"
  "${SOLUTION_ROOT}/../LibSE/Task/Src/task_ins.c"
  "${SOLUTION_ROOT}/../LibSE/Task/Src/task_receive.c"
  "${SOLUTION_ROOT}/../LibSE/Task/Src/task_transmit.c"
  "${SOLUTION_ROOT}/../LibSE/Task/Src/task_monitor.c"
)
target_include_directories(Group_LibSE_Task_App PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Task/Inc"
)
target_compile_definitions(Group_LibSE_Task_App PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Task_App_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Task_App_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Task_App PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Task_App PUBLIC
  Group_LibSE_Task_App_ABSTRACTIONS
)

# group Application/Core/Startup
add_library(Group_Application_Core_Startup OBJECT
  "${SOLUTION_ROOT}/../Core/Src/main.c"
)
target_include_directories(Group_Application_Core_Startup PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Core/Inc"
)
target_compile_definitions(Group_Application_Core_Startup PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_Core_Startup_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_Core_Startup_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Application_Core_Startup PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Application_Core_Startup PUBLIC
  Group_Application_Core_Startup_ABSTRACTIONS
)

# group Application/FreeRTOS
add_library(Group_Application_FreeRTOS OBJECT
  "${SOLUTION_ROOT}/../Core/Src/freertos.c"
)
target_include_directories(Group_Application_FreeRTOS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../Core/Inc"
)
target_compile_definitions(Group_Application_FreeRTOS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_FreeRTOS_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_FreeRTOS_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Application_FreeRTOS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Application_FreeRTOS PUBLIC
  Group_Application_FreeRTOS_ABSTRACTIONS
)
set_source_files_properties("${SOLUTION_ROOT}/../Core/Src/freertos.c" PROPERTIES
  COMPILE_OPTIONS ""
)

# group LibSE/Task/System
add_library(Group_LibSE_Task_System OBJECT
  "${SOLUTION_ROOT}/../LibSE/Task/task_system.c"
)
target_include_directories(Group_LibSE_Task_System PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/../LibSE/Task"
)
target_compile_definitions(Group_LibSE_Task_System PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_LibSE_Task_System_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_LibSE_Task_System_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_LibSE_Task_System PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_LibSE_Task_System PUBLIC
  Group_LibSE_Task_System_ABSTRACTIONS
)
