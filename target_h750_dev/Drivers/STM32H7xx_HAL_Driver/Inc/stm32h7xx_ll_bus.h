/**
  ******************************************************************************
  * @file    stm32h7xx_ll_bus.h
  * @author  MCD Application Team
  * @version $VERSION$
  * @date    $DATE$
  * @brief   Header file of BUS LL module.

  @verbatim
                      ##### RCC Limitations #####
  ==============================================================================
    [..]
      A delay between an RCC peripheral clock enable and the effective peripheral
      enabling should be taken into account in order to manage the peripheral read/write
      from/to registers.
      (+) This delay depends on the peripheral mapping.
        (++) AHB & APB peripherals, 1 dummy read is necessary

    [..]
      Workarounds:
      (#) For AHB & APB peripherals, a dummy read to the peripheral register has been
          inserted in each LL_{BUS}_GRP{x}_EnableClock() function.

  @endverbatim
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
#ifndef STM32H7xx_LL_BUS_H
#define STM32H7xx_LL_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined(RCC)

/** @defgroup BUS_LL BUS
  * @{
  */

/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup BUS_LL_Exported_Constants BUS Exported Constants
  * @{
  */

/** @defgroup BUS_LL_EC_AHB3_GRP1_PERIPH  AHB3 GRP1 PERIPH
  * @{
  */
#define LL_AHB3_GRP1_PERIPH_MDMA           RCC_AHB3ENR_MDMAEN
#define LL_AHB3_GRP1_PERIPH_DMA2D          RCC_AHB3ENR_DMA2DEN

#if defined(JPEG)
#define LL_AHB3_GRP1_PERIPH_JPGDEC         RCC_AHB3ENR_JPGDECEN
#endif /* JPEG */

#define LL_AHB3_GRP1_PERIPH_FMC            RCC_AHB3ENR_FMCEN
#if defined(QUADSPI)
#define LL_AHB3_GRP1_PERIPH_QSPI           RCC_AHB3ENR_QSPIEN
#endif /* QUADSPI */
#if defined(OCTOSPI1) || defined(OCTOSPI2)
#define LL_AHB3_GRP1_PERIPH_OSPI1          RCC_AHB3ENR_OSPI1EN
#define LL_AHB3_GRP1_PERIPH_OSPI2          RCC_AHB3ENR_OSPI2EN
#endif /*(OCTOSPI1) || (OCTOSPI2)*/
#if defined(OCTOSPIM)
#define LL_AHB3_GRP1_PERIPH_OCTOSPIM       RCC_AHB3ENR_IOMNGREN
#endif /* OCTOSPIM */
#if defined(OTFDEC1) || defined(OTFDEC2)
#define LL_AHB3_GRP1_PERIPH_OTFDEC1        RCC_AHB3ENR_OTFDEC1EN
#define LL_AHB3_GRP1_PERIPH_OTFDEC2        RCC_AHB3ENR_OTFDEC2EN
#endif /* (OTFDEC1) || (OTFDEC2) */
#if defined(GFXMMU)
#define LL_AHB3_GRP1_PERIPH_GFXMMU         RCC_AHB3ENR_GFXMMUEN
#endif /* GFXMMU */
#define LL_AHB3_GRP1_PERIPH_SDMMC1         RCC_AHB3ENR_SDMMC1EN
#define LL_AHB3_GRP1_PERIPH_FLASH          RCC_AHB3LPENR_FLASHLPEN
#define LL_AHB3_GRP1_PERIPH_DTCM1          RCC_AHB3LPENR_DTCM1LPEN
#define LL_AHB3_GRP1_PERIPH_DTCM2          RCC_AHB3LPENR_DTCM2LPEN
#define LL_AHB3_GRP1_PERIPH_ITCM           RCC_AHB3LPENR_ITCMLPEN
#if defined(RCC_AHB3LPENR_AXISRAMLPEN)
#define LL_AHB3_GRP1_PERIPH_AXISRAM        RCC_AHB3LPENR_AXISRAMLPEN
#else
#define LL_AHB3_GRP1_PERIPH_AXISRAM1       RCC_AHB3LPENR_AXISRAM1LPEN
#define LL_AHB3_GRP1_PERIPH_AXISRAM        LL_AHB3_GRP1_PERIPH_AXISRAM1   /* for backward compatibility*/
#endif /* RCC_AHB3LPENR_AXISRAMLPEN */
#if defined(CD_AXISRAM2_BASE)
#define LL_AHB3_GRP1_PERIPH_AXISRAM2       RCC_AHB3LPENR_AXISRAM2LPEN
#endif /* CD_AXISRAM2_BASE */
#if defined(CD_AXISRAM3_BASE)
#define LL_AHB3_GRP1_PERIPH_AXISRAM3       RCC_AHB3LPENR_AXISRAM3LPEN
#endif /* CD_AXISRAM3_BASE */
/**
  * @}
  */


/** @defgroup BUS_LL_EC_AHB1_GRP1_PERIPH  AHB1 GRP1 PERIPH
  * @{
  */
#define LL_AHB1_GRP1_PERIPH_DMA1           RCC_AHB1ENR_DMA1EN
#define LL_AHB1_GRP1_PERIPH_DMA2           RCC_AHB1ENR_DMA2EN
#define LL_AHB1_GRP1_PERIPH_ADC12          RCC_AHB1ENR_ADC12EN
#if defined(DUAL_CORE)
#define LL_AHB1_GRP1_PERIPH_ART            RCC_AHB1ENR_ARTEN
#endif /* DUAL_CORE */
#if defined(RCC_AHB1ENR_CRCEN)
#define LL_AHB1_GRP1_PERIPH_CRC            RCC_AHB1ENR_CRCEN
#endif /* RCC_AHB1ENR_CRCEN */
#if defined(ETH)
#define LL_AHB1_GRP1_PERIPH_ETH1MAC        RCC_AHB1ENR_ETH1MACEN
#define LL_AHB1_GRP1_PERIPH_ETH1TX         RCC_AHB1ENR_ETH1TXEN
#define LL_AHB1_GRP1_PERIPH_ETH1RX         RCC_AHB1ENR_ETH1RXEN
#endif /* ETH */
#define LL_AHB1_GRP1_PERIPH_USB1OTGHS      RCC_AHB1ENR_USB1OTGHSEN
#define LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI  RCC_AHB1ENR_USB1OTGHSULPIEN
#if defined(USB2_OTG_FS)
#define LL_AHB1_GRP1_PERIPH_USB2OTGHS      RCC_AHB1ENR_USB2OTGHSEN
#define LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI  RCC_AHB1ENR_USB2OTGHSULPIEN
#endif /* USB2_OTG_FS */
/**
  * @}
  */


/** @defgroup BUS_LL_EC_AHB2_GRP1_PERIPH  AHB2 GRP1 PERIPH
  * @{
  */
#define LL_AHB2_GRP1_PERIPH_DCMI           RCC_AHB2ENR_DCMIEN
#if defined(HSEM) && defined(RCC_AHB2ENR_HSEMEN)
#define LL_AHB2_GRP1_PERIPH_HSEM           RCC_AHB2ENR_HSEMEN
#endif /* HSEM && RCC_AHB2ENR_HSEMEN */
#if defined(CRYP)
#define LL_AHB2_GRP1_PERIPH_CRYP           RCC_AHB2ENR_CRYPEN
#endif /* CRYP */
#if defined(HASH)
#define LL_AHB2_GRP1_PERIPH_HASH           RCC_AHB2ENR_HASHEN
#endif /* HASH */
#define LL_AHB2_GRP1_PERIPH_RNG            RCC_AHB2ENR_RNGEN
#define LL_AHB2_GRP1_PERIPH_SDMMC2         RCC_AHB2ENR_SDMMC2EN
#if defined(BDMA1)
#define LL_AHB2_GRP1_PERIPH_BDMA1          RCC_AHB2ENR_BDMA1EN
#endif /* BDMA1 */
#if defined(RCC_AHB2ENR_D2SRAM1EN)
#define LL_AHB2_GRP1_PERIPH_D2SRAM1        RCC_AHB2ENR_D2SRAM1EN
#else
#define LL_AHB2_GRP1_PERIPH_AHBSRAM1       RCC_AHB2ENR_AHBSRAM1EN
#define LL_AHB2_GRP1_PERIPH_D2SRAM1        LL_AHB2_GRP1_PERIPH_AHBSRAM1    /* for backward compatibility*/
#endif /* RCC_AHB2ENR_D2SRAM1EN */
#if defined(RCC_AHB2ENR_D2SRAM2EN)
#define LL_AHB2_GRP1_PERIPH_D2SRAM2        RCC_AHB2ENR_D2SRAM2EN
#else
#define LL_AHB2_GRP1_PERIPH_AHBSRAM2       RCC_AHB2ENR_AHBSRAM2EN
#define LL_AHB2_GRP1_PERIPH_D2SRAM2        LL_AHB2_GRP1_PERIPH_AHBSRAM2    /* for backward compatibility*/
#endif /* RCC_AHB2ENR_D2SRAM2EN */
#if defined(RCC_AHB2ENR_D2SRAM3EN)
#define LL_AHB2_GRP1_PERIPH_D2SRAM3        RCC_AHB2ENR_D2SRAM3EN
#endif /* RCC_AHB2ENR_D2SRAM3EN */
/**
  * @}
  */


/** @defgroup BUS_LL_EC_AHB4_GRP1_PERIPH  AHB4 GRP1 PERIPH
  * @{
  */
#define LL_AHB4_GRP1_PERIPH_GPIOA          RCC_AHB4ENR_GPIOAEN
#define LL_AHB4_GRP1_PERIPH_GPIOB          RCC_AHB4ENR_GPIOBEN
#define LL_AHB4_GRP1_PERIPH_GPIOC          RCC_AHB4ENR_GPIOCEN
#define LL_AHB4_GRP1_PERIPH_GPIOD          RCC_AHB4ENR_GPIODEN
#define LL_AHB4_GRP1_PERIPH_GPIOE          RCC_AHB4ENR_GPIOEEN
#define LL_AHB4_GRP1_PERIPH_GPIOF          RCC_AHB4ENR_GPIOFEN
#define LL_AHB4_GRP1_PERIPH_GPIOG          RCC_AHB4ENR_GPIOGEN
#define LL_AHB4_GRP1_PERIPH_GPIOH          RCC_AHB4ENR_GPIOHEN
#define LL_AHB4_GRP1_PERIPH_GPIOI          RCC_AHB4ENR_GPIOIEN
#define LL_AHB4_GRP1_PERIPH_GPIOJ          RCC_AHB4ENR_GPIOJEN
#define LL_AHB4_GRP1_PERIPH_GPIOK          RCC_AHB4ENR_GPIOKEN
#if defined(RCC_AHB4ENR_CRCEN)
#define LL_AHB4_GRP1_PERIPH_CRC            RCC_AHB4ENR_CRCEN
#endif /* RCC_AHB4ENR_CRCEN */
#if defined(BDMA2)
#define LL_AHB4_GRP1_PERIPH_BDMA2          RCC_AHB4ENR_BDMA2EN
#define LL_AHB4_GRP1_PERIPH_BDMA           LL_AHB4_GRP1_PERIPH_BDMA2  /* for backward compatibility*/
#else
#define LL_AHB4_GRP1_PERIPH_BDMA           RCC_AHB4ENR_BDMAEN
#endif /* BDMA2 */
#if defined(ADC3)
#define LL_AHB4_GRP1_PERIPH_ADC3           RCC_AHB4ENR_ADC3EN
#endif /* ADC3 */
#if defined(HSEM) && defined(RCC_AHB4ENR_HSEMEN)
#define LL_AHB4_GRP1_PERIPH_HSEM           RCC_AHB4ENR_HSEMEN
#endif /* HSEM && RCC_AHB4ENR_HSEMEN*/
#define LL_AHB4_GRP1_PERIPH_BKPRAM         RCC_AHB4ENR_BKPRAMEN
#if defined(RCC_AHB4LPENR_SRAM4LPEN)
#define LL_AHB4_GRP1_PERIPH_SRAM4          RCC_AHB4LPENR_SRAM4LPEN
#define LL_AHB4_GRP1_PERIPH_D3SRAM1        LL_AHB4_GRP1_PERIPH_SRAM4
#else
#define LL_AHB4_GRP1_PERIPH_SRDSRAM        RCC_AHB4ENR_SRDSRAMEN
#define LL_AHB4_GRP1_PERIPH_SRAM4          LL_AHB4_GRP1_PERIPH_SRDSRAM  /* for backward compatibility*/
#define LL_AHB4_GRP1_PERIPH_D3SRAM1        LL_AHB4_GRP1_PERIPH_SRDSRAM  /* for backward compatibility*/
#endif /* RCC_AHB4ENR_D3SRAM1EN */
/**
  * @}
  */


/** @defgroup BUS_LL_EC_APB3_GRP1_PERIPH  APB3 GRP1 PERIPH
  * @{
  */
#define LL_APB3_GRP1_PERIPH_LTDC           RCC_APB3ENR_LTDCEN
#if defined(DSI)
#define LL_APB3_GRP1_PERIPH_DSI            RCC_APB3ENR_DSIEN
#endif /* DSI */
#define LL_APB3_GRP1_PERIPH_WWDG1          RCC_APB3ENR_WWDG1EN
#if defined(RCC_APB3ENR_WWDGEN)
#define LL_APB3_GRP1_PERIPH_WWDG           LL_APB3_GRP1_PERIPH_WWDG1   /* for backward compatibility*/
#endif
/**
  * @}
  */


/** @defgroup BUS_LL_EC_APB1_GRP1_PERIPH  APB1 GRP1 PERIPH
  * @{
  */
#define LL_APB1_GRP1_PERIPH_TIM2           RCC_APB1LENR_TIM2EN
#define LL_APB1_GRP1_PERIPH_TIM3           RCC_APB1LENR_TIM3EN
#define LL_APB1_GRP1_PERIPH_TIM4           RCC_APB1LENR_TIM4EN
#define LL_APB1_GRP1_PERIPH_TIM5           RCC_APB1LENR_TIM5EN
#define LL_APB1_GRP1_PERIPH_TIM6           RCC_APB1LENR_TIM6EN
#define LL_APB1_GRP1_PERIPH_TIM7           RCC_APB1LENR_TIM7EN
#define LL_APB1_GRP1_PERIPH_TIM12          RCC_APB1LENR_TIM12EN
#define LL_APB1_GRP1_PERIPH_TIM13          RCC_APB1LENR_TIM13EN
#define LL_APB1_GRP1_PERIPH_TIM14          RCC_APB1LENR_TIM14EN
#define LL_APB1_GRP1_PERIPH_LPTIM1         RCC_APB1LENR_LPTIM1EN
#if defined(DUAL_CORE)
#define LL_APB1_GRP1_PERIPH_WWDG2          RCC_APB1LENR_WWDG2EN
#endif /*DUAL_CORE*/
#define LL_APB1_GRP1_PERIPH_SPI2           RCC_APB1LENR_SPI2EN
#define LL_APB1_GRP1_PERIPH_SPI3           RCC_APB1LENR_SPI3EN
#define LL_APB1_GRP1_PERIPH_SPDIFRX        RCC_APB1LENR_SPDIFRXEN
#define LL_APB1_GRP1_PERIPH_USART2         RCC_APB1LENR_USART2EN
#define LL_APB1_GRP1_PERIPH_USART3         RCC_APB1LENR_USART3EN
#define LL_APB1_GRP1_PERIPH_UART4          RCC_APB1LENR_UART4EN
#define LL_APB1_GRP1_PERIPH_UART5          RCC_APB1LENR_UART5EN
#define LL_APB1_GRP1_PERIPH_I2C1           RCC_APB1LENR_I2C1EN
#define LL_APB1_GRP1_PERIPH_I2C2           RCC_APB1LENR_I2C2EN
#define LL_APB1_GRP1_PERIPH_I2C3           RCC_APB1LENR_I2C3EN
#define LL_APB1_GRP1_PERIPH_CEC            RCC_APB1LENR_CECEN
#define LL_APB1_GRP1_PERIPH_DAC12          RCC_APB1LENR_DAC12EN
#define LL_APB1_GRP1_PERIPH_UART7          RCC_APB1LENR_UART7EN
#define LL_APB1_GRP1_PERIPH_UART8          RCC_APB1LENR_UART8EN
/**
  * @}
  */


/** @defgroup BUS_LL_EC_APB1_GRP2_PERIPH  APB1 GRP2 PERIPH
  * @{
  */
#define LL_APB1_GRP2_PERIPH_CRS            RCC_APB1HENR_CRSEN
#define LL_APB1_GRP2_PERIPH_SWPMI1         RCC_APB1HENR_SWPMIEN
#define LL_APB1_GRP2_PERIPH_OPAMP          RCC_APB1HENR_OPAMPEN
#define LL_APB1_GRP2_PERIPH_MDIOS          RCC_APB1HENR_MDIOSEN
#define LL_APB1_GRP2_PERIPH_FDCAN          RCC_APB1HENR_FDCANEN
/**
  * @}
  */


/** @defgroup BUS_LL_EC_APB2_GRP1_PERIPH  APB2 GRP1 PERIPH
  * @{
  */
#define LL_APB2_GRP1_PERIPH_TIM1           RCC_APB2ENR_TIM1EN
#define LL_APB2_GRP1_PERIPH_TIM8           RCC_APB2ENR_TIM8EN
#define LL_APB2_GRP1_PERIPH_USART1         RCC_APB2ENR_USART1EN
#define LL_APB2_GRP1_PERIPH_USART6         RCC_APB2ENR_USART6EN
#if defined(UART9)
#define LL_APB2_GRP1_PERIPH_UART9          RCC_APB2ENR_UART9EN
#endif /* UART9 */
#if defined(USART10)
#define LL_APB2_GRP1_PERIPH_USART10        RCC_APB2ENR_USART10EN
#endif /* USART10 */
#define LL_APB2_GRP1_PERIPH_SPI1           RCC_APB2ENR_SPI1EN
#define LL_APB2_GRP1_PERIPH_SPI4           RCC_APB2ENR_SPI4EN
#define LL_APB2_GRP1_PERIPH_TIM15          RCC_APB2ENR_TIM15EN
#define LL_APB2_GRP1_PERIPH_TIM16          RCC_APB2ENR_TIM16EN
#define LL_APB2_GRP1_PERIPH_TIM17          RCC_APB2ENR_TIM17EN
#define LL_APB2_GRP1_PERIPH_SPI5           RCC_APB2ENR_SPI5EN
#define LL_APB2_GRP1_PERIPH_SAI1           RCC_APB2ENR_SAI1EN
#define LL_APB2_GRP1_PERIPH_SAI2           RCC_APB2ENR_SAI2EN
#if defined(SAI3)
#define LL_APB2_GRP1_PERIPH_SAI3           RCC_APB2ENR_SAI3EN
#endif /* SAI3 */
#define LL_APB2_GRP1_PERIPH_DFSDM1         RCC_APB2ENR_DFSDM1EN
#if defined(HRTIM1)
#define LL_APB2_GRP1_PERIPH_HRTIM          RCC_APB2ENR_HRTIMEN
#endif /* HRTIM1 */
/**
  * @}
  */


/** @defgroup BUS_LL_EC_APB4_GRP1_PERIPH  APB4 GRP1 PERIPH
  * @{
  */
#define LL_APB4_GRP1_PERIPH_SYSCFG         RCC_APB4ENR_SYSCFGEN
#define LL_APB4_GRP1_PERIPH_LPUART1        RCC_APB4ENR_LPUART1EN
#define LL_APB4_GRP1_PERIPH_SPI6           RCC_APB4ENR_SPI6EN
#define LL_APB4_GRP1_PERIPH_I2C4           RCC_APB4ENR_I2C4EN
#define LL_APB4_GRP1_PERIPH_LPTIM2         RCC_APB4ENR_LPTIM2EN
#define LL_APB4_GRP1_PERIPH_LPTIM3         RCC_APB4ENR_LPTIM3EN
#if defined(LPTIM4)
#define LL_APB4_GRP1_PERIPH_LPTIM4         RCC_APB4ENR_LPTIM4EN
#endif /* LPTIM4 */
#if defined(LPTIM5)
#define LL_APB4_GRP1_PERIPH_LPTIM5         RCC_APB4ENR_LPTIM5EN
#endif /* LPTIM5 */
#if defined(DAC2)
#define LL_APB4_GRP1_PERIPH_DAC2           RCC_APB4ENR_DAC2EN
#endif /* DAC2 */
#define LL_APB4_GRP1_PERIPH_COMP12         RCC_APB4ENR_COMP12EN
#define LL_APB4_GRP1_PERIPH_VREF           RCC_APB4ENR_VREFEN
#define LL_APB4_GRP1_PERIPH_RTCAPB         RCC_APB4ENR_RTCAPBEN
#if defined(SAI4)
#define LL_APB4_GRP1_PERIPH_SAI4           RCC_APB4ENR_SAI4EN
#endif /* SAI4 */
#if defined(DTS)
#define LL_APB4_GRP1_PERIPH_DTS            RCC_APB4ENR_DTSEN
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define LL_APB4_GRP1_PERIPH_DFSDM2         RCC_APB4ENR_DFSDM2EN
#endif /* DFSDM2_BASE */
/**
  * @}
  */

/** @defgroup BUS_LL_EC_CLKAM_PERIPH  CLKAM PERIPH
  * @{
  */
#if defined(RCC_D3AMR_BDMAAMEN)
#define LL_CLKAM_PERIPH_BDMA          RCC_D3AMR_BDMAAMEN
#else
#define LL_CLKAM_PERIPH_BDMA2         RCC_SRDAMR_BDMA2AMEN
#define LL_CLKAM_PERIPH_BDMA          LL_CLKAM_PERIPH_BDMA2 /* for backward compatibility*/
#endif /* RCC_D3AMR_BDMAAMEN */
#if defined(RCC_SRDAMR_GPIOAMEN)
#define LL_CLKAM_PERIPH_GPIO          RCC_SRDAMR_GPIOAMEN
#endif /* RCC_SRDAMR_GPIOAMEN */
#if defined(RCC_D3AMR_LPUART1AMEN)
#define LL_CLKAM_PERIPH_LPUART1       RCC_D3AMR_LPUART1AMEN
#else
#define LL_CLKAM_PERIPH_LPUART1       RCC_SRDAMR_LPUART1AMEN
#endif /* RCC_D3AMR_LPUART1AMEN */
#if defined(RCC_D3AMR_SPI6AMEN)
#define LL_CLKAM_PERIPH_SPI6          RCC_D3AMR_SPI6AMEN
#else
#define LL_CLKAM_PERIPH_SPI6          RCC_SRDAMR_SPI6AMEN
#endif /* RCC_D3AMR_SPI6AMEN */
#if defined(RCC_D3AMR_I2C4AMEN)
#define LL_CLKAM_PERIPH_I2C4          RCC_D3AMR_I2C4AMEN
#else
#define LL_CLKAM_PERIPH_I2C4          RCC_SRDAMR_I2C4AMEN
#endif /* RCC_D3AMR_I2C4AMEN */
#if defined(RCC_D3AMR_LPTIM2AMEN)
#define LL_CLKAM_PERIPH_LPTIM2        RCC_D3AMR_LPTIM2AMEN
#else
#define LL_CLKAM_PERIPH_LPTIM2        RCC_SRDAMR_LPTIM2AMEN
#endif /* RCC_D3AMR_LPTIM2AMEN */
#if defined(RCC_D3AMR_LPTIM3AMEN)
#define LL_CLKAM_PERIPH_LPTIM3        RCC_D3AMR_LPTIM3AMEN
#else
#define LL_CLKAM_PERIPH_LPTIM3        RCC_SRDAMR_LPTIM3AMEN
#endif /* RCC_D3AMR_LPTIM3AMEN */
#if defined(RCC_D3AMR_LPTIM4AMEN)
#define LL_CLKAM_PERIPH_LPTIM4        RCC_D3AMR_LPTIM4AMEN
#endif /* RCC_D3AMR_LPTIM4AMEN */
#if defined(RCC_D3AMR_LPTIM5AMEN)
#define LL_CLKAM_PERIPH_LPTIM5        RCC_D3AMR_LPTIM5AMEN
#endif /* RCC_D3AMR_LPTIM5AMEN */
#if defined(DAC2)
#define LL_CLKAM_PERIPH_DAC2          RCC_SRDAMR_DAC2AMEN
#endif /* DAC2 */
#if defined(RCC_D3AMR_COMP12AMEN)
#define LL_CLKAM_PERIPH_COMP12        RCC_D3AMR_COMP12AMEN
#else
#define LL_CLKAM_PERIPH_COMP12        RCC_SRDAMR_COMP12AMEN
#endif /* RCC_D3AMR_COMP12AMEN */
#if defined(RCC_D3AMR_VREFAMEN)
#define LL_CLKAM_PERIPH_VREF          RCC_D3AMR_VREFAMEN
#else
#define LL_CLKAM_PERIPH_VREF          RCC_SRDAMR_VREFAMEN
#endif /* RCC_D3AMR_VREFAMEN */
#if defined(RCC_D3AMR_RTCAMEN)
#define LL_CLKAM_PERIPH_RTC           RCC_D3AMR_RTCAMEN
#else
#define LL_CLKAM_PERIPH_RTC           RCC_SRDAMR_RTCAMEN
#endif /* RCC_D3AMR_RTCAMEN */
#if defined(RCC_D3AMR_CRCAMEN)
#define LL_CLKAM_PERIPH_CRC           RCC_D3AMR_CRCAMEN
#endif /* RCC_D3AMR_CRCAMEN */
#if defined(SAI4)
#define LL_CLKAM_PERIPH_SAI4          RCC_D3AMR_SAI4AMEN
#endif /* SAI4 */
#if defined(ADC3)
#define LL_CLKAM_PERIPH_ADC3          RCC_D3AMR_ADC3AMEN
#endif /* ADC3 */
#if defined(DTS)
#define LL_CLKAM_PERIPH_DTS           RCC_SRDAMR_DTSAMEN
#endif /* DTS */
#if defined(DFSDM2_BASE)
#define LL_CLKAM_PERIPH_DFSDM2        RCC_SRDAMR_DFSDM2AMEN
#endif /* DFSDM2_BASE */
#if defined(RCC_D3AMR_BKPRAMAMEN)
#define LL_CLKAM_PERIPH_BKPRAM        RCC_D3AMR_BKPRAMAMEN
#else
#define LL_CLKAM_PERIPH_BKPRAM        RCC_SRDAMR_BKPRAMAMEN
#endif /* RCC_D3AMR_BKPRAMAMEN */
#if defined(RCC_D3AMR_SRAM4AMEN)
#define LL_CLKAM_PERIPH_SRAM4         RCC_D3AMR_SRAM4AMEN
#else
#define LL_CLKAM_PERIPH_SRDSRAM       RCC_SRDAMR_SRDSRAMAMEN
#define LL_CLKAM_PERIPH_SRAM4         LL_CLKAM_PERIPH_SRDSRAM
#endif /* RCC_D3AMR_SRAM4AMEN */
/**
  * @}
  */

#if defined(RCC_CKGAENR_AXICKG)
/** @defgroup BUS_LL_EC_CKGA_PERIPH  CKGA (AXI Clocks Gating) PERIPH
  * @{
  */
#define LL_CKGA_PERIPH_AXI            RCC_CKGAENR_AXICKG
#define LL_CKGA_PERIPH_AHB            RCC_CKGAENR_AHBCKG
#define LL_CKGA_PERIPH_CPU            RCC_CKGAENR_CPUCKG
#define LL_CKGA_PERIPH_SDMMC          RCC_CKGAENR_SDMMCCKG
#define LL_CKGA_PERIPH_MDMA           RCC_CKGAENR_MDMACKG
#define LL_CKGA_PERIPH_DMA2D          RCC_CKGAENR_DMA2DCKG
#define LL_CKGA_PERIPH_LTDC           RCC_CKGAENR_LTDCCKG
#define LL_CKGA_PERIPH_GFXMMUM        RCC_CKGAENR_GFXMMUMCKG
#define LL_CKGA_PERIPH_AHB12          RCC_CKGAENR_AHB12CKG
#define LL_CKGA_PERIPH_AHB34          RCC_CKGAENR_AHB34CKG
#define LL_CKGA_PERIPH_FLIFT          RCC_CKGAENR_FLIFTCKG
#define LL_CKGA_PERIPH_OCTOSPI2       RCC_CKGAENR_OCTOSPI2CKG
#define LL_CKGA_PERIPH_FMC            RCC_CKGAENR_FMCCKG
#define LL_CKGA_PERIPH_OCTOSPI1       RCC_CKGAENR_OCTOSPI1CKG
#define LL_CKGA_PERIPH_AXIRAM1        RCC_CKGAENR_AXIRAM1CKG
#define LL_CKGA_PERIPH_AXIRAM2        RCC_CKGAENR_AXIRAM2CKG
#define LL_CKGA_PERIPH_AXIRAM3        RCC_CKGAENR_AXIRAM3CKG
#define LL_CKGA_PERIPH_GFXMMUS        RCC_CKGAENR_GFXMMUSCKG
#define LL_CKGA_PERIPH_ECCRAM         RCC_CKGAENR_ECCRAMCKG
#define LL_CKGA_PERIPH_EXTI           RCC_CKGAENR_EXTICKG
#define LL_CKGA_PERIPH_JTAG           RCC_CKGAENR_JTAGCKG
/**
  * @}
  */
#endif /* RCC_CKGAENR_AXICKG */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @defgroup BUS_LL_Exported_Functions BUS Exported Functions
  * @{
  */

/** @defgroup BUS_LL_EF_AHB3 AHB3
  * @{
  */

/**
  * @brief  Enable AHB3 peripherals clock.
  * @rmtoll AHB3ENR      MDMAEN        LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      DMA2DEN       LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      JPGDECEN      LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      FMCEN         LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      QSPIEN        LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      OSPI1EN       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      OSPI2EN       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      IOMNGREN      LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      OTFDEC1EN     LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      OTFDEC2EN     LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      GFXMMU        LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      SDMMC1EN      LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      FLASHEN       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      DTCM1EN       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      DTCM2EN       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      ITCMEN        LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENR      AXISRAMEN     LL_AHB3_GRP1_EnableClock    (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB3 peripheral clock is enabled or not
  * @rmtoll AHB3ENR      MDMAEN        LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      DMA2DEN       LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      JPGDECEN      LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      FMCEN         LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      QSPIEN        LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OSPI1EN       LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OSPI2EN       LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      IOMNGREN      LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OTFDEC1EN     LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OTFDEC2EN     LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      GFXMMU        LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      SDMMC1EN      LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      FLASHEN       LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      DTCM1EN       LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      DTCM2EN       LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      ITCMEN        LL_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      AXISRAMEN     LL_AHB3_GRP1_IsEnabledClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IOMNGR  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_AHB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB3ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable AHB3 peripherals clock.
  * @rmtoll AHB3ENR      MDMAEN        LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      DMA2DEN       LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      JPGDECEN      LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      FMCEN         LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      QSPIEN        LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OSPI1EN       LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OSPI2EN       LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      IOMNGREN      LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OTFDEC1EN     LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OTFDEC2EN     LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      GFXMMU        LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      SDMMC1EN      LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      FLASHEN       LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      DTCM1EN       LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      DTCM2EN       LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      ITCMEN        LL_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      AXISRAMEN     LL_AHB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IOMNGR  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB3ENR, Periphs);
}

/**
  * @brief  Force AHB3 peripherals reset.
  * @rmtoll AHB3RSTR     MDMARST       LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTR     DMA2DRST      LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTR     JPGDECRST     LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTR     FMCRST        LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTR     QSPIRST       LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     OSPI1RST      LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     OSPI2RST      LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     IOMNGRRST     LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     OTFDEC1RST    LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     OTFDEC2RST    LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     GFXMMURST     LL_AHB3_GRP1_ForceReset\n (*)
  *         AHB3RSTR     SDMMC1RST     LL_AHB3_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI     (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB3_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHB3RSTR, Periphs);
}

/**
  * @brief  Release AHB3 peripherals reset.
  * @rmtoll AHB3RSTR     MDMARST       LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTR     DMA2DRST      LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTR     JPGDECRST     LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTR     FMCRST        LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTR     QSPIRST       LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTR     OSPI1RST      LL_AHB3_GRP1_ReleaseReset\n (*)
  *         AHB3RSTR     OSPI2RST      LL_AHB3_GRP1_ReleaseReset\n (*)
  *         AHB3RSTR     IOMNGRRST     LL_AHB3_GRP1_ReleaseReset\n (*)
  *         AHB3RSTR     OTFDEC1RST    LL_AHB3_GRP1_ReleaseReset\n (*)
  *         AHB3RSTR     OTFDEC2RST    LL_AHB3_GRP1_ReleaseReset\n (*)
  *         AHB3RSTR     GFXMMURST     LL_AHB3_GRP1_ReleaseReset\n (*)
  *         AHB3RSTR     SDMMC1RST     LL_AHB3_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI     (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB3_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB3RSTR, Periphs);
}

/**
  * @brief  Enable AHB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB3LPENR    MDMALPEN      LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DMA2DLPEN     LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    JPGDECLPEN    LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    FMCLPEN       LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    QSPILPEN      LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OSPI1LPEN     LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OSPI2LPEN     LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    IOMNGRLPEN    LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    GFXMMULPEN    LL_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    SDMMC1LPEN    LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    FLASHLPEN     LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DTCM1LPEN     LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DTCM2LPEN     LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    ITCMLPEN      LL_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    AXISRAMLPEN   LL_AHB3_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI     (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB3_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB3LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB3LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable AHB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB3LPENR    MDMALPEN      LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DMA2DLPEN     LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    JPGDECLPEN    LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    FMCLPEN       LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    QSPILPEN      LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    OSPI1LPEN     LL_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OSPI2LPEN     LL_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    IOMNGRLPEN    LL_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    GFXMMULPEN    LL_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    SDMMC1LPEN    LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    FLASHLPEN     LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DTCM1LPEN     LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DTCM2LPEN     LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    ITCMLPEN      LL_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    AXISRAMLPEN   LL_AHB3_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI     (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB3_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB3LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB1 AHB1
  * @{
  */

/**
  * @brief  Enable AHB1 peripherals clock.
  * @rmtoll AHB1ENR      DMA1EN        LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      DMA2EN        LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ADC12EN       LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ARTEN         LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ETH1MACEN     LL_AHB1_GRP1_EnableClock\n (*)
  *         AHB1ENR      ETH1TXEN      LL_AHB1_GRP1_EnableClock\n (*)
  *         AHB1ENR      ETH1RXEN      LL_AHB1_GRP1_EnableClock\n (*)
  *         AHB1ENR      USB1OTGHSEN   LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_AHB1_GRP1_EnableClock\n  (*)
  *         AHB1ENR      USB2OTGHSULPIEN  LL_AHB1_GRP1_EnableClock (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB1ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB1ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB1 peripheral clock is enabled or not
  * @rmtoll AHB1ENR      DMA1EN        LL_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      DMA2EN        LL_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ADC12EN       LL_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ARTEN         LL_AHB1_GRP1_IsEnabledClock\n (*)
  *         AHB1ENR      CRCEN         LL_AHB1_GRP1_IsEnabledClock\n (*)
  *         AHB1ENR      ETH1MACEN     LL_AHB1_GRP1_IsEnabledClock\n (*)
  *         AHB1ENR      ETH1TXEN      LL_AHB1_GRP1_IsEnabledClock\n (*)
  *         AHB1ENR      ETH1RXEN      LL_AHB1_GRP1_IsEnabledClock\n (*)
  *         AHB1ENR      USB1OTGHSEN   LL_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      USB2OTGHSULPIEN  LL_AHB1_GRP1_IsEnabledClock (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB1ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable AHB1 peripherals clock.
  * @rmtoll AHB1ENR      DMA1EN        LL_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      DMA2EN        LL_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ADC12EN       LL_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ARTEN         LL_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ETH1MACEN     LL_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ETH1TXEN      LL_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ETH1RXEN      LL_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      USB1OTGHSEN   LL_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_AHB1_GRP1_DisableClock\n  (*)
  *         AHB1ENR      USB2OTGHSULPIEN  LL_AHB1_GRP1_DisableClock (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB1ENR, Periphs);
}

/**
  * @brief  Force AHB1 peripherals reset.
  * @rmtoll AHB1RSTR     DMA1RST       LL_AHB1_GRP1_ForceReset\n
  *         AHB1RSTR     DMA2RST       LL_AHB1_GRP1_ForceReset\n
  *         AHB1RSTR     ADC12RST      LL_AHB1_GRP1_ForceReset\n
  *         AHB1RSTR     ARTRST        LL_AHB1_GRP1_ForceReset\n (*)
  *         AHB1RSTR     CRCRST        LL_AHB1_GRP1_ForceReset\n (*)
  *         AHB1RSTR     ETH1MACRST    LL_AHB1_GRP1_ForceReset\n (*)
  *         AHB1RSTR     USB1OTGHSRST  LL_AHB1_GRP1_ForceReset\n
  *         AHB1RSTR     USB2OTGHSRST  LL_AHB1_GRP1_ForceReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB1_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHB1RSTR, Periphs);
}

/**
  * @brief  Release AHB1 peripherals reset.
  * @rmtoll AHB1RSTR     DMA1RST       LL_AHB1_GRP1_ReleaseReset\n
  *         AHB1RSTR     DMA2RST       LL_AHB1_GRP1_ReleaseReset\n
  *         AHB1RSTR     ADC12RST      LL_AHB1_GRP1_ReleaseReset\n
  *         AHB1RSTR     ARTRST        LL_AHB1_GRP1_ReleaseReset\n (*)
  *         AHB1RSTR     CRCRST        LL_AHB1_GRP1_ReleaseReset\n (*)
  *         AHB1RSTR     ETH1MACRST    LL_AHB1_GRP1_ReleaseReset\n (*)
  *         AHB1RSTR     USB1OTGHSRST  LL_AHB1_GRP1_ReleaseReset\n
  *         AHB1RSTR     USB2OTGHSRST  LL_AHB1_GRP1_ReleaseReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB1RSTR, Periphs);
}

/**
  * @brief  Enable AHB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB1LPENR    DMA1LPEN      LL_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    DMA2LPEN      LL_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ADC12LPEN     LL_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ARTLPEN       LL_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    CRCLPEN       LL_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ETH1MACLPEN   LL_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ETH1TXLPEN    LL_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ETH1RXLPEN    LL_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB1OTGHSLPEN  LL_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB1OTGHSULPILPEN  LL_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB2OTGHSLPEN  LL_AHB1_GRP1_EnableClockSleep\n   (*)
  *         AHB1LPENR    USB2OTGHSULPILPEN  LL_AHB1_GRP1_EnableClockSleep (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB1_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB1LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB1LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable AHB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB1LPENR    DMA1LPEN      LL_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    DMA2LPEN      LL_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ADC12LPEN     LL_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ARTLPEN       LL_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    CRCLPEN       LL_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ETH1MACLPEN   LL_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ETH1TXLPEN    LL_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ETH1RXLPEN    LL_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    USB1OTGHSLPEN  LL_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB1OTGHSULPILPEN  LL_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB2OTGHSLPEN  LL_AHB1_GRP1_DisableClockSleep\n   (*)
  *         AHB1LPENR    USB2OTGHSULPILPEN  LL_AHB1_GRP1_DisableClockSleep (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB1_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB1LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB2 AHB2
  * @{
  */

/**
  * @brief  Enable AHB2 peripherals clock.
  * @rmtoll AHB2ENR      DCMIEN        LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      HSEMEN        LL_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      CRYPEN        LL_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      HASHEN        LL_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      RNGEN         LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      SDMMC2EN      LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      BDMA1EN       LL_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      D2SRAM1EN     LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      D2SRAM2EN     LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      D2SRAM3EN     LL_AHB2_GRP1_EnableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1   (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB2 peripheral clock is enabled or not
  * @rmtoll AHB2ENR      DCMIEN        LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      HSEMEN        LL_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      CRYPEN        LL_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      HASHEN        LL_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      RNGEN         LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      SDMMC2EN      LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      BDMA1EN       LL_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      D2SRAM1EN     LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      D2SRAM2EN     LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      D2SRAM3EN     LL_AHB2_GRP1_IsEnabledClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEMEN (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1   (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_AHB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB2ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable AHB2 peripherals clock.
  * @rmtoll AHB2ENR      DCMIEN        LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      HSEMEN        LL_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      CRYPEN        LL_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      HASHEN        LL_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      RNGEN         LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      SDMMC2EN      LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      BDMA1EN       LL_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      D2SRAM1EN     LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      D2SRAM2EN     LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      D2SRAM3EN     LL_AHB2_GRP1_DisableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEMEN (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1   (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB2ENR, Periphs);
}

/**
  * @brief  Force AHB2 peripherals reset.
  * @rmtoll AHB2RSTR     DCMIRST       LL_AHB2_GRP1_ForceReset\n
  *         AHB2RSTR     HSEMRST       LL_AHB2_GRP1_ForceReset\n (*)
  *         AHB2RSTR     CRYPRST       LL_AHB2_GRP1_ForceReset\n (*)
  *         AHB2RSTR     HASHRST       LL_AHB2_GRP1_ForceReset\n (*)
  *         AHB2RSTR     RNGRST        LL_AHB2_GRP1_ForceReset\n
  *         AHB2RSTR     SDMMC2RST     LL_AHB2_GRP1_ForceReset\n
  *         AHB2RSTR     BDMA1RST      LL_AHB2_GRP1_ForceReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB2_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHB2RSTR, Periphs);
}

/**
  * @brief  Release AHB2 peripherals reset.
  * @rmtoll AHB2RSTR     DCMIRST       LL_AHB2_GRP1_ReleaseReset\n
  *         AHB2RSTR     HSEMRST       LL_AHB2_GRP1_ReleaseReset\n (*)
  *         AHB2RSTR     CRYPRST       LL_AHB2_GRP1_ReleaseReset\n (*)
  *         AHB2RSTR     HASHRST       LL_AHB2_GRP1_ReleaseReset\n (*)
  *         AHB2RSTR     RNGRST        LL_AHB2_GRP1_ReleaseReset\n
  *         AHB2RSTR     SDMMC2RST     LL_AHB2_GRP1_ReleaseReset\n
  *         AHB2RSTR     BDMA1RST      LL_AHB2_GRP1_ReleaseReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB2RSTR, Periphs);
}

/**
  * @brief  Enable AHB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB2LPENR    DCMILPEN      LL_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    CRYPLPEN      LL_AHB2_GRP1_EnableClockSleep\n (*)
  *         AHB2LPENR    HASHLPEN      LL_AHB2_GRP1_EnableClockSleep\n (*)
  *         AHB2LPENR    RNGLPEN       LL_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    BDMA1LPEN     LL_AHB2_GRP1_EnableClockSleep\n (*)
  *         AHB2LPENR    D2SRAM1LPEN   LL_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    D2SRAM2LPEN   LL_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    D2SRAM3LPEN   LL_AHB2_GRP1_EnableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1   (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB2_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB2LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB2LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable AHB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB2LPENR    DCMILPEN      LL_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    CRYPLPEN      LL_AHB2_GRP1_DisableClockSleep\n (*)
  *         AHB2LPENR    HASHLPEN      LL_AHB2_GRP1_DisableClockSleep\n (*)
  *         AHB2LPENR    RNGLPEN       LL_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    BDMA1LPEN     LL_AHB2_GRP1_DisableClockSleep\n (*)
  *         AHB2LPENR    D2SRAM1LPEN   LL_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM2LPEN   LL_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM3LPEN   LL_AHB2_GRP1_DisableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1 (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB2_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB2LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB4 AHB4
  * @{
  */

/**
  * @brief  Enable AHB4 peripherals clock.
  * @rmtoll AHB4ENR      GPIOAEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOBEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOCEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIODEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOEEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOFEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOGEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOHEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOIEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOJEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOKEN       LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      CRCEN         LL_AHB4_GRP1_EnableClock\n (*)
  *         AHB4ENR      BDMAEN        LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      ADC3EN        LL_AHB4_GRP1_EnableClock\n (*)
  *         AHB4ENR      HSEMEN        LL_AHB4_GRP1_EnableClock\n (*)
  *         AHB4ENR      BKPRAMEN      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      SRAM4EN       LL_AHB4_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC   (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB4ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB4ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB4 peripheral clock is enabled or not
  * @rmtoll AHB4ENR      GPIOAEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOBEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOCEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIODEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOEEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOFEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOGEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOHEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOIEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOJEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOKEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      CRCEN         LL_AHB4_GRP1_IsEnabledClock\n (*)
  *         AHB4ENR      BDMAEN        LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      ADC3EN        LL_AHB4_GRP1_IsEnabledClock\n (*)
  *         AHB4ENR      HSEMEN        LL_AHB4_GRP1_IsEnabledClock\n (*)
  *         AHB4ENR      BKPRAMEN      LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      SRAM4EN       LL_AHB4_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_AHB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB4ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable AHB4 peripherals clock.
  * @rmtoll AHB4ENR      GPIOAEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOBEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOCEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIODEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOEEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOFEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOGEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOHEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOIEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOJEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOKEN       LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      CRCEN         LL_AHB4_GRP1_DisableClock\n (*)
  *         AHB4ENR      BDMAEN        LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      ADC3EN        LL_AHB4_GRP1_DisableClock\n (*)
  *         AHB4ENR      HSEMEN        LL_AHB4_GRP1_DisableClock\n (*)
  *         AHB4ENR      BKPRAMEN      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      SRAM4EN       LL_AHB4_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB4_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB4ENR, Periphs);
}

/**
  * @brief  Force AHB4 peripherals reset.
  * @rmtoll AHB4RSTR     GPIOARST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOBRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOCRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIODRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOERST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOFRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOGRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOHRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOIRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOJRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     GPIOKRST      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     CRCRST        LL_AHB4_GRP1_ForceReset\n (*)
  *         AHB4RSTR     BDMARST       LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTR     ADC3RST       LL_AHB4_GRP1_ForceReset\n (*)
  *         AHB4RSTR     HSEMRST       LL_AHB4_GRP1_ForceReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB4_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHB4RSTR, Periphs);
}

/**
  * @brief  Release AHB4 peripherals reset.
  * @rmtoll AHB4RSTR     GPIOARST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOBRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOCRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIODRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOERST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOFRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOGRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOHRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOIRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOJRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     GPIOKRST      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     CRCRST        LL_AHB4_GRP1_ReleaseReset\n (*)
  *         AHB4RSTR     BDMARST       LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTR     ADC3RST       LL_AHB4_GRP1_ReleaseReset\n (*)
  *         AHB4RSTR     HSEMRST       LL_AHB4_GRP1_ReleaseReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_AHB4_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB4RSTR, Periphs);
}

/**
  * @brief  Enable AHB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB4LPENR    GPIOALPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOBLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOCLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIODLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOELPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOFLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOGLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOHLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOILPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOJLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOKLPEN     LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    CRCLPEN       LL_AHB4_GRP1_EnableClockSleep\n (*)
  *         AHB4LPENR    BDMALPEN      LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    ADC3LPEN      LL_AHB4_GRP1_EnableClockSleep\n (*)
  *         AHB4LPENR    BKPRAMLPEN    LL_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    SRAM4LPEN     LL_AHB4_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  * @retval None
*/
__STATIC_INLINE void LL_AHB4_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB4LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB4LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable AHB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB4LPENR    GPIOALPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOBLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOCLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIODLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOELPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOFLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOGLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOHLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOILPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOJLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOKLPEN     LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    CRCLPEN       LL_AHB4_GRP1_DisableClockSleep\n (*)
  *         AHB4LPENR    BDMALPEN      LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    ADC3LPEN      LL_AHB4_GRP1_DisableClockSleep\n (*)
  *         AHB4LPENR    BKPRAMLPEN    LL_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    SRAM4LPEN   LL_AHB4_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  * @retval None
*/
__STATIC_INLINE void LL_AHB4_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB4LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB3 APB3
  * @{
  */

/**
  * @brief  Enable APB3 peripherals clock.
  * @rmtoll APB3ENR      LTDCEN        LL_APB3_GRP1_EnableClock\n (*)
  *         APB3ENR      DSIEN         LL_APB3_GRP1_EnableClock\n (*)
  *         APB3ENR      WWDG1EN       LL_APB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB3 peripheral clock is enabled or not
  * @rmtoll APB3ENR      LTDCEN        LL_APB3_GRP1_IsEnabledClock\n (*)
  *         APB3ENR      DSIEN         LL_APB3_GRP1_IsEnabledClock\n (*)
  *         APB3ENR      WWDG1EN       LL_APB3_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_APB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB3ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable APB3 peripherals clock.
  * @rmtoll APB3ENR      LTDCEN        LL_APB3_GRP1_DisableClock\n
  *         APB3ENR      DSIEN         LL_APB3_GRP1_DisableClock\n
  *         APB3ENR      WWDG1EN       LL_APB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB3ENR, Periphs);
}

/**
  * @brief  Force APB3 peripherals reset.
  * @rmtoll APB3RSTR     LTDCRST       LL_APB3_GRP1_ForceReset\n (*)
  *         APB3RSTR     DSIRST        LL_APB3_GRP1_ForceReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB3_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB3RSTR, Periphs);
}

/**
  * @brief  Release APB3 peripherals reset.
  * @rmtoll APB3RSTR     LTDCRST       LL_APB3_GRP1_ReleaseReset\n
  *         APB3RSTR     DSIRST        LL_APB3_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB3_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB3RSTR, Periphs);
}

/**
  * @brief  Enable APB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB3LPENR    LTDCLPEN      LL_APB3_GRP1_EnableClockSleep\n (*)
  *         APB3LPENR    DSILPEN       LL_APB3_GRP1_EnableClockSleep\n (*)
  *         APB3LPENR    WWDG1LPEN     LL_APB3_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB3_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB3LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB3LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable APB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB3LPENR    LTDCLPEN      LL_APB3_GRP1_DisableClockSleep\n (*)
  *         APB3LPENR    DSILPEN       LL_APB3_GRP1_DisableClockSleep\n (*)
  *         APB3LPENR    WWDG1LPEN     LL_APB3_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB3_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB3LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB1 APB1
  * @{
  */

/**
  * @brief  Enable APB1 peripherals clock.
  * @rmtoll APB1LENR     TIM2EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM3EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM4EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM5EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM6EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM7EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM12EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM13EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM14EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     LPTIM1EN      LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     WWDG2EN       LL_APB1_GRP1_EnableClock\n (*)
  *         APB1LENR     SPI2EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPI3EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPDIFRXEN     LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     USART2EN      LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     USART3EN      LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART4EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART5EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C1EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C2EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C3EN        LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     CECEN         LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     DAC12EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART7EN       LL_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART8EN       LL_APB1_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB1LENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB1LENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 peripheral clock is enabled or not
  * @rmtoll APB1LENR     TIM2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM3EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM4EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM5EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM6EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM7EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM12EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM13EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM14EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     LPTIM1EN      LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     WWDG2EN       LL_APB1_GRP1_IsEnabledClock\n (*)
  *         APB1LENR     SPI2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPI3EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPDIFRXEN     LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     USART2EN      LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     USART3EN      LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART4EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART5EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C1EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C3EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     CECEN         LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     DAC12EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART7EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART8EN       LL_APB1_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1LENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable APB1 peripherals clock.
  * @rmtoll APB1LENR     TIM2EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM3EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM4EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM5EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM6EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM7EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM12EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM13EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM14EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     LPTIM1EN      LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     WWDG2EN       LL_APB1_GRP1_DisableClock\n (*)
  *         APB1LENR     SPI2EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPI3EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPDIFRXEN     LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     USART2EN      LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     USART3EN      LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART4EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART5EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C1EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C2EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C3EN        LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     CECEN         LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     DAC12EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART7EN       LL_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART8EN       LL_APB1_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1LENR, Periphs);
}

/**
  * @brief  Force APB1 peripherals reset.
  * @rmtoll APB1LRSTR    TIM2RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM3RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM4RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM5RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM6RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM7RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM12RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM13RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    TIM14RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    LPTIM1RST     LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    SPI2RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    SPI3RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    SPDIFRXRST    LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    USART2RST     LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    USART3RST     LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    UART4RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    UART5RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    I2C1RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    I2C2RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    I2C3RST       LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    CECRST        LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    DAC12RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    UART7RST      LL_APB1_GRP1_ForceReset\n
  *         APB1LRSTR    UART8RST      LL_APB1_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB1LRSTR, Periphs);
}

/**
  * @brief  Release APB1 peripherals reset.
  * @rmtoll APB1LRSTR    TIM2RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM3RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM4RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM5RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM6RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM7RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM12RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM13RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    TIM14RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    LPTIM1RST     LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    SPI2RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    SPI3RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    SPDIFRXRST    LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    USART2RST     LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    USART3RST     LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    UART4RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    UART5RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    I2C1RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    I2C2RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    I2C3RST       LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    CECRST        LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    DAC12RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    UART7RST      LL_APB1_GRP1_ReleaseReset\n
  *         APB1LRSTR    UART8RST      LL_APB1_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1LRSTR, Periphs);
}

/**
  * @brief  Enable APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1LLPENR   TIM2LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM3LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM4LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM5LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM6LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM7LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM12LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM13LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM14LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   LPTIM1LPEN    LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   WWDG2LPEN     LL_APB1_GRP1_EnableClockSleep\n (*)
  *         APB1LLPENR   SPI2LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPI3LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPDIFRXLPEN   LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   USART2LPEN    LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   USART3LPEN    LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART4LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART5LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C1LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C2LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C3LPEN      LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   CECLPEN       LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   DAC12LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART7LPEN     LL_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART8LPEN     LL_APB1_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB1LLPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB1LLPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1LLPENR   TIM2LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM3LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM4LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM5LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM6LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM7LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM12LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM13LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM14LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   LPTIM1LPEN    LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   WWDG2LPEN     LL_APB1_GRP1_DisableClockSleep\n (*)
  *         APB1LLPENR   SPI2LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPI3LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPDIFRXLPEN   LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   USART2LPEN    LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   USART3LPEN    LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART4LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART5LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C1LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C2LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C3LPEN      LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   CECLPEN       LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   DAC12LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART7LPEN     LL_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART8LPEN     LL_APB1_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1LLPENR, Periphs);
}

/**
  * @brief  Enable APB1 peripherals clock.
  * @rmtoll APB1HENR     CRSEN         LL_APB1_GRP2_EnableClock\n
  *         APB1HENR     SWPMIEN       LL_APB1_GRP2_EnableClock\n
  *         APB1HENR     OPAMPEN       LL_APB1_GRP2_EnableClock\n
  *         APB1HENR     MDIOSEN       LL_APB1_GRP2_EnableClock\n
  *         APB1HENR     FDCANEN       LL_APB1_GRP2_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP2_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB1HENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB1HENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 peripheral clock is enabled or not
  * @rmtoll APB1HENR     CRSEN         LL_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     SWPMIEN       LL_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     OPAMPEN       LL_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     MDIOSEN       LL_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     FDCANEN       LL_APB1_GRP2_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_APB1_GRP2_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1HENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable APB1 peripherals clock.
  * @rmtoll APB1HENR     CRSEN         LL_APB1_GRP2_DisableClock\n
  *         APB1HENR     SWPMIEN       LL_APB1_GRP2_DisableClock\n
  *         APB1HENR     OPAMPEN       LL_APB1_GRP2_DisableClock\n
  *         APB1HENR     MDIOSEN       LL_APB1_GRP2_DisableClock\n
  *         APB1HENR     FDCANEN       LL_APB1_GRP2_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP2_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1HENR, Periphs);
}

/**
  * @brief  Force APB1 peripherals reset.
  * @rmtoll APB1HRSTR    CRSRST        LL_APB1_GRP2_ForceReset\n
  *         APB1HRSTR    SWPMIRST      LL_APB1_GRP2_ForceReset\n
  *         APB1HRSTR    OPAMPRST      LL_APB1_GRP2_ForceReset\n
  *         APB1HRSTR    MDIOSRST      LL_APB1_GRP2_ForceReset\n
  *         APB1HRSTR    FDCANRST      LL_APB1_GRP2_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP2_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB1HRSTR, Periphs);
}

/**
  * @brief  Release APB1 peripherals reset.
  * @rmtoll APB1HRSTR    CRSRST        LL_APB1_GRP2_ReleaseReset\n
  *         APB1HRSTR    SWPMIRST      LL_APB1_GRP2_ReleaseReset\n
  *         APB1HRSTR    OPAMPRST      LL_APB1_GRP2_ReleaseReset\n
  *         APB1HRSTR    MDIOSRST      LL_APB1_GRP2_ReleaseReset\n
  *         APB1HRSTR    FDCANRST      LL_APB1_GRP2_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP2_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1HRSTR, Periphs);
}

/**
  * @brief  Enable APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1HLPENR   CRSLPEN       LL_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   SWPMILPEN     LL_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   OPAMPLPEN     LL_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   MDIOSLPEN     LL_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   FDCANLPEN     LL_APB1_GRP2_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP2_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB1HLPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB1HLPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1HLPENR   CRSLPEN       LL_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   SWPMILPEN     LL_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   OPAMPLPEN     LL_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   MDIOSLPEN     LL_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   FDCANLPEN     LL_APB1_GRP2_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_APB1_GRP2_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1HLPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB2 APB2
  * @{
  */

/**
  * @brief  Enable APB2 peripherals clock.
  * @rmtoll APB2ENR      TIM1EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM8EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      USART1EN      LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      USART6EN      LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      UART9EN       LL_APB2_GRP1_EnableClock\n (*)
  *         APB2ENR      USART10EN     LL_APB2_GRP1_EnableClock\n (*)
  *         APB2ENR      SPI1EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI4EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM15EN       LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM16EN       LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM17EN       LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI5EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI1EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI2EN        LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI3EN        LL_APB2_GRP1_EnableClock\n (*)
  *         APB2ENR      DFSDM1EN      LL_APB2_GRP1_EnableClock\n
  *         APB2ENR      HRTIMEN       LL_APB2_GRP1_EnableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB2 peripheral clock is enabled or not
  * @rmtoll APB2ENR      TIM1EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM8EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART1EN      LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART6EN      LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      UART9EN       LL_APB2_GRP1_IsEnabledClock\n (*)
  *         APB2ENR      USART10EN     LL_APB2_GRP1_IsEnabledClock\n (*)
  *         APB2ENR      SPI1EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI4EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM15EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM16EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM17EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI5EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI1EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI2EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI3EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      DFSDM1EN      LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      HRTIMEN       LL_APB2_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_APB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB2ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable APB2 peripherals clock.
  * @rmtoll APB2ENR      TIM1EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM8EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      USART1EN      LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      USART6EN      LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      UART9EN       LL_APB2_GRP1_DisableClock\n (*)
  *         APB2ENR      USART10EN     LL_APB2_GRP1_DisableClock\n (*)
  *         APB2ENR      SPI1EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI4EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM15EN       LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM16EN       LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM17EN       LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI5EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI1EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI2EN        LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI3EN        LL_APB2_GRP1_DisableClock\n (*)
  *         APB2ENR      DFSDM1EN      LL_APB2_GRP1_DisableClock\n
  *         APB2ENR      HRTIMEN       LL_APB2_GRP1_DisableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB2ENR, Periphs);
}

/**
  * @brief  Force APB2 peripherals reset.
  * @rmtoll APB2RSTR     TIM1RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     TIM8RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     USART1RST     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     USART6RST     LL_APB2_GRP1_ForceReset\n
  *         APB2ENR      UART9RST      LL_APB2_GRP1_ForceReset\n (*)
  *         APB2ENR      USART10RST    LL_APB2_GRP1_ForceReset\n (*)
  *         APB2RSTR     SPI1RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     SPI4RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     TIM15RST      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     TIM16RST      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     TIM17RST      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     SPI5RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     SAI1RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     SAI2RST       LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     SAI3RST       LL_APB2_GRP1_ForceReset\n (*)
  *         APB2RSTR     DFSDM1RST     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTR     HRTIMRST      LL_APB2_GRP1_ForceReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM  (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB2_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB2RSTR, Periphs);
}

/**
  * @brief  Release APB2 peripherals reset.
  * @rmtoll APB2RSTR     TIM1RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     TIM8RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     USART1RST     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     USART6RST     LL_APB2_GRP1_ReleaseReset\n
  *         APB2ENR      UART9RST      LL_APB2_GRP1_ReleaseReset\n (*)
  *         APB2ENR      USART10RST    LL_APB2_GRP1_ReleaseReset\n (*)
  *         APB2RSTR     SPI1RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     SPI4RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     TIM15RST      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     TIM16RST      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     TIM17RST      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     SPI5RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     SAI1RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     SAI2RST       LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     SAI3RST       LL_APB2_GRP1_ReleaseReset\n (*)
  *         APB2RSTR     DFSDM1RST     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTR     HRTIMRST      LL_APB2_GRP1_ReleaseReset   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM  (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB2RSTR, Periphs);
}

/**
  * @brief  Enable APB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB2LPENR    TIM1LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM8LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    USART1LPEN    LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    USART6LPEN    LL_APB2_GRP1_EnableClockSleep\n
  *         APB2ENR      UART9LPEN     LL_APB2_GRP1_EnableClockSleep\n (*)
  *         APB2ENR      USART10LPEN   LL_APB2_GRP1_EnableClockSleep\n (*)
  *         APB2LPENR    SPI1LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI4LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM15LPEN     LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM16LPEN     LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM17LPEN     LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI5LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI1LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI2LPEN      LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI3LPEN      LL_APB2_GRP1_EnableClockSleep\n (*)
  *         APB2LPENR    DFSDM1LPEN    LL_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    HRTIMLPEN     LL_APB2_GRP1_EnableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  * @retval None
*/
__STATIC_INLINE void LL_APB2_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB2LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB2LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable APB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB2LPENR    TIM1LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM8LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    USART1LPEN    LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    USART6LPEN    LL_APB2_GRP1_DisableClockSleep\n
  *         APB2ENR      UART9LPEN     LL_APB2_GRP1_DisableClockSleep\n (*)
  *         APB2ENR      USART10LPEN   LL_APB2_GRP1_DisableClockSleep\n (*)
  *         APB2LPENR    SPI1LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI4LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM15LPEN     LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM16LPEN     LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM17LPEN     LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI5LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI1LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI2LPEN      LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI3LPEN      LL_APB2_GRP1_DisableClockSleep\n (*)
  *         APB2LPENR    DFSDM1LPEN    LL_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    HRTIMLPEN     LL_APB2_GRP1_DisableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  * @retval None
*/
__STATIC_INLINE void LL_APB2_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB2LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB4 APB4
  * @{
  */

/**
  * @brief  Enable APB4 peripherals clock.
  * @rmtoll APB4ENR      SYSCFGEN      LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPUART1EN     LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      SPI6EN        LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      I2C4EN        LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM2EN      LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM3EN      LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM4EN      LL_APB4_GRP1_EnableClock\n  (*)
  *         APB4ENR      LPTIM5EN      LL_APB4_GRP1_EnableClock\n  (*)
  *         APB4ENR      DAC2EN        LL_APB4_GRP1_EnableClock\n  (*)
  *         APB4ENR      COMP12EN      LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      VREFEN        LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      RTCAPBEN      LL_APB4_GRP1_EnableClock\n
  *         APB4ENR      SAI4EN        LL_APB4_GRP1_EnableClock\n  (*)
  *         APB4ENR      DTSEN         LL_APB4_GRP1_EnableClock\n  (*)
  *         APB4ENR      DFSDM2EN      LL_APB4_GRP1_EnableClock    (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB4ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB4ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB4 peripheral clock is enabled or not
  * @rmtoll APB4ENR      SYSCFGEN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPUART1EN     LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      SPI6EN        LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      I2C4EN        LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM2EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM3EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM4EN      LL_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      LPTIM5EN      LL_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      DAC2EN        LL_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      COMP12EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      VREFEN        LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      RTCAPBEN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      SAI4EN        LL_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      DTSEN         LL_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      DFSDM2EN      LL_APB4_GRP1_IsEnabledClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_APB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB4ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable APB4 peripherals clock.
  * @rmtoll APB4ENR      SYSCFGEN      LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPUART1EN     LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      SPI6EN        LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      I2C4EN        LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM2EN      LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM3EN      LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM4EN      LL_APB4_GRP1_DisableClock\n  (*)
  *         APB4ENR      LPTIM5EN      LL_APB4_GRP1_DisableClock\n  (*)
  *         APB4ENR      DAC2EN        LL_APB4_GRP1_DisableClock\n  (*)
  *         APB4ENR      COMP12EN      LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      VREFEN        LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      RTCAPBEN      LL_APB4_GRP1_DisableClock\n
  *         APB4ENR      SAI4EN        LL_APB4_GRP1_DisableClock\n  (*)
  *         APB4ENR      DTSEN         LL_APB4_GRP1_DisableClock\n  (*)
  *         APB4ENR      DFSDM2EN      LL_APB4_GRP1_DisableClock    (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB4_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB4ENR, Periphs);
}

/**
  * @brief  Force APB4 peripherals reset.
  * @rmtoll APB4RSTR     SYSCFGRST     LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     LPUART1RST    LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     SPI6RST       LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     I2C4RST       LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     LPTIM2RST     LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     LPTIM3RST     LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     LPTIM4RST     LL_APB4_GRP1_ForceReset\n  (*)
  *         APB4RSTR     LPTIM5RST     LL_APB4_GRP1_ForceReset\n  (*)
  *         APB4RSTR     DAC2EN        LL_APB4_GRP1_ForceReset\n  (*)
  *         APB4RSTR     COMP12RST     LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     VREFRST       LL_APB4_GRP1_ForceReset\n
  *         APB4RSTR     SAI4RST       LL_APB4_GRP1_ForceReset\n  (*)
  *         APB4RSTR     DTSRST        LL_APB4_GRP1_ForceReset\n  (*)
  *         APB4RSTR      DFSDM2RST    LL_APB4_GRP1_ForceReset    (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB4_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB4RSTR, Periphs);
}

/**
  * @brief  Release APB4 peripherals reset.
  * @rmtoll APB4RSTR     SYSCFGRST     LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     LPUART1RST    LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     SPI6RST       LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     I2C4RST       LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     LPTIM2RST     LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     LPTIM3RST     LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     LPTIM4RST     LL_APB4_GRP1_ReleaseReset\n  (*)
  *         APB4RSTR     LPTIM5RST     LL_APB4_GRP1_ReleaseReset\n  (*)
  *         APB4RSTR     DAC2RST       LL_APB4_GRP1_ReleaseReset\n  (*)
  *         APB4RSTR     COMP12RST     LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     VREFRST       LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     SAI4RST       LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTR     DTSRST        LL_APB4_GRP1_ReleaseReset\n  (*)
  *         APB4RSTR     DFSDM2RST     LL_APB4_GRP1_ReleaseReset    (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB4_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB4RSTR, Periphs);
}

/**
  * @brief  Enable APB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB4LPENR    SYSCFGLPEN    LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPUART1LPEN   LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    SPI6LPEN      LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    I2C4LPEN      LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM2LPEN    LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM3LPEN    LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM4LPEN    LL_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    LPTIM5LPEN    LL_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    DAC2LPEN      LL_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    COMP12LPEN    LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    VREFLPEN      LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    RTCAPBLPEN    LL_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    SAI4LPEN      LL_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    DTSLPEN       LL_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    DFSDM2LPEN    LL_APB4_GRP1_EnableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB4_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB4LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB4LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable APB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB4LPENR    SYSCFGLPEN    LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPUART1LPEN   LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    SPI6LPEN      LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    I2C4LPEN      LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM2LPEN    LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM3LPEN    LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM4LPEN    LL_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4LPENR    LPTIM5LPEN    LL_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4LPENR    DAC2LPEN      LL_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4LPENR    COMP12LPEN    LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    VREFLPEN      LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    RTCAPBLPEN    LL_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    SAI4LPEN      LL_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4LPENR    DTSLPEN       LL_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4LPENR    DFSDM2LPEN    LL_APB4_GRP1_DisableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DAC2   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_APB4_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB4LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_CLKAM
  * @{
  */

/**
  * @brief  Enable peripherals clock for CLKAM Mode.
  * @rmtoll D3AMR / SRDAMR         BDMA          LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         LPUART1       LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         SPI6          LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         I2C4          LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         LPTIM2        LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         LPTIM3        LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         LPTIM4        LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         LPTIM5        LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         DAC2          LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         COMP12        LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         VREF          LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         RTC           LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         CRC           LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         SAI4          LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         ADC3          LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         DTS           LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         DFSDM2        LL_CLKAM_Enable\n  (*)
  *         D3AMR / SRDAMR         BKPRAM        LL_CLKAM_Enable\n
  *         D3AMR / SRDAMR         SRAM4         LL_CLKAM_Enable
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_CLKAM_PERIPH_BDMA
  *         @arg @ref LL_CLKAM_PERIPH_LPUART1
  *         @arg @ref LL_CLKAM_PERIPH_SPI6
  *         @arg @ref LL_CLKAM_PERIPH_I2C4
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM2
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM3
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_CLKAM_PERIPH_DAC2   (*)
  *         @arg @ref LL_CLKAM_PERIPH_COMP12
  *         @arg @ref LL_CLKAM_PERIPH_VREF
  *         @arg @ref LL_CLKAM_PERIPH_RTC
  *         @arg @ref LL_CLKAM_PERIPH_CRC
  *         @arg @ref LL_CLKAM_PERIPH_SAI4   (*)
  *         @arg @ref LL_CLKAM_PERIPH_ADC3   (*)
  *         @arg @ref LL_CLKAM_PERIPH_DTS    (*)
  *         @arg @ref LL_CLKAM_PERIPH_DFSDM2 (*)
  *         @arg @ref LL_CLKAM_PERIPH_BKPRAM
  *         @arg @ref LL_CLKAM_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_CLKAM_Enable(uint32_t Periphs)
{
  __IO uint32_t tmpreg;

#if defined(RCC_D3AMR_BDMAAMEN)
  SET_BIT(RCC->D3AMR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->D3AMR, Periphs);
#else
  SET_BIT(RCC->SRDAMR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->SRDAMR, Periphs);
#endif /* RCC_D3AMR_BDMAAMEN */
  (void)tmpreg;
}

/**
  * @brief  Disable peripherals clock for CLKAM Mode.
  * @rmtoll D3AMR / SRDAMR        BDMA          LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        LPUART1       LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        SPI6          LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        I2C4          LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        LPTIM2        LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        LPTIM3        LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        LPTIM4        LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        LPTIM5        LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        DAC2          LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        COMP12        LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        VREF          LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        RTC           LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        CRC           LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        SAI4          LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        ADC3          LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        DTS           LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        DFSDM2        LL_CLKAM_Disable\n (*)
  *         D3AMR / SRDAMR        BKPRAM        LL_CLKAM_Disable\n
  *         D3AMR / SRDAMR        SRAM4         LL_CLKAM_Disable
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_CLKAM_PERIPH_BDMA
  *         @arg @ref LL_CLKAM_PERIPH_LPUART1
  *         @arg @ref LL_CLKAM_PERIPH_SPI6
  *         @arg @ref LL_CLKAM_PERIPH_I2C4
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM2
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM3
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_CLKAM_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_CLKAM_PERIPH_DAC2   (*)
  *         @arg @ref LL_CLKAM_PERIPH_COMP12
  *         @arg @ref LL_CLKAM_PERIPH_VREF
  *         @arg @ref LL_CLKAM_PERIPH_RTC
  *         @arg @ref LL_CLKAM_PERIPH_CRC
  *         @arg @ref LL_CLKAM_PERIPH_SAI4 (*)
  *         @arg @ref LL_CLKAM_PERIPH_ADC3 (*)
  *         @arg @ref LL_CLKAM_PERIPH_DTS  (*)
  *         @arg @ref LL_CLKAM_PERIPH_DFSDM2  (*)
  *         @arg @ref LL_CLKAM_PERIPH_BKPRAM
  *         @arg @ref LL_CLKAM_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_CLKAM_Disable(uint32_t Periphs)
{
#if defined(RCC_D3AMR_BDMAAMEN)
  CLEAR_BIT(RCC->D3AMR, Periphs);
#else
  CLEAR_BIT(RCC->SRDAMR, Periphs);
#endif /* RCC_D3AMR_BDMAAMEN */
}

#if defined(RCC_CKGAENR_AXICKG)
/**
  * @}
  */

/**
  * @brief  Enable clock gating for AXI bus peripherals.
  * @rmtoll
  * @param :
  * @retval None
*/
__STATIC_INLINE void LL_CKGA_Enable(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->CKGAENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->CKGAENR, Periphs);
  (void)tmpreg;
}
/**
  * @}
  */
#endif /* RCC_CKGAENR_AXICKG */

#if defined(RCC_CKGAENR_AXICKG)
/**
  * @}
  */

/**
  * @brief  Disable  clock gating for AXI bus peripherals.
  * @rmtoll
  * @param :
  * @retval None
*/
__STATIC_INLINE void LL_CKGA_Disable(uint32_t Periphs)
{
  CLEAR_BIT(RCC->CKGAENR, Periphs);
}
/**
  * @}
  */
#endif /* RCC_CKGAENR_AXICKG */


#if defined(DUAL_CORE)
/** @defgroup BUS_LL_EF_AHB3 AHB3
  * @{
  */

/**
  * @brief  Enable C1 AHB3 peripherals clock.
  * @rmtoll AHB3ENR      MDMAEN        LL_C1_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      DMA2DEN       LL_C1_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      JPGDECEN      LL_C1_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      FMCEN         LL_C1_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      QSPIEN        LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      OSPI1EN       LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      OSPI2EN       LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      IOMNGREN      LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      OTFDEC1EN     LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      OTFDEC2EN     LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      GFXMMU        LL_C1_AHB3_GRP1_EnableClock\n (*)
  *         AHB3ENR      SDMMC1EN      LL_C1_AHB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IOMNGR  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 AHB3 peripheral clock is enabled or not
  * @rmtoll AHB3ENR      MDMAEN        LL_C1_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      DMA2DEN       LL_C1_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      JPGDECEN      LL_C1_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      FMCEN         LL_C1_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      QSPIEN        LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OSPI1EN       LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OSPI2EN       LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      IOMNGREN      LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OTFDEC1EN     LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      OTFDEC2EN     LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      GFXMMU        LL_C1_AHB3_GRP1_IsEnabledClock\n (*)
  *         AHB3ENR      SDMMC1EN      LL_C1_AHB3_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IOMNGR  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_AHB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->AHB3ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 AHB3 peripherals clock.
  * @rmtoll AHB3ENR      MDMAEN        LL_C1_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      DMA2DEN       LL_C1_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      JPGDECEN      LL_C1_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      FMCEN         LL_C1_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      QSPIEN        LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OSPI1EN       LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OSPI2EN       LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      IOMNGREN      LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OTFDEC1EN     LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      OTFDEC2EN     LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      GFXMMU        LL_C1_AHB3_GRP1_DisableClock\n (*)
  *         AHB3ENR      SDMMC1EN      LL_C1_AHB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IOMNGR  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2 (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB3ENR, Periphs);
}

/**
  * @brief  Enable C1 AHB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB3LPENR    MDMALPEN      LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DMA2DLPEN     LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    JPGDECLPEN    LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    FMCLPEN       LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    QSPILPEN      LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OSPI1LPEN     LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OSPI2LPEN     LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    IOMNGRLPEN    LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    GFXMMULPEN    LL_C1_AHB3_GRP1_EnableClockSleep\n  (*)
  *         AHB3LPENR    SDMMC1LPEN    LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    FLASHLPEN     LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DTCM1LPEN     LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DTCM2LPEN     LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    ITCMLPEN      LL_C1_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    AXISRAMLPEN   LL_C1_AHB3_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI     (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB3_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB3LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB3LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 AHB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB3LPENR    MDMALPEN      LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DMA2DLPEN     LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    JPGDECLPEN    LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    FMCLPEN       LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    QSPILPEN      LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OSPI1LPEN     LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OSPI2LPEN     LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    IOMNGRLPEN    LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    OTFDEC1LPEN   LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    GFXMMULPEN    LL_C1_AHB3_GRP1_DisableClockSleep\n  (*)
  *         AHB3LPENR    SDMMC1LPEN    LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    FLASHLPEN     LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DTCM1LPEN     LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DTCM2LPEN     LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    ITCMLPEN      LL_C1_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    AXISRAMLPEN   LL_C1_AHB3_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI     (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI1    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OSPI2    (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OCTOSPIM (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC1  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_OTFDEC2  (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_GFXMMU   (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB3_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB3LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB1 AHB1
  * @{
  */

/**
  * @brief  Enable C1 AHB1 peripherals clock.
  * @rmtoll AHB1ENR      DMA1EN        LL_C1_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      DMA2EN        LL_C1_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ADC12EN       LL_C1_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      CRCEN         LL_C1_AHB1_GRP1_EnableClock\n  (*)
  *         AHB1ENR      ARTEN         LL_C1_AHB1_GRP1_EnableClock\n  (*)
  *         AHB1ENR      ETH1MACEN     LL_C1_AHB1_GRP1_EnableClock\n  (*)
  *         AHB1ENR      ETH1TXEN      LL_C1_AHB1_GRP1_EnableClock\n  (*)
  *         AHB1ENR      ETH1RXEN      LL_C1_AHB1_GRP1_EnableClock\n  (*)
  *         AHB1ENR      USB1OTGHSEN   LL_C1_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_C1_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_C1_AHB1_GRP1_EnableClock\n   (*)
  *         AHB1ENR      USB2OTGHSULPIEN  LL_C1_AHB1_GRP1_EnableClock  (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB1ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB1ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 AHB1 peripheral clock is enabled or not
  * @rmtoll AHB1ENR      DMA1EN        LL_C1_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      DMA2EN        LL_C1_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ADC12EN       LL_C1_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      CRCEN         LL_C1_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      ARTEN         LL_C1_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      ETH1MACEN     LL_C1_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      ETH1TXEN      LL_C1_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      ETH1RXEN      LL_C1_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      USB1OTGHSEN   LL_C1_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_C1_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_C1_AHB1_GRP1_IsEnabledClock\n  (*)
  *         AHB1ENR      USB2OTGHSULPIEN  LL_C1_AHB1_GRP1_IsEnabledClock (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->AHB1ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 AHB1 peripherals clock.
  * @rmtoll AHB1ENR      DMA1EN        LL_C1_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      DMA2EN        LL_C1_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ADC12EN       LL_C1_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      CRCEN         LL_C1_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ARTEN         LL_C1_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ETH1MACEN     LL_C1_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ETH1TXEN      LL_C1_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      ETH1RXEN      LL_C1_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      USB1OTGHSEN   LL_C1_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_C1_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_C1_AHB1_GRP1_DisableClock\n (*)
  *         AHB1ENR      USB2OTGHSULPIEN  LL_C1_AHB1_GRP1_DisableClock (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB1ENR, Periphs);
}

/**
  * @brief  Enable C1 AHB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB1LPENR    DMA1LPEN      LL_C1_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    DMA2LPEN      LL_C1_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ADC12LPEN     LL_C1_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    CRCLPEN       LL_C1_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ARTLPEN       LL_C1_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ETH1MACLPEN   LL_C1_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ETH1TXLPEN    LL_C1_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    ETH1RXLPEN    LL_C1_AHB1_GRP1_EnableClockSleep\n (*)
  *         AHB1LPENR    USB1OTGHSLPEN  LL_C1_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB1OTGHSULPILPEN  LL_C1_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB2OTGHSLPEN  LL_C1_AHB1_GRP1_EnableClockSleep\n   (*)
  *         AHB1LPENR    USB2OTGHSULPILPEN  LL_C1_AHB1_GRP1_EnableClockSleep (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB1_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB1LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB1LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 AHB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB1LPENR    DMA1LPEN      LL_C1_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    DMA2LPEN      LL_C1_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ADC12LPEN     LL_C1_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    CRCLPEN       LL_C1_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ARTLPEN       LL_C1_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ETH1MACLPEN   LL_C1_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ETH1TXLPEN    LL_C1_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    ETH1RXLPEN    LL_C1_AHB1_GRP1_DisableClockSleep\n (*)
  *         AHB1LPENR    USB1OTGHSLPEN  LL_C1_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB1OTGHSULPILPEN  LL_C1_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB2OTGHSLPEN  LL_C1_AHB1_GRP1_DisableClockSleep\n   (*)
  *         AHB1LPENR    USB2OTGHSULPILPEN  LL_C1_AHB1_GRP1_DisableClockSleep (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC  (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX   (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS     (*)
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB1_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB1LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB2 AHB2
  * @{
  */

/**
  * @brief  Enable C1 AHB2 peripherals clock.
  * @rmtoll AHB2ENR      DCMIEN        LL_C1_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      HSEMEN        LL_C1_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      CRYPEN        LL_C1_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      HASHEN        LL_C1_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      RNGEN         LL_C1_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      SDMMC2EN      LL_C1_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      BDMA1EN       LL_C1_AHB2_GRP1_EnableClock\n (*)
  *         AHB2ENR      D2SRAM1EN     LL_C1_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      D2SRAM2EN     LL_C1_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      D2SRAM3EN     LL_C1_AHB2_GRP1_EnableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1 (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 AHB2 peripheral clock is enabled or not
  * @rmtoll AHB2ENR      DCMIEN        LL_C1_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      HSEMEN        LL_C1_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      CRYPEN        LL_C1_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      HASHEN        LL_C1_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      RNGEN         LL_C1_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      SDMMC2EN      LL_C1_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      BDMA1EN       LL_C1_AHB2_GRP1_IsEnabledClock\n (*)
  *         AHB2ENR      D2SRAM1EN     LL_C1_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      D2SRAM2EN     LL_C1_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      D2SRAM3EN     LL_C1_AHB2_GRP1_IsEnabledClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1 (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_AHB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->AHB2ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 AHB2 peripherals clock.
  * @rmtoll AHB2ENR      DCMIEN        LL_C1_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      HSEMEN        LL_C1_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      CRYPEN        LL_C1_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      HASHEN        LL_C1_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      RNGEN         LL_C1_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      SDMMC2EN      LL_C1_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      BDMA1EN       LL_C1_AHB2_GRP1_DisableClock\n (*)
  *         AHB2ENR      D2SRAM1EN     LL_C1_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      D2SRAM2EN     LL_C1_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      D2SRAM3EN     LL_C1_AHB2_GRP1_DisableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDMA1 (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB2ENR, Periphs);
}

/**
  * @brief  Enable C1 AHB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB2LPENR    DCMILPEN      LL_C1_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    CRYPLPEN      LL_C1_AHB2_GRP1_EnableClockSleep\n (*)
  *         AHB2LPENR    HASHLPEN      LL_C1_AHB2_GRP1_EnableClockSleep\n (*)
  *         AHB2LPENR    RNGLPEN       LL_C1_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_C1_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_C1_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    BDAM1LPEN     LL_C1_AHB2_GRP1_EnableClockSleep\n (*)
  *         AHB2LPENR    D2SRAM2LPEN   LL_C1_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    D2SRAM3LPEN   LL_C1_AHB2_GRP1_EnableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDAM1 (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB2_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB2LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB2LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 AHB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB2LPENR    DCMILPEN      LL_C1_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    CRYPLPEN      LL_C1_AHB2_GRP1_DisableClockSleep\n (*)
  *         AHB2LPENR    HASHLPEN      LL_C1_AHB2_GRP1_DisableClockSleep\n (*)
  *         AHB2LPENR    RNGLPEN       LL_C1_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_C1_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    BDAM1LPEN     LL_C1_AHB2_GRP1_DisableClockSleep\n (*)
  *         AHB2LPENR    D2SRAM1LPEN   LL_C1_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM2LPEN   LL_C1_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM3LPEN   LL_C1_AHB2_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_BDAM1 (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB2_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB2LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB4 AHB4
  * @{
  */

/**
  * @brief  Enable C1 AHB4 peripherals clock.
  * @rmtoll AHB4ENR      GPIOAEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOBEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOCEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIODEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOEEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOFEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOGEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOHEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOIEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOJEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOKEN       LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      CRCEN         LL_C1_AHB4_GRP1_EnableClock\n (*)
  *         AHB4ENR      BDMAEN        LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      ADC3EN        LL_C1_AHB4_GRP1_EnableClock\n (*)
  *         AHB4ENR      HSEMEN        LL_C1_AHB4_GRP1_EnableClock\n (*)
  *         AHB4ENR      BKPRAMEN      LL_C1_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      SRAM4EN       LL_C1_AHB4_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB4ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB4ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 AHB4 peripheral clock is enabled or not
  * @rmtoll AHB4ENR      GPIOAEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOBEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOCEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIODEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOEEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOFEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOGEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOHEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOIEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOJEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOKEN       LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      CRCEN         LL_C1_AHB4_GRP1_IsEnabledClock\n (*)
  *         AHB4ENR      BDMAEN        LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      ADC3EN        LL_C1_AHB4_GRP1_IsEnabledClock\n (*)
  *         AHB4ENR      HSEMEN        LL_C1_AHB4_GRP1_IsEnabledClock\n (*)
  *         AHB4ENR      BKPRAMEN      LL_C1_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      SRAM4EN       LL_C1_AHB4_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_AHB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->AHB4ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 AHB4 peripherals clock.
  * @rmtoll AHB4ENR      GPIOAEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOBEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOCEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIODEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOEEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOFEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOGEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOHEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOIEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOJEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOKEN       LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      CRCEN         LL_C1_AHB4_GRP1_DisableClock\n (*)
  *         AHB4ENR      BDMAEN        LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      ADC3EN        LL_C1_AHB4_GRP1_DisableClock\n (*)
  *         AHB4ENR      HSEMEN        LL_C1_AHB4_GRP1_DisableClock\n (*)
  *         AHB4ENR      BKPRAMEN      LL_C1_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      SRAM4EN       LL_C1_AHB4_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB4_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB4ENR, Periphs);
}

/**
  * @brief  Enable C1 AHB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB4LPENR    GPIOALPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOBLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOCLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIODLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOELPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOFLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOGLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOHLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOILPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOJLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOKLPEN     LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    CRCLPEN       LL_C1_AHB4_GRP1_EnableClockSleep\n (*)
  *         AHB4LPENR    BDMALPEN      LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    ADC3LPEN      LL_C1_AHB4_GRP1_EnableClockSleep\n (*)
  *         AHB4LPENR    BKPRAMLPEN    LL_C1_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    SRAM4LPEN     LL_C1_AHB4_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB4_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->AHB4LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->AHB4LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 AHB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB4LPENR    GPIOALPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOBLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOCLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIODLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOELPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOFLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOGLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOHLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOILPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOJLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOKLPEN     LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    CRCLPEN       LL_C1_AHB4_GRP1_DisableClockSleep\n (*)
  *         AHB4LPENR    BDMALPEN      LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    ADC3LPEN      LL_C1_AHB4_GRP1_DisableClockSleep\n (*)
  *         AHB4LPENR    BKPRAMLPEN    LL_C1_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    SRAM4LPEN     LL_C1_AHB4_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC  (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3 (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  * @retval None
*/
__STATIC_INLINE void LL_C1_AHB4_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->AHB4LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB3 APB3
  * @{
  */

/**
  * @brief  Enable C1 APB3 peripherals clock.
  * @rmtoll APB3ENR      LTDCEN        LL_C1_APB3_GRP1_EnableClock\n (*)
  *         APB3ENR      DSIEN         LL_C1_APB3_GRP1_EnableClock\n (*)
  *         APB3ENR      WWDG1EN       LL_C1_APB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI  (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 APB3 peripheral clock is enabled or not
  * @rmtoll APB3ENR      LTDCEN        LL_C1_APB3_GRP1_IsEnabledClock\n (*)
  *         APB3ENR      DSIEN         LL_C1_APB3_GRP1_IsEnabledClock\n (*)
  *         APB3ENR      WWDG1EN       LL_C1_APB3_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI  (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_APB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->APB3ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 APB3 peripherals clock.
  * @rmtoll APB3ENR      LTDCEN        LL_C1_APB3_GRP1_DisableClock\n (*)
  *         APB3ENR      DSIEN         LL_C1_APB3_GRP1_DisableClock\n (*)
  *         APB3ENR      WWDG1EN       LL_C1_APB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI  (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB3ENR, Periphs);
}

/**
  * @brief  Enable C1 APB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB3LPENR    LTDCLPEN      LL_C1_APB3_GRP1_EnableClockSleep\n (*)
  *         APB3LPENR    DSILPEN       LL_C1_APB3_GRP1_EnableClockSleep\n (*)
  *         APB3LPENR    WWDG1LPEN     LL_C1_APB3_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI  (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB3_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB3LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB3LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 APB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB3LPENR    LTDCLPEN      LL_C1_APB3_GRP1_DisableClockSleep\n (*)
  *         APB3LPENR    DSILPEN       LL_C1_APB3_GRP1_DisableClockSleep\n (*)
  *         APB3LPENR    WWDG1LPEN     LL_C1_APB3_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI  (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB3_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB3LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB1 APB1
  * @{
  */

/**
  * @brief  Enable C1 APB1 peripherals clock.
  * @rmtoll APB1LENR     TIM2EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM3EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM4EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM5EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM6EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM7EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM12EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM13EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM14EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     LPTIM1EN      LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     WWDG2EN       LL_C1_APB1_GRP1_EnableClock\n (*)
  *         APB1LENR     SPI2EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPI3EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPDIFRXEN     LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     USART2EN      LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     USART3EN      LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART4EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART5EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C1EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C2EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C3EN        LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     CECEN         LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     DAC12EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART7EN       LL_C1_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART8EN       LL_C1_APB1_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB1LENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB1LENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 APB1 peripheral clock is enabled or not
  * @rmtoll APB1LENR     TIM2EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM3EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM4EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM5EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM6EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM7EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM12EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM13EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM14EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     LPTIM1EN      LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     WWDG2EN       LL_C1_APB1_GRP1_IsEnabledClock\n (*)
  *         APB1LENR     SPI2EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPI3EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPDIFRXEN     LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     USART2EN      LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     USART3EN      LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART4EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART5EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C1EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C2EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C3EN        LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     CECEN         LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     DAC12EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART7EN       LL_C1_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART8EN       LL_C1_APB1_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->APB1LENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 APB1 peripherals clock.
  * @rmtoll APB1LENR     TIM2EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM3EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM4EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM5EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM6EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM7EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM12EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM13EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM14EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     LPTIM1EN      LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     WWDG2EN       LL_C1_APB1_GRP1_DisableClock\n (*)
  *         APB1LENR     SPI2EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPI3EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPDIFRXEN     LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     USART2EN      LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     USART3EN      LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART4EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART5EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C1EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C2EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C3EN        LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     CECEN         LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     DAC12EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART7EN       LL_C1_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART8EN       LL_C1_APB1_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE void LL_C1_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB1LENR, Periphs);
}

/**
  * @brief  Enable C1 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1LLPENR   TIM2LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM3LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM4LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM5LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM6LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM7LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM12LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM13LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM14LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   LPTIM1LPEN    LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   WWDG2LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n (*)
  *         APB1LLPENR   SPI2LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPI3LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPDIFRXLPEN   LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   USART2LPEN    LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   USART3LPEN    LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART4LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART5LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C1LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C2LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C3LPEN      LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   CECLPEN       LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   DAC12LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART7LPEN     LL_C1_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART8LPEN     LL_C1_APB1_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB1LLPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB1LLPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1LLPENR   TIM2LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM3LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM4LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM5LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM6LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM7LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM12LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM13LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM14LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   LPTIM1LPEN    LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   WWDG2LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n (*)
  *         APB1LLPENR   SPI2LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPI3LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPDIFRXLPEN   LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   USART2LPEN    LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   USART3LPEN    LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART4LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART5LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C1LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C2LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C3LPEN      LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   CECLPEN       LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   DAC12LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART7LPEN     LL_C1_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART8LPEN     LL_C1_APB1_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2 (*)
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB1LLPENR, Periphs);
}

/**
  * @brief  Enable C1 APB1 peripherals clock.
  * @rmtoll APB1HENR     CRSEN         LL_C1_APB1_GRP2_EnableClock\n
  *         APB1HENR     SWPMIEN       LL_C1_APB1_GRP2_EnableClock\n
  *         APB1HENR     OPAMPEN       LL_C1_APB1_GRP2_EnableClock\n
  *         APB1HENR     MDIOSEN       LL_C1_APB1_GRP2_EnableClock\n
  *         APB1HENR     FDCANEN       LL_C1_APB1_GRP2_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP2_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB1HENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB1HENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 APB1 peripheral clock is enabled or not
  * @rmtoll APB1HENR     CRSEN         LL_C1_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     SWPMIEN       LL_C1_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     OPAMPEN       LL_C1_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     MDIOSEN       LL_C1_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     FDCANEN       LL_C1_APB1_GRP2_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_APB1_GRP2_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->APB1HENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 APB1 peripherals clock.
  * @rmtoll APB1HENR     CRSEN         LL_C1_APB1_GRP2_DisableClock\n
  *         APB1HENR     SWPMIEN       LL_C1_APB1_GRP2_DisableClock\n
  *         APB1HENR     OPAMPEN       LL_C1_APB1_GRP2_DisableClock\n
  *         APB1HENR     MDIOSEN       LL_C1_APB1_GRP2_DisableClock\n
  *         APB1HENR     FDCANEN       LL_C1_APB1_GRP2_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP2_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB1HENR, Periphs);
}

/**
  * @brief  Enable C1 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1HLPENR   CRSLPEN       LL_C1_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   SWPMILPEN     LL_C1_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   OPAMPLPEN     LL_C1_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   MDIOSLPEN     LL_C1_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   FDCANLPEN     LL_C1_APB1_GRP2_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP2_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB1HLPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB1HLPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1HLPENR   CRSLPEN       LL_C1_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   SWPMILPEN     LL_C1_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   OPAMPLPEN     LL_C1_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   MDIOSLPEN     LL_C1_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   FDCANLPEN     LL_C1_APB1_GRP2_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB1_GRP2_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB1HLPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB2 APB2
  * @{
  */

/**
  * @brief  Enable C1 APB2 peripherals clock.
  * @rmtoll APB2ENR      TIM1EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM8EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      USART1EN      LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      USART6EN      LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      UART9EN       LL_C1_APB2_GRP1_EnableClock\n (*)
  *         APB2ENR      USART10EN     LL_C1_APB2_GRP1_EnableClock\n (*)
  *         APB2ENR      SPI1EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI4EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM15EN       LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM16EN       LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM17EN       LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI5EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI1EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI2EN        LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI3EN        LL_C1_APB2_GRP1_EnableClock\n (*)
  *         APB2ENR      DFSDM1EN      LL_C1_APB2_GRP1_EnableClock\n
  *         APB2ENR      HRTIMEN       LL_C1_APB2_GRP1_EnableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 APB2 peripheral clock is enabled or not
  * @rmtoll APB2ENR      TIM1EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM8EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART1EN      LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART6EN      LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      UART9EN       LL_C1_APB2_GRP1_IsEnabledClock\n (*)
  *         APB2ENR      USART10EN     LL_C1_APB2_GRP1_IsEnabledClock\n (*)
  *         APB2ENR      SPI1EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI4EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM15EN       LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM16EN       LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM17EN       LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI5EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI1EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI2EN        LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI3EN        LL_C1_APB2_GRP1_IsEnabledClock\n (*)
  *         APB2ENR      DFSDM1EN      LL_C1_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      HRTIMEN       LL_C1_APB2_GRP1_IsEnabledClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE uint32_t LL_C1_APB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->APB2ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 APB2 peripherals clock.
  * @rmtoll APB2ENR      TIM1EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM8EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      USART1EN      LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      USART6EN      LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      UART9EN       LL_C1_APB2_GRP1_DisableClock\n (*)
  *         APB2ENR      USART10EN     LL_C1_APB2_GRP1_DisableClock\n (*)
  *         APB2ENR      SPI1EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI4EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM15EN       LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM16EN       LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM17EN       LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI5EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI1EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI2EN        LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI3EN        LL_C1_APB2_GRP1_DisableClock\n  (*)
  *         APB2ENR      DFSDM1EN      LL_C1_APB2_GRP1_DisableClock\n
  *         APB2ENR      HRTIMEN       LL_C1_APB2_GRP1_DisableClock    (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB2ENR, Periphs);
}

/**
  * @brief  Enable C1 APB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB2LPENR    TIM1LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM8LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    USART1LPEN    LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    USART6LPEN    LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2ENR      UART9EN       LL_C1_APB2_GRP1_EnableClockSleep\n (*)
  *         APB2ENR      USART10EN     LL_C1_APB2_GRP1_EnableClockSleep\n (*)
  *         APB2LPENR    SPI1LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI4LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM15LPEN     LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM16LPEN     LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM17LPEN     LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI5LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI1LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI2LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI3LPEN      LL_C1_APB2_GRP1_EnableClockSleep\n (*)
  *         APB2LPENR    DFSDM1LPEN    LL_C1_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    HRTIMLPEN     LL_C1_APB2_GRP1_EnableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB2_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB2LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB2LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 APB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB2LPENR    TIM1LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM8LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    USART1LPEN    LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    UART9LPEN     LL_C1_APB2_GRP1_DisableClockSleep\n (*)
  *         APB2LPENR    USART10LPEN   LL_C1_APB2_GRP1_DisableClockSleep\n (*)
  *         APB2LPENR    USART6LPEN    LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI1LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI4LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM15LPEN     LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM16LPEN     LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM17LPEN     LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI5LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI1LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI2LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI3LPEN      LL_C1_APB2_GRP1_DisableClockSleep\n (*)
  *         APB2LPENR    DFSDM1LPEN    LL_C1_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    HRTIMLPEN     LL_C1_APB2_GRP1_DisableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9   (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10 (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3  (*)
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB2_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB2LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB4 APB4
  * @{
  */

/**
  * @brief  Enable C1 APB4 peripherals clock.
  * @rmtoll APB4ENR      SYSCFGEN      LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPUART1EN     LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      SPI6EN        LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      I2C4EN        LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM2EN      LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM3EN      LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM4EN      LL_C1_APB4_GRP1_EnableClock\n (*)
  *         APB4ENR      LPTIM5EN      LL_C1_APB4_GRP1_EnableClock\n (*)
  *         APB4ENR      DAC2EN        LL_C1_APB4_GRP1_EnableClock\n (*)
  *         APB4ENR      COMP12EN      LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      VREFEN        LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      RTCAPBEN      LL_C1_APB4_GRP1_EnableClock\n
  *         APB4ENR      SAI4EN        LL_C1_APB4_GRP1_EnableClock\n (*)
  *         APB4ENR      DTSEN         LL_C1_APB4_GRP1_EnableClock\n (*)
  *         APB4ENR      DFSDM2EN      LL_C1_APB4_GRP1_EnableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB4ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB4ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C1 APB4 peripheral clock is enabled or not
  * @rmtoll APB4ENR      SYSCFGEN      LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPUART1EN     LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      SPI6EN        LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      I2C4EN        LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM2EN      LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM3EN      LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM4EN      LL_C1_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      LPTIM5EN      LL_C1_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      COMP12EN      LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      VREFEN        LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      RTCAPBEN      LL_C1_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      SAI4EN        LL_C1_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      DTSEN         LL_C1_APB4_GRP1_IsEnabledClock\n (*)
  *         APB4ENR      DFSDM2EN      LL_C1_APB4_GRP1_IsEnabledClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C1_APB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C1->APB4ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C1 APB4 peripherals clock.
  * @rmtoll APB4ENR      SYSCFGEN      LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPUART1EN     LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      SPI6EN        LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      I2C4EN        LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM2EN      LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM3EN      LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM4EN      LL_C1_APB4_GRP1_DisableClock\n (*)
  *         APB4ENR      LPTIM5EN      LL_C1_APB4_GRP1_DisableClock\n (*)
  *         APB4ENR      COMP12EN      LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      VREFEN        LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      RTCAPBEN      LL_C1_APB4_GRP1_DisableClock\n
  *         APB4ENR      SAI4EN        LL_C1_APB4_GRP1_DisableClock\n (*)
  *         APB4ENR      DTSEN         LL_C1_APB4_GRP1_DisableClock\n (*)
  *         APB4ENR      DFSDM2EN      LL_C1_APB4_GRP1_DisableClock   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB4_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB4ENR, Periphs);
}

/**
  * @brief  Enable C1 APB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB4LPENR    SYSCFGLPEN    LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPUART1LPEN   LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    SPI6LPEN      LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    I2C4LPEN      LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM2LPEN    LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM3LPEN    LL_C1_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    LPTIM4LPEN    LL_C1_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4LPENR    LPTIM5LPEN    LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    COMP12LPEN    LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    VREFLPEN      LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    RTCAPBLPEN    LL_C1_APB4_GRP1_EnableClockSleep\n
  *         APB4ENR      SAI4EN        LL_C1_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4ENR      DTSEN         LL_C1_APB4_GRP1_EnableClockSleep\n (*)
  *         APB4ENR      DFSDM2EN      LL_C1_APB4_GRP1_EnableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB4_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C1->APB4LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C1->APB4LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C1 APB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB4LPENR    SYSCFGLPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPUART1LPEN   LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    SPI6LPEN      LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    I2C4LPEN      LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM2LPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM3LPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM4LPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM5LPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    COMP12LPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    VREFLPEN      LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    RTCAPBLPEN    LL_C1_APB4_GRP1_DisableClockSleep\n
  *         APB4ENR      SAI4EN        LL_C1_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4ENR      DTSEN         LL_C1_APB4_GRP1_DisableClockSleep\n (*)
  *         APB4ENR      DFSDM2EN      LL_C1_APB4_GRP1_DisableClockSleep   (*)
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5 (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4   (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DTS    (*)
  *         @arg @ref LL_APB4_GRP1_PERIPH_DFSDM2 (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C1_APB4_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C1->APB4LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB3 AHB3
  * @{
  */

/**
  * @brief  Enable C2 AHB3 peripherals clock.
  * @rmtoll AHB3ENR      MDMAEN        LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      DMA2DEN       LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      JPGDECEN      LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      FMCEN         LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      QSPIEN        LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      SDMMC1EN      LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      FLASHEN       LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      DTCM1EN       LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      DTCM2EN       LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      ITCMEN        LL_C2_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      AXISRAMEN     LL_C2_AHB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 AHB3 peripheral clock is enabled or not
  * @rmtoll AHB3ENR      MDMAEN        LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      DMA2DEN       LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      JPGDECEN      LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      FMCEN         LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      QSPIEN        LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      SDMMC1EN      LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      FLASHEN       LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      DTCM1EN       LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      DTCM2EN       LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      ITCMEN        LL_C2_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      AXISRAMEN     LL_C2_AHB3_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_AHB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->AHB3ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 AHB3 peripherals clock.
  * @rmtoll AHB3ENR      MDMAEN        LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      DMA2DEN       LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      JPGDECEN      LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      FMCEN         LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      QSPIEN        LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      SDMMC1EN      LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      FLASHEN       LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      DTCM1EN       LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      DTCM2EN       LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      ITCMEN        LL_C2_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      AXISRAMEN     LL_C2_AHB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_MDMA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB3ENR, Periphs);
}

/**
  * @brief  Enable C2 AHB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB3LPENR    MDMALPEN      LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DMA2DLPEN     LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    JPGDECLPEN    LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    FMCLPEN       LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    QSPILPEN      LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    SDMMC1LPEN    LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    FLASHLPEN     LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DTCM1LPEN     LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    DTCM2LPEN     LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    ITCMLPEN      LL_C2_AHB3_GRP1_EnableClockSleep\n
  *         AHB3LPENR    AXISRAMLPEN   LL_C2_AHB3_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB3_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB3LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB3LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 AHB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB3LPENR    MDMALPEN      LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DMA2DLPEN     LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    JPGDECLPEN    LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    FMCLPEN       LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    QSPILPEN      LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    SDMMC1LPEN    LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    FLASHLPEN     LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DTCM1LPEN     LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    DTCM2LPEN     LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    ITCMLPEN      LL_C2_AHB3_GRP1_DisableClockSleep\n
  *         AHB3LPENR    AXISRAMLPEN   LL_C2_AHB3_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB3_GRP1_PERIPH_JPGDEC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QSPI
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM1
  *         @arg @ref LL_AHB3_GRP1_PERIPH_DTCM2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_ITCM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AXISRAM
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB3_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB3LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB1 AHB1
  * @{
  */

/**
  * @brief  Enable C2 AHB1 peripherals clock.
  * @rmtoll AHB1ENR      DMA1EN        LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      DMA2EN        LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ADC12EN       LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ARTEN         LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ETH1MACEN     LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ETH1TXEN      LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      ETH1RXEN      LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB1OTGHSEN   LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_C2_AHB1_GRP1_EnableClock\n
  *         AHB1ENR      USB2OTGHSULPIEN  LL_C2_AHB1_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB1ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB1ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 AHB1 peripheral clock is enabled or not
  * @rmtoll AHB1ENR      DMA1EN        LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      DMA2EN        LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ADC12EN       LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ARTEN         LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ETH1MACEN     LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ETH1TXEN      LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ETH1RXEN      LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB1OTGHSEN   LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_C2_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      USB2OTGHSULPIEN  LL_C2_AHB1_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->AHB1ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 AHB1 peripherals clock.
  * @rmtoll AHB1ENR      DMA1EN        LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      DMA2EN        LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ADC12EN       LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ARTEN         LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ETH1MACEN     LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ETH1TXEN      LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      ETH1RXEN      LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB1OTGHSEN   LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB1OTGHSULPIEN  LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB2OTGHSEN   LL_C2_AHB1_GRP1_DisableClock\n
  *         AHB1ENR      USB2OTGHSULPIEN  LL_C2_AHB1_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB1ENR, Periphs);
}

/**
  * @brief  Enable C2 AHB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB1LPENR    DMA1LPEN      LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    DMA2LPEN      LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ADC12LPEN     LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ARTLPEN       LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ETH1MACLPEN   LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ETH1TXLPEN    LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    ETH1RXLPEN    LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB1OTGHSLPEN  LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB1OTGHSULPILPEN  LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB2OTGHSLPEN  LL_C2_AHB1_GRP1_EnableClockSleep\n
  *         AHB1LPENR    USB2OTGHSULPILPEN  LL_C2_AHB1_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB1_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB1LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB1LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 AHB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB1LPENR    DMA1LPEN      LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    DMA2LPEN      LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ADC12LPEN     LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ARTLPEN       LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ETH1MACLPEN   LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ETH1TXLPEN    LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    ETH1RXLPEN    LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB1OTGHSLPEN  LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB1OTGHSULPILPEN  LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB2OTGHSLPEN  LL_C2_AHB1_GRP1_DisableClockSleep\n
  *         AHB1LPENR    USB2OTGHSULPILPEN  LL_C2_AHB1_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ART
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB1OTGHSULPI
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHS
  *         @arg @ref LL_AHB1_GRP1_PERIPH_USB2OTGHSULPI
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB1_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB1LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB2 AHB2
  * @{
  */

/**
  * @brief  Enable C2 AHB2 peripherals clock.
  * @rmtoll AHB2ENR      DCMIEN        LL_C2_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      CRYPEN        LL_C2_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      HASHEN        LL_C2_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      RNGEN         LL_C2_AHB2_GRP1_EnableClock\n
  *         AHB2ENR      SDMMC2EN      LL_C2_AHB2_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 AHB2 peripheral clock is enabled or not
  * @rmtoll AHB2ENR      DCMIEN        LL_C2_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      CRYPEN        LL_C2_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      HASHEN        LL_C2_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      RNGEN         LL_C2_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      SDMMC2EN      LL_C2_AHB2_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_AHB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->AHB2ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 AHB2 peripherals clock.
  * @rmtoll AHB2ENR      DCMIEN        LL_C2_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      CRYPEN        LL_C2_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      HASHEN        LL_C2_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      RNGEN         LL_C2_AHB2_GRP1_DisableClock\n
  *         AHB2ENR      SDMMC2EN      LL_C2_AHB2_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB2ENR, Periphs);
}

/**
  * @brief  Enable C2 AHB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB2LPENR    DCMILPEN      LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    CRYPLPEN      LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    HASHLPEN      LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    RNGLPEN       LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    D2SRAM1LPEN   LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    D2SRAM2LPEN   LL_C2_AHB2_GRP1_EnableClockSleep\n
  *         AHB2LPENR    D2SRAM3LPEN   LL_C2_AHB2_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB2_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB2LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB2LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 AHB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB2LPENR    DCMILPEN      LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    CRYPLPEN      LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    HASHLPEN      LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    RNGLPEN       LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    SDMMC2LPEN    LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM1LPEN   LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM2LPEN   LL_C2_AHB2_GRP1_DisableClockSleep\n
  *         AHB2LPENR    D2SRAM3LPEN   LL_C2_AHB2_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_DCMI
  *         @arg @ref LL_AHB2_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_HASH (*)
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM2
  *         @arg @ref LL_AHB2_GRP1_PERIPH_D2SRAM3
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB2_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB2LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB4 AHB4
  * @{
  */

/**
  * @brief  Enable C2 AHB4 peripherals clock.
  * @rmtoll AHB4ENR      GPIOAEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOBEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOCEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIODEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOEEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOFEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOGEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOHEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOIEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOJEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      GPIOKEN       LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      CRCEN         LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      BDMAEN        LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      ADC3EN        LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      HSEMEN        LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      BKPRAMEN      LL_C2_AHB4_GRP1_EnableClock\n
  *         AHB4ENR      SRAM4EN     LL_C2_AHB4_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB4ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB4ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 AHB4 peripheral clock is enabled or not
  * @rmtoll AHB4ENR      GPIOAEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOBEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOCEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIODEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOEEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOFEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOGEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOHEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOIEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOJEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOKEN       LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      CRCEN         LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      BDMAEN        LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      ADC3EN        LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      HSEMEN        LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      BKPRAMEN      LL_C2_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      SRAM4EN       LL_C2_AHB4_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_AHB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->AHB4ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 AHB4 peripherals clock.
  * @rmtoll AHB4ENR      GPIOAEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOBEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOCEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIODEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOEEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOFEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOGEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOHEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOIEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOJEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      GPIOKEN       LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      CRCEN         LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      BDMAEN        LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      ADC3EN        LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      HSEMEN        LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      BKPRAMEN      LL_C2_AHB4_GRP1_DisableClock\n
  *         AHB4ENR      SRAM4EN       LL_C2_AHB4_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3
  *         @arg @ref LL_AHB4_GRP1_PERIPH_HSEM (*)
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB4_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB4ENR, Periphs);
}

/**
  * @brief  Enable C2 AHB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB4LPENR    GPIOALPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOBLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOCLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIODLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOELPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOFLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOGLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOHLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOILPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOJLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    GPIOKLPEN     LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    CRCLPEN       LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    BDMALPEN      LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    ADC3LPEN      LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    BKPRAMLPEN    LL_C2_AHB4_GRP1_EnableClockSleep\n
  *         AHB4LPENR    SRAM4LPEN     LL_C2_AHB4_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB4_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->AHB4LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->AHB4LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 AHB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll AHB4LPENR    GPIOALPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOBLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOCLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIODLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOELPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOFLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOGLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOHLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOILPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOJLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    GPIOKLPEN     LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    CRCLPEN       LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    BDMALPEN      LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    ADC3LPEN      LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    BKPRAMLPEN    LL_C2_AHB4_GRP1_DisableClockSleep\n
  *         AHB4LPENR    SRAM4LPEN     LL_C2_AHB4_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOI
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOJ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOK
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BDMA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_ADC3
  *         @arg @ref LL_AHB4_GRP1_PERIPH_BKPRAM
  *         @arg @ref LL_AHB4_GRP1_PERIPH_SRAM4
  * @retval None
*/
__STATIC_INLINE void LL_C2_AHB4_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->AHB4LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB3 APB3
  * @{
  */

/**
  * @brief  Enable C2 APB3 peripherals clock.
  * @rmtoll APB3ENR      LTDCEN        LL_C2_APB3_GRP1_EnableClock\n
  *         APB3ENR      DSIEN         LL_C2_APB3_GRP1_EnableClock\n
  *         APB3ENR      WWDG1EN       LL_C2_APB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 APB3 peripheral clock is enabled or not
  * @rmtoll APB3ENR      LTDCEN        LL_C2_APB3_GRP1_IsEnabledClock\n
  *         APB3ENR      DSIEN         LL_C2_APB3_GRP1_IsEnabledClock\n
  *         APB3ENR      WWDG1EN       LL_C2_APB3_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_APB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->APB3ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 APB3 peripherals clock.
  * @rmtoll APB3ENR      LTDCEN        LL_C2_APB3_GRP1_DisableClock\n
  *         APB3ENR      DSIEN         LL_C2_APB3_GRP1_DisableClock\n
  *         APB3ENR      WWDG1EN       LL_C2_APB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB3ENR, Periphs);
}

/**
  * @brief  Enable C2 APB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB3LPENR    LTDCLPEN      LL_C2_APB3_GRP1_EnableClockSleep\n
  *         APB3LPENR    DSILPEN       LL_C2_APB3_GRP1_EnableClockSleep\n
  *         APB3LPENR    WWDG1LPEN     LL_C2_APB3_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB3_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB3LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB3LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 APB3 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB3LPENR    LTDCLPEN      LL_C2_APB3_GRP1_DisableClockSleep\n
  *         APB3LPENR    DSILPEN       LL_C2_APB3_GRP1_DisableClockSleep\n
  *         APB3LPENR    WWDG1LPEN     LL_C2_APB3_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB3_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB3_GRP1_PERIPH_DSI (*)
  *         @arg @ref LL_APB3_GRP1_PERIPH_WWDG1
  *
  *         (*) value not defined in all devices.
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB3_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB3LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB1 APB1
  * @{
  */

/**
  * @brief  Enable C2 APB1 peripherals clock.
  * @rmtoll APB1LENR     TIM2EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM3EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM4EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM5EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM6EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM7EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM12EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM13EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     TIM14EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     LPTIM1EN      LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     WWDG2EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPI2EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPI3EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     SPDIFRXEN     LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     USART2EN      LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     USART3EN      LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART4EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART5EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C1EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C2EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     I2C3EN        LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     CECEN         LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     DAC12EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART7EN       LL_C2_APB1_GRP1_EnableClock\n
  *         APB1LENR     UART8EN       LL_C2_APB1_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB1LENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB1LENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 APB1 peripheral clock is enabled or not
  * @rmtoll APB1LENR     TIM2EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM3EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM4EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM5EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM6EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM7EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM12EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM13EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     TIM14EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     LPTIM1EN      LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     WWDG2EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPI2EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPI3EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     SPDIFRXEN     LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     USART2EN      LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     USART3EN      LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART4EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART5EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C1EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C2EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     I2C3EN        LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     CECEN         LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     DAC12EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART7EN       LL_C2_APB1_GRP1_IsEnabledClock\n
  *         APB1LENR     UART8EN       LL_C2_APB1_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->APB1LENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 APB1 peripherals clock.
  * @rmtoll APB1LENR     TIM2EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM3EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM4EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM5EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM6EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM7EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM12EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM13EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     TIM14EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     LPTIM1EN      LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     WWDG2EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPI2EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPI3EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     SPDIFRXEN     LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     USART2EN      LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     USART3EN      LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART4EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART5EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C1EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C2EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     I2C3EN        LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     CECEN         LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     DAC12EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART7EN       LL_C2_APB1_GRP1_DisableClock\n
  *         APB1LENR     UART8EN       LL_C2_APB1_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB1LENR, Periphs);
}

/**
  * @brief  Enable C2 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1LLPENR   TIM2LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM3LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM4LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM5LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM6LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM7LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM12LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM13LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   TIM14LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   LPTIM1LPEN    LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   WWDG2LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPI2LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPI3LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   SPDIFRXLPEN   LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   USART2LPEN    LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   USART3LPEN    LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART4LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART5LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C1LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C2LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   I2C3LPEN      LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   CECLPEN       LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   DAC12LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART7LPEN     LL_C2_APB1_GRP1_EnableClockSleep\n
  *         APB1LLPENR   UART8LPEN     LL_C2_APB1_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB1LLPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB1LLPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1LLPENR   TIM2LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM3LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM4LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM5LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM6LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM7LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM12LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM13LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   TIM14LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   LPTIM1LPEN    LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   WWDG2LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPI2LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPI3LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   SPDIFRXLPEN   LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   USART2LPEN    LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   USART3LPEN    LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART4LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART5LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C1LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C2LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   I2C3LPEN      LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   CECLPEN       LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   DAC12LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART7LPEN     LL_C2_APB1_GRP1_DisableClockSleep\n
  *         APB1LLPENR   UART8LPEN     LL_C2_APB1_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_CEC
  *         @arg @ref LL_APB1_GRP1_PERIPH_DAC12
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB1LLPENR, Periphs);
}

/**
  * @brief  Enable C2 APB1 peripherals clock.
  * @rmtoll APB1HENR     CRSEN         LL_C2_APB1_GRP2_EnableClock\n
  *         APB1HENR     SWPMIEN       LL_C2_APB1_GRP2_EnableClock\n
  *         APB1HENR     OPAMPEN       LL_C2_APB1_GRP2_EnableClock\n
  *         APB1HENR     MDIOSEN       LL_C2_APB1_GRP2_EnableClock\n
  *         APB1HENR     FDCANEN       LL_C2_APB1_GRP2_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP2_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB1HENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB1HENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 APB1 peripheral clock is enabled or not
  * @rmtoll APB1HENR     CRSEN         LL_C2_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     SWPMIEN       LL_C2_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     OPAMPEN       LL_C2_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     MDIOSEN       LL_C2_APB1_GRP2_IsEnabledClock\n
  *         APB1HENR     FDCANEN       LL_C2_APB1_GRP2_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_APB1_GRP2_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->APB1HENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 APB1 peripherals clock.
  * @rmtoll APB1HENR     CRSEN         LL_C2_APB1_GRP2_DisableClock\n
  *         APB1HENR     SWPMIEN       LL_C2_APB1_GRP2_DisableClock\n
  *         APB1HENR     OPAMPEN       LL_C2_APB1_GRP2_DisableClock\n
  *         APB1HENR     MDIOSEN       LL_C2_APB1_GRP2_DisableClock\n
  *         APB1HENR     FDCANEN       LL_C2_APB1_GRP2_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP2_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB1HENR, Periphs);
}

/**
  * @brief  Enable C2 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1HLPENR   CRSLPEN       LL_C2_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   SWPMILPEN     LL_C2_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   OPAMPLPEN     LL_C2_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   MDIOSLPEN     LL_C2_APB1_GRP2_EnableClockSleep\n
  *         APB1HLPENR   FDCANLPEN     LL_C2_APB1_GRP2_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP2_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB1HLPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB1HLPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 APB1 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB1HLPENR   CRSLPEN       LL_C2_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   SWPMILPEN     LL_C2_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   OPAMPLPEN     LL_C2_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   MDIOSLPEN     LL_C2_APB1_GRP2_DisableClockSleep\n
  *         APB1HLPENR   FDCANLPEN     LL_C2_APB1_GRP2_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_CRS
  *         @arg @ref LL_APB1_GRP2_PERIPH_SWPMI1
  *         @arg @ref LL_APB1_GRP2_PERIPH_OPAMP
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB1_GRP2_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB1HLPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB2 APB2
  * @{
  */

/**
  * @brief  Enable C2 APB2 peripherals clock.
  * @rmtoll APB2ENR      TIM1EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM8EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      USART1EN      LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      USART6EN      LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI1EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI4EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM15EN       LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM16EN       LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      TIM17EN       LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      SPI5EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI1EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI2EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      SAI3EN        LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      DFSDM1EN      LL_C2_APB2_GRP1_EnableClock\n
  *         APB2ENR      HRTIMEN       LL_C2_APB2_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 APB2 peripheral clock is enabled or not
  * @rmtoll APB2ENR      TIM1EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM8EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART1EN      LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART6EN      LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI1EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI4EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM15EN       LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM16EN       LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM17EN       LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI5EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI1EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI2EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI3EN        LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      DFSDM1EN      LL_C2_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      HRTIMEN       LL_C2_APB2_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_APB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->APB2ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 APB2 peripherals clock.
  * @rmtoll APB2ENR      TIM1EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM8EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      USART1EN      LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      USART6EN      LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI1EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI4EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM15EN       LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM16EN       LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      TIM17EN       LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      SPI5EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI1EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI2EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      SAI3EN        LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      DFSDM1EN      LL_C2_APB2_GRP1_DisableClock\n
  *         APB2ENR      HRTIMEN       LL_C2_APB2_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB2ENR, Periphs);
}

/**
  * @brief  Enable C2 APB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB2LPENR    TIM1LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM8LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    USART1LPEN    LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    USART6LPEN    LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI1LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI4LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM15LPEN     LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM16LPEN     LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    TIM17LPEN     LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SPI5LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI1LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI2LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    SAI3LPEN      LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    DFSDM1LPEN    LL_C2_APB2_GRP1_EnableClockSleep\n
  *         APB2LPENR    HRTIMLPEN     LL_C2_APB2_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB2_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB2LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB2LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 APB2 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB2LPENR    TIM1LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM8LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    USART1LPEN    LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    USART6LPEN    LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI1LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI4LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM15LPEN     LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM16LPEN     LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    TIM17LPEN     LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SPI5LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI1LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI2LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    SAI3LPEN      LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    DFSDM1LPEN    LL_C2_APB2_GRP1_DisableClockSleep\n
  *         APB2LPENR    HRTIMLPEN     LL_C2_APB2_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI3
  *         @arg @ref LL_APB2_GRP1_PERIPH_DFSDM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_HRTIM
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB2_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB2LPENR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB4 APB4
  * @{
  */

/**
  * @brief  Enable C2 APB4 peripherals clock.
  * @rmtoll APB4ENR      SYSCFGEN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPUART1EN     LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      SPI6EN        LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      I2C4EN        LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM2EN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM3EN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM4EN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      LPTIM5EN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      COMP12EN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      VREFEN        LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      RTCAPBEN      LL_C2_APB4_GRP1_EnableClock\n
  *         APB4ENR      SAI4EN        LL_C2_APB4_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB4ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB4ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if C2 APB4 peripheral clock is enabled or not
  * @rmtoll APB4ENR      SYSCFGEN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPUART1EN     LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      SPI6EN        LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      I2C4EN        LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM2EN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM3EN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM4EN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      LPTIM5EN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      COMP12EN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      VREFEN        LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      RTCAPBEN      LL_C2_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR      SAI4EN        LL_C2_APB4_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4
  * @retval uint32_t
*/
__STATIC_INLINE uint32_t LL_C2_APB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC_C2->APB4ENR, Periphs) == Periphs)?1U:0U);
}

