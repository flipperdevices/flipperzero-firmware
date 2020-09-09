/**
  ******************************************************************************
  * @file    stm32h7xx_hal_opamp_ex.c
  * @author  MCD Application Team
  * @brief   Extended OPAMP HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the operational amplifier(s)(OPAMP1, OPAMP2 etc)
  *          peripheral:
  *           + Extended Initialization and de-initialization functions
  *           + Extended Peripheral Control functions
  *         
  @verbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup OPAMPEx OPAMPEx
  * @brief OPAMP Extended HAL module driver
  * @{
  */

#ifdef HAL_OPAMP_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup OPAMPEx_Exported_Functions OPAMP Extended Exported Functions
  * @{
  */

/** @defgroup OPAMPEx_Exported_Functions_Group1 Extended Input and Output operation functions
  * @brief    Extended operation functions
  *
@verbatim
 ===============================================================================
              ##### Extended IO operation functions #####
 ===============================================================================
  [..]
      (+) OPAMP Self calibration. 

@endverbatim
  * @{
  */

/**
  * @brief  Run the self calibration of 2 OPAMPs in parallel.
  * @note   Trimming values (PMOS & NMOS) are updated and user trimming is 
  *         enabled is calibration is successful.
  * @note   Calibration is performed in the mode specified in OPAMP init
  *         structure (mode normal or low power). To perform calibration for
  *         both modes, repeat this function twice after OPAMP init structure
  *         accordingly updated.
  * @param  hopamp1 handle
  * @param  hopamp2 handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_OPAMPEx_SelfCalibrateAll(OPAMP_HandleTypeDef *hopamp1, OPAMP_HandleTypeDef *hopamp2)
{
  HAL_StatusTypeDef status = HAL_OK;

  uint32_t trimmingvaluen1;
  uint32_t trimmingvaluep1;
  uint32_t trimmingvaluen2;
  uint32_t trimmingvaluep2;

/* Selection of register of trimming depending on power mode: OTR or HSOTR */
  __IO uint32_t* tmp_opamp1_reg_trimming;   
  __IO uint32_t* tmp_opamp2_reg_trimming;

  uint32_t delta;
  uint32_t opampmode1;
  uint32_t opampmode2;
  
  if((hopamp1 == NULL) || (hopamp2 == NULL)) 
  {
    status = HAL_ERROR;
  }
  /* Check if OPAMP in calibration mode and calibration not yet enable */
  else if(hopamp1->State !=  HAL_OPAMP_STATE_READY)
  {
    status = HAL_ERROR;
  }
  else if(hopamp2->State != HAL_OPAMP_STATE_READY)
  {
    status = HAL_ERROR;
  }
  else
  {
      /* Check the parameter */
      assert_param(IS_OPAMP_ALL_INSTANCE(hopamp1->Instance));
      assert_param(IS_OPAMP_ALL_INSTANCE(hopamp2->Instance));
      
      assert_param(IS_OPAMP_POWERMODE(hopamp1->Init.PowerMode));
      assert_param(IS_OPAMP_POWERMODE(hopamp2->Init.PowerMode));

      /* Set Calibration mode */
      /* Non-inverting input connected to calibration reference voltage. */
      SET_BIT(hopamp1->Instance->CSR, OPAMP_CSR_FORCEVP);
      SET_BIT(hopamp2->Instance->CSR, OPAMP_CSR_FORCEVP);

      /* Save OPAMP mode  */
      opampmode1 = READ_BIT(hopamp1->Instance->CSR,OPAMP_CSR_VMSEL);
      opampmode2 = READ_BIT(hopamp2->Instance->CSR,OPAMP_CSR_VMSEL);

      /* Use of standalone mode */ 
      MODIFY_REG(hopamp1->Instance->CSR, OPAMP_CSR_VMSEL, OPAMP_STANDALONE_MODE); 
      MODIFY_REG(hopamp2->Instance->CSR, OPAMP_CSR_VMSEL, OPAMP_STANDALONE_MODE); 

      /*  user trimming values are used for offset calibration */
      SET_BIT(hopamp1->Instance->CSR, OPAMP_CSR_USERTRIM);
      SET_BIT(hopamp2->Instance->CSR, OPAMP_CSR_USERTRIM);
      
      /* Select trimming settings depending on power mode */
      if (hopamp1->Init.PowerMode == OPAMP_POWERMODE_NORMAL)
      {
        tmp_opamp1_reg_trimming = &OPAMP1->OTR;
      }
      else
      {
        tmp_opamp1_reg_trimming = &OPAMP1->HSOTR;
      }
      
      if (hopamp2->Init.PowerMode == OPAMP_POWERMODE_NORMAL)
      {
        tmp_opamp2_reg_trimming = &OPAMP2->OTR;
      }
      else
      {
        tmp_opamp2_reg_trimming = &OPAMP2->HSOTR;
      }
      
      /* Enable calibration */
      SET_BIT (hopamp1->Instance->CSR, OPAMP_CSR_CALON);
      SET_BIT (hopamp2->Instance->CSR, OPAMP_CSR_CALON);
  
      /* 1st calibration - N */
      /* Select 90U% VREF */
      MODIFY_REG(hopamp1->Instance->CSR, OPAMP_CSR_CALSEL, OPAMP_VREF_90VDDA);
      MODIFY_REG(hopamp2->Instance->CSR, OPAMP_CSR_CALSEL, OPAMP_VREF_90VDDA);

      /* Enable the selected opamp */
      SET_BIT (hopamp1->Instance->CSR, OPAMP_CSR_OPAMPxEN);
      SET_BIT (hopamp2->Instance->CSR, OPAMP_CSR_OPAMPxEN);
      
      /* Init trimming counter */    
      /* Medium value */
      trimmingvaluen1 = 16U; 
      trimmingvaluen2 = 16U; 
      delta = 8U; 

      while (delta != 0U)
      {
        /* Set candidate trimming */
        /* OPAMP_POWERMODE_NORMAL */
        MODIFY_REG(*tmp_opamp1_reg_trimming, OPAMP_OTR_TRIMOFFSETN, trimmingvaluen1);
        MODIFY_REG(*tmp_opamp2_reg_trimming, OPAMP_OTR_TRIMOFFSETN, trimmingvaluen2);

        /* OFFTRIMmax delay 2 ms as per datasheet (electrical characteristics */ 
        /* Offset trim time: during calibration, minimum time needed between */
        /* two steps to have 1 mV accuracy */
        HAL_Delay(OPAMP_TRIMMING_DELAY);

        if (READ_BIT(hopamp1->Instance->CSR, OPAMP_CSR_CALOUT)!= 0U)
        { 
          /* OPAMP_CSR_CALOUT is Low try higher trimming */
          trimmingvaluen1 += delta;
        }
        else
        {
          /* OPAMP_CSR_CALOUT is High try lower trimming */
          trimmingvaluen1 -= delta;
        }

        if (READ_BIT(hopamp2->Instance->CSR, OPAMP_CSR_CALOUT)!= 0U) 
        { 
          /* OPAMP_CSR_CALOUT is Low try higher trimming */
          trimmingvaluen2 += delta;
        }
        else
        {
          /* OPAMP_CSR_CALOUT is High try lower trimming */
          trimmingvaluen2 -= delta;
        }
        /* Divide range by 2 to continue dichotomy sweep */
        delta >>= 1U;
      }

      /* Still need to check if right calibration is current value or one step below */
      /* Indeed the first value that causes the OUTCAL bit to change from 0 to 1  */
      /* Set candidate trimming */
      MODIFY_REG(*tmp_opamp1_reg_trimming, OPAMP_OTR_TRIMOFFSETN, trimmingvaluen1);
      MODIFY_REG(*tmp_opamp2_reg_trimming, OPAMP_OTR_TRIMOFFSETN, trimmingvaluen2);
      
      /* OFFTRIMmax delay 2 ms as per datasheet (electrical characteristics */ 
      /* Offset trim time: during calibration, minimum time needed between */
      /* two steps to have 1 mV accuracy */
      HAL_Delay(OPAMP_TRIMMING_DELAY);
      
      if ((READ_BIT(hopamp1->Instance->CSR, OPAMP_CSR_CALOUT)) != 0U)
      {
        /* Trimming value is actually one value more */
        trimmingvaluen1++;
        MODIFY_REG(*tmp_opamp1_reg_trimming, OPAMP_OTR_TRIMOFFSETN, trimmingvaluen1);
      }
       
      if ((READ_BIT(hopamp2->Instance->CSR, OPAMP_CSR_CALOUT)) != 0U)
      {
        /* Trimming value is actually one value more */
        trimmingvaluen2++;
        MODIFY_REG(*tmp_opamp2_reg_trimming, OPAMP_OTR_TRIMOFFSETN, trimmingvaluen2);
      }
      
      /* 2nd calibration - P */
      /* Select 10U% VREF */
      MODIFY_REG(hopamp1->Instance->CSR, OPAMP_CSR_CALSEL, OPAMP_VREF_10VDDA);
      MODIFY_REG(hopamp2->Instance->CSR, OPAMP_CSR_CALSEL, OPAMP_VREF_10VDDA);
     
      /* Init trimming counter */    
      /* Medium value */
      trimmingvaluep1 = 16U; 
      trimmingvaluep2 = 16U; 
      delta = 8U; 
      
      while (delta != 0U)
      {
        /* Set candidate trimming */
        /* OPAMP_POWERMODE_NORMAL */
        MODIFY_REG(*tmp_opamp1_reg_trimming, OPAMP_OTR_TRIMOFFSETP, (trimmingvaluep1<<OPAMP_INPUT_NONINVERTING));
        MODIFY_REG(*tmp_opamp2_reg_trimming, OPAMP_OTR_TRIMOFFSETP, (trimmingvaluep2<<OPAMP_INPUT_NONINVERTING));

        /* OFFTRIMmax delay 2 ms as per datasheet (electrical characteristics */ 
        /* Offset trim time: during calibration, minimum time needed between */
        /* two steps to have 1 mV accuracy */
        HAL_Delay(OPAMP_TRIMMING_DELAY);

        if (READ_BIT(hopamp1->Instance->CSR, OPAMP_CSR_CALOUT)!= 0U) 
        { 
          /* OPAMP_CSR_CALOUT is Low try higher trimming */
          trimmingvaluep1 += delta;
        }
        else
        {
          /* OPAMP_CSR_CALOUT is HIGH try lower trimming */
          trimmingvaluep1 -= delta;
        }

        if (READ_BIT(hopamp2->Instance->CSR, OPAMP_CSR_CALOUT)!= 0U) 
        { 
          /* OPAMP_CSR_CALOUT is Low try higher trimming */
          trimmingvaluep2 += delta;
        }
        else
        {
          /* OPAMP_CSR_CALOUT is High try lower trimming */
          trimmingvaluep2 -= delta;
        }
        /* Divide range by 2 to continue dichotomy sweep */
        delta >>= 1U;
      }
      
      /* Still need to check if right calibration is current value or one step below */
      /* Indeed the first value that causes the OUTCAL bit to change from 1 to 0  */
      /* Set candidate trimming */
      MODIFY_REG(*tmp_opamp1_reg_trimming, OPAMP_OTR_TRIMOFFSETP, (trimmingvaluep1<<OPAMP_INPUT_NONINVERTING));
      MODIFY_REG(*tmp_opamp2_reg_trimming, OPAMP_OTR_TRIMOFFSETP, (trimmingvaluep2<<OPAMP_INPUT_NONINVERTING));
      
      /* OFFTRIMmax delay 2 ms as per datasheet (electrical characteristics */ 
      /* Offset trim time: during calibration, minimum time needed between */
      /* two steps to have 1 mV accuracy */
      HAL_Delay(OPAMP_TRIMMING_DELAY);
      
      if (READ_BIT(hopamp1->Instance->CSR, OPAMP_CSR_CALOUT)!= 0U)
      {
        /* Trimming value is actually one value more */
        trimmingvaluep1++;
        MODIFY_REG(*tmp_opamp1_reg_trimming, OPAMP_OTR_TRIMOFFSETP, (trimmingvaluep1<<OPAMP_INPUT_NONINVERTING));
      }
      
      if (READ_BIT(hopamp2->Instance->CSR, OPAMP_CSR_CALOUT)!= 0U)
      {
        /* Trimming value is actually one value more */
        trimmingvaluep2++;
        MODIFY_REG(*tmp_opamp2_reg_trimming, OPAMP_OTR_TRIMOFFSETP, (trimmingvaluep2<<OPAMP_INPUT_NONINVERTING));
      }
            
      /* Disable calibration & set normal mode (operating mode) */
      CLEAR_BIT (hopamp1->Instance->CSR, OPAMP_CSR_CALON);
      CLEAR_BIT (hopamp2->Instance->CSR, OPAMP_CSR_CALON);

     /* Disable the OPAMPs */
      CLEAR_BIT (hopamp1->Instance->CSR, OPAMP_CSR_OPAMPxEN);
      CLEAR_BIT (hopamp2->Instance->CSR, OPAMP_CSR_OPAMPxEN);

      /* Self calibration is successful */
      /* Store calibration (user trimming) results in init structure. */
      
      /* Set user trimming mode */ 
      hopamp1->Init.UserTrimming = OPAMP_TRIMMING_USER;
      hopamp2->Init.UserTrimming = OPAMP_TRIMMING_USER;

      /* Affect calibration parameters depending on mode normal/high speed */
      if (hopamp1->Init.PowerMode != OPAMP_POWERMODE_HIGHSPEED)
      {
        /* Write calibration result N */
        hopamp1->Init.TrimmingValueN = trimmingvaluen1;
        /* Write calibration result P */
        hopamp1->Init.TrimmingValueP = trimmingvaluep1;
      }
      else
      {
        /* Write calibration result N */
        hopamp1->Init.TrimmingValueNHighSpeed = trimmingvaluen1;
        /* Write calibration result P */
        hopamp1->Init.TrimmingValuePHighSpeed = trimmingvaluep1;
      }
      
      if (hopamp2->Init.PowerMode != OPAMP_POWERMODE_HIGHSPEED)
      {
        /* Write calibration result N */
        hopamp2->Init.TrimmingValueN = trimmingvaluen2;
        /* Write calibration result P */
        hopamp2->Init.TrimmingValueP = trimmingvaluep2;
      }
      else
      {
        /* Write calibration result N */
        hopamp2->Init.TrimmingValueNHighSpeed = trimmingvaluen2;
        /* Write calibration result P */
        hopamp2->Init.TrimmingValuePHighSpeed = trimmingvaluep2;
     
       }
      /* Update OPAMP state */
      hopamp1->State = HAL_OPAMP_STATE_READY;
      hopamp2->State = HAL_OPAMP_STATE_READY;

      /* Restore OPAMP mode after calibration */
      MODIFY_REG(hopamp1->Instance->CSR, OPAMP_CSR_VMSEL, opampmode1);
      MODIFY_REG(hopamp2->Instance->CSR, OPAMP_CSR_VMSEL, opampmode2);
  }
  
  return status;
}

