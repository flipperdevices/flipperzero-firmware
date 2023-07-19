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

#ifndef _SAME70_PWM0_INSTANCE_
#define _SAME70_PWM0_INSTANCE_

/* ========== Register definition for PWM0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PWM0_CLK                       (0x40020000U) /**< \brief (PWM0) PWM Clock Register */
  #define REG_PWM0_ENA                       (0x40020004U) /**< \brief (PWM0) PWM Enable Register */
  #define REG_PWM0_DIS                       (0x40020008U) /**< \brief (PWM0) PWM Disable Register */
  #define REG_PWM0_SR                        (0x4002000CU) /**< \brief (PWM0) PWM Status Register */
  #define REG_PWM0_IER1                      (0x40020010U) /**< \brief (PWM0) PWM Interrupt Enable Register 1 */
  #define REG_PWM0_IDR1                      (0x40020014U) /**< \brief (PWM0) PWM Interrupt Disable Register 1 */
  #define REG_PWM0_IMR1                      (0x40020018U) /**< \brief (PWM0) PWM Interrupt Mask Register 1 */
  #define REG_PWM0_ISR1                      (0x4002001CU) /**< \brief (PWM0) PWM Interrupt Status Register 1 */
  #define REG_PWM0_SCM                       (0x40020020U) /**< \brief (PWM0) PWM Sync Channels Mode Register */
  #define REG_PWM0_DMAR                      (0x40020024U) /**< \brief (PWM0) PWM DMA Register */
  #define REG_PWM0_SCUC                      (0x40020028U) /**< \brief (PWM0) PWM Sync Channels Update Control Register */
  #define REG_PWM0_SCUP                      (0x4002002CU) /**< \brief (PWM0) PWM Sync Channels Update Period Register */
  #define REG_PWM0_SCUPUPD                   (0x40020030U) /**< \brief (PWM0) PWM Sync Channels Update Period Update Register */
  #define REG_PWM0_IER2                      (0x40020034U) /**< \brief (PWM0) PWM Interrupt Enable Register 2 */
  #define REG_PWM0_IDR2                      (0x40020038U) /**< \brief (PWM0) PWM Interrupt Disable Register 2 */
  #define REG_PWM0_IMR2                      (0x4002003CU) /**< \brief (PWM0) PWM Interrupt Mask Register 2 */
  #define REG_PWM0_ISR2                      (0x40020040U) /**< \brief (PWM0) PWM Interrupt Status Register 2 */
  #define REG_PWM0_OOV                       (0x40020044U) /**< \brief (PWM0) PWM Output Override Value Register */
  #define REG_PWM0_OS                        (0x40020048U) /**< \brief (PWM0) PWM Output Selection Register */
  #define REG_PWM0_OSS                       (0x4002004CU) /**< \brief (PWM0) PWM Output Selection Set Register */
  #define REG_PWM0_OSC                       (0x40020050U) /**< \brief (PWM0) PWM Output Selection Clear Register */
  #define REG_PWM0_OSSUPD                    (0x40020054U) /**< \brief (PWM0) PWM Output Selection Set Update Register */
  #define REG_PWM0_OSCUPD                    (0x40020058U) /**< \brief (PWM0) PWM Output Selection Clear Update Register */
  #define REG_PWM0_FMR                       (0x4002005CU) /**< \brief (PWM0) PWM Fault Mode Register */
  #define REG_PWM0_FSR                       (0x40020060U) /**< \brief (PWM0) PWM Fault Status Register */
  #define REG_PWM0_FCR                       (0x40020064U) /**< \brief (PWM0) PWM Fault Clear Register */
  #define REG_PWM0_FPV1                      (0x40020068U) /**< \brief (PWM0) PWM Fault Protection Value Register 1 */
  #define REG_PWM0_FPE                       (0x4002006CU) /**< \brief (PWM0) PWM Fault Protection Enable Register */
  #define REG_PWM0_ELMR                      (0x4002007CU) /**< \brief (PWM0) PWM Event Line 0 Mode Register */
  #define REG_PWM0_SSPR                      (0x400200A0U) /**< \brief (PWM0) PWM Spread Spectrum Register */
  #define REG_PWM0_SSPUP                     (0x400200A4U) /**< \brief (PWM0) PWM Spread Spectrum Update Register */
  #define REG_PWM0_SMMR                      (0x400200B0U) /**< \brief (PWM0) PWM Stepper Motor Mode Register */
  #define REG_PWM0_FPV2                      (0x400200C0U) /**< \brief (PWM0) PWM Fault Protection Value 2 Register */
  #define REG_PWM0_WPCR                      (0x400200E4U) /**< \brief (PWM0) PWM Write Protection Control Register */
  #define REG_PWM0_WPSR                      (0x400200E8U) /**< \brief (PWM0) PWM Write Protection Status Register */
  #define REG_PWM0_VERSION                   (0x400200FCU) /**< \brief (PWM0) Version Register */
  #define REG_PWM0_CMPV0                     (0x40020130U) /**< \brief (PWM0) PWM Comparison 0 Value Register */
  #define REG_PWM0_CMPVUPD0                  (0x40020134U) /**< \brief (PWM0) PWM Comparison 0 Value Update Register */
  #define REG_PWM0_CMPM0                     (0x40020138U) /**< \brief (PWM0) PWM Comparison 0 Mode Register */
  #define REG_PWM0_CMPMUPD0                  (0x4002013CU) /**< \brief (PWM0) PWM Comparison 0 Mode Update Register */
  #define REG_PWM0_CMPV1                     (0x40020140U) /**< \brief (PWM0) PWM Comparison 1 Value Register */
  #define REG_PWM0_CMPVUPD1                  (0x40020144U) /**< \brief (PWM0) PWM Comparison 1 Value Update Register */
  #define REG_PWM0_CMPM1                     (0x40020148U) /**< \brief (PWM0) PWM Comparison 1 Mode Register */
  #define REG_PWM0_CMPMUPD1                  (0x4002014CU) /**< \brief (PWM0) PWM Comparison 1 Mode Update Register */
  #define REG_PWM0_CMPV2                     (0x40020150U) /**< \brief (PWM0) PWM Comparison 2 Value Register */
  #define REG_PWM0_CMPVUPD2                  (0x40020154U) /**< \brief (PWM0) PWM Comparison 2 Value Update Register */
  #define REG_PWM0_CMPM2                     (0x40020158U) /**< \brief (PWM0) PWM Comparison 2 Mode Register */
  #define REG_PWM0_CMPMUPD2                  (0x4002015CU) /**< \brief (PWM0) PWM Comparison 2 Mode Update Register */
  #define REG_PWM0_CMPV3                     (0x40020160U) /**< \brief (PWM0) PWM Comparison 3 Value Register */
  #define REG_PWM0_CMPVUPD3                  (0x40020164U) /**< \brief (PWM0) PWM Comparison 3 Value Update Register */
  #define REG_PWM0_CMPM3                     (0x40020168U) /**< \brief (PWM0) PWM Comparison 3 Mode Register */
  #define REG_PWM0_CMPMUPD3                  (0x4002016CU) /**< \brief (PWM0) PWM Comparison 3 Mode Update Register */
  #define REG_PWM0_CMPV4                     (0x40020170U) /**< \brief (PWM0) PWM Comparison 4 Value Register */
  #define REG_PWM0_CMPVUPD4                  (0x40020174U) /**< \brief (PWM0) PWM Comparison 4 Value Update Register */
  #define REG_PWM0_CMPM4                     (0x40020178U) /**< \brief (PWM0) PWM Comparison 4 Mode Register */
  #define REG_PWM0_CMPMUPD4                  (0x4002017CU) /**< \brief (PWM0) PWM Comparison 4 Mode Update Register */
  #define REG_PWM0_CMPV5                     (0x40020180U) /**< \brief (PWM0) PWM Comparison 5 Value Register */
  #define REG_PWM0_CMPVUPD5                  (0x40020184U) /**< \brief (PWM0) PWM Comparison 5 Value Update Register */
  #define REG_PWM0_CMPM5                     (0x40020188U) /**< \brief (PWM0) PWM Comparison 5 Mode Register */
  #define REG_PWM0_CMPMUPD5                  (0x4002018CU) /**< \brief (PWM0) PWM Comparison 5 Mode Update Register */
  #define REG_PWM0_CMPV6                     (0x40020190U) /**< \brief (PWM0) PWM Comparison 6 Value Register */
  #define REG_PWM0_CMPVUPD6                  (0x40020194U) /**< \brief (PWM0) PWM Comparison 6 Value Update Register */
  #define REG_PWM0_CMPM6                     (0x40020198U) /**< \brief (PWM0) PWM Comparison 6 Mode Register */
  #define REG_PWM0_CMPMUPD6                  (0x4002019CU) /**< \brief (PWM0) PWM Comparison 6 Mode Update Register */
  #define REG_PWM0_CMPV7                     (0x400201A0U) /**< \brief (PWM0) PWM Comparison 7 Value Register */
  #define REG_PWM0_CMPVUPD7                  (0x400201A4U) /**< \brief (PWM0) PWM Comparison 7 Value Update Register */
  #define REG_PWM0_CMPM7                     (0x400201A8U) /**< \brief (PWM0) PWM Comparison 7 Mode Register */
  #define REG_PWM0_CMPMUPD7                  (0x400201ACU) /**< \brief (PWM0) PWM Comparison 7 Mode Update Register */
  #define REG_PWM0_CMR0                      (0x40020200U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 0) */
  #define REG_PWM0_CDTY0                     (0x40020204U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 0) */
  #define REG_PWM0_CDTYUPD0                  (0x40020208U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 0) */
  #define REG_PWM0_CPRD0                     (0x4002020CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 0) */
  #define REG_PWM0_CPRDUPD0                  (0x40020210U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 0) */
  #define REG_PWM0_CCNT0                     (0x40020214U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 0) */
  #define REG_PWM0_DT0                       (0x40020218U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 0) */
  #define REG_PWM0_DTUPD0                    (0x4002021CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 0) */
  #define REG_PWM0_CMR1                      (0x40020220U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 1) */
  #define REG_PWM0_CDTY1                     (0x40020224U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 1) */
  #define REG_PWM0_CDTYUPD1                  (0x40020228U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 1) */
  #define REG_PWM0_CPRD1                     (0x4002022CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 1) */
  #define REG_PWM0_CPRDUPD1                  (0x40020230U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 1) */
  #define REG_PWM0_CCNT1                     (0x40020234U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 1) */
  #define REG_PWM0_DT1                       (0x40020238U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 1) */
  #define REG_PWM0_DTUPD1                    (0x4002023CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 1) */
  #define REG_PWM0_CMR2                      (0x40020240U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 2) */
  #define REG_PWM0_CDTY2                     (0x40020244U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 2) */
  #define REG_PWM0_CDTYUPD2                  (0x40020248U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 2) */
  #define REG_PWM0_CPRD2                     (0x4002024CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 2) */
  #define REG_PWM0_CPRDUPD2                  (0x40020250U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 2) */
  #define REG_PWM0_CCNT2                     (0x40020254U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 2) */
  #define REG_PWM0_DT2                       (0x40020258U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 2) */
  #define REG_PWM0_DTUPD2                    (0x4002025CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 2) */
  #define REG_PWM0_CMR3                      (0x40020260U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 3) */
  #define REG_PWM0_CDTY3                     (0x40020264U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 3) */
  #define REG_PWM0_CDTYUPD3                  (0x40020268U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 3) */
  #define REG_PWM0_CPRD3                     (0x4002026CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 3) */
  #define REG_PWM0_CPRDUPD3                  (0x40020270U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 3) */
  #define REG_PWM0_CCNT3                     (0x40020274U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 3) */
  #define REG_PWM0_DT3                       (0x40020278U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 3) */
  #define REG_PWM0_DTUPD3                    (0x4002027CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 3) */
  #define REG_PWM0_CMUPD0                    (0x40020400U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 0) */
  #define REG_PWM0_CMUPD1                    (0x40020420U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 1) */
  #define REG_PWM0_ETRG1                     (0x4002042CU) /**< \brief (PWM0) PWM External Trigger Register (trg_num = 1) */
  #define REG_PWM0_LEBR1                     (0x40020430U) /**< \brief (PWM0) PWM Leading-Edge Blanking Register (trg_num = 1) */
  #define REG_PWM0_CMUPD2                    (0x40020440U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 2) */
  #define REG_PWM0_ETRG2                     (0x4002044CU) /**< \brief (PWM0) PWM External Trigger Register (trg_num = 2) */
  #define REG_PWM0_LEBR2                     (0x40020450U) /**< \brief (PWM0) PWM Leading-Edge Blanking Register (trg_num = 2) */
  #define REG_PWM0_CMUPD3                    (0x40020460U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 3) */
