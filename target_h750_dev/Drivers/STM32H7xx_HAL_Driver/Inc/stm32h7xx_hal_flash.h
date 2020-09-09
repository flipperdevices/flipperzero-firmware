/**
  ******************************************************************************
  * @file    stm32h7xx_hal_flash.h
  * @author  MCD Application Team
  * @brief   Header file of FLASH HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H7xx_HAL_FLASH_H
#define STM32H7xx_HAL_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup FLASH_Exported_Types FLASH Exported Types
  * @{
  */

/**
  * @brief  FLASH Procedure structure definition
  */
typedef enum
{
  FLASH_PROC_NONE = 0U,
  FLASH_PROC_SECTERASE_BANK1,
  FLASH_PROC_MASSERASE_BANK1,
  FLASH_PROC_PROGRAM_BANK1,
  FLASH_PROC_SECTERASE_BANK2,
  FLASH_PROC_MASSERASE_BANK2,
  FLASH_PROC_PROGRAM_BANK2,
  FLASH_PROC_ALLBANK_MASSERASE
} FLASH_ProcedureTypeDef;


/**
  * @brief  FLASH handle Structure definition
  */
typedef struct
{
  __IO FLASH_ProcedureTypeDef ProcedureOnGoing;   /*!< Internal variable to indicate which procedure is ongoing or not in IT context */

  __IO uint32_t               NbSectorsToErase;   /*!< Internal variable to save the remaining sectors to erase in IT context        */

  __IO uint32_t               VoltageForErase;    /*!< Internal variable to provide voltage range selected by user in IT context     */

  __IO uint32_t               Sector;             /*!< Internal variable to define the current sector which is erasing               */

  __IO uint32_t               Address;            /*!< Internal variable to save address selected for program                        */

  HAL_LockTypeDef             Lock;               /*!< FLASH locking object                                                          */

  __IO uint32_t               ErrorCode;          /*!< FLASH error code                                                              */

}FLASH_ProcessTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */

/** @defgroup FLASH_Error_Code FLASH Error Code
  * @brief    FLASH Error Code
  * @{
  */
#define HAL_FLASH_ERROR_NONE         0x00000000U               /*!< No error                              */

#define HAL_FLASH_ERROR_WRP          FLASH_FLAG_WRPERR         /*!< Write Protection Error                */
#define HAL_FLASH_ERROR_PGS          FLASH_FLAG_PGSERR         /*!< Program Sequence Error                */
#define HAL_FLASH_ERROR_STRB         FLASH_FLAG_STRBERR        /*!< Strobe Error                          */
#define HAL_FLASH_ERROR_INC          FLASH_FLAG_INCERR         /*!< Inconsistency Error                   */
#if defined (FLASH_SR_OPERR)
#define HAL_FLASH_ERROR_OPE          FLASH_FLAG_OPERR          /*!< Operation Error                       */
#endif /* FLASH_SR_OPERR */
#define HAL_FLASH_ERROR_RDP          FLASH_FLAG_RDPERR         /*!< Read Protection Error                 */
#define HAL_FLASH_ERROR_RDS          FLASH_FLAG_RDSERR         /*!< Read Secured Error                    */
#define HAL_FLASH_ERROR_SNECC        FLASH_FLAG_SNECCERR       /*!< ECC Single Correction Error           */
#define HAL_FLASH_ERROR_DBECC        FLASH_FLAG_DBECCERR       /*!< ECC Double Detection Error            */
#define HAL_FLASH_ERROR_CRCRD        FLASH_FLAG_CRCRDERR       /*!< CRC Read Error                        */

#define HAL_FLASH_ERROR_WRP_BANK1    FLASH_FLAG_WRPERR_BANK1   /*!< Write Protection Error on Bank 1      */
#define HAL_FLASH_ERROR_PGS_BANK1    FLASH_FLAG_PGSERR_BANK1   /*!< Program Sequence Error on Bank 1      */
#define HAL_FLASH_ERROR_STRB_BANK1   FLASH_FLAG_STRBERR_BANK1  /*!< Strobe Error on Bank 1                */
#define HAL_FLASH_ERROR_INC_BANK1    FLASH_FLAG_INCERR_BANK1   /*!< Inconsistency Error on Bank 1         */
#if defined (FLASH_SR_OPERR)
#define HAL_FLASH_ERROR_OPE_BANK1    FLASH_FLAG_OPERR_BANK1    /*!< Operation Error on Bank 1             */
#endif /* FLASH_SR_OPERR */
#define HAL_FLASH_ERROR_RDP_BANK1    FLASH_FLAG_RDPERR_BANK1   /*!< Read Protection Error on Bank 1       */
#define HAL_FLASH_ERROR_RDS_BANK1    FLASH_FLAG_RDSERR_BANK1   /*!< Read Secured Error on Bank 1          */
#define HAL_FLASH_ERROR_SNECC_BANK1  FLASH_FLAG_SNECCERR_BANK1 /*!< ECC Single Correction Error on Bank 1 */
#define HAL_FLASH_ERROR_DBECC_BANK1  FLASH_FLAG_DBECCERR_BANK1 /*!< ECC Double Detection Error on Bank 1  */
#define HAL_FLASH_ERROR_CRCRD_BANK1  FLASH_FLAG_CRCRDERR_BANK1 /*!< CRC Read Error on Bank1               */

#define HAL_FLASH_ERROR_WRP_BANK2    FLASH_FLAG_WRPERR_BANK2    /*!< Write Protection Error on Bank 2      */
#define HAL_FLASH_ERROR_PGS_BANK2    FLASH_FLAG_PGSERR_BANK2    /*!< Program Sequence Error on Bank 2      */
#define HAL_FLASH_ERROR_STRB_BANK2   FLASH_FLAG_STRBERR_BANK2   /*!< Strobe Error on Bank 2                */
#define HAL_FLASH_ERROR_INC_BANK2    FLASH_FLAG_INCERR_BANK2    /*!< Inconsistency Error on Bank 2         */
#if defined (FLASH_SR_OPERR)
#define HAL_FLASH_ERROR_OPE_BANK2    FLASH_FLAG_OPERR_BANK2     /*!< Operation Error on Bank 2             */
#endif /* FLASH_SR_OPERR */
#define HAL_FLASH_ERROR_RDP_BANK2    FLASH_FLAG_RDPERR_BANK2    /*!< Read Protection Error on Bank 2       */
#define HAL_FLASH_ERROR_RDS_BANK2    FLASH_FLAG_RDSERR_BANK2    /*!< Read Secured Error on Bank 2          */
#define HAL_FLASH_ERROR_SNECC_BANK2  FLASH_FLAG_SNECCERR_BANK2  /*!< ECC Single Correction Error on Bank 2 */
#define HAL_FLASH_ERROR_DBECC_BANK2  FLASH_FLAG_DBECCERR_BANK2  /*!< ECC Double Detection Error on Bank 2  */
#define HAL_FLASH_ERROR_CRCRD_BANK2  FLASH_FLAG_CRCRDERR_BANK2  /*!< CRC Read Error on Bank2               */

#define HAL_FLASH_ERROR_OB_CHANGE    FLASH_OPTSR_OPTCHANGEERR   /*!< Option Byte Change Error              */
/**
  * @}
  */

