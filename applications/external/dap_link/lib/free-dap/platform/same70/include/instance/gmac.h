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

#ifndef _SAME70_GMAC_INSTANCE_
#define _SAME70_GMAC_INSTANCE_

/* ========== Register definition for GMAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_GMAC_NCR                        (0x40050000U) /**< \brief (GMAC) Network Control Register */
  #define REG_GMAC_NCFGR                      (0x40050004U) /**< \brief (GMAC) Network Configuration Register */
  #define REG_GMAC_NSR                        (0x40050008U) /**< \brief (GMAC) Network Status Register */
  #define REG_GMAC_UR                         (0x4005000CU) /**< \brief (GMAC) User Register */
  #define REG_GMAC_DCFGR                      (0x40050010U) /**< \brief (GMAC) DMA Configuration Register */
  #define REG_GMAC_TSR                        (0x40050014U) /**< \brief (GMAC) Transmit Status Register */
  #define REG_GMAC_RBQB                       (0x40050018U) /**< \brief (GMAC) Receive Buffer Queue Base Address Register */
  #define REG_GMAC_TBQB                       (0x4005001CU) /**< \brief (GMAC) Transmit Buffer Queue Base Address Register */
  #define REG_GMAC_RSR                        (0x40050020U) /**< \brief (GMAC) Receive Status Register */
  #define REG_GMAC_ISR                        (0x40050024U) /**< \brief (GMAC) Interrupt Status Register */
  #define REG_GMAC_IER                        (0x40050028U) /**< \brief (GMAC) Interrupt Enable Register */
  #define REG_GMAC_IDR                        (0x4005002CU) /**< \brief (GMAC) Interrupt Disable Register */
  #define REG_GMAC_IMR                        (0x40050030U) /**< \brief (GMAC) Interrupt Mask Register */
  #define REG_GMAC_MAN                        (0x40050034U) /**< \brief (GMAC) PHY Maintenance Register */
  #define REG_GMAC_RPQ                        (0x40050038U) /**< \brief (GMAC) Received Pause Quantum Register */
  #define REG_GMAC_TPQ                        (0x4005003CU) /**< \brief (GMAC) Transmit Pause Quantum Register */
  #define REG_GMAC_TPSF                       (0x40050040U) /**< \brief (GMAC) TX Partial Store and Forward Register */
  #define REG_GMAC_RPSF                       (0x40050044U) /**< \brief (GMAC) RX Partial Store and Forward Register */
  #define REG_GMAC_RJFML                      (0x40050048U) /**< \brief (GMAC) RX Jumbo Frame Max Length Register */
  #define REG_GMAC_HRB                        (0x40050080U) /**< \brief (GMAC) Hash Register Bottom */
  #define REG_GMAC_HRT                        (0x40050084U) /**< \brief (GMAC) Hash Register Top */
  #define REG_GMAC_SAB1                       (0x40050088U) /**< \brief (GMAC) Specific Address 1 Bottom Register */
  #define REG_GMAC_SAT1                       (0x4005008CU) /**< \brief (GMAC) Specific Address 1 Top Register */
  #define REG_GMAC_SAB2                       (0x40050090U) /**< \brief (GMAC) Specific Address 2 Bottom Register */
  #define REG_GMAC_SAT2                       (0x40050094U) /**< \brief (GMAC) Specific Address 2 Top Register */
  #define REG_GMAC_SAB3                       (0x40050098U) /**< \brief (GMAC) Specific Address 3 Bottom Register */
  #define REG_GMAC_SAT3                       (0x4005009CU) /**< \brief (GMAC) Specific Address 3 Top Register */
  #define REG_GMAC_SAB4                       (0x400500A0U) /**< \brief (GMAC) Specific Address 4 Bottom Register */
  #define REG_GMAC_SAT4                       (0x400500A4U) /**< \brief (GMAC) Specific Address 4 Top Register */
  #define REG_GMAC_TIDM1                      (0x400500A8U) /**< \brief (GMAC) Type ID Match 1 Register */
  #define REG_GMAC_TIDM2                      (0x400500ACU) /**< \brief (GMAC) Type ID Match 2 Register */
  #define REG_GMAC_TIDM3                      (0x400500B0U) /**< \brief (GMAC) Type ID Match 3 Register */
  #define REG_GMAC_TIDM4                      (0x400500B4U) /**< \brief (GMAC) Type ID Match 4 Register */
  #define REG_GMAC_WOL                        (0x400500B8U) /**< \brief (GMAC) Wake on LAN Register */
  #define REG_GMAC_IPGS                       (0x400500BCU) /**< \brief (GMAC) IPG Stretch Register */
  #define REG_GMAC_SVLAN                      (0x400500C0U) /**< \brief (GMAC) Stacked VLAN Register */
  #define REG_GMAC_TPFCP                      (0x400500C4U) /**< \brief (GMAC) Transmit PFC Pause Register */
  #define REG_GMAC_SAMB1                      (0x400500C8U) /**< \brief (GMAC) Specific Address 1 Mask Bottom Register */
  #define REG_GMAC_SAMT1                      (0x400500CCU) /**< \brief (GMAC) Specific Address 1 Mask Top Register */
  #define REG_GMAC_NSC                        (0x400500DCU) /**< \brief (GMAC) 1588 Timer Nanosecond Comparison Register */
  #define REG_GMAC_SCL                        (0x400500E0U) /**< \brief (GMAC) 1588 Timer Second Comparison Low Register */
  #define REG_GMAC_SCH                        (0x400500E4U) /**< \brief (GMAC) 1588 Timer Second Comparison High Register */
  #define REG_GMAC_EFTSH                      (0x400500E8U) /**< \brief (GMAC) PTP Event Frame Transmitted Seconds High Register */
  #define REG_GMAC_EFRSH                      (0x400500ECU) /**< \brief (GMAC) PTP Event Frame Received Seconds High Register */
  #define REG_GMAC_PEFTSH                     (0x400500F0U) /**< \brief (GMAC) PTP Peer Event Frame Transmitted Seconds High Register */
  #define REG_GMAC_PEFRSH                     (0x400500F4U) /**< \brief (GMAC) PTP Peer Event Frame Received Seconds High Register */
  #define REG_GMAC_MID                        (0x400500FCU) /**< \brief (GMAC) Module ID Register */
  #define REG_GMAC_OTLO                       (0x40050100U) /**< \brief (GMAC) Octets Transmitted Low Register */
  #define REG_GMAC_OTHI                       (0x40050104U) /**< \brief (GMAC) Octets Transmitted High Register */
  #define REG_GMAC_FT                         (0x40050108U) /**< \brief (GMAC) Frames Transmitted Register */
  #define REG_GMAC_BCFT                       (0x4005010CU) /**< \brief (GMAC) Broadcast Frames Transmitted Register */
  #define REG_GMAC_MFT                        (0x40050110U) /**< \brief (GMAC) Multicast Frames Transmitted Register */
  #define REG_GMAC_PFT                        (0x40050114U) /**< \brief (GMAC) Pause Frames Transmitted Register */
  #define REG_GMAC_BFT64                      (0x40050118U) /**< \brief (GMAC) 64 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT127                    (0x4005011CU) /**< \brief (GMAC) 65 to 127 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT255                    (0x40050120U) /**< \brief (GMAC) 128 to 255 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT511                    (0x40050124U) /**< \brief (GMAC) 256 to 511 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT1023                   (0x40050128U) /**< \brief (GMAC) 512 to 1023 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT1518                   (0x4005012CU) /**< \brief (GMAC) 1024 to 1518 Byte Frames Transmitted Register */
  #define REG_GMAC_GTBFT1518                  (0x40050130U) /**< \brief (GMAC) Greater Than 1518 Byte Frames Transmitted Register */
  #define REG_GMAC_TUR                        (0x40050134U) /**< \brief (GMAC) Transmit Underruns Register */
  #define REG_GMAC_SCF                        (0x40050138U) /**< \brief (GMAC) Single Collision Frames Register */
  #define REG_GMAC_MCF                        (0x4005013CU) /**< \brief (GMAC) Multiple Collision Frames Register */
  #define REG_GMAC_EC                         (0x40050140U) /**< \brief (GMAC) Excessive Collisions Register */
  #define REG_GMAC_LC                         (0x40050144U) /**< \brief (GMAC) Late Collisions Register */
  #define REG_GMAC_DTF                        (0x40050148U) /**< \brief (GMAC) Deferred Transmission Frames Register */
  #define REG_GMAC_CSE                        (0x4005014CU) /**< \brief (GMAC) Carrier Sense Errors Register */
  #define REG_GMAC_ORLO                       (0x40050150U) /**< \brief (GMAC) Octets Received Low Received Register */
  #define REG_GMAC_ORHI                       (0x40050154U) /**< \brief (GMAC) Octets Received High Received Register */
  #define REG_GMAC_FR                         (0x40050158U) /**< \brief (GMAC) Frames Received Register */
  #define REG_GMAC_BCFR                       (0x4005015CU) /**< \brief (GMAC) Broadcast Frames Received Register */
  #define REG_GMAC_MFR                        (0x40050160U) /**< \brief (GMAC) Multicast Frames Received Register */
  #define REG_GMAC_PFR                        (0x40050164U) /**< \brief (GMAC) Pause Frames Received Register */
  #define REG_GMAC_BFR64                      (0x40050168U) /**< \brief (GMAC) 64 Byte Frames Received Register */
  #define REG_GMAC_TBFR127                    (0x4005016CU) /**< \brief (GMAC) 65 to 127 Byte Frames Received Register */
  #define REG_GMAC_TBFR255                    (0x40050170U) /**< \brief (GMAC) 128 to 255 Byte Frames Received Register */
  #define REG_GMAC_TBFR511                    (0x40050174U) /**< \brief (GMAC) 256 to 511 Byte Frames Received Register */
  #define REG_GMAC_TBFR1023                   (0x40050178U) /**< \brief (GMAC) 512 to 1023 Byte Frames Received Register */
  #define REG_GMAC_TBFR1518                   (0x4005017CU) /**< \brief (GMAC) 1024 to 1518 Byte Frames Received Register */
  #define REG_GMAC_TMXBFR                     (0x40050180U) /**< \brief (GMAC) 1519 to Maximum Byte Frames Received Register */
  #define REG_GMAC_UFR                        (0x40050184U) /**< \brief (GMAC) Undersize Frames Received Register */
  #define REG_GMAC_OFR                        (0x40050188U) /**< \brief (GMAC) Oversize Frames Received Register */
  #define REG_GMAC_JR                         (0x4005018CU) /**< \brief (GMAC) Jabbers Received Register */
  #define REG_GMAC_FCSE                       (0x40050190U) /**< \brief (GMAC) Frame Check Sequence Errors Register */
  #define REG_GMAC_LFFE                       (0x40050194U) /**< \brief (GMAC) Length Field Frame Errors Register */
  #define REG_GMAC_RSE                        (0x40050198U) /**< \brief (GMAC) Receive Symbol Errors Register */
  #define REG_GMAC_AE                         (0x4005019CU) /**< \brief (GMAC) Alignment Errors Register */
  #define REG_GMAC_RRE                        (0x400501A0U) /**< \brief (GMAC) Receive Resource Errors Register */
  #define REG_GMAC_ROE                        (0x400501A4U) /**< \brief (GMAC) Receive Overrun Register */
  #define REG_GMAC_IHCE                       (0x400501A8U) /**< \brief (GMAC) IP Header Checksum Errors Register */
  #define REG_GMAC_TCE                        (0x400501ACU) /**< \brief (GMAC) TCP Checksum Errors Register */
  #define REG_GMAC_UCE                        (0x400501B0U) /**< \brief (GMAC) UDP Checksum Errors Register */
  #define REG_GMAC_TISUBN                     (0x400501BCU) /**< \brief (GMAC) 1588 Timer Increment Sub-nanoseconds Register */
  #define REG_GMAC_TSH                        (0x400501C0U) /**< \brief (GMAC) 1588 Timer Seconds High Register */
  #define REG_GMAC_TSL                        (0x400501D0U) /**< \brief (GMAC) 1588 Timer Seconds Low Register */
  #define REG_GMAC_TN                         (0x400501D4U) /**< \brief (GMAC) 1588 Timer Nanoseconds Register */
  #define REG_GMAC_TA                         (0x400501D8U) /**< \brief (GMAC) 1588 Timer Adjust Register */
  #define REG_GMAC_TI                         (0x400501DCU) /**< \brief (GMAC) 1588 Timer Increment Register */
  #define REG_GMAC_EFTSL                      (0x400501E0U) /**< \brief (GMAC) PTP Event Frame Transmitted Seconds Low Register */
  #define REG_GMAC_EFTN                       (0x400501E4U) /**< \brief (GMAC) PTP Event Frame Transmitted Nanoseconds Register */
  #define REG_GMAC_EFRSL                      (0x400501E8U) /**< \brief (GMAC) PTP Event Frame Received Seconds Low Register */
  #define REG_GMAC_EFRN                       (0x400501ECU) /**< \brief (GMAC) PTP Event Frame Received Nanoseconds Register */
  #define REG_GMAC_PEFTSL                     (0x400501F0U) /**< \brief (GMAC) PTP Peer Event Frame Transmitted Seconds Low Register */
  #define REG_GMAC_PEFTN                      (0x400501F4U) /**< \brief (GMAC) PTP Peer Event Frame Transmitted Nanoseconds Register */
  #define REG_GMAC_PEFRSL                     (0x400501F8U) /**< \brief (GMAC) PTP Peer Event Frame Received Seconds Low Register */
  #define REG_GMAC_PEFRN                      (0x400501FCU) /**< \brief (GMAC) PTP Peer Event Frame Received Nanoseconds Register */
  #define REG_GMAC_RXLPI                      (0x40050270U) /**< \brief (GMAC) Received LPI Transitions */
  #define REG_GMAC_RXLPITIME                  (0x40050274U) /**< \brief (GMAC) Received LPI Time */
  #define REG_GMAC_TXLPI                      (0x40050278U) /**< \brief (GMAC) Transmit LPI Transitions */
  #define REG_GMAC_TXLPITIME                  (0x4005027CU) /**< \brief (GMAC) Transmit LPI Time */
  #define REG_GMAC_ISRPQ                      (0x400503FCU) /**< \brief (GMAC) Interrupt Status Register Priority Queue (index = 1) */
  #define REG_GMAC_TBQBAPQ                    (0x4005043CU) /**< \brief (GMAC) Transmit Buffer Queue Base Address Register Priority Queue (index = 1) */
  #define REG_GMAC_RBQBAPQ                    (0x4005047CU) /**< \brief (GMAC) Receive Buffer Queue Base Address Register Priority Queue (index = 1) */
  #define REG_GMAC_RBSRPQ                     (0x4005049CU) /**< \brief (GMAC) Receive Buffer Size Register Priority Queue (index = 1) */
  #define REG_GMAC_CBSCR                      (0x400504BCU) /**< \brief (GMAC) Credit-Based Shaping Control Register */
  #define REG_GMAC_CBSISQA                    (0x400504C0U) /**< \brief (GMAC) Credit-Based Shaping IdleSlope Register for Queue A */
  #define REG_GMAC_CBSISQB                    (0x400504C4U) /**< \brief (GMAC) Credit-Based Shaping IdleSlope Register for Queue B */
  #define REG_GMAC_ST1RPQ                     (0x40050500U) /**< \brief (GMAC) Screening Type 1 Register Priority Queue (index = 0) */
  #define REG_GMAC_ST2RPQ                     (0x40050540U) /**< \brief (GMAC) Screening Type 2 Register Priority Queue (index = 0) */
  #define REG_GMAC_IERPQ                      (0x400505FCU) /**< \brief (GMAC) Interrupt Enable Register Priority Queue (index = 1) */
  #define REG_GMAC_IDRPQ                      (0x4005061CU) /**< \brief (GMAC) Interrupt Disable Register Priority Queue (index = 1) */
  #define REG_GMAC_IMRPQ                      (0x4005063CU) /**< \brief (GMAC) Interrupt Mask Register Priority Queue (index = 1) */
  #define REG_GMAC_ST2ER                      (0x400506E0U) /**< \brief (GMAC) Screening Type 2 Ethertype Register (index = 0) */
  #define REG_GMAC_ST2CW00                    (0x40050700U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 0) */
  #define REG_GMAC_ST2CW10                    (0x40050704U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 0) */
  #define REG_GMAC_ST2CW01                    (0x40050708U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 1) */
  #define REG_GMAC_ST2CW11                    (0x4005070CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 1) */
  #define REG_GMAC_ST2CW02                    (0x40050710U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 2) */
  #define REG_GMAC_ST2CW12                    (0x40050714U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 2) */
  #define REG_GMAC_ST2CW03                    (0x40050718U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 3) */
  #define REG_GMAC_ST2CW13                    (0x4005071CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 3) */
  #define REG_GMAC_ST2CW04                    (0x40050720U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 4) */
  #define REG_GMAC_ST2CW14                    (0x40050724U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 4) */
  #define REG_GMAC_ST2CW05                    (0x40050728U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 5) */
  #define REG_GMAC_ST2CW15                    (0x4005072CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 5) */
  #define REG_GMAC_ST2CW06                    (0x40050730U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 6) */
  #define REG_GMAC_ST2CW16                    (0x40050734U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 6) */
  #define REG_GMAC_ST2CW07                    (0x40050738U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 7) */
  #define REG_GMAC_ST2CW17                    (0x4005073CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 7) */
  #define REG_GMAC_ST2CW08                    (0x40050740U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 8) */
  #define REG_GMAC_ST2CW18                    (0x40050744U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 8) */
  #define REG_GMAC_ST2CW09                    (0x40050748U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 9) */
  #define REG_GMAC_ST2CW19                    (0x4005074CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 9) */
  #define REG_GMAC_ST2CW010                   (0x40050750U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 10) */
  #define REG_GMAC_ST2CW110                   (0x40050754U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 10) */
  #define REG_GMAC_ST2CW011                   (0x40050758U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 11) */
  #define REG_GMAC_ST2CW111                   (0x4005075CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 11) */
  #define REG_GMAC_ST2CW012                   (0x40050760U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 12) */
  #define REG_GMAC_ST2CW112                   (0x40050764U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 12) */
  #define REG_GMAC_ST2CW013                   (0x40050768U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 13) */
  #define REG_GMAC_ST2CW113                   (0x4005076CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 13) */
  #define REG_GMAC_ST2CW014                   (0x40050770U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 14) */
  #define REG_GMAC_ST2CW114                   (0x40050774U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 14) */
  #define REG_GMAC_ST2CW015                   (0x40050778U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 15) */
  #define REG_GMAC_ST2CW115                   (0x4005077CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 15) */
  #define REG_GMAC_ST2CW016                   (0x40050780U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 16) */
  #define REG_GMAC_ST2CW116                   (0x40050784U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 16) */
  #define REG_GMAC_ST2CW017                   (0x40050788U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 17) */
  #define REG_GMAC_ST2CW117                   (0x4005078CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 17) */
  #define REG_GMAC_ST2CW018                   (0x40050790U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 18) */
  #define REG_GMAC_ST2CW118                   (0x40050794U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 18) */
  #define REG_GMAC_ST2CW019                   (0x40050798U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 19) */
  #define REG_GMAC_ST2CW119                   (0x4005079CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 19) */
  #define REG_GMAC_ST2CW020                   (0x400507A0U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 20) */
  #define REG_GMAC_ST2CW120                   (0x400507A4U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 20) */
  #define REG_GMAC_ST2CW021                   (0x400507A8U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 21) */
  #define REG_GMAC_ST2CW121                   (0x400507ACU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 21) */
  #define REG_GMAC_ST2CW022                   (0x400507B0U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 22) */
  #define REG_GMAC_ST2CW122                   (0x400507B4U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 22) */
  #define REG_GMAC_ST2CW023                   (0x400507B8U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 23) */
  #define REG_GMAC_ST2CW123                   (0x400507BCU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 23) */
