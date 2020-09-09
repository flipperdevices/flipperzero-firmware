;/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
;* File Name          : startup_stm32h743xx.s
;* Author             : MCD Application Team
;* Description        : STM32H743xx devices vector table for EWARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR 
;*                        address.
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;*******************************************************************************
;* @attention
;*
;* <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
;* All rights reserved.</center></h2>
;*
;* This software component is licensed by ST under BSD 3-Clause license,
;* the "License"; You may not use this file except in compliance with the
;* License. You may obtain a copy of the License at:
;*                        opensource.org/licenses/BSD-3-Clause
;*
;*******************************************************************************
;
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
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
        DCD     WWDG_IRQHandler                   ; Window WatchDog Interrupt ( wwdg1_it)                                                 
        DCD     PVD_AVD_IRQHandler                ; PVD/AVD through EXTI Line detection                                  
        DCD     TAMP_STAMP_IRQHandler             ; Tamper and TimeStamps through the EXTI line                      
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
        DCD     TIM1_UP_IRQHandler                ; TIM1 Update 
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
        DCD     0                                 ; Reserved                        
        DCD     TIM8_BRK_TIM12_IRQHandler         ; TIM8 Break Interrupt and TIM12 global interrupt      
        DCD     TIM8_UP_TIM13_IRQHandler          ; TIM8 Update Interrupt and TIM13 global interrupt
        DCD     TIM8_TRG_COM_TIM14_IRQHandler     ; TIM8 Trigger and Commutation Interrupt and TIM14 glob
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
        DCD     ETH_IRQHandler                    ; Ethernet                             
        DCD     ETH_WKUP_IRQHandler               ; Ethernet Wakeup through EXTI line                          
        DCD     FDCAN_CAL_IRQHandler              ; FDCAN calibration unit interrupt                                 
        DCD     0                                 ; Reserved                                          
        DCD     0                                 ; Reserved             
        DCD     0                                 ; Reserved             
        DCD     0                                 ; Reserved                           
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
        DCD     DCMI_IRQHandler                   ; DCMI                                 
        DCD     0                                 ; Reserved                                     
        DCD     RNG_IRQHandler                    ; Rng
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
        DCD     QUADSPI_IRQHandler                ; QUADSPI
        DCD     LPTIM1_IRQHandler                 ; LPTIM1
        DCD     CEC_IRQHandler                    ; HDMI_CEC
        DCD     I2C4_EV_IRQHandler                ; I2C4 Event                                         
        DCD     I2C4_ER_IRQHandler                ; I2C4 Error 
        DCD     SPDIF_RX_IRQHandler               ; SPDIF_RX
        DCD     OTG_FS_EP1_OUT_IRQHandler         ; USB OTG FS End Point 1 Out                      
        DCD     OTG_FS_EP1_IN_IRQHandler          ; USB OTG FS End Point 1 In                       
        DCD     OTG_FS_WKUP_IRQHandler            ; USB OTG FS Wakeup through EXTI                         
        DCD     OTG_FS_IRQHandler                 ; USB OTG FS                  
        DCD     DMAMUX1_OVR_IRQHandler            ; DMAMUX1 Overrun interrupt  
        DCD     HRTIM1_Master_IRQHandler          ; HRTIM Master Timer global Interrupts                
        DCD     HRTIM1_TIMA_IRQHandler            ; HRTIM Timer A global Interrupt                      
        DCD     HRTIM1_TIMB_IRQHandler            ; HRTIM Timer B global Interrupt                      
        DCD     HRTIM1_TIMC_IRQHandler            ; HRTIM Timer C global Interrupt                      
        DCD     HRTIM1_TIMD_IRQHandler            ; HRTIM Timer D global Interrupt                      
        DCD     HRTIM1_TIME_IRQHandler            ; HRTIM Timer E global Interrupt                      
        DCD     HRTIM1_FLT_IRQHandler             ; HRTIM Fault global Interrupt 
        DCD     DFSDM1_FLT0_IRQHandler            ; DFSDM Filter0 Interrupt   
        DCD     DFSDM1_FLT1_IRQHandler            ; DFSDM Filter1 Interrupt                              
        DCD     DFSDM1_FLT2_IRQHandler            ; DFSDM Filter2 Interrupt                              
        DCD     DFSDM1_FLT3_IRQHandler            ; DFSDM Filter3 Interrupt                              
        DCD     SAI3_IRQHandler                   ; SAI3 global Interrupt                               
        DCD     SWPMI1_IRQHandler                 ; Serial Wire Interface 1 global interrupt        
        DCD     TIM15_IRQHandler                  ; TIM15 global Interrupt                          
        DCD     TIM16_IRQHandler                  ; TIM16 global Interrupt                          
        DCD     TIM17_IRQHandler                  ; TIM17 global Interrupt                          
        DCD     MDIOS_WKUP_IRQHandler             ; MDIOS Wakeup  Interrupt                         
        DCD     MDIOS_IRQHandler                  ; MDIOS global Interrupt                          
        DCD     JPEG_IRQHandler                   ; JPEG global Interrupt                           
        DCD     MDMA_IRQHandler                   ; MDMA global Interrupt                           
        DCD     0                                 ; Reserved                            
        DCD     SDMMC2_IRQHandler                 ; SDMMC2 global Interrupt                         
        DCD     HSEM1_IRQHandler                  ; HSEM1 global Interrupt                          
        DCD     0                                 ; Reserved                          
        DCD     ADC3_IRQHandler                   ; ADC3 global Interrupt                           
        DCD     DMAMUX2_OVR_IRQHandler            ; DMAMUX Overrun interrupt                         
        DCD     BDMA_Channel0_IRQHandler          ; BDMA Channel 0 global Interrupt                  
        DCD     BDMA_Channel1_IRQHandler          ; BDMA Channel 1 global Interrupt                  
        DCD     BDMA_Channel2_IRQHandler          ; BDMA Channel 2 global Interrupt                  
        DCD     BDMA_Channel3_IRQHandler          ; BDMA Channel 3 global Interrupt                  
        DCD     BDMA_Channel4_IRQHandler          ; BDMA Channel 4 global Interrupt                  
        DCD     BDMA_Channel5_IRQHandler          ; BDMA Channel 5 global Interrupt                  
        DCD     BDMA_Channel6_IRQHandler          ; BDMA Channel 6 global Interrupt                  
        DCD     BDMA_Channel7_IRQHandler          ; BDMA Channel 7 global Interrupt                  
        DCD     COMP1_IRQHandler                  ; COMP1 global Interrupt                          
        DCD     LPTIM2_IRQHandler                 ; LP TIM2 global interrupt                        
        DCD     LPTIM3_IRQHandler                 ; LP TIM3 global interrupt                        
        DCD     LPTIM4_IRQHandler                 ; LP TIM4 global interrupt                        
        DCD     LPTIM5_IRQHandler                 ; LP TIM5 global interrupt                        
        DCD     LPUART1_IRQHandler                ; LP UART1 interrupt                              
        DCD     0                                 ; Reserved              
        DCD     CRS_IRQHandler                    ; Clock Recovery Global Interrupt                 
        DCD     ECC_IRQHandler                    ; ECC diagnostic Global Interrupt
        DCD     SAI4_IRQHandler                   ; SAI4 global interrupt                           
        DCD     0                                 ; Reserved              
        DCD     0                                 ; Reserved                             
        DCD     WAKEUP_PIN_IRQHandler             ; Interrupt for all 6 wake-up pins 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WWDG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
