# Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause

# Called after the Raspberry Pi Pico SDK has been initialized to add our libraries

add_library(FreeRTOS-Kernel-Core INTERFACE)
target_sources(FreeRTOS-Kernel-Core INTERFACE
        ${FREERTOS_KERNEL_PATH}/croutine.c
        ${FREERTOS_KERNEL_PATH}/event_groups.c
        ${FREERTOS_KERNEL_PATH}/list.c
        ${FREERTOS_KERNEL_PATH}/queue.c
        ${FREERTOS_KERNEL_PATH}/stream_buffer.c
        ${FREERTOS_KERNEL_PATH}/tasks.c
        ${FREERTOS_KERNEL_PATH}/timers.c
        )
target_include_directories(FreeRTOS-Kernel-Core INTERFACE ${FREERTOS_KERNEL_PATH}/include)

add_library(FreeRTOS-Kernel INTERFACE)
target_sources(FreeRTOS-Kernel INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/port.c
)

target_include_directories(FreeRTOS-Kernel INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(FreeRTOS-Kernel INTERFACE
        FreeRTOS-Kernel-Core
        pico_base_headers
        hardware_exception)

target_compile_definitions(FreeRTOS-Kernel INTERFACE
        LIB_FREERTOS_KERNEL=1
        FREERTOS_KERNEL_SMP=0
)

add_library(FreeRTOS-Kernel-Static INTERFACE)
target_compile_definitions(FreeRTOS-Kernel-Static INTERFACE
        configSUPPORT_STATIC_ALLOCATION=1
        )

target_sources(FreeRTOS-Kernel-Static INTERFACE ${CMAKE_CURRENT_LIST_DIR}/idle_task_static_memory.c)
target_link_libraries(FreeRTOS-Kernel-Static INTERFACE FreeRTOS-Kernel)

add_library(FreeRTOS-Kernel-Heap1 INTERFACE)
target_sources(FreeRTOS-Kernel-Heap1 INTERFACE ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_1.c)
target_link_libraries(FreeRTOS-Kernel-Heap1 INTERFACE FreeRTOS-Kernel)

add_library(FreeRTOS-Kernel-Heap2 INTERFACE)
target_sources(FreeRTOS-Kernel-Heap2 INTERFACE ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_2.c)
target_link_libraries(FreeRTOS-Kernel-Heap2 INTERFACE FreeRTOS-Kernel)

add_library(FreeRTOS-Kernel-Heap3 INTERFACE)
target_sources(FreeRTOS-Kernel-Heap3 INTERFACE ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_3.c)
target_link_libraries(FreeRTOS-Kernel-Heap3 INTERFACE FreeRTOS-Kernel)

add_library(FreeRTOS-Kernel-Heap4 INTERFACE)
target_sources(FreeRTOS-Kernel-Heap4 INTERFACE ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c)
target_link_libraries(FreeRTOS-Kernel-Heap4 INTERFACE FreeRTOS-Kernel)

add_library(FreeRTOS-Kernel-Heap5 INTERFACE)
target_sources(FreeRTOS-Kernel-Heap5 INTERFACE ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_5.c)
target_link_libraries(FreeRTOS-Kernel-Heap5 INTERFACE FreeRTOS-Kernel)
