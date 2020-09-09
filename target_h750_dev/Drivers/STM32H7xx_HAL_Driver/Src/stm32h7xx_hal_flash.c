/**
  ******************************************************************************
  * @file    stm32h7xx_hal_flash.c
  * @author  MCD Application Team
  * @brief   FLASH HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the internal FLASH memory:
  *           + Program operations functions
  *           + Memory Control functions
  *           + Peripheral Errors functions
  *
 @verbatim
  ==============================================================================
                        ##### FLASH peripheral features #####
  ==============================================================================

  [..] The Flash memory interface manages CPU AXI I-Code and D-Code accesses
       to the Flash memory. It implements the erase and program Flash memory operations
       and the read and write protection mechanisms.

  [..] The FLASH main features are:
      (+) Flash memory read operations
      (+) Flash memory program/erase operations
      (+) Read / write protections
      (+) Option bytes programming
      (+) Error code correction (ECC) : Data in flash are 266-bits word
          (10 bits added per flash word)

                        ##### How to use this driver #####
 ==============================================================================
    [..]
      This driver provides functions and macros to configure and program the FLASH
      memory of all STM32H7xx devices.

      (#) FLASH Memory IO Programming functions:
           (++) Lock and Unlock the FLASH interface using HAL_FLASH_Unlock() and
                HAL_FLASH_Lock() functions
           (++) Program functions: 256-bit word only
           (++) There Two modes of programming :
            (+++) Polling mode using HAL_FLASH_Program() function
            (+++) Interrupt mode using HAL_FLASH_Program_IT() function

      (#) Interrupts and flags management functions :
           (++) Handle FLASH interrupts by calling HAL_FLASH_IRQHandler()
           (++) Callback functions are called when the flash operations are finished :
                HAL_FLASH_EndOfOperationCallback() when everything is ok, otherwise
                HAL_FLASH_OperationErrorCallback()
           (++) Get error flag status by calling HAL_FLASH_GetError()

      (#) Option bytes management functions :
           (++) Lock and Unlock the option bytes using HAL_FLASH_OB_Unlock() and
                HAL_FLASH_OB_Lock() functions
           (++) Launch the reload of the option bytes using HAL_FLASH_OB_Launch() function.
                In this case, a reset is generated
    [..]
      In addition to these functions, this driver includes a set of macros allowing
      to handle the following operations:
       (+) Set the latency
       (+) Enable/Disable the FLASH interrupts
       (+) Monitor the FLASH flags status
     [..]
    (@) For any Flash memory program operation (erase or program), the CPU clock frequency
        (HCLK) must be at least 1MHz.
    (@) The contents of the Flash memory are not guaranteed if a device reset occurs during
        a Flash memory operation.
    (@) The application can simultaneously request a read and a write operation through each AXI
        interface.
        As the Flash memory is divided into two independent banks, the embedded Flash
        memory interface can drive different operations at the same time on each bank. For
        example a read, write or erase operation can be executed on bank 1 while another read,
        write or erase operation is executed on bank 2.

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
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup FLASH FLASH
  * @brief FLASH HAL module driver
  * @{
  */

#ifdef HAL_FLASH_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup FLASH_Private_Constants
  * @{
  */
#define FLASH_TIMEOUT_VALUE              50000U /* 50 s */

/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FLASH_ProcessTypeDef pFlash;
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

/** @defgroup FLASH_Exported_Functions FLASH Exported functions
  * @{
  */

/** @defgroup FLASH_Exported_Functions_Group1 Programming operation functions
 *  @brief   Programming operation functions
 *
@verbatim
 ===============================================================================
                  ##### Programming operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the FLASH
    program operations.

@endverbatim
  * @{
  */

/**
  * @brief  Program flash word at a specified address
  * @param  TypeProgram Indicate the way to program at a specified address.
  *         This parameter can be a value of @ref FLASH_Type_Program
  * @param  FlashAddress specifies the address to be programmed.
  * @param  DataAddress specifies the address of data to be programmed
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress)
{
  HAL_StatusTypeDef status;
  __IO uint32_t *dest_addr = (__IO uint32_t *)FlashAddress;
  __IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;
  uint32_t bank;
  uint8_t row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));
  assert_param(IS_FLASH_PROGRAM_ADDRESS(FlashAddress));

  /* Process Locked */
  __HAL_LOCK(&pFlash);

#if defined (FLASH_OPTCR_PG_OTP)
  if((IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress)) || (IS_FLASH_PROGRAM_ADDRESS_OTP(FlashAddress)))
