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

#ifndef _SAME70_PIOC_INSTANCE_
#define _SAME70_PIOC_INSTANCE_

/* ========== Register definition for PIOC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PIOC_PER                      (0x400E1200U) /**< \brief (PIOC) PIO Enable Register */
  #define REG_PIOC_PDR                      (0x400E1204U) /**< \brief (PIOC) PIO Disable Register */
  #define REG_PIOC_PSR                      (0x400E1208U) /**< \brief (PIOC) PIO Status Register */
  #define REG_PIOC_OER                      (0x400E1210U) /**< \brief (PIOC) Output Enable Register */
  #define REG_PIOC_ODR                      (0x400E1214U) /**< \brief (PIOC) Output Disable Register */
  #define REG_PIOC_OSR                      (0x400E1218U) /**< \brief (PIOC) Output Status Register */
  #define REG_PIOC_IFER                     (0x400E1220U) /**< \brief (PIOC) Glitch Input Filter Enable Register */
  #define REG_PIOC_IFDR                     (0x400E1224U) /**< \brief (PIOC) Glitch Input Filter Disable Register */
  #define REG_PIOC_IFSR                     (0x400E1228U) /**< \brief (PIOC) Glitch Input Filter Status Register */
  #define REG_PIOC_SODR                     (0x400E1230U) /**< \brief (PIOC) Set Output Data Register */
  #define REG_PIOC_CODR                     (0x400E1234U) /**< \brief (PIOC) Clear Output Data Register */
  #define REG_PIOC_ODSR                     (0x400E1238U) /**< \brief (PIOC) Output Data Status Register */
  #define REG_PIOC_PDSR                     (0x400E123CU) /**< \brief (PIOC) Pin Data Status Register */
  #define REG_PIOC_IER                      (0x400E1240U) /**< \brief (PIOC) Interrupt Enable Register */
  #define REG_PIOC_IDR                      (0x400E1244U) /**< \brief (PIOC) Interrupt Disable Register */
  #define REG_PIOC_IMR                      (0x400E1248U) /**< \brief (PIOC) Interrupt Mask Register */
  #define REG_PIOC_ISR                      (0x400E124CU) /**< \brief (PIOC) Interrupt Status Register */
  #define REG_PIOC_MDER                     (0x400E1250U) /**< \brief (PIOC) Multi-driver Enable Register */
  #define REG_PIOC_MDDR                     (0x400E1254U) /**< \brief (PIOC) Multi-driver Disable Register */
  #define REG_PIOC_MDSR                     (0x400E1258U) /**< \brief (PIOC) Multi-driver Status Register */
  #define REG_PIOC_PUDR                     (0x400E1260U) /**< \brief (PIOC) Pull-up Disable Register */
  #define REG_PIOC_PUER                     (0x400E1264U) /**< \brief (PIOC) Pull-up Enable Register */
  #define REG_PIOC_PUSR                     (0x400E1268U) /**< \brief (PIOC) Pad Pull-up Status Register */
  #define REG_PIOC_ABCDSR                   (0x400E1270U) /**< \brief (PIOC) Peripheral Select Register */
  #define REG_PIOC_IFSCDR                   (0x400E1280U) /**< \brief (PIOC) Input Filter Slow Clock Disable Register */
  #define REG_PIOC_IFSCER                   (0x400E1284U) /**< \brief (PIOC) Input Filter Slow Clock Enable Register */
  #define REG_PIOC_IFSCSR                   (0x400E1288U) /**< \brief (PIOC) Input Filter Slow Clock Status Register */
  #define REG_PIOC_SCDR                     (0x400E128CU) /**< \brief (PIOC) Slow Clock Divider Debouncing Register */
  #define REG_PIOC_PPDDR                    (0x400E1290U) /**< \brief (PIOC) Pad Pull-down Disable Register */
  #define REG_PIOC_PPDER                    (0x400E1294U) /**< \brief (PIOC) Pad Pull-down Enable Register */
  #define REG_PIOC_PPDSR                    (0x400E1298U) /**< \brief (PIOC) Pad Pull-down Status Register */
  #define REG_PIOC_OWER                     (0x400E12A0U) /**< \brief (PIOC) Output Write Enable */
  #define REG_PIOC_OWDR                     (0x400E12A4U) /**< \brief (PIOC) Output Write Disable */
  #define REG_PIOC_OWSR                     (0x400E12A8U) /**< \brief (PIOC) Output Write Status Register */
  #define REG_PIOC_AIMER                    (0x400E12B0U) /**< \brief (PIOC) Additional Interrupt Modes Enable Register */
  #define REG_PIOC_AIMDR                    (0x400E12B4U) /**< \brief (PIOC) Additional Interrupt Modes Disable Register */
  #define REG_PIOC_AIMMR                    (0x400E12B8U) /**< \brief (PIOC) Additional Interrupt Modes Mask Register */
  #define REG_PIOC_ESR                      (0x400E12C0U) /**< \brief (PIOC) Edge Select Register */
  #define REG_PIOC_LSR                      (0x400E12C4U) /**< \brief (PIOC) Level Select Register */
  #define REG_PIOC_ELSR                     (0x400E12C8U) /**< \brief (PIOC) Edge/Level Status Register */
  #define REG_PIOC_FELLSR                   (0x400E12D0U) /**< \brief (PIOC) Falling Edge/Low-Level Select Register */
  #define REG_PIOC_REHLSR                   (0x400E12D4U) /**< \brief (PIOC) Rising Edge/High-Level Select Register */
  #define REG_PIOC_FRLHSR                   (0x400E12D8U) /**< \brief (PIOC) Fall/Rise - Low/High Status Register */
  #define REG_PIOC_LOCKSR                   (0x400E12E0U) /**< \brief (PIOC) Lock Status */
  #define REG_PIOC_WPMR                     (0x400E12E4U) /**< \brief (PIOC) Write Protection Mode Register */
  #define REG_PIOC_WPSR                     (0x400E12E8U) /**< \brief (PIOC) Write Protection Status Register */
  #define REG_PIOC_VERSION                  (0x400E12FCU) /**< \brief (PIOC) Version Register */
  #define REG_PIOC_SCHMITT                  (0x400E1300U) /**< \brief (PIOC) Schmitt Trigger Register */
  #define REG_PIOC_DRIVER                   (0x400E1318U) /**< \brief (PIOC) PIO I/O Drive Register */
  #define REG_PIOC_KER                      (0x400E1320U) /**< \brief (PIOC) Keypad Controller Enable Register */
  #define REG_PIOC_KRCR                     (0x400E1324U) /**< \brief (PIOC) Keypad Controller Row Column Register */
  #define REG_PIOC_KDR                      (0x400E1328U) /**< \brief (PIOC) Keypad Controller Debouncing Register */
  #define REG_PIOC_KIER                     (0x400E1330U) /**< \brief (PIOC) Keypad Controller Interrupt Enable Register */
  #define REG_PIOC_KIDR                     (0x400E1334U) /**< \brief (PIOC) Keypad Controller Interrupt Disable Register */
  #define REG_PIOC_KIMR                     (0x400E1338U) /**< \brief (PIOC) Keypad Controller Interrupt Mask Register */
  #define REG_PIOC_KSR                      (0x400E133CU) /**< \brief (PIOC) Keypad Controller Status Register */
  #define REG_PIOC_KKPR                     (0x400E1340U) /**< \brief (PIOC) Keypad Controller Key Press Register */
  #define REG_PIOC_KKRR                     (0x400E1344U) /**< \brief (PIOC) Keypad Controller Key Release Register */
  #define REG_PIOC_PCMR                     (0x400E1350U) /**< \brief (PIOC) Parallel Capture Mode Register */
  #define REG_PIOC_PCIER                    (0x400E1354U) /**< \brief (PIOC) Parallel Capture Interrupt Enable Register */
  #define REG_PIOC_PCIDR                    (0x400E1358U) /**< \brief (PIOC) Parallel Capture Interrupt Disable Register */
  #define REG_PIOC_PCIMR                    (0x400E135CU) /**< \brief (PIOC) Parallel Capture Interrupt Mask Register */
  #define REG_PIOC_PCISR                    (0x400E1360U) /**< \brief (PIOC) Parallel Capture Interrupt Status Register */
  #define REG_PIOC_PCRHR                    (0x400E1364U) /**< \brief (PIOC) Parallel Capture Reception Holding Register */