#else
  #define REG_PWM0_CLK      (*(__IO uint32_t*)0x40020000U) /**< \brief (PWM0) PWM Clock Register */
  #define REG_PWM0_ENA      (*(__O  uint32_t*)0x40020004U) /**< \brief (PWM0) PWM Enable Register */
  #define REG_PWM0_DIS      (*(__O  uint32_t*)0x40020008U) /**< \brief (PWM0) PWM Disable Register */
  #define REG_PWM0_SR       (*(__I  uint32_t*)0x4002000CU) /**< \brief (PWM0) PWM Status Register */
  #define REG_PWM0_IER1     (*(__O  uint32_t*)0x40020010U) /**< \brief (PWM0) PWM Interrupt Enable Register 1 */
  #define REG_PWM0_IDR1     (*(__O  uint32_t*)0x40020014U) /**< \brief (PWM0) PWM Interrupt Disable Register 1 */
  #define REG_PWM0_IMR1     (*(__I  uint32_t*)0x40020018U) /**< \brief (PWM0) PWM Interrupt Mask Register 1 */
  #define REG_PWM0_ISR1     (*(__I  uint32_t*)0x4002001CU) /**< \brief (PWM0) PWM Interrupt Status Register 1 */
  #define REG_PWM0_SCM      (*(__IO uint32_t*)0x40020020U) /**< \brief (PWM0) PWM Sync Channels Mode Register */
  #define REG_PWM0_DMAR     (*(__O  uint32_t*)0x40020024U) /**< \brief (PWM0) PWM DMA Register */
  #define REG_PWM0_SCUC     (*(__IO uint32_t*)0x40020028U) /**< \brief (PWM0) PWM Sync Channels Update Control Register */
  #define REG_PWM0_SCUP     (*(__IO uint32_t*)0x4002002CU) /**< \brief (PWM0) PWM Sync Channels Update Period Register */
  #define REG_PWM0_SCUPUPD  (*(__O  uint32_t*)0x40020030U) /**< \brief (PWM0) PWM Sync Channels Update Period Update Register */
  #define REG_PWM0_IER2     (*(__O  uint32_t*)0x40020034U) /**< \brief (PWM0) PWM Interrupt Enable Register 2 */
  #define REG_PWM0_IDR2     (*(__O  uint32_t*)0x40020038U) /**< \brief (PWM0) PWM Interrupt Disable Register 2 */
  #define REG_PWM0_IMR2     (*(__I  uint32_t*)0x4002003CU) /**< \brief (PWM0) PWM Interrupt Mask Register 2 */
  #define REG_PWM0_ISR2     (*(__I  uint32_t*)0x40020040U) /**< \brief (PWM0) PWM Interrupt Status Register 2 */
  #define REG_PWM0_OOV      (*(__IO uint32_t*)0x40020044U) /**< \brief (PWM0) PWM Output Override Value Register */
  #define REG_PWM0_OS       (*(__IO uint32_t*)0x40020048U) /**< \brief (PWM0) PWM Output Selection Register */
  #define REG_PWM0_OSS      (*(__O  uint32_t*)0x4002004CU) /**< \brief (PWM0) PWM Output Selection Set Register */
  #define REG_PWM0_OSC      (*(__O  uint32_t*)0x40020050U) /**< \brief (PWM0) PWM Output Selection Clear Register */
  #define REG_PWM0_OSSUPD   (*(__O  uint32_t*)0x40020054U) /**< \brief (PWM0) PWM Output Selection Set Update Register */
  #define REG_PWM0_OSCUPD   (*(__O  uint32_t*)0x40020058U) /**< \brief (PWM0) PWM Output Selection Clear Update Register */
  #define REG_PWM0_FMR      (*(__IO uint32_t*)0x4002005CU) /**< \brief (PWM0) PWM Fault Mode Register */
  #define REG_PWM0_FSR      (*(__I  uint32_t*)0x40020060U) /**< \brief (PWM0) PWM Fault Status Register */
  #define REG_PWM0_FCR      (*(__O  uint32_t*)0x40020064U) /**< \brief (PWM0) PWM Fault Clear Register */
  #define REG_PWM0_FPV1     (*(__IO uint32_t*)0x40020068U) /**< \brief (PWM0) PWM Fault Protection Value Register 1 */
  #define REG_PWM0_FPE      (*(__IO uint32_t*)0x4002006CU) /**< \brief (PWM0) PWM Fault Protection Enable Register */
  #define REG_PWM0_ELMR     (*(__IO uint32_t*)0x4002007CU) /**< \brief (PWM0) PWM Event Line 0 Mode Register */
  #define REG_PWM0_SSPR     (*(__IO uint32_t*)0x400200A0U) /**< \brief (PWM0) PWM Spread Spectrum Register */
  #define REG_PWM0_SSPUP    (*(__O  uint32_t*)0x400200A4U) /**< \brief (PWM0) PWM Spread Spectrum Update Register */
  #define REG_PWM0_SMMR     (*(__IO uint32_t*)0x400200B0U) /**< \brief (PWM0) PWM Stepper Motor Mode Register */
  #define REG_PWM0_FPV2     (*(__IO uint32_t*)0x400200C0U) /**< \brief (PWM0) PWM Fault Protection Value 2 Register */
  #define REG_PWM0_WPCR     (*(__O  uint32_t*)0x400200E4U) /**< \brief (PWM0) PWM Write Protection Control Register */
  #define REG_PWM0_WPSR     (*(__I  uint32_t*)0x400200E8U) /**< \brief (PWM0) PWM Write Protection Status Register */
  #define REG_PWM0_VERSION  (*(__I  uint32_t*)0x400200FCU) /**< \brief (PWM0) Version Register */
  #define REG_PWM0_CMPV0    (*(__IO uint32_t*)0x40020130U) /**< \brief (PWM0) PWM Comparison 0 Value Register */
  #define REG_PWM0_CMPVUPD0 (*(__O  uint32_t*)0x40020134U) /**< \brief (PWM0) PWM Comparison 0 Value Update Register */
  #define REG_PWM0_CMPM0    (*(__IO uint32_t*)0x40020138U) /**< \brief (PWM0) PWM Comparison 0 Mode Register */
  #define REG_PWM0_CMPMUPD0 (*(__O  uint32_t*)0x4002013CU) /**< \brief (PWM0) PWM Comparison 0 Mode Update Register */
  #define REG_PWM0_CMPV1    (*(__IO uint32_t*)0x40020140U) /**< \brief (PWM0) PWM Comparison 1 Value Register */
  #define REG_PWM0_CMPVUPD1 (*(__O  uint32_t*)0x40020144U) /**< \brief (PWM0) PWM Comparison 1 Value Update Register */
  #define REG_PWM0_CMPM1    (*(__IO uint32_t*)0x40020148U) /**< \brief (PWM0) PWM Comparison 1 Mode Register */
  #define REG_PWM0_CMPMUPD1 (*(__O  uint32_t*)0x4002014CU) /**< \brief (PWM0) PWM Comparison 1 Mode Update Register */
  #define REG_PWM0_CMPV2    (*(__IO uint32_t*)0x40020150U) /**< \brief (PWM0) PWM Comparison 2 Value Register */
  #define REG_PWM0_CMPVUPD2 (*(__O  uint32_t*)0x40020154U) /**< \brief (PWM0) PWM Comparison 2 Value Update Register */
  #define REG_PWM0_CMPM2    (*(__IO uint32_t*)0x40020158U) /**< \brief (PWM0) PWM Comparison 2 Mode Register */
  #define REG_PWM0_CMPMUPD2 (*(__O  uint32_t*)0x4002015CU) /**< \brief (PWM0) PWM Comparison 2 Mode Update Register */
  #define REG_PWM0_CMPV3    (*(__IO uint32_t*)0x40020160U) /**< \brief (PWM0) PWM Comparison 3 Value Register */
  #define REG_PWM0_CMPVUPD3 (*(__O  uint32_t*)0x40020164U) /**< \brief (PWM0) PWM Comparison 3 Value Update Register */
  #define REG_PWM0_CMPM3    (*(__IO uint32_t*)0x40020168U) /**< \brief (PWM0) PWM Comparison 3 Mode Register */
  #define REG_PWM0_CMPMUPD3 (*(__O  uint32_t*)0x4002016CU) /**< \brief (PWM0) PWM Comparison 3 Mode Update Register */
  #define REG_PWM0_CMPV4    (*(__IO uint32_t*)0x40020170U) /**< \brief (PWM0) PWM Comparison 4 Value Register */
  #define REG_PWM0_CMPVUPD4 (*(__O  uint32_t*)0x40020174U) /**< \brief (PWM0) PWM Comparison 4 Value Update Register */
  #define REG_PWM0_CMPM4    (*(__IO uint32_t*)0x40020178U) /**< \brief (PWM0) PWM Comparison 4 Mode Register */
  #define REG_PWM0_CMPMUPD4 (*(__O  uint32_t*)0x4002017CU) /**< \brief (PWM0) PWM Comparison 4 Mode Update Register */
  #define REG_PWM0_CMPV5    (*(__IO uint32_t*)0x40020180U) /**< \brief (PWM0) PWM Comparison 5 Value Register */
  #define REG_PWM0_CMPVUPD5 (*(__O  uint32_t*)0x40020184U) /**< \brief (PWM0) PWM Comparison 5 Value Update Register */
  #define REG_PWM0_CMPM5    (*(__IO uint32_t*)0x40020188U) /**< \brief (PWM0) PWM Comparison 5 Mode Register */
  #define REG_PWM0_CMPMUPD5 (*(__O  uint32_t*)0x4002018CU) /**< \brief (PWM0) PWM Comparison 5 Mode Update Register */
  #define REG_PWM0_CMPV6    (*(__IO uint32_t*)0x40020190U) /**< \brief (PWM0) PWM Comparison 6 Value Register */
  #define REG_PWM0_CMPVUPD6 (*(__O  uint32_t*)0x40020194U) /**< \brief (PWM0) PWM Comparison 6 Value Update Register */
  #define REG_PWM0_CMPM6    (*(__IO uint32_t*)0x40020198U) /**< \brief (PWM0) PWM Comparison 6 Mode Register */
  #define REG_PWM0_CMPMUPD6 (*(__O  uint32_t*)0x4002019CU) /**< \brief (PWM0) PWM Comparison 6 Mode Update Register */
  #define REG_PWM0_CMPV7    (*(__IO uint32_t*)0x400201A0U) /**< \brief (PWM0) PWM Comparison 7 Value Register */
  #define REG_PWM0_CMPVUPD7 (*(__O  uint32_t*)0x400201A4U) /**< \brief (PWM0) PWM Comparison 7 Value Update Register */
  #define REG_PWM0_CMPM7    (*(__IO uint32_t*)0x400201A8U) /**< \brief (PWM0) PWM Comparison 7 Mode Register */
  #define REG_PWM0_CMPMUPD7 (*(__O  uint32_t*)0x400201ACU) /**< \brief (PWM0) PWM Comparison 7 Mode Update Register */
  #define REG_PWM0_CMR0     (*(__IO uint32_t*)0x40020200U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 0) */
  #define REG_PWM0_CDTY0    (*(__IO uint32_t*)0x40020204U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 0) */
  #define REG_PWM0_CDTYUPD0 (*(__O  uint32_t*)0x40020208U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 0) */
  #define REG_PWM0_CPRD0    (*(__IO uint32_t*)0x4002020CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 0) */
  #define REG_PWM0_CPRDUPD0 (*(__O  uint32_t*)0x40020210U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 0) */
  #define REG_PWM0_CCNT0    (*(__I  uint32_t*)0x40020214U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 0) */
  #define REG_PWM0_DT0      (*(__IO uint32_t*)0x40020218U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 0) */
  #define REG_PWM0_DTUPD0   (*(__O  uint32_t*)0x4002021CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 0) */
  #define REG_PWM0_CMR1     (*(__IO uint32_t*)0x40020220U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 1) */
  #define REG_PWM0_CDTY1    (*(__IO uint32_t*)0x40020224U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 1) */
  #define REG_PWM0_CDTYUPD1 (*(__O  uint32_t*)0x40020228U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 1) */
  #define REG_PWM0_CPRD1    (*(__IO uint32_t*)0x4002022CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 1) */
  #define REG_PWM0_CPRDUPD1 (*(__O  uint32_t*)0x40020230U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 1) */
  #define REG_PWM0_CCNT1    (*(__I  uint32_t*)0x40020234U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 1) */
  #define REG_PWM0_DT1      (*(__IO uint32_t*)0x40020238U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 1) */
  #define REG_PWM0_DTUPD1   (*(__O  uint32_t*)0x4002023CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 1) */
  #define REG_PWM0_CMR2     (*(__IO uint32_t*)0x40020240U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 2) */
  #define REG_PWM0_CDTY2    (*(__IO uint32_t*)0x40020244U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 2) */
  #define REG_PWM0_CDTYUPD2 (*(__O  uint32_t*)0x40020248U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 2) */
  #define REG_PWM0_CPRD2    (*(__IO uint32_t*)0x4002024CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 2) */
  #define REG_PWM0_CPRDUPD2 (*(__O  uint32_t*)0x40020250U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 2) */
  #define REG_PWM0_CCNT2    (*(__I  uint32_t*)0x40020254U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 2) */
  #define REG_PWM0_DT2      (*(__IO uint32_t*)0x40020258U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 2) */
  #define REG_PWM0_DTUPD2   (*(__O  uint32_t*)0x4002025CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 2) */
  #define REG_PWM0_CMR3     (*(__IO uint32_t*)0x40020260U) /**< \brief (PWM0) PWM Channel Mode Register (ch_num = 3) */
  #define REG_PWM0_CDTY3    (*(__IO uint32_t*)0x40020264U) /**< \brief (PWM0) PWM Channel Duty Cycle Register (ch_num = 3) */
  #define REG_PWM0_CDTYUPD3 (*(__O  uint32_t*)0x40020268U) /**< \brief (PWM0) PWM Channel Duty Cycle Update Register (ch_num = 3) */
  #define REG_PWM0_CPRD3    (*(__IO uint32_t*)0x4002026CU) /**< \brief (PWM0) PWM Channel Period Register (ch_num = 3) */
  #define REG_PWM0_CPRDUPD3 (*(__O  uint32_t*)0x40020270U) /**< \brief (PWM0) PWM Channel Period Update Register (ch_num = 3) */
  #define REG_PWM0_CCNT3    (*(__I  uint32_t*)0x40020274U) /**< \brief (PWM0) PWM Channel Counter Register (ch_num = 3) */
  #define REG_PWM0_DT3      (*(__IO uint32_t*)0x40020278U) /**< \brief (PWM0) PWM Channel Dead Time Register (ch_num = 3) */
  #define REG_PWM0_DTUPD3   (*(__O  uint32_t*)0x4002027CU) /**< \brief (PWM0) PWM Channel Dead Time Update Register (ch_num = 3) */
  #define REG_PWM0_CMUPD0   (*(__O  uint32_t*)0x40020400U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 0) */
  #define REG_PWM0_CMUPD1   (*(__O  uint32_t*)0x40020420U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 1) */
  #define REG_PWM0_ETRG1    (*(__IO uint32_t*)0x4002042CU) /**< \brief (PWM0) PWM External Trigger Register (trg_num = 1) */
  #define REG_PWM0_LEBR1    (*(__IO uint32_t*)0x40020430U) /**< \brief (PWM0) PWM Leading-Edge Blanking Register (trg_num = 1) */
  #define REG_PWM0_CMUPD2   (*(__O  uint32_t*)0x40020440U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 2) */
  #define REG_PWM0_ETRG2    (*(__IO uint32_t*)0x4002044CU) /**< \brief (PWM0) PWM External Trigger Register (trg_num = 2) */
  #define REG_PWM0_LEBR2    (*(__IO uint32_t*)0x40020450U) /**< \brief (PWM0) PWM Leading-Edge Blanking Register (trg_num = 2) */
  #define REG_PWM0_CMUPD3   (*(__O  uint32_t*)0x40020460U) /**< \brief (PWM0) PWM Channel Mode Update Register (ch_num = 3) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_PWM0_INSTANCE_ */