#else
  if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
#endif /* FLASH_OPTCR_PG_OTP */
  {
    bank = FLASH_BANK_1;
  }
  else
  {
    bank = FLASH_BANK_2;
  }

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

  if(status == HAL_OK)
  {
    if(bank == FLASH_BANK_1)
    {
#if defined (FLASH_OPTCR_PG_OTP)
      if (TypeProgram == FLASH_TYPEPROGRAM_OTPWORD)
      {
        /* Set OTP_PG bit */
        SET_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OTP);
      }
      else
#endif /* FLASH_OPTCR_PG_OTP */
      {
        /* Set PG bit */
        SET_BIT(FLASH->CR1, FLASH_CR_PG);
      }
    }
    else
    {
      /* Set PG bit */
      SET_BIT(FLASH->CR2, FLASH_CR_PG);
    }

    __ISB();
    __DSB();

#if defined (FLASH_OPTCR_PG_OTP)
    if (TypeProgram == FLASH_TYPEPROGRAM_OTPWORD)
    {
      /* Program an OTP word (16 bits) */
      *(__IO uint16_t *)FlashAddress = *(__IO uint16_t*)DataAddress;
    }
    else
#endif /* FLASH_OPTCR_PG_OTP */
    {
      /* Program the flash word */
      do
      {
        *dest_addr = *src_addr;
        dest_addr++;
        src_addr++;
        row_index--;
     } while (row_index != 0U);
    }

    __ISB();
    __DSB();

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

#if defined (FLASH_OPTCR_PG_OTP)
    if (TypeProgram == FLASH_TYPEPROGRAM_OTPWORD)
    {
      /* If the program operation is completed, disable the OTP_PG */
      CLEAR_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OTP);
    }
    else
#endif /* FLASH_OPTCR_PG_OTP */
    {
      if(bank == FLASH_BANK_1)
      {
        /* If the program operation is completed, disable the PG */
        CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);
      }
      else
      {
        /* If the program operation is completed, disable the PG */
        CLEAR_BIT(FLASH->CR2, FLASH_CR_PG);
      }
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief  Program flash words of 256 bits at a specified address with interrupt enabled.
  * @param  TypeProgram Indicate the way to program at a specified address.
  *                      This parameter can be a value of @ref FLASH_Type_Program
  * @param  FlashAddress specifies the address to be programmed.
  * @param  DataAddress specifies the address of data (256 bits) to be programmed
  *
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress)
{
  HAL_StatusTypeDef status;
  __IO uint32_t *dest_addr = (__IO uint32_t*)FlashAddress;
  __IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;
  uint32_t bank;
  uint8_t row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));
  assert_param(IS_FLASH_PROGRAM_ADDRESS(FlashAddress));

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

#if defined (FLASH_OPTCR_PG_OTP)
  if((IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress)) || (IS_FLASH_PROGRAM_ADDRESS_OTP(FlashAddress)))
#else
  if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
