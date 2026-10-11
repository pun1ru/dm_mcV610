cmake_minimum_required(VERSION 3.27)

# Roots
include("../roots.cmake")

set(CONTEXT dm_mcV610+dm_mcV610)
set(TARGET ${CONTEXT})
set(DNAME STM32H723VGTx)
set(DPACK Keil::STM32H7xx_DFP@4.1.3)
set(DPACK_DIR "${CMSIS_PACK_ROOT}/Keil/STM32H7xx_DFP/4.1.3")
set(OUT_DIR "${SOLUTION_ROOT}/out/dm_mcV610/dm_mcV610")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_COMMANDS ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json)
set(COMPILE_COMMANDS ${OUT_DIR}/compile_commands.json)
set(COMPILE_MACROS_C ${OUT_DIR}/compile_macros_c.h)
set(LD_SCRIPT "${SOLUTION_ROOT}/dm_mcV610_dm_mcV610.sct")
set(LD_SCRIPT_PP ${LD_SCRIPT})

# Processor Options
set(CPU Cortex-M7)
set(FPU DP_FPU)
set(DSP DSP)
set(BYTE_ORDER Little-endian)

# Preprocessor options
set(CPP_OPTIONS_C "-xc" "-std=c99" "-fno-rtti" "-funsigned-char" "-fshort-enums" "-fshort-wchar" "-ffunction-sections" "-Wno-packed" "-Wno-missing-variable-declarations" "-Wno-missing-prototypes" "-Wno-missing-noreturn" "-Wno-sign-conversion" "-Wno-nonportable-include-path" "-Wno-reserved-id-macro" "-Wno-unused-macros" "-Wno-documentation-unknown-command" "-Wno-documentation" "-Wno-license-management" "-Wno-parentheses-equality")

# Toolchain config map
set(COMPILER AC6)
include("toolchain.cmake")

# Setup project
project(${CONTEXT} LANGUAGES C ASM)

# Enable color diagnostics
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Compilation database
add_custom_target(database DEPENDS ${COMPILE_COMMANDS} ${COMPILE_MACROS_C})
add_custom_command(OUTPUT ${COMPILE_COMMANDS}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_COMPILE_COMMANDS}" "${COMPILE_COMMANDS}"
  DEPENDS "${CMAKE_COMPILE_COMMANDS}"
)
add_custom_command(OUTPUT ${COMPILE_MACROS_C}
  COMMAND ${CPP} ${CPP_OPTIONS_C} ${CPP_DUMP_MACROS} "${COMPILE_MACROS_C}"
)
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})

