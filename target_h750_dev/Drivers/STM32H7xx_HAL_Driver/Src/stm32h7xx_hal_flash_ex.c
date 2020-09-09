/**
  ******************************************************************************
  * @file    stm32h7xx_hal_flash_ex.c
  * @author  MCD Application Team
  * @brief   Extended FLASH HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the FLASH extension peripheral:
  *           + Extended programming operations functions
  *
 @verbatim
 ==============================================================================
                   ##### Flash Extension features #####
  ==============================================================================

  [..] Comparing to other previous devices, the FLASH interface for STM32H7xx
       devices contains the following additional features

       (+) Capacity up to 2 Mbyte with dual bank architecture supporting read-while-write
           capability (RWW)
       (+) Dual bank memory organization
       (+) PCROP protection for all banks
       (+) Global readout protection (RDP)
       (+) Write protection
       (+) Secure access only protection
       (+) Bank / register swapping
       (+) Cyclic Redundancy Check (CRC)

                        ##### How to use this driver #####
 ==============================================================================
  [..] This driver provides functions to configure and program the FLASH memory
       of all STM32H7xx devices. It includes
      (#) FLASH Memory Erase functions:
           (++) Lock and Unlock the FLASH interface using HAL_FLASH_Unlock() and
                HAL_FLASH_Lock() functions
           (++) Erase function: Sector erase, bank erase and dual-bank mass erase
           (++) There are two modes of erase :
             (+++) Polling Mode using HAL_FLASHEx_Erase()
             (+++) Interrupt Mode using HAL_FLASHEx_Erase_IT()

      (#) Option Bytes Programming functions: Use HAL_FLASHEx_OBProgram() to:
        (++) Set/Reset the write protection per bank
        (++) Set the Read protection Level
        (++) Set the BOR level
        (++) Program the user Option Bytes
        (++) PCROP protection configuration and control per bank
        (++) Secure area configuration and control per bank
        (++) Core Boot address configuration

      (#) FLASH Memory Lock and unlock per Bank: HAL_FLASHEx_Lock_Bank1(), HAL_FLASHEx_Unlock_Bank1(),
          HAL_FLASHEx_Lock_Bank2() and HAL_FLASHEx_Unlock_Bank2() functions

      (#) FLASH CRC computation function: Use HAL_FLASHEx_ComputeCRC() to:
          (++) Enable CRC feature
          (++) Program the desired burst size
          (++) Define the user Flash Area on which the CRC has be computed
          (++) Perform the CRC computation
          (++) Disable CRC feature

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

/** @defgroup FLASHEx  FLASHEx
  * @brief FLASH HAL Extension module driver
  * @{
  */

#ifdef HAL_FLASH_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup FLASHEx_Private_Constants
  * @{
  */
#define FLASH_TIMEOUT_VALUE       50000U /* 50 s */

/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup FLASHEx_Private_Functions FLASHEx Private Functions
  * @{
  */
static void FLASH_MassErase(uint32_t VoltageRange, uint32_t Banks);
static void FLASH_OB_EnableWRP(uint32_t WRPSector, uint32_t Banks);
static void FLASH_OB_DisableWRP(uint32_t WRPSector, uint32_t Bank);
static void FLASH_OB_GetWRP(uint32_t *WRPState, uint32_t *WRPSector, uint32_t Bank);
static void FLASH_OB_RDPConfig(uint32_t RDPLevel);
static uint32_t FLASH_OB_GetRDP(void);
static void FLASH_OB_PCROPConfig(uint32_t PCROConfigRDP, uint32_t PCROPStartAddr, uint32_t PCROPEndAddr, uint32_t Banks);
static void FLASH_OB_GetPCROP(uint32_t *PCROPConfig, uint32_t *PCROPStartAddr,uint32_t *PCROPEndAddr, uint32_t Bank);
static void FLASH_OB_BOR_LevelConfig(uint32_t Level);
static uint32_t FLASH_OB_GetBOR(void);
static void FLASH_OB_UserConfig(uint32_t UserType, uint32_t UserConfig);
static uint32_t FLASH_OB_GetUser(void);
static void FLASH_OB_BootAddConfig(uint32_t BootOption, uint32_t BootAddress0, uint32_t BootAddress1);
static void FLASH_OB_GetBootAdd(uint32_t *BootAddress0, uint32_t *BootAddress1);
static void FLASH_OB_SecureAreaConfig(uint32_t SecureAreaConfig, uint32_t SecureAreaStartAddr, uint32_t SecureAreaEndAddr, uint32_t Banks);
static void FLASH_OB_GetSecureArea(uint32_t *SecureAreaConfig, uint32_t *SecureAreaStartAddr, uint32_t *SecureAreaEndAddr, uint32_t Bank);
static void FLASH_CRC_AddSector(uint32_t Sector, uint32_t Bank);
static void FLASH_CRC_SelectAddress(uint32_t CRCStartAddr, uint32_t CRCEndAddr, uint32_t Bank);

#if defined (DUAL_CORE)
static void FLASH_OB_CM4BootAddConfig(uint32_t BootOption, uint32_t BootAddress0, uint32_t BootAddress1);
static void FLASH_OB_GetCM4BootAdd(uint32_t *BootAddress0, uint32_t *BootAddress1);
#endif /* DUAL_CORE */

#if defined (FLASH_OTPBL_LOCKBL)
static void FLASH_OB_OTP_LockConfig(uint32_t OTP_Block);
static uint32_t FLASH_OB_OTP_GetLock(void);
#endif /* FLASH_OTPBL_LOCKBL */
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/
/** @defgroup FLASHEx_Exported_Functions FLASHEx Exported Functions
  * @{
  */

/** @defgroup FLASHEx_Exported_Functions_Group1 Extended IO operation functions
 *  @brief   Extended IO operation functions
 *
@verbatim
 ===============================================================================
                ##### Extended programming operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the Extension FLASH
    programming operations Operations.

@endverbatim
  * @{
  */
/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors
  * @param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  *
  * @param[out]  SectorError pointer to variable  that contains the configuration
  *          information on faulty sector in case of error (0xFFFFFFFF means that all
  *          the sectors have been correctly erased)
  *
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t sector_index;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));
  assert_param(IS_FLASH_BANK(pEraseInit->Banks));

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed on Bank1 */
  if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  /* Wait for last operation to be completed on Bank2 */
  if((pEraseInit->Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  if(status == HAL_OK)
  {
    if(pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /* Mass erase to be done */
      FLASH_MassErase(pEraseInit->VoltageRange, pEraseInit->Banks);

      /* Wait for last operation to be completed */
      if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
      {
        if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1) != HAL_OK)
        {
          status = HAL_ERROR;
        }
        /* if the erase operation is completed, disable the Bank1 BER Bit */
        FLASH->CR1 &= (~FLASH_CR_BER);
      }
      if((pEraseInit->Banks & FLASH_BANK_2) == FLASH_BANK_2)
      {
        if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2) != HAL_OK)
        {
          status = HAL_ERROR;
        }
        /* if the erase operation is completed, disable the Bank2 BER Bit */
        FLASH->CR2 &= (~FLASH_CR_BER);
      }
    }
    else
    {
      /*Initialization of SectorError variable*/
      *SectorError = 0xFFFFFFFFU;

      /* Erase by sector by sector to be done*/
      for(sector_index = pEraseInit->Sector; sector_index < (pEraseInit->NbSectors + pEraseInit->Sector); sector_index++)
      {
        FLASH_Erase_Sector(sector_index, pEraseInit->Banks, pEraseInit->VoltageRange);

        if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
        {
          /* Wait for last operation to be completed */
          status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1);

          /* If the erase operation is completed, disable the SER Bit */
          FLASH->CR1 &= (~(FLASH_CR_SER | FLASH_CR_SNB));
        }
        if((pEraseInit->Banks & FLASH_BANK_2) == FLASH_BANK_2)
        {
          /* Wait for last operation to be completed */
          status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2);

          /* If the erase operation is completed, disable the SER Bit */
          FLASH->CR2 &= (~(FLASH_CR_SER | FLASH_CR_SNB));
        }

        if(status != HAL_OK)
        {
          /* In case of error, stop erase procedure and return the faulty sector */
          *SectorError = sector_index;
          break;
        }
      }
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors with interrupt enabled
  * @param  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  *
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));
  assert_param(IS_FLASH_BANK(pEraseInit->Banks));

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed */
  if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  if((pEraseInit->Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  if (status != HAL_OK)
  {
    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);
  }
  else
  {
    if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
    {
      /* Enable End of Operation and Error interrupts for Bank 1 */
#if defined (FLASH_CR_OPERRIE)
      __HAL_FLASH_ENABLE_IT_BANK1(FLASH_IT_EOP_BANK1     | FLASH_IT_WRPERR_BANK1 | FLASH_IT_PGSERR_BANK1 | \
                                  FLASH_IT_STRBERR_BANK1 | FLASH_IT_INCERR_BANK1 | FLASH_IT_OPERR_BANK1);
#else
      __HAL_FLASH_ENABLE_IT_BANK1(FLASH_IT_EOP_BANK1     | FLASH_IT_WRPERR_BANK1 | FLASH_IT_PGSERR_BANK1 | \
                                  FLASH_IT_STRBERR_BANK1 | FLASH_IT_INCERR_BANK1);
#endif /* FLASH_CR_OPERRIE */
    }
    if((pEraseInit->Banks & FLASH_BANK_2) == FLASH_BANK_2)
    {
      /* Enable End of Operation and Error interrupts for Bank 2 */
#if defined (FLASH_CR_OPERRIE)
      __HAL_FLASH_ENABLE_IT_BANK2(FLASH_IT_EOP_BANK2     | FLASH_IT_WRPERR_BANK2 | FLASH_IT_PGSERR_BANK2 | \
                                  FLASH_IT_STRBERR_BANK2 | FLASH_IT_INCERR_BANK2 | FLASH_IT_OPERR_BANK2);
#else
      __HAL_FLASH_ENABLE_IT_BANK2(FLASH_IT_EOP_BANK2     | FLASH_IT_WRPERR_BANK2 | FLASH_IT_PGSERR_BANK2 | \
                                  FLASH_IT_STRBERR_BANK2 | FLASH_IT_INCERR_BANK2);
#endif /* FLASH_CR_OPERRIE */
    }

    if(pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /*Mass erase to be done*/
      if(pEraseInit->Banks == FLASH_BANK_1)
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_MASSERASE_BANK1;
      }
      else if(pEraseInit->Banks == FLASH_BANK_2)
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_MASSERASE_BANK2;
      }
      else
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_ALLBANK_MASSERASE;
      }

      FLASH_MassErase(pEraseInit->VoltageRange, pEraseInit->Banks);
    }
    else
    {
      /* Erase by sector to be done */
      if(pEraseInit->Banks == FLASH_BANK_1)
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_SECTERASE_BANK1;
      }
      else
      {
        pFlash.ProcedureOnGoing = FLASH_PROC_SECTERASE_BANK2;
      }

      pFlash.NbSectorsToErase = pEraseInit->NbSectors;
      pFlash.Sector = pEraseInit->Sector;
      pFlash.VoltageForErase = pEraseInit->VoltageRange;

      /* Erase first sector and wait for IT */
      FLASH_Erase_Sector(pEraseInit->Sector, pEraseInit->Banks, pEraseInit->VoltageRange);
    }
  }

  return status;
}

