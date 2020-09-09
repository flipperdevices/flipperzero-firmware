/**
  ******************************************************************************
  * @file    stm32H7xx_hal_flash_ex.h
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
#ifndef STM32H7xx_HAL_FLASH_EX_H
#define STM32H7xx_HAL_FLASH_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup FLASHEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup FLASHEx_Exported_Types FLASH Exported Types
  * @{
  */

/**
  * @brief  FLASH Erase structure definition
  */
typedef struct
{
  uint32_t TypeErase;   /*!< Mass erase or sector Erase.
                             This parameter can be a value of @ref FLASHEx_Type_Erase */

  uint32_t Banks;       /*!< Select banks to erase when Mass erase is enabled.
                             This parameter must be a value of @ref FLASHEx_Banks */

  uint32_t Sector;      /*!< Initial FLASH sector to erase when Mass erase is disabled
                             This parameter must be a value of @ref FLASH_Sectors */

  uint32_t NbSectors;   /*!< Number of sectors to be erased.
                             This parameter must be a value between 1 and (max number of sectors - value of Initial sector)*/

  uint32_t VoltageRange;/*!< The device voltage range which defines the erase parallelism
                             This parameter must be a value of @ref FLASHEx_Voltage_Range */

} FLASH_EraseInitTypeDef;


/**
  * @brief  FLASH Option Bytes Program structure definition
  */
typedef struct
{
  uint32_t OptionType;     /*!< Option byte to be configured.
                                This parameter can be a value of @ref FLASHEx_Option_Type */

  uint32_t WRPState;       /*!< Write protection activation or deactivation.
                                This parameter can be a value of @ref FLASHEx_WRP_State */

  uint32_t WRPSector;      /*!< Specifies the sector(s) to be write protected.
                                The value of this parameter depend on device used within the same series */

  uint32_t RDPLevel;       /*!< Set the read protection level.
                                This parameter can be a value of @ref FLASHEx_Option_Bytes_Read_Protection */

  uint32_t BORLevel;       /*!< Set the BOR Level.
                                This parameter can be a value of @ref FLASHEx_BOR_Reset_Level */

  uint32_t USERType;       /*!< User option byte(s) to be configured (used for OPTIONBYTE_USER).
                                This parameter can be a combination of @ref FLASHEx_OB_USER_Type */

  uint32_t USERConfig;     /*!< Program the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY /
                                IWDG_FREEZE_STOP / IWDG_FREEZE_SANDBY / IO_HSLV / SWAP_BANK_OPT */

  uint32_t Banks;          /*!< Select banks for WRP , PCROP and secure area config .
                                This parameter must be a value of @ref FLASHEx_Banks */

  uint32_t PCROPConfig;    /*!< specifies if the PCROP area shall be erased or not
                                when RDP level decreased from Level 1 to Level 0 or during a mass erase.
                                This parameter must be a value of @ref FLASHEx_OB_PCROP_RDP enumeration */

  uint32_t PCROPStartAddr; /*!< PCROP Start address (used for OPTIONBYTE_PCROP).
                                This parameter must be a value between begin and end of a bank */

  uint32_t PCROPEndAddr;   /*!< PCROP End address (used for OPTIONBYTE_PCROP).
                                This parameter must be a value between PCROP Start address and end of a bank */

  uint32_t BootConfig;     /*!< Specifies if the Boot Address to be configured BOOT_ADD0, BOOT_ADD1
                                or both. This parameter must be a value of @ref FLASHEx_OB_BOOT_OPTION enumeration */

  uint32_t BootAddr0;      /*!< Boot Address 0.
                                This parameter must be a value between begin and end of a bank */

  uint32_t BootAddr1;      /*!< Boot Address 1.
                                This parameter must be a value between begin and end of a bank */
#if defined(DUAL_CORE)
  uint32_t CM4BootConfig;  /*!< specifies if the CM4 boot Address to be configured BOOT_ADD0, BOOT_ADD1
                                or both.
                                This parameter must be a value of @ref FLASHEx_OB_BOOT_OPTION enumeration */

  uint32_t CM4BootAddr0;   /*!< CM4 Boot Address 0.
                                This parameter must be a value between begin and end of a bank */

  uint32_t CM4BootAddr1;   /*!< CM4 Boot Address 1.
                                This parameter must be a value between begin and end of a bank */
#endif /*DUAL_CORE*/

  uint32_t SecureAreaConfig;    /*!< specifies if the bank secured area shall be erased or not
                                     when RDP level decreased from Level 1 to Level 0 or during a mass erase.
                                     This parameter must be a value of @ref FLASHEx_OB_SECURE_RDP enumeration */

  uint32_t SecureAreaStartAddr; /*!< Bank Secure area Start address.
                                     This parameter must be a value between begin address and end address of bank1 */

  uint32_t SecureAreaEndAddr;   /*!< Bank Secure area End address.
                                     This parameter must be a value between Secure Area Start address and end address of a bank1 */

#if defined (FLASH_OTPBL_LOCKBL)
  uint32_t OTPBlockLock;   /*!< Specifies the OTP block(s) to be locked.
                                This parameter must be a value of @ref FLASHEx_OTP_Blocks */
#endif /* FLASH_OTPBL_LOCKBL */

} FLASH_OBProgramInitTypeDef;

/**
  * @brief  FLASH Erase structure definition
  */