# Setup context
add_executable(${CONTEXT})
set_target_properties(${CONTEXT} PROPERTIES PREFIX "" SUFFIX ".axf" OUTPUT_NAME "dm_mcV610")
set_target_properties(${CONTEXT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
add_library(${CONTEXT}_GLOBAL INTERFACE)

# Includes
target_include_directories(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C,CXX>:
    "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F"
  >
  "${SOLUTION_ROOT}/RTE/_dm_mcV610"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Include"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/PrivateInclude"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS/5.9.0/CMSIS/Core/Include"
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Control/ADRC"
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Control/PID"
  "${SOLUTION_ROOT}/../LibSE/Algorithm/INS/Madgwick"
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Math"
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Process/ekf"
  "${SOLUTION_ROOT}/../LibSE/Algorithm/Process/kalman_filter"
  "${SOLUTION_ROOT}/../LibSE/BSP/CAN"
  "${SOLUTION_ROOT}/../LibSE/BSP/DWT"
  "${SOLUTION_ROOT}/../LibSE/BSP/PWM"
  "${SOLUTION_ROOT}/../LibSE/BSP/SPI"
  "${SOLUTION_ROOT}/../Core/Inc"
  "${SOLUTION_ROOT}/../LibSE/BSP/UART"
  "${SOLUTION_ROOT}/../USB_DEVICE/App"
  "${SOLUTION_ROOT}/../USB_DEVICE/Target"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Bmi088"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Buzzer"
  "${SOLUTION_ROOT}/../Drivers/CMSIS/Include"
  "${SOLUTION_ROOT}/../Drivers/CMSIS/Device/ST/STM32H7xx/Include"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS"
  "${SOLUTION_ROOT}/../Middlewares/Third_Party/FreeRTOS/Source/include"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/DMMotor"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/LKMotor"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Motor/RMMotor"
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/DT7"
  "${SOLUTION_ROOT}/../LibSE/Drivers/RC/VT13"
  "${SOLUTION_ROOT}/../LibSE/Drivers/Referee"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy"
  "${SOLUTION_ROOT}/../Drivers/STM32H7xx_HAL_Driver/Inc"
  "${SOLUTION_ROOT}/../LibSE/Drivers/UI"
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
  "${SOLUTION_ROOT}/../Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
  "${SOLUTION_ROOT}/../LibSE/Drivers/WS2812"
  "${SOLUTION_ROOT}/../LibSE/Module/INS"
  "${SOLUTION_ROOT}/../LibSE/Module/Monitor"
  "${SOLUTION_ROOT}/../LibSE/Task/Inc"
  "${SOLUTION_ROOT}/../LibSE/Task"
)

# Defines
target_compile_definitions(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C,CXX>:
    USE_HAL_DRIVER
    STM32H723xx
    STM32H723xx
    _RTE_
  >
)

# Compile options
target_compile_options(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:ASM>:
    "SHELL:${ASM_CPU}"
    "SHELL:${ASM_FLAGS}"
    "SHELL:${ASM_BYTE_ORDER}"
    "SHELL:-masm=auto"
  >
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:${CC_CPU}"
    "SHELL:${CC_FLAGS}"
    "SHELL:${CC_BYTE_ORDER}"
    "SHELL:-std=c99"
    "SHELL:-fno-rtti"
    "SHELL:-funsigned-char"
    "SHELL:-fshort-enums"
    "SHELL:-fshort-wchar"
    "SHELL:-ffunction-sections"
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
  >
)

# Compile Options Abstractions
add_library(${CONTEXT}_ABSTRACTIONS INTERFACE)
cbuild_set_options_flags(CC "debug" "on" "" "" CC_OPTIONS_FLAGS_${CONTEXT})
cbuild_set_options_flags(ASM "debug" "on" "" "" ASM_OPTIONS_FLAGS_${CONTEXT})
target_compile_options(${CONTEXT}_ABSTRACTIONS INTERFACE
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:${CC_OPTIONS_FLAGS_${CONTEXT}}"
  >
  $<$<COMPILE_LANGUAGE:ASM>:
    "SHELL:${ASM_OPTIONS_FLAGS_${CONTEXT}}"
  >
)

# Add groups and components
include("groups.cmake")
include("components.cmake")

target_link_libraries(${CONTEXT} PUBLIC
  Group_LibSE_Algorithm_Control_ADRC
  Group_LibSE_Algorithm_Control_PID
  Group_LibSE_Algorithm_INS_Madgwick
  Group_LibSE_Algorithm_Math
  Group_LibSE_Algorithm_Process_ekf
  Group_LibSE_Algorithm_Process_kalman_filter
  Group_LibSE_BSP_CAN
  Group_LibSE_BSP_DWT
  Group_LibSE_BSP_PWM
  Group_LibSE_BSP_SPI
  Group_Application_Core
  Group_LibSE_BSP_UART
  Group_Application_USB_Device
  Group_Application_USB_Device_Headers
  Group_Application_USB_Device_Target
  Group_LibSE_Drivers_Bmi088
  Group_LibSE_Drivers_Buzzer
  Group_Application_System
  Group_ThirdParty_CMSIS_Core
  Group_ThirdParty_CMSIS_Device
  Group_ThirdParty_CMSIS_Startup
  Group_ThirdParty_FreeRTOS_Source
  Group_ThirdParty_FreeRTOS_CMSIS_RTOS
  Group_ThirdParty_FreeRTOS_Include
  Group_LibSE_Drivers_Motor
  Group_LibSE_Drivers_Motor_DMMotor
  Group_LibSE_Drivers_Motor_LKMotor
  Group_LibSE_Drivers_Motor_RMMotor
  Group_LibSE_Drivers_RC_DT7
  Group_LibSE_Drivers_RC_VT13
  Group_LibSE_Drivers_Referee
  Group_ThirdParty_STM32H7xx_HAL_Driver_Source
  Group_ThirdParty_STM32H7xx_HAL_Driver_Include
  Group_LibSE_Drivers_UI
  Group_ThirdParty_USB_Device_Library_Source
  Group_ThirdParty_USB_Device_Library_CDC
  Group_ThirdParty_USB_Device_Library_Core
  Group_LibSE_Drivers_WS2812
  Group_LibSE_Module_INS
  Group_LibSE_Module_Monitor
  Group_LibSE_Task_App
  Group_Application_Core_Startup
  Group_Application_FreeRTOS
  Group_LibSE_Task_System
  ARM_CMSIS_CORE_5_6_0
  ARM_CMSIS_DSP_Source_1_16_2
)

# Linker options
target_link_options(${CONTEXT} PUBLIC
  "SHELL:${LD_CPU}"
  "SHELL:${_LS}\"${LD_SCRIPT_PP}\""
  "SHELL:--map"
  "SHELL:--load_addr_map_info"
  "SHELL:--xref"
  "SHELL:--callgraph"
  "SHELL:--symbols"
  "SHELL:--info sizes"
  "SHELL:--info totals"
  "SHELL:--info unused"
  "SHELL:--info veneers"
  "SHELL:--strict"
  "SHELL:--summary_stderr"
  "SHELL:--info summarysizes"
)
set(LD_DEPS ${LD_SCRIPT})
set_target_properties(${CONTEXT} PROPERTIES LINK_DEPENDS "${LD_DEPS}")
