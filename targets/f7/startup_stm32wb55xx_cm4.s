.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb


.global g_pfnVectors


/* Globals from the linker script */
.word _sidata   /* start address for the initialization values of the .data section */
.word _sdata    /* start address for the .data section */
.word _edata    /* end   address for the .data section */
.word _sbss     /* start address for the .bss section */
.word _ebss     /* end   address for the .bss section */
.word _sMB_MEM2 /* start address for the .MB_MEM2 section */
.word _eMB_MEM2 /* end   address for the .MB_MEM2 section */


/* INIT_BSS macro is used to fill the specified region [start : end] with zeros */
.macro INIT_BSS start, end
    ldr r0, =\start
    ldr r1, =\end
    movs r3, #0
    bl LoopFillZerobss
.endm


/* INIT_DATA macro is used to copy data in the region [start : end] starting from 'src' */
.macro INIT_DATA start, end, src
    ldr r0, =\start
    ldr r1, =\end
    ldr r2, =\src
    movs r3, #0
    bl LoopCopyDataInit
.endm


.section .text.data_initializers
CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4
LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit
    bx lr

FillZerobss:
    str r3, [r0]
    adds r0, r0, #4
LoopFillZerobss:
    cmp r0, r1
    bcc FillZerobss
    bx lr


.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
.size Reset_Handler, .-Reset_Handler
Reset_Handler:
    /* set stack pointer */
    ldr r0, =_estack
    mov sp, r0
    /* Call the clock system intitialization function.*/
    bl SystemInit
    /* Copy the data segment initializers from flash to SRAM */
    INIT_DATA _sdata, _edata, _sidata
    /* Zero fill the bss segments. */
    INIT_BSS _sbss, _ebss
    INIT_BSS _sMB_MEM2, _eMB_MEM2
    /* Call static constructors */
    bl __libc_init_array
    /* Call the application s entry point.*/
    bl main
LoopForever:
    b LoopForever


.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDG_IRQHandler
    .word PVD_PVM_IRQHandler
    .word TAMP_STAMP_LSECSS_IRQHandler
    .word RTC_WKUP_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA1_Channel1_IRQHandler
    .word DMA1_Channel2_IRQHandler
    .word DMA1_Channel3_IRQHandler
    .word DMA1_Channel4_IRQHandler
    .word DMA1_Channel5_IRQHandler
    .word DMA1_Channel6_IRQHandler
    .word DMA1_Channel7_IRQHandler
    .word ADC1_IRQHandler
    .word USB_HP_IRQHandler
    .word USB_LP_IRQHandler
    .word C2SEV_PWR_C2H_IRQHandler
    .word COMP_IRQHandler
    .word EXTI9_5_IRQHandler
    .word TIM1_BRK_IRQHandler
    .word TIM1_UP_TIM16_IRQHandler
    .word TIM1_TRG_COM_TIM17_IRQHandler
    .word TIM1_CC_IRQHandler
    .word TIM2_IRQHandler
    .word PKA_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word I2C3_EV_IRQHandler
    .word I2C3_ER_IRQHandler
    .word SPI1_IRQHandler
    .word SPI2_IRQHandler
    .word USART1_IRQHandler
    .word LPUART1_IRQHandler
    .word SAI1_IRQHandler
    .word TSC_IRQHandler
    .word EXTI15_10_IRQHandler
    .word RTC_Alarm_IRQHandler
    .word CRS_IRQHandler
    .word PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQHandler
    .word IPCC_C1_RX_IRQHandler
    .word IPCC_C1_TX_IRQHandler
    .word HSEM_IRQHandler
    .word LPTIM1_IRQHandler
    .word LPTIM2_IRQHandler
    .word LCD_IRQHandler
    .word QUADSPI_IRQHandler
    .word AES1_IRQHandler
    .word AES2_IRQHandler
    .word RNG_IRQHandler
    .word FPU_IRQHandler
    .word DMA2_Channel1_IRQHandler
    .word DMA2_Channel2_IRQHandler
    .word DMA2_Channel3_IRQHandler
    .word DMA2_Channel4_IRQHandler
    .word DMA2_Channel5_IRQHandler
    .word DMA2_Channel6_IRQHandler
    .word DMA2_Channel7_IRQHandler
    .word DMAMUX1_OVR_IRQHandler