/** @defgroup FLASH_Type_Program FLASH Type Program
  * @{
  */
#define FLASH_TYPEPROGRAM_FLASHWORD  0x01U        /*!< Program a flash word at a specified address */
#if defined (FLASH_OPTCR_PG_OTP)
#define FLASH_TYPEPROGRAM_OTPWORD    0x02U        /*!< Program an OTP word at a specified address  */
#endif /* FLASH_OPTCR_PG_OTP */
/**
  * @}
  */

/** @defgroup FLASH_Flag_definition FLASH Flag definition
  * @brief Flag definition
  * @{
  */
#define FLASH_FLAG_BSY                     FLASH_SR_BSY             /*!< FLASH Busy flag */
#define FLASH_FLAG_WBNE                    FLASH_SR_WBNE            /*!< Write Buffer Not Empty flag */
#define FLASH_FLAG_QW                      FLASH_SR_QW              /*!< Wait Queue on flag */
#define FLASH_FLAG_CRC_BUSY                FLASH_SR_CRC_BUSY        /*!< CRC Busy flag */
#define FLASH_FLAG_EOP                     FLASH_SR_EOP             /*!< End Of Program on flag */
#define FLASH_FLAG_WRPERR                  FLASH_SR_WRPERR          /*!< Write Protection Error on flag */
#define FLASH_FLAG_PGSERR                  FLASH_SR_PGSERR          /*!< Program Sequence Error on flag */
#define FLASH_FLAG_STRBERR                 FLASH_SR_STRBERR         /*!< Strobe Error flag */
#define FLASH_FLAG_INCERR                  FLASH_SR_INCERR          /*!< Inconsistency Error on flag */
#if defined (FLASH_SR_OPERR)
#define FLASH_FLAG_OPERR                   FLASH_SR_OPERR           /*!< Operation Error on flag */
#endif /* FLASH_SR_OPERR */
#define FLASH_FLAG_RDPERR                  FLASH_SR_RDPERR          /*!< Read Protection Error on flag */
#define FLASH_FLAG_RDSERR                  FLASH_SR_RDSERR          /*!< Read Secured Error on flag */
#define FLASH_FLAG_SNECCERR                FLASH_SR_SNECCERR        /*!< Single ECC Error Correction on flag */
#define FLASH_FLAG_DBECCERR                FLASH_SR_DBECCERR        /*!< Double Detection ECC Error on flag */
#define FLASH_FLAG_CRCEND                  FLASH_SR_CRCEND          /*!< CRC End of Calculation flag */
#define FLASH_FLAG_CRCRDERR                FLASH_SR_CRCRDERR        /*!< CRC Read Error on bank flag */

#define FLASH_FLAG_BSY_BANK1               FLASH_SR_BSY             /*!< FLASH Bank 1 Busy flag */
#define FLASH_FLAG_WBNE_BANK1              FLASH_SR_WBNE            /*!< Write Buffer Not Empty on Bank 1 flag */
#define FLASH_FLAG_QW_BANK1                FLASH_SR_QW              /*!< Wait Queue on Bank 1 flag */
#define FLASH_FLAG_CRC_BUSY_BANK1          FLASH_SR_CRC_BUSY        /*!< CRC Busy on Bank 1 flag */
#define FLASH_FLAG_EOP_BANK1               FLASH_SR_EOP             /*!< End Of Program on Bank 1 flag */
#define FLASH_FLAG_WRPERR_BANK1            FLASH_SR_WRPERR          /*!< Write Protection Error on Bank 1 flag */
#define FLASH_FLAG_PGSERR_BANK1            FLASH_SR_PGSERR          /*!< Program Sequence Error on Bank 1 flag */
#define FLASH_FLAG_STRBERR_BANK1           FLASH_SR_STRBERR         /*!< Strobe Error on Bank 1 flag */
#define FLASH_FLAG_INCERR_BANK1            FLASH_SR_INCERR          /*!< Inconsistency Error on Bank 1 flag */
#if defined (FLASH_SR_OPERR)
#define FLASH_FLAG_OPERR_BANK1             FLASH_SR_OPERR           /*!< Operation Error on Bank 1 flag */
#endif /* FLASH_SR_OPERR */
#define FLASH_FLAG_RDPERR_BANK1            FLASH_SR_RDPERR          /*!< Read Protection Error on Bank 1 flag */
#define FLASH_FLAG_RDSERR_BANK1            FLASH_SR_RDSERR          /*!< Read Secured Error on Bank 1 flag */
#define FLASH_FLAG_SNECCERR_BANK1          FLASH_SR_SNECCERR        /*!< Single ECC Error Correction on Bank 1 flag */
#define FLASH_FLAG_DBECCERR_BANK1          FLASH_SR_DBECCERR        /*!< Double Detection ECC Error on Bank 1 flag */
#define FLASH_FLAG_CRCEND_BANK1            FLASH_SR_CRCEND          /*!< CRC End of Calculation on Bank 1 flag */
#define FLASH_FLAG_CRCRDERR_BANK1          FLASH_SR_CRCRDERR        /*!< CRC Read error on Bank 1 flag */

#if defined (FLASH_SR_OPERR)
#define FLASH_FLAG_ALL_ERRORS_BANK1       (FLASH_FLAG_WRPERR_BANK1   | FLASH_FLAG_PGSERR_BANK1   | \
                                           FLASH_FLAG_STRBERR_BANK1  | FLASH_FLAG_INCERR_BANK1   | \
                                           FLASH_FLAG_OPERR_BANK1    | FLASH_FLAG_RDPERR_BANK1   | \
                                           FLASH_FLAG_RDSERR_BANK1   | FLASH_FLAG_SNECCERR_BANK1 | \
                                           FLASH_FLAG_DBECCERR_BANK1 | FLASH_FLAG_CRCRDERR_BANK1) /*!< All Bank 1 error flags */
#else
#define FLASH_FLAG_ALL_ERRORS_BANK1       (FLASH_FLAG_WRPERR_BANK1   | FLASH_FLAG_PGSERR_BANK1   | \
                                           FLASH_FLAG_STRBERR_BANK1  | FLASH_FLAG_INCERR_BANK1   | \
                                           FLASH_FLAG_RDPERR_BANK1   | FLASH_FLAG_RDSERR_BANK1   | \
                                           FLASH_FLAG_SNECCERR_BANK1 | FLASH_FLAG_DBECCERR_BANK1 | \
                                           FLASH_FLAG_CRCRDERR_BANK1) /*!< All Bank 1 error flags */
#endif /* FLASH_SR_OPERR */

#define FLASH_FLAG_ALL_BANK1              (FLASH_FLAG_BSY_BANK1      | FLASH_FLAG_WBNE_BANK1     | \
                                           FLASH_FLAG_QW_BANK1       | FLASH_FLAG_CRC_BUSY_BANK1 | \
                                           FLASH_FLAG_EOP_BANK1      | FLASH_FLAG_CRCEND_BANK1   | \
                                           FLASH_FLAG_ALL_ERRORS_BANK1) /*!< All Bank 1 flags */