/**
  * @brief  Disable C2 APB4 peripherals clock.
  * @rmtoll APB4ENR      SYSCFGEN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPUART1EN     LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      SPI6EN        LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      I2C4EN        LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM2EN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM3EN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM4EN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      LPTIM5EN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      COMP12EN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      VREFEN        LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      RTCAPBEN      LL_C2_APB4_GRP1_DisableClock\n
  *         APB4ENR      SAI4EN        LL_C2_APB4_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB4_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB4ENR, Periphs);
}

/**
  * @brief  Enable C2 APB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB4LPENR    SYSCFGLPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPUART1LPEN   LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    SPI6LPEN      LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    I2C4LPEN      LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM2LPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM3LPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM4LPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    LPTIM5LPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    COMP12LPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    VREFLPEN      LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    RTCAPBLPEN    LL_C2_APB4_GRP1_EnableClockSleep\n
  *         APB4LPENR    SAI4LPEN      LL_C2_APB4_GRP1_EnableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB4_GRP1_EnableClockSleep(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC_C2->APB4LPENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC_C2->APB4LPENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable C2 APB4 peripherals clock during Low Power (Sleep) mode.
  * @rmtoll APB4LPENR    SYSCFGLPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPUART1LPEN   LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    SPI6LPEN      LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    I2C4LPEN      LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM2LPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM3LPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM4LPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    LPTIM5LPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    COMP12LPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    VREFLPEN      LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    RTCAPBLPEN    LL_C2_APB4_GRP1_DisableClockSleep\n
  *         APB4LPENR    SAI4LPEN      LL_C2_APB4_GRP1_DisableClockSleep
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_COMP12
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREF
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SAI4
  * @retval None
*/
__STATIC_INLINE void LL_C2_APB4_GRP1_DisableClockSleep(uint32_t Periphs)
{
  CLEAR_BIT(RCC_C2->APB4LPENR, Periphs);
}

/**
  * @}
  */

#endif /*DUAL_CORE*/

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(RCC) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_LL_BUS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