#endif /* FLASH_OPTCR_PG_OTP */
  {
    bank = FLASH_BANK_1;
  }
  else
  {
    bank = FLASH_BANK_2;
  }

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

  if (status != HAL_OK)
  {
    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);
  }
  else
  {
    pFlash.Address = FlashAddress;

    if(bank == FLASH_BANK_1)
    {
      /* Set internal variables used by the IRQ handler */
      pFlash.ProcedureOnGoing = FLASH_PROC_PROGRAM_BANK1;

#if defined (FLASH_OPTCR_PG_OTP)
      if (TypeProgram == FLASH_TYPEPROGRAM_OTPWORD)
      {
        /* Set OTP_PG bit */
        SET_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OTP);
      }
      else
#endif /* FLASH_OPTCR_PG_OTP */
      {
        /* Set PG bit */
        SET_BIT(FLASH->CR1, FLASH_CR_PG);
      }

      /* Enable End of Operation and Error interrupts for Bank 1 */
#if defined (FLASH_CR_OPERRIE)
      __HAL_FLASH_ENABLE_IT_BANK1(FLASH_IT_EOP_BANK1     | FLASH_IT_WRPERR_BANK1 | FLASH_IT_PGSERR_BANK1 | \
                                  FLASH_IT_STRBERR_BANK1 | FLASH_IT_INCERR_BANK1 | FLASH_IT_OPERR_BANK1);
#else
      __HAL_FLASH_ENABLE_IT_BANK1(FLASH_IT_EOP_BANK1     | FLASH_IT_WRPERR_BANK1 | FLASH_IT_PGSERR_BANK1 | \
                                  FLASH_IT_STRBERR_BANK1 | FLASH_IT_INCERR_BANK1);
#endif /* FLASH_CR_OPERRIE */
    }
    else
    {
      /* Set internal variables used by the IRQ handler */
      pFlash.ProcedureOnGoing = FLASH_PROC_PROGRAM_BANK2;

      /* Set PG bit */
      SET_BIT(FLASH->CR2, FLASH_CR_PG);

      /* Enable End of Operation and Error interrupts for Bank2 */
#if defined (FLASH_CR_OPERRIE)
      __HAL_FLASH_ENABLE_IT_BANK2(FLASH_IT_EOP_BANK2     | FLASH_IT_WRPERR_BANK2 | FLASH_IT_PGSERR_BANK2 | \
                                  FLASH_IT_STRBERR_BANK2 | FLASH_IT_INCERR_BANK2 | FLASH_IT_OPERR_BANK2);
#else
      __HAL_FLASH_ENABLE_IT_BANK2(FLASH_IT_EOP_BANK2     | FLASH_IT_WRPERR_BANK2 | FLASH_IT_PGSERR_BANK2 | \
                                  FLASH_IT_STRBERR_BANK2 | FLASH_IT_INCERR_BANK2);
#endif /* FLASH_CR_OPERRIE */
    }

    __ISB();
    __DSB();

#if defined (FLASH_OPTCR_PG_OTP)
    if (TypeProgram == FLASH_TYPEPROGRAM_OTPWORD)
    {
      /* Program an OTP word (16 bits) */
      *(__IO uint16_t *)FlashAddress = *(__IO uint16_t*)DataAddress;
    }
    else
#endif /* FLASH_OPTCR_PG_OTP */
    {
      /* Program the flash word */
      do
      {
        *dest_addr = *src_addr;
        dest_addr++;
        src_addr++;
        row_index--;
      } while (row_index != 0U);
    }

    __ISB();
    __DSB();
  }

  return status;
}

/**
  * @brief This function handles FLASH interrupt request.
  * @retval None
  */
