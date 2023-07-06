/** 
  ******************************************************************************
  * @file    stm32wbxx_nucleo.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for:
  *          - LEDs and push-button available on STM32WBXX-Nucleo Kit 
  *            from STMicroelectronics
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32WBXX_NUCLEO_H
#define __STM32WBXX_NUCLEO_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32WBXX_NUCLEO
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"
   
/** @defgroup STM32WBXX_NUCLEO_Exported_Types Exported Types
  * @{
  */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  /* Color led aliases */
  LED_BLUE   = LED1,
  LED_GREEN  = LED2,
  LED_RED    = LED3
}Led_TypeDef;

typedef enum 
{  
  BUTTON_SW1 = 0,
  BUTTON_SW2 = 1,
  BUTTON_SW3 = 2,
}Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

typedef enum 
{ 
  JOY_NONE  = 0,
  JOY_SEL   = 1,
  JOY_DOWN  = 2,
  JOY_LEFT  = 3,
  JOY_RIGHT = 4,
  JOY_UP    = 5
} JOYState_TypeDef;

/**
  * @}
  */ 

/** @defgroup STM32WBXX_NUCLEO_Exported_Constants Exported Constants
  * @{
  */ 

/** 
  * @brief Define for STM32WBXX_NUCLEO board  
  */ 
#if !defined (USE_STM32WBXX_NUCLEO)
 #define USE_STM32WBXX_NUCLEO
#endif

/** @defgroup STM32WBXX_NUCLEO_LED LED Constants
  * @{
  */
#define LEDn                                    3

#define LED1_PIN                                GPIO_PIN_5
#define LED1_GPIO_PORT                          GPIOB
#define LED1_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED2_PIN                                GPIO_PIN_0
#define LED2_GPIO_PORT                          GPIOB
#define LED2_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED3_PIN                                GPIO_PIN_1
#define LED3_GPIO_PORT                          GPIOB
#define LED3_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)         __HAL_RCC_GPIOB_CLK_ENABLE() /* All Led on same port */
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)        __HAL_RCC_GPIOB_CLK_DISABLE() /* All Led on same port */
/**
  * @}
  */ 
  
/** @defgroup STM32WBXX_NUCLEO_BUTTON BUTTON Constants
  * @{
  */  
#define BUTTONn                                 3

/**
 * @brief Key push-button
 */
#define BUTTON_SW1_PIN                          GPIO_PIN_4
#define BUTTON_SW1_GPIO_PORT                    GPIOC
#define BUTTON_SW1_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON_SW1_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define BUTTON_SW1_EXTI_LINE                    GPIO_PIN_4
#ifdef CORE_CM0PLUS
#define BUTTON_SW1_EXTI_IRQn                    EXTI15_4_IRQn
#else
#define BUTTON_SW1_EXTI_IRQn                    EXTI4_IRQn
#endif

#define BUTTON_SW2_PIN                          GPIO_PIN_0
#define BUTTON_SW2_GPIO_PORT                    GPIOD
#define BUTTON_SW2_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUTTON_SW2_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOD_CLK_DISABLE()
#define BUTTON_SW2_EXTI_LINE                    GPIO_PIN_0
#ifdef CORE_CM0PLUS
#define BUTTON_SW2_EXTI_IRQn                    EXTI1_0_IRQn
#else
#define BUTTON_SW2_EXTI_IRQn                    EXTI0_IRQn
#endif

#define BUTTON_SW3_PIN                          GPIO_PIN_1
#define BUTTON_SW3_GPIO_PORT                    GPIOD
#define BUTTON_SW3_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUTTON_SW3_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOD_CLK_DISABLE()
#define BUTTON_SW3_EXTI_LINE                    GPIO_PIN_1
#ifdef CORE_CM0PLUS
#define BUTTON_SW3_EXTI_IRQn                    EXTI1_0_IRQn
#else
#define BUTTON_SW3_EXTI_IRQn                    EXTI1_IRQn
#endif

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)    do { if ((__INDEX__) == BUTTON_SW1) BUTTON_SW1_GPIO_CLK_ENABLE(); else \
                                              if ((__INDEX__) == BUTTON_SW2) BUTTON_SW2_GPIO_CLK_ENABLE(); else \
                                              if ((__INDEX__) == BUTTON_SW3) BUTTON_SW3_GPIO_CLK_ENABLE();} while(0)

