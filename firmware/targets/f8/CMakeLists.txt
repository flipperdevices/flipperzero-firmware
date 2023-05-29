set(PARENT_TARGET "firmware/targets/f8")
set(ACTUAL_TARGET "firmware/targets/f8")

set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/${ACTUAL_TARGET}/stm32wb55xx_flash.ld PARENT_SCOPE)

# All relative to project root
set(PARENT_TARGET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${PARENT_TARGET})
set(TARGET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${ACTUAL_TARGET})
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(CUBE_DIR ${LIB_DIR}/STM32CubeWB)

file(GLOB TARGET_SOURCES 
	${TARGET_SOURCE_DIR}/startup_stm32wb55xx_cm4.s

	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_adc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_dma.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_gpio.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_i2c.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_lptim.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rcc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rtc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_spi.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_tim.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_usart.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_lpuart.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_utils.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rng.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_comp.c

	${LIB_DIR}/FreeRTOS-Kernel/event_groups.c
	${LIB_DIR}/FreeRTOS-Kernel/list.c
	${LIB_DIR}/FreeRTOS-Kernel/queue.c
	${LIB_DIR}/FreeRTOS-Kernel/stream_buffer.c
	${LIB_DIR}/FreeRTOS-Kernel/tasks.c
	${LIB_DIR}/FreeRTOS-Kernel/timers.c
	${LIB_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c
	${LIB_DIR}/FreeRTOS-glue/cmsis_os2.c

	${TARGET_SOURCE_DIR}/ble_glue/*.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities/otp.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities/stm_list.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities/dbg_trace.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/svc/Src/svc_ctl.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/template/osal.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_hci_le.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_gap_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_gatt_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_hal_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_l2cap_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/tl_mbox.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/hci_tl.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/hci_tl_if.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl_if.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/shci.c

	${TARGET_SOURCE_DIR}/Src/*.c
	${TARGET_SOURCE_DIR}/fatfs/*.c
	${PARENT_TARGET_SOURCE_DIR}/furi_hal/*.c
)

# Look here for header files.
set(TARGET_DIRECTORIES 
    ${TARGET_SOURCE_DIR}/Inc
    ${PARENT_TARGET_SOURCE_DIR}/furi_hal
	${TARGET_SOURCE_DIR}/fatfs
	${PARENT_TARGET_SOURCE_DIR}/ble_glue
	#${PARENT_TARGET_SOURCE_DIR}/usb-glue
	#${PARENT_TARGET_SOURCE_DIR}/usb-glue/class-cdc-dual
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc

	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Inc
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy
	${LIB_DIR}/FreeRTOS-Kernel/include
	${LIB_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM4F
	${LIB_DIR}/FreeRTOS-glue
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
	${CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32WBxx/Include
	${CUBE_DIR}/Drivers/CMSIS/Device/ST
	${CUBE_DIR}/Drivers/CMSIS/Include
	
	${CUBE_DIR}/Utilities/lpm/tiny_lpm
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/template
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci
)

set(TARGET_FLAGS 
	STM32WB55xx
	STM32WB
	USB_PMASIZE=0x400
	PARENT_SCOPE
)

# add include and source to parent scope
set(SOURCES 
    ${SOURCES} 
    ${TARGET_SOURCES} 
    PARENT_SCOPE
)

set(INCLUDE_DIRECTORIES 
    ${INCLUDE_DIRECTORIES} 
    ${TARGET_DIRECTORIES} 
    PARENT_SCOPE
)