typedef struct
{
  uint32_t TypeCRC;      /*!< CRC Selection Type.
                              This parameter can be a value of @ref FLASHEx_CRC_Selection_Type */

  uint32_t BurstSize;    /*!< CRC Burst Size.
                              This parameter can be a value of @ref FLASHEx_CRC_Burst_Size */

  uint32_t Bank;         /*!< Select bank where CRC computation is enabled.
                              This parameter must be FLASH_BANK_1 or FLASH_BANK_2 */

  uint32_t Sector;       /*!< Initial FLASH sector from which starts the CRC computation
                              This parameter must be a value of @ref FLASH_Sectors */

  uint32_t NbSectors;    /*!< Number of sectors to be computed.
                              This parameter must be a value between 1 and (max number of sectors - value of Initial sector)*/

  uint32_t CRCStartAddr; /*!< CRC Start address.
                              This parameter must be a value between begin address and end address of a bank */

  uint32_t CRCEndAddr;   /*!< CRC End address.
                              This parameter must be a value between CRC Start address and end address of a bank */

} FLASH_CRCInitTypeDef;

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASHEx_Exported_Constants FLASH Exported Constants
  * @{
  */

/** @defgroup FLASHEx_Type_Erase FLASH Type Erase
  * @{
  */
#define FLASH_TYPEERASE_SECTORS      0x00U  /*!< Sectors erase only          */
#define FLASH_TYPEERASE_MASSERASE    0x01U  /*!< Flash Mass erase activation */
/**
  * @}
  */

#if defined (FLASH_CR_PSIZE)
/** @defgroup FLASHEx_Voltage_Range FLASH Voltage Range
  * @{
  */
#define FLASH_VOLTAGE_RANGE_1        0x00000000U       /*!< Flash program/erase by 8 bits  */
#define FLASH_VOLTAGE_RANGE_2        FLASH_CR_PSIZE_0  /*!< Flash program/erase by 16 bits */
#define FLASH_VOLTAGE_RANGE_3        FLASH_CR_PSIZE_1  /*!< Flash program/erase by 32 bits */
#define FLASH_VOLTAGE_RANGE_4        FLASH_CR_PSIZE    /*!< Flash program/erase by 64 bits */
/**
  * @}
  */
#endif /* FLASH_CR_PSIZE */

/** @defgroup FLASHEx_WRP_State FLASH WRP State
  * @{
  */
#define OB_WRPSTATE_DISABLE          0x00000000U  /*!< Disable the write protection of the desired bank 1 sectors */
#define OB_WRPSTATE_ENABLE           0x00000001U  /*!< Enable the write protection of the desired bank 1 sectors  */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Type FLASH Option Type
  * @{
  */
#define OPTIONBYTE_WRP           0x01U  /*!< WRP option byte configuration  */
#define OPTIONBYTE_RDP           0x02U  /*!< RDP option byte configuration  */
#define OPTIONBYTE_USER          0x04U  /*!< USER option byte configuration */
#define OPTIONBYTE_PCROP         0x08U  /*!< PCROP option byte configuration */
#define OPTIONBYTE_BOR           0x10U  /*!< BOR option byte configuration */
#define OPTIONBYTE_SECURE_AREA   0x20U  /*!< secure area option byte configuration */
#if defined (DUAL_CORE)
#define OPTIONBYTE_CM7_BOOTADD   0x40U  /*!< CM7 BOOT ADD option byte configuration */
#define OPTIONBYTE_CM4_BOOTADD   0x80U  /*!< CM4 BOOT ADD option byte configuration */
#define OPTIONBYTE_BOOTADD       OPTIONBYTE_CM7_BOOTADD  /*!< BOOT ADD option byte configuration */
#else /* Single core */
#define OPTIONBYTE_BOOTADD       0x40U  /*!< BOOT ADD option byte configuration */
#endif /*DUAL_CORE*/
#if defined (FLASH_OTPBL_LOCKBL)
#define OPTIONBYTE_OTP_LOCK      0x80U  /*!< OTP Lock option byte configuration */
#endif /* FLASH_OTPBL_LOCKBL */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_Read_Protection FLASH Option Bytes Read Protection
  * @{
  */
#define OB_RDP_LEVEL_0       0xAA00U
#define OB_RDP_LEVEL_1       0x5500U
#define OB_RDP_LEVEL_2       0xCC00U   /*!< Warning: When enabling read protection level 2
                                            it s no more possible to go back to level 1 or 0 */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_WWatchdog FLASH Option Bytes WWatchdog
  * @{
  */
#define OB_WWDG_SW           0x10U  /*!< Software WWDG selected */
#define OB_WWDG_HW           0x00U  /*!< Hardware WWDG selected */
/**
  * @}
  */


/** @defgroup FLASHEx_Option_Bytes_IWatchdog FLASH Option Bytes IWatchdog
  * @{
  */
#define OB_IWDG_SW           0x20U  /*!< Software IWDG selected */
#define OB_IWDG_HW           0x00U  /*!< Hardware IWDG selected */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_nRST_STOP FLASH Option Bytes nRST_STOP
  * @{
  */
#define OB_STOP_NO_RST       0x40U /*!< No reset generated when entering in STOP */
#define OB_STOP_RST          0x00U /*!< Reset generated when entering in STOP    */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_nRST_STDBY FLASH Option Bytes nRST_STDBY
  * @{
  */
#define OB_STDBY_NO_RST      0x80U /*!< No reset generated when entering in STANDBY */
#define OB_STDBY_RST         0x00U /*!< Reset generated when entering in STANDBY    */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWDG_FREEZE_STOP FLASH IWDG Counter Freeze in STOP
  * @{
  */
#define OB_IWDG_STOP_FREEZE  0x00000000U /*!< Freeze IWDG counter in STOP mode */
#define OB_IWDG_STOP_ACTIVE  FLASH_OPTSR_FZ_IWDG_STOP /*!< IWDG counter active in STOP mode */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWDG_FREEZE_SANDBY FLASH IWDG Counter Freeze in STANDBY
  * @{
  */
#define OB_IWDG_STDBY_FREEZE 0x00000000U /*!< Freeze IWDG counter in STANDBY mode */
#define OB_IWDG_STDBY_ACTIVE FLASH_OPTSR_FZ_IWDG_SDBY  /*!< IWDG counter active in STANDBY mode */
/**
  * @}
  */

/** @defgroup FLASHEx_BOR_Reset_Level FLASH BOR Reset Level
  * @{
  */
#define OB_BOR_LEVEL0        0x00000000U                /*!< Reset level threshold is set to 1.6V */
#define OB_BOR_LEVEL1        FLASH_OPTSR_BOR_LEV_0      /*!< Reset level threshold is set to 2.1V */
#define OB_BOR_LEVEL2        FLASH_OPTSR_BOR_LEV_1      /*!< Reset level threshold is set to 2.4V */
#define OB_BOR_LEVEL3        (FLASH_OPTSR_BOR_LEV_1 | FLASH_OPTSR_BOR_LEV_0) /*!< Reset level threshold is set to 2.7V  */
/**
  * @}
  */



/** @defgroup FLASHEx_Boot_Address FLASH Boot Address
  * @{
  */
#define OB_BOOTADDR_ITCM_RAM     0x0000U  /*!< Boot from ITCM RAM (0x00000000)                 */
#define OB_BOOTADDR_SYSTEM       0x0040U  /*!< Boot from System memory bootloader (0x00100000) */
#define OB_BOOTADDR_ITCM_FLASH   0x0080U  /*!< Boot from Flash on ITCM interface (0x00200000)  */
#define OB_BOOTADDR_AXIM_FLASH   0x2000U  /*!< Boot from Flash on AXIM interface (0x08000000)  */
#define OB_BOOTADDR_DTCM_RAM     0x8000U  /*!< Boot from DTCM RAM (0x20000000)                 */
#define OB_BOOTADDR_SRAM1        0x8004U  /*!< Boot from SRAM1 (0x20010000)                    */
#define OB_BOOTADDR_SRAM2        0x8013U  /*!< Boot from SRAM2 (0x2004C000)                    */
/**
  * @}
  */

/** @defgroup FLASH_Latency FLASH Latency
  * @{
  */
#define FLASH_LATENCY_0          FLASH_ACR_LATENCY_0WS   /*!< FLASH Zero Latency cycle      */
#define FLASH_LATENCY_1          FLASH_ACR_LATENCY_1WS   /*!< FLASH One Latency cycle       */
#define FLASH_LATENCY_2          FLASH_ACR_LATENCY_2WS   /*!< FLASH Two Latency cycles      */
#define FLASH_LATENCY_3          FLASH_ACR_LATENCY_3WS   /*!< FLASH Three Latency cycles    */
#define FLASH_LATENCY_4          FLASH_ACR_LATENCY_4WS   /*!< FLASH Four Latency cycles     */
#define FLASH_LATENCY_5          FLASH_ACR_LATENCY_5WS   /*!< FLASH Five Latency cycles     */
#define FLASH_LATENCY_6          FLASH_ACR_LATENCY_6WS   /*!< FLASH Six Latency cycles      */
#define FLASH_LATENCY_7          FLASH_ACR_LATENCY_7WS   /*!< FLASH Seven Latency cycles    */
#define FLASH_LATENCY_8          FLASH_ACR_LATENCY_8WS   /*!< FLASH Eight Latency cycle     */
#define FLASH_LATENCY_9          FLASH_ACR_LATENCY_9WS   /*!< FLASH Nine Latency cycle      */
#define FLASH_LATENCY_10         FLASH_ACR_LATENCY_10WS  /*!< FLASH Ten Latency cycles      */
#define FLASH_LATENCY_11         FLASH_ACR_LATENCY_11WS  /*!< FLASH Eleven Latency cycles   */
#define FLASH_LATENCY_12         FLASH_ACR_LATENCY_12WS  /*!< FLASH Twelve Latency cycles   */
#define FLASH_LATENCY_13         FLASH_ACR_LATENCY_13WS  /*!< FLASH Thirteen Latency cycles */
#define FLASH_LATENCY_14         FLASH_ACR_LATENCY_14WS  /*!< FLASH Fourteen Latency cycles */
#define FLASH_LATENCY_15         FLASH_ACR_LATENCY_15WS  /*!< FLASH Fifteen Latency cycles  */
/**
  * @}
  */

/** @defgroup FLASHEx_Banks FLASH Banks
  * @{
  */
#define FLASH_BANK_1             0x01U                         /*!< Bank 1   */
#define FLASH_BANK_2             0x02U                         /*!< Bank 2   */
#define FLASH_BANK_BOTH          (FLASH_BANK_1 | FLASH_BANK_2) /*!< Bank1 and Bank2  */
/**
  * @}
  */

/** @defgroup FLASHEx_OB_PCROP_RDP  FLASHEx OB PCROP RDP
  * @{
  */
#define OB_PCROP_RDP_NOT_ERASE   0x00000000U     /*!< PCROP area is not erased when the RDP level
                                                      is decreased from Level 1 to Level 0 or during a mass erase */
#define OB_PCROP_RDP_ERASE       FLASH_PRAR_DMEP /*!< PCROP area is erased when the RDP level is
                                                      decreased from Level 1 to Level 0 (full mass erase) */

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_Write_Protection FLASH Option Bytes Write Protection
  * @{
  */
#if (FLASH_SECTOR_TOTAL == 128)
#define OB_WRP_SECTOR_0TO3       0x00000001U /*!< Write protection of Sector0  to Sector3    */
#define OB_WRP_SECTOR_4TO7       0x00000002U /*!< Write protection of Sector4  to Sector7    */
#define OB_WRP_SECTOR_8TO11      0x00000004U /*!< Write protection of Sector8  to Sector11   */
#define OB_WRP_SECTOR_12TO15     0x00000008U /*!< Write protection of Sector12 to Sector15   */
#define OB_WRP_SECTOR_16TO19     0x00000010U /*!< Write protection of Sector16 to Sector19   */
#define OB_WRP_SECTOR_20TO23     0x00000020U /*!< Write protection of Sector20 to Sector23   */
#define OB_WRP_SECTOR_24TO27     0x00000040U /*!< Write protection of Sector24 to Sector27   */
#define OB_WRP_SECTOR_28TO31     0x00000080U /*!< Write protection of Sector28 to Sector31   */
#define OB_WRP_SECTOR_32TO35     0x00000100U /*!< Write protection of Sector32 to Sector35   */
#define OB_WRP_SECTOR_36TO39     0x00000200U /*!< Write protection of Sector36 to Sector39   */
#define OB_WRP_SECTOR_40TO43     0x00000400U /*!< Write protection of Sector40 to Sector43   */
#define OB_WRP_SECTOR_44TO47     0x00000800U /*!< Write protection of Sector44 to Sector47   */
#define OB_WRP_SECTOR_48TO51     0x00001000U /*!< Write protection of Sector48 to Sector51   */
#define OB_WRP_SECTOR_52TO55     0x00002000U /*!< Write protection of Sector52 to Sector55   */
#define OB_WRP_SECTOR_56TO59     0x00004000U /*!< Write protection of Sector56 to Sector59   */
#define OB_WRP_SECTOR_60TO63     0x00008000U /*!< Write protection of Sector60 to Sector63   */
#define OB_WRP_SECTOR_64TO67     0x00010000U /*!< Write protection of Sector64 to Sector67   */
#define OB_WRP_SECTOR_68TO71     0x00020000U /*!< Write protection of Sector68 to Sector71   */
#define OB_WRP_SECTOR_72TO75     0x00040000U /*!< Write protection of Sector72 to Sector75   */
#define OB_WRP_SECTOR_76TO79     0x00080000U /*!< Write protection of Sector76 to Sector79   */
#define OB_WRP_SECTOR_80TO83     0x00100000U /*!< Write protection of Sector80 to Sector83   */
#define OB_WRP_SECTOR_84TO87     0x00200000U /*!< Write protection of Sector84 to Sector87   */
#define OB_WRP_SECTOR_88TO91     0x00400000U /*!< Write protection of Sector88 to Sector91   */
#define OB_WRP_SECTOR_92TO95     0x00800000U /*!< Write protection of Sector92 to Sector95   */
#define OB_WRP_SECTOR_96TO99     0x01000000U /*!< Write protection of Sector96  to Sector99  */
#define OB_WRP_SECTOR_100TO103   0x02000000U /*!< Write protection of Sector100 to Sector103 */
#define OB_WRP_SECTOR_104TO107   0x04000000U /*!< Write protection of Sector104 to Sector107 */
#define OB_WRP_SECTOR_108TO111   0x08000000U /*!< Write protection of Sector108 to Sector111 */
#define OB_WRP_SECTOR_112TO115   0x10000000U /*!< Write protection of Sector112 to Sector115 */
#define OB_WRP_SECTOR_116TO119   0x20000000U /*!< Write protection of Sector116 to Sector119 */
#define OB_WRP_SECTOR_120TO123   0x40000000U /*!< Write protection of Sector120 to Sector123 */
#define OB_WRP_SECTOR_124TO127   0x80000000U /*!< Write protection of Sector124 to Sector127 */
#define OB_WRP_SECTOR_ALL        0xFFFFFFFFU /*!< Write protection of all Sectors            */
#else
#define OB_WRP_SECTOR_0          0x00000001U /*!< Write protection of Sector0                */
#define OB_WRP_SECTOR_1          0x00000002U /*!< Write protection of Sector1                */
#define OB_WRP_SECTOR_2          0x00000004U /*!< Write protection of Sector2                */
#define OB_WRP_SECTOR_3          0x00000008U /*!< Write protection of Sector3                */
#define OB_WRP_SECTOR_4          0x00000010U /*!< Write protection of Sector4                */
#define OB_WRP_SECTOR_5          0x00000020U /*!< Write protection of Sector5                */
#define OB_WRP_SECTOR_6          0x00000040U /*!< Write protection of Sector6                */
#define OB_WRP_SECTOR_7          0x00000080U /*!< Write protection of Sector7                */
#define OB_WRP_SECTOR_ALL        0x000000FFU /*!< Write protection of all Sectors            */
#endif /* FLASH_SECTOR_TOTAL == 128 */
/**
  * @}
  */

/** @defgroup FLASHEx_OB_SECURITY  FLASHEx OB SECURITY
  * @{
  */
#define OB_SECURITY_DISABLE   0x00000000U             /*!< security enabled */
#define OB_SECURITY_ENABLE    FLASH_OPTSR_SECURITY    /*!< security disabled */
/**
  * @}
  */

/** @defgroup FLASHEx_OB_ST_RAM_SIZE  FLASHEx OB ST RAM SIZE
  * @{
  */
#define OB_ST_RAM_SIZE_2KB    0x00000000U               /*!< 2 Kbytes reserved to ST code */
#define OB_ST_RAM_SIZE_4KB    FLASH_OPTSR_ST_RAM_SIZE_0 /*!< 4 Kbytes reserved to ST code */
#define OB_ST_RAM_SIZE_8KB    FLASH_OPTSR_ST_RAM_SIZE_1 /*!< 8 Kbytes reserved to ST code */
#define OB_ST_RAM_SIZE_16KB   FLASH_OPTSR_ST_RAM_SIZE   /*!< 16 Kbytes reserved to ST code */
/**
  * @}
  */

#if defined(DUAL_CORE)
/** @defgroup FLASHEx_OB_BCM7  FLASHEx OB BCM7
  * @{
  */
#define OB_BCM7_DISABLE       0x00000000U              /*!< CM7 Boot disabled */
#define OB_BCM7_ENABLE        FLASH_OPTSR_BCM7         /*!< CM7 Boot enabled */

/**
  * @}
  */

/** @defgroup FLASHEx_OB_BCM4  FLASHEx OB BCM4
  * @{
  */
#define OB_BCM4_DISABLE       0x00000000U              /*!< CM4 Boot disabled */
#define OB_BCM4_ENABLE        FLASH_OPTSR_BCM4         /*!< CM4 Boot enabled */
/**
  * @}
  */
#endif /* DUAL_CORE */

/** @defgroup FLASHEx_OB_IWDG1_SW  FLASHEx OB IWDG1 SW
  * @{
  */
#define OB_IWDG1_SW            FLASH_OPTSR_IWDG1_SW /*!< Hardware independent watchdog 1 */
#define OB_IWDG1_HW            0x00000000U          /*!< Software independent watchdog 1 */
/**
  * @}
  */

#if defined(DUAL_CORE)
/** @defgroup FLASHEx_OB_IWDG2_SW  FLASHEx OB IWDG2 SW
  * @{
  */
#define OB_IWDG2_SW            FLASH_OPTSR_IWDG2_SW  /*!< Hardware independent watchdog 2*/
#define OB_IWDG2_HW            0x00000000U           /*!< Software independent watchdog 2*/
/**
  * @}
  */
#endif

/** @defgroup FLASHEx_OB_NRST_STOP_D1  FLASHEx OB NRST STOP D1
  * @{
  */
#define OB_STOP_RST_D1         0x00000000U              /*!< Reset generated when entering the D1 to stop mode */
#define OB_STOP_NO_RST_D1      FLASH_OPTSR_NRST_STOP_D1 /*!< No reset generated when entering the D1 to stop mode */
/**
  * @}
  */

/** @defgroup FLASHEx_OB_NRST_STDBY_D1  FLASHEx OB NRST STDBY D1
  * @{
  */
#define OB_STDBY_RST_D1        0x00000000U              /*!< Reset generated when entering the D1 to standby mode */
#define OB_STDBY_NO_RST_D1     FLASH_OPTSR_NRST_STBY_D1 /*!< No reset generated when entering the D1 to standby mode */
/**
  * @}
  */

#if defined (DUAL_CORE)
/** @defgroup FLASHEx_OB_NRST_STOP_D2  FLASHEx OB NRST STOP D2
  * @{
  */
#define OB_STOP_RST_D2         0x00000000U              /*!< Reset generated when entering the D2 to stop mode */
#define OB_STOP_NO_RST_D2      FLASH_OPTSR_NRST_STOP_D2 /*!< No reset generated when entering the D2 to stop mode */
/**
  * @}
  */

/** @defgroup FLASHEx_OB_NRST_STDBY_D2  FLASHEx OB NRST STDBY D2
  * @{
  */
#define OB_STDBY_RST_D2        0x00000000U              /*!< Reset generated when entering the D2 to standby mode */
#define OB_STDBY_NO_RST_D2     FLASH_OPTSR_NRST_STBY_D2 /*!< No reset generated when entering the D2 to standby mode */
/**
  * @}
  */
#endif /* DUAL_CORE */

/** @defgroup FLASHEx_OB_SWAP_BANK  FLASHEx OB SWAP BANK
  * @{
  */
#define OB_SWAP_BANK_DISABLE   0x00000000U               /*!< Bank swap disabled */
#define OB_SWAP_BANK_ENABLE    FLASH_OPTSR_SWAP_BANK_OPT /*!< Bank swap enabled */
/**
  * @}
  */

/** @defgroup FLASHEx_OB_IOHSLV FLASHEx OB IOHSLV
  * @{
  */
#define OB_IOHSLV_DISABLE      0x00000000U         /*!< IOHSLV disabled */
#define OB_IOHSLV_ENABLE       FLASH_OPTSR_IO_HSLV /*!< IOHSLV enabled */
/**
  * @}
  */

#if defined (FLASH_OPTSR_VDDMMC_HSLV)
/** @defgroup FLASHEx_OB_VDDMMC_HSLV FLASHEx OB VDDMMC HSLV
  * @{
  */
#define OB_VDDMMC_HSLV_DISABLE 0x00000000U             /*!< VDDMMC HSLV disabled */
#define OB_VDDMMC_HSLV_ENABLE  FLASH_OPTSR_VDDMMC_HSLV /*!< VDDMMC HSLV enabled */
/**
  * @}
  */
#endif /* FLASH_OPTSR_VDDMMC_HSLV */

/** @defgroup FLASHEx_OB_BOOT_OPTION  FLASHEx OB BOOT OPTION
  * @{
  */
#define OB_BOOT_ADD0           0x01U       /*!< Select Boot Address 0 */
#define OB_BOOT_ADD1           0x02U       /*!< Select Boot Address 1 */
#define OB_BOOT_ADD_BOTH       0x03U       /*!< Select Boot Address 0 and 1 */
/**
  * @}
  */

 /** @defgroup FLASHEx_OB_USER_Type  FLASHEx OB USER Type
  * @{
  */
#define OB_USER_IWDG1_SW          0x0001U /*!< Independent watchdog selection */
#define OB_USER_NRST_STOP_D1      0x0002U /*!< Reset when entering Stop mode selection*/
#define OB_USER_NRST_STDBY_D1     0x0004U /*!< Reset when entering standby mode selection*/
#define OB_USER_IWDG_STOP         0x0008U /*!< Independent watchdog counter freeze in stop mode */
#define OB_USER_IWDG_STDBY        0x0010U /*!< Independent watchdog counter freeze in standby mode */
#define OB_USER_ST_RAM_SIZE       0x0020U /*!< dedicated DTCM Ram size selection */
#define OB_USER_SECURITY          0x0040U /*!< security selection */
#define OB_USER_IOHSLV            0x0080U /*!< IO HSLV selection */
#define OB_USER_SWAP_BANK         0x0100U /*!< Bank swap selection */
#if defined (FLASH_OPTSR_VDDMMC_HSLV)
#define OB_USER_VDDMMC_HSLV       0x0200U /*!< VDDMMC HSLV selection */
#endif /* FLASH_OPTSR_VDDMMC_HSLV */
#if defined (DUAL_CORE)
#define OB_USER_IWDG2_SW          0x0200U /*!< Window watchdog selection */
#define OB_USER_BCM4              0x0400U /*!< CM4 boot selection */
#define OB_USER_BCM7              0x0800U /*!< CM7 boot selection */
#define OB_USER_NRST_STOP_D2      0x1000U /*!< Reset when entering Stop mode selection*/
#define OB_USER_NRST_STDBY_D2     0x2000U /*!< Reset when entering standby mode selection*/
#endif /*DUAL_CORE*/

/**
  * @}
  */

/** @defgroup FLASHEx_OB_SECURE_RDP  FLASHEx OB SECURE RDP
  * @{
  */
#define OB_SECURE_RDP_NOT_ERASE   0x00000000U     /*!< Secure area is not erased when the RDP level
                                                       is decreased from Level 1 to Level 0 or during a mass erase */
#define OB_SECURE_RDP_ERASE       FLASH_SCAR_DMES /*!< Secure area is erased when the RDP level is
                                                       decreased from Level 1 to Level 0 (full mass erase) */
/**
  * @}
  */

/** @defgroup FLASHEx_CRC_Selection_Type FLASH CRC Selection Type
  * @{
  */
#define FLASH_CRC_ADDR         0x00000000U              /*!< CRC selection type by address  */
#define FLASH_CRC_SECTORS      FLASH_CRCCR_CRC_BY_SECT  /*!< CRC selection type by sectors  */
#define FLASH_CRC_BANK         (FLASH_CRCCR_ALL_BANK | FLASH_CRCCR_CRC_BY_SECT) /*!< CRC selection type by bank */
/**
  * @}
  */

/** @defgroup FLASHEx_CRC_Burst_Size FLASH CRC Burst Size
  * @{
  */
#define FLASH_CRC_BURST_SIZE_4    0x00000000U              /*!< Every burst has a size of 4 Flash words (256-bit)  */
#define FLASH_CRC_BURST_SIZE_16   FLASH_CRCCR_CRC_BURST_0  /*!< Every burst has a size of 16 Flash words (256-bit)   */
#define FLASH_CRC_BURST_SIZE_64   FLASH_CRCCR_CRC_BURST_1  /*!< Every burst has a size of 64 Flash words (256-bit)   */
#define FLASH_CRC_BURST_SIZE_256  FLASH_CRCCR_CRC_BURST    /*!< Every burst has a size of 256 Flash words (256-bit) */
/**
  * @}
  */

/** @defgroup FLASHEx_Programming_Delay FLASH Programming Delay
  * @{
  */
#define FLASH_PROGRAMMING_DELAY_0   0x00000000U            /*!< programming delay set for Flash running at 70 MHz or below          */
#define FLASH_PROGRAMMING_DELAY_1   FLASH_ACR_WRHIGHFREQ_0 /*!< programming delay set for Flash running between 70 MHz and 185 MHz  */
#define FLASH_PROGRAMMING_DELAY_2   FLASH_ACR_WRHIGHFREQ_1 /*!< programming delay set for Flash running between 185 MHz and 225 MHz */
#define FLASH_PROGRAMMING_DELAY_3   FLASH_ACR_WRHIGHFREQ   /*!< programming delay set for Flash at startup */
/**
  * @}
  */

#if defined (FLASH_OTPBL_LOCKBL)
/** @defgroup FLASHEx_OTP_Blocks FLASH OTP blocks
  * @{
  */
#define FLASH_OTP_BLOCK_0          0x00000001U /*!< OTP Block0     */
#define FLASH_OTP_BLOCK_1          0x00000002U /*!< OTP Block1     */
#define FLASH_OTP_BLOCK_2          0x00000004U /*!< OTP Block2     */
#define FLASH_OTP_BLOCK_3          0x00000008U /*!< OTP Block3     */
#define FLASH_OTP_BLOCK_4          0x00000010U /*!< OTP Block4     */
#define FLASH_OTP_BLOCK_5          0x00000020U /*!< OTP Block5     */
#define FLASH_OTP_BLOCK_6          0x00000040U /*!< OTP Block6     */
#define FLASH_OTP_BLOCK_7          0x00000080U /*!< OTP Block7     */
#define FLASH_OTP_BLOCK_8          0x00000100U /*!< OTP Block8     */
#define FLASH_OTP_BLOCK_9          0x00000200U /*!< OTP Block9     */
#define FLASH_OTP_BLOCK_10         0x00000400U /*!< OTP Block10    */
#define FLASH_OTP_BLOCK_11         0x00000800U /*!< OTP Block11    */
#define FLASH_OTP_BLOCK_12         0x00001000U /*!< OTP Block12    */
#define FLASH_OTP_BLOCK_13         0x00002000U /*!< OTP Block13    */
#define FLASH_OTP_BLOCK_14         0x00004000U /*!< OTP Block14    */
#define FLASH_OTP_BLOCK_15         0x00008000U /*!< OTP Block15    */
#define FLASH_OTP_BLOCK_ALL        0x0000FFFFU /*!< OTP All Blocks */
/**
  * @}
  */
#endif /* FLASH_OTPBL_LOCKBL */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup FLASHEx_Exported_Macros FLASH Exported Macros
  * @{
  */
/**
  * @brief  Calculate the FLASH Boot Base Adress (BOOT_ADD0 or BOOT_ADD1)
  * @note   Returned value BOOT_ADDx[15:0] corresponds to boot address [29:14].
  * @param  __ADDRESS__: FLASH Boot Address (in the range 0x0000 0000 to 0x2004 FFFF with a granularity of 16KB)
  * @retval The FLASH Boot Base Adress
  */
#define __HAL_FLASH_CALC_BOOT_BASE_ADR(__ADDRESS__) ((__ADDRESS__) >> 14U)
 /**
  * @}
  */

#if defined (FLASH_CR_PSIZE)
/**
  * @brief  Set the FLASH Program/Erase parallelism.
  * @param  __PSIZE__ FLASH Program/Erase parallelism
  *         This parameter can be a value of @ref FLASH_Program_Parallelism
  * @param  __BANK__: Flash bank (FLASH_BANK_1 or FLASH_BANK_2)
  * @retval none
  */
#define __HAL_FLASH_SET_PSIZE(__PSIZE__, __BANK__) (((__BANK__) == FLASH_BANK_1)  ? \
                              MODIFY_REG(FLASH->CR1, FLASH_CR_PSIZE, (__PSIZE__)) : \
                              MODIFY_REG(FLASH->CR2, FLASH_CR_PSIZE, (__PSIZE__)))

/**
  * @brief  Get the FLASH Program/Erase parallelism.
  * @param  __BANK__ Flash bank (FLASH_BANK_1 or FLASH_BANK_2)
  * @retval FLASH Program/Erase parallelism
  *         This return value can be a value of @ref FLASH_Program_Parallelism
  */
#define __HAL_FLASH_GET_PSIZE(__BANK__) (((__BANK__) == FLASH_BANK_1) ? \
                              READ_BIT((FLASH->CR1), FLASH_CR_PSIZE)  : \
                              READ_BIT((FLASH->CR2), FLASH_CR_PSIZE))
#endif /* FLASH_CR_PSIZE */

/**
  * @brief  Set the FLASH Programming Delay.
  * @param  __DELAY__ FLASH Programming Delay
  *         This parameter can be a value of @ref FLASHEx_Programming_Delay
  * @retval none
  */
#define __HAL_FLASH_SET_PROGRAM_DELAY(__DELAY__)  MODIFY_REG(FLASH->ACR, FLASH_ACR_WRHIGHFREQ, (__DELAY__))

/**
  * @brief  Get the FLASH Programming Delay.
  * @retval FLASH Programming Delay
  *         This return value can be a value of @ref FLASHEx_Programming_Delay
  */
#define __HAL_FLASH_GET_PROGRAM_DELAY()     READ_BIT(FLASH->ACR, FLASH_ACR_WRHIGHFREQ)

/* Exported functions --------------------------------------------------------*/
/** @addtogroup FLASHEx_Exported_Functions
  * @{
  */

/** @addtogroup FLASHEx_Exported_Functions_Group1
  * @{
  */
/* Extension Program operation functions  *************************************/
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void              HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);

HAL_StatusTypeDef HAL_FLASHEx_Unlock_Bank1(void);
HAL_StatusTypeDef HAL_FLASHEx_Lock_Bank1(void);
HAL_StatusTypeDef HAL_FLASHEx_Unlock_Bank2(void);
HAL_StatusTypeDef HAL_FLASHEx_Lock_Bank2(void);

HAL_StatusTypeDef HAL_FLASHEx_ComputeCRC(FLASH_CRCInitTypeDef *pCRCInit, uint32_t *CRC_Result);

/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup FLASHEx_Private_Macros FLASHEx Private Macros
  * @{
  */

/** @defgroup FLASHEx_IS_FLASH_Definitions FLASHEx Private macros to check input parameters
  * @{
  */

#define IS_FLASH_TYPEERASE(VALUE)        (((VALUE) == FLASH_TYPEERASE_SECTORS) || \
                                          ((VALUE) == FLASH_TYPEERASE_MASSERASE))

#if defined (FLASH_CR_PSIZE)
#define IS_VOLTAGERANGE(RANGE)           (((RANGE) == FLASH_VOLTAGE_RANGE_1) || \
                                          ((RANGE) == FLASH_VOLTAGE_RANGE_2) || \
                                          ((RANGE) == FLASH_VOLTAGE_RANGE_3) || \
                                          ((RANGE) == FLASH_VOLTAGE_RANGE_4))
#endif /* FLASH_CR_PSIZE */

#define IS_WRPSTATE(VALUE)               (((VALUE) == OB_WRPSTATE_DISABLE) || \
                                          ((VALUE) == OB_WRPSTATE_ENABLE))

#if defined (FLASH_OPTSR_VDDMMC_HSLV)
#define IS_OPTIONBYTE(VALUE)             (((VALUE) <= 0x03FFU) && ((VALUE) != 0U))
#elif defined (DUAL_CORE)
#define IS_OPTIONBYTE(VALUE)             (((VALUE) <= 0x3FFFU) && ((VALUE) != 0U))
#else
#define IS_OPTIONBYTE(VALUE)             (((VALUE) <= 0x01FFU) && ((VALUE) != 0U))
#endif /*DUAL_CORE*/

#define IS_OB_BOOT_ADDRESS(ADDRESS)      ((ADDRESS) <= 0x8013U)

#define IS_OB_RDP_LEVEL(LEVEL)           (((LEVEL) == OB_RDP_LEVEL_0)   ||\
                                          ((LEVEL) == OB_RDP_LEVEL_1)   ||\
                                          ((LEVEL) == OB_RDP_LEVEL_2))

#define IS_OB_WWDG_SOURCE(SOURCE)        (((SOURCE) == OB_WWDG_SW) || ((SOURCE) == OB_WWDG_HW))

#define IS_OB_IWDG_SOURCE(SOURCE)        (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))

#define IS_OB_STOP_SOURCE(SOURCE)        (((SOURCE) == OB_STOP_NO_RST) || ((SOURCE) == OB_STOP_RST))

#define IS_OB_STDBY_SOURCE(SOURCE)       (((SOURCE) == OB_STDBY_NO_RST) || ((SOURCE) == OB_STDBY_RST))

#define IS_OB_IWDG_STOP_FREEZE(FREEZE)   (((FREEZE) == OB_IWDG_STOP_FREEZE) || ((FREEZE) == OB_IWDG_STOP_ACTIVE))

#define IS_OB_IWDG_STDBY_FREEZE(FREEZE)  (((FREEZE) == OB_IWDG_STDBY_FREEZE) || ((FREEZE) == OB_IWDG_STDBY_ACTIVE))

#define IS_OB_BOR_LEVEL(LEVEL)           (((LEVEL) == OB_BOR_LEVEL0) || ((LEVEL) == OB_BOR_LEVEL1) || \
                                          ((LEVEL) == OB_BOR_LEVEL2) || ((LEVEL) == OB_BOR_LEVEL3))

#define IS_FLASH_LATENCY(LATENCY)        (((LATENCY) == FLASH_LATENCY_0)  || \
                                          ((LATENCY) == FLASH_LATENCY_1)  || \
                                          ((LATENCY) == FLASH_LATENCY_2)  || \
                                          ((LATENCY) == FLASH_LATENCY_3)  || \
                                          ((LATENCY) == FLASH_LATENCY_4)  || \
                                          ((LATENCY) == FLASH_LATENCY_5)  || \
                                          ((LATENCY) == FLASH_LATENCY_6)  || \
                                          ((LATENCY) == FLASH_LATENCY_7)  || \
                                          ((LATENCY) == FLASH_LATENCY_8)  || \
                                          ((LATENCY) == FLASH_LATENCY_9)  || \
                                          ((LATENCY) == FLASH_LATENCY_10) || \
                                          ((LATENCY) == FLASH_LATENCY_11) || \
                                          ((LATENCY) == FLASH_LATENCY_12) || \
                                          ((LATENCY) == FLASH_LATENCY_13) || \
                                          ((LATENCY) == FLASH_LATENCY_14) || \
                                          ((LATENCY) == FLASH_LATENCY_15))

#define IS_FLASH_SECTOR(SECTOR)          ((SECTOR) < FLASH_SECTOR_TOTAL)

#if (FLASH_SECTOR_TOTAL == 8U)
#define IS_OB_WRP_SECTOR(SECTOR)         ((((SECTOR) & 0xFFFFFF00U) == 0x00000000U) && ((SECTOR) != 0x00000000U))
#else
#define IS_OB_WRP_SECTOR(SECTOR)         ((SECTOR) != 0x00000000U)
#endif /* FLASH_SECTOR_TOTAL == 8U */

#define IS_OB_PCROP_RDP(CONFIG)          (((CONFIG) == OB_PCROP_RDP_NOT_ERASE) || \
                                          ((CONFIG) == OB_PCROP_RDP_ERASE))

#define IS_OB_SECURE_RDP(CONFIG)         (((CONFIG) == OB_SECURE_RDP_NOT_ERASE) || \
                                          ((CONFIG) == OB_SECURE_RDP_ERASE))

