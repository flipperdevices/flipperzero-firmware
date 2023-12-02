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

#ifndef _SAME70_PIOB_INSTANCE_
#define _SAME70_PIOB_INSTANCE_

/* ========== Register definition for PIOB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PIOB_PER                      (0x400E1000U) /**< \brief (PIOB) PIO Enable Register */
  #define REG_PIOB_PDR                      (0x400E1004U) /**< \brief (PIOB) PIO Disable Register */
  #define REG_PIOB_PSR                      (0x400E1008U) /**< \brief (PIOB) PIO Status Register */
  #define REG_PIOB_OER                      (0x400E1010U) /**< \brief (PIOB) Output Enable Register */
  #define REG_PIOB_ODR                      (0x400E1014U) /**< \brief (PIOB) Output Disable Register */
  #define REG_PIOB_OSR                      (0x400E1018U) /**< \brief (PIOB) Output Status Register */
  #define REG_PIOB_IFER                     (0x400E1020U) /**< \brief (PIOB) Glitch Input Filter Enable Register */
  #define REG_PIOB_IFDR                     (0x400E1024U) /**< \brief (PIOB) Glitch Input Filter Disable Register */
  #define REG_PIOB_IFSR                     (0x400E1028U) /**< \brief (PIOB) Glitch Input Filter Status Register */
  #define REG_PIOB_SODR                     (0x400E1030U) /**< \brief (PIOB) Set Output Data Register */
  #define REG_PIOB_CODR                     (0x400E1034U) /**< \brief (PIOB) Clear Output Data Register */
  #define REG_PIOB_ODSR                     (0x400E1038U) /**< \brief (PIOB) Output Data Status Register */
  #define REG_PIOB_PDSR                     (0x400E103CU) /**< \brief (PIOB) Pin Data Status Register */
  #define REG_PIOB_IER                      (0x400E1040U) /**< \brief (PIOB) Interrupt Enable Register */
  #define REG_PIOB_IDR                      (0x400E1044U) /**< \brief (PIOB) Interrupt Disable Register */
  #define REG_PIOB_IMR                      (0x400E1048U) /**< \brief (PIOB) Interrupt Mask Register */
  #define REG_PIOB_ISR                      (0x400E104CU) /**< \brief (PIOB) Interrupt Status Register */
  #define REG_PIOB_MDER                     (0x400E1050U) /**< \brief (PIOB) Multi-driver Enable Register */
  #define REG_PIOB_MDDR                     (0x400E1054U) /**< \brief (PIOB) Multi-driver Disable Register */
  #define REG_PIOB_MDSR                     (0x400E1058U) /**< \brief (PIOB) Multi-driver Status Register */
  #define REG_PIOB_PUDR                     (0x400E1060U) /**< \brief (PIOB) Pull-up Disable Register */
  #define REG_PIOB_PUER                     (0x400E1064U) /**< \brief (PIOB) Pull-up Enable Register */
  #define REG_PIOB_PUSR                     (0x400E1068U) /**< \brief (PIOB) Pad Pull-up Status Register */
  #define REG_PIOB_ABCDSR                   (0x400E1070U) /**< \brief (PIOB) Peripheral Select Register */
  #define REG_PIOB_IFSCDR                   (0x400E1080U) /**< \brief (PIOB) Input Filter Slow Clock Disable Register */
  #define REG_PIOB_IFSCER                   (0x400E1084U) /**< \brief (PIOB) Input Filter Slow Clock Enable Register */
  #define REG_PIOB_IFSCSR                   (0x400E1088U) /**< \brief (PIOB) Input Filter Slow Clock Status Register */
  #define REG_PIOB_SCDR                     (0x400E108CU) /**< \brief (PIOB) Slow Clock Divider Debouncing Register */
  #define REG_PIOB_PPDDR                    (0x400E1090U) /**< \brief (PIOB) Pad Pull-down Disable Register */
  #define REG_PIOB_PPDER                    (0x400E1094U) /**< \brief (PIOB) Pad Pull-down Enable Register */
  #define REG_PIOB_PPDSR                    (0x400E1098U) /**< \brief (PIOB) Pad Pull-down Status Register */
  #define REG_PIOB_OWER                     (0x400E10A0U) /**< \brief (PIOB) Output Write Enable */
  #define REG_PIOB_OWDR                     (0x400E10A4U) /**< \brief (PIOB) Output Write Disable */
  #define REG_PIOB_OWSR                     (0x400E10A8U) /**< \brief (PIOB) Output Write Status Register */
  #define REG_PIOB_AIMER                    (0x400E10B0U) /**< \brief (PIOB) Additional Interrupt Modes Enable Register */
  #define REG_PIOB_AIMDR                    (0x400E10B4U) /**< \brief (PIOB) Additional Interrupt Modes Disable Register */
  #define REG_PIOB_AIMMR                    (0x400E10B8U) /**< \brief (PIOB) Additional Interrupt Modes Mask Register */
  #define REG_PIOB_ESR                      (0x400E10C0U) /**< \brief (PIOB) Edge Select Register */
  #define REG_PIOB_LSR                      (0x400E10C4U) /**< \brief (PIOB) Level Select Register */
  #define REG_PIOB_ELSR                     (0x400E10C8U) /**< \brief (PIOB) Edge/Level Status Register */
  #define REG_PIOB_FELLSR                   (0x400E10D0U) /**< \brief (PIOB) Falling Edge/Low-Level Select Register */
  #define REG_PIOB_REHLSR                   (0x400E10D4U) /**< \brief (PIOB) Rising Edge/High-Level Select Register */
  #define REG_PIOB_FRLHSR                   (0x400E10D8U) /**< \brief (PIOB) Fall/Rise - Low/High Status Register */
  #define REG_PIOB_LOCKSR                   (0x400E10E0U) /**< \brief (PIOB) Lock Status */
  #define REG_PIOB_WPMR                     (0x400E10E4U) /**< \brief (PIOB) Write Protection Mode Register */
  #define REG_PIOB_WPSR                     (0x400E10E8U) /**< \brief (PIOB) Write Protection Status Register */
  #define REG_PIOB_VERSION                  (0x400E10FCU) /**< \brief (PIOB) Version Register */
  #define REG_PIOB_SCHMITT                  (0x400E1100U) /**< \brief (PIOB) Schmitt Trigger Register */
  #define REG_PIOB_DRIVER                   (0x400E1118U) /**< \brief (PIOB) PIO I/O Drive Register */
  #define REG_PIOB_KER                      (0x400E1120U) /**< \brief (PIOB) Keypad Controller Enable Register */
  #define REG_PIOB_KRCR                     (0x400E1124U) /**< \brief (PIOB) Keypad Controller Row Column Register */
  #define REG_PIOB_KDR                      (0x400E1128U) /**< \brief (PIOB) Keypad Controller Debouncing Register */
  #define REG_PIOB_KIER                     (0x400E1130U) /**< \brief (PIOB) Keypad Controller Interrupt Enable Register */
  #define REG_PIOB_KIDR                     (0x400E1134U) /**< \brief (PIOB) Keypad Controller Interrupt Disable Register */
  #define REG_PIOB_KIMR                     (0x400E1138U) /**< \brief (PIOB) Keypad Controller Interrupt Mask Register */
  #define REG_PIOB_KSR                      (0x400E113CU) /**< \brief (PIOB) Keypad Controller Status Register */
  #define REG_PIOB_KKPR                     (0x400E1140U) /**< \brief (PIOB) Keypad Controller Key Press Register */
  #define REG_PIOB_KKRR                     (0x400E1144U) /**< \brief (PIOB) Keypad Controller Key Release Register */
  #define REG_PIOB_PCMR                     (0x400E1150U) /**< \brief (PIOB) Parallel Capture Mode Register */
  #define REG_PIOB_PCIER                    (0x400E1154U) /**< \brief (PIOB) Parallel Capture Interrupt Enable Register */
  #define REG_PIOB_PCIDR                    (0x400E1158U) /**< \brief (PIOB) Parallel Capture Interrupt Disable Register */
  #define REG_PIOB_PCIMR                    (0x400E115CU) /**< \brief (PIOB) Parallel Capture Interrupt Mask Register */
  #define REG_PIOB_PCISR                    (0x400E1160U) /**< \brief (PIOB) Parallel Capture Interrupt Status Register */
  #define REG_PIOB_PCRHR                    (0x400E1164U) /**< \brief (PIOB) Parallel Capture Reception Holding Register */