WWDG_IRQHandler  
        B WWDG_IRQHandler

        PUBWEAK PVD_AVD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PVD_AVD_IRQHandler  
        B PVD_AVD_IRQHandler

        PUBWEAK TAMP_STAMP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TAMP_STAMP_IRQHandler  
        B TAMP_STAMP_IRQHandler

        PUBWEAK RTC_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
RTC_WKUP_IRQHandler  
        B RTC_WKUP_IRQHandler

        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FLASH_IRQHandler  
        B FLASH_IRQHandler

        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCC_IRQHandler  
        B RCC_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI0_IRQHandler  
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI1_IRQHandler  
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI2_IRQHandler  
        B EXTI2_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI3_IRQHandler
        B EXTI3_IRQHandler

        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
EXTI4_IRQHandler  
        B EXTI4_IRQHandler

        PUBWEAK DMA1_Stream0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream0_IRQHandler  
        B DMA1_Stream0_IRQHandler

        PUBWEAK DMA1_Stream1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream1_IRQHandler  
        B DMA1_Stream1_IRQHandler

        PUBWEAK DMA1_Stream2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream2_IRQHandler  
        B DMA1_Stream2_IRQHandler

        PUBWEAK DMA1_Stream3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream3_IRQHandler  
        B DMA1_Stream3_IRQHandler

        PUBWEAK DMA1_Stream4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream4_IRQHandler  
        B DMA1_Stream4_IRQHandler

        PUBWEAK DMA1_Stream5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream5_IRQHandler  
        B DMA1_Stream5_IRQHandler

        PUBWEAK DMA1_Stream6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream6_IRQHandler  
        B DMA1_Stream6_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADC_IRQHandler  
        B ADC_IRQHandler

        PUBWEAK FDCAN1_IT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
