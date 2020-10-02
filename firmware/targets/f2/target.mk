BOOT_ADDRESS	= 0x08000000
FW_ADDRESS		= 0x08008000
OS_OFFSET		= 0x00008000

BOOT_CFLAGS		= -DBOOT_ADDRESS=$(BOOT_ADDRESS) -DFW_ADDRESS=$(FW_ADDRESS) -DOS_OFFSET=$(OS_OFFSET)
MCU_FLAGS		= -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS			+= $(MCU_FLAGS) $(BOOT_CFLAGS) -DSTM32L476xx -Wall -fdata-sections -ffunction-sections
LDFLAGS			+= $(MCU_FLAGS) -specs=nosys.specs -specs=nano.specs 

CUBE_DIR		= ../lib/STM32CubeL4
# CMSIS
CUBE_CMSIS_DIR	= $(CUBE_DIR)/Drivers/CMSIS
CFLAGS			+= -I$(CUBE_CMSIS_DIR)/Include
CFLAGS			+= -I$(CUBE_CMSIS_DIR)/RTOS2/Include
CFLAGS			+= -I$(CUBE_CMSIS_DIR)/Device/ST/STM32L4xx/Include
ASM_SOURCES		+= $(CUBE_CMSIS_DIR)/Device/ST/STM32L4xx/Source/Templates/gcc/startup_stm32l476xx.s
# HAL
CUBE_HAL_DIR	= $(CUBE_DIR)/Drivers/STM32L4xx_HAL_Driver
CFLAGS			+= -I$(CUBE_HAL_DIR)/Inc
C_SOURCES		+= $(filter-out $(wildcard $(CUBE_HAL_DIR)/Src/*template.c), $(wildcard $(CUBE_HAL_DIR)/Src/*.c))
# Freertos
CUBE_FREERTOS_DIR	= $(CUBE_DIR)/Middlewares/Third_Party/FreeRTOS
CFLAGS			+= -I$(CUBE_FREERTOS_DIR)/Source/include
CFLAGS			+= -I$(CUBE_FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/port.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/portable/MemMang/heap_4.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/CMSIS_RTOS_V2/cmsis_os2.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/tasks.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/queue.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/list.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/timers.c
C_SOURCES		+= $(CUBE_FREERTOS_DIR)/Source/event_groups.c
# USB
CUBE_USB_DIR	= $(CUBE_DIR)/Middlewares/ST/STM32_USB_Device_Library
CFLAGS			+= -I$(CUBE_USB_DIR)/Core/Inc
CFLAGS			+= -I$(CUBE_USB_DIR)/Class/CDC/Inc
C_SOURCES		+= $(CUBE_USB_DIR)/Core/Src/usbd_core.c
C_SOURCES		+= $(CUBE_USB_DIR)/Core/Src/usbd_ctlreq.c
C_SOURCES		+= $(CUBE_USB_DIR)/Core/Src/usbd_ioreq.c
C_SOURCES		+= $(CUBE_USB_DIR)/Class/CDC/Src/usbd_cdc.c
# Common
CFLAGS			+= -Itargets/f2/
CFLAGS			+= \
	-DUSE_HAL_DRIVER \
	-DBUTON_INVERT=false \
	-DDEBUG_UART=huart1
LDFLAGS			+= -Ttargets/f2/STM32L476RGTx_FLASH.ld