#define FLASH_FLAG_BSY_BANK2               (FLASH_SR_BSY      | 0x80000000U)        /*!< FLASH Bank 2 Busy flag */
#define FLASH_FLAG_WBNE_BANK2              (FLASH_SR_WBNE     | 0x80000000U)        /*!< Write Buffer Not Empty on Bank 2 flag */
#define FLASH_FLAG_QW_BANK2                (FLASH_SR_QW       | 0x80000000U)        /*!< Wait Queue on Bank 2 flag */
#define FLASH_FLAG_CRC_BUSY_BANK2          (FLASH_SR_CRC_BUSY | 0x80000000U)        /*!< CRC Busy on Bank 2 flag */
#define FLASH_FLAG_EOP_BANK2               (FLASH_SR_EOP      | 0x80000000U)        /*!< End Of Program on Bank 2 flag */
#define FLASH_FLAG_WRPERR_BANK2            (FLASH_SR_WRPERR   | 0x80000000U)        /*!< Write Protection Error on Bank 2 flag */
#define FLASH_FLAG_PGSERR_BANK2            (FLASH_SR_PGSERR   | 0x80000000U)        /*!< Program Sequence Error on Bank 2 flag */
#define FLASH_FLAG_STRBERR_BANK2           (FLASH_SR_STRBERR  | 0x80000000U)        /*!< Strobe Error on Bank 2 flag */
#define FLASH_FLAG_INCERR_BANK2            (FLASH_SR_INCERR   | 0x80000000U)        /*!< Inconsistency Error on Bank 2 flag */
#if defined (FLASH_SR_OPERR)
#define FLASH_FLAG_OPERR_BANK2             (FLASH_SR_OPERR    | 0x80000000U)        /*!< Operation Error on Bank 2 flag */
#endif /* FLASH_SR_OPERR */
#define FLASH_FLAG_RDPERR_BANK2            (FLASH_SR_RDPERR   | 0x80000000U)        /*!< Read Protection Error on Bank 2 flag */
#define FLASH_FLAG_RDSERR_BANK2            (FLASH_SR_RDSERR   | 0x80000000U)        /*!< Read Secured Error on Bank 2 flag */
#define FLASH_FLAG_SNECCERR_BANK2          (FLASH_SR_SNECCERR | 0x80000000U)        /*!< Single ECC Error Correction on Bank 2 flag */
#define FLASH_FLAG_DBECCERR_BANK2          (FLASH_SR_DBECCERR | 0x80000000U)        /*!< Double Detection ECC Error on Bank 2 flag */
#define FLASH_FLAG_CRCEND_BANK2            (FLASH_SR_CRCEND   | 0x80000000U)        /*!< CRC End of Calculation on Bank 2 flag */
#define FLASH_FLAG_CRCRDERR_BANK2          (FLASH_SR_CRCRDERR | 0x80000000U)        /*!< CRC Read error on Bank 2 flag */

#if defined (FLASH_SR_OPERR)
#define FLASH_FLAG_ALL_ERRORS_BANK2       (FLASH_FLAG_WRPERR_BANK2   | FLASH_FLAG_PGSERR_BANK2   | \
                                           FLASH_FLAG_STRBERR_BANK2  | FLASH_FLAG_INCERR_BANK2   | \
                                           FLASH_FLAG_OPERR_BANK2    | FLASH_FLAG_RDPERR_BANK2   | \
                                           FLASH_FLAG_RDSERR_BANK2   | FLASH_FLAG_SNECCERR_BANK2 | \
                                           FLASH_FLAG_DBECCERR_BANK2 | FLASH_FLAG_CRCRDERR_BANK2) /*!< All Bank 2 error flags */
#else
#define FLASH_FLAG_ALL_ERRORS_BANK2       (FLASH_FLAG_WRPERR_BANK2   | FLASH_FLAG_PGSERR_BANK2   | \
                                           FLASH_FLAG_STRBERR_BANK2  | FLASH_FLAG_INCERR_BANK2   | \
                                           FLASH_FLAG_RDPERR_BANK2   | FLASH_FLAG_RDSERR_BANK2   | \
                                           FLASH_FLAG_SNECCERR_BANK2 | FLASH_FLAG_DBECCERR_BANK2 | \
                                           FLASH_FLAG_CRCRDERR_BANK2) /*!< All Bank 2 error flags */
#endif /* FLASH_SR_OPERR */

#define FLASH_FLAG_ALL_BANK2              (FLASH_FLAG_BSY_BANK2      | FLASH_FLAG_WBNE_BANK2     | \
                                           FLASH_FLAG_QW_BANK2       | FLASH_FLAG_CRC_BUSY_BANK2 | \
                                           FLASH_FLAG_EOP_BANK2      | FLASH_FLAG_CRCEND_BANK2   | \
                                           FLASH_FLAG_ALL_ERRORS_BANK2) /*!< All Bank 2 flags */
/**
  * @}
  */

/** @defgroup FLASH_Interrupt_definition FLASH Interrupt definition
  * @brief FLASH Interrupt definition
  * @{
  */
#define FLASH_IT_EOP_BANK1                  FLASH_CR_EOPIE       /*!< End of FLASH Bank 1 Operation Interrupt source */
#define FLASH_IT_WRPERR_BANK1               FLASH_CR_WRPERRIE    /*!< Write Protection Error on Bank 1 Interrupt source */
#define FLASH_IT_PGSERR_BANK1               FLASH_CR_PGSERRIE    /*!< Program Sequence Error on Bank 1 Interrupt source */
#define FLASH_IT_STRBERR_BANK1              FLASH_CR_STRBERRIE   /*!< Strobe Error on Bank 1 Interrupt source */
#define FLASH_IT_INCERR_BANK1               FLASH_CR_INCERRIE    /*!< Inconsistency Error on Bank 1 Interrupt source */
#if defined (FLASH_CR_OPERRIE)
#define FLASH_IT_OPERR_BANK1                FLASH_CR_OPERRIE     /*!< Operation Error on Bank 1 Interrupt source */
#endif /* FLASH_CR_OPERRIE */
#define FLASH_IT_RDPERR_BANK1               FLASH_CR_RDPERRIE    /*!< Read protection Error on Bank 1 Interrupt source */
#define FLASH_IT_RDSERR_BANK1               FLASH_CR_RDSERRIE    /*!< Read Secured Error on Bank 1 Interrupt source */
#define FLASH_IT_SNECCERR_BANK1             FLASH_CR_SNECCERRIE  /*!< Single ECC Error Correction on Bank 1 Interrupt source */
#define FLASH_IT_DBECCERR_BANK1             FLASH_CR_DBECCERRIE  /*!< Double Detection ECC Error on Bank 1 Interrupt source */
#define FLASH_IT_CRCEND_BANK1               FLASH_CR_CRCENDIE    /*!< CRC End on Bank 1 Interrupt source */
#define FLASH_IT_CRCRDERR_BANK1             FLASH_CR_CRCRDERRIE  /*!< CRC Read error on Bank 1 Interrupt source */

#if defined (FLASH_CR_OPERRIE)
#define FLASH_IT_ALL_BANK1                 (FLASH_IT_EOP_BANK1       | FLASH_IT_WRPERR_BANK1    | \
                                            FLASH_IT_PGSERR_BANK1    | FLASH_IT_STRBERR_BANK1   | \
                                            FLASH_IT_INCERR_BANK1    | FLASH_IT_OPERR_BANK1     | \
                                            FLASH_IT_RDPERR_BANK1    | FLASH_IT_RDSERR_BANK1    | \
                                            FLASH_IT_SNECCERR_BANK1  | FLASH_IT_DBECCERR_BANK1  | \
                                            FLASH_IT_CRCEND_BANK1    | FLASH_IT_CRCRDERR_BANK1) /*!< All Bank 1 Interrupt sources */
