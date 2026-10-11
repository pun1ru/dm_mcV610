# components.cmake

# component ARM::CMSIS:CORE@5.6.0
add_library(ARM_CMSIS_CORE_5_6_0 INTERFACE)
target_include_directories(ARM_CMSIS_CORE_5_6_0 INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/ARM/CMSIS/5.9.0/CMSIS/Core/Include"
)
target_compile_definitions(ARM_CMSIS_CORE_5_6_0 INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_link_libraries(ARM_CMSIS_CORE_5_6_0 INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# component ARM::CMSIS:DSP&Source@1.16.2
add_library(ARM_CMSIS_DSP_Source_1_16_2 OBJECT
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/BasicMathFunctions/BasicMathFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/BasicMathFunctions/BasicMathFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/BayesFunctions/BayesFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/BayesFunctions/BayesFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/CommonTables/CommonTables.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/CommonTables/CommonTablesF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/ComplexMathFunctions/ComplexMathFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/ComplexMathFunctions/ComplexMathFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/ControllerFunctions/ControllerFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/DistanceFunctions/DistanceFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/DistanceFunctions/DistanceFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/FastMathFunctions/FastMathFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/FastMathFunctions/FastMathFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/FilteringFunctions/FilteringFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/FilteringFunctions/FilteringFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/InterpolationFunctions/InterpolationFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/InterpolationFunctions/InterpolationFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/MatrixFunctions/MatrixFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/MatrixFunctions/MatrixFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/QuaternionMathFunctions/QuaternionMathFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/SVMFunctions/SVMFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/SVMFunctions/SVMFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/StatisticsFunctions/StatisticsFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/StatisticsFunctions/StatisticsFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/SupportFunctions/SupportFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/SupportFunctions/SupportFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/TransformFunctions/TransformFunctions.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/TransformFunctions/TransformFunctionsF16.c"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Source/WindowFunctions/WindowFunctions.c"
)
target_include_directories(ARM_CMSIS_DSP_Source_1_16_2 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/Include"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS-DSP/1.16.2/PrivateInclude"
)
target_compile_definitions(ARM_CMSIS_DSP_Source_1_16_2 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(ARM_CMSIS_DSP_Source_1_16_2 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(ARM_CMSIS_DSP_Source_1_16_2 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)