/**
  * @brief  Program option bytes
  * @param  pOBInit pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  *
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit)
{
  HAL_StatusTypeDef status;

  /* Check the parameters */
  assert_param(IS_OPTIONBYTE(pOBInit->OptionType));

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset Error Code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed */
  if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    status = HAL_OK;
  }

  if(status == HAL_OK)
  {
    /*Write protection configuration*/
    if((pOBInit->OptionType & OPTIONBYTE_WRP) == OPTIONBYTE_WRP)
    {
      assert_param(IS_WRPSTATE(pOBInit->WRPState));

      if(pOBInit->WRPState == OB_WRPSTATE_ENABLE)
      {
        /*Enable of Write protection on the selected Sector*/
        FLASH_OB_EnableWRP(pOBInit->WRPSector,pOBInit->Banks);
      }
      else
      {
        /*Disable of Write protection on the selected Sector*/
        FLASH_OB_DisableWRP(pOBInit->WRPSector, pOBInit->Banks);
      }
    }

    /* Read protection configuration */
    if((pOBInit->OptionType & OPTIONBYTE_RDP) != 0U)
    {
      /* Configure the Read protection level */
      FLASH_OB_RDPConfig(pOBInit->RDPLevel);
    }

    /* User Configuration */
    if((pOBInit->OptionType & OPTIONBYTE_USER) != 0U)
    {
      /* Configure the user option bytes */
      FLASH_OB_UserConfig(pOBInit->USERType, pOBInit->USERConfig);
    }

    /* PCROP Configuration */
    if((pOBInit->OptionType & OPTIONBYTE_PCROP) != 0U)
    {
      assert_param(IS_FLASH_BANK(pOBInit->Banks));

      /*Configure the Proprietary code readout protection */
      FLASH_OB_PCROPConfig(pOBInit->PCROPConfig, pOBInit->PCROPStartAddr, pOBInit->PCROPEndAddr, pOBInit->Banks);
    }

    /* BOR Level configuration */
    if((pOBInit->OptionType & OPTIONBYTE_BOR) == OPTIONBYTE_BOR)
    {
      FLASH_OB_BOR_LevelConfig(pOBInit->BORLevel);
    }

#if defined(DUAL_CORE)
    /* CM7 Boot Address  configuration */
    if((pOBInit->OptionType & OPTIONBYTE_CM7_BOOTADD) == OPTIONBYTE_CM7_BOOTADD)
    {
      FLASH_OB_BootAddConfig(pOBInit->BootConfig, pOBInit->BootAddr0, pOBInit->BootAddr1);
    }

    /* CM4 Boot Address  configuration */
    if((pOBInit->OptionType & OPTIONBYTE_CM4_BOOTADD) == OPTIONBYTE_CM4_BOOTADD)
    {
      FLASH_OB_CM4BootAddConfig(pOBInit->CM4BootConfig, pOBInit->CM4BootAddr0, pOBInit->CM4BootAddr1);
    }
#else /* Single Core*/
    /* Boot Address  configuration */
    if((pOBInit->OptionType & OPTIONBYTE_BOOTADD) == OPTIONBYTE_BOOTADD)
    {
      FLASH_OB_BootAddConfig(pOBInit->BootConfig, pOBInit->BootAddr0, pOBInit->BootAddr1);
    }
#endif /*DUAL_CORE*/

    /* Secure area configuration */
    if((pOBInit->OptionType & OPTIONBYTE_SECURE_AREA) == OPTIONBYTE_SECURE_AREA)
    {
      FLASH_OB_SecureAreaConfig(pOBInit->SecureAreaConfig, pOBInit->SecureAreaStartAddr, pOBInit->SecureAreaEndAddr,pOBInit->Banks);
    }

#if defined(FLASH_OTPBL_LOCKBL)
    /* OTP Block Lock configuration */
    if((pOBInit->OptionType & OPTIONBYTE_OTP_LOCK) == OPTIONBYTE_OTP_LOCK)
    {
      FLASH_OB_OTP_LockConfig(pOBInit->OTPBlockLock);
    }
#endif /* FLASH_OTPBL_LOCKBL */
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief Get the Option byte configuration
  * @param  pOBInit pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  * @note   The parameter Banks of the pOBInit structure must be set exclusively to FLASH_BANK_1 or FLASH_BANK_2,
  *         as this parameter is use to get the given Bank WRP, PCROP and secured area configuration.
  *
  * @retval None
  */
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit)
{
  pOBInit->OptionType = (OPTIONBYTE_USER | OPTIONBYTE_RDP | OPTIONBYTE_BOR);

  /* Get Read protection level */
  pOBInit->RDPLevel = FLASH_OB_GetRDP();

  /* Get the user option bytes */
  pOBInit->USERConfig = FLASH_OB_GetUser();

  /*Get BOR Level*/
  pOBInit->BORLevel = FLASH_OB_GetBOR();

  if ((pOBInit->Banks == FLASH_BANK_1) || (pOBInit->Banks == FLASH_BANK_2))
  {
    pOBInit->OptionType |= (OPTIONBYTE_WRP | OPTIONBYTE_PCROP | OPTIONBYTE_SECURE_AREA);

    /* Get write protection on the selected area */
    FLASH_OB_GetWRP(&(pOBInit->WRPState), &(pOBInit->WRPSector), pOBInit->Banks);

    /* Get the Proprietary code readout protection */
    FLASH_OB_GetPCROP(&(pOBInit->PCROPConfig), &(pOBInit->PCROPStartAddr), &(pOBInit->PCROPEndAddr), pOBInit->Banks);

    /*Get Bank Secure area*/
    FLASH_OB_GetSecureArea(&(pOBInit->SecureAreaConfig), &(pOBInit->SecureAreaStartAddr), &(pOBInit->SecureAreaEndAddr), pOBInit->Banks);
  }

  /*Get Boot Address*/
  FLASH_OB_GetBootAdd(&(pOBInit->BootAddr0), &(pOBInit->BootAddr1));
#if defined(DUAL_CORE)
  pOBInit->OptionType |= OPTIONBYTE_CM7_BOOTADD | OPTIONBYTE_CM4_BOOTADD;

  /*Get CM4 Boot Address*/
  FLASH_OB_GetCM4BootAdd(&(pOBInit->CM4BootAddr0), &(pOBInit->CM4BootAddr1));
#else
  pOBInit->OptionType |= OPTIONBYTE_BOOTADD;
#endif /*DUAL_CORE*/

#if defined (FLASH_OTPBL_LOCKBL)
  pOBInit->OptionType |= OPTIONBYTE_OTP_LOCK;

  /* Get OTP Block Lock */
  pOBInit->OTPBlockLock = FLASH_OB_OTP_GetLock();
#endif /* FLASH_OTPBL_LOCKBL */
}

