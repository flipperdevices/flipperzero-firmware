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

#ifndef _SAME70_PIOE_INSTANCE_
#define _SAME70_PIOE_INSTANCE_

/* ========== Register definition for PIOE peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PIOE_PER                      (0x400E1600U) /**< \brief (PIOE) PIO Enable Register */
  #define REG_PIOE_PDR                      (0x400E1604U) /**< \brief (PIOE) PIO Disable Register */
  #define REG_PIOE_PSR                      (0x400E1608U) /**< \brief (PIOE) PIO Status Register */
  #define REG_PIOE_OER                      (0x400E1610U) /**< \brief (PIOE) Output Enable Register */
  #define REG_PIOE_ODR                      (0x400E1614U) /**< \brief (PIOE) Output Disable Register */
  #define REG_PIOE_OSR                      (0x400E1618U) /**< \brief (PIOE) Output Status Register */
  #define REG_PIOE_IFER                     (0x400E1620U) /**< \brief (PIOE) Glitch Input Filter Enable Register */
  #define REG_PIOE_IFDR                     (0x400E1624U) /**< \brief (PIOE) Glitch Input Filter Disable Register */
  #define REG_PIOE_IFSR                     (0x400E1628U) /**< \brief (PIOE) Glitch Input Filter Status Register */
  #define REG_PIOE_SODR                     (0x400E1630U) /**< \brief (PIOE) Set Output Data Register */
  #define REG_PIOE_CODR                     (0x400E1634U) /**< \brief (PIOE) Clear Output Data Register */
  #define REG_PIOE_ODSR                     (0x400E1638U) /**< \brief (PIOE) Output Data Status Register */
  #define REG_PIOE_PDSR                     (0x400E163CU) /**< \brief (PIOE) Pin Data Status Register */
  #define REG_PIOE_IER                      (0x400E1640U) /**< \brief (PIOE) Interrupt Enable Register */
  #define REG_PIOE_IDR                      (0x400E1644U) /**< \brief (PIOE) Interrupt Disable Register */
  #define REG_PIOE_IMR                      (0x400E1648U) /**< \brief (PIOE) Interrupt Mask Register */
  #define REG_PIOE_ISR                      (0x400E164CU) /**< \brief (PIOE) Interrupt Status Register */
  #define REG_PIOE_MDER                     (0x400E1650U) /**< \brief (PIOE) Multi-driver Enable Register */
  #define REG_PIOE_MDDR                     (0x400E1654U) /**< \brief (PIOE) Multi-driver Disable Register */
  #define REG_PIOE_MDSR                     (0x400E1658U) /**< \brief (PIOE) Multi-driver Status Register */
  #define REG_PIOE_PUDR                     (0x400E1660U) /**< \brief (PIOE) Pull-up Disable Register */
  #define REG_PIOE_PUER                     (0x400E1664U) /**< \brief (PIOE) Pull-up Enable Register */
  #define REG_PIOE_PUSR                     (0x400E1668U) /**< \brief (PIOE) Pad Pull-up Status Register */
  #define REG_PIOE_ABCDSR                   (0x400E1670U) /**< \brief (PIOE) Peripheral Select Register */
  #define REG_PIOE_IFSCDR                   (0x400E1680U) /**< \brief (PIOE) Input Filter Slow Clock Disable Register */
  #define REG_PIOE_IFSCER                   (0x400E1684U) /**< \brief (PIOE) Input Filter Slow Clock Enable Register */
  #define REG_PIOE_IFSCSR                   (0x400E1688U) /**< \brief (PIOE) Input Filter Slow Clock Status Register */
  #define REG_PIOE_SCDR                     (0x400E168CU) /**< \brief (PIOE) Slow Clock Divider Debouncing Register */
  #define REG_PIOE_PPDDR                    (0x400E1690U) /**< \brief (PIOE) Pad Pull-down Disable Register */
  #define REG_PIOE_PPDER                    (0x400E1694U) /**< \brief (PIOE) Pad Pull-down Enable Register */
  #define REG_PIOE_PPDSR                    (0x400E1698U) /**< \brief (PIOE) Pad Pull-down Status Register */
  #define REG_PIOE_OWER                     (0x400E16A0U) /**< \brief (PIOE) Output Write Enable */
  #define REG_PIOE_OWDR                     (0x400E16A4U) /**< \brief (PIOE) Output Write Disable */
  #define REG_PIOE_OWSR                     (0x400E16A8U) /**< \brief (PIOE) Output Write Status Register */
  #define REG_PIOE_AIMER                    (0x400E16B0U) /**< \brief (PIOE) Additional Interrupt Modes Enable Register */
  #define REG_PIOE_AIMDR                    (0x400E16B4U) /**< \brief (PIOE) Additional Interrupt Modes Disable Register */
  #define REG_PIOE_AIMMR                    (0x400E16B8U) /**< \brief (PIOE) Additional Interrupt Modes Mask Register */
  #define REG_PIOE_ESR                      (0x400E16C0U) /**< \brief (PIOE) Edge Select Register */
  #define REG_PIOE_LSR                      (0x400E16C4U) /**< \brief (PIOE) Level Select Register */
  #define REG_PIOE_ELSR                     (0x400E16C8U) /**< \brief (PIOE) Edge/Level Status Register */
  #define REG_PIOE_FELLSR                   (0x400E16D0U) /**< \brief (PIOE) Falling Edge/Low-Level Select Register */
  #define REG_PIOE_REHLSR                   (0x400E16D4U) /**< \brief (PIOE) Rising Edge/High-Level Select Register */
  #define REG_PIOE_FRLHSR                   (0x400E16D8U) /**< \brief (PIOE) Fall/Rise - Low/High Status Register */
  #define REG_PIOE_LOCKSR                   (0x400E16E0U) /**< \brief (PIOE) Lock Status */
  #define REG_PIOE_WPMR                     (0x400E16E4U) /**< \brief (PIOE) Write Protection Mode Register */
  #define REG_PIOE_WPSR                     (0x400E16E8U) /**< \brief (PIOE) Write Protection Status Register */
  #define REG_PIOE_VERSION                  (0x400E16FCU) /**< \brief (PIOE) Version Register */
  #define REG_PIOE_SCHMITT                  (0x400E1700U) /**< \brief (PIOE) Schmitt Trigger Register */
  #define REG_PIOE_DRIVER                   (0x400E1718U) /**< \brief (PIOE) PIO I/O Drive Register */
  #define REG_PIOE_KER                      (0x400E1720U) /**< \brief (PIOE) Keypad Controller Enable Register */
  #define REG_PIOE_KRCR                     (0x400E1724U) /**< \brief (PIOE) Keypad Controller Row Column Register */
  #define REG_PIOE_KDR                      (0x400E1728U) /**< \brief (PIOE) Keypad Controller Debouncing Register */
  #define REG_PIOE_KIER                     (0x400E1730U) /**< \brief (PIOE) Keypad Controller Interrupt Enable Register */
  #define REG_PIOE_KIDR                     (0x400E1734U) /**< \brief (PIOE) Keypad Controller Interrupt Disable Register */
  #define REG_PIOE_KIMR                     (0x400E1738U) /**< \brief (PIOE) Keypad Controller Interrupt Mask Register */
  #define REG_PIOE_KSR                      (0x400E173CU) /**< \brief (PIOE) Keypad Controller Status Register */
  #define REG_PIOE_KKPR                     (0x400E1740U) /**< \brief (PIOE) Keypad Controller Key Press Register */
  #define REG_PIOE_KKRR                     (0x400E1744U) /**< \brief (PIOE) Keypad Controller Key Release Register */
  #define REG_PIOE_PCMR                     (0x400E1750U) /**< \brief (PIOE) Parallel Capture Mode Register */
  #define REG_PIOE_PCIER                    (0x400E1754U) /**< \brief (PIOE) Parallel Capture Interrupt Enable Register */
  #define REG_PIOE_PCIDR                    (0x400E1758U) /**< \brief (PIOE) Parallel Capture Interrupt Disable Register */
  #define REG_PIOE_PCIMR                    (0x400E175CU) /**< \brief (PIOE) Parallel Capture Interrupt Mask Register */
  #define REG_PIOE_PCISR                    (0x400E1760U) /**< \brief (PIOE) Parallel Capture Interrupt Status Register */
  #define REG_PIOE_PCRHR                    (0x400E1764U) /**< \brief (PIOE) Parallel Capture Reception Holding Register */