void HAL_FLASH_IRQHandler(void)
{
  uint32_t temp;
  uint32_t errorflag;
  FLASH_ProcedureTypeDef procedure;

  /* Check FLASH Bank1 End of Operation flag  */
  if(__HAL_FLASH_GET_FLAG_BANK1(FLASH_SR_EOP) != RESET)
  {
    if(pFlash.ProcedureOnGoing == FLASH_PROC_SECTERASE_BANK1)
    {
      /* Nb of sector to erased can be decreased */
      pFlash.NbSectorsToErase--;

      /* Check if there are still sectors to erase */
      if(pFlash.NbSectorsToErase != 0U)
      {
        /* Indicate user which sector has been erased */
        HAL_FLASH_EndOfOperationCallback(pFlash.Sector);

        /* Clear bank 1 End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_EOP_BANK1);

        /* Increment sector number */
        pFlash.Sector++;
        temp = pFlash.Sector;
        FLASH_Erase_Sector(temp, FLASH_BANK_1, pFlash.VoltageForErase);
      }
      else
      {
        /* No more sectors to Erase, user callback can be called */
        /* Reset Sector and stop Erase sectors procedure */
        pFlash.Sector = 0xFFFFFFFFU;
        pFlash.ProcedureOnGoing = FLASH_PROC_NONE;

        /* FLASH EOP interrupt user callback */
        HAL_FLASH_EndOfOperationCallback(pFlash.Sector);

        /* Clear FLASH End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_EOP_BANK1);
      }
    }
    else
    {
      procedure = pFlash.ProcedureOnGoing;

      if((procedure == FLASH_PROC_MASSERASE_BANK1) || (procedure == FLASH_PROC_ALLBANK_MASSERASE))
      {
        /* MassErase ended. Return the selected bank */
        /* FLASH EOP interrupt user callback */
        HAL_FLASH_EndOfOperationCallback(FLASH_BANK_1);
      }
      else if(procedure == FLASH_PROC_PROGRAM_BANK1)
      {
        /* Program ended. Return the selected address */
        /* FLASH EOP interrupt user callback */
        HAL_FLASH_EndOfOperationCallback(pFlash.Address);
      }
      else
      {
        /* Nothing to do */
      }

      if((procedure != FLASH_PROC_SECTERASE_BANK2) && \
         (procedure != FLASH_PROC_MASSERASE_BANK2) && \
         (procedure != FLASH_PROC_PROGRAM_BANK2))
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_NONE;
        /* Clear FLASH End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_EOP_BANK1);
      }
    }
  }

  /* Check FLASH Bank2 End of Operation flag  */
  if(__HAL_FLASH_GET_FLAG_BANK2(FLASH_SR_EOP) != RESET)
  {
    if(pFlash.ProcedureOnGoing == FLASH_PROC_SECTERASE_BANK2)
    {
      /*Nb of sector to erased can be decreased*/
      pFlash.NbSectorsToErase--;

      /* Check if there are still sectors to erase*/
      if(pFlash.NbSectorsToErase != 0U)
      {
        /*Indicate user which sector has been erased*/
        HAL_FLASH_EndOfOperationCallback(pFlash.Sector);

        /* Clear bank 2 End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_EOP_BANK2);

        /*Increment sector number*/
        pFlash.Sector++;
        temp = pFlash.Sector;
        FLASH_Erase_Sector(temp, FLASH_BANK_2, pFlash.VoltageForErase);
      }
      else
      {
        /* No more sectors to Erase, user callback can be called */
        /* Reset Sector and stop Erase sectors procedure */
        pFlash.Sector = 0xFFFFFFFFU;
        pFlash.ProcedureOnGoing = FLASH_PROC_NONE;

        /* FLASH EOP interrupt user callback */
        HAL_FLASH_EndOfOperationCallback(pFlash.Sector);

        /* Clear FLASH End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_EOP_BANK2);
      }
    }
    else
    {
      procedure = pFlash.ProcedureOnGoing;

      if((procedure == FLASH_PROC_MASSERASE_BANK2) || (procedure == FLASH_PROC_ALLBANK_MASSERASE))
      {
        /*MassErase ended. Return the selected bank*/
        /* FLASH EOP interrupt user callback */
        HAL_FLASH_EndOfOperationCallback(FLASH_BANK_2);
      }
      else if(procedure == FLASH_PROC_PROGRAM_BANK2)
      {
        /* Program ended. Return the selected address */
        /* FLASH EOP interrupt user callback */
        HAL_FLASH_EndOfOperationCallback(pFlash.Address);
      }
      else
      {
        /* Nothing to do */
      }

      if((procedure != FLASH_PROC_SECTERASE_BANK1) && \
         (procedure != FLASH_PROC_MASSERASE_BANK1) && \
         (procedure != FLASH_PROC_PROGRAM_BANK1))
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_NONE;
        /* Clear FLASH End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_EOP_BANK2);
      }
    }
  }

  /* Check FLASH Bank1 operation error flags */
#if defined (FLASH_SR_OPERR)
  errorflag = FLASH->SR1 & (FLASH_FLAG_WRPERR_BANK1 | FLASH_FLAG_PGSERR_BANK1 | FLASH_FLAG_STRBERR_BANK1 | \
                            FLASH_FLAG_INCERR_BANK1 | FLASH_FLAG_OPERR_BANK1);
#else
  errorflag = FLASH->SR1 & (FLASH_FLAG_WRPERR_BANK1 | FLASH_FLAG_PGSERR_BANK1 | FLASH_FLAG_STRBERR_BANK1 | \
                            FLASH_FLAG_INCERR_BANK1);
#endif /* FLASH_SR_OPERR */

  if(errorflag != 0U)
  {
    /* Save the error code */
    pFlash.ErrorCode |= errorflag;

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG_BANK1(errorflag);

    procedure = pFlash.ProcedureOnGoing;

    if(procedure == FLASH_PROC_SECTERASE_BANK1)
    {
      /* Return the faulty sector */
      temp = pFlash.Sector;
      pFlash.Sector = 0xFFFFFFFFU;
    }
    else if((procedure == FLASH_PROC_MASSERASE_BANK1) || (procedure == FLASH_PROC_ALLBANK_MASSERASE))
    {
      /* Return the faulty bank */
      temp = FLASH_BANK_1;
    }
    else
    {
      /* Return the faulty address */
      temp = pFlash.Address;
    }

    /* Stop the procedure ongoing*/
    pFlash.ProcedureOnGoing = FLASH_PROC_NONE;

    /* FLASH error interrupt user callback */
    HAL_FLASH_OperationErrorCallback(temp);
  }

  /* Check FLASH Bank2 operation error flags */
#if defined (FLASH_SR_OPERR)
  errorflag = FLASH->SR2 & ((FLASH_FLAG_WRPERR_BANK2 | FLASH_FLAG_PGSERR_BANK2 | FLASH_FLAG_STRBERR_BANK2 | \
                             FLASH_FLAG_INCERR_BANK2 | FLASH_FLAG_OPERR_BANK2) & 0x7FFFFFFFU);
#else
  errorflag = FLASH->SR2 & ((FLASH_FLAG_WRPERR_BANK2 | FLASH_FLAG_PGSERR_BANK2 | FLASH_FLAG_STRBERR_BANK2 | \
                             FLASH_FLAG_INCERR_BANK2) & 0x7FFFFFFFU);
#endif /* FLASH_SR_OPERR */

  if(errorflag != 0U)
  {
    /* Save the error code */
    pFlash.ErrorCode |= (errorflag | 0x80000000U);

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG_BANK2(errorflag);

    procedure = pFlash.ProcedureOnGoing;

    if(procedure== FLASH_PROC_SECTERASE_BANK2)
    {
      /*return the faulty sector*/
      temp = pFlash.Sector;
      pFlash.Sector = 0xFFFFFFFFU;
    }
    else if((procedure == FLASH_PROC_MASSERASE_BANK2) || (procedure == FLASH_PROC_ALLBANK_MASSERASE))
    {
      /*return the faulty bank*/
      temp = FLASH_BANK_2;
    }
    else
    {
      /*return the faulty address*/
      temp = pFlash.Address;
    }

    /*Stop the procedure ongoing*/
    pFlash.ProcedureOnGoing = FLASH_PROC_NONE;

    /* FLASH error interrupt user callback */
    HAL_FLASH_OperationErrorCallback(temp);
  }

  if(pFlash.ProcedureOnGoing == FLASH_PROC_NONE)
  {
#if defined (FLASH_CR_OPERRIE)
    /* Disable Bank1 Operation and Error source interrupt */
    __HAL_FLASH_DISABLE_IT_BANK1(FLASH_IT_EOP_BANK1    | FLASH_IT_WRPERR_BANK1 | FLASH_IT_PGSERR_BANK1 | \
                                 FLASH_IT_STRBERR_BANK1 | FLASH_IT_INCERR_BANK1 | FLASH_IT_OPERR_BANK1);

    /* Disable Bank2 Operation and Error source interrupt */
    __HAL_FLASH_DISABLE_IT_BANK2(FLASH_IT_EOP_BANK2    | FLASH_IT_WRPERR_BANK2 | FLASH_IT_PGSERR_BANK2 | \
                                 FLASH_IT_STRBERR_BANK2 | FLASH_IT_INCERR_BANK2 | FLASH_IT_OPERR_BANK2);
#else
    /* Disable Bank1 Operation and Error source interrupt */
    __HAL_FLASH_DISABLE_IT_BANK1(FLASH_IT_EOP_BANK1    | FLASH_IT_WRPERR_BANK1 | FLASH_IT_PGSERR_BANK1 | \
                                 FLASH_IT_STRBERR_BANK1 | FLASH_IT_INCERR_BANK1);

    /* Disable Bank2 Operation and Error source interrupt */
    __HAL_FLASH_DISABLE_IT_BANK2(FLASH_IT_EOP_BANK2    | FLASH_IT_WRPERR_BANK2 | FLASH_IT_PGSERR_BANK2 | \
                                 FLASH_IT_STRBERR_BANK2 | FLASH_IT_INCERR_BANK2);
#endif /* FLASH_CR_OPERRIE */

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);
  }
}

/**
  * @brief  FLASH end of operation interrupt callback
  * @param  ReturnValue The value saved in this parameter depends on the ongoing procedure
  *                  Mass Erase: Bank number which has been requested to erase
  *                  Sectors Erase: Sector which has been erased
  *                    (if 0xFFFFFFFF, it means that all the selected sectors have been erased)
  *                  Program: Address which was selected for data program
  * @retval None
  */
__weak void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ReturnValue);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_EndOfOperationCallback could be implemented in the user file
   */
}

/**
  * @brief  FLASH operation error interrupt callback
  * @param  ReturnValue The value saved in this parameter depends on the ongoing procedure
  *                 Mass Erase: Bank number which has been requested to erase
  *                 Sectors Erase: Sector number which returned an error
  *                 Program: Address which was selected for data program
  * @retval None
  */
__weak void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ReturnValue);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup FLASH_Exported_Functions_Group2 Peripheral Control functions
 *  @brief   Management functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the FLASH
    memory operations.

