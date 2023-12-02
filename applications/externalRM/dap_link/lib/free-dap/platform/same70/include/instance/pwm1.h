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

#ifndef _SAME70_PWM1_INSTANCE_
#define _SAME70_PWM1_INSTANCE_

/* ========== Register definition for PWM1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PWM1_CLK                       (0x4005C000U) /**< \brief (PWM1) PWM Clock Register */
  #define REG_PWM1_ENA                       (0x4005C004U) /**< \brief (PWM1) PWM Enable Register */
  #define REG_PWM1_DIS                       (0x4005C008U) /**< \brief (PWM1) PWM Disable Register */
  #define REG_PWM1_SR                        (0x4005C00CU) /**< \brief (PWM1) PWM Status Register */
  #define REG_PWM1_IER1                      (0x4005C010U) /**< \brief (PWM1) PWM Interrupt Enable Register 1 */
  #define REG_PWM1_IDR1                      (0x4005C014U) /**< \brief (PWM1) PWM Interrupt Disable Register 1 */
  #define REG_PWM1_IMR1                      (0x4005C018U) /**< \brief (PWM1) PWM Interrupt Mask Register 1 */
  #define REG_PWM1_ISR1                      (0x4005C01CU) /**< \brief (PWM1) PWM Interrupt Status Register 1 */
  #define REG_PWM1_SCM                       (0x4005C020U) /**< \brief (PWM1) PWM Sync Channels Mode Register */
  #define REG_PWM1_DMAR                      (0x4005C024U) /**< \brief (PWM1) PWM DMA Register */
  #define REG_PWM1_SCUC                      (0x4005C028U) /**< \brief (PWM1) PWM Sync Channels Update Control Register */
  #define REG_PWM1_SCUP                      (0x4005C02CU) /**< \brief (PWM1) PWM Sync Channels Update Period Register */
  #define REG_PWM1_SCUPUPD                   (0x4005C030U) /**< \brief (PWM1) PWM Sync Channels Update Period Update Register */
  #define REG_PWM1_IER2                      (0x4005C034U) /**< \brief (PWM1) PWM Interrupt Enable Register 2 */
  #define REG_PWM1_IDR2                      (0x4005C038U) /**< \brief (PWM1) PWM Interrupt Disable Register 2 */
  #define REG_PWM1_IMR2                      (0x4005C03CU) /**< \brief (PWM1) PWM Interrupt Mask Register 2 */
  #define REG_PWM1_ISR2                      (0x4005C040U) /**< \brief (PWM1) PWM Interrupt Status Register 2 */
  #define REG_PWM1_OOV                       (0x4005C044U) /**< \brief (PWM1) PWM Output Override Value Register */
  #define REG_PWM1_OS                        (0x4005C048U) /**< \brief (PWM1) PWM Output Selection Register */
  #define REG_PWM1_OSS                       (0x4005C04CU) /**< \brief (PWM1) PWM Output Selection Set Register */
  #define REG_PWM1_OSC                       (0x4005C050U) /**< \brief (PWM1) PWM Output Selection Clear Register */
  #define REG_PWM1_OSSUPD                    (0x4005C054U) /**< \brief (PWM1) PWM Output Selection Set Update Register */
  #define REG_PWM1_OSCUPD                    (0x4005C058U) /**< \brief (PWM1) PWM Output Selection Clear Update Register */
  #define REG_PWM1_FMR                       (0x4005C05CU) /**< \brief (PWM1) PWM Fault Mode Register */
  #define REG_PWM1_FSR                       (0x4005C060U) /**< \brief (PWM1) PWM Fault Status Register */
  #define REG_PWM1_FCR                       (0x4005C064U) /**< \brief (PWM1) PWM Fault Clear Register */
  #define REG_PWM1_FPV1                      (0x4005C068U) /**< \brief (PWM1) PWM Fault Protection Value Register 1 */
  #define REG_PWM1_FPE                       (0x4005C06CU) /**< \brief (PWM1) PWM Fault Protection Enable Register */
  #define REG_PWM1_ELMR                      (0x4005C07CU) /**< \brief (PWM1) PWM Event Line 0 Mode Register */
  #define REG_PWM1_SSPR                      (0x4005C0A0U) /**< \brief (PWM1) PWM Spread Spectrum Register */
  #define REG_PWM1_SSPUP                     (0x4005C0A4U) /**< \brief (PWM1) PWM Spread Spectrum Update Register */
  #define REG_PWM1_SMMR                      (0x4005C0B0U) /**< \brief (PWM1) PWM Stepper Motor Mode Register */
  #define REG_PWM1_FPV2                      (0x4005C0C0U) /**< \brief (PWM1) PWM Fault Protection Value 2 Register */
  #define REG_PWM1_WPCR                      (0x4005C0E4U) /**< \brief (PWM1) PWM Write Protection Control Register */
  #define REG_PWM1_WPSR                      (0x4005C0E8U) /**< \brief (PWM1) PWM Write Protection Status Register */
  #define REG_PWM1_VERSION                   (0x4005C0FCU) /**< \brief (PWM1) Version Register */
  #define REG_PWM1_CMPV0                     (0x4005C130U) /**< \brief (PWM1) PWM Comparison 0 Value Register */
  #define REG_PWM1_CMPVUPD0                  (0x4005C134U) /**< \brief (PWM1) PWM Comparison 0 Value Update Register */
  #define REG_PWM1_CMPM0                     (0x4005C138U) /**< \brief (PWM1) PWM Comparison 0 Mode Register */
  #define REG_PWM1_CMPMUPD0                  (0x4005C13CU) /**< \brief (PWM1) PWM Comparison 0 Mode Update Register */
  #define REG_PWM1_CMPV1                     (0x4005C140U) /**< \brief (PWM1) PWM Comparison 1 Value Register */
  #define REG_PWM1_CMPVUPD1                  (0x4005C144U) /**< \brief (PWM1) PWM Comparison 1 Value Update Register */
  #define REG_PWM1_CMPM1                     (0x4005C148U) /**< \brief (PWM1) PWM Comparison 1 Mode Register */
  #define REG_PWM1_CMPMUPD1                  (0x4005C14CU) /**< \brief (PWM1) PWM Comparison 1 Mode Update Register */
  #define REG_PWM1_CMPV2                     (0x4005C150U) /**< \brief (PWM1) PWM Comparison 2 Value Register */
  #define REG_PWM1_CMPVUPD2                  (0x4005C154U) /**< \brief (PWM1) PWM Comparison 2 Value Update Register */
  #define REG_PWM1_CMPM2                     (0x4005C158U) /**< \brief (PWM1) PWM Comparison 2 Mode Register */
  #define REG_PWM1_CMPMUPD2                  (0x4005C15CU) /**< \brief (PWM1) PWM Comparison 2 Mode Update Register */
  #define REG_PWM1_CMPV3                     (0x4005C160U) /**< \brief (PWM1) PWM Comparison 3 Value Register */
  #define REG_PWM1_CMPVUPD3                  (0x4005C164U) /**< \brief (PWM1) PWM Comparison 3 Value Update Register */
  #define REG_PWM1_CMPM3                     (0x4005C168U) /**< \brief (PWM1) PWM Comparison 3 Mode Register */
  #define REG_PWM1_CMPMUPD3                  (0x4005C16CU) /**< \brief (PWM1) PWM Comparison 3 Mode Update Register */
  #define REG_PWM1_CMPV4                     (0x4005C170U) /**< \brief (PWM1) PWM Comparison 4 Value Register */
  #define REG_PWM1_CMPVUPD4                  (0x4005C174U) /**< \brief (PWM1) PWM Comparison 4 Value Update Register */
  #define REG_PWM1_CMPM4                     (0x4005C178U) /**< \brief (PWM1) PWM Comparison 4 Mode Register */
  #define REG_PWM1_CMPMUPD4                  (0x4005C17CU) /**< \brief (PWM1) PWM Comparison 4 Mode Update Register */
  #define REG_PWM1_CMPV5                     (0x4005C180U) /**< \brief (PWM1) PWM Comparison 5 Value Register */
  #define REG_PWM1_CMPVUPD5                  (0x4005C184U) /**< \brief (PWM1) PWM Comparison 5 Value Update Register */
  #define REG_PWM1_CMPM5                     (0x4005C188U) /**< \brief (PWM1) PWM Comparison 5 Mode Register */
  #define REG_PWM1_CMPMUPD5                  (0x4005C18CU) /**< \brief (PWM1) PWM Comparison 5 Mode Update Register */
  #define REG_PWM1_CMPV6                     (0x4005C190U) /**< \brief (PWM1) PWM Comparison 6 Value Register */
  #define REG_PWM1_CMPVUPD6                  (0x4005C194U) /**< \brief (PWM1) PWM Comparison 6 Value Update Register */
  #define REG_PWM1_CMPM6                     (0x4005C198U) /**< \brief (PWM1) PWM Comparison 6 Mode Register */
  #define REG_PWM1_CMPMUPD6                  (0x4005C19CU) /**< \brief (PWM1) PWM Comparison 6 Mode Update Register */
  #define REG_PWM1_CMPV7                     (0x4005C1A0U) /**< \brief (PWM1) PWM Comparison 7 Value Register */
  #define REG_PWM1_CMPVUPD7                  (0x4005C1A4U) /**< \brief (PWM1) PWM Comparison 7 Value Update Register */
  #define REG_PWM1_CMPM7                     (0x4005C1A8U) /**< \brief (PWM1) PWM Comparison 7 Mode Register */
  #define REG_PWM1_CMPMUPD7                  (0x4005C1ACU) /**< \brief (PWM1) PWM Comparison 7 Mode Update Register */
  #define REG_PWM1_CMR0                      (0x4005C200U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 0) */
  #define REG_PWM1_CDTY0                     (0x4005C204U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 0) */
  #define REG_PWM1_CDTYUPD0                  (0x4005C208U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 0) */
  #define REG_PWM1_CPRD0                     (0x4005C20CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 0) */
  #define REG_PWM1_CPRDUPD0                  (0x4005C210U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 0) */
  #define REG_PWM1_CCNT0                     (0x4005C214U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 0) */
  #define REG_PWM1_DT0                       (0x4005C218U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 0) */
  #define REG_PWM1_DTUPD0                    (0x4005C21CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 0) */
  #define REG_PWM1_CMR1                      (0x4005C220U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 1) */
  #define REG_PWM1_CDTY1                     (0x4005C224U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 1) */
  #define REG_PWM1_CDTYUPD1                  (0x4005C228U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 1) */
  #define REG_PWM1_CPRD1                     (0x4005C22CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 1) */
  #define REG_PWM1_CPRDUPD1                  (0x4005C230U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 1) */
  #define REG_PWM1_CCNT1                     (0x4005C234U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 1) */
  #define REG_PWM1_DT1                       (0x4005C238U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 1) */
  #define REG_PWM1_DTUPD1                    (0x4005C23CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 1) */
  #define REG_PWM1_CMR2                      (0x4005C240U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 2) */
  #define REG_PWM1_CDTY2                     (0x4005C244U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 2) */
  #define REG_PWM1_CDTYUPD2                  (0x4005C248U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 2) */
  #define REG_PWM1_CPRD2                     (0x4005C24CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 2) */
  #define REG_PWM1_CPRDUPD2                  (0x4005C250U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 2) */
  #define REG_PWM1_CCNT2                     (0x4005C254U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 2) */
  #define REG_PWM1_DT2                       (0x4005C258U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 2) */
  #define REG_PWM1_DTUPD2                    (0x4005C25CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 2) */
  #define REG_PWM1_CMR3                      (0x4005C260U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 3) */
  #define REG_PWM1_CDTY3                     (0x4005C264U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 3) */
  #define REG_PWM1_CDTYUPD3                  (0x4005C268U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 3) */
  #define REG_PWM1_CPRD3                     (0x4005C26CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 3) */
  #define REG_PWM1_CPRDUPD3                  (0x4005C270U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 3) */
  #define REG_PWM1_CCNT3                     (0x4005C274U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 3) */
  #define REG_PWM1_DT3                       (0x4005C278U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 3) */
  #define REG_PWM1_DTUPD3                    (0x4005C27CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 3) */
  #define REG_PWM1_CMUPD0                    (0x4005C400U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 0) */
  #define REG_PWM1_CMUPD1                    (0x4005C420U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 1) */
  #define REG_PWM1_ETRG1                     (0x4005C42CU) /**< \brief (PWM1) PWM External Trigger Register (trg_num = 1) */
  #define REG_PWM1_LEBR1                     (0x4005C430U) /**< \brief (PWM1) PWM Leading-Edge Blanking Register (trg_num = 1) */
  #define REG_PWM1_CMUPD2                    (0x4005C440U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 2) */
  #define REG_PWM1_ETRG2                     (0x4005C44CU) /**< \brief (PWM1) PWM External Trigger Register (trg_num = 2) */
  #define REG_PWM1_LEBR2                     (0x4005C450U) /**< \brief (PWM1) PWM Leading-Edge Blanking Register (trg_num = 2) */
  #define REG_PWM1_CMUPD3                    (0x4005C460U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 3) */
