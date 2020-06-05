
cmake_minimum_required(VERSION 3.13)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/stm32-cmake/cmake/gcc_stm32.cmake)
set(CMAKE_MODULE_PATH    ${CMAKE_CURRENT_LIST_DIR}/stm32-cmake/cmake)

find_package(CMSIS REQUIRED)
find_package(STM32HAL COMPONENTS gpio tim uart REQUIRED)

add_library(stm_cube_impl ${CMSIS_SOURCES} ${STM32HAL_SOURCES})
target_include_directories(stm_cube_impl PUBLIC ${CMSIS_INCLUDE_DIRS} ${STM32HAL_INCLUDE_DIR})
stm32_set_target_properties(stm_cube_impl)

# stm_cube target is made to propagate properties of stm_cube_impl, as stm32_set_target_properties 
# sets properties privately for given target. This eliminates need to call stm32_set_target_properties
# on every target that links against STM32HAL.
add_library(stm_cube INTERFACE)
target_link_libraries(stm_cube INTERFACE stm_cube_impl)
target_compile_definitions(stm_cube INTERFACE $<TARGET_PROPERTY:stm_cube_impl,COMPILE_DEFINITIONS>)
target_link_options(stm_cube INTERFACE -T${CMAKE_CURRENT_BINARY_DIR}/stm_cube_impl_flash.ld)