/**
  * @brief  Unlock the FLASH Bank1 control registers access
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Unlock_Bank1(void)
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

  return HAL_OK;
}

/**
  * @brief  Locks the FLASH Bank1 control registers access
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Lock_Bank1(void)
{
  /* Set the LOCK Bit to lock the FLASH Bank1 Registers access */
  SET_BIT(FLASH->CR1, FLASH_CR_LOCK);
  return HAL_OK;
}

/**
  * @brief  Unlock the FLASH Bank2 control registers access
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Unlock_Bank2(void)
{
  if(READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
  {
    /* Authorize the FLASH Bank2 Registers access */
    WRITE_REG(FLASH->KEYR2, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR2, FLASH_KEY2);

    /* Verify Flash Bank1 is unlocked */
    if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Locks the FLASH Bank2 control registers access
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Lock_Bank2(void)
{
  /* Set the LOCK Bit to lock the FLASH Bank2 Registers access */
  SET_BIT(FLASH->CR2, FLASH_CR_LOCK);
  return HAL_OK;
}

/*
  * @brief  Perform a CRC computation on the specified FLASH memory area
  * @param  pCRCInit pointer to an FLASH_CRCInitTypeDef structure that
  *         contains the configuration information for the CRC computation.
  * @note   CRC computation uses CRC-32 (Ethernet) polynomial 0x4C11DB7
  * @note   The application should avoid running a CRC on PCROP or secure-only
  *         user Flash memory area since it may alter the expected CRC value.
  *         A special error flag (CRC read error: CRCRDERR) can be used to
  *         detect such a case.
  * @retval HAL Status
*/
HAL_StatusTypeDef HAL_FLASHEx_ComputeCRC(FLASH_CRCInitTypeDef *pCRCInit, uint32_t *CRC_Result)
{
  HAL_StatusTypeDef status;
  uint32_t sector_index;

  /* Check the parameters */
  assert_param(IS_FLASH_BANK_EXCLUSIVE(pCRCInit->Bank));
  assert_param(IS_FLASH_TYPECRC(pCRCInit->TypeCRC));

  /* Wait for OB change operation to be completed */
  status = FLASH_OB_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    if (pCRCInit->Bank == FLASH_BANK_1)
    {
      /* Enable CRC feature */
      FLASH->CR1 |= FLASH_CR_CRC_EN;

      /* Clear CRC flags in Status Register: CRC end of calculation and CRC read error */
      FLASH->CCR1 |= (FLASH_CCR_CLR_CRCEND | FLASH_CCR_CLR_CRCRDERR);

      /* Clear current CRC result, program burst size and define memory area on which CRC has to be computed */
      FLASH->CRCCR1 |= FLASH_CRCCR_CLEAN_CRC | pCRCInit->BurstSize | pCRCInit->TypeCRC;

      if (pCRCInit->TypeCRC == FLASH_CRC_SECTORS)
      {
        /* Clear sectors list */
        FLASH->CRCCR1 |= FLASH_CRCCR_CLEAN_SECT;

        /* Select CRC sectors */
        for(sector_index = pCRCInit->Sector; sector_index < (pCRCInit->NbSectors + pCRCInit->Sector); sector_index++)
        {
          FLASH_CRC_AddSector(sector_index, FLASH_BANK_1);
        }
      }
      else if (pCRCInit->TypeCRC == FLASH_CRC_BANK)
      {
        /* Enable Bank 1 CRC select bit */
        FLASH->CRCCR1 |= FLASH_CRCCR_ALL_BANK;
      }
      else
      {
        /* Select CRC start and end addresses */
        FLASH_CRC_SelectAddress(pCRCInit->CRCStartAddr, pCRCInit->CRCEndAddr, FLASH_BANK_1);
      }

      /* Start the CRC calculation */
      FLASH->CRCCR1 |= FLASH_CRCCR_START_CRC;

      /* Wait on CRC busy flag */
      status = FLASH_CRC_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1);

      /* Return CRC result */
      (*CRC_Result) = FLASH->CRCDATA;

      /* Disable CRC feature */
      FLASH->CR1 &= (~FLASH_CR_CRC_EN);

      /* Clear CRC flags */
      __HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_CRCEND_BANK1 | FLASH_FLAG_CRCRDERR_BANK1);
    }
    else
    {
      /* Enable CRC feature */
      FLASH->CR2 |= FLASH_CR_CRC_EN;

      /* Clear CRC flags in Status Register: CRC end of calculation and CRC read error */
      FLASH->CCR2 |= (FLASH_CCR_CLR_CRCEND | FLASH_CCR_CLR_CRCRDERR);

      /* Clear current CRC result, program burst size and define memory area on which CRC has to be computed */
      FLASH->CRCCR2 |= FLASH_CRCCR_CLEAN_CRC | pCRCInit->BurstSize | pCRCInit->TypeCRC;

      if (pCRCInit->TypeCRC == FLASH_CRC_SECTORS)
      {
        /* Clear sectors list */
        FLASH->CRCCR2 |= FLASH_CRCCR_CLEAN_SECT;

        /* Add CRC sectors */
        for(sector_index = pCRCInit->Sector; sector_index < (pCRCInit->NbSectors + pCRCInit->Sector); sector_index++)
        {
          FLASH_CRC_AddSector(sector_index, FLASH_BANK_2);
        }
      }
      else if (pCRCInit->TypeCRC == FLASH_CRC_BANK)
      {
        /* Enable Bank 2 CRC select bit */
        FLASH->CRCCR2 |= FLASH_CRCCR_ALL_BANK;
      }
      else
      {
        /* Select CRC start and end addresses */
        FLASH_CRC_SelectAddress(pCRCInit->CRCStartAddr, pCRCInit->CRCEndAddr, FLASH_BANK_2);
      }

      /* Start the CRC calculation */
      FLASH->CRCCR2 |= FLASH_CRCCR_START_CRC;

      /* Wait on CRC busy flag */
      status = FLASH_CRC_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_2);

      /* Return CRC result */
      (*CRC_Result) = FLASH->CRCDATA;

      /* Disable CRC feature */
      FLASH->CR2 &= (~FLASH_CR_CRC_EN);

      /* Clear CRC flags */
      __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_CRCEND_BANK2 | FLASH_FLAG_CRCRDERR_BANK2);
    }
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @addtogroup FLASHEx_Private_Functions
  * @{
  */

/**
  * @brief  Mass erase of FLASH memory
  * @param  VoltageRange The device program/erase parallelism.
  *          This parameter can be one of the following values:
  *            @arg FLASH_VOLTAGE_RANGE_1 : Flash program/erase by 8 bits
  *            @arg FLASH_VOLTAGE_RANGE_2 : Flash program/erase by 16 bits
  *            @arg FLASH_VOLTAGE_RANGE_3 : Flash program/erase by 32 bits
  *            @arg FLASH_VOLTAGE_RANGE_4 : Flash program/erase by 64 bits
  *
  * @param  Banks Banks to be erased
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: Bank1 to be erased
  *            @arg FLASH_BANK_2: Bank2 to be erased
  *            @arg FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  *
  * @retval HAL Status
  */
static void FLASH_MassErase(uint32_t VoltageRange, uint32_t Banks)
{
  /* Check the parameters */
#if defined (FLASH_CR_PSIZE)
  assert_param(IS_VOLTAGERANGE(VoltageRange));
#else
  UNUSED(VoltageRange);
#endif /* FLASH_CR_PSIZE */
  assert_param(IS_FLASH_BANK(Banks));

  /* Flash Mass Erase */
  if((Banks & FLASH_BANK_BOTH) == FLASH_BANK_BOTH)
  {
#if defined (FLASH_CR_PSIZE)
    /* Reset Program/erase VoltageRange for Bank1 and Bank2 */
    FLASH->CR1 &= (~FLASH_CR_PSIZE);
    FLASH->CR2 &= (~FLASH_CR_PSIZE);

    /* Set voltage range */
    FLASH->CR1 |= VoltageRange;
    FLASH->CR2 |= VoltageRange;
#endif /* FLASH_CR_PSIZE */

    /* Set Mass Erase Bit */
    FLASH->OPTCR |= FLASH_OPTCR_MER;
  }
  else
  {
    /* Proceed to erase Flash Bank  */
    if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
    {
#if defined (FLASH_CR_PSIZE)
      /* Set Program/erase VoltageRange for Bank1 */
      FLASH->CR1 &= (~FLASH_CR_PSIZE);
      FLASH->CR1 |=  VoltageRange;
#endif /* FLASH_CR_PSIZE */

      /* Erase Bank1 */
      FLASH->CR1 |= (FLASH_CR_BER | FLASH_CR_START);
    }
    if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
    {
#if defined (FLASH_CR_PSIZE)
      /* Set Program/erase VoltageRange for Bank2 */
      FLASH->CR2 &= (~FLASH_CR_PSIZE);
      FLASH->CR2 |= VoltageRange;
#endif /* FLASH_CR_PSIZE */

      /* Erase Bank2 */
      FLASH->CR2 |= (FLASH_CR_BER | FLASH_CR_START);
    }
  }
}

/**
  * @brief  Erase the specified FLASH memory sector
  * @param  Sector FLASH sector to erase
  *          This parameter can be a value of @ref FLASH_Sectors
  * @param  Banks Banks to be erased
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: Bank1 to be erased
  *            @arg FLASH_BANK_2: Bank2 to be erased
  *            @arg FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  * @param  VoltageRange The device program/erase parallelism.
  *          This parameter can be one of the following values:
  *            @arg FLASH_VOLTAGE_RANGE_1 : Flash program/erase by 8 bits
  *            @arg FLASH_VOLTAGE_RANGE_2 : Flash program/erase by 16 bits
  *            @arg FLASH_VOLTAGE_RANGE_3 : Flash program/erase by 32 bits
  *            @arg FLASH_VOLTAGE_RANGE_4 : Flash program/erase by 64 bits
  *
  * @retval None
  */
void FLASH_Erase_Sector(uint32_t Sector, uint32_t Banks, uint32_t VoltageRange)
{
  assert_param(IS_FLASH_SECTOR(Sector));
  assert_param(IS_FLASH_BANK_EXCLUSIVE(Banks));
#if defined (FLASH_CR_PSIZE)
  assert_param(IS_VOLTAGERANGE(VoltageRange));
#else
  UNUSED(VoltageRange);
#endif /* FLASH_CR_PSIZE */

  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
#if defined (FLASH_CR_PSIZE)
    /* Reset Program/erase VoltageRange and Sector Number for Bank1 */
    FLASH->CR1 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);

    FLASH->CR1 |= (FLASH_CR_SER | VoltageRange | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#else
    /* Reset Sector Number for Bank1 */
    FLASH->CR1 &= ~(FLASH_CR_SNB);

    FLASH->CR1 |= (FLASH_CR_SER | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#endif /* FLASH_CR_PSIZE */
  }

  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
#if defined (FLASH_CR_PSIZE)
    /* Reset Program/erase VoltageRange and Sector Number for Bank2 */
    FLASH->CR2 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);

    FLASH->CR2 |= (FLASH_CR_SER | VoltageRange  | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#else
    /* Reset Sector Number for Bank2 */
    FLASH->CR2 &= ~(FLASH_CR_SNB);

    FLASH->CR2 |= (FLASH_CR_SER | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#endif /* FLASH_CR_PSIZE */
  }
}

/**
  * @brief  Enable the write protection of the desired bank1 or bank 2 sectors
  * @param  WRPSector specifies the sector(s) to be write protected.
  *          This parameter can be one of the following values:
  *            @arg WRPSector:  A combination of OB_WRP_SECTOR_0 to OB_WRP_SECTOR_7 or OB_WRP_SECTOR_All
  *
  * @param  Banks the specific bank to apply WRP sectors
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: enable WRP on specified bank1 sectors
  *            @arg FLASH_BANK_2: enable WRP on specified bank2 sectors
  *            @arg FLASH_BANK_BOTH: enable WRP on both bank1 and bank2 specified sectors
  *
  * @retval HAL FLASH State
  */
static void FLASH_OB_EnableWRP(uint32_t WRPSector, uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_OB_WRP_SECTOR(WRPSector));
  assert_param(IS_FLASH_BANK(Banks));

  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    /* Enable Write Protection for bank 1 */
    FLASH->WPSN_PRG1 &= (~(WRPSector & FLASH_WPSN_WRPSN));
  }

  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    /* Enable Write Protection for bank 2 */
    FLASH->WPSN_PRG2 &= (~(WRPSector & FLASH_WPSN_WRPSN));
  }
}

