;******************** (C) COPYRIGHT 2019 STMicroelectronics ********************
;* File Name          : startup_stm32h7b0xxq.s
;* @author  MCD Application Team
;* Description        : STM32H7xx devices vector table for MDK-ARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler
;*                      - Set the vector table entries with the exceptions ISR address
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>
;******************************************************************************
;* @attention
;*
;* Copyright (c) 2019 STMicroelectronics.
;* All rights reserved.
;*
;* This software component is licensed by ST under BSD 3-Clause license,
;* the "License"; You may not use this file except in compliance with the
;* License. You may obtain a copy of the License at:
;*                        opensource.org/licenses/BSD-3-Clause
;*
;******************************************************************************

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                      ; Top of Stack
                DCD     Reset_Handler                     ; Reset Handler
                DCD     NMI_Handler                       ; NMI Handler
                DCD     HardFault_Handler                 ; Hard Fault Handler
                DCD     MemManage_Handler                 ; MPU Fault Handler
                DCD     BusFault_Handler                  ; Bus Fault Handler
                DCD     UsageFault_Handler                ; Usage Fault Handler
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     SVC_Handler                       ; SVCall Handler
                DCD     DebugMon_Handler                  ; Debug Monitor Handler
                DCD     0                                 ; Reserved
                DCD     PendSV_Handler                    ; PendSV Handler
                DCD     SysTick_Handler                   ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler                   ; Window WatchDog interrupt
                DCD     PVD_PVM_IRQHandler                ; PVD/PVM through EXTI Line detection
                DCD     RTC_TAMP_STAMP_CSS_LSE_IRQHandler ; Tamper and TimeStamps through the EXTI line
                DCD     RTC_WKUP_IRQHandler               ; RTC Wakeup through the EXTI line
                DCD     FLASH_IRQHandler                  ; FLASH
                DCD     RCC_IRQHandler                    ; RCC
                DCD     EXTI0_IRQHandler                  ; EXTI Line0
                DCD     EXTI1_IRQHandler                  ; EXTI Line1
                DCD     EXTI2_IRQHandler                  ; EXTI Line2
                DCD     EXTI3_IRQHandler                  ; EXTI Line3
                DCD     EXTI4_IRQHandler                  ; EXTI Line4
                DCD     DMA1_Stream0_IRQHandler           ; DMA1 Stream 0
                DCD     DMA1_Stream1_IRQHandler           ; DMA1 Stream 1
                DCD     DMA1_Stream2_IRQHandler           ; DMA1 Stream 2
                DCD     DMA1_Stream3_IRQHandler           ; DMA1 Stream 3
                DCD     DMA1_Stream4_IRQHandler           ; DMA1 Stream 4
                DCD     DMA1_Stream5_IRQHandler           ; DMA1 Stream 5
                DCD     DMA1_Stream6_IRQHandler           ; DMA1 Stream 6
                DCD     ADC_IRQHandler                    ; ADC1, ADC2
                DCD     FDCAN1_IT0_IRQHandler             ; FDCAN1 interrupt line 0
                DCD     FDCAN2_IT0_IRQHandler             ; FDCAN2 interrupt line 0
                DCD     FDCAN1_IT1_IRQHandler             ; FDCAN1 interrupt line 1
                DCD     FDCAN2_IT1_IRQHandler             ; FDCAN2 interrupt line 1
                DCD     EXTI9_5_IRQHandler                ; External Line[9:5]s
                DCD     TIM1_BRK_IRQHandler               ; TIM1 Break interrupt
                DCD     TIM1_UP_IRQHandler                ; TIM1 Update Interrupt
                DCD     TIM1_TRG_COM_IRQHandler           ; TIM1 Trigger and Commutation Interrupt
                DCD     TIM1_CC_IRQHandler                ; TIM1 Capture Compare
                DCD     TIM2_IRQHandler                   ; TIM2
                DCD     TIM3_IRQHandler                   ; TIM3
                DCD     TIM4_IRQHandler                   ; TIM4
                DCD     I2C1_EV_IRQHandler                ; I2C1 Event
                DCD     I2C1_ER_IRQHandler                ; I2C1 Error
                DCD     I2C2_EV_IRQHandler                ; I2C2 Event
                DCD     I2C2_ER_IRQHandler                ; I2C2 Error
                DCD     SPI1_IRQHandler                   ; SPI1
                DCD     SPI2_IRQHandler                   ; SPI2
                DCD     USART1_IRQHandler                 ; USART1
                DCD     USART2_IRQHandler                 ; USART2
                DCD     USART3_IRQHandler                 ; USART3
                DCD     EXTI15_10_IRQHandler              ; External Line[15:10]
                DCD     RTC_Alarm_IRQHandler              ; RTC Alarm (A and B) through EXTI Line
                DCD     DFSDM2_IRQHandler                 ; DFSDM2 Interrupt
                DCD     TIM8_BRK_TIM12_IRQHandler         ; TIM8 Break Interrupt and TIM12 global interrupt
                DCD     TIM8_UP_TIM13_IRQHandler          ; TIM8 Update Interrupt and TIM13 global interrupt
                DCD     TIM8_TRG_COM_TIM14_IRQHandler     ; TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt
                DCD     TIM8_CC_IRQHandler                ; TIM8 Capture Compare Interrupt
                DCD     DMA1_Stream7_IRQHandler           ; DMA1 Stream7
                DCD     FMC_IRQHandler                    ; FMC
                DCD     SDMMC1_IRQHandler                 ; SDMMC1
                DCD     TIM5_IRQHandler                   ; TIM5
                DCD     SPI3_IRQHandler                   ; SPI3
                DCD     UART4_IRQHandler                  ; UART4
                DCD     UART5_IRQHandler                  ; UART5
                DCD     TIM6_DAC_IRQHandler               ; TIM6 and DAC1&2 underrun errors
                DCD     TIM7_IRQHandler                   ; TIM7
                DCD     DMA2_Stream0_IRQHandler           ; DMA2 Stream 0
                DCD     DMA2_Stream1_IRQHandler           ; DMA2 Stream 1
                DCD     DMA2_Stream2_IRQHandler           ; DMA2 Stream 2
                DCD     DMA2_Stream3_IRQHandler           ; DMA2 Stream 3
                DCD     DMA2_Stream4_IRQHandler           ; DMA2 Stream 4
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     FDCAN_CAL_IRQHandler              ; FDCAN calibration unit interrupt
                DCD     DFSDM1_FLT4_IRQHandler            ; DFSDM Filter4 Interrupt
                DCD     DFSDM1_FLT5_IRQHandler            ; DFSDM Filter5 Interrupt
                DCD     DFSDM1_FLT6_IRQHandler            ; DFSDM Filter6 Interrupt
                DCD     DFSDM1_FLT7_IRQHandler            ; DFSDM Filter7 Interrupt
                DCD     DMA2_Stream5_IRQHandler           ; DMA2 Stream 5
                DCD     DMA2_Stream6_IRQHandler           ; DMA2 Stream 6
                DCD     DMA2_Stream7_IRQHandler           ; DMA2 Stream 7
                DCD     USART6_IRQHandler                 ; USART6
                DCD     I2C3_EV_IRQHandler                ; I2C3 event
                DCD     I2C3_ER_IRQHandler                ; I2C3 error
                DCD     OTG_HS_EP1_OUT_IRQHandler         ; USB OTG HS End Point 1 Out
                DCD     OTG_HS_EP1_IN_IRQHandler          ; USB OTG HS End Point 1 In
                DCD     OTG_HS_WKUP_IRQHandler            ; USB OTG HS Wakeup through EXTI
                DCD     OTG_HS_IRQHandler                 ; USB OTG HS
                DCD     DCMI_PSSI_IRQHandler              ; DCMI,PSSI
                DCD     CRYP_IRQHandler                   ; CRYP crypto
                DCD     HASH_RNG_IRQHandler               ; Hash and Rng
                DCD     FPU_IRQHandler                    ; FPU
                DCD     UART7_IRQHandler                  ; UART7
                DCD     UART8_IRQHandler                  ; UART8
                DCD     SPI4_IRQHandler                   ; SPI4
                DCD     SPI5_IRQHandler                   ; SPI5
                DCD     SPI6_IRQHandler                   ; SPI6
                DCD     SAI1_IRQHandler                   ; SAI1
                DCD     LTDC_IRQHandler                   ; LTDC
                DCD     LTDC_ER_IRQHandler                ; LTDC error
                DCD     DMA2D_IRQHandler                  ; DMA2D
                DCD     SAI2_IRQHandler                   ; SAI2
                DCD     OCTOSPI1_IRQHandler               ; OCTOSPI1
                DCD     LPTIM1_IRQHandler                 ; LPTIM1
                DCD     CEC_IRQHandler                    ; HDMI_CEC
                DCD     I2C4_EV_IRQHandler                ; I2C4 Event
                DCD     I2C4_ER_IRQHandler                ; I2C4 Error
                DCD     SPDIF_RX_IRQHandler               ; SPDIF_RX
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     DMAMUX1_OVR_IRQHandler            ; DMAMUX1 Overrun interrupt
                DCD     0                                 ;  Reserved
                DCD     0                                 ;  Reserved
                DCD     0                                 ;  Reserved
                DCD     0                                 ;  Reserved
                DCD     0                                 ;  Reserved
                DCD     0                                 ;  Reserved
                DCD     0                                 ;  Reserved
                DCD     DFSDM1_FLT0_IRQHandler            ; DFSDM Filter0 Interrupt
                DCD     DFSDM1_FLT1_IRQHandler            ; DFSDM Filter1 Interrupt
                DCD     DFSDM1_FLT2_IRQHandler            ; DFSDM Filter2 Interrupt
                DCD     DFSDM1_FLT3_IRQHandler            ; DFSDM Filter3 Interrupt
                DCD     0                                 ;  Reserved
                DCD     SWPMI1_IRQHandler                 ;  Serial Wire Interface 1 global interrupt
                DCD     TIM15_IRQHandler                  ;  TIM15 global Interrupt
                DCD     TIM16_IRQHandler                  ;  TIM16 global Interrupt
                DCD     TIM17_IRQHandler                  ;  TIM17 global Interrupt
                DCD     MDIOS_WKUP_IRQHandler             ;  MDIOS Wakeup  Interrupt
                DCD     MDIOS_IRQHandler                  ;  MDIOS global Interrupt
                DCD     JPEG_IRQHandler                   ;  JPEG global Interrupt
                DCD     MDMA_IRQHandler                   ;  MDMA global Interrupt
                DCD     0                                 ;  Reserved
                DCD     SDMMC2_IRQHandler                 ;  SDMMC2 global Interrupt
                DCD     HSEM1_IRQHandler                  ;  HSEM1 global Interrupt
                DCD     0                                 ;  Reserved
                DCD     DAC2_IRQHandler                   ;  DAC2 global Interrupt
                DCD     DMAMUX2_OVR_IRQHandler            ; DMAMUX Overrun interrupt
                DCD     BDMA2_Channel0_IRQHandler         ;  BDMA2 Channel 0 global Interrupt
                DCD     BDMA2_Channel1_IRQHandler         ;  BDMA2 Channel 1 global Interrupt
                DCD     BDMA2_Channel2_IRQHandler         ;  BDMA2 Channel 2 global Interrupt
                DCD     BDMA2_Channel3_IRQHandler         ;  BDMA2 Channel 3 global Interrupt
                DCD     BDMA2_Channel4_IRQHandler         ;  BDMA2 Channel 4 global Interrupt
                DCD     BDMA2_Channel5_IRQHandler         ;  BDMA2 Channel 5 global Interrupt
                DCD     BDMA2_Channel6_IRQHandler         ;  BDMA2 Channel 6 global Interrupt
                DCD     BDMA2_Channel7_IRQHandler         ;  BDMA2 Channel 7 global Interrupt
                DCD     COMP_IRQHandler                   ;  COMP global Interrupt
                DCD     LPTIM2_IRQHandler                 ;  LP TIM2 global interrupt
                DCD     LPTIM3_IRQHandler                 ;  LP TIM3 global interrupt
                DCD     UART9_IRQHandler                  ;  UART9 global interrupt
                DCD     USART10_IRQHandler                ;  USART10 global interrupt
                DCD     LPUART1_IRQHandler                ;  LPUART1 interrupt
                DCD     0                                 ;  Reserved
                DCD     CRS_IRQHandler                    ;  Clock Recovery Global Interrupt
                DCD     ECC_IRQHandler                    ;  ECC_IRQHandler
                DCD     0                                 ;  Reserved
                DCD     DTS_IRQHandler                    ;  DTS
                DCD     0                                 ;  Reserved
                DCD     WAKEUP_PIN_IRQHandler             ;  Interrupt for all 6 wake-up pins
                DCD     OCTOSPI2_IRQHandler               ;  OCTOSPI2
                DCD     OTFDEC1_IRQHandler                ;  OTFDEC1
                DCD     OTFDEC2_IRQHandler                ;  OTFDEC2
                DCD     GFXMMU_IRQHandler                 ;  GFXMMU
                DCD     BDMA1_IRQHandler                  ;  BDMA1


