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

#ifndef _SAME70_RSWDT_COMPONENT_
#define _SAME70_RSWDT_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Reinforced Safety Watchdog Timer */
/* ============================================================================= */
/** \addtogroup SAME70_RSWDT Reinforced Safety Watchdog Timer */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Rswdt hardware registers */
typedef struct {
  __O  uint32_t RSWDT_CR; /**< \brief (Rswdt Offset: 0x00) Control Register */
  __IO uint32_t RSWDT_MR; /**< \brief (Rswdt Offset: 0x04) Mode Register */
  __I  uint32_t RSWDT_SR; /**< \brief (Rswdt Offset: 0x08) Status Register */
} Rswdt;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- RSWDT_CR : (RSWDT Offset: 0x00) Control Register -------- */
#define RSWDT_CR_WDRSTT (0x1u << 0) /**< \brief (RSWDT_CR) Watchdog Restart */
#define RSWDT_CR_KEY_Pos 24
#define RSWDT_CR_KEY_Msk (0xffu << RSWDT_CR_KEY_Pos) /**< \brief (RSWDT_CR) Password */
#define RSWDT_CR_KEY(value) ((RSWDT_CR_KEY_Msk & ((value) << RSWDT_CR_KEY_Pos)))
#define   RSWDT_CR_KEY_PASSWD (0xC4u << 24) /**< \brief (RSWDT_CR) Writing any other value in this field aborts the write operation. */
/* -------- RSWDT_MR : (RSWDT Offset: 0x04) Mode Register -------- */
#define RSWDT_MR_WDV_Pos 0
#define RSWDT_MR_WDV_Msk (0xfffu << RSWDT_MR_WDV_Pos) /**< \brief (RSWDT_MR) Watchdog Counter Value */
#define RSWDT_MR_WDV(value) ((RSWDT_MR_WDV_Msk & ((value) << RSWDT_MR_WDV_Pos)))
#define RSWDT_MR_WDFIEN (0x1u << 12) /**< \brief (RSWDT_MR) Watchdog Fault Interrupt Enable */
#define RSWDT_MR_WDRSTEN (0x1u << 13) /**< \brief (RSWDT_MR) Watchdog Reset Enable */
#define RSWDT_MR_WDRPROC (0x1u << 14) /**< \brief (RSWDT_MR) Watchdog Reset Processor */
#define RSWDT_MR_WDDIS (0x1u << 15) /**< \brief (RSWDT_MR) Watchdog Disable */
#define RSWDT_MR_ALLONES_Pos 16
#define RSWDT_MR_ALLONES_Msk (0xfffu << RSWDT_MR_ALLONES_Pos) /**< \brief (RSWDT_MR) Must Always Be Written with 0xFFF */
#define RSWDT_MR_ALLONES(value) ((RSWDT_MR_ALLONES_Msk & ((value) << RSWDT_MR_ALLONES_Pos)))
#define RSWDT_MR_WDDBGHLT (0x1u << 28) /**< \brief (RSWDT_MR) Watchdog Debug Halt */
#define RSWDT_MR_WDIDLEHLT (0x1u << 29) /**< \brief (RSWDT_MR) Watchdog Idle Halt */
/* -------- RSWDT_SR : (RSWDT Offset: 0x08) Status Register -------- */
#define RSWDT_SR_WDUNF (0x1u << 0) /**< \brief (RSWDT_SR) Watchdog Underflow */

/*@}*/


#endif /* _SAME70_RSWDT_COMPONENT_ */