/**
  * @brief  Disable the write protection of the desired bank1 or bank 2 sectors
  * @param  WRPSector specifies the sector(s) to disable write protection.
  *          This parameter can be one of the following values:
  *            @arg WRPSector:  A combination of FLASH_OB_WRP_SECTOR_0 to FLASH_OB_WRP_SECTOR_7 or FLASH_OB_WRP_SECTOR_All
  *
  * @param  Banks the specific bank to apply WRP sectors
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: disable WRP on specified bank1 sectors
  *            @arg FLASH_BANK_2: disable WRP on specified bank2 sectors
  *            @arg FLASH_BANK_BOTH: disable WRP on both bank1 and bank2 specified sectors
  *
  * @retval HAL FLASH State
  */
static void FLASH_OB_DisableWRP(uint32_t WRPSector, uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_OB_WRP_SECTOR(WRPSector));
  assert_param(IS_FLASH_BANK(Banks));

  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    /* Disable Write Protection for bank 1 */
    FLASH->WPSN_PRG1 |= (WRPSector & FLASH_WPSN_WRPSN);
  }

  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    /* Disable Write Protection for bank 2 */
    FLASH->WPSN_PRG2 |= (WRPSector & FLASH_WPSN_WRPSN);
  }
}

/**
  * @brief  Get the write protection of the given bank 1 or bank 2 sectors
  * @param  WRPState gives the write protection state on the given bank.
  *          This parameter can be one of the following values:
  *          @arg WRPState: OB_WRPSTATE_DISABLE or OB_WRPSTATE_ENABLE

  * @param  WRPSector gives the write protected sector(s) on the given bank .
  *          This parameter can be one of the following values:
  *          @arg WRPSector: A combination of FLASH_OB_WRP_SECTOR_0 to FLASH_OB_WRP_SECTOR_7 or FLASH_OB_WRP_SECTOR_All
  *
  * @param  Bank the specific bank to apply WRP sectors
  *          This parameter can be exclusively one of the following values:
  *            @arg FLASH_BANK_1: Get bank1 WRP sectors
  *            @arg FLASH_BANK_2: Get bank2 WRP sectors
  *            @arg FLASH_BANK_BOTH: note allowed in this functions
  *
  * @retval HAL FLASH State
  */
