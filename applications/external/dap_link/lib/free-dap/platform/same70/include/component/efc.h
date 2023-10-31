/**
 * \file
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _SAME70_EFC_COMPONENT_
#define _SAME70_EFC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Embedded Flash Controller */
/* ============================================================================= */
/** \addtogroup SAME70_EFC Embedded Flash Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Efc hardware registers */
typedef struct {
  __IO uint32_t EEFC_FMR;      /**< \brief (Efc Offset: 0x00) EEFC Flash Mode Register */
  __O  uint32_t EEFC_FCR;      /**< \brief (Efc Offset: 0x04) EEFC Flash Command Register */
  __I  uint32_t EEFC_FSR;      /**< \brief (Efc Offset: 0x08) EEFC Flash Status Register */
  __I  uint32_t EEFC_FRR;      /**< \brief (Efc Offset: 0x0C) EEFC Flash Result Register */
  __I  uint32_t Reserved1[1];
  __I  uint32_t EEFC_VERSION;  /**< \brief (Efc Offset: 0x14) EEFC Version Register */
  __I  uint32_t Reserved2[51];
  __IO uint32_t EEFC_WPMR;     /**< \brief (Efc Offset: 0xE4) Write Protection Mode Register */
} Efc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- EEFC_FMR : (EFC Offset: 0x00) EEFC Flash Mode Register -------- */
#define EEFC_FMR_FRDY (0x1u << 0) /**< \brief (EEFC_FMR) Flash Ready Interrupt Enable */
#define EEFC_FMR_FWS_Pos 8
#define EEFC_FMR_FWS_Msk (0xfu << EEFC_FMR_FWS_Pos) /**< \brief (EEFC_FMR) Flash Wait State */
#define EEFC_FMR_FWS(value) ((EEFC_FMR_FWS_Msk & ((value) << EEFC_FMR_FWS_Pos)))
#define EEFC_FMR_SCOD (0x1u << 16) /**< \brief (EEFC_FMR) Sequential Code Optimization Disable */
#define EEFC_FMR_CLOE (0x1u << 26) /**< \brief (EEFC_FMR) Code Loop Optimization Enable */
/* -------- EEFC_FCR : (EFC Offset: 0x04) EEFC Flash Command Register -------- */
#define EEFC_FCR_FCMD_Pos 0
#define EEFC_FCR_FCMD_Msk (0xffu << EEFC_FCR_FCMD_Pos) /**< \brief (EEFC_FCR) Flash Command */
#define EEFC_FCR_FCMD(value) ((EEFC_FCR_FCMD_Msk & ((value) << EEFC_FCR_FCMD_Pos)))
#define   EEFC_FCR_FCMD_GETD (0x0u << 0) /**< \brief (EEFC_FCR) Get Flash descriptor */
#define   EEFC_FCR_FCMD_WP (0x1u << 0) /**< \brief (EEFC_FCR) Write page */
#define   EEFC_FCR_FCMD_WPL (0x2u << 0) /**< \brief (EEFC_FCR) Write page and lock */
#define   EEFC_FCR_FCMD_EWP (0x3u << 0) /**< \brief (EEFC_FCR) Erase page and write page */
#define   EEFC_FCR_FCMD_EWPL (0x4u << 0) /**< \brief (EEFC_FCR) Erase page and write page then lock */
#define   EEFC_FCR_FCMD_EA (0x5u << 0) /**< \brief (EEFC_FCR) Erase all */
#define   EEFC_FCR_FCMD_EPA (0x7u << 0) /**< \brief (EEFC_FCR) Erase pages */
#define   EEFC_FCR_FCMD_SLB (0x8u << 0) /**< \brief (EEFC_FCR) Set lock bit */
#define   EEFC_FCR_FCMD_CLB (0x9u << 0) /**< \brief (EEFC_FCR) Clear lock bit */
#define   EEFC_FCR_FCMD_GLB (0xAu << 0) /**< \brief (EEFC_FCR) Get lock bit */
#define   EEFC_FCR_FCMD_SGPB (0xBu << 0) /**< \brief (EEFC_FCR) Set GPNVM bit */
#define   EEFC_FCR_FCMD_CGPB (0xCu << 0) /**< \brief (EEFC_FCR) Clear GPNVM bit */
#define   EEFC_FCR_FCMD_GGPB (0xDu << 0) /**< \brief (EEFC_FCR) Get GPNVM bit */
#define   EEFC_FCR_FCMD_STUI (0xEu << 0) /**< \brief (EEFC_FCR) Start read unique identifier */
#define   EEFC_FCR_FCMD_SPUI (0xFu << 0) /**< \brief (EEFC_FCR) Stop read unique identifier */
#define   EEFC_FCR_FCMD_GCALB (0x10u << 0) /**< \brief (EEFC_FCR) Get CALIB bit */
#define   EEFC_FCR_FCMD_ES (0x11u << 0) /**< \brief (EEFC_FCR) Erase sector */
#define   EEFC_FCR_FCMD_WUS (0x12u << 0) /**< \brief (EEFC_FCR) Write user signature */
#define   EEFC_FCR_FCMD_EUS (0x13u << 0) /**< \brief (EEFC_FCR) Erase user signature */
#define   EEFC_FCR_FCMD_STUS (0x14u << 0) /**< \brief (EEFC_FCR) Start read user signature */
#define   EEFC_FCR_FCMD_SPUS (0x15u << 0) /**< \brief (EEFC_FCR) Stop read user signature */
#define EEFC_FCR_FARG_Pos 8
#define EEFC_FCR_FARG_Msk (0xffffu << EEFC_FCR_FARG_Pos) /**< \brief (EEFC_FCR) Flash Command Argument */
#define EEFC_FCR_FARG(value) ((EEFC_FCR_FARG_Msk & ((value) << EEFC_FCR_FARG_Pos)))
#define EEFC_FCR_FKEY_Pos 24
#define EEFC_FCR_FKEY_Msk (0xffu << EEFC_FCR_FKEY_Pos) /**< \brief (EEFC_FCR) Flash Writing Protection Key */
#define EEFC_FCR_FKEY(value) ((EEFC_FCR_FKEY_Msk & ((value) << EEFC_FCR_FKEY_Pos)))
#define   EEFC_FCR_FKEY_PASSWD (0x5Au << 24) /**< \brief (EEFC_FCR) The 0x5A value enables the command defined by the bits of the register. If the field is written with a different value, the write is not performed and no action is started. */
/* -------- EEFC_FSR : (EFC Offset: 0x08) EEFC Flash Status Register -------- */
#define EEFC_FSR_FRDY (0x1u << 0) /**< \brief (EEFC_FSR) Flash Ready Status (cleared when Flash is busy) */
#define EEFC_FSR_FCMDE (0x1u << 1) /**< \brief (EEFC_FSR) Flash Command Error Status (cleared on read or by writing EEFC_FCR) */
#define EEFC_FSR_FLOCKE (0x1u << 2) /**< \brief (EEFC_FSR) Flash Lock Error Status (cleared on read) */
#define EEFC_FSR_FLERR (0x1u << 3) /**< \brief (EEFC_FSR) Flash Error Status (cleared when a programming operation starts) */
#define EEFC_FSR_UECCELSB (0x1u << 16) /**< \brief (EEFC_FSR) Unique ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) */
#define EEFC_FSR_MECCELSB (0x1u << 17) /**< \brief (EEFC_FSR) Multiple ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) */
#define EEFC_FSR_UECCEMSB (0x1u << 18) /**< \brief (EEFC_FSR) Unique ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) */
#define EEFC_FSR_MECCEMSB (0x1u << 19) /**< \brief (EEFC_FSR) Multiple ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) */
/* -------- EEFC_FRR : (EFC Offset: 0x0C) EEFC Flash Result Register -------- */
#define EEFC_FRR_FVALUE_Pos 0
#define EEFC_FRR_FVALUE_Msk (0xffffffffu << EEFC_FRR_FVALUE_Pos) /**< \brief (EEFC_FRR) Flash Result Value */
/* -------- EEFC_VERSION : (EFC Offset: 0x14) EEFC Version Register -------- */
#define EEFC_VERSION_VERSION_Pos 0
#define EEFC_VERSION_VERSION_Msk (0xfffu << EEFC_VERSION_VERSION_Pos) /**< \brief (EEFC_VERSION) Version of the Hardware Module */
#define EEFC_VERSION_MFN_Pos 16
#define EEFC_VERSION_MFN_Msk (0x7u << EEFC_VERSION_MFN_Pos) /**< \brief (EEFC_VERSION) Metal Fix Number */
/* -------- EEFC_WPMR : (EFC Offset: 0xE4) Write Protection Mode Register -------- */
#define EEFC_WPMR_WPEN (0x1u << 0) /**< \brief (EEFC_WPMR) Write Protection Enable */
#define EEFC_WPMR_WPKEY_Pos 8
#define EEFC_WPMR_WPKEY_Msk (0xffffffu << EEFC_WPMR_WPKEY_Pos) /**< \brief (EEFC_WPMR) Write Protection Key */
#define EEFC_WPMR_WPKEY(value) ((EEFC_WPMR_WPKEY_Msk & ((value) << EEFC_WPMR_WPKEY_Pos)))
#define   EEFC_WPMR_WPKEY_PASSWD (0x454643u << 8) /**< \brief (EEFC_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0. */

/*@}*/


#endif /* _SAME70_EFC_COMPONENT_ */