#define IS_OB_USER_SWAP_BANK(VALUE)      (((VALUE) == OB_SWAP_BANK_DISABLE) || ((VALUE) == OB_SWAP_BANK_ENABLE))

#define IS_OB_USER_IOHSLV(VALUE)         (((VALUE) == OB_IOHSLV_DISABLE) || ((VALUE) == OB_IOHSLV_ENABLE))

#if defined (FLASH_OPTSR_VDDMMC_HSLV)
#define IS_OB_USER_VDDMMC_HSLV(VALUE)    (((VALUE) == OB_VDDMMC_HSLV_DISABLE) || ((VALUE) == OB_VDDMMC_HSLV_ENABLE))
#endif /* FLASH_OPTSR_VDDMMC_HSLV */

#define IS_OB_IWDG1_SOURCE(SOURCE)       (((SOURCE) == OB_IWDG1_SW) || ((SOURCE) == OB_IWDG1_HW))
#if defined (DUAL_CORE)
#define IS_OB_IWDG2_SOURCE(SOURCE)       (((SOURCE) == OB_IWDG2_SW) || ((SOURCE) == OB_IWDG2_HW))
#endif /* DUAL_CORE */
#define IS_OB_STOP_D1_RESET(VALUE)       (((VALUE) == OB_STOP_NO_RST_D1) || ((VALUE) == OB_STOP_RST_D1))