static void FLASH_OB_GetWRP(uint32_t *WRPState, uint32_t *WRPSector, uint32_t Bank)
{
  uint32_t regvalue = 0U;

  if((Bank & FLASH_BANK_BOTH) == FLASH_BANK_1)
  {
    regvalue = FLASH->WPSN_CUR1;
  }

  if((Bank & FLASH_BANK_BOTH) == FLASH_BANK_2)
  {
    regvalue = FLASH->WPSN_CUR2;
  }

  (*WRPSector) = (~regvalue) & FLASH_WPSN_WRPSN;

  if(*WRPSector == 0U)
  {
    (*WRPState) = OB_WRPSTATE_DISABLE;
  }
  else
  {
    (*WRPState) = OB_WRPSTATE_ENABLE;
  }
}

/**
  * @brief  Set the read protection level.
  *
  * @note   To configure the RDP level, the option lock bit OPTLOCK must be
  *         cleared with the call of the HAL_FLASH_OB_Unlock() function.
  * @note   To validate the RDP level, the option bytes must be reloaded
  *         through the call of the HAL_FLASH_OB_Launch() function.
  * @note   !!! Warning : When enabling OB_RDP level 2 it's no more possible
  *         to go back to level 1 or 0 !!!
  *
  * @param  RDPLevel specifies the read protection level.
  *         This parameter can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  *
  * @retval HAL status
  */
static void FLASH_OB_RDPConfig(uint32_t RDPLevel)
{
  /* Check the parameters */
  assert_param(IS_OB_RDP_LEVEL(RDPLevel));

  /* Configure the RDP level in the option bytes register */
  MODIFY_REG(FLASH->OPTSR_PRG, FLASH_OPTSR_RDP, RDPLevel);
}

/**
  * @brief  Get the read protection level.
  * @retval RDPLevel specifies the read protection level.
  *         This return value can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  */
static uint32_t FLASH_OB_GetRDP(void)
{
  uint32_t rdp_level = READ_BIT(FLASH->OPTSR_CUR, FLASH_OPTSR_RDP);
  
  if ((rdp_level != OB_RDP_LEVEL_0) && (rdp_level != OB_RDP_LEVEL_2))
  {
    return (OB_RDP_LEVEL_1);
  }
  else
  {
    return rdp_level;
  }
}

#if defined(DUAL_CORE)
/**
  * @brief  Program the FLASH User Option Byte.
  *
  * @note   To configure the user option bytes, the option lock bit OPTLOCK must
  *         be cleared with the call of the HAL_FLASH_OB_Unlock() function.
  *
  * @note   To validate the user option bytes, the option bytes must be reloaded
  *         through the call of the HAL_FLASH_OB_Launch() function.
  *
  * @param  UserType The FLASH User Option Bytes to be modified :
  *                   a combination of @ref FLASHEx_OB_USER_Type
  *
  * @param  UserConfig The FLASH User Option Bytes values:
  *         IWDG1_SW(Bit4), IWDG2_SW(Bit 5), nRST_STOP_D1(Bit 6), nRST_STDY_D1(Bit 7),
  *         FZ_IWDG_STOP(Bit 17), FZ_IWDG_SDBY(Bit 18), ST_RAM_SIZE(Bit[19:20]),
  *         SECURITY(Bit 21), BCM4(Bit 22), BCM7(Bit 23), nRST_STOP_D2(Bit 24),
  *         nRST_STDY_D2(Bit 25), IO_HSLV (Bit 29) and SWAP_BANK_OPT(Bit 31).
  *
  * @retval HAL status
  */
#else
/**
  * @brief  Program the FLASH User Option Byte.
  *
  * @note   To configure the user option bytes, the option lock bit OPTLOCK must
  *         be cleared with the call of the HAL_FLASH_OB_Unlock() function.
  *
  * @note   To validate the user option bytes, the option bytes must be reloaded
  *         through the call of the HAL_FLASH_OB_Launch() function.
  *
  * @param  UserType The FLASH User Option Bytes to be modified :
  *                   a combination of @arg FLASHEx_OB_USER_Type
  *
  * @param  UserConfig The FLASH User Option Bytes values:
  *         IWDG_SW(Bit4), nRST_STOP_D1(Bit 6), nRST_STDY_D1(Bit 7),
  *         FZ_IWDG_STOP(Bit 17), FZ_IWDG_SDBY(Bit 18), ST_RAM_SIZE(Bit[19:20]),
  *         SECURITY(Bit 21), IO_HSLV (Bit 29) and SWAP_BANK_OPT(Bit 31).
  *
  * @retval HAL status
  */
