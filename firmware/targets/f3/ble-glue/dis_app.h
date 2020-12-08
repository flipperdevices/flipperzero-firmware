/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dis_app.h
  * @author  MCD Application Team
  * @brief   Header for dis_application.c module
  ******************************************************************************
  * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIS_APP_H
#define __DIS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
#define DISAPP_MANUFACTURER_NAME              "STM"
#define DISAPP_MODEL_NUMBER                   "4502-1.0"
#define DISAPP_SERIAL_NUMBER                  "1.0"
#define DISAPP_HARDWARE_REVISION_NUMBER       "1.0"
#define DISAPP_FIRMWARE_REVISION_NUMBER       "1.0"
#define DISAPP_SOFTWARE_REVISION_NUMBER       "1.0"
#define DISAPP_OUI                            0x123456
#define DISAPP_MANUFACTURER_ID                0x9ABCDE
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void DISAPP_Init(void);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*__DIS_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