FDCAN1_IT0_IRQHandler  
        B FDCAN1_IT0_IRQHandler

        PUBWEAK FDCAN2_IT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
FDCAN2_IT0_IRQHandler  
        B FDCAN2_IT0_IRQHandler

        PUBWEAK FDCAN1_IT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
FDCAN1_IT1_IRQHandler  
        B FDCAN1_IT1_IRQHandler

        PUBWEAK FDCAN2_IT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
FDCAN2_IT1_IRQHandler  
        B FDCAN2_IT1_IRQHandler

        PUBWEAK EXTI9_5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
EXTI9_5_IRQHandler  
        B EXTI9_5_IRQHandler

        PUBWEAK TIM1_BRK_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM1_BRK_IRQHandler  
        B TIM1_BRK_IRQHandler

        PUBWEAK TIM1_UP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM1_UP_IRQHandler  
        B TIM1_UP_IRQHandler

        PUBWEAK TIM1_TRG_COM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM1_TRG_COM_IRQHandler  
        B TIM1_TRG_COM_IRQHandler
        
        PUBWEAK TIM1_CC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM1_CC_IRQHandler  
        B TIM1_CC_IRQHandler

        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM2_IRQHandler  
        B TIM2_IRQHandler

        PUBWEAK TIM3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM3_IRQHandler  
        B TIM3_IRQHandler

        PUBWEAK TIM4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM4_IRQHandler  
        B TIM4_IRQHandler

        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C1_EV_IRQHandler  
        B I2C1_EV_IRQHandler

        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C1_ER_IRQHandler  
        B I2C1_ER_IRQHandler

        PUBWEAK I2C2_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C2_EV_IRQHandler  
        B I2C2_EV_IRQHandler

        PUBWEAK I2C2_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C2_ER_IRQHandler  
        B I2C2_ER_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI1_IRQHandler  
        B SPI1_IRQHandler

        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI2_IRQHandler  
        B SPI2_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART1_IRQHandler  
        B USART1_IRQHandler

        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART2_IRQHandler  
        B USART2_IRQHandler

        PUBWEAK USART3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART3_IRQHandler  
        B USART3_IRQHandler

        PUBWEAK EXTI15_10_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)   
EXTI15_10_IRQHandler  
        B EXTI15_10_IRQHandler

        PUBWEAK RTC_Alarm_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)   
RTC_Alarm_IRQHandler  
        B RTC_Alarm_IRQHandler
      
        PUBWEAK TIM8_BRK_TIM12_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM8_BRK_TIM12_IRQHandler  
        B TIM8_BRK_TIM12_IRQHandler

        PUBWEAK TIM8_UP_TIM13_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM8_UP_TIM13_IRQHandler  
        B TIM8_UP_TIM13_IRQHandler

        PUBWEAK TIM8_TRG_COM_TIM14_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
