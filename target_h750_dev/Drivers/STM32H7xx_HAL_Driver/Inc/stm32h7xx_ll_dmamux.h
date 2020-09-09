/**
  ******************************************************************************
  * @file    stm32h7xx_ll_dmamux.h
  * @author  MCD Application Team
  * @brief   Header file of DMAMUX LL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H7xx_LL_DMAMUX_H
#define STM32H7xx_LL_DMAMUX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (DMAMUX1) || defined (DMAMUX2)

/** @defgroup DMAMUX_LL DMAMUX
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup DMAMUX_LL_Private_Constants DMAMUX Private Constants
  * @{
  */
/* Define used to get DMAMUX CCR register size */
#define DMAMUX_CCR_SIZE                   0x00000004U

/* Define used to get DMAMUX RGCR register size */
#define DMAMUX_RGCR_SIZE                  0x00000004U

/* Define used to get DMAMUX RequestGenerator offset */
#define DMAMUX_REQ_GEN_OFFSET             (DMAMUX1_RequestGenerator0_BASE - DMAMUX1_BASE)
/* Define used to get DMAMUX Channel Status offset */
#define DMAMUX_CH_STATUS_OFFSET           (DMAMUX1_ChannelStatus_BASE - DMAMUX1_BASE)
/* Define used to get DMAMUX RequestGenerator status offset */
#define DMAMUX_REQ_GEN_STATUS_OFFSET      (DMAMUX1_RequestGenStatus_BASE - DMAMUX1_BASE)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup DMAMUX_LL_Exported_Constants DMAMUX Exported Constants
  * @{
  */
/** @defgroup DMAMUX_LL_EC_CLEAR_FLAG Clear Flags Defines
  * @brief    Flags defines which can be used with LL_DMAMUX_WriteReg function
  * @{
  */
#define LL_DMAMUX_CFR_CSOF0               DMAMUX_CFR_CSOF0       /*!< Synchronization Event Overrun Flag Channel 0   */
#define LL_DMAMUX_CFR_CSOF1               DMAMUX_CFR_CSOF1       /*!< Synchronization Event Overrun Flag Channel 1   */
#define LL_DMAMUX_CFR_CSOF2               DMAMUX_CFR_CSOF2       /*!< Synchronization Event Overrun Flag Channel 2   */
#define LL_DMAMUX_CFR_CSOF3               DMAMUX_CFR_CSOF3       /*!< Synchronization Event Overrun Flag Channel 3   */
#define LL_DMAMUX_CFR_CSOF4               DMAMUX_CFR_CSOF4       /*!< Synchronization Event Overrun Flag Channel 4   */
#define LL_DMAMUX_CFR_CSOF5               DMAMUX_CFR_CSOF5       /*!< Synchronization Event Overrun Flag Channel 5   */
#define LL_DMAMUX_CFR_CSOF6               DMAMUX_CFR_CSOF6       /*!< Synchronization Event Overrun Flag Channel 6   */
#define LL_DMAMUX_CFR_CSOF7               DMAMUX_CFR_CSOF7       /*!< Synchronization Event Overrun Flag Channel 7   */
#define LL_DMAMUX_CFR_CSOF8               DMAMUX_CFR_CSOF8       /*!< Synchronization Event Overrun Flag Channel 8   */
#define LL_DMAMUX_CFR_CSOF9               DMAMUX_CFR_CSOF9       /*!< Synchronization Event Overrun Flag Channel 9   */
#define LL_DMAMUX_CFR_CSOF10              DMAMUX_CFR_CSOF10      /*!< Synchronization Event Overrun Flag Channel 10  */
#define LL_DMAMUX_CFR_CSOF11              DMAMUX_CFR_CSOF11      /*!< Synchronization Event Overrun Flag Channel 11  */
#define LL_DMAMUX_CFR_CSOF12              DMAMUX_CFR_CSOF12      /*!< Synchronization Event Overrun Flag Channel 12  */
#define LL_DMAMUX_CFR_CSOF13              DMAMUX_CFR_CSOF13      /*!< Synchronization Event Overrun Flag Channel 13  */
#define LL_DMAMUX_CFR_CSOF14              DMAMUX_CFR_CSOF14      /*!< Synchronization Event Overrun Flag Channel 14  */
#define LL_DMAMUX_CFR_CSOF15              DMAMUX_CFR_CSOF15      /*!< Synchronization Event Overrun Flag Channel 15  */
#define LL_DMAMUX_RGCFR_RGCOF0            DMAMUX_RGCFR_COF0      /*!< Request Generator 0 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF1            DMAMUX_RGCFR_COF1      /*!< Request Generator 1 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF2            DMAMUX_RGCFR_COF2      /*!< Request Generator 2 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF3            DMAMUX_RGCFR_COF3      /*!< Request Generator 3 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF4            DMAMUX_RGCFR_COF4      /*!< Request Generator 4 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF5            DMAMUX_RGCFR_COF5      /*!< Request Generator 5 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF6            DMAMUX_RGCFR_COF6      /*!< Request Generator 6 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGCFR_RGCOF7            DMAMUX_RGCFR_COF7      /*!< Request Generator 7 Trigger Event Overrun Flag */
/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_DMAMUX_ReadReg function
  * @{
  */
#define LL_DMAMUX_CSR_SOF0                DMAMUX_CSR_SOF0       /*!< Synchronization Event Overrun Flag Channel 0   */
#define LL_DMAMUX_CSR_SOF1                DMAMUX_CSR_SOF1       /*!< Synchronization Event Overrun Flag Channel 1   */
#define LL_DMAMUX_CSR_SOF2                DMAMUX_CSR_SOF2       /*!< Synchronization Event Overrun Flag Channel 2   */
#define LL_DMAMUX_CSR_SOF3                DMAMUX_CSR_SOF3       /*!< Synchronization Event Overrun Flag Channel 3   */
#define LL_DMAMUX_CSR_SOF4                DMAMUX_CSR_SOF4       /*!< Synchronization Event Overrun Flag Channel 4   */
#define LL_DMAMUX_CSR_SOF5                DMAMUX_CSR_SOF5       /*!< Synchronization Event Overrun Flag Channel 5   */
#define LL_DMAMUX_CSR_SOF6                DMAMUX_CSR_SOF6       /*!< Synchronization Event Overrun Flag Channel 6   */
#define LL_DMAMUX_CSR_SOF7                DMAMUX_CSR_SOF7       /*!< Synchronization Event Overrun Flag Channel 7   */
#define LL_DMAMUX_CSR_SOF8                DMAMUX_CSR_SOF8       /*!< Synchronization Event Overrun Flag Channel 8   */
#define LL_DMAMUX_CSR_SOF9                DMAMUX_CSR_SOF9       /*!< Synchronization Event Overrun Flag Channel 9   */
#define LL_DMAMUX_CSR_SOF10               DMAMUX_CSR_SOF10      /*!< Synchronization Event Overrun Flag Channel 10  */
#define LL_DMAMUX_CSR_SOF11               DMAMUX_CSR_SOF11      /*!< Synchronization Event Overrun Flag Channel 11  */
#define LL_DMAMUX_CSR_SOF12               DMAMUX_CSR_SOF12      /*!< Synchronization Event Overrun Flag Channel 12  */
#define LL_DMAMUX_CSR_SOF13               DMAMUX_CSR_SOF13      /*!< Synchronization Event Overrun Flag Channel 13  */
#define LL_DMAMUX_CSR_SOF14               DMAMUX_CSR_SOF14      /*!< Synchronization Event Overrun Flag Channel 14  */
#define LL_DMAMUX_CSR_SOF15               DMAMUX_CSR_SOF15      /*!< Synchronization Event Overrun Flag Channel 15  */
#define LL_DMAMUX_RGSR_RGOF0              DMAMUX_RGSR_OF0       /*!< Request Generator 0 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF1              DMAMUX_RGSR_OF1       /*!< Request Generator 1 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF2              DMAMUX_RGSR_OF2       /*!< Request Generator 2 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF3              DMAMUX_RGSR_OF3       /*!< Request Generator 3 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF4              DMAMUX_RGSR_OF4       /*!< Request Generator 4 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF5              DMAMUX_RGSR_OF5       /*!< Request Generator 5 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF6              DMAMUX_RGSR_OF6       /*!< Request Generator 6 Trigger Event Overrun Flag */
#define LL_DMAMUX_RGSR_RGOF7              DMAMUX_RGSR_OF7       /*!< Request Generator 7 Trigger Event Overrun Flag */
/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_DMA_ReadReg and  LL_DMAMUX_WriteReg functions
  * @{
  */
#define LL_DMAMUX_CCR_SOIE                DMAMUX_CxCR_SOIE          /*!< Synchronization Event Overrun Interrupt               */
#define LL_DMAMUX_RGCR_RGOIE              DMAMUX_RGxCR_OIE          /*!< Request Generation Trigger Event Overrun Interrupt    */
/**
  * @}
  */

/** @defgroup DMAMUX_Request_selection DMAMUX Request selection
  * @brief    DMA Request selection
  * @{
  */
/* DMAMUX1 requests */
#define LL_DMAMUX1_REQ_MEM2MEM          0U   /*!< memory to memory transfer       */
#define LL_DMAMUX1_REQ_GENERATOR0       1U   /*!< DMAMUX1 request generator 0     */
#define LL_DMAMUX1_REQ_GENERATOR1       2U   /*!< DMAMUX1 request generator 1     */
#define LL_DMAMUX1_REQ_GENERATOR2       3U   /*!< DMAMUX1 request generator 2     */
#define LL_DMAMUX1_REQ_GENERATOR3       4U   /*!< DMAMUX1 request generator 3     */
#define LL_DMAMUX1_REQ_GENERATOR4       5U   /*!< DMAMUX1 request generator 4     */
#define LL_DMAMUX1_REQ_GENERATOR5       6U   /*!< DMAMUX1 request generator 5     */
#define LL_DMAMUX1_REQ_GENERATOR6       7U   /*!< DMAMUX1 request generator 6     */
#define LL_DMAMUX1_REQ_GENERATOR7       8U   /*!< DMAMUX1 request generator 7     */
#define LL_DMAMUX1_REQ_ADC1             9U   /*!< DMAMUX1 ADC1 request            */
#define LL_DMAMUX1_REQ_ADC2             10U  /*!< DMAMUX1 ADC2 request            */
#define LL_DMAMUX1_REQ_TIM1_CH1         11U  /*!< DMAMUX1 TIM1 CH1 request        */
#define LL_DMAMUX1_REQ_TIM1_CH2         12U  /*!< DMAMUX1 TIM1 CH2 request        */
#define LL_DMAMUX1_REQ_TIM1_CH3         13U  /*!< DMAMUX1 TIM1 CH3 request        */
#define LL_DMAMUX1_REQ_TIM1_CH4         14U  /*!< DMAMUX1 TIM1 CH4 request        */
#define LL_DMAMUX1_REQ_TIM1_UP          15U  /*!< DMAMUX1 TIM1 UP request         */
#define LL_DMAMUX1_REQ_TIM1_TRIG        16U  /*!< DMAMUX1 TIM1 TRIG request       */
#define LL_DMAMUX1_REQ_TIM1_COM         17U  /*!< DMAMUX1 TIM1 COM request        */
#define LL_DMAMUX1_REQ_TIM2_CH1         18U  /*!< DMAMUX1 TIM2 CH1 request        */
#define LL_DMAMUX1_REQ_TIM2_CH2         19U  /*!< DMAMUX1 TIM2 CH2 request        */
#define LL_DMAMUX1_REQ_TIM2_CH3         20U  /*!< DMAMUX1 TIM2 CH3 request        */
#define LL_DMAMUX1_REQ_TIM2_CH4         21U  /*!< DMAMUX1 TIM2 CH4 request        */
#define LL_DMAMUX1_REQ_TIM2_UP          22U  /*!< DMAMUX1 TIM2 UP request         */
#define LL_DMAMUX1_REQ_TIM3_CH1         23U  /*!< DMAMUX1 TIM3 CH1 request        */
#define LL_DMAMUX1_REQ_TIM3_CH2         24U  /*!< DMAMUX1 TIM3 CH2 request        */
#define LL_DMAMUX1_REQ_TIM3_CH3         25U  /*!< DMAMUX1 TIM3 CH3 request        */
#define LL_DMAMUX1_REQ_TIM3_CH4         26U  /*!< DMAMUX1 TIM3 CH4 request        */
#define LL_DMAMUX1_REQ_TIM3_UP          27U  /*!< DMAMUX1 TIM3 UP request         */
#define LL_DMAMUX1_REQ_TIM3_TRIG        28U  /*!< DMAMUX1 TIM3 TRIG request       */
#define LL_DMAMUX1_REQ_TIM4_CH1         29U  /*!< DMAMUX1 TIM4 CH1 request        */
#define LL_DMAMUX1_REQ_TIM4_CH2         30U  /*!< DMAMUX1 TIM4 CH2 request        */
#define LL_DMAMUX1_REQ_TIM4_CH3         31U  /*!< DMAMUX1 TIM4 CH3 request        */
#define LL_DMAMUX1_REQ_TIM4_UP          32U  /*!< DMAMUX1 TIM4 UP request         */
#define LL_DMAMUX1_REQ_I2C1_RX          33U  /*!< DMAMUX1 I2C1 RX request         */
#define LL_DMAMUX1_REQ_I2C1_TX          34U  /*!< DMAMUX1 I2C1 TX request         */
#define LL_DMAMUX1_REQ_I2C2_RX          35U  /*!< DMAMUX1 I2C2 RX request         */
#define LL_DMAMUX1_REQ_I2C2_TX          36U  /*!< DMAMUX1 I2C2 TX request         */
#define LL_DMAMUX1_REQ_SPI1_RX          37U  /*!< DMAMUX1 SPI1 RX request         */
#define LL_DMAMUX1_REQ_SPI1_TX          38U  /*!< DMAMUX1 SPI1 TX request         */
#define LL_DMAMUX1_REQ_SPI2_RX          39U  /*!< DMAMUX1 SPI2 RX request         */
#define LL_DMAMUX1_REQ_SPI2_TX          40U  /*!< DMAMUX1 SPI2 TX request         */
#define LL_DMAMUX1_REQ_USART1_RX        41U  /*!< DMAMUX1 USART1 RX request       */
#define LL_DMAMUX1_REQ_USART1_TX        42U  /*!< DMAMUX1 USART1 TX request       */
#define LL_DMAMUX1_REQ_USART2_RX        43U  /*!< DMAMUX1 USART2 RX request       */
#define LL_DMAMUX1_REQ_USART2_TX        44U  /*!< DMAMUX1 USART2 TX request       */
#define LL_DMAMUX1_REQ_USART3_RX        45U  /*!< DMAMUX1 USART3 RX request       */
#define LL_DMAMUX1_REQ_USART3_TX        46U  /*!< DMAMUX1 USART3 TX request       */
#define LL_DMAMUX1_REQ_TIM8_CH1         47U  /*!< DMAMUX1 TIM8 CH1 request        */
#define LL_DMAMUX1_REQ_TIM8_CH2         48U  /*!< DMAMUX1 TIM8 CH2 request        */
#define LL_DMAMUX1_REQ_TIM8_CH3         49U  /*!< DMAMUX1 TIM8 CH3 request        */
#define LL_DMAMUX1_REQ_TIM8_CH4         50U  /*!< DMAMUX1 TIM8 CH4 request        */
#define LL_DMAMUX1_REQ_TIM8_UP          51U  /*!< DMAMUX1 TIM8 UP request         */
#define LL_DMAMUX1_REQ_TIM8_TRIG        52U  /*!< DMAMUX1 TIM8 TRIG request       */
#define LL_DMAMUX1_REQ_TIM8_COM         53U  /*!< DMAMUX1 TIM8 COM request        */
#define LL_DMAMUX1_REQ_TIM5_CH1         55U  /*!< DMAMUX1 TIM5 CH1 request        */
#define LL_DMAMUX1_REQ_TIM5_CH2         56U  /*!< DMAMUX1 TIM5 CH2 request        */
#define LL_DMAMUX1_REQ_TIM5_CH3         57U  /*!< DMAMUX1 TIM5 CH3 request        */
#define LL_DMAMUX1_REQ_TIM5_CH4         58U  /*!< DMAMUX1 TIM5 CH4 request        */
#define LL_DMAMUX1_REQ_TIM5_UP          59U  /*!< DMAMUX1 TIM5 UP request         */
#define LL_DMAMUX1_REQ_TIM5_TRIG        60U  /*!< DMAMUX1 TIM5 TRIG request       */
#define LL_DMAMUX1_REQ_SPI3_RX          61U  /*!< DMAMUX1 SPI3 RX request         */
#define LL_DMAMUX1_REQ_SPI3_TX          62U  /*!< DMAMUX1 SPI3 TX request         */
#define LL_DMAMUX1_REQ_UART4_RX         63U  /*!< DMAMUX1 UART4 RX request        */
#define LL_DMAMUX1_REQ_UART4_TX         64U  /*!< DMAMUX1 UART4 TX request        */
#define LL_DMAMUX1_REQ_UART5_RX         65U  /*!< DMAMUX1 UART5 RX request        */
#define LL_DMAMUX1_REQ_UART5_TX         66U  /*!< DMAMUX1 UART5 TX request        */
#define LL_DMAMUX1_REQ_DAC1_CH1         67U  /*!< DMAMUX1 DAC1 Channel 1 request  */
#define LL_DMAMUX1_REQ_DAC1_CH2         68U  /*!< DMAMUX1 DAC1 Channel 2 request  */
#define LL_DMAMUX1_REQ_TIM6_UP          69U  /*!< DMAMUX1 TIM6 UP request         */
#define LL_DMAMUX1_REQ_TIM7_UP          70U  /*!< DMAMUX1 TIM7 UP request         */
#define LL_DMAMUX1_REQ_USART6_RX        71U  /*!< DMAMUX1 USART6 RX request       */
#define LL_DMAMUX1_REQ_USART6_TX        72U  /*!< DMAMUX1 USART6 TX request       */
#define LL_DMAMUX1_REQ_I2C3_RX          73U  /*!< DMAMUX1 I2C3 RX request         */
#define LL_DMAMUX1_REQ_I2C3_TX          74U  /*!< DMAMUX1 I2C3 TX request         */
#if defined (PSSI)
#define LL_DMAMUX1_REQ_DCMI_PSSI        75U  /*!< DMAMUX1 DCMI/PSSI request       */
#define LL_DMAMUX1_REQ_DCMI             LL_DMAMUX1_REQ_DCMI_PSSI /* Legacy define */
#else
#define LL_DMAMUX1_REQ_DCMI             75U  /*!< DMAMUX1 DCMI request            */
#endif /* PSSI */
#define LL_DMAMUX1_REQ_CRYP_IN          76U  /*!< DMAMUX1 CRYP IN request         */
#define LL_DMAMUX1_REQ_CRYP_OUT         77U  /*!< DMAMUX1 CRYP OUT request        */
#define LL_DMAMUX1_REQ_HASH_IN          78U  /*!< DMAMUX1 HASH IN request         */
#define LL_DMAMUX1_REQ_UART7_RX         79U  /*!< DMAMUX1 UART7 RX request        */
#define LL_DMAMUX1_REQ_UART7_TX         80U  /*!< DMAMUX1 UART7 TX request        */
#define LL_DMAMUX1_REQ_UART8_RX         81U  /*!< DMAMUX1 UART8 RX request        */
#define LL_DMAMUX1_REQ_UART8_TX         82U  /*!< DMAMUX1 UART8 TX request        */
#define LL_DMAMUX1_REQ_SPI4_RX          83U  /*!< DMAMUX1 SPI4 RX request         */
#define LL_DMAMUX1_REQ_SPI4_TX          84U  /*!< DMAMUX1 SPI4 TX request         */
#define LL_DMAMUX1_REQ_SPI5_RX          85U  /*!< DMAMUX1 SPI5 RX request         */
#define LL_DMAMUX1_REQ_SPI5_TX          86U  /*!< DMAMUX1 SPI5 TX request         */
#define LL_DMAMUX1_REQ_SAI1_A           87U  /*!< DMAMUX1 SAI1 A request          */
#define LL_DMAMUX1_REQ_SAI1_B           88U  /*!< DMAMUX1 SAI1 B request          */
#define LL_DMAMUX1_REQ_SAI2_A           89U  /*!< DMAMUX1 SAI2 A request          */
#define LL_DMAMUX1_REQ_SAI2_B           90U  /*!< DMAMUX1 SAI2 B request          */
#define LL_DMAMUX1_REQ_SWPMI_RX         91U  /*!< DMAMUX1 SWPMI RX request        */
#define LL_DMAMUX1_REQ_SWPMI_TX         92U  /*!< DMAMUX1 SWPMI TX request        */
#define LL_DMAMUX1_REQ_SPDIF_RX_DT      93U  /*!< DMAMUX1 SPDIF RXDT request      */
#define LL_DMAMUX1_REQ_SPDIF_RX_CS      94U  /*!< DMAMUX1 SPDIF RXCS request      */
#if defined (HRTIM1)
#define LL_DMAMUX1_REQ_HRTIM_MASTER     95U  /*!< DMAMUX1 HRTIM1 Master request 1 */
#define LL_DMAMUX1_REQ_HRTIM_TIMER_A    96U  /*!< DMAMUX1 HRTIM1 TimerA request 2 */
#define LL_DMAMUX1_REQ_HRTIM_TIMER_B    97U  /*!< DMAMUX1 HRTIM1 TimerB request 3 */
#define LL_DMAMUX1_REQ_HRTIM_TIMER_C    98U  /*!< DMAMUX1 HRTIM1 TimerC request 4 */
#define LL_DMAMUX1_REQ_HRTIM_TIMER_D    99U  /*!< DMAMUX1 HRTIM1 TimerD request 5 */
#define LL_DMAMUX1_REQ_HRTIM_TIMER_E   100U  /*!< DMAMUX1 HRTIM1 TimerE request 6 */
#endif /* HRTIM1 */
#define LL_DMAMUX1_REQ_DFSDM1_FLT0     101U  /*!< DMAMUX1 DFSDM1 Filter0 request  */
#define LL_DMAMUX1_REQ_DFSDM1_FLT1     102U  /*!< DMAMUX1 DFSDM1 Filter1 request  */
#define LL_DMAMUX1_REQ_DFSDM1_FLT2     103U  /*!< DMAMUX1 DFSDM1 Filter2 request  */
#define LL_DMAMUX1_REQ_DFSDM1_FLT3     104U  /*!< DMAMUX1 DFSDM1 Filter3 request  */
#define LL_DMAMUX1_REQ_TIM15_CH1       105U  /*!< DMAMUX1 TIM15 CH1 request       */
#define LL_DMAMUX1_REQ_TIM15_UP        106U  /*!< DMAMUX1 TIM15 UP request        */
#define LL_DMAMUX1_REQ_TIM15_TRIG      107U  /*!< DMAMUX1 TIM15 TRIG request      */
#define LL_DMAMUX1_REQ_TIM15_COM       108U  /*!< DMAMUX1 TIM15 COM request       */
#define LL_DMAMUX1_REQ_TIM16_CH1       109U  /*!< DMAMUX1 TIM16 CH1 request       */
#define LL_DMAMUX1_REQ_TIM16_UP        110U  /*!< DMAMUX1 TIM16 UP request        */
#define LL_DMAMUX1_REQ_TIM17_CH1       111U  /*!< DMAMUX1 TIM17 CH1 request       */
#define LL_DMAMUX1_REQ_TIM17_UP        112U  /*!< DMAMUX1 TIM17 UP request        */
#if defined (SAI3)
#define LL_DMAMUX1_REQ_SAI3_A          113U  /*!< DMAMUX1 SAI3 A request          */
#define LL_DMAMUX1_REQ_SAI3_B          114U  /*!< DMAMUX1 SAI3 B request          */
#endif /* SAI3 */
#if defined (ADC3)
#define LL_DMAMUX1_REQ_ADC3            115U  /*!< DMAMUX1 ADC3  request           */
#endif /* ADC3 */
#if defined (UART9)
#define LL_DMAMUX1_REQ_UART9_RX        116U  /*!< DMAMUX1 UART9 RX request        */
#define LL_DMAMUX1_REQ_UART9_TX        117U  /*!< DMAMUX1 UART9 TX request        */
#endif /* UART9 */
#if defined (USART10)
#define LL_DMAMUX1_REQ_USART10_RX      118U  /*!< DMAMUX1 USART10 RX request      */
#define LL_DMAMUX1_REQ_USART10_TX      119U  /*!< DMAMUX1 USART10 TX request      */
#endif /* USART10 */

/* DMAMUX2 requests */
#define LL_DMAMUX2_REQ_MEM2MEM          0U  /*!< memory to memory transfer        */
#define LL_DMAMUX2_REQ_GENERATOR0       1U  /*!< DMAMUX2 request generator 0      */
#define LL_DMAMUX2_REQ_GENERATOR1       2U  /*!< DMAMUX2 request generator 1      */
#define LL_DMAMUX2_REQ_GENERATOR2       3U  /*!< DMAMUX2 request generator 2      */
#define LL_DMAMUX2_REQ_GENERATOR3       4U  /*!< DMAMUX2 request generator 3      */
#define LL_DMAMUX2_REQ_GENERATOR4       5U  /*!< DMAMUX2 request generator 4      */
#define LL_DMAMUX2_REQ_GENERATOR5       6U  /*!< DMAMUX2 request generator 5      */
#define LL_DMAMUX2_REQ_GENERATOR6       7U  /*!< DMAMUX2 request generator 6      */
#define LL_DMAMUX2_REQ_GENERATOR7       8U  /*!< DMAMUX2 request generator 7      */
#define LL_DMAMUX2_REQ_LPUART1_RX       9U  /*!< DMAMUX2 LP_UART1_RX request      */
#define LL_DMAMUX2_REQ_LPUART1_TX      10U  /*!< DMAMUX2 LP_UART1_TX request      */
#define LL_DMAMUX2_REQ_SPI6_RX         11U  /*!< DMAMUX2 SPI6 RX request          */
#define LL_DMAMUX2_REQ_SPI6_TX         12U  /*!< DMAMUX2 SPI6 TX request          */
#define LL_DMAMUX2_REQ_I2C4_RX         13U  /*!< DMAMUX2 I2C4 RX request          */
#define LL_DMAMUX2_REQ_I2C4_TX         14U  /*!< DMAMUX2 I2C4 TX request          */
#if defined (SAI4)
#define LL_DMAMUX2_REQ_SAI4_A          15U  /*!< DMAMUX2 SAI4 A request           */
#define LL_DMAMUX2_REQ_SAI4_B          16U  /*!< DMAMUX2 SAI4 B request           */
#endif /* SAI4 */
#if defined (ADC3)
#define LL_DMAMUX2_REQ_ADC3            17U  /*!< DMAMUX2 ADC3 request             */
#endif /* ADC3 */
#if defined (DAC2)
#define LL_DMAMUX2_REQ_DAC2_CH1        17U  /*!< DMAMUX2 DAC2 CH1 request         */
#endif /* DAC2 */
#if defined (DFSDM2_Channel0)
#define LL_DMAMUX2_REQ_DFSDM2_FLT0     18U  /*!< DMAMUX2 DFSDM2 Filter0 request   */
#endif /* DFSDM2_Channel0 */
/**
  * @}
  */


/** @defgroup DMAMUX_LL_EC_CHANNEL DMAMUX Channel
  * @{
  */
#define LL_DMAMUX_CHANNEL_0     0x00000000U  /*!< DMAMUX1 Channel 0  connected to DMA1 Channel 0 , DMAMUX2 Channel 0 connected to BDMA Channel 0 */
#define LL_DMAMUX_CHANNEL_1     0x00000001U  /*!< DMAMUX1 Channel 1  connected to DMA1 Channel 1 , DMAMUX2 Channel 1 connected to BDMA Channel 1 */
#define LL_DMAMUX_CHANNEL_2     0x00000002U  /*!< DMAMUX1 Channel 2  connected to DMA1 Channel 2 , DMAMUX2 Channel 2 connected to BDMA Channel 2 */
#define LL_DMAMUX_CHANNEL_3     0x00000003U  /*!< DMAMUX1 Channel 3  connected to DMA1 Channel 3 , DMAMUX2 Channel 3 connected to BDMA Channel 3 */
#define LL_DMAMUX_CHANNEL_4     0x00000004U  /*!< DMAMUX1 Channel 4  connected to DMA1 Channel 4 , DMAMUX2 Channel 4 connected to BDMA Channel 4 */
#define LL_DMAMUX_CHANNEL_5     0x00000005U  /*!< DMAMUX1 Channel 5  connected to DMA1 Channel 5 , DMAMUX2 Channel 5 connected to BDMA Channel 5 */
#define LL_DMAMUX_CHANNEL_6     0x00000006U  /*!< DMAMUX1 Channel 6  connected to DMA1 Channel 6 , DMAMUX2 Channel 6 connected to BDMA Channel 6 */
#define LL_DMAMUX_CHANNEL_7     0x00000007U  /*!< DMAMUX1 Channel 7  connected to DMA1 Channel 7 , DMAMUX2 Channel 7 connected to BDMA Channel 7 */
#define LL_DMAMUX_CHANNEL_8     0x00000008U  /*!< DMAMUX1 Channel 8  connected to DMA2 Channel 0 */
#define LL_DMAMUX_CHANNEL_9     0x00000009U  /*!< DMAMUX1 Channel 9  connected to DMA2 Channel 1 */
#define LL_DMAMUX_CHANNEL_10    0x0000000AU  /*!< DMAMUX1 Channel 10 connected to DMA2 Channel 2 */
#define LL_DMAMUX_CHANNEL_11    0x0000000BU  /*!< DMAMUX1 Channel 11 connected to DMA2 Channel 3 */
#define LL_DMAMUX_CHANNEL_12    0x0000000CU  /*!< DMAMUX1 Channel 12 connected to DMA2 Channel 4 */
#define LL_DMAMUX_CHANNEL_13    0x0000000DU  /*!< DMAMUX1 Channel 13 connected to DMA2 Channel 5 */
#define LL_DMAMUX_CHANNEL_14    0x0000000EU  /*!< DMAMUX1 Channel 14 connected to DMA2 Channel 6 */
#define LL_DMAMUX_CHANNEL_15    0x0000000FU  /*!< DMAMUX1 Channel 15 connected to DMA2 Channel 7 */
/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_SYNC_NO Synchronization Signal Polarity
  * @{
  */
#define LL_DMAMUX_SYNC_NO_EVENT            0x00000000U                               /*!< All requests are blocked                            */
#define LL_DMAMUX_SYNC_POL_RISING          DMAMUX_CxCR_SPOL_0                        /*!< Synchronization on event on rising edge             */
#define LL_DMAMUX_SYNC_POL_FALLING         DMAMUX_CxCR_SPOL_1                        /*!< Synchronization on event on falling edge            */
#define LL_DMAMUX_SYNC_POL_RISING_FALLING  (DMAMUX_CxCR_SPOL_0 | DMAMUX_CxCR_SPOL_1) /*!< Synchronization on event on rising and falling edge */
/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_SYNC_EVT Synchronization Signal Event
  * @{
  */
#define LL_DMAMUX1_SYNC_DMAMUX1_CH0_EVT   0x00000000U   /*!< DMAMUX1 synchronization Signal is DMAMUX1 Channel0 Event */
#define LL_DMAMUX1_SYNC_DMAMUX1_CH1_EVT   0x01000000U   /*!< DMAMUX1 synchronization Signal is DMAMUX1 Channel1 Event */
#define LL_DMAMUX1_SYNC_DMAMUX1_CH2_EVT   0x02000000U   /*!< DMAMUX1 synchronization Signal is DMAMUX1 Channel2 Event */
#define LL_DMAMUX1_SYNC_LPTIM1_OUT        0x03000000U   /*!< DMAMUX1 synchronization Signal is LPTIM1 OUT             */
#define LL_DMAMUX1_SYNC_LPTIM2_OUT        0x04000000U   /*!< DMAMUX1 synchronization Signal is LPTIM2 OUT             */
#define LL_DMAMUX1_SYNC_LPTIM3_OUT        0x05000000U   /*!< DMAMUX1 synchronization Signal is LPTIM3 OUT             */
#define LL_DMAMUX1_SYNC_EXTI0             0x06000000U   /*!< DMAMUX1 synchronization Signal is EXTI0 IT               */
#define LL_DMAMUX1_SYNC_TIM12_TRGO        0x07000000U   /*!< DMAMUX1 synchronization Signal is TIM12 TRGO             */

#define LL_DMAMUX2_SYNC_DMAMUX2_CH0_EVT   0x00000000U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel0 Event */
#define LL_DMAMUX2_SYNC_DMAMUX2_CH1_EVT   0x01000000U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel1 Event */
#define LL_DMAMUX2_SYNC_DMAMUX2_CH2_EVT   0x02000000U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel2 Event */
#define LL_DMAMUX2_SYNC_DMAMUX2_CH3_EVT   0x03000000U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel3 Event */
#define LL_DMAMUX2_SYNC_DMAMUX2_CH4_EVT   0x04000000U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel4 Event */
#define LL_DMAMUX2_SYNC_DMAMUX2_CH5_EVT   0x05000000U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel5 Event */
#define LL_DMAMUX2_SYNC_LPUART1_RX_WKUP   0x06000000U   /*!< DMAMUX2 synchronization Signal is LPUART1 RX Wakeup      */
#define LL_DMAMUX2_SYNC_LPUART1_TX_WKUP   0x07000000U   /*!< DMAMUX2 synchronization Signal is LPUART1 TX Wakeup      */
#define LL_DMAMUX2_SYNC_LPTIM2_OUT        0x08000000U   /*!< DMAMUX2 synchronization Signal is LPTIM2 output          */
#define LL_DMAMUX2_SYNC_LPTIM3_OUT        0x09000000U   /*!< DMAMUX2 synchronization Signal is LPTIM3 output          */
#define LL_DMAMUX2_SYNC_I2C4_WKUP         0x0A000000U   /*!< DMAMUX2 synchronization Signal is I2C4 Wakeup            */
#define LL_DMAMUX2_SYNC_SPI6_WKUP         0x0B000000U   /*!< DMAMUX2 synchronization Signal is SPI6 Wakeup            */
#define LL_DMAMUX2_SYNC_COMP1_OUT         0x0C000000U   /*!< DMAMUX2 synchronization Signal is Comparator 1 output    */
#define LL_DMAMUX2_SYNC_RTC_WKUP          0x0D000000U   /*!< DMAMUX2 synchronization Signal is RTC Wakeup             */
#define LL_DMAMUX2_SYNC_EXTI0             0x0E000000U   /*!< DMAMUX2 synchronization Signal is EXTI0 IT               */
#define LL_DMAMUX2_SYNC_EXTI2             0x0F000000U   /*!< DMAMUX2 synchronization Signal is EXTI2 IT               */

/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_REQUEST_GENERATOR Request Generator Channel
  * @{
  */
#define LL_DMAMUX_REQ_GEN_0           0x00000000U
#define LL_DMAMUX_REQ_GEN_1           0x00000001U
#define LL_DMAMUX_REQ_GEN_2           0x00000002U
#define LL_DMAMUX_REQ_GEN_3           0x00000003U
#define LL_DMAMUX_REQ_GEN_4           0x00000004U
#define LL_DMAMUX_REQ_GEN_5           0x00000005U
#define LL_DMAMUX_REQ_GEN_6           0x00000006U
#define LL_DMAMUX_REQ_GEN_7           0x00000007U
/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_REQUEST_GEN_POLARITY External Request Signal Generation Polarity
  * @{
  */
#define LL_DMAMUX_REQ_GEN_NO_EVENT             0x00000000U                                  /*!< No external DMA request  generation                        */
#define LL_DMAMUX_REQ_GEN_POL_RISING           DMAMUX_RGxCR_GPOL_0                          /*!< External DMA request generation on event on rising edge    */
#define LL_DMAMUX_REQ_GEN_POL_FALLING          DMAMUX_RGxCR_GPOL_1                          /*!< External DMA request generation on event on falling edge   */
#define LL_DMAMUX_REQ_GEN_POL_RISING_FALLING   (DMAMUX_RGxCR_GPOL_0 | DMAMUX_RGxCR_GPOL_1)  /*!< External DMA request generation on rising and falling edge */
/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_REQUEST_GEN External Request Signal Generation
  * @{
  */
#define LL_DMAMUX1_REQ_GEN_DMAMUX1_CH0_EVT   0U   /*!< DMAMUX1 Request generator Signal is DMAMUX1 Channel0 Event        */
#define LL_DMAMUX1_REQ_GEN_DMAMUX1_CH1_EVT   1U   /*!< DMAMUX1 Request generator Signal is DMAMUX1 Channel1 Event        */
#define LL_DMAMUX1_REQ_GEN_DMAMUX1_CH2_EVT   2U   /*!< DMAMUX1 Request generator Signal is DMAMUX1 Channel2 Event        */
#define LL_DMAMUX1_REQ_GEN_LPTIM1_OUT        3U   /*!< DMAMUX1 Request generator Signal is LPTIM1 OUT                    */
#define LL_DMAMUX1_REQ_GEN_LPTIM2_OUT        4U   /*!< DMAMUX1 Request generator Signal is LPTIM2 OUT                    */
#define LL_DMAMUX1_REQ_GEN_LPTIM3_OUT        5U   /*!< DMAMUX1 Request generator Signal is LPTIM3 OUT                    */
#define LL_DMAMUX1_REQ_GEN_EXTI0             6U   /*!< DMAMUX1 Request generator Signal is EXTI0 IT                      */
#define LL_DMAMUX1_REQ_GEN_TIM12_TRGO        7U   /*!< DMAMUX1 Request generator Signal is TIM12 TRGO                    */

#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH0_EVT   0U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel0 Event        */
#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH1_EVT   1U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel1 Event        */
#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH2_EVT   2U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel2 Event        */
#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH3_EVT   3U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel3 Event        */
#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH4_EVT   4U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel4 Event        */
#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH5_EVT   5U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel5 Event        */
#define LL_DMAMUX2_REQ_GEN_DMAMUX2_CH6_EVT   6U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel6 Event        */
#define LL_DMAMUX2_REQ_GEN_LPUART1_RX_WKUP   7U   /*!< DMAMUX2 Request generator Signal is LPUART1 RX Wakeup             */
#define LL_DMAMUX2_REQ_GEN_LPUART1_TX_WKUP   8U   /*!< DMAMUX2 Request generator Signal is LPUART1 TX Wakeup             */
#define LL_DMAMUX2_REQ_GEN_LPTIM2_WKUP       9U   /*!< DMAMUX2 Request generator Signal is LPTIM2 Wakeup                 */
#define LL_DMAMUX2_REQ_GEN_LPTIM2_OUT       10U   /*!< DMAMUX2 Request generator Signal is LPTIM2 OUT                    */
#define LL_DMAMUX2_REQ_GEN_LPTIM3_WKUP      11U   /*!< DMAMUX2 Request generator Signal is LPTIM3 Wakeup                 */
#define LL_DMAMUX2_REQ_GEN_LPTIM3_OUT       12U   /*!< DMAMUX2 Request generator Signal is LPTIM3 OUT                    */
#if defined (LPTIM4)
#define LL_DMAMUX2_REQ_GEN_LPTIM4_WKUP      13U   /*!< DMAMUX2 Request generator Signal is LPTIM4 Wakeup                 */
#endif /* LPTIM4 */
#if defined (LPTIM5)
#define LL_DMAMUX2_REQ_GEN_LPTIM5_WKUP      14U   /*!< DMAMUX2 Request generator Signal is LPTIM5 Wakeup                 */
#endif /* LPTIM5 */
#define LL_DMAMUX2_REQ_GEN_I2C4_WKUP        15U   /*!< DMAMUX2 Request generator Signal is I2C4 Wakeup                   */
#define LL_DMAMUX2_REQ_GEN_SPI6_WKUP        16U   /*!< DMAMUX2 Request generator Signal is SPI6 Wakeup                   */
#define LL_DMAMUX2_REQ_GEN_COMP1_OUT        17U   /*!< DMAMUX2 Request generator Signal is Comparator 1 output           */
#define LL_DMAMUX2_REQ_GEN_COMP2_OUT        18U   /*!< DMAMUX2 Request generator Signal is Comparator 2 output           */
#define LL_DMAMUX2_REQ_GEN_RTC_WKUP         19U   /*!< DMAMUX2 Request generator Signal is RTC Wakeup                    */
#define LL_DMAMUX2_REQ_GEN_EXTI0            20U   /*!< DMAMUX2 Request generator Signal is EXTI0                         */
#define LL_DMAMUX2_REQ_GEN_EXTI2            21U   /*!< DMAMUX2 Request generator Signal is EXTI2                         */
#define LL_DMAMUX2_REQ_GEN_I2C4_IT_EVT      22U   /*!< DMAMUX2 Request generator Signal is I2C4 IT Event                 */
#define LL_DMAMUX2_REQ_GEN_SPI6_IT          23U   /*!< DMAMUX2 Request generator Signal is SPI6 IT                       */
#define LL_DMAMUX2_REQ_GEN_LPUART1_TX_IT    24U   /*!< DMAMUX2 Request generator Signal is LPUART1 Tx IT                 */
#define LL_DMAMUX2_REQ_GEN_LPUART1_RX_IT    25U   /*!< DMAMUX2 Request generator Signal is LPUART1 Rx IT                 */
#if defined (ADC3)
#define LL_DMAMUX2_REQ_GEN_ADC3_IT          26U   /*!< DMAMUX2 Request generator Signal is ADC3 IT                       */
#define LL_DMAMUX2_REQ_GEN_ADC3_AWD1_OUT    27U   /*!< DMAMUX2 Request generator Signal is ADC3 Analog Watchdog 1 output */
#endif /* ADC3 */
#define LL_DMAMUX2_REQ_GEN_BDMA_CH0_IT      28U   /*!< DMAMUX2 Request generator Signal is BDMA Channel 0 IT             */
#define LL_DMAMUX2_REQ_GEN_BDMA_CH1_IT      29U   /*!< DMAMUX2 Request generator Signal is BDMA Channel 1 IT             */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup DMAMUX_LL_Exported_Macros DMAMUX Exported Macros
  * @{
  */

/** @defgroup DMAMUX_LL_EM_WRITE_READ Common Write and read registers macros
  * @{
  */
/**
  * @brief  Write a value in DMAMUX register
  * @param  __INSTANCE__ DMAMUX Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_DMAMUX_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in DMAMUX register
  * @param  __INSTANCE__ DMAMUX Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_DMAMUX_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup DMAMUX_LL_Exported_Functions DMAMUX Exported Functions
 * @{
 */

/** @defgroup DMAMUX_LL_EF_Configuration Configuration
  * @{
  */
/**
  * @brief  Set DMAMUX request ID for DMAMUX Channel x.
  * @note   DMAMUX1 channel 0 to 7 are mapped to DMA1 channel 0 to 7.
  *         DMAMUX1 channel 8 to 15 are mapped to DMA2 channel 0 to 7.
  *         DMAMUX2 channel 0 to 7 are mapped to  BDMA channel 0 to 7.
  * @rmtoll CxCR         DMAREQ_ID     LL_DMAMUX_SetRequestID
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @param  Request This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX1_REQ_MEM2MEM
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR0
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR1
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR2
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR3
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR4
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR5
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR6
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR7
  *         @arg @ref LL_DMAMUX1_REQ_ADC1
  *         @arg @ref LL_DMAMUX1_REQ_ADC2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_UP
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_TX
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_TX
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM6_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM7_UP
  *         @arg @ref LL_DMAMUX1_REQ_USART6_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART6_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_TX
  *         @arg @ref LL_DMAMUX1_REQ_DCMI_PSSI
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_IN
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_OUT
  *         @arg @ref LL_DMAMUX1_REQ_HASH_IN
  *         @arg @ref LL_DMAMUX1_REQ_UART7_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART7_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_TX
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_B
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_B
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_RX
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_DT
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_CS
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_MASTER (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_C (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_D (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_E (*)
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT0
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT1
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT2
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT3
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_UP
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_ADC3 (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_TX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_TX (*)
  *         @arg @ref LL_DMAMUX2_REQ_MEM2MEM
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR0
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR1
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR2
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR3
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR4
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR5
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR6
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR7
  *         @arg @ref LL_DMAMUX2_REQ_LPUART1_RX
  *         @arg @ref LL_DMAMUX2_REQ_LPUART1_TX
  *         @arg @ref LL_DMAMUX2_REQ_SPI6_RX
  *         @arg @ref LL_DMAMUX2_REQ_SPI6_TX
  *         @arg @ref LL_DMAMUX2_REQ_I2C4_RX
  *         @arg @ref LL_DMAMUX2_REQ_I2C4_TX
  *         @arg @ref LL_DMAMUX2_REQ_SAI4_A (*)
  *         @arg @ref LL_DMAMUX2_REQ_SAI4_B (*)
  *         @arg @ref LL_DMAMUX2_REQ_ADC3 (*)
  *         @arg @ref LL_DMAMUX2_REQ_DAC2_CH1 (*)
  *         @arg @ref LL_DMAMUX2_REQ_DFSDM2_FLT0 (*)
  * @note   (*) Availability depends on devices.
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetRequestID(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel, uint32_t Request)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_DMAREQ_ID, Request);
}

/**
  * @brief  Get DMAMUX request ID for DMAMUX Channel x.
  * @note   DMAMUX1 channel 0 to 7  are mapped to DMA1 channel 0 to 7.
  *         DMAMUX1 channel 8 to 15 are mapped to DMA2 channel 0 to 7.
  *         DMAMUX2 channel 0 to 7  are mapped to BDMA channel 0 to 7.
  * @rmtoll CxCR         DMAREQ_ID     LL_DMAMUX_GetRequestID
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMAMUX1_REQ_MEM2MEM
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR0
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR1
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR2
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR3
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR4
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR5
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR6
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR7
  *         @arg @ref LL_DMAMUX1_REQ_ADC1
  *         @arg @ref LL_DMAMUX1_REQ_ADC2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_UP
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_TX
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_TX
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM6_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM7_UP
  *         @arg @ref LL_DMAMUX1_REQ_USART6_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART6_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_TX
  *         @arg @ref LL_DMAMUX1_REQ_DCMI_PSSI
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_IN
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_OUT
  *         @arg @ref LL_DMAMUX1_REQ_HASH_IN
  *         @arg @ref LL_DMAMUX1_REQ_UART7_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART7_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_TX
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_B
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_B
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_RX
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_DT
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_CS
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_MASTER (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_C (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_D (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_E (*)
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT0
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT1
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT2
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT3
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_UP
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_ADC3 (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_TX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_TX (*)
  *         @arg @ref LL_DMAMUX2_REQ_MEM2MEM
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR0
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR1
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR2
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR3
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR4
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR5
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR6
  *         @arg @ref LL_DMAMUX2_REQ_GENERATOR7
  *         @arg @ref LL_DMAMUX2_REQ_LPUART1_RX
  *         @arg @ref LL_DMAMUX2_REQ_LPUART1_TX
  *         @arg @ref LL_DMAMUX2_REQ_SPI6_RX
  *         @arg @ref LL_DMAMUX2_REQ_SPI6_TX
  *         @arg @ref LL_DMAMUX2_REQ_I2C4_RX
  *         @arg @ref LL_DMAMUX2_REQ_I2C4_TX
  *         @arg @ref LL_DMAMUX2_REQ_SAI4_A (*)
  *         @arg @ref LL_DMAMUX2_REQ_SAI4_B (*)
  *         @arg @ref LL_DMAMUX2_REQ_ADC3 (*)
  *         @arg @ref LL_DMAMUX2_REQ_DAC2_CH1 (*)
  *         @arg @ref LL_DMAMUX2_REQ_DFSDM2_FLT0 (*)
  * @note   (*) Availability depends on devices.
  * @retval None
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetRequestID(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)(READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_DMAREQ_ID));
}

/**
  * @brief  Set the number of DMA request that will be autorized after a synchronization event and/or the number of DMA request needed to generate an event.
  * @rmtoll CxCR         NBREQ         LL_DMAMUX_SetSyncRequestNb
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @param  RequestNb This parameter must be a value between Min_Data = 1 and Max_Data = 32.
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetSyncRequestNb(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel, uint32_t RequestNb)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_NBREQ, (RequestNb - 1U) << DMAMUX_CxCR_NBREQ_Pos);
}

/**
  * @brief  Get the number of DMA request that will be autorized after a synchronization event and/or the number of DMA request needed to generate an event.
  * @rmtoll CxCR         NBREQ         LL_DMAMUX_GetSyncRequestNb
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval Between Min_Data = 1 and Max_Data = 32
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetSyncRequestNb(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)((READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_NBREQ) >> DMAMUX_CxCR_NBREQ_Pos) + 1U);
}

/**
  * @brief  Set the polarity of the signal on which the DMA request is synchronized.
  * @rmtoll CxCR         SPOL          LL_DMAMUX_SetSyncPolarity
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_SYNC_NO_EVENT
  *         @arg @ref LL_DMAMUX_SYNC_POL_RISING
  *         @arg @ref LL_DMAMUX_SYNC_POL_FALLING
  *         @arg @ref LL_DMAMUX_SYNC_POL_RISING_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetSyncPolarity(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel, uint32_t Polarity)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SPOL, Polarity);
}

/**
  * @brief  Get the polarity of the signal on which the DMA request is synchronized.
  * @rmtoll CxCR         SPOL          LL_DMAMUX_GetSyncPolarity
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMAMUX_SYNC_NO_EVENT
  *         @arg @ref LL_DMAMUX_SYNC_POL_RISING
  *         @arg @ref LL_DMAMUX_SYNC_POL_FALLING
  *         @arg @ref LL_DMAMUX_SYNC_POL_RISING_FALLING
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetSyncPolarity(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)(READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SPOL));
}

/**
  * @brief  Enable the Event Generation on DMAMUX channel x.
  * @rmtoll CxCR         EGE           LL_DMAMUX_EnableEventGeneration
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_EnableEventGeneration(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_EGE);
}

/**
  * @brief  Disable the Event Generation on DMAMUX channel x.
  * @rmtoll CxCR         EGE           LL_DMAMUX_DisableEventGeneration
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_DisableEventGeneration(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  CLEAR_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_EGE);
}

/**
  * @brief  Check if the Event Generation on DMAMUX channel x is enabled or disabled.
  * @rmtoll CxCR         EGE           LL_DMAMUX_IsEnabledEventGeneration
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsEnabledEventGeneration(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_EGE) == (DMAMUX_CxCR_EGE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the synchronization mode.
  * @rmtoll CxCR         SE            LL_DMAMUX_EnableSync
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_EnableSync(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SE);
}

/**
  * @brief  Disable the synchronization mode.
  * @rmtoll CxCR         SE            LL_DMAMUX_DisableSync
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_DisableSync(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  CLEAR_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SE);
}

/**
  * @brief  Check if the synchronization mode is enabled or disabled.
  * @rmtoll CxCR         SE            LL_DMAMUX_IsEnabledSync
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsEnabledSync(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SE) == (DMAMUX_CxCR_SE)) ? 1UL : 0UL);
}

/**
  * @brief  Set DMAMUX synchronization ID  on DMAMUX Channel x.
  * @rmtoll CxCR         SYNC_ID       LL_DMAMUX_SetSyncID
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @param  SyncID This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH0_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH1_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH2_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM1_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_EXTI0
  *         @arg @ref LL_DMAMUX1_SYNC_TIM12_TRGO
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH0_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH1_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH2_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH3_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH4_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH5_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_LPUART1_RX_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_LPUART1_TX_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_I2C4_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_SPI6_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_COMP1_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_RTC_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_EXTI0
  *         @arg @ref LL_DMAMUX2_SYNC_EXTI2
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetSyncID(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel, uint32_t SyncID)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SYNC_ID, SyncID);
}

/**
  * @brief  Get DMAMUX synchronization ID  on DMAMUX Channel x.
  * @rmtoll CxCR         SYNC_ID       LL_DMAMUX_GetSyncID
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH0_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH1_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH2_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM1_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_EXTI0
  *         @arg @ref LL_DMAMUX1_SYNC_TIM12_TRGO
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH0_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH1_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH2_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH3_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH4_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH5_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_LPUART1_RX_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_LPUART1_TX_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_I2C4_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_SPI6_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_COMP1_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_RTC_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_EXTI0
  *         @arg @ref LL_DMAMUX2_SYNC_EXTI2
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetSyncID(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)(READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SYNC_ID));
}

/**
  * @brief  Enable the Request Generator.
  * @rmtoll RGxCR        GE            LL_DMAMUX_EnableRequestGen
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_EnableRequestGen(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * (RequestGenChannel))))->RGCR, DMAMUX_RGxCR_GE);
}

/**
  * @brief  Disable the Request Generator.
  * @rmtoll RGxCR        GE            LL_DMAMUX_DisableRequestGen
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_DisableRequestGen(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  CLEAR_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * (RequestGenChannel))))->RGCR, DMAMUX_RGxCR_GE);
}

/**
  * @brief  Check if the Request Generator is enabled or disabled.
  * @rmtoll RGxCR        GE            LL_DMAMUX_IsEnabledRequestGen
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsEnabledRequestGen(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_GE) == (DMAMUX_RGxCR_GE)) ? 1UL : 0UL);
}

/**
  * @brief  Set the polarity of the signal on which the DMA request is generated.
  * @rmtoll RGxCR        GPOL          LL_DMAMUX_SetRequestGenPolarity
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_NO_EVENT
  *         @arg @ref LL_DMAMUX_REQ_GEN_POL_RISING
  *         @arg @ref LL_DMAMUX_REQ_GEN_POL_FALLING
  *         @arg @ref LL_DMAMUX_REQ_GEN_POL_RISING_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetRequestGenPolarity(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel, uint32_t Polarity)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_GPOL, Polarity);
}

/**
  * @brief  Get the polarity of the signal on which the DMA request is generated.
  * @rmtoll RGxCR        GPOL          LL_DMAMUX_GetRequestGenPolarity
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_NO_EVENT
  *         @arg @ref LL_DMAMUX_REQ_GEN_POL_RISING
  *         @arg @ref LL_DMAMUX_REQ_GEN_POL_FALLING
  *         @arg @ref LL_DMAMUX_REQ_GEN_POL_RISING_FALLING
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetRequestGenPolarity(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)(READ_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_GPOL));
}

/**
  * @brief  Set the number of DMA request that will be autorized after a generation event.
  * @note   This field can only be written when Generator is disabled.
  * @rmtoll RGxCR        GNBREQ        LL_DMAMUX_SetGenRequestNb
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @param  RequestNb This parameter must be a value between Min_Data = 1 and Max_Data = 32.
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetGenRequestNb(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel, uint32_t RequestNb)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_GNBREQ, (RequestNb - 1U) << DMAMUX_RGxCR_GNBREQ_Pos);
}

/**
  * @brief  Get the number of DMA request that will be autorized after a generation event.
  * @rmtoll RGxCR        GNBREQ        LL_DMAMUX_GetGenRequestNb
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval Between Min_Data = 1 and Max_Data = 32
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetGenRequestNb(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)((READ_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_GNBREQ) >> DMAMUX_RGxCR_GNBREQ_Pos) + 1U);
}

/**
  * @brief  Set DMAMUX external Request Signal ID on DMAMUX Request Generation Trigger Event Channel x.
  * @rmtoll RGxCR        SIG_ID        LL_DMAMUX_SetRequestSignalID
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @param  RequestSignalID This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX1_REQ_GEN_DMAMUX1_CH0_EVT
  *         @arg @ref LL_DMAMUX1_REQ_GEN_DMAMUX1_CH1_EVT
  *         @arg @ref LL_DMAMUX1_REQ_GEN_DMAMUX1_CH2_EVT
  *         @arg @ref LL_DMAMUX1_REQ_GEN_LPTIM1_OUT
  *         @arg @ref LL_DMAMUX1_REQ_GEN_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX1_REQ_GEN_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX1_REQ_GEN_EXTI0
  *         @arg @ref LL_DMAMUX1_REQ_GEN_TIM12_TRGO
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH0_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH1_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH2_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH3_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH4_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH5_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_DMAMUX2_CH6_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPUART1_RX_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPUART1_TX_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPTIM2_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPTIM3_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPTIM4_WKUP (*)
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPTIM5_WKUP (*)
  *         @arg @ref LL_DMAMUX2_REQ_GEN_I2C4_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_SPI6_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_COMP1_OUT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_COMP2_OUT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_RTC_WKUP
  *         @arg @ref LL_DMAMUX2_REQ_GEN_EXTI0
  *         @arg @ref LL_DMAMUX2_REQ_GEN_EXTI2
  *         @arg @ref LL_DMAMUX2_REQ_GEN_I2C4_IT_EVT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_SPI6_IT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPUART1_TX_IT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_LPUART1_RX_IT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_ADC3_IT (*)
  *         @arg @ref LL_DMAMUX2_REQ_GEN_ADC3_AWD1_OUT (*)
  *         @arg @ref LL_DMAMUX2_REQ_GEN_BDMA_CH0_IT
  *         @arg @ref LL_DMAMUX2_REQ_GEN_BDMA_CH1_IT
  * @note   (*) Availability depends on devices.
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_SetRequestSignalID(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel, uint32_t RequestSignalID)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  MODIFY_REG(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_SIG_ID, RequestSignalID);
}

/**
  * @brief  Get DMAMUX external Request Signal ID set on DMAMUX Channel x.
  * @rmtoll RGxCR        SIG_ID        LL_DMAMUX_GetRequestSignalID
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH0_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH1_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_DMAMUX1_CH2_EVT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM1_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX1_SYNC_EXTI0
  *         @arg @ref LL_DMAMUX1_SYNC_TIM12_TRGO
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH0_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH1_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH2_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH3_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH4_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_DMAMUX2_CH5_EVT
  *         @arg @ref LL_DMAMUX2_SYNC_LPUART1_RX_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_LPUART1_TX_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_LPTIM2_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_LPTIM3_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_I2C4_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_SPI6_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_COMP1_OUT
  *         @arg @ref LL_DMAMUX2_SYNC_RTC_WKUP
  *         @arg @ref LL_DMAMUX2_SYNC_EXTI0
  *         @arg @ref LL_DMAMUX2_SYNC_EXTI2
  */
__STATIC_INLINE uint32_t LL_DMAMUX_GetRequestSignalID(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (uint32_t)(READ_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_SIG_ID));
}

/**
  * @}
  */

/** @defgroup DMAMUX_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 0.
  * @rmtoll CSR          SOF0          LL_DMAMUX_IsActiveFlag_SO0
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO0(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF0) == (DMAMUX_CSR_SOF0)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 1.
  * @rmtoll CSR          SOF1          LL_DMAMUX_IsActiveFlag_SO1
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO1(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF1) == (DMAMUX_CSR_SOF1)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 2.
  * @rmtoll CSR          SOF2          LL_DMAMUX_IsActiveFlag_SO2
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO2(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF2) == (DMAMUX_CSR_SOF2)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 3.
  * @rmtoll CSR          SOF3          LL_DMAMUX_IsActiveFlag_SO3
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO3(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF3) == (DMAMUX_CSR_SOF3)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 4.
  * @rmtoll CSR          SOF4          LL_DMAMUX_IsActiveFlag_SO4
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO4(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF4) == (DMAMUX_CSR_SOF4)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 5.
  * @rmtoll CSR          SOF5          LL_DMAMUX_IsActiveFlag_SO5
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO5(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF5) == (DMAMUX_CSR_SOF5)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 6.
  * @rmtoll CSR          SOF6          LL_DMAMUX_IsActiveFlag_SO6
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO6(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF6) == (DMAMUX_CSR_SOF6)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 7.
  * @rmtoll CSR          SOF7          LL_DMAMUX_IsActiveFlag_SO7
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO7(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF7) == (DMAMUX_CSR_SOF7)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 8.
  * @rmtoll CSR          SOF8          LL_DMAMUX_IsActiveFlag_SO8
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO8(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF8) == (DMAMUX_CSR_SOF8)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 9.
  * @rmtoll CSR          SOF9          LL_DMAMUX_IsActiveFlag_SO9
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO9(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF9) == (DMAMUX_CSR_SOF9)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 10.
  * @rmtoll CSR          SOF10         LL_DMAMUX_IsActiveFlag_SO10
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO10(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF10) == (DMAMUX_CSR_SOF10)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 11.
  * @rmtoll CSR          SOF11         LL_DMAMUX_IsActiveFlag_SO11
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO11(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF11) == (DMAMUX_CSR_SOF11)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 12.
  * @rmtoll CSR          SOF12         LL_DMAMUX_IsActiveFlag_SO12
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO12(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF12) == (DMAMUX_CSR_SOF12)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 13.
  * @rmtoll CSR          SOF13         LL_DMAMUX_IsActiveFlag_SO13
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO13(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF13) == (DMAMUX_CSR_SOF13)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 14.
  * @rmtoll CSR          SOF14         LL_DMAMUX_IsActiveFlag_SO14
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO14(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF14) == (DMAMUX_CSR_SOF14)) ? 1UL : 0UL);
}

/**
  * @brief  Get Synchronization Event Overrun Flag Channel 15.
  * @rmtoll CSR          SOF15         LL_DMAMUX_IsActiveFlag_SO15
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_SO15(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CSR, DMAMUX_CSR_SOF15) == (DMAMUX_CSR_SOF15)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 0 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF0           LL_DMAMUX_IsActiveFlag_RGO0
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO0(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF0) == (DMAMUX_RGSR_OF0)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 1 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF1           LL_DMAMUX_IsActiveFlag_RGO1
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO1(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF1) == (DMAMUX_RGSR_OF1)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 2 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF2           LL_DMAMUX_IsActiveFlag_RGO2
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO2(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF2) == (DMAMUX_RGSR_OF2)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 3 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF3           LL_DMAMUX_IsActiveFlag_RGO3
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO3(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF3) == (DMAMUX_RGSR_OF3)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 4 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF4           LL_DMAMUX_IsActiveFlag_RGO4
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO4(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF4) == (DMAMUX_RGSR_OF4)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 5 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF5           LL_DMAMUX_IsActiveFlag_RGO5
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO5(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF5) == (DMAMUX_RGSR_OF5)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 6 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF6           LL_DMAMUX_IsActiveFlag_RGO6
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO6(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF6) == (DMAMUX_RGSR_OF6)) ? 1UL : 0UL);
}

/**
  * @brief  Get Request Generator 7 Trigger Event Overrun Flag.
  * @rmtoll RGSR         OF7           LL_DMAMUX_IsActiveFlag_RGO7
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsActiveFlag_RGO7(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGSR, DMAMUX_RGSR_OF7) == (DMAMUX_RGSR_OF7)) ? 1UL : 0UL);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 0.
  * @rmtoll CFR          CSOF0         LL_DMAMUX_ClearFlag_SO0
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO0(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF0);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 1.
  * @rmtoll CFR          CSOF1         LL_DMAMUX_ClearFlag_SO1
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO1(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF1);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 2.
  * @rmtoll CFR          CSOF2         LL_DMAMUX_ClearFlag_SO2
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO2(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF2);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 3.
  * @rmtoll CFR          CSOF3         LL_DMAMUX_ClearFlag_SO3
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO3(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF3);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 4.
  * @rmtoll CFR          CSOF4         LL_DMAMUX_ClearFlag_SO4
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO4(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF4);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 5.
  * @rmtoll CFR          CSOF5         LL_DMAMUX_ClearFlag_SO5
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO5(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF5);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 6.
  * @rmtoll CFR          CSOF6         LL_DMAMUX_ClearFlag_SO6
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO6(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF6);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 7.
  * @rmtoll CFR          CSOF7         LL_DMAMUX_ClearFlag_SO7
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO7(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF7);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 8.
  * @rmtoll CFR          CSOF8         LL_DMAMUX_ClearFlag_SO8
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO8(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF8);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 9.
  * @rmtoll CFR          CSOF9         LL_DMAMUX_ClearFlag_SO9
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO9(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF9);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 10.
  * @rmtoll CFR          CSOF10        LL_DMAMUX_ClearFlag_SO10
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO10(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF10);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 11.
  * @rmtoll CFR          CSOF11        LL_DMAMUX_ClearFlag_SO11
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO11(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF11);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 12.
  * @rmtoll CFR          CSOF12        LL_DMAMUX_ClearFlag_SO12
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO12(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF12);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 13.
  * @rmtoll CFR          CSOF13        LL_DMAMUX_ClearFlag_SO13
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO13(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF13);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 14.
  * @rmtoll CFR          CSOF14        LL_DMAMUX_ClearFlag_SO14
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO14(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF14);
}

/**
  * @brief  Clear Synchronization Event Overrun Flag Channel 15.
  * @rmtoll CFR          CSOF15        LL_DMAMUX_ClearFlag_SO15
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_SO15(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_ChannelStatus_TypeDef *)(dmamux_base_addr + DMAMUX_CH_STATUS_OFFSET))->CFR, DMAMUX_CFR_CSOF15);
}

/**
  * @brief  Clear Request Generator 0 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF0          LL_DMAMUX_ClearFlag_RGO0
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO0(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF0);
}

/**
  * @brief  Clear Request Generator 1 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF1          LL_DMAMUX_ClearFlag_RGO1
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO1(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF1);
}

/**
  * @brief  Clear Request Generator 2 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF2          LL_DMAMUX_ClearFlag_RGO2
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO2(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF2);
}

/**
  * @brief  Clear Request Generator 3 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF3          LL_DMAMUX_ClearFlag_RGO3
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO3(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF3);
}

/**
  * @brief  Clear Request Generator 4 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF4          LL_DMAMUX_ClearFlag_RGO4
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO4(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF4);
}

/**
  * @brief  Clear Request Generator 5 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF5          LL_DMAMUX_ClearFlag_RGO5
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO5(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF5);
}

/**
  * @brief  Clear Request Generator 6 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF6          LL_DMAMUX_ClearFlag_RGO6
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO6(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF6);
}

/**
  * @brief  Clear Request Generator 7 Trigger Event Overrun Flag.
  * @rmtoll RGCFR        COF7          LL_DMAMUX_ClearFlag_RGO7
  * @param  DMAMUXx DMAMUXx DMAMUXx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_ClearFlag_RGO7(DMAMUX_Channel_TypeDef *DMAMUXx)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGenStatus_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_STATUS_OFFSET))->RGCFR, DMAMUX_RGCFR_COF7);
}

/**
  * @}
  */

/** @defgroup DMAMUX_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief  Enable the Synchronization Event Overrun Interrupt on DMAMUX channel x.
  * @rmtoll CxCR         SOIE          LL_DMAMUX_EnableIT_SO
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_EnableIT_SO(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_Channel_TypeDef *)((uint32_t)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel)))))->CCR, DMAMUX_CxCR_SOIE);
}

/**
  * @brief  Disable the Synchronization Event Overrun Interrupt on DMAMUX channel x.
  * @rmtoll CxCR         SOIE          LL_DMAMUX_DisableIT_SO
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_DisableIT_SO(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  CLEAR_BIT(((DMAMUX_Channel_TypeDef *)((uint32_t)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel)))))->CCR, DMAMUX_CxCR_SOIE);
}

/**
  * @brief  Check if the Synchronization Event Overrun Interrupt on DMAMUX channel x is enabled or disabled.
  * @rmtoll CxCR         SOIE          LL_DMAMUX_IsEnabledIT_SO
  * @param  DMAMUXx DMAMUXx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_CHANNEL_0
  *         @arg @ref LL_DMAMUX_CHANNEL_1
  *         @arg @ref LL_DMAMUX_CHANNEL_2
  *         @arg @ref LL_DMAMUX_CHANNEL_3
  *         @arg @ref LL_DMAMUX_CHANNEL_4
  *         @arg @ref LL_DMAMUX_CHANNEL_5
  *         @arg @ref LL_DMAMUX_CHANNEL_6
  *         @arg @ref LL_DMAMUX_CHANNEL_7
  *         @arg @ref LL_DMAMUX_CHANNEL_8
  *         @arg @ref LL_DMAMUX_CHANNEL_9
  *         @arg @ref LL_DMAMUX_CHANNEL_10
  *         @arg @ref LL_DMAMUX_CHANNEL_11
  *         @arg @ref LL_DMAMUX_CHANNEL_12
  *         @arg @ref LL_DMAMUX_CHANNEL_13
  *         @arg @ref LL_DMAMUX_CHANNEL_14
  *         @arg @ref LL_DMAMUX_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsEnabledIT_SO(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t Channel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return (READ_BIT(((DMAMUX_Channel_TypeDef *)(dmamux_base_addr + (DMAMUX_CCR_SIZE * (Channel))))->CCR, DMAMUX_CxCR_SOIE));
}

/**
  * @brief  Enable the Request Generation Trigger Event Overrun Interrupt on DMAMUX channel x.
  * @rmtoll RGxCR        OIE           LL_DMAMUX_EnableIT_RGO
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_EnableIT_RGO(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  SET_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_OIE);
}

/**
  * @brief  Disable the Request Generation Trigger Event Overrun Interrupt on DMAMUX channel x.
  * @rmtoll RGxCR        OIE           LL_DMAMUX_DisableIT_RGO
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval None
  */
__STATIC_INLINE void LL_DMAMUX_DisableIT_RGO(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  CLEAR_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_OIE);
}

/**
  * @brief  Check if the Request Generation Trigger Event Overrun Interrupt on DMAMUX channel x is enabled or disabled.
  * @rmtoll RGxCR        OIE           LL_DMAMUX_IsEnabledIT_RGO
  * @param  DMAMUXx DMAMUXx Instance
  * @param  RequestGenChannel This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX_REQ_GEN_0
  *         @arg @ref LL_DMAMUX_REQ_GEN_1
  *         @arg @ref LL_DMAMUX_REQ_GEN_2
  *         @arg @ref LL_DMAMUX_REQ_GEN_3
  *         @arg @ref LL_DMAMUX_REQ_GEN_4
  *         @arg @ref LL_DMAMUX_REQ_GEN_5
  *         @arg @ref LL_DMAMUX_REQ_GEN_6
  *         @arg @ref LL_DMAMUX_REQ_GEN_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMAMUX_IsEnabledIT_RGO(DMAMUX_Channel_TypeDef *DMAMUXx, uint32_t RequestGenChannel)
{
  register uint32_t dmamux_base_addr = (uint32_t)DMAMUXx;

  return ((READ_BIT(((DMAMUX_RequestGen_TypeDef *)(dmamux_base_addr + DMAMUX_REQ_GEN_OFFSET + (DMAMUX_RGCR_SIZE * RequestGenChannel)))->RGCR, DMAMUX_RGxCR_OIE) == (DMAMUX_RGxCR_OIE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* DMAMUX1 || DMAMUX2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_LL_DMAMUX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