#define IS_OB_STDBY_D1_RESET(VALUE)      (((VALUE) == OB_STDBY_NO_RST_D1) || ((VALUE) == OB_STDBY_RST_D1))

#define IS_OB_USER_IWDG_STOP(VALUE)      (((VALUE) == OB_IWDG_STOP_FREEZE) || ((VALUE) == OB_IWDG_STOP_ACTIVE))

#define IS_OB_USER_IWDG_STDBY(VALUE)     (((VALUE) == OB_IWDG_STDBY_FREEZE) || ((VALUE) == OB_IWDG_STDBY_ACTIVE))

#define IS_OB_USER_ST_RAM_SIZE(VALUE)    (((VALUE) == OB_ST_RAM_SIZE_2KB) || ((VALUE) == OB_ST_RAM_SIZE_4KB) || \
                                          ((VALUE) == OB_ST_RAM_SIZE_8KB) || ((VALUE) == OB_ST_RAM_SIZE_16KB))

#define IS_OB_USER_SECURITY(VALUE)       (((VALUE) == OB_SECURITY_ENABLE) || ((VALUE) == OB_SECURITY_DISABLE))

#if defined (DUAL_CORE)
#define IS_OB_USER_BCM4(VALUE)           (((VALUE) == OB_BCM4_DISABLE) || ((VALUE) == OB_BCM4_ENABLE))