TIM8_TRG_COM_TIM14_IRQHandler  
        B TIM8_TRG_COM_TIM14_IRQHandler

        PUBWEAK TIM8_CC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
TIM8_CC_IRQHandler  
        B TIM8_CC_IRQHandler

        PUBWEAK DMA1_Stream7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_Stream7_IRQHandler  
        B DMA1_Stream7_IRQHandler

        PUBWEAK FMC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FMC_IRQHandler  
        B FMC_IRQHandler

        PUBWEAK SDMMC1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SDMMC1_IRQHandler  
        B SDMMC1_IRQHandler

        PUBWEAK TIM5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM5_IRQHandler  
        B TIM5_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI3_IRQHandler  
        B SPI3_IRQHandler

        PUBWEAK UART4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART4_IRQHandler  
        B UART4_IRQHandler

        PUBWEAK UART5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART5_IRQHandler  
        B UART5_IRQHandler

        PUBWEAK TIM6_DAC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)   
TIM6_DAC_IRQHandler  
        B TIM6_DAC_IRQHandler

        PUBWEAK TIM7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)   
TIM7_IRQHandler  
        B TIM7_IRQHandler

        PUBWEAK DMA2_Stream0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream0_IRQHandler  
        B DMA2_Stream0_IRQHandler

        PUBWEAK DMA2_Stream1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream1_IRQHandler  
        B DMA2_Stream1_IRQHandler

        PUBWEAK DMA2_Stream2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream2_IRQHandler  
        B DMA2_Stream2_IRQHandler

        PUBWEAK DMA2_Stream3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream3_IRQHandler  
        B DMA2_Stream3_IRQHandler

        PUBWEAK DMA2_Stream4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream4_IRQHandler  
        B DMA2_Stream4_IRQHandler

        PUBWEAK ETH_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ETH_IRQHandler  
        B ETH_IRQHandler

        PUBWEAK ETH_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
ETH_WKUP_IRQHandler  
        B ETH_WKUP_IRQHandler

        PUBWEAK FDCAN_CAL_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
FDCAN_CAL_IRQHandler  
        B FDCAN_CAL_IRQHandler

        PUBWEAK DMA2_Stream5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream5_IRQHandler  
        B DMA2_Stream5_IRQHandler

        PUBWEAK DMA2_Stream6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream6_IRQHandler  
        B DMA2_Stream6_IRQHandler

        PUBWEAK DMA2_Stream7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA2_Stream7_IRQHandler  
        B DMA2_Stream7_IRQHandler

        PUBWEAK USART6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART6_IRQHandler  
        B USART6_IRQHandler

        PUBWEAK I2C3_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C3_EV_IRQHandler  
        B I2C3_EV_IRQHandler

        PUBWEAK I2C3_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C3_ER_IRQHandler  
        B I2C3_ER_IRQHandler

        PUBWEAK OTG_HS_EP1_OUT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
OTG_HS_EP1_OUT_IRQHandler  
        B OTG_HS_EP1_OUT_IRQHandler

        PUBWEAK OTG_HS_EP1_IN_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
OTG_HS_EP1_IN_IRQHandler  
        B OTG_HS_EP1_IN_IRQHandler

        PUBWEAK OTG_HS_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
OTG_HS_WKUP_IRQHandler  
        B OTG_HS_WKUP_IRQHandler

        PUBWEAK OTG_HS_IRQHandler                
        SECTION .text:CODE:NOROOT:REORDER(1)
OTG_HS_IRQHandler                  
        B OTG_HS_IRQHandler                

        PUBWEAK DCMI_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DCMI_IRQHandler  
        B DCMI_IRQHandler

        PUBWEAK RNG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
RNG_IRQHandler  
        B RNG_IRQHandler

        PUBWEAK FPU_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
