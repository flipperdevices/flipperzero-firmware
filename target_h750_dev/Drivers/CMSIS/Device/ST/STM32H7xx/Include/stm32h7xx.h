/**
  ******************************************************************************
  * @file    stm32h7xx.h
  * @author  MCD Application Team
  * @brief   CMSIS STM32H7xx Device Peripheral Access Layer Header File.
  *
  *          The file is the unique include file that the application programmer
  *          is using in the C source code, usually in main.c. This file contains:
  *           - Configuration section that allows to select:
  *              - The STM32H7xx device used in the target application
  *              - To use or not the peripheral’s drivers in application code(i.e.
  *                code will be based on direct access to peripheral’s registers
  *                rather than drivers API), this option is controlled by
  *                "#define USE_HAL_DRIVER"
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32h7xx
  * @{
  */

#ifndef STM32H7xx_H
#define STM32H7xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/** @addtogroup Library_configuration_section
  * @{
  */

/**
  * @brief STM32 Family
  */
#if !defined  (STM32H7)
#define STM32H7
#endif /* STM32H7 */


/* Uncomment the line below according to the target STM32H7 device used in your
   application
  */

#if !defined (STM32H743xx) && !defined (STM32H753xx)  && !defined (STM32H750xx) && !defined (STM32H742xx) && \
    !defined (STM32H745xx) && !defined (STM32H755xx)  && !defined (STM32H747xx) && !defined (STM32H757xx) && \
    !defined (STM32H7A3xx) && !defined (STM32H7A3xxQ) && !defined (STM32H7B3xx) && !defined (STM32H7B3xxQ) && !defined (STM32H7B0xx)  && !defined (STM32H7B0xxQ)
  /* #define STM32H742xx */   /*!< STM32H742VI, STM32H742ZI, STM32H742AI, STM32H742II, STM32H742BI, STM32H742XI Devices */
  /* #define STM32H743xx */   /*!< STM32H743VI, STM32H743ZI, STM32H743AI, STM32H743II, STM32H743BI, STM32H743XI Devices */
  /* #define STM32H753xx */   /*!< STM32H753VI, STM32H753ZI, STM32H753AI, STM32H753II, STM32H753BI, STM32H753XI Devices */
  /* #define STM32H750xx */   /*!< STM32H750V, STM32H750I, STM32H750X Devices */
  /* #define STM32H747xx */   /*!< STM32H747ZI, STM32H747AI, STM32H747II, STM32H747BI, STM32H747XI Devices */
  /* #define STM32H757xx */   /*!< STM32H757ZI, STM32H757AI, STM32H757II, STM32H757BI, STM32H757XI Devices */
  /* #define STM32H745xx */   /*!< STM32H745ZI, STM32H745II, STM32H745BI, STM32H745XI Devices  */
  /* #define STM32H755xx */   /*!< STM32H755ZI, STM32H755II, STM32H755BI, STM32H755XI Devices  */
  /* #define STM32H7B0xx */   /*!< STM32H7B0ABIxQ, STM32H7B0IBTx, STM32H7B0RBTx, STM32H7B0VBTx, STM32H7B0ZBTx, STM32H7B0IBKxQ */
  /* #define STM32H7A3xx */   /*!< STM32H7A3IIK6, STM32H7A3IIT6, STM32H7A3NIH6, STM32H7A3RIT6, STM32H7A3VIH6, STM32H7A3VIT6, STM32H7A3ZIT6 */
  /* #define STM32H7A3xxQ */  /*!< STM32H7A3QIY6Q, STM32H7A3IIK6Q, STM32H7A3IIT6Q, STM32H7A3LIH6Q, STM32H7A3VIH6Q, STM32H7A3VIT6Q, STM32H7A3AII6Q, STM32H7A3ZIT6Q */
  /* #define STM32H7B3xx */   /*!< STM32H7B3IIK6, STM32H7B3IIT6, STM32H7B3NIH6, STM32H7B3RIT6, STM32H7B3VIH6, STM32H7B3VIT6, STM32H7B3ZIT6 */
  /* #define STM32H7B3xxQ */  /*!< STM32H7B3QIY6Q, STM32H7B3IIK6Q, STM32H7B3IIT6Q, STM32H7B3LIH6Q, STM32H7B3VIH6Q, STM32H7B3VIT6Q, STM32H7B3AII6Q, STM32H7B3ZIT6Q */
#endif

/*  Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor.
  */

#if defined(DUAL_CORE) && !defined(CORE_CM4) && !defined(CORE_CM7)
 #error "Dual core device, please select CORE_CM4 or CORE_CM7"
#endif

#if !defined  (USE_HAL_DRIVER)
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will
   be based on direct access to peripherals registers
   */
  /*#define USE_HAL_DRIVER */
#endif /* USE_HAL_DRIVER */

/**
  * @brief CMSIS Device version number V1.8.0
  */
#define __STM32H7xx_CMSIS_DEVICE_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32H7xx_CMSIS_DEVICE_VERSION_SUB1   (0x08) /*!< [23:16] sub1 version */
#define __STM32H7xx_CMSIS_DEVICE_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32H7xx_CMSIS_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32H7xx_CMSIS_DEVICE_VERSION        ((__CMSIS_DEVICE_VERSION_MAIN     << 24)\
                                      |(__CMSIS_DEVICE_HAL_VERSION_SUB1 << 16)\
                                      |(__CMSIS_DEVICE_HAL_VERSION_SUB2 << 8 )\
                                      |(__CMSIS_DEVICE_HAL_VERSION_RC))

/**
  * @}
  */

/** @addtogroup Device_Included
  * @{
  */

#if defined(STM32H743xx)
  #include "stm32h743xx.h"
#elif defined(STM32H753xx)
  #include "stm32h753xx.h"
#elif defined(STM32H750xx)
  #include "stm32h750xx.h"
#elif defined(STM32H742xx)
  #include "stm32h742xx.h"
#elif defined(STM32H745xx)
  #include "stm32h745xx.h"
#elif defined(STM32H755xx)
  #include "stm32h755xx.h"
#elif defined(STM32H747xx)
  #include "stm32h747xx.h"
#elif defined(STM32H757xx)
  #include "stm32h757xx.h"
#elif defined(STM32H7B0xx)
  #include "stm32h7b0xx.h"
#elif defined(STM32H7B0xxQ)
  #include "stm32h7b0xxq.h"
#elif defined(STM32H7A3xx)
  #include "stm32h7a3xx.h"
#elif defined(STM32H7B3xx)
  #include "stm32h7b3xx.h"
#elif defined(STM32H7A3xxQ)
  #include "stm32h7a3xxq.h"
#elif defined(STM32H7B3xxQ)
  #include "stm32h7b3xxq.h"
#else
 #error "Please select first the target STM32H7xx device used in your application (in stm32h7xx.h file)"
#endif

/**
  * @}
  */

/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  SUCCESS = 0,
  ERROR = !SUCCESS
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))


/**
  * @}
  */

#if defined (USE_HAL_DRIVER)
 #include "stm32h7xx_hal.h"
#endif /* USE_HAL_DRIVER */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STM32H7xx_H */
/**
  * @}
  */

/**
  * @}
  */




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