#else
#define FLASH_IT_ALL_BANK1                 (FLASH_IT_EOP_BANK1       | FLASH_IT_WRPERR_BANK1    | \
                                            FLASH_IT_PGSERR_BANK1    | FLASH_IT_STRBERR_BANK1   | \
                                            FLASH_IT_INCERR_BANK1    | FLASH_IT_RDPERR_BANK1    | \
                                            FLASH_IT_RDSERR_BANK1    | FLASH_IT_SNECCERR_BANK1  | \
                                            FLASH_IT_DBECCERR_BANK1  | FLASH_IT_CRCEND_BANK1    | \
                                            FLASH_IT_CRCRDERR_BANK1) /*!< All Bank 1 Interrupt sources */
#endif /* FLASH_CR_OPERRIE */

#define FLASH_IT_EOP_BANK2                 (FLASH_CR_EOPIE      | 0x80000000U)  /*!< End of FLASH Bank 2 Operation Interrupt source */
#define FLASH_IT_WRPERR_BANK2              (FLASH_CR_WRPERRIE   | 0x80000000U)  /*!< Write Protection Error on Bank 2 Interrupt source */
#define FLASH_IT_PGSERR_BANK2              (FLASH_CR_PGSERRIE   | 0x80000000U)  /*!< Program Sequence Error on Bank 2 Interrupt source */
#define FLASH_IT_STRBERR_BANK2             (FLASH_CR_STRBERRIE  | 0x80000000U)  /*!< Strobe Error on Bank 2 Interrupt source */
#define FLASH_IT_INCERR_BANK2              (FLASH_CR_INCERRIE   | 0x80000000U)  /*!< Inconsistency Error on Bank 2 Interrupt source */
#if defined (FLASH_CR_OPERRIE)
#define FLASH_IT_OPERR_BANK2               (FLASH_CR_OPERRIE    | 0x80000000U)  /*!< Operation Error on Bank 2 Interrupt source */
#endif /* FLASH_CR_OPERRIE */
#define FLASH_IT_RDPERR_BANK2              (FLASH_CR_RDPERRIE   | 0x80000000U)  /*!< Read protection Error on Bank 2 Interrupt source */
#define FLASH_IT_RDSERR_BANK2              (FLASH_CR_RDSERRIE   | 0x80000000U)  /*!< Read Secured Error on Bank 2 Interrupt source */
#define FLASH_IT_SNECCERR_BANK2            (FLASH_CR_SNECCERRIE | 0x80000000U)  /*!< Single ECC Error Correction on Bank 2 Interrupt source */
#define FLASH_IT_DBECCERR_BANK2            (FLASH_CR_DBECCERRIE | 0x80000000U)  /*!< Double Detection ECC Error on Bank 2 Interrupt source */
#define FLASH_IT_CRCEND_BANK2              (FLASH_CR_CRCENDIE   | 0x80000000U)  /*!< CRC End on Bank 2 Interrupt source */
#define FLASH_IT_CRCRDERR_BANK2            (FLASH_CR_CRCRDERRIE | 0x80000000U)  /*!< CRC Read Error on Bank 2 Interrupt source */

#if defined (FLASH_CR_OPERRIE)
#define FLASH_IT_ALL_BANK2                 (FLASH_IT_EOP_BANK2       | FLASH_IT_WRPERR_BANK2    | \
                                            FLASH_IT_PGSERR_BANK2    | FLASH_IT_STRBERR_BANK2   | \
                                            FLASH_IT_INCERR_BANK2    | FLASH_IT_OPERR_BANK2     | \
                                            FLASH_IT_RDPERR_BANK2    | FLASH_IT_RDSERR_BANK2    | \
                                            FLASH_IT_SNECCERR_BANK2  | FLASH_IT_DBECCERR_BANK2  | \
                                            FLASH_IT_CRCEND_BANK2    | FLASH_IT_CRCRDERR_BANK2) /*!< All Bank 2 Interrupt sources */
#else
#define FLASH_IT_ALL_BANK2                 (FLASH_IT_EOP_BANK2       | FLASH_IT_WRPERR_BANK2    | \
                                            FLASH_IT_PGSERR_BANK2    | FLASH_IT_STRBERR_BANK2   | \
                                            FLASH_IT_INCERR_BANK2    | FLASH_IT_RDPERR_BANK2    | \
                                            FLASH_IT_RDSERR_BANK2    | FLASH_IT_SNECCERR_BANK2  | \
                                            FLASH_IT_DBECCERR_BANK2  | FLASH_IT_CRCEND_BANK2    | \
                                            FLASH_IT_CRCRDERR_BANK2) /*!< All Bank 2 Interrupt sources */
#endif /* FLASH_CR_OPERRIE */
/**
  * @}
  */

#if defined (FLASH_CR_PSIZE)
/** @defgroup FLASH_Program_Parallelism FLASH Program Parallelism
  * @{
  */
#define FLASH_PSIZE_BYTE           0x00000000U       /*!< Flash program/erase by 8 bits  */
#define FLASH_PSIZE_HALF_WORD      FLASH_CR_PSIZE_0  /*!< Flash program/erase by 16 bits */
#define FLASH_PSIZE_WORD           FLASH_CR_PSIZE_1  /*!< Flash program/erase by 32 bits */
#define FLASH_PSIZE_DOUBLE_WORD    FLASH_CR_PSIZE    /*!< Flash program/erase by 64 bits */
/**
  * @}
  */
#endif /* FLASH_CR_PSIZE */


/** @defgroup FLASH_Keys FLASH Keys
  * @{
  */
#define FLASH_KEY1                 0x45670123U
#define FLASH_KEY2                 0xCDEF89ABU
#define FLASH_OPT_KEY1             0x08192A3BU
#define FLASH_OPT_KEY2             0x4C5D6E7FU
/**
  * @}
  */

/** @defgroup FLASH_Sectors FLASH Sectors
  * @{
  */