FPU_IRQHandler  
        B FPU_IRQHandler

        PUBWEAK UART7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)      
UART7_IRQHandler 
        B UART7_IRQHandler  

        PUBWEAK UART8_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
UART8_IRQHandler             
        B UART8_IRQHandler
        
        PUBWEAK SPI4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SPI4_IRQHandler
        B SPI4_IRQHandler                 

        PUBWEAK SPI5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SPI5_IRQHandler   
        B SPI5_IRQHandler                  

        PUBWEAK SPI6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SPI6_IRQHandler 
        B SPI6_IRQHandler                    

        PUBWEAK SAI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SAI1_IRQHandler  
        B SAI1_IRQHandler                  

        PUBWEAK LTDC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LTDC_IRQHandler 
        B LTDC_IRQHandler                     

        PUBWEAK LTDC_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LTDC_ER_IRQHandler 
        B LTDC_ER_IRQHandler                 

        PUBWEAK DMA2D_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DMA2D_IRQHandler 
        B DMA2D_IRQHandler                  

       PUBWEAK SAI2_IRQHandler
       SECTION .text:CODE:NOROOT:REORDER(1) 
SAI2_IRQHandler 
        B SAI2_IRQHandler          

       PUBWEAK QUADSPI_IRQHandler
       SECTION .text:CODE:NOROOT:REORDER(1) 
QUADSPI_IRQHandler 
        B QUADSPI_IRQHandler       
        
        PUBWEAK LPTIM1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LPTIM1_IRQHandler 
        B LPTIM1_IRQHandler   
        
        PUBWEAK CEC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
CEC_IRQHandler 
        B CEC_IRQHandler 

        PUBWEAK I2C4_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C4_EV_IRQHandler 
        B I2C4_EV_IRQHandler   
        
        PUBWEAK I2C4_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
I2C4_ER_IRQHandler 
        B I2C4_ER_IRQHandler 
 
        PUBWEAK SPDIF_RX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SPDIF_RX_IRQHandler 
        B SPDIF_RX_IRQHandler 

        PUBWEAK OTG_FS_EP1_OUT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
OTG_FS_EP1_OUT_IRQHandler 
        B OTG_FS_EP1_OUT_IRQHandler 

        PUBWEAK OTG_FS_EP1_IN_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
OTG_FS_EP1_IN_IRQHandler 
        B OTG_FS_EP1_IN_IRQHandler

        PUBWEAK OTG_FS_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
OTG_FS_WKUP_IRQHandler 
        B OTG_FS_WKUP_IRQHandler

        PUBWEAK OTG_FS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
OTG_FS_IRQHandler 
        B OTG_FS_IRQHandler 

        PUBWEAK DMAMUX1_OVR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DMAMUX1_OVR_IRQHandler 
        B DMAMUX1_OVR_IRQHandler 

        PUBWEAK HRTIM1_Master_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_Master_IRQHandler 
        B HRTIM1_Master_IRQHandler 

        PUBWEAK HRTIM1_TIMA_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_TIMA_IRQHandler 
        B HRTIM1_TIMA_IRQHandler 

        PUBWEAK HRTIM1_TIMB_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_TIMB_IRQHandler 
        B HRTIM1_TIMB_IRQHandler 

        PUBWEAK HRTIM1_TIMC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_TIMC_IRQHandler 
        B HRTIM1_TIMC_IRQHandler 

        PUBWEAK HRTIM1_TIMD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_TIMD_IRQHandler 
        B HRTIM1_TIMD_IRQHandler

        PUBWEAK HRTIM1_TIME_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_TIME_IRQHandler 
        B HRTIM1_TIME_IRQHandler

        PUBWEAK HRTIM1_FLT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HRTIM1_FLT_IRQHandler 
        B HRTIM1_FLT_IRQHandler

        PUBWEAK DFSDM1_FLT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DFSDM1_FLT0_IRQHandler 
        B DFSDM1_FLT0_IRQHandler 

        PUBWEAK DFSDM1_FLT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DFSDM1_FLT1_IRQHandler 
        B DFSDM1_FLT1_IRQHandler

        PUBWEAK DFSDM1_FLT2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DFSDM1_FLT2_IRQHandler 
        B DFSDM1_FLT2_IRQHandler

        PUBWEAK DFSDM1_FLT3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DFSDM1_FLT3_IRQHandler 
        B DFSDM1_FLT3_IRQHandler	

        PUBWEAK SAI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SAI3_IRQHandler 
        B SAI3_IRQHandler

        PUBWEAK SWPMI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SWPMI1_IRQHandler 
        B SWPMI1_IRQHandler

        PUBWEAK TIM15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