#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)    do { if ((__INDEX__) == BUTTON_SW1) BUTTON_SW1_GPIO_CLK_DISABLE(); else \
                                              if ((__INDEX__) == BUTTON_SW2) BUTTON_SW2_GPIO_CLK_DISABLE(); else \
                                              if ((__INDEX__) == BUTTON_SW3) BUTTON_SW3_GPIO_CLK_DISABLE();} while(0)

/**
  * @}
  */

/** @addtogroup STM32WBXX_NUCLEO_BUS BUS Constants
  * @{
  */
/*###################### SPI1 ###################################*/
#define NUCLEO_SPIx                                 SPI1
#define NUCLEO_SPIx_CLK_ENABLE()                    __HAL_RCC_SPI1_CLK_ENABLE()

#define NUCLEO_SPIx_SCK_AF                          GPIO_AF5_SPI1
#define NUCLEO_SPIx_SCK_GPIO_PORT                   GPIOA
#define NUCLEO_SPIx_SCK_PIN                         GPIO_PIN_5
#define NUCLEO_SPIx_SCK_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPIx_SCK_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

#define NUCLEO_SPIx_MISO_MOSI_AF                    GPIO_AF5_SPI1
#define NUCLEO_SPIx_MISO_MOSI_GPIO_PORT             GPIOA
#define NUCLEO_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define NUCLEO_SPIx_MISO_PIN                        GPIO_PIN_6
#define NUCLEO_SPIx_MOSI_PIN                        GPIO_PIN_7
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define NUCLEO_SPIx_TIMEOUT_MAX                   1000


/**
  * @brief  SD Control Lines management
  */  
#define SD_CS_LOW()       HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()      HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)
    
/**
  * @brief  LCD Control Lines management
  */
#define LCD_CS_LOW()      HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET)
#define LCD_CS_HIGH()     HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET)
#define LCD_DC_LOW()      HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_PIN, GPIO_PIN_RESET)
#define LCD_DC_HIGH()     HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_PIN, GPIO_PIN_SET)

/**
  * @brief  SD Control Interface pins
  */
#define SD_CS_PIN                                 GPIO_PIN_10
#define SD_CS_GPIO_PORT                           GPIOC
#define SD_CS_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOC_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()                  __HAL_RCC_GPIOC_CLK_DISABLE()

/**
  * @brief  LCD Control Interface pins
  */
#define LCD_CS_PIN                                 GPIO_PIN_4
#define LCD_CS_GPIO_PORT                           GPIOA
#define LCD_CS_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOA_CLK_ENABLE()
#define LCD_CS_GPIO_CLK_DISABLE()                  __HAL_RCC_GPIOA_CLK_DISABLE()

/**
  * @brief  LCD Data/Command Interface pins
  */
#define LCD_DC_PIN                                 GPIO_PIN_12
#define LCD_DC_GPIO_PORT                           GPIOC
#define LCD_DC_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOC_CLK_ENABLE()
#define LCD_DC_GPIO_CLK_DISABLE()                  __HAL_RCC_GPIOC_CLK_DISABLE()
     
/*##################### ADC1 ###################################*/
/**
  * @brief  ADC Interface pins
  *         used to detect motion of Joystick available on Adafruit 1.8" TFT shield
  */
#define NUCLEO_ADCx                                 ADC1
#define NUCLEO_ADCx_CLK_ENABLE()                    __HAL_RCC_ADC_CLK_ENABLE()
    
#define NUCLEO_ADCx_GPIO_PORT                       GPIOA
#define NUCLEO_ADCx_GPIO_PIN                        GPIO_PIN_0
#define NUCLEO_ADCx_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_ADCx_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOA_CLK_DISABLE()

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32WBXX_NUCLEO_Exported_Functions
  * @{
  */
uint32_t         BSP_GetVersion(void);  

/** @addtogroup STM32WBXX_NUCLEO_LED_Functions
  * @{
  */
void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
void             BSP_LED_Toggle(Led_TypeDef Led);
/**
  * @}
  */

/** @addtogroup STM32WBXX_NUCLEO_BUTTON_Functions
  * @{
  */
void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);
#ifdef HAL_ADC_MODULE_ENABLED
uint8_t          BSP_JOY_Init(void);
JOYState_TypeDef BSP_JOY_GetState(void);
#endif /* HAL_ADC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32WBXX_NUCLEO_H */
