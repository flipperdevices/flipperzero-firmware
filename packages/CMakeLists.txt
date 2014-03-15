cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../gcc_stm32.cmake")
project(stm32_packages)

if (NOT DEFINED TARGET_TRIPLET)
  set(TARGET_TRIPLET "arm-none-eabi")
endif()
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules")

set(STM_ARCHIVE_BASEURL "http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware")
set(STM_ARCHIVE_F1 "stsw-stm32054.zip")
set(STM_ARCHIVE_F4 "stm32f4_dsp_stdperiph_lib.zip")
set(STM_ARCHIVE_VERSION_F1 "3.5.0")
set(STM_ARCHIVE_VERSION_F4 "1.3.0")
set(STM_ARCHIVE_DIR_F1 "STM32F10x_StdPeriph_Lib_V${STM_ARCHIVE_VERSION_F1}")
set(STM_ARCHIVE_DIR_F4 "STM32F4xx_DSP_StdPeriph_Lib_V${STM_ARCHIVE_VERSION_F4}")

set(PACKAGE_TYPE "deb")

if(NOT DEFINED STM32_FAMILY)
  message(FATAL_ERROR "Please specify STM32_FAMILY, eg cmake -DSTM32_FAMILY=F4")
endif()

set(STM_ARCHIVE "${STM_ARCHIVE_${STM32_FAMILY}}")
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${STM_ARCHIVE}") 
  message(STATUS "Using already-downloaded archive: ${STM_ARCHIVE}")
else()
  message(STATUS "From st.com, downloading ${STM_ARCHIVE}")
  file(DOWNLOAD "${STM_ARCHIVE_BASEURL}/${STM_ARCHIVE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${STM_ARCHIVE}" SHOW_PROGRESS)
endif()

set(STM_ARCHIVE_DIR "${STM_ARCHIVE_DIR_${STM32_FAMILY}}")
set(STM_ARCHIVE_FULLDIR "${CMAKE_CURRENT_BINARY_DIR}/${STM_ARCHIVE_DIR}")
if(EXISTS "${STM_ARCHIVE_FULLDIR}")
  message(STATUS "Using already-extracted path: ${STM_ARCHIVE_DIR}")
else()
  execute_process(COMMAND unzip -o ${STM_ARCHIVE})
endif()

add_custom_target(debs ALL)

# Install everything into a subdirectory so that we can package it from there.
set(STM32F1_CHIP_TYPES HD HD_VL MD MD_VL LD LD_VL XL)
set(STM32F4_CHIP_TYPES 401xx 40_41xxx 427_437xx 429_439xx)
set(STM32_CHIP_TYPES "${STM32${STM32_FAMILY}_CHIP_TYPES}")
set(STM32${STM32_FAMILY}_StdPeriphLib_DIR "${STM_ARCHIVE_FULLDIR}")

# Add targets for building cmsis libraries
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install/usr/${TARGET_TRIPLET}/")
add_subdirectory(../cmsis ${CMAKE_CURRENT_BINARY_DIR}/cmsis)

# Add targets for building stdperiph libraries, which need the cmsis headers
get_property(CMSIS_INCLUDE_DIR DIRECTORY ../cmsis PROPERTY INCLUDE_DIRECTORIES)
add_subdirectory(../stdperiph ${CMAKE_CURRENT_BINARY_DIR}/stdperiph)

# Ensure all those get built.
add_custom_target(install_stm32${STM32_FAMILY_LOWER})
add_dependencies(install_stm32${STM32_FAMILY_LOWER}
                 cmsis_${STM32_FAMILY_LOWER}
                 stdperiph_${STM32_FAMILY_LOWER})

set(MODULES_DIR ${CMAKE_CURRENT_BINARY_DIR}/install/usr/share/cmake-2.8/Modules)
add_custom_command(TARGET install_stm32${STM32_FAMILY_LOWER} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -P cmake_install.cmake
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/FindCMSIS.cmake
          ${MODULES_DIR}/FindCMSIS.cmake
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/FindStdPeriphLib.cmake
          ${MODULES_DIR}/FindStdPeriphLib.cmake
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../gcc_stm32.cmake
          ${MODULES_DIR}/gcc_stm32.cmake
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../gcc_stm32${STM32_FAMILY_LOWER}.cmake
          ${MODULES_DIR}/gcc_stm32${STM32_FAMILY_LOWER}.cmake
)

# Create package.
set(PACKAGE_NAME "stm32${STM32_FAMILY_LOWER}-stdperiph")
set(PACKAGE_VERSION "${STM_ARCHIVE_VERSION_${STM32_FAMILY}}")
set(PACKAGE_FILENAME "${PACKAGE_NAME}_${PACKAGE_VERSION}_all.${PACKAGE_TYPE}")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_FILENAME}
  COMMAND fpm -a all -s dir -t ${PACKAGE_TYPE} -n ${PACKAGE_NAME}
              -d gcc-arm-none-eabi
              -v ${PACKAGE_VERSION} -C install usr
)
add_custom_target(${PACKAGE_FILENAME}_
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_FILENAME}
)
add_dependencies(${PACKAGE_FILENAME}_ install_stm32${STM32_FAMILY_LOWER})
add_dependencies(debs ${PACKAGE_FILENAME}_)
