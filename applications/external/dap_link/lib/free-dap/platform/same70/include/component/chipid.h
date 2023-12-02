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

#ifndef _SAME70_CHIPID_COMPONENT_
#define _SAME70_CHIPID_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Chip Identifier */
/* ============================================================================= */
/** \addtogroup SAME70_CHIPID Chip Identifier */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Chipid hardware registers */
typedef struct {
  __I uint32_t CHIPID_CIDR; /**< \brief (Chipid Offset: 0x0) Chip ID Register */
  __I uint32_t CHIPID_EXID; /**< \brief (Chipid Offset: 0x4) Chip ID Extension Register */
} Chipid;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- CHIPID_CIDR : (CHIPID Offset: 0x0) Chip ID Register -------- */
#define CHIPID_CIDR_VERSION_Pos 0
#define CHIPID_CIDR_VERSION_Msk (0x1fu << CHIPID_CIDR_VERSION_Pos) /**< \brief (CHIPID_CIDR) Version of the Device */
#define CHIPID_CIDR_EPROC_Pos 5
#define CHIPID_CIDR_EPROC_Msk (0x7u << CHIPID_CIDR_EPROC_Pos) /**< \brief (CHIPID_CIDR) Embedded Processor */
#define   CHIPID_CIDR_EPROC_SAMx7 (0x0u << 5) /**< \brief (CHIPID_CIDR) Cortex-M7 */
#define   CHIPID_CIDR_EPROC_ARM946ES (0x1u << 5) /**< \brief (CHIPID_CIDR) ARM946ES */
#define   CHIPID_CIDR_EPROC_ARM7TDMI (0x2u << 5) /**< \brief (CHIPID_CIDR) ARM7TDMI */
#define   CHIPID_CIDR_EPROC_CM3 (0x3u << 5) /**< \brief (CHIPID_CIDR) Cortex-M3 */
#define   CHIPID_CIDR_EPROC_ARM920T (0x4u << 5) /**< \brief (CHIPID_CIDR) ARM920T */
#define   CHIPID_CIDR_EPROC_ARM926EJS (0x5u << 5) /**< \brief (CHIPID_CIDR) ARM926EJS */
#define   CHIPID_CIDR_EPROC_CA5 (0x6u << 5) /**< \brief (CHIPID_CIDR) Cortex-A5 */
#define   CHIPID_CIDR_EPROC_CM4 (0x7u << 5) /**< \brief (CHIPID_CIDR) Cortex-M4 */
#define CHIPID_CIDR_NVPSIZ_Pos 8
#define CHIPID_CIDR_NVPSIZ_Msk (0xfu << CHIPID_CIDR_NVPSIZ_Pos) /**< \brief (CHIPID_CIDR) Nonvolatile Program Memory Size */
#define   CHIPID_CIDR_NVPSIZ_NONE (0x0u << 8) /**< \brief (CHIPID_CIDR) None */
#define   CHIPID_CIDR_NVPSIZ_8K (0x1u << 8) /**< \brief (CHIPID_CIDR) 8 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_16K (0x2u << 8) /**< \brief (CHIPID_CIDR) 16 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_32K (0x3u << 8) /**< \brief (CHIPID_CIDR) 32 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_64K (0x5u << 8) /**< \brief (CHIPID_CIDR) 64 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_128K (0x7u << 8) /**< \brief (CHIPID_CIDR) 128 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_160K (0x8u << 8) /**< \brief (CHIPID_CIDR) 160 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_256K (0x9u << 8) /**< \brief (CHIPID_CIDR) 256 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_512K (0xAu << 8) /**< \brief (CHIPID_CIDR) 512 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_1024K (0xCu << 8) /**< \brief (CHIPID_CIDR) 1024 Kbytes */
#define   CHIPID_CIDR_NVPSIZ_2048K (0xEu << 8) /**< \brief (CHIPID_CIDR) 2048 Kbytes */
#define CHIPID_CIDR_NVPSIZ2_Pos 12
#define CHIPID_CIDR_NVPSIZ2_Msk (0xfu << CHIPID_CIDR_NVPSIZ2_Pos) /**< \brief (CHIPID_CIDR) Second Nonvolatile Program Memory Size */
#define   CHIPID_CIDR_NVPSIZ2_NONE (0x0u << 12) /**< \brief (CHIPID_CIDR) None */
#define   CHIPID_CIDR_NVPSIZ2_8K (0x1u << 12) /**< \brief (CHIPID_CIDR) 8 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_16K (0x2u << 12) /**< \brief (CHIPID_CIDR) 16 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_32K (0x3u << 12) /**< \brief (CHIPID_CIDR) 32 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_64K (0x5u << 12) /**< \brief (CHIPID_CIDR) 64 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_128K (0x7u << 12) /**< \brief (CHIPID_CIDR) 128 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_256K (0x9u << 12) /**< \brief (CHIPID_CIDR) 256 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_512K (0xAu << 12) /**< \brief (CHIPID_CIDR) 512 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_1024K (0xCu << 12) /**< \brief (CHIPID_CIDR) 1024 Kbytes */
#define   CHIPID_CIDR_NVPSIZ2_2048K (0xEu << 12) /**< \brief (CHIPID_CIDR) 2048 Kbytes */
#define CHIPID_CIDR_SRAMSIZ_Pos 16
#define CHIPID_CIDR_SRAMSIZ_Msk (0xfu << CHIPID_CIDR_SRAMSIZ_Pos) /**< \brief (CHIPID_CIDR) Internal SRAM Size */
#define   CHIPID_CIDR_SRAMSIZ_48K (0x0u << 16) /**< \brief (CHIPID_CIDR) 48 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_192K (0x1u << 16) /**< \brief (CHIPID_CIDR) 192 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_384K (0x2u << 16) /**< \brief (CHIPID_CIDR) 384 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_6K (0x3u << 16) /**< \brief (CHIPID_CIDR) 6 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_24K (0x4u << 16) /**< \brief (CHIPID_CIDR) 24 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_4K (0x5u << 16) /**< \brief (CHIPID_CIDR) 4 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_80K (0x6u << 16) /**< \brief (CHIPID_CIDR) 80 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_160K (0x7u << 16) /**< \brief (CHIPID_CIDR) 160 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_8K (0x8u << 16) /**< \brief (CHIPID_CIDR) 8 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_16K (0x9u << 16) /**< \brief (CHIPID_CIDR) 16 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_32K (0xAu << 16) /**< \brief (CHIPID_CIDR) 32 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_64K (0xBu << 16) /**< \brief (CHIPID_CIDR) 64 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_128K (0xCu << 16) /**< \brief (CHIPID_CIDR) 128 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_256K (0xDu << 16) /**< \brief (CHIPID_CIDR) 256 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_96K (0xEu << 16) /**< \brief (CHIPID_CIDR) 96 Kbytes */
#define   CHIPID_CIDR_SRAMSIZ_512K (0xFu << 16) /**< \brief (CHIPID_CIDR) 512 Kbytes */
#define CHIPID_CIDR_ARCH_Pos 20
#define CHIPID_CIDR_ARCH_Msk (0xffu << CHIPID_CIDR_ARCH_Pos) /**< \brief (CHIPID_CIDR) Architecture Identifier */
#define   CHIPID_CIDR_ARCH_SAME70 (0x10u << 20) /**< \brief (CHIPID_CIDR) SAM E70 */
#define   CHIPID_CIDR_ARCH_SAMS70 (0x11u << 20) /**< \brief (CHIPID_CIDR) SAM S70 */
#define   CHIPID_CIDR_ARCH_SAMV71 (0x12u << 20) /**< \brief (CHIPID_CIDR) SAM V71 */
#define   CHIPID_CIDR_ARCH_SAMV70 (0x13u << 20) /**< \brief (CHIPID_CIDR) SAM V70 */
#define CHIPID_CIDR_NVPTYP_Pos 28
#define CHIPID_CIDR_NVPTYP_Msk (0x7u << CHIPID_CIDR_NVPTYP_Pos) /**< \brief (CHIPID_CIDR) Nonvolatile Program Memory Type */
#define   CHIPID_CIDR_NVPTYP_ROM (0x0u << 28) /**< \brief (CHIPID_CIDR) ROM */
#define   CHIPID_CIDR_NVPTYP_ROMLESS (0x1u << 28) /**< \brief (CHIPID_CIDR) ROMless or on-chip Flash */
#define   CHIPID_CIDR_NVPTYP_FLASH (0x2u << 28) /**< \brief (CHIPID_CIDR) Embedded Flash Memory */
#define   CHIPID_CIDR_NVPTYP_ROM_FLASH (0x3u << 28) /**< \brief (CHIPID_CIDR) ROM and Embedded Flash Memory- NVPSIZ is ROM size- NVPSIZ2 is Flash size */
#define   CHIPID_CIDR_NVPTYP_SRAM (0x4u << 28) /**< \brief (CHIPID_CIDR) SRAM emulating ROM */
#define CHIPID_CIDR_EXT (0x1u << 31) /**< \brief (CHIPID_CIDR) Extension Flag */
/* -------- CHIPID_EXID : (CHIPID Offset: 0x4) Chip ID Extension Register -------- */
#define CHIPID_EXID_EXID_Pos 0
#define CHIPID_EXID_EXID_Msk (0xffffffffu << CHIPID_EXID_EXID_Pos) /**< \brief (CHIPID_EXID) Chip ID Extension */

/*@}*/


#endif /* _SAME70_CHIPID_COMPONENT_ */