#else
  #define REG_GMAC_NCR       (*(__IO uint32_t*)0x40050000U) /**< \brief (GMAC) Network Control Register */
  #define REG_GMAC_NCFGR     (*(__IO uint32_t*)0x40050004U) /**< \brief (GMAC) Network Configuration Register */
  #define REG_GMAC_NSR       (*(__I  uint32_t*)0x40050008U) /**< \brief (GMAC) Network Status Register */
  #define REG_GMAC_UR        (*(__IO uint32_t*)0x4005000CU) /**< \brief (GMAC) User Register */
  #define REG_GMAC_DCFGR     (*(__IO uint32_t*)0x40050010U) /**< \brief (GMAC) DMA Configuration Register */
  #define REG_GMAC_TSR       (*(__IO uint32_t*)0x40050014U) /**< \brief (GMAC) Transmit Status Register */
  #define REG_GMAC_RBQB      (*(__IO uint32_t*)0x40050018U) /**< \brief (GMAC) Receive Buffer Queue Base Address Register */
  #define REG_GMAC_TBQB      (*(__IO uint32_t*)0x4005001CU) /**< \brief (GMAC) Transmit Buffer Queue Base Address Register */
  #define REG_GMAC_RSR       (*(__IO uint32_t*)0x40050020U) /**< \brief (GMAC) Receive Status Register */
  #define REG_GMAC_ISR       (*(__I  uint32_t*)0x40050024U) /**< \brief (GMAC) Interrupt Status Register */
  #define REG_GMAC_IER       (*(__O  uint32_t*)0x40050028U) /**< \brief (GMAC) Interrupt Enable Register */
  #define REG_GMAC_IDR       (*(__O  uint32_t*)0x4005002CU) /**< \brief (GMAC) Interrupt Disable Register */
  #define REG_GMAC_IMR       (*(__IO uint32_t*)0x40050030U) /**< \brief (GMAC) Interrupt Mask Register */
  #define REG_GMAC_MAN       (*(__IO uint32_t*)0x40050034U) /**< \brief (GMAC) PHY Maintenance Register */
  #define REG_GMAC_RPQ       (*(__I  uint32_t*)0x40050038U) /**< \brief (GMAC) Received Pause Quantum Register */
  #define REG_GMAC_TPQ       (*(__IO uint32_t*)0x4005003CU) /**< \brief (GMAC) Transmit Pause Quantum Register */
  #define REG_GMAC_TPSF      (*(__IO uint32_t*)0x40050040U) /**< \brief (GMAC) TX Partial Store and Forward Register */
  #define REG_GMAC_RPSF      (*(__IO uint32_t*)0x40050044U) /**< \brief (GMAC) RX Partial Store and Forward Register */
  #define REG_GMAC_RJFML     (*(__IO uint32_t*)0x40050048U) /**< \brief (GMAC) RX Jumbo Frame Max Length Register */
  #define REG_GMAC_HRB       (*(__IO uint32_t*)0x40050080U) /**< \brief (GMAC) Hash Register Bottom */
  #define REG_GMAC_HRT       (*(__IO uint32_t*)0x40050084U) /**< \brief (GMAC) Hash Register Top */
  #define REG_GMAC_SAB1      (*(__IO uint32_t*)0x40050088U) /**< \brief (GMAC) Specific Address 1 Bottom Register */
  #define REG_GMAC_SAT1      (*(__IO uint32_t*)0x4005008CU) /**< \brief (GMAC) Specific Address 1 Top Register */
  #define REG_GMAC_SAB2      (*(__IO uint32_t*)0x40050090U) /**< \brief (GMAC) Specific Address 2 Bottom Register */
  #define REG_GMAC_SAT2      (*(__IO uint32_t*)0x40050094U) /**< \brief (GMAC) Specific Address 2 Top Register */
  #define REG_GMAC_SAB3      (*(__IO uint32_t*)0x40050098U) /**< \brief (GMAC) Specific Address 3 Bottom Register */
  #define REG_GMAC_SAT3      (*(__IO uint32_t*)0x4005009CU) /**< \brief (GMAC) Specific Address 3 Top Register */
  #define REG_GMAC_SAB4      (*(__IO uint32_t*)0x400500A0U) /**< \brief (GMAC) Specific Address 4 Bottom Register */
  #define REG_GMAC_SAT4      (*(__IO uint32_t*)0x400500A4U) /**< \brief (GMAC) Specific Address 4 Top Register */
  #define REG_GMAC_TIDM1     (*(__IO uint32_t*)0x400500A8U) /**< \brief (GMAC) Type ID Match 1 Register */
  #define REG_GMAC_TIDM2     (*(__IO uint32_t*)0x400500ACU) /**< \brief (GMAC) Type ID Match 2 Register */
  #define REG_GMAC_TIDM3     (*(__IO uint32_t*)0x400500B0U) /**< \brief (GMAC) Type ID Match 3 Register */
  #define REG_GMAC_TIDM4     (*(__IO uint32_t*)0x400500B4U) /**< \brief (GMAC) Type ID Match 4 Register */
  #define REG_GMAC_WOL       (*(__IO uint32_t*)0x400500B8U) /**< \brief (GMAC) Wake on LAN Register */
  #define REG_GMAC_IPGS      (*(__IO uint32_t*)0x400500BCU) /**< \brief (GMAC) IPG Stretch Register */
  #define REG_GMAC_SVLAN     (*(__IO uint32_t*)0x400500C0U) /**< \brief (GMAC) Stacked VLAN Register */
  #define REG_GMAC_TPFCP     (*(__IO uint32_t*)0x400500C4U) /**< \brief (GMAC) Transmit PFC Pause Register */
  #define REG_GMAC_SAMB1     (*(__IO uint32_t*)0x400500C8U) /**< \brief (GMAC) Specific Address 1 Mask Bottom Register */
  #define REG_GMAC_SAMT1     (*(__IO uint32_t*)0x400500CCU) /**< \brief (GMAC) Specific Address 1 Mask Top Register */
  #define REG_GMAC_NSC       (*(__IO uint32_t*)0x400500DCU) /**< \brief (GMAC) 1588 Timer Nanosecond Comparison Register */
  #define REG_GMAC_SCL       (*(__IO uint32_t*)0x400500E0U) /**< \brief (GMAC) 1588 Timer Second Comparison Low Register */
  #define REG_GMAC_SCH       (*(__IO uint32_t*)0x400500E4U) /**< \brief (GMAC) 1588 Timer Second Comparison High Register */
  #define REG_GMAC_EFTSH     (*(__I  uint32_t*)0x400500E8U) /**< \brief (GMAC) PTP Event Frame Transmitted Seconds High Register */
  #define REG_GMAC_EFRSH     (*(__I  uint32_t*)0x400500ECU) /**< \brief (GMAC) PTP Event Frame Received Seconds High Register */
  #define REG_GMAC_PEFTSH    (*(__I  uint32_t*)0x400500F0U) /**< \brief (GMAC) PTP Peer Event Frame Transmitted Seconds High Register */
  #define REG_GMAC_PEFRSH    (*(__I  uint32_t*)0x400500F4U) /**< \brief (GMAC) PTP Peer Event Frame Received Seconds High Register */
  #define REG_GMAC_MID       (*(__I  uint32_t*)0x400500FCU) /**< \brief (GMAC) Module ID Register */
  #define REG_GMAC_OTLO      (*(__I  uint32_t*)0x40050100U) /**< \brief (GMAC) Octets Transmitted Low Register */
  #define REG_GMAC_OTHI      (*(__I  uint32_t*)0x40050104U) /**< \brief (GMAC) Octets Transmitted High Register */
  #define REG_GMAC_FT        (*(__I  uint32_t*)0x40050108U) /**< \brief (GMAC) Frames Transmitted Register */
  #define REG_GMAC_BCFT      (*(__I  uint32_t*)0x4005010CU) /**< \brief (GMAC) Broadcast Frames Transmitted Register */
  #define REG_GMAC_MFT       (*(__I  uint32_t*)0x40050110U) /**< \brief (GMAC) Multicast Frames Transmitted Register */
  #define REG_GMAC_PFT       (*(__I  uint32_t*)0x40050114U) /**< \brief (GMAC) Pause Frames Transmitted Register */
  #define REG_GMAC_BFT64     (*(__I  uint32_t*)0x40050118U) /**< \brief (GMAC) 64 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT127   (*(__I  uint32_t*)0x4005011CU) /**< \brief (GMAC) 65 to 127 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT255   (*(__I  uint32_t*)0x40050120U) /**< \brief (GMAC) 128 to 255 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT511   (*(__I  uint32_t*)0x40050124U) /**< \brief (GMAC) 256 to 511 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT1023  (*(__I  uint32_t*)0x40050128U) /**< \brief (GMAC) 512 to 1023 Byte Frames Transmitted Register */
  #define REG_GMAC_TBFT1518  (*(__I  uint32_t*)0x4005012CU) /**< \brief (GMAC) 1024 to 1518 Byte Frames Transmitted Register */
  #define REG_GMAC_GTBFT1518 (*(__I  uint32_t*)0x40050130U) /**< \brief (GMAC) Greater Than 1518 Byte Frames Transmitted Register */
  #define REG_GMAC_TUR       (*(__I  uint32_t*)0x40050134U) /**< \brief (GMAC) Transmit Underruns Register */
  #define REG_GMAC_SCF       (*(__I  uint32_t*)0x40050138U) /**< \brief (GMAC) Single Collision Frames Register */
  #define REG_GMAC_MCF       (*(__I  uint32_t*)0x4005013CU) /**< \brief (GMAC) Multiple Collision Frames Register */
  #define REG_GMAC_EC        (*(__I  uint32_t*)0x40050140U) /**< \brief (GMAC) Excessive Collisions Register */
  #define REG_GMAC_LC        (*(__I  uint32_t*)0x40050144U) /**< \brief (GMAC) Late Collisions Register */
  #define REG_GMAC_DTF       (*(__I  uint32_t*)0x40050148U) /**< \brief (GMAC) Deferred Transmission Frames Register */
  #define REG_GMAC_CSE       (*(__I  uint32_t*)0x4005014CU) /**< \brief (GMAC) Carrier Sense Errors Register */
  #define REG_GMAC_ORLO      (*(__I  uint32_t*)0x40050150U) /**< \brief (GMAC) Octets Received Low Received Register */
  #define REG_GMAC_ORHI      (*(__I  uint32_t*)0x40050154U) /**< \brief (GMAC) Octets Received High Received Register */
  #define REG_GMAC_FR        (*(__I  uint32_t*)0x40050158U) /**< \brief (GMAC) Frames Received Register */
  #define REG_GMAC_BCFR      (*(__I  uint32_t*)0x4005015CU) /**< \brief (GMAC) Broadcast Frames Received Register */
  #define REG_GMAC_MFR       (*(__I  uint32_t*)0x40050160U) /**< \brief (GMAC) Multicast Frames Received Register */
  #define REG_GMAC_PFR       (*(__I  uint32_t*)0x40050164U) /**< \brief (GMAC) Pause Frames Received Register */
  #define REG_GMAC_BFR64     (*(__I  uint32_t*)0x40050168U) /**< \brief (GMAC) 64 Byte Frames Received Register */
  #define REG_GMAC_TBFR127   (*(__I  uint32_t*)0x4005016CU) /**< \brief (GMAC) 65 to 127 Byte Frames Received Register */
  #define REG_GMAC_TBFR255   (*(__I  uint32_t*)0x40050170U) /**< \brief (GMAC) 128 to 255 Byte Frames Received Register */
  #define REG_GMAC_TBFR511   (*(__I  uint32_t*)0x40050174U) /**< \brief (GMAC) 256 to 511 Byte Frames Received Register */
  #define REG_GMAC_TBFR1023  (*(__I  uint32_t*)0x40050178U) /**< \brief (GMAC) 512 to 1023 Byte Frames Received Register */
  #define REG_GMAC_TBFR1518  (*(__I  uint32_t*)0x4005017CU) /**< \brief (GMAC) 1024 to 1518 Byte Frames Received Register */
  #define REG_GMAC_TMXBFR    (*(__I  uint32_t*)0x40050180U) /**< \brief (GMAC) 1519 to Maximum Byte Frames Received Register */
  #define REG_GMAC_UFR       (*(__I  uint32_t*)0x40050184U) /**< \brief (GMAC) Undersize Frames Received Register */
  #define REG_GMAC_OFR       (*(__I  uint32_t*)0x40050188U) /**< \brief (GMAC) Oversize Frames Received Register */
  #define REG_GMAC_JR        (*(__I  uint32_t*)0x4005018CU) /**< \brief (GMAC) Jabbers Received Register */
  #define REG_GMAC_FCSE      (*(__I  uint32_t*)0x40050190U) /**< \brief (GMAC) Frame Check Sequence Errors Register */
  #define REG_GMAC_LFFE      (*(__I  uint32_t*)0x40050194U) /**< \brief (GMAC) Length Field Frame Errors Register */
  #define REG_GMAC_RSE       (*(__I  uint32_t*)0x40050198U) /**< \brief (GMAC) Receive Symbol Errors Register */
  #define REG_GMAC_AE        (*(__I  uint32_t*)0x4005019CU) /**< \brief (GMAC) Alignment Errors Register */
  #define REG_GMAC_RRE       (*(__I  uint32_t*)0x400501A0U) /**< \brief (GMAC) Receive Resource Errors Register */
  #define REG_GMAC_ROE       (*(__I  uint32_t*)0x400501A4U) /**< \brief (GMAC) Receive Overrun Register */
  #define REG_GMAC_IHCE      (*(__I  uint32_t*)0x400501A8U) /**< \brief (GMAC) IP Header Checksum Errors Register */
  #define REG_GMAC_TCE       (*(__I  uint32_t*)0x400501ACU) /**< \brief (GMAC) TCP Checksum Errors Register */
  #define REG_GMAC_UCE       (*(__I  uint32_t*)0x400501B0U) /**< \brief (GMAC) UDP Checksum Errors Register */
  #define REG_GMAC_TISUBN    (*(__IO uint32_t*)0x400501BCU) /**< \brief (GMAC) 1588 Timer Increment Sub-nanoseconds Register */
  #define REG_GMAC_TSH       (*(__IO uint32_t*)0x400501C0U) /**< \brief (GMAC) 1588 Timer Seconds High Register */
  #define REG_GMAC_TSL       (*(__IO uint32_t*)0x400501D0U) /**< \brief (GMAC) 1588 Timer Seconds Low Register */
  #define REG_GMAC_TN        (*(__IO uint32_t*)0x400501D4U) /**< \brief (GMAC) 1588 Timer Nanoseconds Register */
  #define REG_GMAC_TA        (*(__O  uint32_t*)0x400501D8U) /**< \brief (GMAC) 1588 Timer Adjust Register */
  #define REG_GMAC_TI        (*(__IO uint32_t*)0x400501DCU) /**< \brief (GMAC) 1588 Timer Increment Register */
  #define REG_GMAC_EFTSL     (*(__I  uint32_t*)0x400501E0U) /**< \brief (GMAC) PTP Event Frame Transmitted Seconds Low Register */
  #define REG_GMAC_EFTN      (*(__I  uint32_t*)0x400501E4U) /**< \brief (GMAC) PTP Event Frame Transmitted Nanoseconds Register */
  #define REG_GMAC_EFRSL     (*(__I  uint32_t*)0x400501E8U) /**< \brief (GMAC) PTP Event Frame Received Seconds Low Register */
  #define REG_GMAC_EFRN      (*(__I  uint32_t*)0x400501ECU) /**< \brief (GMAC) PTP Event Frame Received Nanoseconds Register */
  #define REG_GMAC_PEFTSL    (*(__I  uint32_t*)0x400501F0U) /**< \brief (GMAC) PTP Peer Event Frame Transmitted Seconds Low Register */
  #define REG_GMAC_PEFTN     (*(__I  uint32_t*)0x400501F4U) /**< \brief (GMAC) PTP Peer Event Frame Transmitted Nanoseconds Register */
  #define REG_GMAC_PEFRSL    (*(__I  uint32_t*)0x400501F8U) /**< \brief (GMAC) PTP Peer Event Frame Received Seconds Low Register */
  #define REG_GMAC_PEFRN     (*(__I  uint32_t*)0x400501FCU) /**< \brief (GMAC) PTP Peer Event Frame Received Nanoseconds Register */
  #define REG_GMAC_RXLPI     (*(__I  uint32_t*)0x40050270U) /**< \brief (GMAC) Received LPI Transitions */
  #define REG_GMAC_RXLPITIME (*(__I  uint32_t*)0x40050274U) /**< \brief (GMAC) Received LPI Time */
  #define REG_GMAC_TXLPI     (*(__I  uint32_t*)0x40050278U) /**< \brief (GMAC) Transmit LPI Transitions */
  #define REG_GMAC_TXLPITIME (*(__I  uint32_t*)0x4005027CU) /**< \brief (GMAC) Transmit LPI Time */
  #define REG_GMAC_ISRPQ     (*(__I  uint32_t*)0x400503FCU) /**< \brief (GMAC) Interrupt Status Register Priority Queue (index = 1) 0 */
  #define REG_GMAC_TBQBAPQ   (*(__IO uint32_t*)0x4005043CU) /**< \brief (GMAC) Transmit Buffer Queue Base Address Register Priority Queue (index = 1) */
  #define REG_GMAC_RBQBAPQ   (*(__IO uint32_t*)0x4005047CU) /**< \brief (GMAC) Receive Buffer Queue Base Address Register Priority Queue (index = 1) */
  #define REG_GMAC_RBSRPQ    (*(__IO uint32_t*)0x4005049CU) /**< \brief (GMAC) Receive Buffer Size Register Priority Queue (index = 1) */
  #define REG_GMAC_CBSCR     (*(__IO uint32_t*)0x400504BCU) /**< \brief (GMAC) Credit-Based Shaping Control Register */
  #define REG_GMAC_CBSISQA   (*(__IO uint32_t*)0x400504C0U) /**< \brief (GMAC) Credit-Based Shaping IdleSlope Register for Queue A */
  #define REG_GMAC_CBSISQB   (*(__IO uint32_t*)0x400504C4U) /**< \brief (GMAC) Credit-Based Shaping IdleSlope Register for Queue B */
  #define REG_GMAC_ST1RPQ    (*(__IO uint32_t*)0x40050500U) /**< \brief (GMAC) Screening Type 1 Register Priority Queue (index = 0) */
  #define REG_GMAC_ST2RPQ    (*(__IO uint32_t*)0x40050540U) /**< \brief (GMAC) Screening Type 2 Register Priority Queue (index = 0) */
  #define REG_GMAC_IERPQ     (*(__O  uint32_t*)0x400505FCU) /**< \brief (GMAC) Interrupt Enable Register Priority Queue (index = 1) */
  #define REG_GMAC_IDRPQ     (*(__O  uint32_t*)0x4005061CU) /**< \brief (GMAC) Interrupt Disable Register Priority Queue (index = 1) */
  #define REG_GMAC_IMRPQ     (*(__IO uint32_t*)0x4005063CU) /**< \brief (GMAC) Interrupt Mask Register Priority Queue (index = 1) */
  #define REG_GMAC_ST2ER     (*(__IO uint32_t*)0x400506E0U) /**< \brief (GMAC) Screening Type 2 Ethertype Register (index = 0) */
  #define REG_GMAC_ST2CW00   (*(__IO uint32_t*)0x40050700U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 0) */
  #define REG_GMAC_ST2CW10   (*(__IO uint32_t*)0x40050704U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 0) */
  #define REG_GMAC_ST2CW01   (*(__IO uint32_t*)0x40050708U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 1) */
  #define REG_GMAC_ST2CW11   (*(__IO uint32_t*)0x4005070CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 1) */
  #define REG_GMAC_ST2CW02   (*(__IO uint32_t*)0x40050710U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 2) */
  #define REG_GMAC_ST2CW12   (*(__IO uint32_t*)0x40050714U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 2) */
  #define REG_GMAC_ST2CW03   (*(__IO uint32_t*)0x40050718U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 3) */
  #define REG_GMAC_ST2CW13   (*(__IO uint32_t*)0x4005071CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 3) */
  #define REG_GMAC_ST2CW04   (*(__IO uint32_t*)0x40050720U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 4) */
  #define REG_GMAC_ST2CW14   (*(__IO uint32_t*)0x40050724U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 4) */
  #define REG_GMAC_ST2CW05   (*(__IO uint32_t*)0x40050728U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 5) */
  #define REG_GMAC_ST2CW15   (*(__IO uint32_t*)0x4005072CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 5) */
  #define REG_GMAC_ST2CW06   (*(__IO uint32_t*)0x40050730U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 6) */
  #define REG_GMAC_ST2CW16   (*(__IO uint32_t*)0x40050734U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 6) */
  #define REG_GMAC_ST2CW07   (*(__IO uint32_t*)0x40050738U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 7) */
  #define REG_GMAC_ST2CW17   (*(__IO uint32_t*)0x4005073CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 7) */
  #define REG_GMAC_ST2CW08   (*(__IO uint32_t*)0x40050740U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 8) */
  #define REG_GMAC_ST2CW18   (*(__IO uint32_t*)0x40050744U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 8) */
  #define REG_GMAC_ST2CW09   (*(__IO uint32_t*)0x40050748U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 9) */
  #define REG_GMAC_ST2CW19   (*(__IO uint32_t*)0x4005074CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 9) */
  #define REG_GMAC_ST2CW010  (*(__IO uint32_t*)0x40050750U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 10) */
  #define REG_GMAC_ST2CW110  (*(__IO uint32_t*)0x40050754U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 10) */
  #define REG_GMAC_ST2CW011  (*(__IO uint32_t*)0x40050758U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 11) */
  #define REG_GMAC_ST2CW111  (*(__IO uint32_t*)0x4005075CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 11) */
  #define REG_GMAC_ST2CW012  (*(__IO uint32_t*)0x40050760U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 12) */
  #define REG_GMAC_ST2CW112  (*(__IO uint32_t*)0x40050764U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 12) */
  #define REG_GMAC_ST2CW013  (*(__IO uint32_t*)0x40050768U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 13) */
  #define REG_GMAC_ST2CW113  (*(__IO uint32_t*)0x4005076CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 13) */
  #define REG_GMAC_ST2CW014  (*(__IO uint32_t*)0x40050770U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 14) */
  #define REG_GMAC_ST2CW114  (*(__IO uint32_t*)0x40050774U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 14) */
  #define REG_GMAC_ST2CW015  (*(__IO uint32_t*)0x40050778U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 15) */
  #define REG_GMAC_ST2CW115  (*(__IO uint32_t*)0x4005077CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 15) */
  #define REG_GMAC_ST2CW016  (*(__IO uint32_t*)0x40050780U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 16) */
  #define REG_GMAC_ST2CW116  (*(__IO uint32_t*)0x40050784U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 16) */
  #define REG_GMAC_ST2CW017  (*(__IO uint32_t*)0x40050788U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 17) */
  #define REG_GMAC_ST2CW117  (*(__IO uint32_t*)0x4005078CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 17) */
  #define REG_GMAC_ST2CW018  (*(__IO uint32_t*)0x40050790U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 18) */
  #define REG_GMAC_ST2CW118  (*(__IO uint32_t*)0x40050794U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 18) */
  #define REG_GMAC_ST2CW019  (*(__IO uint32_t*)0x40050798U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 19) */
  #define REG_GMAC_ST2CW119  (*(__IO uint32_t*)0x4005079CU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 19) */
  #define REG_GMAC_ST2CW020  (*(__IO uint32_t*)0x400507A0U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 20) */
  #define REG_GMAC_ST2CW120  (*(__IO uint32_t*)0x400507A4U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 20) */
  #define REG_GMAC_ST2CW021  (*(__IO uint32_t*)0x400507A8U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 21) */
  #define REG_GMAC_ST2CW121  (*(__IO uint32_t*)0x400507ACU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 21) */
  #define REG_GMAC_ST2CW022  (*(__IO uint32_t*)0x400507B0U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 22) */
  #define REG_GMAC_ST2CW122  (*(__IO uint32_t*)0x400507B4U) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 22) */
  #define REG_GMAC_ST2CW023  (*(__IO uint32_t*)0x400507B8U) /**< \brief (GMAC) Screening Type 2 Compare Word 0 Register (index = 23) */
  #define REG_GMAC_ST2CW123  (*(__IO uint32_t*)0x400507BCU) /**< \brief (GMAC) Screening Type 2 Compare Word 1 Register (index = 23) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_GMAC_INSTANCE_ */