@endverbatim
  * @{
  */

/**
  * @brief  Unlock the FLASH control registers access
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Unlock(void)
{
  if(READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
  {
    /* Authorize the FLASH Bank1 Registers access */
    WRITE_REG(FLASH->KEYR1, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR1, FLASH_KEY2);

    /* Verify Flash Bank1 is unlocked */
    if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
    {
      return HAL_ERROR;
    }
  }

  if(READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
  {
    /* Authorize the FLASH Bank2 Registers access */
    WRITE_REG(FLASH->KEYR2, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR2, FLASH_KEY2);

    /* Verify Flash Bank2 is unlocked */
    if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Locks the FLASH control registers access
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH Bank1 Control Register access */
  SET_BIT(FLASH->CR1, FLASH_CR_LOCK);

  /* Verify Flash Bank1 is locked */
  if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) == 0U)
  {
    return HAL_ERROR;
  }

  /* Set the LOCK Bit to lock the FLASH Bank2 Control Register access */
  SET_BIT(FLASH->CR2, FLASH_CR_LOCK);

  /* Verify Flash Bank2 is locked */
  if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) == 0U)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Unlock the FLASH Option Control Registers access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void)
{
  if(READ_BIT(FLASH->OPTCR, FLASH_OPTCR_OPTLOCK) != 0U)
  {
    /* Authorizes the Option Byte registers programming */
    WRITE_REG(FLASH->OPTKEYR, FLASH_OPT_KEY1);
    WRITE_REG(FLASH->OPTKEYR, FLASH_OPT_KEY2);

    /* Verify that the Option Bytes are unlocked */
    if (READ_BIT(FLASH->OPTCR, FLASH_OPTCR_OPTLOCK) != 0U)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Lock the FLASH Option Control Registers access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void)
{
  /* Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access */
  SET_BIT(FLASH->OPTCR, FLASH_OPTCR_OPTLOCK);

  /* Verify that the Option Bytes are locked */
  if (READ_BIT(FLASH->OPTCR, FLASH_OPTCR_OPTLOCK) == 0U)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Launch the option bytes loading.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void)
{
  HAL_StatusTypeDef status;

  /* Wait for CRC computation to be completed */
  if (FLASH_CRC_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else if (FLASH_CRC_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    status = HAL_OK;
  }

  if (status == HAL_OK)
  {
    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->OPTCR, FLASH_OPTCR_OPTSTART);

    /* Wait for OB change operation to be completed */
    status = FLASH_OB_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
  }

  return status;
}

/**
  * @}
  */

/** @defgroup FLASH_Exported_Functions_Group3 Peripheral State and Errors functions
 *  @brief   Peripheral Errors functions
 *
@verbatim
 ===============================================================================
                ##### Peripheral Errors functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time Errors of the FLASH peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Get the specific FLASH error flag.
  * @retval HAL_FLASH_ERRORCode The returned value can be:
  *            @arg HAL_FLASH_ERROR_NONE       : No error set
  *
  *            @arg HAL_FLASH_ERROR_WRP_BANK1  : Write Protection Error on Bank 1
  *            @arg HAL_FLASH_ERROR_PGS_BANK1  : Program Sequence Error on Bank 1
  *            @arg HAL_FLASH_ERROR_STRB_BANK1 : Strobe Error on Bank 1
  *            @arg HAL_FLASH_ERROR_INC_BANK1  : Inconsistency Error on Bank 1
  *            @arg HAL_FLASH_ERROR_OPE_BANK1  : Operation Error on Bank 1
  *            @arg HAL_FLASH_ERROR_RDP_BANK1  : Read Protection Error on Bank 1
  *            @arg HAL_FLASH_ERROR_RDS_BANK1  : Read Secured Error on Bank 1
  *            @arg HAL_FLASH_ERROR_SNECC_BANK1: ECC Single Correction Error on Bank 1
  *            @arg HAL_FLASH_ERROR_DBECC_BANK1: ECC Double Detection Error on Bank 1
  *            @arg HAL_FLASH_ERROR_CRCRD_BANK1: CRC Read Error on Bank 1
  *
  *            @arg HAL_FLASH_ERROR_WRP_BANK2  : Write Protection Error on Bank 2
  *            @arg HAL_FLASH_ERROR_PGS_BANK2  : Program Sequence Error on Bank 2
  *            @arg HAL_FLASH_ERROR_STRB_BANK2 : Strobe Error on Bank 2
  *            @arg HAL_FLASH_ERROR_INC_BANK2  : Inconsistency Error on Bank 2
  *            @arg HAL_FLASH_ERROR_OPE_BANK2  : Operation Error on Bank 2
  *            @arg HAL_FLASH_ERROR_RDP_BANK2  : Read Protection Error on Bank 2
  *            @arg HAL_FLASH_ERROR_RDS_BANK2  : Read Secured Error on Bank 2
  *            @arg HAL_FLASH_ERROR_SNECC_BANK2: SNECC Error on Bank 2
  *            @arg HAL_FLASH_ERROR_DBECC_BANK2: Double Detection ECC on Bank 2
  *            @arg HAL_FLASH_ERROR_CRCRD_BANK2: CRC Read Error on Bank 2
*/

uint32_t HAL_FLASH_GetError(void)
{
   return pFlash.ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @addtogroup FLASH_Private_Functions
  * @{
  */

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout
  * @param  Bank flash FLASH_BANK_1 or FLASH_BANK_2
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout, uint32_t Bank)
{
  /* Wait for the FLASH operation to complete by polling on QW flag to be reset.
     Even if the FLASH operation fails, the QW flag will be reset and an error
     flag will be set */

  uint32_t bsyflag, errorflag;
  uint32_t tickstart = HAL_GetTick();

  assert_param(IS_FLASH_BANK_EXCLUSIVE(Bank));

  /* Select bsyflag depending on Bank */
  if(Bank == FLASH_BANK_1)
  {
    bsyflag = FLASH_FLAG_QW_BANK1;
  }
  else
  {
    bsyflag = FLASH_FLAG_QW_BANK2;
  }

  while(__HAL_FLASH_GET_FLAG(bsyflag))
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        return HAL_TIMEOUT;
      }
    }
  }

  /* Get Error Flags */
  if (Bank == FLASH_BANK_1)
  {
    errorflag = FLASH->SR1 & FLASH_FLAG_ALL_ERRORS_BANK1;
  }
  else
  {
    errorflag = (FLASH->SR2 & FLASH_FLAG_ALL_ERRORS_BANK2) | 0x80000000U;
  }

  /* In case of error reported in Flash SR1 or SR2 registers (ECCC not managed as an error) */
  if((errorflag & 0x7DFFFFFFU) != 0U)
  {
    /*Save the error code*/
    pFlash.ErrorCode |= errorflag;

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(errorflag);

    return HAL_ERROR;
  }

  /* Check FLASH End of Operation flag  */
  if(Bank == FLASH_BANK_1)
  {
    if (__HAL_FLASH_GET_FLAG_BANK1(FLASH_FLAG_EOP_BANK1))
    {
      /* Clear FLASH End of Operation pending bit */
      __HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_EOP_BANK1);
    }
  }
  else
  {
    if (__HAL_FLASH_GET_FLAG_BANK2(FLASH_FLAG_EOP_BANK2))
    {
      /* Clear FLASH End of Operation pending bit */
      __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_EOP_BANK2);
    }
  }

  return HAL_OK;
}

