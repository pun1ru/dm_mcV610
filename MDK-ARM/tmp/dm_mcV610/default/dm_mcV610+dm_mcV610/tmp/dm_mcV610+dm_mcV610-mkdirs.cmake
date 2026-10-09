# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610")
  file(MAKE_DIRECTORY "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610")
endif()
file(MAKE_DIRECTORY
  "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/1"
  "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610"
  "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610/tmp"
  "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610/src/dm_mcV610+dm_mcV610-stamp"
  "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610/src"
  "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610/src/dm_mcV610+dm_mcV610-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610/src/dm_mcV610+dm_mcV610-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Embedded/STM32project/dm_mcV610/MDK-ARM/tmp/dm_mcV610/default/dm_mcV610+dm_mcV610/src/dm_mcV610+dm_mcV610-stamp${cfgdir}") # cfgdir has leading slash
endif()
