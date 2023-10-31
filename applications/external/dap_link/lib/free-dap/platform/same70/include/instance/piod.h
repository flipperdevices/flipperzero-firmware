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

#ifndef _SAME70_PIOD_INSTANCE_
#define _SAME70_PIOD_INSTANCE_

/* ========== Register definition for PIOD peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PIOD_PER                      (0x400E1400U) /**< \brief (PIOD) PIO Enable Register */
  #define REG_PIOD_PDR                      (0x400E1404U) /**< \brief (PIOD) PIO Disable Register */
  #define REG_PIOD_PSR                      (0x400E1408U) /**< \brief (PIOD) PIO Status Register */
  #define REG_PIOD_OER                      (0x400E1410U) /**< \brief (PIOD) Output Enable Register */
  #define REG_PIOD_ODR                      (0x400E1414U) /**< \brief (PIOD) Output Disable Register */
  #define REG_PIOD_OSR                      (0x400E1418U) /**< \brief (PIOD) Output Status Register */
  #define REG_PIOD_IFER                     (0x400E1420U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
  #define REG_PIOD_IFDR                     (0x400E1424U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
  #define REG_PIOD_IFSR                     (0x400E1428U) /**< \brief (PIOD) Glitch Input Filter Status Register */
  #define REG_PIOD_SODR                     (0x400E1430U) /**< \brief (PIOD) Set Output Data Register */
  #define REG_PIOD_CODR                     (0x400E1434U) /**< \brief (PIOD) Clear Output Data Register */
  #define REG_PIOD_ODSR                     (0x400E1438U) /**< \brief (PIOD) Output Data Status Register */
  #define REG_PIOD_PDSR                     (0x400E143CU) /**< \brief (PIOD) Pin Data Status Register */
  #define REG_PIOD_IER                      (0x400E1440U) /**< \brief (PIOD) Interrupt Enable Register */
  #define REG_PIOD_IDR                      (0x400E1444U) /**< \brief (PIOD) Interrupt Disable Register */
  #define REG_PIOD_IMR                      (0x400E1448U) /**< \brief (PIOD) Interrupt Mask Register */
  #define REG_PIOD_ISR                      (0x400E144CU) /**< \brief (PIOD) Interrupt Status Register */
  #define REG_PIOD_MDER                     (0x400E1450U) /**< \brief (PIOD) Multi-driver Enable Register */
  #define REG_PIOD_MDDR                     (0x400E1454U) /**< \brief (PIOD) Multi-driver Disable Register */
  #define REG_PIOD_MDSR                     (0x400E1458U) /**< \brief (PIOD) Multi-driver Status Register */
  #define REG_PIOD_PUDR                     (0x400E1460U) /**< \brief (PIOD) Pull-up Disable Register */
  #define REG_PIOD_PUER                     (0x400E1464U) /**< \brief (PIOD) Pull-up Enable Register */
  #define REG_PIOD_PUSR                     (0x400E1468U) /**< \brief (PIOD) Pad Pull-up Status Register */
  #define REG_PIOD_ABCDSR                   (0x400E1470U) /**< \brief (PIOD) Peripheral Select Register */
  #define REG_PIOD_IFSCDR                   (0x400E1480U) /**< \brief (PIOD) Input Filter Slow Clock Disable Register */
  #define REG_PIOD_IFSCER                   (0x400E1484U) /**< \brief (PIOD) Input Filter Slow Clock Enable Register */
  #define REG_PIOD_IFSCSR                   (0x400E1488U) /**< \brief (PIOD) Input Filter Slow Clock Status Register */
  #define REG_PIOD_SCDR                     (0x400E148CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
  #define REG_PIOD_PPDDR                    (0x400E1490U) /**< \brief (PIOD) Pad Pull-down Disable Register */
  #define REG_PIOD_PPDER                    (0x400E1494U) /**< \brief (PIOD) Pad Pull-down Enable Register */
  #define REG_PIOD_PPDSR                    (0x400E1498U) /**< \brief (PIOD) Pad Pull-down Status Register */
  #define REG_PIOD_OWER                     (0x400E14A0U) /**< \brief (PIOD) Output Write Enable */
  #define REG_PIOD_OWDR                     (0x400E14A4U) /**< \brief (PIOD) Output Write Disable */
  #define REG_PIOD_OWSR                     (0x400E14A8U) /**< \brief (PIOD) Output Write Status Register */
  #define REG_PIOD_AIMER                    (0x400E14B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
  #define REG_PIOD_AIMDR                    (0x400E14B4U) /**< \brief (PIOD) Additional Interrupt Modes Disable Register */
  #define REG_PIOD_AIMMR                    (0x400E14B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
  #define REG_PIOD_ESR                      (0x400E14C0U) /**< \brief (PIOD) Edge Select Register */
  #define REG_PIOD_LSR                      (0x400E14C4U) /**< \brief (PIOD) Level Select Register */
  #define REG_PIOD_ELSR                     (0x400E14C8U) /**< \brief (PIOD) Edge/Level Status Register */
  #define REG_PIOD_FELLSR                   (0x400E14D0U) /**< \brief (PIOD) Falling Edge/Low-Level Select Register */
  #define REG_PIOD_REHLSR                   (0x400E14D4U) /**< \brief (PIOD) Rising Edge/High-Level Select Register */
  #define REG_PIOD_FRLHSR                   (0x400E14D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
  #define REG_PIOD_LOCKSR                   (0x400E14E0U) /**< \brief (PIOD) Lock Status */
  #define REG_PIOD_WPMR                     (0x400E14E4U) /**< \brief (PIOD) Write Protection Mode Register */
  #define REG_PIOD_WPSR                     (0x400E14E8U) /**< \brief (PIOD) Write Protection Status Register */
  #define REG_PIOD_VERSION                  (0x400E14FCU) /**< \brief (PIOD) Version Register */
  #define REG_PIOD_SCHMITT                  (0x400E1500U) /**< \brief (PIOD) Schmitt Trigger Register */
  #define REG_PIOD_DRIVER                   (0x400E1518U) /**< \brief (PIOD) PIO I/O Drive Register */
  #define REG_PIOD_KER                      (0x400E1520U) /**< \brief (PIOD) Keypad Controller Enable Register */
  #define REG_PIOD_KRCR                     (0x400E1524U) /**< \brief (PIOD) Keypad Controller Row Column Register */
  #define REG_PIOD_KDR                      (0x400E1528U) /**< \brief (PIOD) Keypad Controller Debouncing Register */
  #define REG_PIOD_KIER                     (0x400E1530U) /**< \brief (PIOD) Keypad Controller Interrupt Enable Register */
  #define REG_PIOD_KIDR                     (0x400E1534U) /**< \brief (PIOD) Keypad Controller Interrupt Disable Register */
  #define REG_PIOD_KIMR                     (0x400E1538U) /**< \brief (PIOD) Keypad Controller Interrupt Mask Register */
  #define REG_PIOD_KSR                      (0x400E153CU) /**< \brief (PIOD) Keypad Controller Status Register */
  #define REG_PIOD_KKPR                     (0x400E1540U) /**< \brief (PIOD) Keypad Controller Key Press Register */
  #define REG_PIOD_KKRR                     (0x400E1544U) /**< \brief (PIOD) Keypad Controller Key Release Register */
  #define REG_PIOD_PCMR                     (0x400E1550U) /**< \brief (PIOD) Parallel Capture Mode Register */
  #define REG_PIOD_PCIER                    (0x400E1554U) /**< \brief (PIOD) Parallel Capture Interrupt Enable Register */
  #define REG_PIOD_PCIDR                    (0x400E1558U) /**< \brief (PIOD) Parallel Capture Interrupt Disable Register */
  #define REG_PIOD_PCIMR                    (0x400E155CU) /**< \brief (PIOD) Parallel Capture Interrupt Mask Register */
  #define REG_PIOD_PCISR                    (0x400E1560U) /**< \brief (PIOD) Parallel Capture Interrupt Status Register */
  #define REG_PIOD_PCRHR                    (0x400E1564U) /**< \brief (PIOD) Parallel Capture Reception Holding Register */