#else
  #define REG_PWM1_CLK      (*(__IO uint32_t*)0x4005C000U) /**< \brief (PWM1) PWM Clock Register */
  #define REG_PWM1_ENA      (*(__O  uint32_t*)0x4005C004U) /**< \brief (PWM1) PWM Enable Register */
  #define REG_PWM1_DIS      (*(__O  uint32_t*)0x4005C008U) /**< \brief (PWM1) PWM Disable Register */
  #define REG_PWM1_SR       (*(__I  uint32_t*)0x4005C00CU) /**< \brief (PWM1) PWM Status Register */
  #define REG_PWM1_IER1     (*(__O  uint32_t*)0x4005C010U) /**< \brief (PWM1) PWM Interrupt Enable Register 1 */
  #define REG_PWM1_IDR1     (*(__O  uint32_t*)0x4005C014U) /**< \brief (PWM1) PWM Interrupt Disable Register 1 */
  #define REG_PWM1_IMR1     (*(__I  uint32_t*)0x4005C018U) /**< \brief (PWM1) PWM Interrupt Mask Register 1 */
  #define REG_PWM1_ISR1     (*(__I  uint32_t*)0x4005C01CU) /**< \brief (PWM1) PWM Interrupt Status Register 1 */
  #define REG_PWM1_SCM      (*(__IO uint32_t*)0x4005C020U) /**< \brief (PWM1) PWM Sync Channels Mode Register */
  #define REG_PWM1_DMAR     (*(__O  uint32_t*)0x4005C024U) /**< \brief (PWM1) PWM DMA Register */
  #define REG_PWM1_SCUC     (*(__IO uint32_t*)0x4005C028U) /**< \brief (PWM1) PWM Sync Channels Update Control Register */
  #define REG_PWM1_SCUP     (*(__IO uint32_t*)0x4005C02CU) /**< \brief (PWM1) PWM Sync Channels Update Period Register */
  #define REG_PWM1_SCUPUPD  (*(__O  uint32_t*)0x4005C030U) /**< \brief (PWM1) PWM Sync Channels Update Period Update Register */
  #define REG_PWM1_IER2     (*(__O  uint32_t*)0x4005C034U) /**< \brief (PWM1) PWM Interrupt Enable Register 2 */
  #define REG_PWM1_IDR2     (*(__O  uint32_t*)0x4005C038U) /**< \brief (PWM1) PWM Interrupt Disable Register 2 */
  #define REG_PWM1_IMR2     (*(__I  uint32_t*)0x4005C03CU) /**< \brief (PWM1) PWM Interrupt Mask Register 2 */
  #define REG_PWM1_ISR2     (*(__I  uint32_t*)0x4005C040U) /**< \brief (PWM1) PWM Interrupt Status Register 2 */
  #define REG_PWM1_OOV      (*(__IO uint32_t*)0x4005C044U) /**< \brief (PWM1) PWM Output Override Value Register */
  #define REG_PWM1_OS       (*(__IO uint32_t*)0x4005C048U) /**< \brief (PWM1) PWM Output Selection Register */
  #define REG_PWM1_OSS      (*(__O  uint32_t*)0x4005C04CU) /**< \brief (PWM1) PWM Output Selection Set Register */
  #define REG_PWM1_OSC      (*(__O  uint32_t*)0x4005C050U) /**< \brief (PWM1) PWM Output Selection Clear Register */
  #define REG_PWM1_OSSUPD   (*(__O  uint32_t*)0x4005C054U) /**< \brief (PWM1) PWM Output Selection Set Update Register */
  #define REG_PWM1_OSCUPD   (*(__O  uint32_t*)0x4005C058U) /**< \brief (PWM1) PWM Output Selection Clear Update Register */
  #define REG_PWM1_FMR      (*(__IO uint32_t*)0x4005C05CU) /**< \brief (PWM1) PWM Fault Mode Register */
  #define REG_PWM1_FSR      (*(__I  uint32_t*)0x4005C060U) /**< \brief (PWM1) PWM Fault Status Register */
  #define REG_PWM1_FCR      (*(__O  uint32_t*)0x4005C064U) /**< \brief (PWM1) PWM Fault Clear Register */
  #define REG_PWM1_FPV1     (*(__IO uint32_t*)0x4005C068U) /**< \brief (PWM1) PWM Fault Protection Value Register 1 */
  #define REG_PWM1_FPE      (*(__IO uint32_t*)0x4005C06CU) /**< \brief (PWM1) PWM Fault Protection Enable Register */
  #define REG_PWM1_ELMR     (*(__IO uint32_t*)0x4005C07CU) /**< \brief (PWM1) PWM Event Line 0 Mode Register */
  #define REG_PWM1_SSPR     (*(__IO uint32_t*)0x4005C0A0U) /**< \brief (PWM1) PWM Spread Spectrum Register */
  #define REG_PWM1_SSPUP    (*(__O  uint32_t*)0x4005C0A4U) /**< \brief (PWM1) PWM Spread Spectrum Update Register */
  #define REG_PWM1_SMMR     (*(__IO uint32_t*)0x4005C0B0U) /**< \brief (PWM1) PWM Stepper Motor Mode Register */
  #define REG_PWM1_FPV2     (*(__IO uint32_t*)0x4005C0C0U) /**< \brief (PWM1) PWM Fault Protection Value 2 Register */
  #define REG_PWM1_WPCR     (*(__O  uint32_t*)0x4005C0E4U) /**< \brief (PWM1) PWM Write Protection Control Register */
  #define REG_PWM1_WPSR     (*(__I  uint32_t*)0x4005C0E8U) /**< \brief (PWM1) PWM Write Protection Status Register */
  #define REG_PWM1_VERSION  (*(__I  uint32_t*)0x4005C0FCU) /**< \brief (PWM1) Version Register */
  #define REG_PWM1_CMPV0    (*(__IO uint32_t*)0x4005C130U) /**< \brief (PWM1) PWM Comparison 0 Value Register */
  #define REG_PWM1_CMPVUPD0 (*(__O  uint32_t*)0x4005C134U) /**< \brief (PWM1) PWM Comparison 0 Value Update Register */
  #define REG_PWM1_CMPM0    (*(__IO uint32_t*)0x4005C138U) /**< \brief (PWM1) PWM Comparison 0 Mode Register */
  #define REG_PWM1_CMPMUPD0 (*(__O  uint32_t*)0x4005C13CU) /**< \brief (PWM1) PWM Comparison 0 Mode Update Register */
  #define REG_PWM1_CMPV1    (*(__IO uint32_t*)0x4005C140U) /**< \brief (PWM1) PWM Comparison 1 Value Register */
  #define REG_PWM1_CMPVUPD1 (*(__O  uint32_t*)0x4005C144U) /**< \brief (PWM1) PWM Comparison 1 Value Update Register */
  #define REG_PWM1_CMPM1    (*(__IO uint32_t*)0x4005C148U) /**< \brief (PWM1) PWM Comparison 1 Mode Register */
  #define REG_PWM1_CMPMUPD1 (*(__O  uint32_t*)0x4005C14CU) /**< \brief (PWM1) PWM Comparison 1 Mode Update Register */
  #define REG_PWM1_CMPV2    (*(__IO uint32_t*)0x4005C150U) /**< \brief (PWM1) PWM Comparison 2 Value Register */
  #define REG_PWM1_CMPVUPD2 (*(__O  uint32_t*)0x4005C154U) /**< \brief (PWM1) PWM Comparison 2 Value Update Register */
  #define REG_PWM1_CMPM2    (*(__IO uint32_t*)0x4005C158U) /**< \brief (PWM1) PWM Comparison 2 Mode Register */
  #define REG_PWM1_CMPMUPD2 (*(__O  uint32_t*)0x4005C15CU) /**< \brief (PWM1) PWM Comparison 2 Mode Update Register */
  #define REG_PWM1_CMPV3    (*(__IO uint32_t*)0x4005C160U) /**< \brief (PWM1) PWM Comparison 3 Value Register */
  #define REG_PWM1_CMPVUPD3 (*(__O  uint32_t*)0x4005C164U) /**< \brief (PWM1) PWM Comparison 3 Value Update Register */
  #define REG_PWM1_CMPM3    (*(__IO uint32_t*)0x4005C168U) /**< \brief (PWM1) PWM Comparison 3 Mode Register */
  #define REG_PWM1_CMPMUPD3 (*(__O  uint32_t*)0x4005C16CU) /**< \brief (PWM1) PWM Comparison 3 Mode Update Register */
  #define REG_PWM1_CMPV4    (*(__IO uint32_t*)0x4005C170U) /**< \brief (PWM1) PWM Comparison 4 Value Register */
  #define REG_PWM1_CMPVUPD4 (*(__O  uint32_t*)0x4005C174U) /**< \brief (PWM1) PWM Comparison 4 Value Update Register */
  #define REG_PWM1_CMPM4    (*(__IO uint32_t*)0x4005C178U) /**< \brief (PWM1) PWM Comparison 4 Mode Register */
  #define REG_PWM1_CMPMUPD4 (*(__O  uint32_t*)0x4005C17CU) /**< \brief (PWM1) PWM Comparison 4 Mode Update Register */
  #define REG_PWM1_CMPV5    (*(__IO uint32_t*)0x4005C180U) /**< \brief (PWM1) PWM Comparison 5 Value Register */
  #define REG_PWM1_CMPVUPD5 (*(__O  uint32_t*)0x4005C184U) /**< \brief (PWM1) PWM Comparison 5 Value Update Register */
  #define REG_PWM1_CMPM5    (*(__IO uint32_t*)0x4005C188U) /**< \brief (PWM1) PWM Comparison 5 Mode Register */
  #define REG_PWM1_CMPMUPD5 (*(__O  uint32_t*)0x4005C18CU) /**< \brief (PWM1) PWM Comparison 5 Mode Update Register */
  #define REG_PWM1_CMPV6    (*(__IO uint32_t*)0x4005C190U) /**< \brief (PWM1) PWM Comparison 6 Value Register */
  #define REG_PWM1_CMPVUPD6 (*(__O  uint32_t*)0x4005C194U) /**< \brief (PWM1) PWM Comparison 6 Value Update Register */
  #define REG_PWM1_CMPM6    (*(__IO uint32_t*)0x4005C198U) /**< \brief (PWM1) PWM Comparison 6 Mode Register */
  #define REG_PWM1_CMPMUPD6 (*(__O  uint32_t*)0x4005C19CU) /**< \brief (PWM1) PWM Comparison 6 Mode Update Register */
  #define REG_PWM1_CMPV7    (*(__IO uint32_t*)0x4005C1A0U) /**< \brief (PWM1) PWM Comparison 7 Value Register */
  #define REG_PWM1_CMPVUPD7 (*(__O  uint32_t*)0x4005C1A4U) /**< \brief (PWM1) PWM Comparison 7 Value Update Register */
  #define REG_PWM1_CMPM7    (*(__IO uint32_t*)0x4005C1A8U) /**< \brief (PWM1) PWM Comparison 7 Mode Register */
  #define REG_PWM1_CMPMUPD7 (*(__O  uint32_t*)0x4005C1ACU) /**< \brief (PWM1) PWM Comparison 7 Mode Update Register */
  #define REG_PWM1_CMR0     (*(__IO uint32_t*)0x4005C200U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 0) */
  #define REG_PWM1_CDTY0    (*(__IO uint32_t*)0x4005C204U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 0) */
  #define REG_PWM1_CDTYUPD0 (*(__O  uint32_t*)0x4005C208U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 0) */
  #define REG_PWM1_CPRD0    (*(__IO uint32_t*)0x4005C20CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 0) */
  #define REG_PWM1_CPRDUPD0 (*(__O  uint32_t*)0x4005C210U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 0) */
  #define REG_PWM1_CCNT0    (*(__I  uint32_t*)0x4005C214U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 0) */
  #define REG_PWM1_DT0      (*(__IO uint32_t*)0x4005C218U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 0) */
  #define REG_PWM1_DTUPD0   (*(__O  uint32_t*)0x4005C21CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 0) */
  #define REG_PWM1_CMR1     (*(__IO uint32_t*)0x4005C220U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 1) */
  #define REG_PWM1_CDTY1    (*(__IO uint32_t*)0x4005C224U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 1) */
  #define REG_PWM1_CDTYUPD1 (*(__O  uint32_t*)0x4005C228U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 1) */
  #define REG_PWM1_CPRD1    (*(__IO uint32_t*)0x4005C22CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 1) */
  #define REG_PWM1_CPRDUPD1 (*(__O  uint32_t*)0x4005C230U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 1) */
  #define REG_PWM1_CCNT1    (*(__I  uint32_t*)0x4005C234U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 1) */
  #define REG_PWM1_DT1      (*(__IO uint32_t*)0x4005C238U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 1) */
  #define REG_PWM1_DTUPD1   (*(__O  uint32_t*)0x4005C23CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 1) */
  #define REG_PWM1_CMR2     (*(__IO uint32_t*)0x4005C240U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 2) */
  #define REG_PWM1_CDTY2    (*(__IO uint32_t*)0x4005C244U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 2) */
  #define REG_PWM1_CDTYUPD2 (*(__O  uint32_t*)0x4005C248U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 2) */
  #define REG_PWM1_CPRD2    (*(__IO uint32_t*)0x4005C24CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 2) */
  #define REG_PWM1_CPRDUPD2 (*(__O  uint32_t*)0x4005C250U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 2) */
  #define REG_PWM1_CCNT2    (*(__I  uint32_t*)0x4005C254U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 2) */
  #define REG_PWM1_DT2      (*(__IO uint32_t*)0x4005C258U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 2) */
  #define REG_PWM1_DTUPD2   (*(__O  uint32_t*)0x4005C25CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 2) */
  #define REG_PWM1_CMR3     (*(__IO uint32_t*)0x4005C260U) /**< \brief (PWM1) PWM Channel Mode Register (ch_num = 3) */
  #define REG_PWM1_CDTY3    (*(__IO uint32_t*)0x4005C264U) /**< \brief (PWM1) PWM Channel Duty Cycle Register (ch_num = 3) */
  #define REG_PWM1_CDTYUPD3 (*(__O  uint32_t*)0x4005C268U) /**< \brief (PWM1) PWM Channel Duty Cycle Update Register (ch_num = 3) */
  #define REG_PWM1_CPRD3    (*(__IO uint32_t*)0x4005C26CU) /**< \brief (PWM1) PWM Channel Period Register (ch_num = 3) */
  #define REG_PWM1_CPRDUPD3 (*(__O  uint32_t*)0x4005C270U) /**< \brief (PWM1) PWM Channel Period Update Register (ch_num = 3) */
  #define REG_PWM1_CCNT3    (*(__I  uint32_t*)0x4005C274U) /**< \brief (PWM1) PWM Channel Counter Register (ch_num = 3) */
  #define REG_PWM1_DT3      (*(__IO uint32_t*)0x4005C278U) /**< \brief (PWM1) PWM Channel Dead Time Register (ch_num = 3) */
  #define REG_PWM1_DTUPD3   (*(__O  uint32_t*)0x4005C27CU) /**< \brief (PWM1) PWM Channel Dead Time Update Register (ch_num = 3) */
  #define REG_PWM1_CMUPD0   (*(__O  uint32_t*)0x4005C400U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 0) */
  #define REG_PWM1_CMUPD1   (*(__O  uint32_t*)0x4005C420U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 1) */
  #define REG_PWM1_ETRG1    (*(__IO uint32_t*)0x4005C42CU) /**< \brief (PWM1) PWM External Trigger Register (trg_num = 1) */
  #define REG_PWM1_LEBR1    (*(__IO uint32_t*)0x4005C430U) /**< \brief (PWM1) PWM Leading-Edge Blanking Register (trg_num = 1) */
  #define REG_PWM1_CMUPD2   (*(__O  uint32_t*)0x4005C440U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 2) */
  #define REG_PWM1_ETRG2    (*(__IO uint32_t*)0x4005C44CU) /**< \brief (PWM1) PWM External Trigger Register (trg_num = 2) */
  #define REG_PWM1_LEBR2    (*(__IO uint32_t*)0x4005C450U) /**< \brief (PWM1) PWM Leading-Edge Blanking Register (trg_num = 2) */
  #define REG_PWM1_CMUPD3   (*(__O  uint32_t*)0x4005C460U) /**< \brief (PWM1) PWM Channel Mode Update Register (ch_num = 3) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_PWM1_INSTANCE_ */