#define FLASH_SECTOR_0             0U       /*!< Sector Number 0   */
#define FLASH_SECTOR_1             1U       /*!< Sector Number 1   */
#define FLASH_SECTOR_2             2U       /*!< Sector Number 2   */
#define FLASH_SECTOR_3             3U       /*!< Sector Number 3   */
#define FLASH_SECTOR_4             4U       /*!< Sector Number 4   */
#define FLASH_SECTOR_5             5U       /*!< Sector Number 5   */
#define FLASH_SECTOR_6             6U       /*!< Sector Number 6   */
#define FLASH_SECTOR_7             7U       /*!< Sector Number 7   */
#if (FLASH_SECTOR_TOTAL == 128)
#define FLASH_SECTOR_8             8U       /*!< Sector Number 8   */
#define FLASH_SECTOR_9             9U       /*!< Sector Number 9   */
#define FLASH_SECTOR_10            10U      /*!< Sector Number 10  */
#define FLASH_SECTOR_11            11U      /*!< Sector Number 11  */
#define FLASH_SECTOR_12            12U      /*!< Sector Number 12  */
#define FLASH_SECTOR_13            13U      /*!< Sector Number 13  */
#define FLASH_SECTOR_14            14U      /*!< Sector Number 14  */
#define FLASH_SECTOR_15            15U      /*!< Sector Number 15  */
#define FLASH_SECTOR_16            16U      /*!< Sector Number 16  */
#define FLASH_SECTOR_17            17U      /*!< Sector Number 17  */
#define FLASH_SECTOR_18            18U      /*!< Sector Number 18  */
#define FLASH_SECTOR_19            19U      /*!< Sector Number 19  */
#define FLASH_SECTOR_20            20U      /*!< Sector Number 20  */
#define FLASH_SECTOR_21            21U      /*!< Sector Number 21  */
#define FLASH_SECTOR_22            22U      /*!< Sector Number 22  */
#define FLASH_SECTOR_23            23U      /*!< Sector Number 23  */
#define FLASH_SECTOR_24            24U      /*!< Sector Number 24  */
#define FLASH_SECTOR_25            25U      /*!< Sector Number 25  */
#define FLASH_SECTOR_26            26U      /*!< Sector Number 26  */
#define FLASH_SECTOR_27            27U      /*!< Sector Number 27  */
#define FLASH_SECTOR_28            28U      /*!< Sector Number 28  */
#define FLASH_SECTOR_29            29U      /*!< Sector Number 29  */
#define FLASH_SECTOR_30            30U      /*!< Sector Number 30  */
#define FLASH_SECTOR_31            31U      /*!< Sector Number 31  */
#define FLASH_SECTOR_32            32U      /*!< Sector Number 32  */
#define FLASH_SECTOR_33            33U      /*!< Sector Number 33  */
#define FLASH_SECTOR_34            34U      /*!< Sector Number 34  */
#define FLASH_SECTOR_35            35U      /*!< Sector Number 35  */
#define FLASH_SECTOR_36            36U      /*!< Sector Number 36  */
#define FLASH_SECTOR_37            37U      /*!< Sector Number 37  */
#define FLASH_SECTOR_38            38U      /*!< Sector Number 38  */
#define FLASH_SECTOR_39            39U      /*!< Sector Number 39  */
#define FLASH_SECTOR_40            40U      /*!< Sector Number 40  */
#define FLASH_SECTOR_41            41U      /*!< Sector Number 41  */
#define FLASH_SECTOR_42            42U      /*!< Sector Number 42  */
#define FLASH_SECTOR_43            43U      /*!< Sector Number 43  */
#define FLASH_SECTOR_44            44U      /*!< Sector Number 44  */
#define FLASH_SECTOR_45            45U      /*!< Sector Number 45  */
#define FLASH_SECTOR_46            46U      /*!< Sector Number 46  */
#define FLASH_SECTOR_47            47U      /*!< Sector Number 47  */
#define FLASH_SECTOR_48            48U      /*!< Sector Number 48  */
#define FLASH_SECTOR_49            49U      /*!< Sector Number 49  */
#define FLASH_SECTOR_50            50U      /*!< Sector Number 50  */
#define FLASH_SECTOR_51            51U      /*!< Sector Number 51  */
#define FLASH_SECTOR_52            52U      /*!< Sector Number 52  */
#define FLASH_SECTOR_53            53U      /*!< Sector Number 53  */
#define FLASH_SECTOR_54            54U      /*!< Sector Number 54  */
#define FLASH_SECTOR_55            55U      /*!< Sector Number 55  */
#define FLASH_SECTOR_56            56U      /*!< Sector Number 56  */
#define FLASH_SECTOR_57            57U      /*!< Sector Number 57  */
#define FLASH_SECTOR_58            58U      /*!< Sector Number 58  */
#define FLASH_SECTOR_59            59U      /*!< Sector Number 59  */
#define FLASH_SECTOR_60            60U      /*!< Sector Number 60  */
#define FLASH_SECTOR_61            61U      /*!< Sector Number 61  */
#define FLASH_SECTOR_62            62U      /*!< Sector Number 62  */
#define FLASH_SECTOR_63            63U      /*!< Sector Number 63  */
#define FLASH_SECTOR_64            64U      /*!< Sector Number 64  */
#define FLASH_SECTOR_65            65U      /*!< Sector Number 65  */
#define FLASH_SECTOR_66            66U      /*!< Sector Number 66  */
#define FLASH_SECTOR_67            67U      /*!< Sector Number 67  */
#define FLASH_SECTOR_68            68U      /*!< Sector Number 68  */
#define FLASH_SECTOR_69            69U      /*!< Sector Number 69  */
#define FLASH_SECTOR_70            70U      /*!< Sector Number 70  */
#define FLASH_SECTOR_71            71U      /*!< Sector Number 71  */
#define FLASH_SECTOR_72            72U      /*!< Sector Number 72  */
#define FLASH_SECTOR_73            73U      /*!< Sector Number 73  */
#define FLASH_SECTOR_74            74U      /*!< Sector Number 74  */
#define FLASH_SECTOR_75            75U      /*!< Sector Number 75  */
#define FLASH_SECTOR_76            76U      /*!< Sector Number 76  */
#define FLASH_SECTOR_77            77U      /*!< Sector Number 77  */
#define FLASH_SECTOR_78            78U      /*!< Sector Number 78  */
#define FLASH_SECTOR_79            79U      /*!< Sector Number 79  */
#define FLASH_SECTOR_80            80U      /*!< Sector Number 80  */
#define FLASH_SECTOR_81            81U      /*!< Sector Number 81  */
#define FLASH_SECTOR_82            82U      /*!< Sector Number 82  */
#define FLASH_SECTOR_83            83U      /*!< Sector Number 83  */
#define FLASH_SECTOR_84            84U      /*!< Sector Number 84  */
#define FLASH_SECTOR_85            85U      /*!< Sector Number 85  */
#define FLASH_SECTOR_86            86U      /*!< Sector Number 86  */
#define FLASH_SECTOR_87            87U      /*!< Sector Number 87  */
#define FLASH_SECTOR_88            88U      /*!< Sector Number 88  */
#define FLASH_SECTOR_89            89U      /*!< Sector Number 89  */
#define FLASH_SECTOR_90            90U      /*!< Sector Number 90  */
#define FLASH_SECTOR_91            91U      /*!< Sector Number 91  */
#define FLASH_SECTOR_92            92U      /*!< Sector Number 92  */
#define FLASH_SECTOR_93            93U      /*!< Sector Number 93  */
#define FLASH_SECTOR_94            94U      /*!< Sector Number 94  */
#define FLASH_SECTOR_95            95U      /*!< Sector Number 95  */
#define FLASH_SECTOR_96            96U      /*!< Sector Number 96  */
#define FLASH_SECTOR_97            97U      /*!< Sector Number 97  */
#define FLASH_SECTOR_98            98U      /*!< Sector Number 98  */
#define FLASH_SECTOR_99            99U      /*!< Sector Number 99  */
#define FLASH_SECTOR_100           100U     /*!< Sector Number 100 */
#define FLASH_SECTOR_101           101U     /*!< Sector Number 101 */
#define FLASH_SECTOR_102           102U     /*!< Sector Number 102 */
#define FLASH_SECTOR_103           103U     /*!< Sector Number 103 */
#define FLASH_SECTOR_104           104U     /*!< Sector Number 104 */
#define FLASH_SECTOR_105           105U     /*!< Sector Number 105 */
#define FLASH_SECTOR_106           106U     /*!< Sector Number 106 */
#define FLASH_SECTOR_107           107U     /*!< Sector Number 107 */
#define FLASH_SECTOR_108           108U     /*!< Sector Number 108 */
#define FLASH_SECTOR_109           109U     /*!< Sector Number 109 */
#define FLASH_SECTOR_110           110U     /*!< Sector Number 110 */
#define FLASH_SECTOR_111           111U     /*!< Sector Number 111 */
#define FLASH_SECTOR_112           112U     /*!< Sector Number 112 */
#define FLASH_SECTOR_113           113U     /*!< Sector Number 113 */
#define FLASH_SECTOR_114           114U     /*!< Sector Number 114 */
#define FLASH_SECTOR_115           115U     /*!< Sector Number 115 */
#define FLASH_SECTOR_116           116U     /*!< Sector Number 116 */
#define FLASH_SECTOR_117           117U     /*!< Sector Number 117 */
#define FLASH_SECTOR_118           118U     /*!< Sector Number 118 */
#define FLASH_SECTOR_119           119U     /*!< Sector Number 119 */
#define FLASH_SECTOR_120           120U     /*!< Sector Number 120 */
#define FLASH_SECTOR_121           121U     /*!< Sector Number 121 */
#define FLASH_SECTOR_122           122U     /*!< Sector Number 122 */
#define FLASH_SECTOR_123           123U     /*!< Sector Number 123 */
#define FLASH_SECTOR_124           124U     /*!< Sector Number 124 */
#define FLASH_SECTOR_125           125U     /*!< Sector Number 125 */
#define FLASH_SECTOR_126           126U     /*!< Sector Number 126 */
#define FLASH_SECTOR_127           127U     /*!< Sector Number 127 */
#endif /* FLASH_SECTOR_TOTAL == 128 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup FLASH_Exported_Macros FLASH Exported Macros
  * @{
  */
/**
  * @brief  Set the FLASH Latency.
  * @param  __LATENCY__: FLASH Latency
  *         The value of this parameter depend on device used within the same series
  * @retval none
  */
#define __HAL_FLASH_SET_LATENCY(__LATENCY__) \
                  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, (uint32_t)(__LATENCY__))