#else
  #define REG_PIOB_PER     (*(__O  uint32_t*)0x400E1000U) /**< \brief (PIOB) PIO Enable Register */
  #define REG_PIOB_PDR     (*(__O  uint32_t*)0x400E1004U) /**< \brief (PIOB) PIO Disable Register */
  #define REG_PIOB_PSR     (*(__I  uint32_t*)0x400E1008U) /**< \brief (PIOB) PIO Status Register */
  #define REG_PIOB_OER     (*(__O  uint32_t*)0x400E1010U) /**< \brief (PIOB) Output Enable Register */
  #define REG_PIOB_ODR     (*(__O  uint32_t*)0x400E1014U) /**< \brief (PIOB) Output Disable Register */
  #define REG_PIOB_OSR     (*(__I  uint32_t*)0x400E1018U) /**< \brief (PIOB) Output Status Register */
  #define REG_PIOB_IFER    (*(__O  uint32_t*)0x400E1020U) /**< \brief (PIOB) Glitch Input Filter Enable Register */
  #define REG_PIOB_IFDR    (*(__O  uint32_t*)0x400E1024U) /**< \brief (PIOB) Glitch Input Filter Disable Register */
  #define REG_PIOB_IFSR    (*(__I  uint32_t*)0x400E1028U) /**< \brief (PIOB) Glitch Input Filter Status Register */
  #define REG_PIOB_SODR    (*(__O  uint32_t*)0x400E1030U) /**< \brief (PIOB) Set Output Data Register */
  #define REG_PIOB_CODR    (*(__O  uint32_t*)0x400E1034U) /**< \brief (PIOB) Clear Output Data Register */
  #define REG_PIOB_ODSR    (*(__IO uint32_t*)0x400E1038U) /**< \brief (PIOB) Output Data Status Register */
  #define REG_PIOB_PDSR    (*(__I  uint32_t*)0x400E103CU) /**< \brief (PIOB) Pin Data Status Register */
  #define REG_PIOB_IER     (*(__O  uint32_t*)0x400E1040U) /**< \brief (PIOB) Interrupt Enable Register */
  #define REG_PIOB_IDR     (*(__O  uint32_t*)0x400E1044U) /**< \brief (PIOB) Interrupt Disable Register */
  #define REG_PIOB_IMR     (*(__I  uint32_t*)0x400E1048U) /**< \brief (PIOB) Interrupt Mask Register */
  #define REG_PIOB_ISR     (*(__I  uint32_t*)0x400E104CU) /**< \brief (PIOB) Interrupt Status Register */
  #define REG_PIOB_MDER    (*(__O  uint32_t*)0x400E1050U) /**< \brief (PIOB) Multi-driver Enable Register */
  #define REG_PIOB_MDDR    (*(__O  uint32_t*)0x400E1054U) /**< \brief (PIOB) Multi-driver Disable Register */
  #define REG_PIOB_MDSR    (*(__I  uint32_t*)0x400E1058U) /**< \brief (PIOB) Multi-driver Status Register */
  #define REG_PIOB_PUDR    (*(__O  uint32_t*)0x400E1060U) /**< \brief (PIOB) Pull-up Disable Register */
  #define REG_PIOB_PUER    (*(__O  uint32_t*)0x400E1064U) /**< \brief (PIOB) Pull-up Enable Register */
  #define REG_PIOB_PUSR    (*(__I  uint32_t*)0x400E1068U) /**< \brief (PIOB) Pad Pull-up Status Register */
  #define REG_PIOB_ABCDSR  (*(__IO uint32_t*)0x400E1070U) /**< \brief (PIOB) Peripheral Select Register */
  #define REG_PIOB_IFSCDR  (*(__O  uint32_t*)0x400E1080U) /**< \brief (PIOB) Input Filter Slow Clock Disable Register */
  #define REG_PIOB_IFSCER  (*(__O  uint32_t*)0x400E1084U) /**< \brief (PIOB) Input Filter Slow Clock Enable Register */
  #define REG_PIOB_IFSCSR  (*(__I  uint32_t*)0x400E1088U) /**< \brief (PIOB) Input Filter Slow Clock Status Register */
  #define REG_PIOB_SCDR    (*(__IO uint32_t*)0x400E108CU) /**< \brief (PIOB) Slow Clock Divider Debouncing Register */
  #define REG_PIOB_PPDDR   (*(__O  uint32_t*)0x400E1090U) /**< \brief (PIOB) Pad Pull-down Disable Register */
  #define REG_PIOB_PPDER   (*(__O  uint32_t*)0x400E1094U) /**< \brief (PIOB) Pad Pull-down Enable Register */
  #define REG_PIOB_PPDSR   (*(__I  uint32_t*)0x400E1098U) /**< \brief (PIOB) Pad Pull-down Status Register */
  #define REG_PIOB_OWER    (*(__O  uint32_t*)0x400E10A0U) /**< \brief (PIOB) Output Write Enable */
  #define REG_PIOB_OWDR    (*(__O  uint32_t*)0x400E10A4U) /**< \brief (PIOB) Output Write Disable */
  #define REG_PIOB_OWSR    (*(__I  uint32_t*)0x400E10A8U) /**< \brief (PIOB) Output Write Status Register */
  #define REG_PIOB_AIMER   (*(__O  uint32_t*)0x400E10B0U) /**< \brief (PIOB) Additional Interrupt Modes Enable Register */
  #define REG_PIOB_AIMDR   (*(__O  uint32_t*)0x400E10B4U) /**< \brief (PIOB) Additional Interrupt Modes Disable Register */
  #define REG_PIOB_AIMMR   (*(__I  uint32_t*)0x400E10B8U) /**< \brief (PIOB) Additional Interrupt Modes Mask Register */
  #define REG_PIOB_ESR     (*(__O  uint32_t*)0x400E10C0U) /**< \brief (PIOB) Edge Select Register */
  #define REG_PIOB_LSR     (*(__O  uint32_t*)0x400E10C4U) /**< \brief (PIOB) Level Select Register */
  #define REG_PIOB_ELSR    (*(__I  uint32_t*)0x400E10C8U) /**< \brief (PIOB) Edge/Level Status Register */
  #define REG_PIOB_FELLSR  (*(__O  uint32_t*)0x400E10D0U) /**< \brief (PIOB) Falling Edge/Low-Level Select Register */
  #define REG_PIOB_REHLSR  (*(__O  uint32_t*)0x400E10D4U) /**< \brief (PIOB) Rising Edge/High-Level Select Register */
  #define REG_PIOB_FRLHSR  (*(__I  uint32_t*)0x400E10D8U) /**< \brief (PIOB) Fall/Rise - Low/High Status Register */
  #define REG_PIOB_LOCKSR  (*(__I  uint32_t*)0x400E10E0U) /**< \brief (PIOB) Lock Status */
  #define REG_PIOB_WPMR    (*(__IO uint32_t*)0x400E10E4U) /**< \brief (PIOB) Write Protection Mode Register */
  #define REG_PIOB_WPSR    (*(__I  uint32_t*)0x400E10E8U) /**< \brief (PIOB) Write Protection Status Register */
  #define REG_PIOB_VERSION (*(__I  uint32_t*)0x400E10FCU) /**< \brief (PIOB) Version Register */
  #define REG_PIOB_SCHMITT (*(__IO uint32_t*)0x400E1100U) /**< \brief (PIOB) Schmitt Trigger Register */
  #define REG_PIOB_DRIVER  (*(__IO uint32_t*)0x400E1118U) /**< \brief (PIOB) PIO I/O Drive Register */
  #define REG_PIOB_KER     (*(__IO uint32_t*)0x400E1120U) /**< \brief (PIOB) Keypad Controller Enable Register */
  #define REG_PIOB_KRCR    (*(__IO uint32_t*)0x400E1124U) /**< \brief (PIOB) Keypad Controller Row Column Register */
  #define REG_PIOB_KDR     (*(__IO uint32_t*)0x400E1128U) /**< \brief (PIOB) Keypad Controller Debouncing Register */
  #define REG_PIOB_KIER    (*(__O  uint32_t*)0x400E1130U) /**< \brief (PIOB) Keypad Controller Interrupt Enable Register */
  #define REG_PIOB_KIDR    (*(__O  uint32_t*)0x400E1134U) /**< \brief (PIOB) Keypad Controller Interrupt Disable Register */
  #define REG_PIOB_KIMR    (*(__I  uint32_t*)0x400E1138U) /**< \brief (PIOB) Keypad Controller Interrupt Mask Register */
  #define REG_PIOB_KSR     (*(__I  uint32_t*)0x400E113CU) /**< \brief (PIOB) Keypad Controller Status Register */
  #define REG_PIOB_KKPR    (*(__I  uint32_t*)0x400E1140U) /**< \brief (PIOB) Keypad Controller Key Press Register */
  #define REG_PIOB_KKRR    (*(__I  uint32_t*)0x400E1144U) /**< \brief (PIOB) Keypad Controller Key Release Register */
  #define REG_PIOB_PCMR    (*(__IO uint32_t*)0x400E1150U) /**< \brief (PIOB) Parallel Capture Mode Register */
  #define REG_PIOB_PCIER   (*(__O  uint32_t*)0x400E1154U) /**< \brief (PIOB) Parallel Capture Interrupt Enable Register */
  #define REG_PIOB_PCIDR   (*(__O  uint32_t*)0x400E1158U) /**< \brief (PIOB) Parallel Capture Interrupt Disable Register */
  #define REG_PIOB_PCIMR   (*(__I  uint32_t*)0x400E115CU) /**< \brief (PIOB) Parallel Capture Interrupt Mask Register */
  #define REG_PIOB_PCISR   (*(__I  uint32_t*)0x400E1160U) /**< \brief (PIOB) Parallel Capture Interrupt Status Register */
  #define REG_PIOB_PCRHR   (*(__I  uint32_t*)0x400E1164U) /**< \brief (PIOB) Parallel Capture Reception Holding Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_PIOB_INSTANCE_ */