#endif /*DUAL_CORE*/
static void FLASH_OB_UserConfig(uint32_t UserType, uint32_t UserConfig)
{
  uint32_t optr_reg_val = 0;
  uint32_t optr_reg_mask = 0;

  /* Check the parameters */
  assert_param(IS_OB_USER_TYPE(UserType));

  if((UserType & OB_USER_IWDG1_SW) != 0U)
  {
    /* IWDG_HW option byte should be modified */
    assert_param(IS_OB_IWDG1_SOURCE(UserConfig & FLASH_OPTSR_IWDG1_SW));

    /* Set value and mask for IWDG_HW option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_IWDG1_SW);
    optr_reg_mask |= FLASH_OPTSR_IWDG1_SW;
  }
#if defined(DUAL_CORE)
  if((UserType & OB_USER_IWDG2_SW) != 0U)
  {
    /* IWDG2_SW option byte should be modified */
    assert_param(IS_OB_IWDG2_SOURCE(UserConfig & FLASH_OPTSR_IWDG2_SW));

    /* Set value and mask for IWDG2_SW option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_IWDG2_SW);
    optr_reg_mask |= FLASH_OPTSR_IWDG2_SW;
  }
#endif /*DUAL_CORE*/
  if((UserType & OB_USER_NRST_STOP_D1) != 0U)
  {
    /* NRST_STOP option byte should be modified */
    assert_param(IS_OB_STOP_D1_RESET(UserConfig & FLASH_OPTSR_NRST_STOP_D1));

    /* Set value and mask for NRST_STOP option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_NRST_STOP_D1);
    optr_reg_mask |= FLASH_OPTSR_NRST_STOP_D1;
  }

  if((UserType & OB_USER_NRST_STDBY_D1) != 0U)
  {
    /* NRST_STDBY option byte should be modified */
    assert_param(IS_OB_STDBY_D1_RESET(UserConfig & FLASH_OPTSR_NRST_STBY_D1));

    /* Set value and mask for NRST_STDBY option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_NRST_STBY_D1);
    optr_reg_mask |= FLASH_OPTSR_NRST_STBY_D1;
  }

  if((UserType & OB_USER_IWDG_STOP) != 0U)
  {
    /* IWDG_STOP option byte should be modified */
    assert_param(IS_OB_USER_IWDG_STOP(UserConfig & FLASH_OPTSR_FZ_IWDG_STOP));

    /* Set value and mask for IWDG_STOP option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_FZ_IWDG_STOP);
    optr_reg_mask |= FLASH_OPTSR_FZ_IWDG_STOP;
  }

  if((UserType & OB_USER_IWDG_STDBY) != 0U)
  {
    /* IWDG_STDBY option byte should be modified */
    assert_param(IS_OB_USER_IWDG_STDBY(UserConfig & FLASH_OPTSR_FZ_IWDG_SDBY));

    /* Set value and mask for IWDG_STDBY option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_FZ_IWDG_SDBY);
    optr_reg_mask |= FLASH_OPTSR_FZ_IWDG_SDBY;
  }

  if((UserType & OB_USER_ST_RAM_SIZE) != 0U)
  {
    /* ST_RAM_SIZE option byte should be modified */
    assert_param(IS_OB_USER_ST_RAM_SIZE(UserConfig & FLASH_OPTSR_ST_RAM_SIZE));

    /* Set value and mask for ST_RAM_SIZE option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_ST_RAM_SIZE);
    optr_reg_mask |= FLASH_OPTSR_ST_RAM_SIZE;
  }

  if((UserType & OB_USER_SECURITY) != 0U)
  {
    /* SECURITY option byte should be modified */
    assert_param(IS_OB_USER_SECURITY(UserConfig & FLASH_OPTSR_SECURITY));

    /* Set value and mask for SECURITY option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_SECURITY);
    optr_reg_mask |= FLASH_OPTSR_SECURITY;
  }

#if defined(DUAL_CORE)
  if((UserType & OB_USER_BCM4) != 0U)
  {
    /* BCM4 option byte should be modified */
    assert_param(IS_OB_USER_BCM4(UserConfig & FLASH_OPTSR_BCM4));

    /* Set value and mask for BCM4 option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_BCM4);
    optr_reg_mask |= FLASH_OPTSR_BCM4;
  }

  if((UserType & OB_USER_BCM7) != 0U)
  {
    /* BCM7 option byte should be modified */
    assert_param(IS_OB_USER_BCM7(UserConfig & FLASH_OPTSR_BCM7));

    /* Set value and mask for BCM7 option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_BCM7);
    optr_reg_mask |= FLASH_OPTSR_BCM7;
  }

  if((UserType & OB_USER_NRST_STOP_D2) != 0U)
  {
    /* NRST_STOP option byte should be modified */
    assert_param(IS_OB_STOP_D2_RESET(UserConfig & FLASH_OPTSR_NRST_STOP_D2));

    /* Set value and mask for NRST_STOP option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_NRST_STOP_D2);
    optr_reg_mask |= FLASH_OPTSR_NRST_STOP_D2;
  }

  if((UserType & OB_USER_NRST_STDBY_D2) != 0U)
  {
    /* NRST_STDBY option byte should be modified */
    assert_param(IS_OB_STDBY_D2_RESET(UserConfig & FLASH_OPTSR_NRST_STBY_D2));

    /* Set value and mask for NRST_STDBY option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_NRST_STBY_D2);
    optr_reg_mask |= FLASH_OPTSR_NRST_STBY_D2;
  }
#endif /* DUAL_CORE */
  if((UserType & OB_USER_SWAP_BANK) != 0U)
  {
    /* SWAP_BANK_OPT option byte should be modified */
    assert_param(IS_OB_USER_SWAP_BANK(UserConfig & FLASH_OPTSR_SWAP_BANK_OPT));

    /* Set value and mask for SWAP_BANK_OPT option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_SWAP_BANK_OPT);
    optr_reg_mask |= FLASH_OPTSR_SWAP_BANK_OPT;
  }

  if((UserType & OB_USER_IOHSLV) != 0U)
  {
    /* IOHSLV_OPT option byte should be modified */
    assert_param(IS_OB_USER_IOHSLV(UserConfig & FLASH_OPTSR_IO_HSLV));

    /* Set value and mask for IOHSLV_OPT option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_IO_HSLV);
    optr_reg_mask |= FLASH_OPTSR_IO_HSLV;
  }

#if defined (FLASH_OPTSR_VDDMMC_HSLV)
  if((UserType & OB_USER_VDDMMC_HSLV) != 0U)
  {
    /* VDDMMC_HSLV option byte should be modified */
    assert_param(IS_OB_USER_VDDMMC_HSLV(UserConfig & FLASH_OPTSR_VDDMMC_HSLV));

    /* Set value and mask for VDDMMC_HSLV option byte */
    optr_reg_val |= (UserConfig & FLASH_OPTSR_VDDMMC_HSLV);
    optr_reg_mask |= FLASH_OPTSR_VDDMMC_HSLV;
  }
#endif /* FLASH_OPTSR_VDDMMC_HSLV */

  /* Configure the option bytes register */
  MODIFY_REG(FLASH->OPTSR_PRG, optr_reg_mask, optr_reg_val);
}

#if defined(DUAL_CORE)
/**
  * @brief  Return the FLASH User Option Byte value.
  * @retval The FLASH User Option Bytes values
  *         IWDG1_SW(Bit4), IWDG2_SW(Bit 5), nRST_STOP_D1(Bit 6), nRST_STDY_D1(Bit 7),
  *         FZ_IWDG_STOP(Bit 17), FZ_IWDG_SDBY(Bit 18), ST_RAM_SIZE(Bit[19:20]),
  *         SECURITY(Bit 21), BCM4(Bit 22), BCM7(Bit 23), nRST_STOP_D2(Bit 24),
  *         nRST_STDY_D2(Bit 25), IO_HSLV (Bit 29) and SWAP_BANK_OPT(Bit 31).
  */
#else
/**
  * @brief  Return the FLASH User Option Byte value.
  * @retval The FLASH User Option Bytes values
  *         IWDG_SW(Bit4), nRST_STOP_D1(Bit 6), nRST_STDY_D1(Bit 7),
  *         FZ_IWDG_STOP(Bit 17), FZ_IWDG_SDBY(Bit 18), ST_RAM_SIZE(Bit[19:20]),
  *         SECURITY(Bit 21), IO_HSLV (Bit 29) and SWAP_BANK_OPT(Bit 31).
  */
#endif /*DUAL_CORE*/
static uint32_t FLASH_OB_GetUser(void)
{
  uint32_t userConfig = READ_REG(FLASH->OPTSR_CUR);
  userConfig &= (~(FLASH_OPTSR_BOR_LEV | FLASH_OPTSR_RDP));

  return userConfig;
}

/**
  * @brief  Configure the Proprietary code readout protection of the desired addresses
  *
  * @note   To configure the PCROP options, the option lock bit OPTLOCK must be
  *         cleared with the call of the HAL_FLASH_OB_Unlock() function.
  * @note   To validate the PCROP options, the option bytes must be reloaded
  *         through the call of the HAL_FLASH_OB_Launch() function.
  *
  * @param  PCROPConfig specifies if the PCROP area for the given Bank shall be erased or not
  *         when RDP level decreased from Level 1 to Level 0, or after a bank erase with protection removal
  *         This parameter must be a value of @arg FLASHEx_OB_PCROP_RDP enumeration
  *
  * @param  PCROPStartAddr specifies the start address of the Proprietary code readout protection
  *          This parameter can be an address between begin and end of the bank
  *
  * @param  PCROPEndAddr specifies the end address of the Proprietary code readout protection
  *          This parameter can be an address between PCROPStartAddr and end of the bank
  *
  * @param  Banks the specific bank to apply PCROP protection
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: PCROP on specified bank1 area
  *            @arg FLASH_BANK_2: PCROP on specified bank2 area
  *            @arg FLASH_BANK_BOTH: PCROP on specified bank1 and bank2 area (same config will be applied on both banks)
  *
  * @retval None
  */
static void FLASH_OB_PCROPConfig(uint32_t PCROPConfig, uint32_t PCROPStartAddr, uint32_t PCROPEndAddr, uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_FLASH_BANK(Banks));
  assert_param(IS_OB_PCROP_RDP(PCROPConfig));

  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK1(PCROPStartAddr));
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK1(PCROPEndAddr));

    /* Configure the Proprietary code readout protection */
    FLASH->PRAR_PRG1 = ((PCROPStartAddr - FLASH_BANK1_BASE) >> 8)                                 | \
                       (((PCROPEndAddr - FLASH_BANK1_BASE) >> 8) << FLASH_PRAR_PROT_AREA_END_Pos) | \
                       PCROPConfig;
  }

  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK2(PCROPStartAddr));
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK2(PCROPEndAddr));

    /* Configure the Proprietary code readout protection */
    FLASH->PRAR_PRG2 = ((PCROPStartAddr - FLASH_BANK2_BASE) >> 8)                                 | \
                       (((PCROPEndAddr - FLASH_BANK2_BASE) >> 8) << FLASH_PRAR_PROT_AREA_END_Pos) | \
                       PCROPConfig;
  }
}