/**
  * @brief  Get the FLASH Latency.
  * @retval FLASH Latency
  *          The value of this parameter depend on device used within the same series
  */
#define __HAL_FLASH_GET_LATENCY()     (READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY))

/**
  * @brief  Enable the specified FLASH interrupt.
  * @param  __INTERRUPT__ : FLASH interrupt
  *   In case of Bank 1 This parameter can be any combination of the following values:
  *     @arg FLASH_IT_EOP_BANK1       : End of FLASH Bank 1 Operation Interrupt source
  *     @arg FLASH_IT_WRPERR_BANK1    : Write Protection Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_PGSERR_BANK1    : Program Sequence Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_STRBERR_BANK1   : Strobe Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_INCERR_BANK1    : Inconsistency Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_OPERR_BANK1     : Operation Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_RDPERR_BANK1    : Read protection Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_RDSERR_BANK1    : Read secure Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_SNECCERR_BANK1  : Single ECC Error Correction on Bank 1 Interrupt source
  *     @arg FLASH_IT_DBECCERR_BANK1  : Double Detection ECC Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_CRCEND_BANK1    : CRC End on Bank 1 Interrupt source
  *     @arg FLASH_IT_CRCRDERR_BANK1  : CRC Read error on Bank 1 Interrupt source
  *     @arg FLASH_IT_ALL_BANK1       : All Bank 1 Interrupt sources
  *
  *   In case of Bank 2, this parameter can be any combination of the following values:
  *     @arg FLASH_IT_EOP_BANK2       : End of FLASH Bank 2 Operation Interrupt source
  *     @arg FLASH_IT_WRPERR_BANK2    : Write Protection Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_PGSERR_BANK2    : Program Sequence Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_STRBERR_BANK2   : Strobe Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_INCERR_BANK2    : Inconsistency Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_OPERR_BANK2     : Operation Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_RDPERR_BANK2    : Read protection Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_RDSERR_BANK2    : Read secure Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_SNECCERR_BANK2  : Single ECC Error Correction on Bank 2 Interrupt source
  *     @arg FLASH_IT_DBECCERR_BANK2  : Double Detection ECC Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_CRCEND_BANK2    : CRC End on Bank 2 Interrupt source
  *     @arg FLASH_IT_CRCRDERR_BANK2  : CRC Read error on Bank 2 Interrupt source
  *     @arg FLASH_IT_ALL_BANK2       : All Bank 2 Interrupt sources
  * @retval none
  */

#define __HAL_FLASH_ENABLE_IT_BANK1(__INTERRUPT__)      (FLASH->CR1 |= (__INTERRUPT__))

#define __HAL_FLASH_ENABLE_IT_BANK2(__INTERRUPT__)      (FLASH->CR2 |= ((__INTERRUPT__) & 0x7FFFFFFFU))

#define __HAL_FLASH_ENABLE_IT(__INTERRUPT__)    (IS_FLASH_IT_BANK1(__INTERRUPT__) ? \
                                                 __HAL_FLASH_ENABLE_IT_BANK1(__INTERRUPT__) : \
                                                 __HAL_FLASH_ENABLE_IT_BANK2(__INTERRUPT__))


/**
  * @brief  Disable the specified FLASH interrupt.
  * @param  __INTERRUPT__ : FLASH interrupt
  *   In case of Bank 1 This parameter can be any combination of the following values:
  *     @arg FLASH_IT_EOP_BANK1       : End of FLASH Bank 1 Operation Interrupt source
  *     @arg FLASH_IT_WRPERR_BANK1    : Write Protection Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_PGSERR_BANK1    : Program Sequence Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_STRBERR_BANK1   : Strobe Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_INCERR_BANK1    : Inconsistency Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_OPERR_BANK1     : Operation Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_RDPERR_BANK1    : Read protection Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_RDSERR_BANK1    : Read secure Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_SNECCERR_BANK1  : Single ECC Error Correction on Bank 1 Interrupt source
  *     @arg FLASH_IT_DBECCERR_BANK1  : Double Detection ECC Error on Bank 1 Interrupt source
  *     @arg FLASH_IT_CRCEND_BANK1    : CRC End on Bank 1 Interrupt source
  *     @arg FLASH_IT_CRCRDERR_BANK1  : CRC Read error on Bank 1 Interrupt source
  *     @arg FLASH_IT_ALL_BANK1       : All Bank 1 Interrupt sources
  *
  *   In case of Bank 2, this parameter can be any combination of the following values:
  *     @arg FLASH_IT_EOP_BANK2       : End of FLASH Bank 2 Operation Interrupt source
  *     @arg FLASH_IT_WRPERR_BANK2    : Write Protection Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_PGSERR_BANK2    : Program Sequence Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_STRBERR_BANK2   : Strobe Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_INCERR_BANK2    : Inconsistency Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_OPERR_BANK2     : Operation Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_RDPERR_BANK2    : Read protection Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_RDSERR_BANK2    : Read secure Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_SNECCERR_BANK2  : Single ECC Error Correction on Bank 2 Interrupt source
  *     @arg FLASH_IT_DBECCERR_BANK2  : Double Detection ECC Error on Bank 2 Interrupt source
  *     @arg FLASH_IT_CRCEND_BANK2    : CRC End on Bank 2 Interrupt source
  *     @arg FLASH_IT_CRCRDERR_BANK2  : CRC Read error on Bank 2 Interrupt source
  *     @arg FLASH_IT_ALL_BANK2       : All Bank 2 Interrupt sources
  * @retval none
  */