#else
  #define REG_PIOC_PER     (*(__O  uint32_t*)0x400E1200U) /**< \brief (PIOC) PIO Enable Register */
  #define REG_PIOC_PDR     (*(__O  uint32_t*)0x400E1204U) /**< \brief (PIOC) PIO Disable Register */
  #define REG_PIOC_PSR     (*(__I  uint32_t*)0x400E1208U) /**< \brief (PIOC) PIO Status Register */
  #define REG_PIOC_OER     (*(__O  uint32_t*)0x400E1210U) /**< \brief (PIOC) Output Enable Register */
  #define REG_PIOC_ODR     (*(__O  uint32_t*)0x400E1214U) /**< \brief (PIOC) Output Disable Register */
  #define REG_PIOC_OSR     (*(__I  uint32_t*)0x400E1218U) /**< \brief (PIOC) Output Status Register */
  #define REG_PIOC_IFER    (*(__O  uint32_t*)0x400E1220U) /**< \brief (PIOC) Glitch Input Filter Enable Register */
  #define REG_PIOC_IFDR    (*(__O  uint32_t*)0x400E1224U) /**< \brief (PIOC) Glitch Input Filter Disable Register */
  #define REG_PIOC_IFSR    (*(__I  uint32_t*)0x400E1228U) /**< \brief (PIOC) Glitch Input Filter Status Register */
  #define REG_PIOC_SODR    (*(__O  uint32_t*)0x400E1230U) /**< \brief (PIOC) Set Output Data Register */
  #define REG_PIOC_CODR    (*(__O  uint32_t*)0x400E1234U) /**< \brief (PIOC) Clear Output Data Register */
  #define REG_PIOC_ODSR    (*(__IO uint32_t*)0x400E1238U) /**< \brief (PIOC) Output Data Status Register */
  #define REG_PIOC_PDSR    (*(__I  uint32_t*)0x400E123CU) /**< \brief (PIOC) Pin Data Status Register */
  #define REG_PIOC_IER     (*(__O  uint32_t*)0x400E1240U) /**< \brief (PIOC) Interrupt Enable Register */
  #define REG_PIOC_IDR     (*(__O  uint32_t*)0x400E1244U) /**< \brief (PIOC) Interrupt Disable Register */
  #define REG_PIOC_IMR     (*(__I  uint32_t*)0x400E1248U) /**< \brief (PIOC) Interrupt Mask Register */
  #define REG_PIOC_ISR     (*(__I  uint32_t*)0x400E124CU) /**< \brief (PIOC) Interrupt Status Register */
  #define REG_PIOC_MDER    (*(__O  uint32_t*)0x400E1250U) /**< \brief (PIOC) Multi-driver Enable Register */
  #define REG_PIOC_MDDR    (*(__O  uint32_t*)0x400E1254U) /**< \brief (PIOC) Multi-driver Disable Register */
  #define REG_PIOC_MDSR    (*(__I  uint32_t*)0x400E1258U) /**< \brief (PIOC) Multi-driver Status Register */
  #define REG_PIOC_PUDR    (*(__O  uint32_t*)0x400E1260U) /**< \brief (PIOC) Pull-up Disable Register */
  #define REG_PIOC_PUER    (*(__O  uint32_t*)0x400E1264U) /**< \brief (PIOC) Pull-up Enable Register */
  #define REG_PIOC_PUSR    (*(__I  uint32_t*)0x400E1268U) /**< \brief (PIOC) Pad Pull-up Status Register */
  #define REG_PIOC_ABCDSR  (*(__IO uint32_t*)0x400E1270U) /**< \brief (PIOC) Peripheral Select Register */
  #define REG_PIOC_IFSCDR  (*(__O  uint32_t*)0x400E1280U) /**< \brief (PIOC) Input Filter Slow Clock Disable Register */
  #define REG_PIOC_IFSCER  (*(__O  uint32_t*)0x400E1284U) /**< \brief (PIOC) Input Filter Slow Clock Enable Register */
  #define REG_PIOC_IFSCSR  (*(__I  uint32_t*)0x400E1288U) /**< \brief (PIOC) Input Filter Slow Clock Status Register */
  #define REG_PIOC_SCDR    (*(__IO uint32_t*)0x400E128CU) /**< \brief (PIOC) Slow Clock Divider Debouncing Register */
  #define REG_PIOC_PPDDR   (*(__O  uint32_t*)0x400E1290U) /**< \brief (PIOC) Pad Pull-down Disable Register */
  #define REG_PIOC_PPDER   (*(__O  uint32_t*)0x400E1294U) /**< \brief (PIOC) Pad Pull-down Enable Register */
  #define REG_PIOC_PPDSR   (*(__I  uint32_t*)0x400E1298U) /**< \brief (PIOC) Pad Pull-down Status Register */
  #define REG_PIOC_OWER    (*(__O  uint32_t*)0x400E12A0U) /**< \brief (PIOC) Output Write Enable */
  #define REG_PIOC_OWDR    (*(__O  uint32_t*)0x400E12A4U) /**< \brief (PIOC) Output Write Disable */
  #define REG_PIOC_OWSR    (*(__I  uint32_t*)0x400E12A8U) /**< \brief (PIOC) Output Write Status Register */
  #define REG_PIOC_AIMER   (*(__O  uint32_t*)0x400E12B0U) /**< \brief (PIOC) Additional Interrupt Modes Enable Register */
  #define REG_PIOC_AIMDR   (*(__O  uint32_t*)0x400E12B4U) /**< \brief (PIOC) Additional Interrupt Modes Disable Register */
  #define REG_PIOC_AIMMR   (*(__I  uint32_t*)0x400E12B8U) /**< \brief (PIOC) Additional Interrupt Modes Mask Register */
  #define REG_PIOC_ESR     (*(__O  uint32_t*)0x400E12C0U) /**< \brief (PIOC) Edge Select Register */
  #define REG_PIOC_LSR     (*(__O  uint32_t*)0x400E12C4U) /**< \brief (PIOC) Level Select Register */
  #define REG_PIOC_ELSR    (*(__I  uint32_t*)0x400E12C8U) /**< \brief (PIOC) Edge/Level Status Register */
  #define REG_PIOC_FELLSR  (*(__O  uint32_t*)0x400E12D0U) /**< \brief (PIOC) Falling Edge/Low-Level Select Register */
  #define REG_PIOC_REHLSR  (*(__O  uint32_t*)0x400E12D4U) /**< \brief (PIOC) Rising Edge/High-Level Select Register */
  #define REG_PIOC_FRLHSR  (*(__I  uint32_t*)0x400E12D8U) /**< \brief (PIOC) Fall/Rise - Low/High Status Register */
  #define REG_PIOC_LOCKSR  (*(__I  uint32_t*)0x400E12E0U) /**< \brief (PIOC) Lock Status */
  #define REG_PIOC_WPMR    (*(__IO uint32_t*)0x400E12E4U) /**< \brief (PIOC) Write Protection Mode Register */
  #define REG_PIOC_WPSR    (*(__I  uint32_t*)0x400E12E8U) /**< \brief (PIOC) Write Protection Status Register */
  #define REG_PIOC_VERSION (*(__I  uint32_t*)0x400E12FCU) /**< \brief (PIOC) Version Register */
  #define REG_PIOC_SCHMITT (*(__IO uint32_t*)0x400E1300U) /**< \brief (PIOC) Schmitt Trigger Register */
  #define REG_PIOC_DRIVER  (*(__IO uint32_t*)0x400E1318U) /**< \brief (PIOC) PIO I/O Drive Register */
  #define REG_PIOC_KER     (*(__IO uint32_t*)0x400E1320U) /**< \brief (PIOC) Keypad Controller Enable Register */
  #define REG_PIOC_KRCR    (*(__IO uint32_t*)0x400E1324U) /**< \brief (PIOC) Keypad Controller Row Column Register */
  #define REG_PIOC_KDR     (*(__IO uint32_t*)0x400E1328U) /**< \brief (PIOC) Keypad Controller Debouncing Register */
  #define REG_PIOC_KIER    (*(__O  uint32_t*)0x400E1330U) /**< \brief (PIOC) Keypad Controller Interrupt Enable Register */
  #define REG_PIOC_KIDR    (*(__O  uint32_t*)0x400E1334U) /**< \brief (PIOC) Keypad Controller Interrupt Disable Register */
  #define REG_PIOC_KIMR    (*(__I  uint32_t*)0x400E1338U) /**< \brief (PIOC) Keypad Controller Interrupt Mask Register */
  #define REG_PIOC_KSR     (*(__I  uint32_t*)0x400E133CU) /**< \brief (PIOC) Keypad Controller Status Register */
  #define REG_PIOC_KKPR    (*(__I  uint32_t*)0x400E1340U) /**< \brief (PIOC) Keypad Controller Key Press Register */
  #define REG_PIOC_KKRR    (*(__I  uint32_t*)0x400E1344U) /**< \brief (PIOC) Keypad Controller Key Release Register */
  #define REG_PIOC_PCMR    (*(__IO uint32_t*)0x400E1350U) /**< \brief (PIOC) Parallel Capture Mode Register */
  #define REG_PIOC_PCIER   (*(__O  uint32_t*)0x400E1354U) /**< \brief (PIOC) Parallel Capture Interrupt Enable Register */
  #define REG_PIOC_PCIDR   (*(__O  uint32_t*)0x400E1358U) /**< \brief (PIOC) Parallel Capture Interrupt Disable Register */
  #define REG_PIOC_PCIMR   (*(__I  uint32_t*)0x400E135CU) /**< \brief (PIOC) Parallel Capture Interrupt Mask Register */
  #define REG_PIOC_PCISR   (*(__I  uint32_t*)0x400E1360U) /**< \brief (PIOC) Parallel Capture Interrupt Status Register */
  #define REG_PIOC_PCRHR   (*(__I  uint32_t*)0x400E1364U) /**< \brief (PIOC) Parallel Capture Reception Holding Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_PIOC_INSTANCE_ */