__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler    PROC
                 EXPORT  Reset_Handler                    [WEAK]
        IMPORT  SystemInit
        IMPORT  __main

                 LDR     R0, =SystemInit
                 BLX     R0
                 LDR     R0, =__main
                 BX      R0
                 ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                      [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler                [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler                [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler                 [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler               [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                      [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler                  [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                    [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                   [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  WWDG_IRQHandler                   [WEAK]
                EXPORT  PVD_PVM_IRQHandler                [WEAK]
                EXPORT  RTC_TAMP_STAMP_CSS_LSE_IRQHandler [WEAK]
                EXPORT  RTC_WKUP_IRQHandler               [WEAK]
                EXPORT  FLASH_IRQHandler                  [WEAK]
                EXPORT  RCC_IRQHandler                    [WEAK]
                EXPORT  EXTI0_IRQHandler                  [WEAK]
                EXPORT  EXTI1_IRQHandler                  [WEAK]
                EXPORT  EXTI2_IRQHandler                  [WEAK]
                EXPORT  EXTI3_IRQHandler                  [WEAK]
                EXPORT  EXTI4_IRQHandler                  [WEAK]
                EXPORT  DMA1_Stream0_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream1_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream2_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream3_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream4_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream5_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream6_IRQHandler           [WEAK]
                EXPORT  DMA1_Stream7_IRQHandler           [WEAK]
                EXPORT  ADC_IRQHandler                    [WEAK]
                EXPORT  FDCAN1_IT0_IRQHandler             [WEAK]
                EXPORT  FDCAN2_IT0_IRQHandler             [WEAK]
                EXPORT  FDCAN1_IT1_IRQHandler             [WEAK]
                EXPORT  FDCAN2_IT1_IRQHandler             [WEAK]
                EXPORT  EXTI9_5_IRQHandler                [WEAK]
                EXPORT  TIM1_BRK_IRQHandler               [WEAK]
                EXPORT  TIM1_UP_IRQHandler                [WEAK]
                EXPORT  TIM1_TRG_COM_IRQHandler           [WEAK]
                EXPORT  TIM1_CC_IRQHandler                [WEAK]
                EXPORT  TIM2_IRQHandler                   [WEAK]
                EXPORT  TIM3_IRQHandler                   [WEAK]
                EXPORT  TIM4_IRQHandler                   [WEAK]
                EXPORT  I2C1_EV_IRQHandler                [WEAK]
                EXPORT  I2C1_ER_IRQHandler                [WEAK]
                EXPORT  I2C2_EV_IRQHandler                [WEAK]
                EXPORT  I2C2_ER_IRQHandler                [WEAK]
                EXPORT  SPI1_IRQHandler                   [WEAK]
                EXPORT  SPI2_IRQHandler                   [WEAK]
                EXPORT  USART1_IRQHandler                 [WEAK]
                EXPORT  USART2_IRQHandler                 [WEAK]
                EXPORT  USART3_IRQHandler                 [WEAK]
                EXPORT  EXTI15_10_IRQHandler              [WEAK]
                EXPORT  RTC_Alarm_IRQHandler              [WEAK]
                EXPORT  DFSDM2_IRQHandler                 [WEAK]
                EXPORT  TIM8_BRK_TIM12_IRQHandler         [WEAK]
                EXPORT  TIM8_UP_TIM13_IRQHandler          [WEAK]
                EXPORT  TIM8_TRG_COM_TIM14_IRQHandler     [WEAK]
                EXPORT  TIM8_CC_IRQHandler                [WEAK]
                EXPORT  DMA1_Stream7_IRQHandler           [WEAK]
                EXPORT  FMC_IRQHandler                    [WEAK]
                EXPORT  SDMMC1_IRQHandler                 [WEAK]
                EXPORT  TIM5_IRQHandler                   [WEAK]
                EXPORT  SPI3_IRQHandler                   [WEAK]
                EXPORT  UART4_IRQHandler                  [WEAK]
                EXPORT  UART5_IRQHandler                  [WEAK]
                EXPORT  TIM6_DAC_IRQHandler               [WEAK]
                EXPORT  TIM7_IRQHandler                   [WEAK]
                EXPORT  DMA2_Stream0_IRQHandler           [WEAK]
                EXPORT  DMA2_Stream1_IRQHandler           [WEAK]
                EXPORT  DMA2_Stream2_IRQHandler           [WEAK]
                EXPORT  DMA2_Stream3_IRQHandler           [WEAK]
                EXPORT  DMA2_Stream4_IRQHandler           [WEAK]
                EXPORT  FDCAN_CAL_IRQHandler              [WEAK]
                EXPORT  DFSDM1_FLT4_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT5_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT6_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT7_IRQHandler            [WEAK]
                EXPORT  DMA2_Stream5_IRQHandler           [WEAK]
                EXPORT  DMA2_Stream6_IRQHandler           [WEAK]
                EXPORT  DMA2_Stream7_IRQHandler           [WEAK]
                EXPORT  USART6_IRQHandler                 [WEAK]
                EXPORT  I2C3_EV_IRQHandler                [WEAK]
                EXPORT  I2C3_ER_IRQHandler                [WEAK]
                EXPORT  OTG_HS_EP1_OUT_IRQHandler         [WEAK]
                EXPORT  OTG_HS_EP1_IN_IRQHandler          [WEAK]
                EXPORT  OTG_HS_WKUP_IRQHandler            [WEAK]
                EXPORT  OTG_HS_IRQHandler                 [WEAK]
                EXPORT  DCMI_PSSI_IRQHandler              [WEAK]
                EXPORT  CRYP_IRQHandler                   [WEAK]
                EXPORT  HASH_RNG_IRQHandler               [WEAK]
                EXPORT  FPU_IRQHandler                    [WEAK]
                EXPORT  UART7_IRQHandler                  [WEAK]
                EXPORT  UART8_IRQHandler                  [WEAK]
                EXPORT  SPI4_IRQHandler                   [WEAK]
                EXPORT  SPI5_IRQHandler                   [WEAK]
                EXPORT  SPI6_IRQHandler                   [WEAK]
                EXPORT  SAI1_IRQHandler                   [WEAK]
                EXPORT  LTDC_IRQHandler                   [WEAK]
                EXPORT  LTDC_ER_IRQHandler                [WEAK]
                EXPORT  DMA2D_IRQHandler                  [WEAK]
                EXPORT  SAI2_IRQHandler                   [WEAK]
                EXPORT  OCTOSPI1_IRQHandler               [WEAK]
                EXPORT  LPTIM1_IRQHandler                 [WEAK]
                EXPORT  CEC_IRQHandler                    [WEAK]
                EXPORT  I2C4_EV_IRQHandler                [WEAK]
                EXPORT  I2C4_ER_IRQHandler                [WEAK]
                EXPORT  SPDIF_RX_IRQHandler               [WEAK]
                EXPORT  DMAMUX1_OVR_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT0_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT1_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT2_IRQHandler            [WEAK]
                EXPORT  DFSDM1_FLT3_IRQHandler            [WEAK]
                EXPORT  SWPMI1_IRQHandler                 [WEAK]
                EXPORT  TIM15_IRQHandler                  [WEAK]
                EXPORT  TIM16_IRQHandler                  [WEAK]
                EXPORT  TIM17_IRQHandler                  [WEAK]
                EXPORT  MDIOS_WKUP_IRQHandler             [WEAK]
                EXPORT  MDIOS_IRQHandler                  [WEAK]
                EXPORT  JPEG_IRQHandler                   [WEAK]
                EXPORT  MDMA_IRQHandler                   [WEAK]
                EXPORT  SDMMC2_IRQHandler                 [WEAK]
                EXPORT  HSEM1_IRQHandler                  [WEAK]
                EXPORT  DAC2_IRQHandler                   [WEAK]
                EXPORT  DMAMUX2_OVR_IRQHandler            [WEAK]
                EXPORT  BDMA2_Channel0_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel1_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel2_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel3_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel4_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel5_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel6_IRQHandler         [WEAK]
                EXPORT  BDMA2_Channel7_IRQHandler         [WEAK]
                EXPORT  COMP_IRQHandler                   [WEAK]
                EXPORT  LPTIM2_IRQHandler                 [WEAK]
                EXPORT  LPTIM3_IRQHandler                 [WEAK]
                EXPORT  UART9_IRQHandler                  [WEAK]
                EXPORT  USART10_IRQHandler                [WEAK]
                EXPORT  LPUART1_IRQHandler                [WEAK]
                EXPORT  CRS_IRQHandler                    [WEAK]
                EXPORT  ECC_IRQHandler                    [WEAK]
                EXPORT  DTS_IRQHandler                    [WEAK]
                EXPORT  WAKEUP_PIN_IRQHandler             [WEAK]
                EXPORT  OCTOSPI2_IRQHandler               [WEAK]
                EXPORT  OTFDEC1_IRQHandler                [WEAK]
                EXPORT  OTFDEC2_IRQHandler                [WEAK]
                EXPORT  GFXMMU_IRQHandler                 [WEAK]
                EXPORT  BDMA1_IRQHandler                  [WEAK]


WWDG_IRQHandler
PVD_PVM_IRQHandler
RTC_TAMP_STAMP_CSS_LSE_IRQHandler
RTC_WKUP_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Stream0_IRQHandler
DMA1_Stream1_IRQHandler
DMA1_Stream2_IRQHandler
DMA1_Stream3_IRQHandler
DMA1_Stream4_IRQHandler
DMA1_Stream5_IRQHandler
DMA1_Stream6_IRQHandler
ADC_IRQHandler
FDCAN1_IT0_IRQHandler
FDCAN2_IT0_IRQHandler
FDCAN1_IT1_IRQHandler
FDCAN2_IT1_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
EXTI15_10_IRQHandler
RTC_Alarm_IRQHandler
DFSDM2_IRQHandler
TIM8_BRK_TIM12_IRQHandler
TIM8_UP_TIM13_IRQHandler
TIM8_TRG_COM_TIM14_IRQHandler
TIM8_CC_IRQHandler
DMA1_Stream7_IRQHandler
FMC_IRQHandler
SDMMC1_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_DAC_IRQHandler
TIM7_IRQHandler
DMA2_Stream0_IRQHandler
DMA2_Stream1_IRQHandler
DMA2_Stream2_IRQHandler
DMA2_Stream3_IRQHandler
DMA2_Stream4_IRQHandler
FDCAN_CAL_IRQHandler
DFSDM1_FLT4_IRQHandler
DFSDM1_FLT5_IRQHandler
DFSDM1_FLT6_IRQHandler
DFSDM1_FLT7_IRQHandler
DMA2_Stream5_IRQHandler
DMA2_Stream6_IRQHandler
DMA2_Stream7_IRQHandler
USART6_IRQHandler
I2C3_EV_IRQHandler
I2C3_ER_IRQHandler
OTG_HS_EP1_OUT_IRQHandler
OTG_HS_EP1_IN_IRQHandler
OTG_HS_WKUP_IRQHandler
OTG_HS_IRQHandler
DCMI_PSSI_IRQHandler
CRYP_IRQHandler
HASH_RNG_IRQHandler
FPU_IRQHandler
UART7_IRQHandler
UART8_IRQHandler
SPI4_IRQHandler
SPI5_IRQHandler
SPI6_IRQHandler
SAI1_IRQHandler
LTDC_IRQHandler
LTDC_ER_IRQHandler
DMA2D_IRQHandler
SAI2_IRQHandler
OCTOSPI1_IRQHandler
LPTIM1_IRQHandler
CEC_IRQHandler
I2C4_EV_IRQHandler
I2C4_ER_IRQHandler
SPDIF_RX_IRQHandler
DMAMUX1_OVR_IRQHandler
DFSDM1_FLT0_IRQHandler
DFSDM1_FLT1_IRQHandler
DFSDM1_FLT2_IRQHandler
DFSDM1_FLT3_IRQHandler
SWPMI1_IRQHandler
TIM15_IRQHandler
TIM16_IRQHandler
TIM17_IRQHandler
MDIOS_WKUP_IRQHandler
MDIOS_IRQHandler
JPEG_IRQHandler
MDMA_IRQHandler
SDMMC2_IRQHandler
HSEM1_IRQHandler
DAC2_IRQHandler
DMAMUX2_OVR_IRQHandler
BDMA2_Channel0_IRQHandler
BDMA2_Channel1_IRQHandler
BDMA2_Channel2_IRQHandler
BDMA2_Channel3_IRQHandler
BDMA2_Channel4_IRQHandler
BDMA2_Channel5_IRQHandler
BDMA2_Channel6_IRQHandler
BDMA2_Channel7_IRQHandler
COMP_IRQHandler
LPTIM2_IRQHandler
LPTIM3_IRQHandler
UART9_IRQHandler
USART10_IRQHandler
LPUART1_IRQHandler
CRS_IRQHandler
ECC_IRQHandler
DTS_IRQHandler
WAKEUP_PIN_IRQHandler
OCTOSPI2_IRQHandler
OTFDEC1_IRQHandler
OTFDEC2_IRQHandler
GFXMMU_IRQHandler
BDMA1_IRQHandler

                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE*****