#define __HAL_FLASH_DISABLE_IT_BANK1(__INTERRUPT__)  (FLASH->CR1 &= ~(uint32_t)(__INTERRUPT__))

#define __HAL_FLASH_DISABLE_IT_BANK2(__INTERRUPT__)  (FLASH->CR2 &= ~(uint32_t)((__INTERRUPT__) & 0x7FFFFFFFU))

#define __HAL_FLASH_DISABLE_IT(__INTERRUPT__)  (IS_FLASH_IT_BANK1(__INTERRUPT__) ? \
                                                __HAL_FLASH_DISABLE_IT_BANK1(__INTERRUPT__) : \
                                                __HAL_FLASH_DISABLE_IT_BANK2(__INTERRUPT__))


/**
  * @brief  Checks whether the specified FLASH flag is set or not.
  * @param  __FLAG__: specifies the FLASH flag to check.
  *   In case of Bank 1 This parameter can be one of the following values :
  *     @arg FLASH_FLAG_BSY_BANK1      : FLASH Bank 1 Busy flag
  *     @arg FLASH_FLAG_WBNE_BANK1     : Write Buffer Not Empty on Bank 1 flag
  *     @arg FLASH_FLAG_QW_BANK1       : Wait Queue on Bank 1 flag
  *     @arg FLASH_FLAG_CRC_BUSY_BANK1 : CRC module is working on Bank 1 flag
  *     @arg FLASH_FLAG_EOP_BANK1      : End Of Program on Bank 1 flag
  *     @arg FLASH_FLAG_WRPERR_BANK1   : Write Protection Error on Bank 1 flag
  *     @arg FLASH_FLAG_PGSERR_BANK1   : Program Sequence Error on Bank 1 flag
  *     @arg FLASH_FLAG_STRBER_BANK1   : Program Alignment Error on Bank 1 flag
  *     @arg FLASH_FLAG_INCERR_BANK1   : Inconsistency Error on Bank 1 flag
  *     @arg FLASH_FLAG_OPERR_BANK1    : Operation Error on Bank 1 flag
  *     @arg FLASH_FLAG_RDPERR_BANK1   : Read Protection Error on Bank 1 flag
  *     @arg FLASH_FLAG_RDSERR_BANK1   : Read secure  Error on Bank 1 flag
  *     @arg FLASH_FLAG_SNECCE_BANK1   : Single ECC Error Correction on Bank 1 flag
  *     @arg FLASH_FLAG_DBECCE_BANK1   : Double Detection ECC Error on Bank 1 flag
  *     @arg FLASH_FLAG_CRCEND_BANK1   : CRC End on Bank 1 flag
  *     @arg FLASH_FLAG_CRCRDERR_BANK1 : CRC Read error on Bank 1 flag
  *
  *   In case of Bank 2 This parameter can be one of the following values :
  *     @arg FLASH_FLAG_BSY_BANK2      : FLASH Bank 2 Busy flag
  *     @arg FLASH_FLAG_WBNE_BANK2     : Write Buffer Not Empty on Bank 2 flag
  *     @arg FLASH_FLAG_QW_BANK2       : Wait Queue on Bank 2 flag
  *     @arg FLASH_FLAG_CRC_BUSY_BANK2 : CRC module is working on Bank 2 flag
  *     @arg FLASH_FLAG_EOP_BANK2      : End Of Program on Bank 2 flag
  *     @arg FLASH_FLAG_WRPERR_BANK2   : Write Protection Error on Bank 2 flag
  *     @arg FLASH_FLAG_PGSERR_BANK2   : Program Sequence Error on Bank 2 flag
  *     @arg FLASH_FLAG_STRBER_BANK2   : Program Alignment Error on Bank 2 flag
  *     @arg FLASH_FLAG_INCERR_BANK2   : Inconsistency Error on Bank 2 flag
  *     @arg FLASH_FLAG_OPERR_BANK2    : Operation Error on Bank 2 flag
  *     @arg FLASH_FLAG_RDPERR_BANK2   : Read Protection Error on Bank 2 flag
  *     @arg FLASH_FLAG_RDSERR_BANK2   : Read secure  Error on Bank 2 flag
  *     @arg FLASH_FLAG_SNECCE_BANK2   : Single ECC Error Correction on Bank 2 flag
  *     @arg FLASH_FLAG_DBECCE_BANK2   : Double Detection ECC Error on Bank 2 flag
  *     @arg FLASH_FLAG_CRCEND_BANK2   : CRC End on Bank 2 flag
  *     @arg FLASH_FLAG_CRCRDERR_BANK2 : CRC Read error on Bank 2 flag
  * @retval The new state of FLASH_FLAG (SET or RESET).
  */
#define __HAL_FLASH_GET_FLAG_BANK1(__FLAG__)     (READ_BIT(FLASH->SR1, (__FLAG__)) == (__FLAG__))

#define __HAL_FLASH_GET_FLAG_BANK2(__FLAG__)     (READ_BIT(FLASH->SR2, ((__FLAG__) & 0x7FFFFFFFU)) == (((__FLAG__) & 0x7FFFFFFFU)))

#define __HAL_FLASH_GET_FLAG(__FLAG__)           (IS_FLASH_FLAG_BANK1(__FLAG__) ?  __HAL_FLASH_GET_FLAG_BANK1(__FLAG__) : \
                                                  __HAL_FLASH_GET_FLAG_BANK2(__FLAG__))