/**
  * @brief  Get the Proprietary code readout protection configuration on a given Bank
  *
  * @param  PCROPConfig indicates if the PCROP area for the given Bank shall be erased or not
  *         when RDP level decreased from Level 1 to Level 0 or after a bank erase with protection removal
  *
  * @param  PCROPStartAddr gives the start address of the Proprietary code readout protection of the bank
  *
  * @param  PCROPEndAddr gives the end address of the Proprietary code readout protection of the bank
  *
  * @param  Bank the specific bank to apply PCROP protection
  *          This parameter can be exclusively one of the following values:
  *            @arg FLASH_BANK_1: PCROP on specified bank1 area
  *            @arg FLASH_BANK_2: PCROP on specified bank2 area
  *            @arg FLASH_BANK_BOTH: is  not allowed here
  *
  * @retval None
  */
static void FLASH_OB_GetPCROP(uint32_t *PCROPConfig, uint32_t *PCROPStartAddr, uint32_t *PCROPEndAddr, uint32_t Bank)
{
  uint32_t regvalue = 0;
  uint32_t bankBase = 0;

  if((Bank & FLASH_BANK_BOTH) == FLASH_BANK_1)
  {
    regvalue = FLASH->PRAR_CUR1;
    bankBase = FLASH_BANK1_BASE;
  }

  if((Bank & FLASH_BANK_BOTH) == FLASH_BANK_2)
  {
    regvalue = FLASH->PRAR_CUR2;
    bankBase = FLASH_BANK2_BASE;
  }

  (*PCROPConfig) =  (regvalue & FLASH_PRAR_DMEP);

  (*PCROPStartAddr) = ((regvalue & FLASH_PRAR_PROT_AREA_START) << 8) + bankBase;
  (*PCROPEndAddr) = (regvalue & FLASH_PRAR_PROT_AREA_END) >> FLASH_PRAR_PROT_AREA_END_Pos;
  (*PCROPEndAddr) = ((*PCROPEndAddr) << 8) + bankBase;
}

/**
  * @brief  Set the BOR Level.
  * @param  Level specifies the Option Bytes BOR Reset Level.
  *          This parameter can be one of the following values:
  *            @arg OB_BOR_LEVEL0: Reset level threshold is set to 1.6V
  *            @arg OB_BOR_LEVEL1: Reset level threshold is set to 2.1V
  *            @arg OB_BOR_LEVEL2: Reset level threshold is set to 2.4V
  *            @arg OB_BOR_LEVEL3: Reset level threshold is set to 2.7V
  * @retval None
  */
static void FLASH_OB_BOR_LevelConfig(uint32_t Level)
{
  assert_param(IS_OB_BOR_LEVEL(Level));

  /* Configure BOR_LEV option byte */
  MODIFY_REG(FLASH->OPTSR_PRG, FLASH_OPTSR_BOR_LEV, Level);
}

/**
  * @brief  Get the BOR Level.
  * @retval The Option Bytes BOR Reset Level.
  *            This parameter can be one of the following values:
  *            @arg OB_BOR_LEVEL0: Reset level threshold is set to 1.6V
  *            @arg OB_BOR_LEVEL1: Reset level threshold is set to 2.1V
  *            @arg OB_BOR_LEVEL2: Reset level threshold is set to 2.4V
  *            @arg OB_BOR_LEVEL3: Reset level threshold is set to 2.7V
  */
static uint32_t FLASH_OB_GetBOR(void)
{
  return (FLASH->OPTSR_CUR & FLASH_OPTSR_BOR_LEV);
}

/**
  * @brief  Set Boot address
  * @param  BootOption Boot address option byte to be programmed,
  *                     This parameter must be a value of @ref FLASHEx_OB_BOOT_OPTION
                        (OB_BOOT_ADD0, OB_BOOT_ADD1 or OB_BOOT_ADD_BOTH)
  *
  * @param  BootAddress0 Specifies the Boot Address 0
  * @param  BootAddress1 Specifies the Boot Address 1
  * @retval HAL Status
  */
static void FLASH_OB_BootAddConfig(uint32_t BootOption, uint32_t BootAddress0, uint32_t BootAddress1)
{
  /* Check the parameters */
  assert_param(IS_OB_BOOT_ADD_OPTION(BootOption));

  if((BootOption & OB_BOOT_ADD0) == OB_BOOT_ADD0)
  {
    /* Check the parameters */
    assert_param(IS_BOOT_ADDRESS(BootAddress0));

    /* Configure CM7 BOOT ADD0 */
#if defined(DUAL_CORE)
    MODIFY_REG(FLASH->BOOT7_PRG, FLASH_BOOT7_BCM7_ADD0, (BootAddress0 >> 16));
#else /* Single Core*/
    MODIFY_REG(FLASH->BOOT_PRG, FLASH_BOOT_ADD0, (BootAddress0 >> 16));
#endif /* DUAL_CORE */
  }

  if((BootOption & OB_BOOT_ADD1) == OB_BOOT_ADD1)
  {
    /* Check the parameters */
    assert_param(IS_BOOT_ADDRESS(BootAddress1));

    /* Configure CM7 BOOT ADD1 */
#if defined(DUAL_CORE)
    MODIFY_REG(FLASH->BOOT7_PRG, FLASH_BOOT7_BCM7_ADD1, BootAddress1);
#else /* Single Core*/
    MODIFY_REG(FLASH->BOOT_PRG, FLASH_BOOT_ADD1, BootAddress1);
#endif /* DUAL_CORE */
  }
}

/**
  * @brief  Get Boot address
  * @param  BootAddress0 Specifies the Boot Address 0.
  * @param  BootAddress1 Specifies the Boot Address 1.
  * @retval HAL Status
  */
static void FLASH_OB_GetBootAdd(uint32_t *BootAddress0, uint32_t *BootAddress1)
{
  uint32_t regvalue;

#if defined(DUAL_CORE)
  regvalue = FLASH->BOOT7_CUR;

  (*BootAddress0) = (regvalue & FLASH_BOOT7_BCM7_ADD0) << 16;
  (*BootAddress1) = (regvalue & FLASH_BOOT7_BCM7_ADD1);
#else /* Single Core */
  regvalue = FLASH->BOOT_CUR;

  (*BootAddress0) = (regvalue & FLASH_BOOT_ADD0) << 16;
  (*BootAddress1) = (regvalue & FLASH_BOOT_ADD1);
#endif /* DUAL_CORE */
}

#if defined(DUAL_CORE)
/**
  * @brief  Set CM4 Boot address
  * @param  BootOption Boot address option byte to be programmed,
  *                     This parameter must be a value of @ref FLASHEx_OB_BOOT_OPTION
                        (OB_BOOT_ADD0, OB_BOOT_ADD1 or OB_BOOT_ADD_BOTH)
  *
  * @param  BootAddress0 Specifies the CM4 Boot Address 0.
  * @param  BootAddress1 Specifies the CM4 Boot Address 1.
  * @retval HAL Status
  */
