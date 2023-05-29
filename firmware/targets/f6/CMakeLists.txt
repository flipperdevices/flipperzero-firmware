set(PARENT_TARGET "firmware/targets/f6")
set(ACTUAL_TARGET "firmware/targets/f6")

set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/${ACTUAL_TARGET}/stm32wb55xx_flash_cm4_with_bootloader.ld PARENT_SCOPE)

# All relative to project root
set(PARENT_TARGET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${PARENT_TARGET})
set(TARGET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${ACTUAL_TARGET})
set(CUBE_DIR ${CMAKE_SOURCE_DIR}/lib/STM32CubeWB)

file(GLOB TARGET_SOURCES 
	${TARGET_SOURCE_DIR}/startup_stm32wb55xx_cm4.s

	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_adc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_adc_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_comp.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_cortex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_cryp.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_crc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_crc_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_exti.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_gpio.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_hsem.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_i2c.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_i2c_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_ipcc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pcd.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pcd_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pka.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rng.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rtc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rtc_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_spi.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_spi_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart_ex.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_adc.c
	#${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_usb.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_i2c.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_gpio.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_lptim.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_tim.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_dma.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_usart.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_lpuart.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rcc.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_utils.c
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_spi.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c

	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities/otp.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities/stm_list.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/utilities/dbg_trace.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/svc/Src/svc_ctl.c
	#${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/svc/Src/dis.c
	#${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/svc/Src/hrs.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/template/osal.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_hci_le.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_gap_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_gatt_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/ble/core/auto/ble_hal_aci.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/tl_mbox.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/hci_tl.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/hci_tl_if.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl_if.c
	${CUBE_DIR}/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/shci.c
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c

	${PARENT_TARGET_SOURCE_DIR}/ble-glue/*.c
	${PARENT_TARGET_SOURCE_DIR}/usb-glue/*.c
	${PARENT_TARGET_SOURCE_DIR}/usb-glue/*/*.c
	${TARGET_SOURCE_DIR}/Src/*.c
	${TARGET_SOURCE_DIR}/Src/fatfs/*.c
	${PARENT_TARGET_SOURCE_DIR}/furi-hal/*.c
)

# Look here for header files.
set(TARGET_DIRECTORIES 
    ${TARGET_SOURCE_DIR}/Inc
    ${PARENT_TARGET_SOURCE_DIR}/furi-hal
	${TARGET_SOURCE_DIR}/Src/fatfs
	${PARENT_TARGET_SOURCE_DIR}/ble-glue
	#${PARENT_TARGET_SOURCE_DIR}/usb-glue
	#${PARENT_TARGET_SOURCE_DIR}/usb-glue/class-cdc-dual
	#${CUBE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc

	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Inc
	${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
	${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
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
	BOOT_ADDRESS=0x08000000
	FW_ADDRESS=0x08008000
	OS_OFFSET=0x00008000
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