/**
  * @brief  Clear the specified FLASH flag.
  * @param  __FLAG__: specifies the FLASH flags to clear.
  *    In case of Bank 1, this parameter can be any combination of the following values:
  *     @arg FLASH_FLAG_EOP_BANK1        : End Of Program on Bank 1 flag
  *     @arg FLASH_FLAG_WRPERR_BANK1     : Write Protection Error on Bank 1 flag
  *     @arg FLASH_FLAG_PGSERR_BANK1     : Program Sequence Error on Bank 1 flag
  *     @arg FLASH_FLAG_STRBER_BANK1     : Program Alignment Error on Bank 1 flag
  *     @arg FLASH_FLAG_INCERR_BANK1     : Inconsistency Error on Bank 1 flag
  *     @arg FLASH_FLAG_OPERR_BANK1      : Operation Error on Bank 1 flag
  *     @arg FLASH_FLAG_RDPERR_BANK1     : Read Protection Error on Bank 1 flag
  *     @arg FLASH_FLAG_RDSERR_BANK1     : Read secure  Error on Bank 1 flag
  *     @arg FLASH_FLAG_SNECCE_BANK1     : Single ECC Error Correction on Bank 1 flag
  *     @arg FLASH_FLAG_DBECCE_BANK1     : Double Detection ECC Error on Bank 1 flag
  *     @arg FLASH_FLAG_CRCEND_BANK1     : CRC End on Bank 1 flag
  *     @arg FLASH_FLAG_CRCRDERR_BANK1   : CRC Read error on Bank 1 flag
  *     @arg FLASH_FLAG_ALL_ERRORS_BANK1 : All Bank 1 error flags
  *     @arg FLASH_FLAG_ALL_BANK1        : All Bank 1 flags
  *
  *   In case of Bank 2, this parameter can be any combination of the following values :
  *     @arg FLASH_FLAG_EOP_BANK2        : End Of Program on Bank 2 flag
  *     @arg FLASH_FLAG_WRPERR_BANK2     : Write Protection Error on Bank 2 flag
  *     @arg FLASH_FLAG_PGSERR_BANK2     : Program Sequence Error on Bank 2 flag
  *     @arg FLASH_FLAG_STRBER_BANK2     : Program Alignment Error on Bank 2 flag
  *     @arg FLASH_FLAG_INCERR_BANK2     : Inconsistency Error on Bank 2 flag
  *     @arg FLASH_FLAG_OPERR_BANK2      : Operation Error on Bank 2 flag
  *     @arg FLASH_FLAG_RDPERR_BANK2     : Read Protection Error on Bank 2 flag
  *     @arg FLASH_FLAG_RDSERR_BANK2     : Read secure  Error on Bank 2 flag
  *     @arg FLASH_FLAG_SNECCE_BANK2     : Single ECC Error Correction on Bank 2 flag
  *     @arg FLASH_FLAG_DBECCE_BANK2     : Double Detection ECC Error on Bank 2 flag
  *     @arg FLASH_FLAG_CRCEND_BANK2     : CRC End on Bank 2 flag
  *     @arg FLASH_FLAG_CRCRDERR_BANK2   : CRC Read error on Bank 2 flag
  *     @arg FLASH_FLAG_ALL_ERRORS_BANK2 : All Bank 2 error flags
  *     @arg FLASH_FLAG_ALL_BANK2        : All Bank 2 flags
  * @retval none
  */

#define __HAL_FLASH_CLEAR_FLAG_BANK1(__FLAG__)    WRITE_REG(FLASH->CCR1, (__FLAG__))

#define __HAL_FLASH_CLEAR_FLAG_BANK2(__FLAG__)    WRITE_REG(FLASH->CCR2, ((__FLAG__) & 0x7FFFFFFFU))

#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)         (IS_FLASH_FLAG_BANK1(__FLAG__) ?  __HAL_FLASH_CLEAR_FLAG_BANK1(__FLAG__) : \
                                                   __HAL_FLASH_CLEAR_FLAG_BANK2(__FLAG__))

/**
  * @}
  */

/* Include FLASH HAL Extension module */
#include "stm32h7xx_hal_flash_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup FLASH_Exported_Functions
  * @{
  */
/** @addtogroup FLASH_Exported_Functions_Group1
  * @{
  */
/* Program operation functions  ***********************************************/
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress);
/* FLASH IRQ handler method */
void HAL_FLASH_IRQHandler(void);
/* Callbacks in non blocking modes */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
/**
  * @}
  */

/** @addtogroup FLASH_Exported_Functions_Group2
  * @{
  */
/* Peripheral Control functions  **********************************************/
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
/* Option bytes control */
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);
/**
  * @}
  */

/** @addtogroup FLASH_Exported_Functions_Group3
  * @{
  */
/* Peripheral State functions  ************************************************/
uint32_t HAL_FLASH_GetError(void);
/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Variables FLASH Private Variables
  * @{
  */
extern FLASH_ProcessTypeDef pFlash;
/**
  * @}
  */
/* Private constants ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Constants FLASH Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup FLASH_Private_Macros FLASH Private Macros
  * @{
  */

#if defined (FLASH_OPTCR_PG_OTP)
#define IS_FLASH_TYPEPROGRAM(VALUE)      (((VALUE) == FLASH_TYPEPROGRAM_FLASHWORD) || \
                                          ((VALUE) == FLASH_TYPEPROGRAM_OTPWORD))
#else
#define IS_FLASH_TYPEPROGRAM(VALUE)      ((VALUE) == FLASH_TYPEPROGRAM_FLASHWORD)
#endif /* FLASH_OPTCR_PG_OTP */

#define IS_FLASH_IT_BANK1(IT)            (((IT) & FLASH_IT_ALL_BANK1) == (IT))

#define IS_FLASH_IT_BANK2(IT)            (((IT) & FLASH_IT_ALL_BANK2) == (IT))

#define IS_FLASH_FLAG_BANK1(FLAG)        (((FLAG) & FLASH_FLAG_ALL_BANK1) == (FLAG))

#define IS_FLASH_FLAG_BANK2(FLAG)        (((FLAG) & FLASH_FLAG_ALL_BANK2) == (FLAG))

#define IS_FLASH_PROGRAM_ADDRESS_BANK1(ADDRESS) (((ADDRESS) >= FLASH_BANK1_BASE) && ((ADDRESS) < FLASH_BANK2_BASE))
#define IS_FLASH_PROGRAM_ADDRESS_BANK2(ADDRESS) (((ADDRESS) >= FLASH_BANK2_BASE ) && ((ADDRESS) <= FLASH_END))

#if defined (FLASH_OPTCR_PG_OTP)
#define IS_FLASH_PROGRAM_ADDRESS_OTP(ADDRESS)   (((ADDRESS) >= 0x08FFF000U) && ((ADDRESS) <= 0x08FFF3FFU))
#define IS_FLASH_PROGRAM_ADDRESS(ADDRESS)       (IS_FLASH_PROGRAM_ADDRESS_BANK1(ADDRESS) || \
                                                 IS_FLASH_PROGRAM_ADDRESS_BANK2(ADDRESS) || \
                                                 IS_FLASH_PROGRAM_ADDRESS_OTP(ADDRESS))
#else
#define IS_FLASH_PROGRAM_ADDRESS(ADDRESS)       (IS_FLASH_PROGRAM_ADDRESS_BANK1(ADDRESS) || \
                                                 IS_FLASH_PROGRAM_ADDRESS_BANK2(ADDRESS))
#endif /* FLASH_OPTCR_PG_OTP */

#define IS_BOOT_ADDRESS(ADDRESS)           ((ADDRESS) <= (0x3FFF0000U))

#define IS_FLASH_BANK(BANK)                (((BANK) == FLASH_BANK_1)  || \
                                            ((BANK) == FLASH_BANK_2)  || \
                                            ((BANK) == FLASH_BANK_BOTH))

#define IS_FLASH_BANK_EXCLUSIVE(BANK)      (((BANK) == FLASH_BANK_1)  || \
                                            ((BANK) == FLASH_BANK_2))


/**
  * @}
  */
/* Private functions ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Functions FLASH Private functions
  * @{
  */
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout, uint32_t Bank);
HAL_StatusTypeDef FLASH_OB_WaitForLastOperation(uint32_t Timeout);
HAL_StatusTypeDef FLASH_CRC_WaitForLastOperation(uint32_t Timeout, uint32_t Bank);
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

#endif /* STM32H7xx_HAL_FLASH_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