/**
  * @}
  */

/** @defgroup OPAMPEx_Exported_Functions_Group2 Peripheral Control functions 
 *  @brief   Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
             ##### Peripheral Control functions #####
 ===============================================================================
    [..]
      (+) OPAMP unlock. 

@endverbatim
  * @{
  */

/**
  * @brief  Unlock the selected OPAMP configuration.
  * @note   This function must be called only when OPAMP is in state "locked".
  * @param  hopamp: OPAMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_OPAMPEx_Unlock(OPAMP_HandleTypeDef* hopamp)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the OPAMP handle allocation */
  /* Check if OPAMP locked */
  if(hopamp == NULL)
  {
    status = HAL_ERROR;
  }    
  /* Check the OPAMP handle allocation */
  /* Check if OPAMP locked */
  else if(hopamp->State == HAL_OPAMP_STATE_BUSYLOCKED)
  {
    /* Check the parameter */
    assert_param(IS_OPAMP_ALL_INSTANCE(hopamp->Instance));
  
   /* OPAMP state changed to locked */
    hopamp->State = HAL_OPAMP_STATE_BUSY;
  }
  else
  {
    status = HAL_ERROR;
  }
      
  return status; 
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

/**
  * @}
  */

#endif /* HAL_OPAMP_MODULE_ENABLED */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