static void FLASH_OB_CM4BootAddConfig(uint32_t BootOption, uint32_t BootAddress0, uint32_t BootAddress1)
{
  /* Check the parameters */
  assert_param(IS_OB_BOOT_ADD_OPTION(BootOption));

  if((BootOption & OB_BOOT_ADD0) == OB_BOOT_ADD0)
  {
    /* Check the parameters */
    assert_param(IS_BOOT_ADDRESS(BootAddress0));

    /* Configure CM4 BOOT ADD0 */
    MODIFY_REG(FLASH->BOOT4_PRG, FLASH_BOOT4_BCM4_ADD0, (BootAddress0 >> 16));

  }

  if((BootOption & OB_BOOT_ADD1) == OB_BOOT_ADD1)
  {
    /* Check the parameters */
    assert_param(IS_BOOT_ADDRESS(BootAddress1));

    /* Configure CM4 BOOT ADD1 */
    MODIFY_REG(FLASH->BOOT4_PRG, FLASH_BOOT4_BCM4_ADD1, BootAddress1);
  }
}

/**
  * @brief  Get CM4 Boot address
  * @param  BootAddress0 Specifies the CM4 Boot Address 0.
  * @param  BootAddress1 Specifies the CM4 Boot Address 1.
  * @retval HAL Status
  */
static void FLASH_OB_GetCM4BootAdd(uint32_t *BootAddress0, uint32_t *BootAddress1)
{
  uint32_t regvalue;

  regvalue = FLASH->BOOT4_CUR;

  (*BootAddress0) = (regvalue & FLASH_BOOT4_BCM4_ADD0) << 16;
  (*BootAddress1) = (regvalue & FLASH_BOOT4_BCM4_ADD1);
}
#endif /*DUAL_CORE*/

/**
  * @brief  Set secure area configuration
  * @param  SecureAreaConfig specify if the secure area will be deleted or not
  *         when RDP level decreased from Level 1 to Level 0 or during a mass erase.
  *
  * @param  SecureAreaStartAddr Specifies the secure area start address
  * @param  SecureAreaEndAddr Specifies the secure area end address
  * @param  Banks the specific bank to apply Security protection
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: Secure area on specified bank1 area
  *            @arg FLASH_BANK_2: Secure area on specified bank2 area
  *            @arg FLASH_BANK_BOTH: Secure area on specified bank1 and bank2 area (same config will be applied on both banks)
  * @retval None
  */
static void FLASH_OB_SecureAreaConfig(uint32_t SecureAreaConfig, uint32_t SecureAreaStartAddr, uint32_t SecureAreaEndAddr, uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_FLASH_BANK(Banks));
  assert_param(IS_OB_SECURE_RDP(SecureAreaConfig));

  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    /* Check the parameters */
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK1(SecureAreaStartAddr));
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK1(SecureAreaEndAddr));

    /* Configure the secure area */
    FLASH->SCAR_PRG1 = ((SecureAreaStartAddr - FLASH_BANK1_BASE) >> 8)                                | \
                       (((SecureAreaEndAddr - FLASH_BANK1_BASE) >> 8) << FLASH_SCAR_SEC_AREA_END_Pos) | \
                       (SecureAreaConfig & FLASH_SCAR_DMES);
  }

  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    /* Check the parameters */
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK2(SecureAreaStartAddr));
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK2(SecureAreaEndAddr));

    /* Configure the secure area */
    FLASH->SCAR_PRG2 = ((SecureAreaStartAddr - FLASH_BANK2_BASE) >> 8)                                | \
                       (((SecureAreaEndAddr - FLASH_BANK2_BASE) >> 8) << FLASH_SCAR_SEC_AREA_END_Pos) | \
                       (SecureAreaConfig & FLASH_SCAR_DMES);
  }
}

/**
  * @brief  Get secure area configuration
  * @param  SecureAreaConfig indicates if the secure area will be deleted or not
  *         when RDP level decreased from Level 1 to Level 0 or during a mass erase.
  * @param  SecureAreaStartAddr gives the secure area start address
  * @param  SecureAreaEndAddr gives the secure area end address
  * @param  Bank Specifies the Bank
  * @retval None
  */
static void FLASH_OB_GetSecureArea(uint32_t *SecureAreaConfig, uint32_t *SecureAreaStartAddr, uint32_t *SecureAreaEndAddr, uint32_t Bank)
{
  uint32_t regvalue = 0;
  uint32_t bankBase = 0;

  /* Check Bank parameter value */
  if((Bank & FLASH_BANK_BOTH) == FLASH_BANK_1)
  {
    regvalue = FLASH->SCAR_CUR1;
    bankBase = FLASH_BANK1_BASE;
  }

  if((Bank & FLASH_BANK_BOTH) == FLASH_BANK_2)
  {
    regvalue = FLASH->SCAR_CUR2;
    bankBase = FLASH_BANK2_BASE;
  }

  /* Get the secure area settings */
  (*SecureAreaConfig) = (regvalue & FLASH_SCAR_DMES);
  (*SecureAreaStartAddr) = ((regvalue & FLASH_SCAR_SEC_AREA_START) << 8) + bankBase;
  (*SecureAreaEndAddr) = (regvalue & FLASH_SCAR_SEC_AREA_END) >> FLASH_SCAR_SEC_AREA_END_Pos;
  (*SecureAreaEndAddr) = ((*SecureAreaEndAddr) << 8) + bankBase;
}

/**
  * @brief  Add a CRC sector to the list of sectors on which the CRC will be calculated
  * @param  Sector Specifies the CRC sector number
  * @param  Bank Specifies the Bank
  * @retval None
  */
static void FLASH_CRC_AddSector(uint32_t Sector, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FLASH_SECTOR(Sector));

  if (Bank == FLASH_BANK_1)
  {
    /* Clear CRC sector */
    FLASH->CRCCR1 &= (~FLASH_CRCCR_CRC_SECT);

    /* Select CRC Sector and activate ADD_SECT bit */
    FLASH->CRCCR1 |= Sector | FLASH_CRCCR_ADD_SECT;
  }
  else
  {
    /* Clear CRC sector */
    FLASH->CRCCR2 &= (~FLASH_CRCCR_CRC_SECT);

    /* Select CRC Sector and activate ADD_SECT bit */
    FLASH->CRCCR2 |= Sector | FLASH_CRCCR_ADD_SECT;
  }
}

/**
  * @brief  Select CRC start and end memory addresses on which the CRC will be calculated
  * @param  CRCStartAddr Specifies the CRC start address
  * @param  CRCEndAddr Specifies the CRC end address
  * @param  Bank Specifies the Bank
  * @retval None
  */
static void FLASH_CRC_SelectAddress(uint32_t CRCStartAddr, uint32_t CRCEndAddr, uint32_t Bank)
{
  if (Bank == FLASH_BANK_1)
  {
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK1(CRCStartAddr));
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK1(CRCEndAddr));

    /* Write CRC Start and End addresses */
    FLASH->CRCSADD1 = CRCStartAddr;
    FLASH->CRCEADD1 = CRCEndAddr;
  }
  else
  {
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK2(CRCStartAddr));
    assert_param(IS_FLASH_PROGRAM_ADDRESS_BANK2(CRCEndAddr));

    /* Write CRC Start and End addresses */
    FLASH->CRCSADD2 = CRCStartAddr;
    FLASH->CRCEADD2 = CRCEndAddr;
  }
}
/**
  * @}
  */

#if defined (FLASH_OTPBL_LOCKBL)
/**
  * @brief  Configure the OTP Block Lock.
  * @param  OTP_Block specifies the OTP Block to lock.
  *         This parameter can be a value of @ref FLASHEx_OTP_Blocks
  * @retval None
  */
static void FLASH_OB_OTP_LockConfig(uint32_t OTP_Block)
{
  /* Check the parameters */
  assert_param(IS_OTP_BLOCK(OTP_Block));

  /* Configure the OTP Block lock in the option bytes register */
  FLASH->OTPBL_PRG |= (OTP_Block & FLASH_OTPBL_LOCKBL);
}

/**
  * @brief  Get the OTP Block Lock.
  * @retval OTP_Block specifies the OTP Block to lock.
  *         This return value can be a value of @ref FLASHEx_OTP_Blocks
  */
static uint32_t FLASH_OB_OTP_GetLock(void)
{
  return (FLASH->OTPBL_CUR);
}
#endif /* FLASH_OTPBL_LOCKBL */

#endif /* HAL_FLASH_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