#else
  #define REG_PIOD_PER     (*(__O  uint32_t*)0x400E1400U) /**< \brief (PIOD) PIO Enable Register */
  #define REG_PIOD_PDR     (*(__O  uint32_t*)0x400E1404U) /**< \brief (PIOD) PIO Disable Register */
  #define REG_PIOD_PSR     (*(__I  uint32_t*)0x400E1408U) /**< \brief (PIOD) PIO Status Register */
  #define REG_PIOD_OER     (*(__O  uint32_t*)0x400E1410U) /**< \brief (PIOD) Output Enable Register */
  #define REG_PIOD_ODR     (*(__O  uint32_t*)0x400E1414U) /**< \brief (PIOD) Output Disable Register */
  #define REG_PIOD_OSR     (*(__I  uint32_t*)0x400E1418U) /**< \brief (PIOD) Output Status Register */
  #define REG_PIOD_IFER    (*(__O  uint32_t*)0x400E1420U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
  #define REG_PIOD_IFDR    (*(__O  uint32_t*)0x400E1424U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
  #define REG_PIOD_IFSR    (*(__I  uint32_t*)0x400E1428U) /**< \brief (PIOD) Glitch Input Filter Status Register */
  #define REG_PIOD_SODR    (*(__O  uint32_t*)0x400E1430U) /**< \brief (PIOD) Set Output Data Register */
  #define REG_PIOD_CODR    (*(__O  uint32_t*)0x400E1434U) /**< \brief (PIOD) Clear Output Data Register */
  #define REG_PIOD_ODSR    (*(__IO uint32_t*)0x400E1438U) /**< \brief (PIOD) Output Data Status Register */
  #define REG_PIOD_PDSR    (*(__I  uint32_t*)0x400E143CU) /**< \brief (PIOD) Pin Data Status Register */
  #define REG_PIOD_IER     (*(__O  uint32_t*)0x400E1440U) /**< \brief (PIOD) Interrupt Enable Register */
  #define REG_PIOD_IDR     (*(__O  uint32_t*)0x400E1444U) /**< \brief (PIOD) Interrupt Disable Register */
  #define REG_PIOD_IMR     (*(__I  uint32_t*)0x400E1448U) /**< \brief (PIOD) Interrupt Mask Register */
  #define REG_PIOD_ISR     (*(__I  uint32_t*)0x400E144CU) /**< \brief (PIOD) Interrupt Status Register */
  #define REG_PIOD_MDER    (*(__O  uint32_t*)0x400E1450U) /**< \brief (PIOD) Multi-driver Enable Register */
  #define REG_PIOD_MDDR    (*(__O  uint32_t*)0x400E1454U) /**< \brief (PIOD) Multi-driver Disable Register */
  #define REG_PIOD_MDSR    (*(__I  uint32_t*)0x400E1458U) /**< \brief (PIOD) Multi-driver Status Register */
  #define REG_PIOD_PUDR    (*(__O  uint32_t*)0x400E1460U) /**< \brief (PIOD) Pull-up Disable Register */
  #define REG_PIOD_PUER    (*(__O  uint32_t*)0x400E1464U) /**< \brief (PIOD) Pull-up Enable Register */
  #define REG_PIOD_PUSR    (*(__I  uint32_t*)0x400E1468U) /**< \brief (PIOD) Pad Pull-up Status Register */
  #define REG_PIOD_ABCDSR  (*(__IO uint32_t*)0x400E1470U) /**< \brief (PIOD) Peripheral Select Register */
  #define REG_PIOD_IFSCDR  (*(__O  uint32_t*)0x400E1480U) /**< \brief (PIOD) Input Filter Slow Clock Disable Register */
  #define REG_PIOD_IFSCER  (*(__O  uint32_t*)0x400E1484U) /**< \brief (PIOD) Input Filter Slow Clock Enable Register */
  #define REG_PIOD_IFSCSR  (*(__I  uint32_t*)0x400E1488U) /**< \brief (PIOD) Input Filter Slow Clock Status Register */
  #define REG_PIOD_SCDR    (*(__IO uint32_t*)0x400E148CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
  #define REG_PIOD_PPDDR   (*(__O  uint32_t*)0x400E1490U) /**< \brief (PIOD) Pad Pull-down Disable Register */
  #define REG_PIOD_PPDER   (*(__O  uint32_t*)0x400E1494U) /**< \brief (PIOD) Pad Pull-down Enable Register */
  #define REG_PIOD_PPDSR   (*(__I  uint32_t*)0x400E1498U) /**< \brief (PIOD) Pad Pull-down Status Register */
  #define REG_PIOD_OWER    (*(__O  uint32_t*)0x400E14A0U) /**< \brief (PIOD) Output Write Enable */
  #define REG_PIOD_OWDR    (*(__O  uint32_t*)0x400E14A4U) /**< \brief (PIOD) Output Write Disable */
  #define REG_PIOD_OWSR    (*(__I  uint32_t*)0x400E14A8U) /**< \brief (PIOD) Output Write Status Register */
  #define REG_PIOD_AIMER   (*(__O  uint32_t*)0x400E14B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
  #define REG_PIOD_AIMDR   (*(__O  uint32_t*)0x400E14B4U) /**< \brief (PIOD) Additional Interrupt Modes Disable Register */
  #define REG_PIOD_AIMMR   (*(__I  uint32_t*)0x400E14B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
  #define REG_PIOD_ESR     (*(__O  uint32_t*)0x400E14C0U) /**< \brief (PIOD) Edge Select Register */
  #define REG_PIOD_LSR     (*(__O  uint32_t*)0x400E14C4U) /**< \brief (PIOD) Level Select Register */
  #define REG_PIOD_ELSR    (*(__I  uint32_t*)0x400E14C8U) /**< \brief (PIOD) Edge/Level Status Register */
  #define REG_PIOD_FELLSR  (*(__O  uint32_t*)0x400E14D0U) /**< \brief (PIOD) Falling Edge/Low-Level Select Register */
  #define REG_PIOD_REHLSR  (*(__O  uint32_t*)0x400E14D4U) /**< \brief (PIOD) Rising Edge/High-Level Select Register */
  #define REG_PIOD_FRLHSR  (*(__I  uint32_t*)0x400E14D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
  #define REG_PIOD_LOCKSR  (*(__I  uint32_t*)0x400E14E0U) /**< \brief (PIOD) Lock Status */
  #define REG_PIOD_WPMR    (*(__IO uint32_t*)0x400E14E4U) /**< \brief (PIOD) Write Protection Mode Register */
  #define REG_PIOD_WPSR    (*(__I  uint32_t*)0x400E14E8U) /**< \brief (PIOD) Write Protection Status Register */
  #define REG_PIOD_VERSION (*(__I  uint32_t*)0x400E14FCU) /**< \brief (PIOD) Version Register */
  #define REG_PIOD_SCHMITT (*(__IO uint32_t*)0x400E1500U) /**< \brief (PIOD) Schmitt Trigger Register */
  #define REG_PIOD_DRIVER  (*(__IO uint32_t*)0x400E1518U) /**< \brief (PIOD) PIO I/O Drive Register */
  #define REG_PIOD_KER     (*(__IO uint32_t*)0x400E1520U) /**< \brief (PIOD) Keypad Controller Enable Register */
  #define REG_PIOD_KRCR    (*(__IO uint32_t*)0x400E1524U) /**< \brief (PIOD) Keypad Controller Row Column Register */
  #define REG_PIOD_KDR     (*(__IO uint32_t*)0x400E1528U) /**< \brief (PIOD) Keypad Controller Debouncing Register */
  #define REG_PIOD_KIER    (*(__O  uint32_t*)0x400E1530U) /**< \brief (PIOD) Keypad Controller Interrupt Enable Register */
  #define REG_PIOD_KIDR    (*(__O  uint32_t*)0x400E1534U) /**< \brief (PIOD) Keypad Controller Interrupt Disable Register */
  #define REG_PIOD_KIMR    (*(__I  uint32_t*)0x400E1538U) /**< \brief (PIOD) Keypad Controller Interrupt Mask Register */
  #define REG_PIOD_KSR     (*(__I  uint32_t*)0x400E153CU) /**< \brief (PIOD) Keypad Controller Status Register */
  #define REG_PIOD_KKPR    (*(__I  uint32_t*)0x400E1540U) /**< \brief (PIOD) Keypad Controller Key Press Register */
  #define REG_PIOD_KKRR    (*(__I  uint32_t*)0x400E1544U) /**< \brief (PIOD) Keypad Controller Key Release Register */
  #define REG_PIOD_PCMR    (*(__IO uint32_t*)0x400E1550U) /**< \brief (PIOD) Parallel Capture Mode Register */
  #define REG_PIOD_PCIER   (*(__O  uint32_t*)0x400E1554U) /**< \brief (PIOD) Parallel Capture Interrupt Enable Register */
  #define REG_PIOD_PCIDR   (*(__O  uint32_t*)0x400E1558U) /**< \brief (PIOD) Parallel Capture Interrupt Disable Register */
  #define REG_PIOD_PCIMR   (*(__I  uint32_t*)0x400E155CU) /**< \brief (PIOD) Parallel Capture Interrupt Mask Register */
  #define REG_PIOD_PCISR   (*(__I  uint32_t*)0x400E1560U) /**< \brief (PIOD) Parallel Capture Interrupt Status Register */
  #define REG_PIOD_PCRHR   (*(__I  uint32_t*)0x400E1564U) /**< \brief (PIOD) Parallel Capture Reception Holding Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_PIOD_INSTANCE_ */
