/**
  ******************************************************************************
  * @file    Display/LCD_Paint/Src/ts_calibration.c 
  * @author  MCD Application Team
  * @brief   This application code shows how to calibrate the touchscreen.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_HAL_Applications
  * @{
  */

/** @addtogroup LCD_Paint
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TS_StateTypeDef  TS_State;
static uint8_t Calibration_Done = 0;
static int16_t  A1, A2, B1, B2;
static int16_t aPhysX[2], aPhysY[2], aLogX[2], aLogY[2];
/* Private function prototypes -----------------------------------------------*/
static void TouchscreenCalibration_SetHint(void);
static void GetPhysValues(int16_t LogX, int16_t LogY, int16_t * pPhysX, int16_t * pPhysY);
static void WaitForPressedState(uint8_t Pressed);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Performs the TS calibration
  * @param  None
  * @retval None
  */
void Touchscreen_Calibration(void)
{ 
  uint8_t status = 0;
  uint8_t i = 0;

  TouchscreenCalibration_SetHint();
  
  status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  
  if (status != TS_OK)
  {
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t*)"ERROR", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 80, (uint8_t*)"Touchscreen cannot be initialized", CENTER_MODE);
  }
  
  while (1)
  {
    if (status == TS_OK)
    {
      aLogX[0] = 15;
      aLogY[0] = 15;
      aLogX[1] = BSP_LCD_GetXSize() - 15;
      aLogY[1] = BSP_LCD_GetYSize() - 15;
      
      for (i = 0; i < 2; i++) 
      {
        GetPhysValues(aLogX[i], aLogY[i], &aPhysX[i], &aPhysY[i]);
      }
      A1 = (1000 * ( aLogX[1] - aLogX[0]))/ ( aPhysX[1] - aPhysX[0]); 
      B1 = (1000 * aLogX[0]) - A1 * aPhysX[0]; 
      
      A2 = (1000 * ( aLogY[1] - aLogY[0]))/ ( aPhysY[1] - aPhysY[0]); 
      B2 = (1000 * aLogY[0]) - A2 * aPhysY[0]; 
      
      Calibration_Done = 1;
      return;
    }
   
    HAL_Delay(5);
  }
}

/**
  * @brief  Display calibration hint
  * @param  None
  * @retval None
  */
static void TouchscreenCalibration_SetHint(void)
{
  /* Clear the LCD */ 
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  
  /* Set Touchscreen Demo description */
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

  BSP_LCD_SetFont(&Font12);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 - 27, (uint8_t*)"Before using the Touchscreen", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 - 12, (uint8_t*)"you need to calibrate it.", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 + 3, (uint8_t*)"Press on the black circles", CENTER_MODE);
}


/**
  * @brief  Get Physical position
  * @param  LogX : logical X position
  * @param  LogY : logical Y position
  * @param  pPhysX : Physical X position
  * @param  pPhysY : Physical Y position
  * @retval None
  */
static void GetPhysValues(int16_t LogX, int16_t LogY, int16_t * pPhysX, int16_t * pPhysY) 
{
  /* Draw the ring */
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_FillCircle(LogX, LogY, 5);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillCircle(LogX, LogY, 2);
  
  /* Wait until touch is pressed */
  WaitForPressedState(1);
  
  BSP_TS_GetState(&TS_State);
  *pPhysX = TS_State.x;
  *pPhysY = TS_State.y; 
  
  /* Wait until touch is released */
  WaitForPressedState(0);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillCircle(LogX, LogY, 5);
}

/**
  * @brief  wait for touch detected
  * @param  Pressed: touch pressed. 
  * @retval None
  */
static void WaitForPressedState(uint8_t Pressed) 
{
  TS_StateTypeDef  State;
  
  do 
  {
    BSP_TS_GetState(&State);
    HAL_Delay(10);
    if (State.TouchDetected == Pressed) 
    {
      uint16_t TimeStart = HAL_GetTick();
      do {
        BSP_TS_GetState(&State);      
        HAL_Delay(10);
        if (State.TouchDetected != Pressed) 
        {
          break;
        } else if ((HAL_GetTick() - 100) > TimeStart) 
        {
          return;
        }
      } while (1);
    }
  } while (1);
}

/**
  * @brief  Calibrate X position
  * @param  x : X position
  * @retval calibrated x
  */
uint16_t Calibration_GetX(uint16_t x)
{
  return (((A1 * x) + B1)/1000);
}

/**
  * @brief  Calibrate Y position
  * @param  y : Y position
  * @retval calibrated y
  */
uint16_t Calibration_GetY(uint16_t y)
{
  return (((A2 * y) + B2)/1000);
}

/**check if the TS is calibrated
  * @param  None
* @retval calibration state (1 : calibrated / 0: no)
  */
uint8_t IsCalibrationDone(void)
{
  return (Calibration_Done);
}

/**
  * @}
  */ 
  
/**
  * @}
  */
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