#define IS_OB_USER_BCM7(VALUE)           (((VALUE) == OB_BCM7_DISABLE) || ((VALUE) == OB_BCM7_ENABLE))

#define IS_OB_STOP_D2_RESET(VALUE)       (((VALUE) == OB_STOP_NO_RST_D2) || ((VALUE) == OB_STOP_RST_D2))

#define IS_OB_STDBY_D2_RESET(VALUE)      (((VALUE) == OB_STDBY_NO_RST_D2) || ((VALUE) == OB_STDBY_RST_D2))
#endif /* DUAL_CORE */
#if defined (DUAL_CORE)
#define IS_OB_USER_TYPE(TYPE)            (((TYPE) <= (uint32_t)0x3FFFU) && ((TYPE) != 0U))
#else
#define IS_OB_USER_TYPE(TYPE)            (((TYPE) <= (uint32_t)0x73FU) && ((TYPE) != 0U))
#endif /* DUAL_CORE */

#define IS_OB_BOOT_ADD_OPTION(VALUE)     (((VALUE) == OB_BOOT_ADD0)  || \
                                          ((VALUE) == OB_BOOT_ADD1)  || \
                                          ((VALUE) == OB_BOOT_ADD_BOTH))

#define IS_FLASH_TYPECRC(VALUE)          (((VALUE) == FLASH_CRC_ADDR)    || \
                                          ((VALUE) == FLASH_CRC_SECTORS) || \
                                          ((VALUE) == FLASH_CRC_BANK))

#if defined (FLASH_OTPBL_LOCKBL)
#define IS_OTP_BLOCK(VALUE)              ((((VALUE) & 0xFFFF0000U) == 0x00000000U) && ((VALUE) != 0x00000000U))
#endif /* FLASH_OTPBL_LOCKBL */
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup FLASHEx_Private_Functions FLASHEx Private Functions
  * @{
  */
void FLASH_Erase_Sector(uint32_t Sector, uint32_t Banks, uint32_t VoltageRange);
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

#endif /* STM32H7xx_HAL_FLASH_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
