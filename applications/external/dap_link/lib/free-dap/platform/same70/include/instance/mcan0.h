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

#ifndef _SAME70_MCAN0_INSTANCE_
#define _SAME70_MCAN0_INSTANCE_

/* ========== Register definition for MCAN0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_MCAN0_CREL                    (0x40030000U) /**< \brief (MCAN0) Core Release Register */
  #define REG_MCAN0_ENDN                    (0x40030004U) /**< \brief (MCAN0) Endian Register */
  #define REG_MCAN0_CUST                    (0x40030008U) /**< \brief (MCAN0) Customer Register */
  #define REG_MCAN0_FBTP                    (0x4003000CU) /**< \brief (MCAN0) Fast Bit Timing and Prescaler Register */
  #define REG_MCAN0_DBTP                    (0x4003000CU) /**< \brief (MCAN0) Data Bit Timing and Prescaler Register */
  #define REG_MCAN0_TEST                    (0x40030010U) /**< \brief (MCAN0) Test Register */
  #define REG_MCAN0_RWD                     (0x40030014U) /**< \brief (MCAN0) RAM Watchdog Register */
  #define REG_MCAN0_CCCR                    (0x40030018U) /**< \brief (MCAN0) CC Control Register */
  #define REG_MCAN0_BTP                     (0x4003001CU) /**< \brief (MCAN0) Bit Timing and Prescaler Register */
  #define REG_MCAN0_NBTP                    (0x4003001CU) /**< \brief (MCAN0) Nominal Bit Timing and Prescaler Register */
  #define REG_MCAN0_TSCC                    (0x40030020U) /**< \brief (MCAN0) Timestamp Counter Configuration Register */
  #define REG_MCAN0_TSCV                    (0x40030024U) /**< \brief (MCAN0) Timestamp Counter Value Register */
  #define REG_MCAN0_TOCC                    (0x40030028U) /**< \brief (MCAN0) Timeout Counter Configuration Register */
  #define REG_MCAN0_TOCV                    (0x4003002CU) /**< \brief (MCAN0) Timeout Counter Value Register */
  #define REG_MCAN0_ECR                     (0x40030040U) /**< \brief (MCAN0) Error Counter Register */
  #define REG_MCAN0_PSR                     (0x40030044U) /**< \brief (MCAN0) Protocol Status Register */
  #define REG_MCAN0_TDCR                    (0x40030048U) /**< \brief (MCAN0) Transmit Delay Compensation Register */
  #define REG_MCAN0_IR                      (0x40030050U) /**< \brief (MCAN0) Interrupt Register */
  #define REG_MCAN0_IE                      (0x40030054U) /**< \brief (MCAN0) Interrupt Enable Register */
  #define REG_MCAN0_ILS                     (0x40030058U) /**< \brief (MCAN0) Interrupt Line Select Register */
  #define REG_MCAN0_ILE                     (0x4003005CU) /**< \brief (MCAN0) Interrupt Line Enable Register */
  #define REG_MCAN0_GFC                     (0x40030080U) /**< \brief (MCAN0) Global Filter Configuration Register */
  #define REG_MCAN0_SIDFC                   (0x40030084U) /**< \brief (MCAN0) Standard ID Filter Configuration Register */
  #define REG_MCAN0_XIDFC                   (0x40030088U) /**< \brief (MCAN0) Extended ID Filter Configuration Register */
  #define REG_MCAN0_XIDAM                   (0x40030090U) /**< \brief (MCAN0) Extended ID AND Mask Register */
  #define REG_MCAN0_HPMS                    (0x40030094U) /**< \brief (MCAN0) High Priority Message Status Register */
  #define REG_MCAN0_NDAT1                   (0x40030098U) /**< \brief (MCAN0) New Data 1 Register */
  #define REG_MCAN0_NDAT2                   (0x4003009CU) /**< \brief (MCAN0) New Data 2 Register */
  #define REG_MCAN0_RXF0C                   (0x400300A0U) /**< \brief (MCAN0) Receive FIFO 0 Configuration Register */
  #define REG_MCAN0_RXF0S                   (0x400300A4U) /**< \brief (MCAN0) Receive FIFO 0 Status Register */
  #define REG_MCAN0_RXF0A                   (0x400300A8U) /**< \brief (MCAN0) Receive FIFO 0 Acknowledge Register */
  #define REG_MCAN0_RXBC                    (0x400300ACU) /**< \brief (MCAN0) Receive Rx Buffer Configuration Register */
  #define REG_MCAN0_RXF1C                   (0x400300B0U) /**< \brief (MCAN0) Receive FIFO 1 Configuration Register */
  #define REG_MCAN0_RXF1S                   (0x400300B4U) /**< \brief (MCAN0) Receive FIFO 1 Status Register */
  #define REG_MCAN0_RXF1A                   (0x400300B8U) /**< \brief (MCAN0) Receive FIFO 1 Acknowledge Register */
  #define REG_MCAN0_RXESC                   (0x400300BCU) /**< \brief (MCAN0) Receive Buffer / FIFO Element Size Configuration Register */
  #define REG_MCAN0_TXBC                    (0x400300C0U) /**< \brief (MCAN0) Transmit Buffer Configuration Register */
  #define REG_MCAN0_TXFQS                   (0x400300C4U) /**< \brief (MCAN0) Transmit FIFO/Queue Status Register */
  #define REG_MCAN0_TXESC                   (0x400300C8U) /**< \brief (MCAN0) Transmit Buffer Element Size Configuration Register */
  #define REG_MCAN0_TXBRP                   (0x400300CCU) /**< \brief (MCAN0) Transmit Buffer Request Pending Register */
  #define REG_MCAN0_TXBAR                   (0x400300D0U) /**< \brief (MCAN0) Transmit Buffer Add Request Register */
  #define REG_MCAN0_TXBCR                   (0x400300D4U) /**< \brief (MCAN0) Transmit Buffer Cancellation Request Register */
  #define REG_MCAN0_TXBTO                   (0x400300D8U) /**< \brief (MCAN0) Transmit Buffer Transmission Occurred Register */
  #define REG_MCAN0_TXBCF                   (0x400300DCU) /**< \brief (MCAN0) Transmit Buffer Cancellation Finished Register */
  #define REG_MCAN0_TXBTIE                  (0x400300E0U) /**< \brief (MCAN0) Transmit Buffer Transmission Interrupt Enable Register */
  #define REG_MCAN0_TXBCIE                  (0x400300E4U) /**< \brief (MCAN0) Transmit Buffer Cancellation Finished Interrupt Enable Register */
  #define REG_MCAN0_TXEFC                   (0x400300F0U) /**< \brief (MCAN0) Transmit Event FIFO Configuration Register */
  #define REG_MCAN0_TXEFS                   (0x400300F4U) /**< \brief (MCAN0) Transmit Event FIFO Status Register */
  #define REG_MCAN0_TXEFA                   (0x400300F8U) /**< \brief (MCAN0) Transmit Event FIFO Acknowledge Register */