TIM15_IRQHandler 
        B TIM15_IRQHandler

        PUBWEAK TIM16_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
TIM16_IRQHandler 
        B TIM16_IRQHandler

        PUBWEAK TIM17_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
TIM17_IRQHandler 
        B TIM17_IRQHandler

        PUBWEAK MDIOS_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
MDIOS_WKUP_IRQHandler 
        B MDIOS_WKUP_IRQHandler

        PUBWEAK MDIOS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
MDIOS_IRQHandler 
        B MDIOS_IRQHandler

        PUBWEAK JPEG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
JPEG_IRQHandler 
        B JPEG_IRQHandler

        PUBWEAK MDMA_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
MDMA_IRQHandler 
        B MDMA_IRQHandler

        PUBWEAK SDMMC2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SDMMC2_IRQHandler 
        B SDMMC2_IRQHandler

        PUBWEAK HSEM1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
HSEM1_IRQHandler 
        B HSEM1_IRQHandler

        PUBWEAK ADC3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
ADC3_IRQHandler 
        B ADC3_IRQHandler

        PUBWEAK DMAMUX2_OVR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DMAMUX2_OVR_IRQHandler 
        B DMAMUX2_OVR_IRQHandler

        PUBWEAK BDMA_Channel0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel0_IRQHandler 
        B BDMA_Channel0_IRQHandler

        PUBWEAK BDMA_Channel1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel1_IRQHandler 
        B BDMA_Channel1_IRQHandler

        PUBWEAK BDMA_Channel2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel2_IRQHandler 
        B BDMA_Channel2_IRQHandler

        PUBWEAK BDMA_Channel3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel3_IRQHandler 
        B BDMA_Channel3_IRQHandler

        PUBWEAK BDMA_Channel4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel4_IRQHandler 
        B BDMA_Channel4_IRQHandler

        PUBWEAK BDMA_Channel5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel5_IRQHandler 
        B BDMA_Channel5_IRQHandler

        PUBWEAK BDMA_Channel6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel6_IRQHandler 
        B BDMA_Channel6_IRQHandler

        PUBWEAK BDMA_Channel7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BDMA_Channel7_IRQHandler 
        B BDMA_Channel7_IRQHandler

        PUBWEAK COMP1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
COMP1_IRQHandler 
        B COMP1_IRQHandler

        PUBWEAK LPTIM2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LPTIM2_IRQHandler 
        B LPTIM2_IRQHandler

        PUBWEAK LPTIM3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LPTIM3_IRQHandler 
        B LPTIM3_IRQHandler

        PUBWEAK LPTIM4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LPTIM4_IRQHandler 
        B LPTIM4_IRQHandler

        PUBWEAK LPTIM5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LPTIM5_IRQHandler 
        B LPTIM5_IRQHandler	

        PUBWEAK LPUART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
LPUART1_IRQHandler 
        B LPUART1_IRQHandler

        PUBWEAK CRS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
CRS_IRQHandler 
        B CRS_IRQHandler

        PUBWEAK ECC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
ECC_IRQHandler 
        B ECC_IRQHandler

        PUBWEAK SAI4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
SAI4_IRQHandler 
        B SAI4_IRQHandler

        PUBWEAK WAKEUP_PIN_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
WAKEUP_PIN_IRQHandler 
        B WAKEUP_PIN_IRQHandler 
        END
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
