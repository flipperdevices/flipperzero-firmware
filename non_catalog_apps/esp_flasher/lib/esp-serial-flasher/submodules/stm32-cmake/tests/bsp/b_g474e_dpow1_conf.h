/**
  ******************************************************************************
  * @file    b_g474e_dpow1_conf_template.h
  * @author  MCD Application Team
  * @brief   B-G474E-DPOW1 board configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#ifndef B_G474E_DPOW1_CONF_H
#define B_G474E_DPOW1_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B-G474E-DPOW1
  * @{
  */

/** @addtogroup B-G474E-DPOW1_CONFIG
  * @{
  */

/** @addtogroup B-G474E-DPOW1_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* JOYstick define */
#define USE_BSP_JOY_FEATURE                 1U

/* COM define */
#define USE_BSP_COM_FEATURE                 0U

/* COM LOG define */
#define USE_COM_LOG                         0U

/* USBPD BSP PWR TRACE define */
#define USE_BSP_PWR_TRACE                 0U

#if (USE_BSP_PWR_TRACE > 0u)
#define USBPD_PWR_TRACE(_PORT_,...)  UTIL_ADV_TRACE_FSend(__VA_ARGS__)
#else
#define USBPD_PWR_TRACE(_PORT_,...)
#endif /* USE_BSP_PWR_TRACE */

/* IRQ priorities */
#define BSP_JOY_SEL_IT_PRIORITY            15U

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

#endif /* B_G474E_DPOW1_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