/**
  * @brief  Wait for a FLASH Option Bytes change operation to complete.
  * @param  Timeout maximum flash operation timeout
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_OB_WaitForLastOperation(uint32_t Timeout)
{
  /* Get timeout */
  uint32_t tickstart = HAL_GetTick();

  /* Wait for the FLASH Option Bytes change operation to complete by polling on OPT_BUSY flag to be reset */
  while(READ_BIT(FLASH->OPTSR_CUR, FLASH_OPTSR_OPT_BUSY) != 0U)
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        return HAL_TIMEOUT;
      }
    }
  }

  /* Check option byte change error */
  if(READ_BIT(FLASH->OPTSR_CUR, FLASH_OPTSR_OPTCHANGEERR) != 0U)
  {
    /* Save the error code */
    pFlash.ErrorCode |= HAL_FLASH_ERROR_OB_CHANGE;

    /* Clear the OB error flag */
    FLASH->OPTCCR |= FLASH_OPTCCR_CLR_OPTCHANGEERR;

    return HAL_ERROR;
  }

  /* If there is no error flag set */
  return HAL_OK;
}

/**
  * @brief  Wait for a FLASH CRC computation to complete.
  * @param  Timeout maximum flash operation timeout
  * @param  Bank flash FLASH_BANK_1 or FLASH_BANK_2
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_CRC_WaitForLastOperation(uint32_t Timeout, uint32_t Bank)
{
  uint32_t bsyflag;
  uint32_t tickstart = HAL_GetTick();

  assert_param(IS_FLASH_BANK_EXCLUSIVE(Bank));

  /* Select bsyflag depending on Bank */
  if(Bank == FLASH_BANK_1)
  {
    bsyflag = FLASH_FLAG_CRC_BUSY_BANK1;
  }
  else
  {
    bsyflag = FLASH_FLAG_CRC_BUSY_BANK2;
  }

  /* Wait for the FLASH CRC computation to complete by polling on CRC_BUSY flag to be reset */
  while(__HAL_FLASH_GET_FLAG(bsyflag))
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        return HAL_TIMEOUT;
      }
    }
  }

  /* Check FLASH CRC read error flag  */
  if(Bank == FLASH_BANK_1)
  {
    if (__HAL_FLASH_GET_FLAG_BANK1(FLASH_FLAG_CRCRDERR_BANK1))
    {
      /* Save the error code */
      pFlash.ErrorCode |= HAL_FLASH_ERROR_CRCRD_BANK1;

      /* Clear FLASH CRC read error pending bit */
      __HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_CRCRDERR_BANK1);

      return HAL_ERROR;
    }
  }
  else
  {
    if (__HAL_FLASH_GET_FLAG_BANK2(FLASH_FLAG_CRCRDERR_BANK2))
    {
      /* Save the error code */
      pFlash.ErrorCode |= HAL_FLASH_ERROR_CRCRD_BANK2;

      /* Clear FLASH CRC read error pending bit */
      __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_CRCRDERR_BANK2);

      return HAL_ERROR;
    }
  }

  /* If there is no error flag set */
  return HAL_OK;
}

/**
  * @}
  */

#endif /* HAL_FLASH_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