#else
  #define REG_MCAN0_CREL   (*(__I  uint32_t*)0x40030000U) /**< \brief (MCAN0) Core Release Register */
  #define REG_MCAN0_ENDN   (*(__I  uint32_t*)0x40030004U) /**< \brief (MCAN0) Endian Register */
  #define REG_MCAN0_CUST   (*(__IO uint32_t*)0x40030008U) /**< \brief (MCAN0) Customer Register */
  #define REG_MCAN0_FBTP   (*(__IO uint32_t*)0x4003000CU) /**< \brief (MCAN0) Fast Bit Timing and Prescaler Register */
  #define REG_MCAN0_DBTP   (*(__IO uint32_t*)0x4003000CU) /**< \brief (MCAN0) Data Bit Timing and Prescaler Register */
  #define REG_MCAN0_TEST   (*(__IO uint32_t*)0x40030010U) /**< \brief (MCAN0) Test Register */
  #define REG_MCAN0_RWD    (*(__IO uint32_t*)0x40030014U) /**< \brief (MCAN0) RAM Watchdog Register */
  #define REG_MCAN0_CCCR   (*(__IO uint32_t*)0x40030018U) /**< \brief (MCAN0) CC Control Register */
  #define REG_MCAN0_BTP    (*(__IO uint32_t*)0x4003001CU) /**< \brief (MCAN0) Bit Timing and Prescaler Register */
  #define REG_MCAN0_NBTP   (*(__IO uint32_t*)0x4003001CU) /**< \brief (MCAN0) Nominal Bit Timing and Prescaler Register */
  #define REG_MCAN0_TSCC   (*(__IO uint32_t*)0x40030020U) /**< \brief (MCAN0) Timestamp Counter Configuration Register */
  #define REG_MCAN0_TSCV   (*(__IO uint32_t*)0x40030024U) /**< \brief (MCAN0) Timestamp Counter Value Register */
  #define REG_MCAN0_TOCC   (*(__IO uint32_t*)0x40030028U) /**< \brief (MCAN0) Timeout Counter Configuration Register */
  #define REG_MCAN0_TOCV   (*(__IO uint32_t*)0x4003002CU) /**< \brief (MCAN0) Timeout Counter Value Register */
  #define REG_MCAN0_ECR    (*(__I  uint32_t*)0x40030040U) /**< \brief (MCAN0) Error Counter Register */
  #define REG_MCAN0_PSR    (*(__I  uint32_t*)0x40030044U) /**< \brief (MCAN0) Protocol Status Register */
  #define REG_MCAN0_TDCR   (*(__IO uint32_t*)0x40030048U) /**< \brief (MCAN0) Transmit Delay Compensation Register */
  #define REG_MCAN0_IR     (*(__IO uint32_t*)0x40030050U) /**< \brief (MCAN0) Interrupt Register */
  #define REG_MCAN0_IE     (*(__IO uint32_t*)0x40030054U) /**< \brief (MCAN0) Interrupt Enable Register */
  #define REG_MCAN0_ILS    (*(__IO uint32_t*)0x40030058U) /**< \brief (MCAN0) Interrupt Line Select Register */
  #define REG_MCAN0_ILE    (*(__IO uint32_t*)0x4003005CU) /**< \brief (MCAN0) Interrupt Line Enable Register */
  #define REG_MCAN0_GFC    (*(__IO uint32_t*)0x40030080U) /**< \brief (MCAN0) Global Filter Configuration Register */
  #define REG_MCAN0_SIDFC  (*(__IO uint32_t*)0x40030084U) /**< \brief (MCAN0) Standard ID Filter Configuration Register */
  #define REG_MCAN0_XIDFC  (*(__IO uint32_t*)0x40030088U) /**< \brief (MCAN0) Extended ID Filter Configuration Register */
  #define REG_MCAN0_XIDAM  (*(__IO uint32_t*)0x40030090U) /**< \brief (MCAN0) Extended ID AND Mask Register */
  #define REG_MCAN0_HPMS   (*(__I  uint32_t*)0x40030094U) /**< \brief (MCAN0) High Priority Message Status Register */
  #define REG_MCAN0_NDAT1  (*(__IO uint32_t*)0x40030098U) /**< \brief (MCAN0) New Data 1 Register */
  #define REG_MCAN0_NDAT2  (*(__IO uint32_t*)0x4003009CU) /**< \brief (MCAN0) New Data 2 Register */
  #define REG_MCAN0_RXF0C  (*(__IO uint32_t*)0x400300A0U) /**< \brief (MCAN0) Receive FIFO 0 Configuration Register */
  #define REG_MCAN0_RXF0S  (*(__I  uint32_t*)0x400300A4U) /**< \brief (MCAN0) Receive FIFO 0 Status Register */
  #define REG_MCAN0_RXF0A  (*(__IO uint32_t*)0x400300A8U) /**< \brief (MCAN0) Receive FIFO 0 Acknowledge Register */
  #define REG_MCAN0_RXBC   (*(__IO uint32_t*)0x400300ACU) /**< \brief (MCAN0) Receive Rx Buffer Configuration Register */
  #define REG_MCAN0_RXF1C  (*(__IO uint32_t*)0x400300B0U) /**< \brief (MCAN0) Receive FIFO 1 Configuration Register */
  #define REG_MCAN0_RXF1S  (*(__I  uint32_t*)0x400300B4U) /**< \brief (MCAN0) Receive FIFO 1 Status Register */
  #define REG_MCAN0_RXF1A  (*(__IO uint32_t*)0x400300B8U) /**< \brief (MCAN0) Receive FIFO 1 Acknowledge Register */
  #define REG_MCAN0_RXESC  (*(__IO uint32_t*)0x400300BCU) /**< \brief (MCAN0) Receive Buffer / FIFO Element Size Configuration Register */
  #define REG_MCAN0_TXBC   (*(__IO uint32_t*)0x400300C0U) /**< \brief (MCAN0) Transmit Buffer Configuration Register */
  #define REG_MCAN0_TXFQS  (*(__I  uint32_t*)0x400300C4U) /**< \brief (MCAN0) Transmit FIFO/Queue Status Register */
  #define REG_MCAN0_TXESC  (*(__IO uint32_t*)0x400300C8U) /**< \brief (MCAN0) Transmit Buffer Element Size Configuration Register */
  #define REG_MCAN0_TXBRP  (*(__I  uint32_t*)0x400300CCU) /**< \brief (MCAN0) Transmit Buffer Request Pending Register */
  #define REG_MCAN0_TXBAR  (*(__IO uint32_t*)0x400300D0U) /**< \brief (MCAN0) Transmit Buffer Add Request Register */
  #define REG_MCAN0_TXBCR  (*(__IO uint32_t*)0x400300D4U) /**< \brief (MCAN0) Transmit Buffer Cancellation Request Register */
  #define REG_MCAN0_TXBTO  (*(__I  uint32_t*)0x400300D8U) /**< \brief (MCAN0) Transmit Buffer Transmission Occurred Register */
  #define REG_MCAN0_TXBCF  (*(__I  uint32_t*)0x400300DCU) /**< \brief (MCAN0) Transmit Buffer Cancellation Finished Register */
  #define REG_MCAN0_TXBTIE (*(__IO uint32_t*)0x400300E0U) /**< \brief (MCAN0) Transmit Buffer Transmission Interrupt Enable Register */
  #define REG_MCAN0_TXBCIE (*(__IO uint32_t*)0x400300E4U) /**< \brief (MCAN0) Transmit Buffer Cancellation Finished Interrupt Enable Register */
  #define REG_MCAN0_TXEFC  (*(__IO uint32_t*)0x400300F0U) /**< \brief (MCAN0) Transmit Event FIFO Configuration Register */
  #define REG_MCAN0_TXEFS  (*(__I  uint32_t*)0x400300F4U) /**< \brief (MCAN0) Transmit Event FIFO Status Register */
  #define REG_MCAN0_TXEFA  (*(__IO uint32_t*)0x400300F8U) /**< \brief (MCAN0) Transmit Event FIFO Acknowledge Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_MCAN0_INSTANCE_ */