#else
  #define REG_PIOE_PER     (*(__O  uint32_t*)0x400E1600U) /**< \brief (PIOE) PIO Enable Register */
  #define REG_PIOE_PDR     (*(__O  uint32_t*)0x400E1604U) /**< \brief (PIOE) PIO Disable Register */
  #define REG_PIOE_PSR     (*(__I  uint32_t*)0x400E1608U) /**< \brief (PIOE) PIO Status Register */
  #define REG_PIOE_OER     (*(__O  uint32_t*)0x400E1610U) /**< \brief (PIOE) Output Enable Register */
  #define REG_PIOE_ODR     (*(__O  uint32_t*)0x400E1614U) /**< \brief (PIOE) Output Disable Register */
  #define REG_PIOE_OSR     (*(__I  uint32_t*)0x400E1618U) /**< \brief (PIOE) Output Status Register */
  #define REG_PIOE_IFER    (*(__O  uint32_t*)0x400E1620U) /**< \brief (PIOE) Glitch Input Filter Enable Register */
  #define REG_PIOE_IFDR    (*(__O  uint32_t*)0x400E1624U) /**< \brief (PIOE) Glitch Input Filter Disable Register */
  #define REG_PIOE_IFSR    (*(__I  uint32_t*)0x400E1628U) /**< \brief (PIOE) Glitch Input Filter Status Register */
  #define REG_PIOE_SODR    (*(__O  uint32_t*)0x400E1630U) /**< \brief (PIOE) Set Output Data Register */
  #define REG_PIOE_CODR    (*(__O  uint32_t*)0x400E1634U) /**< \brief (PIOE) Clear Output Data Register */
  #define REG_PIOE_ODSR    (*(__IO uint32_t*)0x400E1638U) /**< \brief (PIOE) Output Data Status Register */
  #define REG_PIOE_PDSR    (*(__I  uint32_t*)0x400E163CU) /**< \brief (PIOE) Pin Data Status Register */
  #define REG_PIOE_IER     (*(__O  uint32_t*)0x400E1640U) /**< \brief (PIOE) Interrupt Enable Register */
  #define REG_PIOE_IDR     (*(__O  uint32_t*)0x400E1644U) /**< \brief (PIOE) Interrupt Disable Register */
  #define REG_PIOE_IMR     (*(__I  uint32_t*)0x400E1648U) /**< \brief (PIOE) Interrupt Mask Register */
  #define REG_PIOE_ISR     (*(__I  uint32_t*)0x400E164CU) /**< \brief (PIOE) Interrupt Status Register */
  #define REG_PIOE_MDER    (*(__O  uint32_t*)0x400E1650U) /**< \brief (PIOE) Multi-driver Enable Register */
  #define REG_PIOE_MDDR    (*(__O  uint32_t*)0x400E1654U) /**< \brief (PIOE) Multi-driver Disable Register */
  #define REG_PIOE_MDSR    (*(__I  uint32_t*)0x400E1658U) /**< \brief (PIOE) Multi-driver Status Register */
  #define REG_PIOE_PUDR    (*(__O  uint32_t*)0x400E1660U) /**< \brief (PIOE) Pull-up Disable Register */
  #define REG_PIOE_PUER    (*(__O  uint32_t*)0x400E1664U) /**< \brief (PIOE) Pull-up Enable Register */
  #define REG_PIOE_PUSR    (*(__I  uint32_t*)0x400E1668U) /**< \brief (PIOE) Pad Pull-up Status Register */
  #define REG_PIOE_ABCDSR  (*(__IO uint32_t*)0x400E1670U) /**< \brief (PIOE) Peripheral Select Register */
  #define REG_PIOE_IFSCDR  (*(__O  uint32_t*)0x400E1680U) /**< \brief (PIOE) Input Filter Slow Clock Disable Register */
  #define REG_PIOE_IFSCER  (*(__O  uint32_t*)0x400E1684U) /**< \brief (PIOE) Input Filter Slow Clock Enable Register */
  #define REG_PIOE_IFSCSR  (*(__I  uint32_t*)0x400E1688U) /**< \brief (PIOE) Input Filter Slow Clock Status Register */
  #define REG_PIOE_SCDR    (*(__IO uint32_t*)0x400E168CU) /**< \brief (PIOE) Slow Clock Divider Debouncing Register */
  #define REG_PIOE_PPDDR   (*(__O  uint32_t*)0x400E1690U) /**< \brief (PIOE) Pad Pull-down Disable Register */
  #define REG_PIOE_PPDER   (*(__O  uint32_t*)0x400E1694U) /**< \brief (PIOE) Pad Pull-down Enable Register */
  #define REG_PIOE_PPDSR   (*(__I  uint32_t*)0x400E1698U) /**< \brief (PIOE) Pad Pull-down Status Register */
  #define REG_PIOE_OWER    (*(__O  uint32_t*)0x400E16A0U) /**< \brief (PIOE) Output Write Enable */
  #define REG_PIOE_OWDR    (*(__O  uint32_t*)0x400E16A4U) /**< \brief (PIOE) Output Write Disable */
  #define REG_PIOE_OWSR    (*(__I  uint32_t*)0x400E16A8U) /**< \brief (PIOE) Output Write Status Register */
  #define REG_PIOE_AIMER   (*(__O  uint32_t*)0x400E16B0U) /**< \brief (PIOE) Additional Interrupt Modes Enable Register */
  #define REG_PIOE_AIMDR   (*(__O  uint32_t*)0x400E16B4U) /**< \brief (PIOE) Additional Interrupt Modes Disable Register */
  #define REG_PIOE_AIMMR   (*(__I  uint32_t*)0x400E16B8U) /**< \brief (PIOE) Additional Interrupt Modes Mask Register */
  #define REG_PIOE_ESR     (*(__O  uint32_t*)0x400E16C0U) /**< \brief (PIOE) Edge Select Register */
  #define REG_PIOE_LSR     (*(__O  uint32_t*)0x400E16C4U) /**< \brief (PIOE) Level Select Register */
  #define REG_PIOE_ELSR    (*(__I  uint32_t*)0x400E16C8U) /**< \brief (PIOE) Edge/Level Status Register */
  #define REG_PIOE_FELLSR  (*(__O  uint32_t*)0x400E16D0U) /**< \brief (PIOE) Falling Edge/Low-Level Select Register */
  #define REG_PIOE_REHLSR  (*(__O  uint32_t*)0x400E16D4U) /**< \brief (PIOE) Rising Edge/High-Level Select Register */
  #define REG_PIOE_FRLHSR  (*(__I  uint32_t*)0x400E16D8U) /**< \brief (PIOE) Fall/Rise - Low/High Status Register */
  #define REG_PIOE_LOCKSR  (*(__I  uint32_t*)0x400E16E0U) /**< \brief (PIOE) Lock Status */
  #define REG_PIOE_WPMR    (*(__IO uint32_t*)0x400E16E4U) /**< \brief (PIOE) Write Protection Mode Register */
  #define REG_PIOE_WPSR    (*(__I  uint32_t*)0x400E16E8U) /**< \brief (PIOE) Write Protection Status Register */
  #define REG_PIOE_VERSION (*(__I  uint32_t*)0x400E16FCU) /**< \brief (PIOE) Version Register */
  #define REG_PIOE_SCHMITT (*(__IO uint32_t*)0x400E1700U) /**< \brief (PIOE) Schmitt Trigger Register */
  #define REG_PIOE_DRIVER  (*(__IO uint32_t*)0x400E1718U) /**< \brief (PIOE) PIO I/O Drive Register */
  #define REG_PIOE_KER     (*(__IO uint32_t*)0x400E1720U) /**< \brief (PIOE) Keypad Controller Enable Register */
  #define REG_PIOE_KRCR    (*(__IO uint32_t*)0x400E1724U) /**< \brief (PIOE) Keypad Controller Row Column Register */
  #define REG_PIOE_KDR     (*(__IO uint32_t*)0x400E1728U) /**< \brief (PIOE) Keypad Controller Debouncing Register */
  #define REG_PIOE_KIER    (*(__O  uint32_t*)0x400E1730U) /**< \brief (PIOE) Keypad Controller Interrupt Enable Register */
  #define REG_PIOE_KIDR    (*(__O  uint32_t*)0x400E1734U) /**< \brief (PIOE) Keypad Controller Interrupt Disable Register */
  #define REG_PIOE_KIMR    (*(__I  uint32_t*)0x400E1738U) /**< \brief (PIOE) Keypad Controller Interrupt Mask Register */
  #define REG_PIOE_KSR     (*(__I  uint32_t*)0x400E173CU) /**< \brief (PIOE) Keypad Controller Status Register */
  #define REG_PIOE_KKPR    (*(__I  uint32_t*)0x400E1740U) /**< \brief (PIOE) Keypad Controller Key Press Register */
  #define REG_PIOE_KKRR    (*(__I  uint32_t*)0x400E1744U) /**< \brief (PIOE) Keypad Controller Key Release Register */
  #define REG_PIOE_PCMR    (*(__IO uint32_t*)0x400E1750U) /**< \brief (PIOE) Parallel Capture Mode Register */
  #define REG_PIOE_PCIER   (*(__O  uint32_t*)0x400E1754U) /**< \brief (PIOE) Parallel Capture Interrupt Enable Register */
  #define REG_PIOE_PCIDR   (*(__O  uint32_t*)0x400E1758U) /**< \brief (PIOE) Parallel Capture Interrupt Disable Register */
  #define REG_PIOE_PCIMR   (*(__I  uint32_t*)0x400E175CU) /**< \brief (PIOE) Parallel Capture Interrupt Mask Register */
  #define REG_PIOE_PCISR   (*(__I  uint32_t*)0x400E1760U) /**< \brief (PIOE) Parallel Capture Interrupt Status Register */
  #define REG_PIOE_PCRHR   (*(__I  uint32_t*)0x400E1764U) /**< \brief (PIOE) Parallel Capture Reception Holding Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_PIOE_INSTANCE_ */
