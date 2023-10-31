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

#ifndef _SAME70_GMAC_COMPONENT_
#define _SAME70_GMAC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Gigabit Ethernet MAC */
/* ============================================================================= */
/** \addtogroup SAME70_GMAC Gigabit Ethernet MAC */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief GmacSa hardware registers */
typedef struct {
  __IO uint32_t GMAC_SAB; /**< \brief (GmacSa Offset: 0x0) Specific Address 1 Bottom Register */
  __IO uint32_t GMAC_SAT; /**< \brief (GmacSa Offset: 0x4) Specific Address 1 Top Register */
} GmacSa;
/** \brief Gmac hardware registers */
#define GMACSA_NUMBER 4
typedef struct {
  __IO uint32_t GMAC_NCR;               /**< \brief (Gmac Offset: 0x000) Network Control Register */
  __IO uint32_t GMAC_NCFGR;             /**< \brief (Gmac Offset: 0x004) Network Configuration Register */
  __I  uint32_t GMAC_NSR;               /**< \brief (Gmac Offset: 0x008) Network Status Register */
  __IO uint32_t GMAC_UR;                /**< \brief (Gmac Offset: 0x00C) User Register */
  __IO uint32_t GMAC_DCFGR;             /**< \brief (Gmac Offset: 0x010) DMA Configuration Register */
  __IO uint32_t GMAC_TSR;               /**< \brief (Gmac Offset: 0x014) Transmit Status Register */
  __IO uint32_t GMAC_RBQB;              /**< \brief (Gmac Offset: 0x018) Receive Buffer Queue Base Address Register */
  __IO uint32_t GMAC_TBQB;              /**< \brief (Gmac Offset: 0x01C) Transmit Buffer Queue Base Address Register */
  __IO uint32_t GMAC_RSR;               /**< \brief (Gmac Offset: 0x020) Receive Status Register */
  __I  uint32_t GMAC_ISR;               /**< \brief (Gmac Offset: 0x024) Interrupt Status Register */
  __O  uint32_t GMAC_IER;               /**< \brief (Gmac Offset: 0x028) Interrupt Enable Register */
  __O  uint32_t GMAC_IDR;               /**< \brief (Gmac Offset: 0x02C) Interrupt Disable Register */
  __IO uint32_t GMAC_IMR;               /**< \brief (Gmac Offset: 0x030) Interrupt Mask Register */
  __IO uint32_t GMAC_MAN;               /**< \brief (Gmac Offset: 0x034) PHY Maintenance Register */
  __I  uint32_t GMAC_RPQ;               /**< \brief (Gmac Offset: 0x038) Received Pause Quantum Register */
  __IO uint32_t GMAC_TPQ;               /**< \brief (Gmac Offset: 0x03C) Transmit Pause Quantum Register */
  __IO uint32_t GMAC_TPSF;              /**< \brief (Gmac Offset: 0x040) TX Partial Store and Forward Register */
  __IO uint32_t GMAC_RPSF;              /**< \brief (Gmac Offset: 0x044) RX Partial Store and Forward Register */
  __IO uint32_t GMAC_RJFML;             /**< \brief (Gmac Offset: 0x048) RX Jumbo Frame Max Length Register */
  __I  uint32_t Reserved1[13];
  __IO uint32_t GMAC_HRB;               /**< \brief (Gmac Offset: 0x080) Hash Register Bottom */
  __IO uint32_t GMAC_HRT;               /**< \brief (Gmac Offset: 0x084) Hash Register Top */
       GmacSa   GMAC_SA[GMACSA_NUMBER]; /**< \brief (Gmac Offset: 0x088) 1 .. 4 */
  __IO uint32_t GMAC_TIDM1;             /**< \brief (Gmac Offset: 0x0A8) Type ID Match 1 Register */
  __IO uint32_t GMAC_TIDM2;             /**< \brief (Gmac Offset: 0x0AC) Type ID Match 2 Register */
  __IO uint32_t GMAC_TIDM3;             /**< \brief (Gmac Offset: 0x0B0) Type ID Match 3 Register */
  __IO uint32_t GMAC_TIDM4;             /**< \brief (Gmac Offset: 0x0B4) Type ID Match 4 Register */
  __IO uint32_t GMAC_WOL;               /**< \brief (Gmac Offset: 0x0B8) Wake on LAN Register */
  __IO uint32_t GMAC_IPGS;              /**< \brief (Gmac Offset: 0x0BC) IPG Stretch Register */
  __IO uint32_t GMAC_SVLAN;             /**< \brief (Gmac Offset: 0x0C0) Stacked VLAN Register */
  __IO uint32_t GMAC_TPFCP;             /**< \brief (Gmac Offset: 0x0C4) Transmit PFC Pause Register */
  __IO uint32_t GMAC_SAMB1;             /**< \brief (Gmac Offset: 0x0C8) Specific Address 1 Mask Bottom Register */
  __IO uint32_t GMAC_SAMT1;             /**< \brief (Gmac Offset: 0x0CC) Specific Address 1 Mask Top Register */
  __I  uint32_t Reserved2[3];
  __IO uint32_t GMAC_NSC;               /**< \brief (Gmac Offset: 0x0DC) 1588 Timer Nanosecond Comparison Register */
  __IO uint32_t GMAC_SCL;               /**< \brief (Gmac Offset: 0x0E0) 1588 Timer Second Comparison Low Register */
  __IO uint32_t GMAC_SCH;               /**< \brief (Gmac Offset: 0x0E4) 1588 Timer Second Comparison High Register */
  __I  uint32_t GMAC_EFTSH;             /**< \brief (Gmac Offset: 0x0E8) PTP Event Frame Transmitted Seconds High Register */
  __I  uint32_t GMAC_EFRSH;             /**< \brief (Gmac Offset: 0x0EC) PTP Event Frame Received Seconds High Register */
  __I  uint32_t GMAC_PEFTSH;            /**< \brief (Gmac Offset: 0x0F0) PTP Peer Event Frame Transmitted Seconds High Register */
  __I  uint32_t GMAC_PEFRSH;            /**< \brief (Gmac Offset: 0x0F4) PTP Peer Event Frame Received Seconds High Register */
  __I  uint32_t Reserved3[1];
  __I  uint32_t GMAC_MID;               /**< \brief (Gmac Offset: 0x0FC) Module ID Register */
  __I  uint32_t GMAC_OTLO;              /**< \brief (Gmac Offset: 0x100) Octets Transmitted Low Register */
  __I  uint32_t GMAC_OTHI;              /**< \brief (Gmac Offset: 0x104) Octets Transmitted High Register */
  __I  uint32_t GMAC_FT;                /**< \brief (Gmac Offset: 0x108) Frames Transmitted Register */
  __I  uint32_t GMAC_BCFT;              /**< \brief (Gmac Offset: 0x10C) Broadcast Frames Transmitted Register */
  __I  uint32_t GMAC_MFT;               /**< \brief (Gmac Offset: 0x110) Multicast Frames Transmitted Register */
  __I  uint32_t GMAC_PFT;               /**< \brief (Gmac Offset: 0x114) Pause Frames Transmitted Register */
  __I  uint32_t GMAC_BFT64;             /**< \brief (Gmac Offset: 0x118) 64 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_TBFT127;           /**< \brief (Gmac Offset: 0x11C) 65 to 127 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_TBFT255;           /**< \brief (Gmac Offset: 0x120) 128 to 255 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_TBFT511;           /**< \brief (Gmac Offset: 0x124) 256 to 511 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_TBFT1023;          /**< \brief (Gmac Offset: 0x128) 512 to 1023 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_TBFT1518;          /**< \brief (Gmac Offset: 0x12C) 1024 to 1518 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_GTBFT1518;         /**< \brief (Gmac Offset: 0x130) Greater Than 1518 Byte Frames Transmitted Register */
  __I  uint32_t GMAC_TUR;               /**< \brief (Gmac Offset: 0x134) Transmit Underruns Register */
  __I  uint32_t GMAC_SCF;               /**< \brief (Gmac Offset: 0x138) Single Collision Frames Register */
  __I  uint32_t GMAC_MCF;               /**< \brief (Gmac Offset: 0x13C) Multiple Collision Frames Register */
  __I  uint32_t GMAC_EC;                /**< \brief (Gmac Offset: 0x140) Excessive Collisions Register */
  __I  uint32_t GMAC_LC;                /**< \brief (Gmac Offset: 0x144) Late Collisions Register */
  __I  uint32_t GMAC_DTF;               /**< \brief (Gmac Offset: 0x148) Deferred Transmission Frames Register */
  __I  uint32_t GMAC_CSE;               /**< \brief (Gmac Offset: 0x14C) Carrier Sense Errors Register */
  __I  uint32_t GMAC_ORLO;              /**< \brief (Gmac Offset: 0x150) Octets Received Low Received Register */
  __I  uint32_t GMAC_ORHI;              /**< \brief (Gmac Offset: 0x154) Octets Received High Received Register */
  __I  uint32_t GMAC_FR;                /**< \brief (Gmac Offset: 0x158) Frames Received Register */
  __I  uint32_t GMAC_BCFR;              /**< \brief (Gmac Offset: 0x15C) Broadcast Frames Received Register */
  __I  uint32_t GMAC_MFR;               /**< \brief (Gmac Offset: 0x160) Multicast Frames Received Register */
  __I  uint32_t GMAC_PFR;               /**< \brief (Gmac Offset: 0x164) Pause Frames Received Register */
  __I  uint32_t GMAC_BFR64;             /**< \brief (Gmac Offset: 0x168) 64 Byte Frames Received Register */
  __I  uint32_t GMAC_TBFR127;           /**< \brief (Gmac Offset: 0x16C) 65 to 127 Byte Frames Received Register */
  __I  uint32_t GMAC_TBFR255;           /**< \brief (Gmac Offset: 0x170) 128 to 255 Byte Frames Received Register */
  __I  uint32_t GMAC_TBFR511;           /**< \brief (Gmac Offset: 0x174) 256 to 511 Byte Frames Received Register */
  __I  uint32_t GMAC_TBFR1023;          /**< \brief (Gmac Offset: 0x178) 512 to 1023 Byte Frames Received Register */
  __I  uint32_t GMAC_TBFR1518;          /**< \brief (Gmac Offset: 0x17C) 1024 to 1518 Byte Frames Received Register */
  __I  uint32_t GMAC_TMXBFR;            /**< \brief (Gmac Offset: 0x180) 1519 to Maximum Byte Frames Received Register */
  __I  uint32_t GMAC_UFR;               /**< \brief (Gmac Offset: 0x184) Undersize Frames Received Register */
  __I  uint32_t GMAC_OFR;               /**< \brief (Gmac Offset: 0x188) Oversize Frames Received Register */
  __I  uint32_t GMAC_JR;                /**< \brief (Gmac Offset: 0x18C) Jabbers Received Register */
  __I  uint32_t GMAC_FCSE;              /**< \brief (Gmac Offset: 0x190) Frame Check Sequence Errors Register */
  __I  uint32_t GMAC_LFFE;              /**< \brief (Gmac Offset: 0x194) Length Field Frame Errors Register */
  __I  uint32_t GMAC_RSE;               /**< \brief (Gmac Offset: 0x198) Receive Symbol Errors Register */
  __I  uint32_t GMAC_AE;                /**< \brief (Gmac Offset: 0x19C) Alignment Errors Register */
  __I  uint32_t GMAC_RRE;               /**< \brief (Gmac Offset: 0x1A0) Receive Resource Errors Register */
  __I  uint32_t GMAC_ROE;               /**< \brief (Gmac Offset: 0x1A4) Receive Overrun Register */
  __I  uint32_t GMAC_IHCE;              /**< \brief (Gmac Offset: 0x1A8) IP Header Checksum Errors Register */
  __I  uint32_t GMAC_TCE;               /**< \brief (Gmac Offset: 0x1AC) TCP Checksum Errors Register */
  __I  uint32_t GMAC_UCE;               /**< \brief (Gmac Offset: 0x1B0) UDP Checksum Errors Register */
  __I  uint32_t Reserved4[2];
  __IO uint32_t GMAC_TISUBN;            /**< \brief (Gmac Offset: 0x1BC) 1588 Timer Increment Sub-nanoseconds Register */
  __IO uint32_t GMAC_TSH;               /**< \brief (Gmac Offset: 0x1C0) 1588 Timer Seconds High Register */
  __I  uint32_t Reserved5[3];
  __IO uint32_t GMAC_TSL;               /**< \brief (Gmac Offset: 0x1D0) 1588 Timer Seconds Low Register */
  __IO uint32_t GMAC_TN;                /**< \brief (Gmac Offset: 0x1D4) 1588 Timer Nanoseconds Register */
  __O  uint32_t GMAC_TA;                /**< \brief (Gmac Offset: 0x1D8) 1588 Timer Adjust Register */
  __IO uint32_t GMAC_TI;                /**< \brief (Gmac Offset: 0x1DC) 1588 Timer Increment Register */
  __I  uint32_t GMAC_EFTSL;             /**< \brief (Gmac Offset: 0x1E0) PTP Event Frame Transmitted Seconds Low Register */
  __I  uint32_t GMAC_EFTN;              /**< \brief (Gmac Offset: 0x1E4) PTP Event Frame Transmitted Nanoseconds Register */
  __I  uint32_t GMAC_EFRSL;             /**< \brief (Gmac Offset: 0x1E8) PTP Event Frame Received Seconds Low Register */
  __I  uint32_t GMAC_EFRN;              /**< \brief (Gmac Offset: 0x1EC) PTP Event Frame Received Nanoseconds Register */
  __I  uint32_t GMAC_PEFTSL;            /**< \brief (Gmac Offset: 0x1F0) PTP Peer Event Frame Transmitted Seconds Low Register */
  __I  uint32_t GMAC_PEFTN;             /**< \brief (Gmac Offset: 0x1F4) PTP Peer Event Frame Transmitted Nanoseconds Register */
  __I  uint32_t GMAC_PEFRSL;            /**< \brief (Gmac Offset: 0x1F8) PTP Peer Event Frame Received Seconds Low Register */
  __I  uint32_t GMAC_PEFRN;             /**< \brief (Gmac Offset: 0x1FC) PTP Peer Event Frame Received Nanoseconds Register */
  __I  uint32_t Reserved6[28];
  __I  uint32_t GMAC_RXLPI;             /**< \brief (Gmac Offset: 0x270) Received LPI Transitions */
  __I  uint32_t GMAC_RXLPITIME;         /**< \brief (Gmac Offset: 0x274) Received LPI Time */
  __I  uint32_t GMAC_TXLPI;             /**< \brief (Gmac Offset: 0x278) Transmit LPI Transitions */
  __I  uint32_t GMAC_TXLPITIME;         /**< \brief (Gmac Offset: 0x27C) Transmit LPI Time */
  __I  uint32_t Reserved7[96];
  __I  uint32_t GMAC_ISRPQ[5];          /**< \brief (Gmac Offset: 0x400) Interrupt Status Register Priority Queue (index = 1) */
  __I  uint32_t Reserved8[11];
  __IO uint32_t GMAC_TBQBAPQ[5];        /**< \brief (Gmac Offset: 0x440) Transmit Buffer Queue Base Address Register Priority Queue (index = 1) */
  __I  uint32_t Reserved9[11];
  __IO uint32_t GMAC_RBQBAPQ[5];        /**< \brief (Gmac Offset: 0x480) Receive Buffer Queue Base Address Register Priority Queue (index = 1) */
  __I  uint32_t Reserved10[3];
  __IO uint32_t GMAC_RBSRPQ[5];         /**< \brief (Gmac Offset: 0x4A0) Receive Buffer Size Register Priority Queue (index = 1) */
  __I  uint32_t Reserved11[2];
  __IO uint32_t GMAC_CBSCR;             /**< \brief (Gmac Offset: 0x4BC) Credit-Based Shaping Control Register */
  __IO uint32_t GMAC_CBSISQA;           /**< \brief (Gmac Offset: 0x4C0) Credit-Based Shaping IdleSlope Register for Queue A */
  __IO uint32_t GMAC_CBSISQB;           /**< \brief (Gmac Offset: 0x4C4) Credit-Based Shaping IdleSlope Register for Queue B */
  __I  uint32_t Reserved12[14];
  __IO uint32_t GMAC_ST1RPQ[4];         /**< \brief (Gmac Offset: 0x500) Screening Type 1 Register Priority Queue (index = 0) */
  __I  uint32_t Reserved13[12];
  __IO uint32_t GMAC_ST2RPQ[8];         /**< \brief (Gmac Offset: 0x540) Screening Type 2 Register Priority Queue (index = 0) */
  __I  uint32_t Reserved14[39];
  __O  uint32_t GMAC_IERPQ[5];          /**< \brief (Gmac Offset: 0x5FC) Interrupt Enable Register Priority Queue (index = 1) */
  __I  uint32_t Reserved15[3];
  __O  uint32_t GMAC_IDRPQ[5];          /**< \brief (Gmac Offset: 0x61C) Interrupt Disable Register Priority Queue (index = 1) */
  __I  uint32_t Reserved16[3];
  __IO uint32_t GMAC_IMRPQ[5];          /**< \brief (Gmac Offset: 0x63C) Interrupt Mask Register Priority Queue (index = 1) */
  __I  uint32_t Reserved17[36];
  __IO uint32_t GMAC_ST2ER[4];          /**< \brief (Gmac Offset: 0x6E0) Screening Type 2 Ethertype Register (index = 0) */
  __I  uint32_t Reserved18[4];
  __IO uint32_t GMAC_ST2CW00;           /**< \brief (Gmac Offset: 0x700) Screening Type 2 Compare Word 0 Register (index = 0) */
  __IO uint32_t GMAC_ST2CW10;           /**< \brief (Gmac Offset: 0x704) Screening Type 2 Compare Word 1 Register (index = 0) */
  __IO uint32_t GMAC_ST2CW01;           /**< \brief (Gmac Offset: 0x708) Screening Type 2 Compare Word 0 Register (index = 1) */
  __IO uint32_t GMAC_ST2CW11;           /**< \brief (Gmac Offset: 0x70C) Screening Type 2 Compare Word 1 Register (index = 1) */
  __IO uint32_t GMAC_ST2CW02;           /**< \brief (Gmac Offset: 0x710) Screening Type 2 Compare Word 0 Register (index = 2) */
  __IO uint32_t GMAC_ST2CW12;           /**< \brief (Gmac Offset: 0x714) Screening Type 2 Compare Word 1 Register (index = 2) */
  __IO uint32_t GMAC_ST2CW03;           /**< \brief (Gmac Offset: 0x718) Screening Type 2 Compare Word 0 Register (index = 3) */
  __IO uint32_t GMAC_ST2CW13;           /**< \brief (Gmac Offset: 0x71C) Screening Type 2 Compare Word 1 Register (index = 3) */
  __IO uint32_t GMAC_ST2CW04;           /**< \brief (Gmac Offset: 0x720) Screening Type 2 Compare Word 0 Register (index = 4) */
  __IO uint32_t GMAC_ST2CW14;           /**< \brief (Gmac Offset: 0x724) Screening Type 2 Compare Word 1 Register (index = 4) */
  __IO uint32_t GMAC_ST2CW05;           /**< \brief (Gmac Offset: 0x728) Screening Type 2 Compare Word 0 Register (index = 5) */
  __IO uint32_t GMAC_ST2CW15;           /**< \brief (Gmac Offset: 0x72C) Screening Type 2 Compare Word 1 Register (index = 5) */
  __IO uint32_t GMAC_ST2CW06;           /**< \brief (Gmac Offset: 0x730) Screening Type 2 Compare Word 0 Register (index = 6) */
  __IO uint32_t GMAC_ST2CW16;           /**< \brief (Gmac Offset: 0x734) Screening Type 2 Compare Word 1 Register (index = 6) */
  __IO uint32_t GMAC_ST2CW07;           /**< \brief (Gmac Offset: 0x738) Screening Type 2 Compare Word 0 Register (index = 7) */
  __IO uint32_t GMAC_ST2CW17;           /**< \brief (Gmac Offset: 0x73C) Screening Type 2 Compare Word 1 Register (index = 7) */
  __IO uint32_t GMAC_ST2CW08;           /**< \brief (Gmac Offset: 0x740) Screening Type 2 Compare Word 0 Register (index = 8) */
  __IO uint32_t GMAC_ST2CW18;           /**< \brief (Gmac Offset: 0x744) Screening Type 2 Compare Word 1 Register (index = 8) */
  __IO uint32_t GMAC_ST2CW09;           /**< \brief (Gmac Offset: 0x748) Screening Type 2 Compare Word 0 Register (index = 9) */
  __IO uint32_t GMAC_ST2CW19;           /**< \brief (Gmac Offset: 0x74C) Screening Type 2 Compare Word 1 Register (index = 9) */
  __IO uint32_t GMAC_ST2CW010;          /**< \brief (Gmac Offset: 0x750) Screening Type 2 Compare Word 0 Register (index = 10) */
  __IO uint32_t GMAC_ST2CW110;          /**< \brief (Gmac Offset: 0x754) Screening Type 2 Compare Word 1 Register (index = 10) */
  __IO uint32_t GMAC_ST2CW011;          /**< \brief (Gmac Offset: 0x758) Screening Type 2 Compare Word 0 Register (index = 11) */
  __IO uint32_t GMAC_ST2CW111;          /**< \brief (Gmac Offset: 0x75C) Screening Type 2 Compare Word 1 Register (index = 11) */
  __IO uint32_t GMAC_ST2CW012;          /**< \brief (Gmac Offset: 0x760) Screening Type 2 Compare Word 0 Register (index = 12) */
  __IO uint32_t GMAC_ST2CW112;          /**< \brief (Gmac Offset: 0x764) Screening Type 2 Compare Word 1 Register (index = 12) */
  __IO uint32_t GMAC_ST2CW013;          /**< \brief (Gmac Offset: 0x768) Screening Type 2 Compare Word 0 Register (index = 13) */
  __IO uint32_t GMAC_ST2CW113;          /**< \brief (Gmac Offset: 0x76C) Screening Type 2 Compare Word 1 Register (index = 13) */
  __IO uint32_t GMAC_ST2CW014;          /**< \brief (Gmac Offset: 0x770) Screening Type 2 Compare Word 0 Register (index = 14) */
  __IO uint32_t GMAC_ST2CW114;          /**< \brief (Gmac Offset: 0x774) Screening Type 2 Compare Word 1 Register (index = 14) */
  __IO uint32_t GMAC_ST2CW015;          /**< \brief (Gmac Offset: 0x778) Screening Type 2 Compare Word 0 Register (index = 15) */
  __IO uint32_t GMAC_ST2CW115;          /**< \brief (Gmac Offset: 0x77C) Screening Type 2 Compare Word 1 Register (index = 15) */
  __IO uint32_t GMAC_ST2CW016;          /**< \brief (Gmac Offset: 0x780) Screening Type 2 Compare Word 0 Register (index = 16) */
  __IO uint32_t GMAC_ST2CW116;          /**< \brief (Gmac Offset: 0x784) Screening Type 2 Compare Word 1 Register (index = 16) */
  __IO uint32_t GMAC_ST2CW017;          /**< \brief (Gmac Offset: 0x788) Screening Type 2 Compare Word 0 Register (index = 17) */
  __IO uint32_t GMAC_ST2CW117;          /**< \brief (Gmac Offset: 0x78C) Screening Type 2 Compare Word 1 Register (index = 17) */
  __IO uint32_t GMAC_ST2CW018;          /**< \brief (Gmac Offset: 0x790) Screening Type 2 Compare Word 0 Register (index = 18) */
  __IO uint32_t GMAC_ST2CW118;          /**< \brief (Gmac Offset: 0x794) Screening Type 2 Compare Word 1 Register (index = 18) */
  __IO uint32_t GMAC_ST2CW019;          /**< \brief (Gmac Offset: 0x798) Screening Type 2 Compare Word 0 Register (index = 19) */
  __IO uint32_t GMAC_ST2CW119;          /**< \brief (Gmac Offset: 0x79C) Screening Type 2 Compare Word 1 Register (index = 19) */
  __IO uint32_t GMAC_ST2CW020;          /**< \brief (Gmac Offset: 0x7A0) Screening Type 2 Compare Word 0 Register (index = 20) */
  __IO uint32_t GMAC_ST2CW120;          /**< \brief (Gmac Offset: 0x7A4) Screening Type 2 Compare Word 1 Register (index = 20) */
  __IO uint32_t GMAC_ST2CW021;          /**< \brief (Gmac Offset: 0x7A8) Screening Type 2 Compare Word 0 Register (index = 21) */
  __IO uint32_t GMAC_ST2CW121;          /**< \brief (Gmac Offset: 0x7AC) Screening Type 2 Compare Word 1 Register (index = 21) */
  __IO uint32_t GMAC_ST2CW022;          /**< \brief (Gmac Offset: 0x7B0) Screening Type 2 Compare Word 0 Register (index = 22) */
  __IO uint32_t GMAC_ST2CW122;          /**< \brief (Gmac Offset: 0x7B4) Screening Type 2 Compare Word 1 Register (index = 22) */
  __IO uint32_t GMAC_ST2CW023;          /**< \brief (Gmac Offset: 0x7B8) Screening Type 2 Compare Word 0 Register (index = 23) */
  __IO uint32_t GMAC_ST2CW123;          /**< \brief (Gmac Offset: 0x7BC) Screening Type 2 Compare Word 1 Register (index = 23) */
} Gmac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- GMAC_NCR : (GMAC Offset: 0x000) Network Control Register -------- */
#define GMAC_NCR_LBL (0x1u << 1) /**< \brief (GMAC_NCR) Loop Back Local */
#define GMAC_NCR_RXEN (0x1u << 2) /**< \brief (GMAC_NCR) Receive Enable */
#define GMAC_NCR_TXEN (0x1u << 3) /**< \brief (GMAC_NCR) Transmit Enable */
#define GMAC_NCR_MPE (0x1u << 4) /**< \brief (GMAC_NCR) Management Port Enable */
#define GMAC_NCR_CLRSTAT (0x1u << 5) /**< \brief (GMAC_NCR) Clear Statistics Registers */
#define GMAC_NCR_INCSTAT (0x1u << 6) /**< \brief (GMAC_NCR) Increment Statistics Registers */
#define GMAC_NCR_WESTAT (0x1u << 7) /**< \brief (GMAC_NCR) Write Enable for Statistics Registers */
#define GMAC_NCR_BP (0x1u << 8) /**< \brief (GMAC_NCR) Back pressure */
#define GMAC_NCR_TSTART (0x1u << 9) /**< \brief (GMAC_NCR) Start Transmission */
#define GMAC_NCR_THALT (0x1u << 10) /**< \brief (GMAC_NCR) Transmit Halt */
#define GMAC_NCR_TXPF (0x1u << 11) /**< \brief (GMAC_NCR) Transmit Pause Frame */
#define GMAC_NCR_TXZQPF (0x1u << 12) /**< \brief (GMAC_NCR) Transmit Zero Quantum Pause Frame */
#define GMAC_NCR_SRTSM (0x1u << 15) /**< \brief (GMAC_NCR) Store Receive Time Stamp to Memory */
#define GMAC_NCR_ENPBPR (0x1u << 16) /**< \brief (GMAC_NCR) Enable PFC Priority-based Pause Reception */
#define GMAC_NCR_TXPBPF (0x1u << 17) /**< \brief (GMAC_NCR) Transmit PFC Priority-based Pause Frame */
#define GMAC_NCR_FNP (0x1u << 18) /**< \brief (GMAC_NCR) Flush Next Packet */
/* -------- GMAC_NCFGR : (GMAC Offset: 0x004) Network Configuration Register -------- */
#define GMAC_NCFGR_SPD (0x1u << 0) /**< \brief (GMAC_NCFGR) Speed */
#define GMAC_NCFGR_FD (0x1u << 1) /**< \brief (GMAC_NCFGR) Full Duplex */
#define GMAC_NCFGR_DNVLAN (0x1u << 2) /**< \brief (GMAC_NCFGR) Discard Non-VLAN FRAMES */
#define GMAC_NCFGR_JFRAME (0x1u << 3) /**< \brief (GMAC_NCFGR) Jumbo Frame Size */
#define GMAC_NCFGR_CAF (0x1u << 4) /**< \brief (GMAC_NCFGR) Copy All Frames */
#define GMAC_NCFGR_NBC (0x1u << 5) /**< \brief (GMAC_NCFGR) No Broadcast */
#define GMAC_NCFGR_MTIHEN (0x1u << 6) /**< \brief (GMAC_NCFGR) Multicast Hash Enable */
#define GMAC_NCFGR_UNIHEN (0x1u << 7) /**< \brief (GMAC_NCFGR) Unicast Hash Enable */
#define GMAC_NCFGR_MAXFS (0x1u << 8) /**< \brief (GMAC_NCFGR) 1536 Maximum Frame Size */
#define GMAC_NCFGR_RTY (0x1u << 12) /**< \brief (GMAC_NCFGR) Retry Test */
#define GMAC_NCFGR_PEN (0x1u << 13) /**< \brief (GMAC_NCFGR) Pause Enable */
#define GMAC_NCFGR_RXBUFO_Pos 14
#define GMAC_NCFGR_RXBUFO_Msk (0x3u << GMAC_NCFGR_RXBUFO_Pos) /**< \brief (GMAC_NCFGR) Receive Buffer Offset */
#define GMAC_NCFGR_RXBUFO(value) ((GMAC_NCFGR_RXBUFO_Msk & ((value) << GMAC_NCFGR_RXBUFO_Pos)))
#define GMAC_NCFGR_LFERD (0x1u << 16) /**< \brief (GMAC_NCFGR) Length Field Error Frame Discard */
#define GMAC_NCFGR_RFCS (0x1u << 17) /**< \brief (GMAC_NCFGR) Remove FCS */
#define GMAC_NCFGR_CLK_Pos 18
#define GMAC_NCFGR_CLK_Msk (0x7u << GMAC_NCFGR_CLK_Pos) /**< \brief (GMAC_NCFGR) MDC CLock Division */
#define GMAC_NCFGR_CLK(value) ((GMAC_NCFGR_CLK_Msk & ((value) << GMAC_NCFGR_CLK_Pos)))
#define   GMAC_NCFGR_CLK_MCK_8 (0x0u << 18) /**< \brief (GMAC_NCFGR) MCK divided by 8 (MCK up to 20 MHz) */
#define   GMAC_NCFGR_CLK_MCK_16 (0x1u << 18) /**< \brief (GMAC_NCFGR) MCK divided by 16 (MCK up to 40 MHz) */
#define   GMAC_NCFGR_CLK_MCK_32 (0x2u << 18) /**< \brief (GMAC_NCFGR) MCK divided by 32 (MCK up to 80 MHz) */
#define   GMAC_NCFGR_CLK_MCK_48 (0x3u << 18) /**< \brief (GMAC_NCFGR) MCK divided by 48 (MCK up to 120 MHz) */
#define   GMAC_NCFGR_CLK_MCK_64 (0x4u << 18) /**< \brief (GMAC_NCFGR) MCK divided by 64 (MCK up to 160 MHz) */
#define   GMAC_NCFGR_CLK_MCK_96 (0x5u << 18) /**< \brief (GMAC_NCFGR) MCK divided by 96 (MCK up to 240 MHz) */
#define GMAC_NCFGR_DBW_Pos 21
#define GMAC_NCFGR_DBW_Msk (0x3u << GMAC_NCFGR_DBW_Pos) /**< \brief (GMAC_NCFGR) Data Bus Width */
#define GMAC_NCFGR_DBW(value) ((GMAC_NCFGR_DBW_Msk & ((value) << GMAC_NCFGR_DBW_Pos)))
#define GMAC_NCFGR_DCPF (0x1u << 23) /**< \brief (GMAC_NCFGR) Disable Copy of Pause Frames */
#define GMAC_NCFGR_RXCOEN (0x1u << 24) /**< \brief (GMAC_NCFGR) Receive Checksum Offload Enable */
#define GMAC_NCFGR_EFRHD (0x1u << 25) /**< \brief (GMAC_NCFGR) Enable Frames Received in Half Duplex */
#define GMAC_NCFGR_IRXFCS (0x1u << 26) /**< \brief (GMAC_NCFGR) Ignore RX FCS */
#define GMAC_NCFGR_IPGSEN (0x1u << 28) /**< \brief (GMAC_NCFGR) IP Stretch Enable */
#define GMAC_NCFGR_RXBP (0x1u << 29) /**< \brief (GMAC_NCFGR) Receive Bad Preamble */
#define GMAC_NCFGR_IRXER (0x1u << 30) /**< \brief (GMAC_NCFGR) Ignore IPG GRXER */
/* -------- GMAC_NSR : (GMAC Offset: 0x008) Network Status Register -------- */
#define GMAC_NSR_MDIO (0x1u << 1) /**< \brief (GMAC_NSR) MDIO Input Status */
#define GMAC_NSR_IDLE (0x1u << 2) /**< \brief (GMAC_NSR) PHY Management Logic Idle */
/* -------- GMAC_UR : (GMAC Offset: 0x00C) User Register -------- */
#define GMAC_UR_RMII (0x1u << 0) /**< \brief (GMAC_UR) Reduced MII Mode */
/* -------- GMAC_DCFGR : (GMAC Offset: 0x010) DMA Configuration Register -------- */
#define GMAC_DCFGR_FBLDO_Pos 0
#define GMAC_DCFGR_FBLDO_Msk (0x1fu << GMAC_DCFGR_FBLDO_Pos) /**< \brief (GMAC_DCFGR) Fixed Burst Length for DMA Data Operations: */
#define GMAC_DCFGR_FBLDO(value) ((GMAC_DCFGR_FBLDO_Msk & ((value) << GMAC_DCFGR_FBLDO_Pos)))
#define   GMAC_DCFGR_FBLDO_SINGLE (0x1u << 0) /**< \brief (GMAC_DCFGR) 00001: Always use SINGLE AHB bursts */
#define   GMAC_DCFGR_FBLDO_INCR4 (0x4u << 0) /**< \brief (GMAC_DCFGR) 001xx: Attempt to use INCR4 AHB bursts (Default) */
#define   GMAC_DCFGR_FBLDO_INCR8 (0x8u << 0) /**< \brief (GMAC_DCFGR) 01xxx: Attempt to use INCR8 AHB bursts */
#define   GMAC_DCFGR_FBLDO_INCR16 (0x10u << 0) /**< \brief (GMAC_DCFGR) 1xxxx: Attempt to use INCR16 AHB bursts */
#define GMAC_DCFGR_ESMA (0x1u << 6) /**< \brief (GMAC_DCFGR) Endian Swap Mode Enable for Management Descriptor Accesses */
#define GMAC_DCFGR_ESPA (0x1u << 7) /**< \brief (GMAC_DCFGR) Endian Swap Mode Enable for Packet Data Accesses */
#define GMAC_DCFGR_RXBMS_Pos 8
#define GMAC_DCFGR_RXBMS_Msk (0x3u << GMAC_DCFGR_RXBMS_Pos) /**< \brief (GMAC_DCFGR) Receiver Packet Buffer Memory Size Select */
#define GMAC_DCFGR_RXBMS(value) ((GMAC_DCFGR_RXBMS_Msk & ((value) << GMAC_DCFGR_RXBMS_Pos)))
#define   GMAC_DCFGR_RXBMS_EIGHTH (0x0u << 8) /**< \brief (GMAC_DCFGR) 4/8 Kbyte Memory Size */
#define   GMAC_DCFGR_RXBMS_QUARTER (0x1u << 8) /**< \brief (GMAC_DCFGR) 4/4 Kbytes Memory Size */
#define   GMAC_DCFGR_RXBMS_HALF (0x2u << 8) /**< \brief (GMAC_DCFGR) 4/2 Kbytes Memory Size */
#define   GMAC_DCFGR_RXBMS_FULL (0x3u << 8) /**< \brief (GMAC_DCFGR) 4 Kbytes Memory Size */
#define GMAC_DCFGR_TXPBMS (0x1u << 10) /**< \brief (GMAC_DCFGR) Transmitter Packet Buffer Memory Size Select */
#define GMAC_DCFGR_TXCOEN (0x1u << 11) /**< \brief (GMAC_DCFGR) Transmitter Checksum Generation Offload Enable */
#define GMAC_DCFGR_DRBS_Pos 16
#define GMAC_DCFGR_DRBS_Msk (0xffu << GMAC_DCFGR_DRBS_Pos) /**< \brief (GMAC_DCFGR) DMA Receive Buffer Size */
#define GMAC_DCFGR_DRBS(value) ((GMAC_DCFGR_DRBS_Msk & ((value) << GMAC_DCFGR_DRBS_Pos)))
#define GMAC_DCFGR_DDRP (0x1u << 24) /**< \brief (GMAC_DCFGR) DMA Discard Receive Packets */
/* -------- GMAC_TSR : (GMAC Offset: 0x014) Transmit Status Register -------- */
#define GMAC_TSR_UBR (0x1u << 0) /**< \brief (GMAC_TSR) Used Bit Read */
#define GMAC_TSR_COL (0x1u << 1) /**< \brief (GMAC_TSR) Collision Occurred */
#define GMAC_TSR_RLE (0x1u << 2) /**< \brief (GMAC_TSR) Retry Limit Exceeded */
#define GMAC_TSR_TXGO (0x1u << 3) /**< \brief (GMAC_TSR) Transmit Go */
#define GMAC_TSR_TFC (0x1u << 4) /**< \brief (GMAC_TSR) Transmit Frame Corruption Due to AHB Error */
#define GMAC_TSR_TXCOMP (0x1u << 5) /**< \brief (GMAC_TSR) Transmit Complete */
#define GMAC_TSR_HRESP (0x1u << 8) /**< \brief (GMAC_TSR) HRESP Not OK */
/* -------- GMAC_RBQB : (GMAC Offset: 0x018) Receive Buffer Queue Base Address Register -------- */
#define GMAC_RBQB_ADDR_Pos 2
#define GMAC_RBQB_ADDR_Msk (0x3fffffffu << GMAC_RBQB_ADDR_Pos) /**< \brief (GMAC_RBQB) Receive Buffer Queue Base Address */
#define GMAC_RBQB_ADDR(value) ((GMAC_RBQB_ADDR_Msk & ((value) << GMAC_RBQB_ADDR_Pos)))
/* -------- GMAC_TBQB : (GMAC Offset: 0x01C) Transmit Buffer Queue Base Address Register -------- */
#define GMAC_TBQB_ADDR_Pos 2
#define GMAC_TBQB_ADDR_Msk (0x3fffffffu << GMAC_TBQB_ADDR_Pos) /**< \brief (GMAC_TBQB) Transmit Buffer Queue Base Address */
#define GMAC_TBQB_ADDR(value) ((GMAC_TBQB_ADDR_Msk & ((value) << GMAC_TBQB_ADDR_Pos)))
/* -------- GMAC_RSR : (GMAC Offset: 0x020) Receive Status Register -------- */
#define GMAC_RSR_BNA (0x1u << 0) /**< \brief (GMAC_RSR) Buffer Not Available */
#define GMAC_RSR_REC (0x1u << 1) /**< \brief (GMAC_RSR) Frame Received */
#define GMAC_RSR_RXOVR (0x1u << 2) /**< \brief (GMAC_RSR) Receive Overrun */
#define GMAC_RSR_HNO (0x1u << 3) /**< \brief (GMAC_RSR) HRESP Not OK */
/* -------- GMAC_ISR : (GMAC Offset: 0x024) Interrupt Status Register -------- */
#define GMAC_ISR_MFS (0x1u << 0) /**< \brief (GMAC_ISR) Management Frame Sent */
#define GMAC_ISR_RCOMP (0x1u << 1) /**< \brief (GMAC_ISR) Receive Complete */
#define GMAC_ISR_RXUBR (0x1u << 2) /**< \brief (GMAC_ISR) RX Used Bit Read */
#define GMAC_ISR_TXUBR (0x1u << 3) /**< \brief (GMAC_ISR) TX Used Bit Read */
#define GMAC_ISR_TUR (0x1u << 4) /**< \brief (GMAC_ISR) Transmit Underrun */
#define GMAC_ISR_RLEX (0x1u << 5) /**< \brief (GMAC_ISR) Retry Limit Exceeded */
#define GMAC_ISR_TFC (0x1u << 6) /**< \brief (GMAC_ISR) Transmit Frame Corruption Due to AHB Error */
#define GMAC_ISR_TCOMP (0x1u << 7) /**< \brief (GMAC_ISR) Transmit Complete */
#define GMAC_ISR_ROVR (0x1u << 10) /**< \brief (GMAC_ISR) Receive Overrun */
#define GMAC_ISR_HRESP (0x1u << 11) /**< \brief (GMAC_ISR) HRESP Not OK */
#define GMAC_ISR_PFNZ (0x1u << 12) /**< \brief (GMAC_ISR) Pause Frame with Non-zero Pause Quantum Received */
#define GMAC_ISR_PTZ (0x1u << 13) /**< \brief (GMAC_ISR) Pause Time Zero */
#define GMAC_ISR_PFTR (0x1u << 14) /**< \brief (GMAC_ISR) Pause Frame Transmitted */
#define GMAC_ISR_DRQFR (0x1u << 18) /**< \brief (GMAC_ISR) PTP Delay Request Frame Received */
#define GMAC_ISR_SFR (0x1u << 19) /**< \brief (GMAC_ISR) PTP Sync Frame Received */
#define GMAC_ISR_DRQFT (0x1u << 20) /**< \brief (GMAC_ISR) PTP Delay Request Frame Transmitted */
#define GMAC_ISR_SFT (0x1u << 21) /**< \brief (GMAC_ISR) PTP Sync Frame Transmitted */
#define GMAC_ISR_PDRQFR (0x1u << 22) /**< \brief (GMAC_ISR) PDelay Request Frame Received */
#define GMAC_ISR_PDRSFR (0x1u << 23) /**< \brief (GMAC_ISR) PDelay Response Frame Received */
#define GMAC_ISR_PDRQFT (0x1u << 24) /**< \brief (GMAC_ISR) PDelay Request Frame Transmitted */
#define GMAC_ISR_PDRSFT (0x1u << 25) /**< \brief (GMAC_ISR) PDelay Response Frame Transmitted */
#define GMAC_ISR_SRI (0x1u << 26) /**< \brief (GMAC_ISR) TSU Seconds Register Increment */
#define GMAC_ISR_WOL (0x1u << 28) /**< \brief (GMAC_ISR) Wake On LAN */
/* -------- GMAC_IER : (GMAC Offset: 0x028) Interrupt Enable Register -------- */
#define GMAC_IER_MFS (0x1u << 0) /**< \brief (GMAC_IER) Management Frame Sent */
#define GMAC_IER_RCOMP (0x1u << 1) /**< \brief (GMAC_IER) Receive Complete */
#define GMAC_IER_RXUBR (0x1u << 2) /**< \brief (GMAC_IER) RX Used Bit Read */
#define GMAC_IER_TXUBR (0x1u << 3) /**< \brief (GMAC_IER) TX Used Bit Read */
#define GMAC_IER_TUR (0x1u << 4) /**< \brief (GMAC_IER) Transmit Underrun */
#define GMAC_IER_RLEX (0x1u << 5) /**< \brief (GMAC_IER) Retry Limit Exceeded or Late Collision */
#define GMAC_IER_TFC (0x1u << 6) /**< \brief (GMAC_IER) Transmit Frame Corruption Due to AHB Error */
#define GMAC_IER_TCOMP (0x1u << 7) /**< \brief (GMAC_IER) Transmit Complete */
#define GMAC_IER_ROVR (0x1u << 10) /**< \brief (GMAC_IER) Receive Overrun */
#define GMAC_IER_HRESP (0x1u << 11) /**< \brief (GMAC_IER) HRESP Not OK */
#define GMAC_IER_PFNZ (0x1u << 12) /**< \brief (GMAC_IER) Pause Frame with Non-zero Pause Quantum Received */
#define GMAC_IER_PTZ (0x1u << 13) /**< \brief (GMAC_IER) Pause Time Zero */
#define GMAC_IER_PFTR (0x1u << 14) /**< \brief (GMAC_IER) Pause Frame Transmitted */
#define GMAC_IER_EXINT (0x1u << 15) /**< \brief (GMAC_IER) External Interrupt */
#define GMAC_IER_DRQFR (0x1u << 18) /**< \brief (GMAC_IER) PTP Delay Request Frame Received */
#define GMAC_IER_SFR (0x1u << 19) /**< \brief (GMAC_IER) PTP Sync Frame Received */
#define GMAC_IER_DRQFT (0x1u << 20) /**< \brief (GMAC_IER) PTP Delay Request Frame Transmitted */
#define GMAC_IER_SFT (0x1u << 21) /**< \brief (GMAC_IER) PTP Sync Frame Transmitted */
#define GMAC_IER_PDRQFR (0x1u << 22) /**< \brief (GMAC_IER) PDelay Request Frame Received */
#define GMAC_IER_PDRSFR (0x1u << 23) /**< \brief (GMAC_IER) PDelay Response Frame Received */
#define GMAC_IER_PDRQFT (0x1u << 24) /**< \brief (GMAC_IER) PDelay Request Frame Transmitted */
#define GMAC_IER_PDRSFT (0x1u << 25) /**< \brief (GMAC_IER) PDelay Response Frame Transmitted */
#define GMAC_IER_SRI (0x1u << 26) /**< \brief (GMAC_IER) TSU Seconds Register Increment */
#define GMAC_IER_WOL (0x1u << 28) /**< \brief (GMAC_IER) Wake On LAN */
/* -------- GMAC_IDR : (GMAC Offset: 0x02C) Interrupt Disable Register -------- */
#define GMAC_IDR_MFS (0x1u << 0) /**< \brief (GMAC_IDR) Management Frame Sent */
#define GMAC_IDR_RCOMP (0x1u << 1) /**< \brief (GMAC_IDR) Receive Complete */
#define GMAC_IDR_RXUBR (0x1u << 2) /**< \brief (GMAC_IDR) RX Used Bit Read */
#define GMAC_IDR_TXUBR (0x1u << 3) /**< \brief (GMAC_IDR) TX Used Bit Read */
#define GMAC_IDR_TUR (0x1u << 4) /**< \brief (GMAC_IDR) Transmit Underrun */
#define GMAC_IDR_RLEX (0x1u << 5) /**< \brief (GMAC_IDR) Retry Limit Exceeded or Late Collision */
#define GMAC_IDR_TFC (0x1u << 6) /**< \brief (GMAC_IDR) Transmit Frame Corruption Due to AHB Error */
#define GMAC_IDR_TCOMP (0x1u << 7) /**< \brief (GMAC_IDR) Transmit Complete */
#define GMAC_IDR_ROVR (0x1u << 10) /**< \brief (GMAC_IDR) Receive Overrun */
#define GMAC_IDR_HRESP (0x1u << 11) /**< \brief (GMAC_IDR) HRESP Not OK */
#define GMAC_IDR_PFNZ (0x1u << 12) /**< \brief (GMAC_IDR) Pause Frame with Non-zero Pause Quantum Received */
#define GMAC_IDR_PTZ (0x1u << 13) /**< \brief (GMAC_IDR) Pause Time Zero */
#define GMAC_IDR_PFTR (0x1u << 14) /**< \brief (GMAC_IDR) Pause Frame Transmitted */
#define GMAC_IDR_EXINT (0x1u << 15) /**< \brief (GMAC_IDR) External Interrupt */
#define GMAC_IDR_DRQFR (0x1u << 18) /**< \brief (GMAC_IDR) PTP Delay Request Frame Received */
#define GMAC_IDR_SFR (0x1u << 19) /**< \brief (GMAC_IDR) PTP Sync Frame Received */
#define GMAC_IDR_DRQFT (0x1u << 20) /**< \brief (GMAC_IDR) PTP Delay Request Frame Transmitted */
#define GMAC_IDR_SFT (0x1u << 21) /**< \brief (GMAC_IDR) PTP Sync Frame Transmitted */
#define GMAC_IDR_PDRQFR (0x1u << 22) /**< \brief (GMAC_IDR) PDelay Request Frame Received */
#define GMAC_IDR_PDRSFR (0x1u << 23) /**< \brief (GMAC_IDR) PDelay Response Frame Received */
#define GMAC_IDR_PDRQFT (0x1u << 24) /**< \brief (GMAC_IDR) PDelay Request Frame Transmitted */
#define GMAC_IDR_PDRSFT (0x1u << 25) /**< \brief (GMAC_IDR) PDelay Response Frame Transmitted */
#define GMAC_IDR_SRI (0x1u << 26) /**< \brief (GMAC_IDR) TSU Seconds Register Increment */
#define GMAC_IDR_WOL (0x1u << 28) /**< \brief (GMAC_IDR) Wake On LAN */
/* -------- GMAC_IMR : (GMAC Offset: 0x030) Interrupt Mask Register -------- */
#define GMAC_IMR_MFS (0x1u << 0) /**< \brief (GMAC_IMR) Management Frame Sent */
#define GMAC_IMR_RCOMP (0x1u << 1) /**< \brief (GMAC_IMR) Receive Complete */
#define GMAC_IMR_RXUBR (0x1u << 2) /**< \brief (GMAC_IMR) RX Used Bit Read */
#define GMAC_IMR_TXUBR (0x1u << 3) /**< \brief (GMAC_IMR) TX Used Bit Read */
#define GMAC_IMR_TUR (0x1u << 4) /**< \brief (GMAC_IMR) Transmit Underrun */
#define GMAC_IMR_RLEX (0x1u << 5) /**< \brief (GMAC_IMR) Retry Limit Exceeded */
#define GMAC_IMR_TFC (0x1u << 6) /**< \brief (GMAC_IMR) Transmit Frame Corruption Due to AHB Error */
#define GMAC_IMR_TCOMP (0x1u << 7) /**< \brief (GMAC_IMR) Transmit Complete */
#define GMAC_IMR_ROVR (0x1u << 10) /**< \brief (GMAC_IMR) Receive Overrun */
#define GMAC_IMR_HRESP (0x1u << 11) /**< \brief (GMAC_IMR) HRESP Not OK */
#define GMAC_IMR_PFNZ (0x1u << 12) /**< \brief (GMAC_IMR) Pause Frame with Non-zero Pause Quantum Received */
#define GMAC_IMR_PTZ (0x1u << 13) /**< \brief (GMAC_IMR) Pause Time Zero */
#define GMAC_IMR_PFTR (0x1u << 14) /**< \brief (GMAC_IMR) Pause Frame Transmitted */
#define GMAC_IMR_EXINT (0x1u << 15) /**< \brief (GMAC_IMR) External Interrupt */
#define GMAC_IMR_DRQFR (0x1u << 18) /**< \brief (GMAC_IMR) PTP Delay Request Frame Received */
#define GMAC_IMR_SFR (0x1u << 19) /**< \brief (GMAC_IMR) PTP Sync Frame Received */
#define GMAC_IMR_DRQFT (0x1u << 20) /**< \brief (GMAC_IMR) PTP Delay Request Frame Transmitted */
#define GMAC_IMR_SFT (0x1u << 21) /**< \brief (GMAC_IMR) PTP Sync Frame Transmitted */
#define GMAC_IMR_PDRQFR (0x1u << 22) /**< \brief (GMAC_IMR) PDelay Request Frame Received */
#define GMAC_IMR_PDRSFR (0x1u << 23) /**< \brief (GMAC_IMR) PDelay Response Frame Received */
#define GMAC_IMR_PDRQFT (0x1u << 24) /**< \brief (GMAC_IMR) PDelay Request Frame Transmitted */
#define GMAC_IMR_PDRSFT (0x1u << 25) /**< \brief (GMAC_IMR) PDelay Response Frame Transmitted */
#define GMAC_IMR_SRI (0x1u << 26) /**< \brief (GMAC_IMR) TSU Seconds Register Increment */
#define GMAC_IMR_WOL (0x1u << 28) /**< \brief (GMAC_IMR) Wake On LAN */
/* -------- GMAC_MAN : (GMAC Offset: 0x034) PHY Maintenance Register -------- */
#define GMAC_MAN_DATA_Pos 0
#define GMAC_MAN_DATA_Msk (0xffffu << GMAC_MAN_DATA_Pos) /**< \brief (GMAC_MAN) PHY Data */
#define GMAC_MAN_DATA(value) ((GMAC_MAN_DATA_Msk & ((value) << GMAC_MAN_DATA_Pos)))
#define GMAC_MAN_WTN_Pos 16
#define GMAC_MAN_WTN_Msk (0x3u << GMAC_MAN_WTN_Pos) /**< \brief (GMAC_MAN) Write Ten */
#define GMAC_MAN_WTN(value) ((GMAC_MAN_WTN_Msk & ((value) << GMAC_MAN_WTN_Pos)))
#define GMAC_MAN_REGA_Pos 18
#define GMAC_MAN_REGA_Msk (0x1fu << GMAC_MAN_REGA_Pos) /**< \brief (GMAC_MAN) Register Address */
#define GMAC_MAN_REGA(value) ((GMAC_MAN_REGA_Msk & ((value) << GMAC_MAN_REGA_Pos)))
#define GMAC_MAN_PHYA_Pos 23
#define GMAC_MAN_PHYA_Msk (0x1fu << GMAC_MAN_PHYA_Pos) /**< \brief (GMAC_MAN) PHY Address */
#define GMAC_MAN_PHYA(value) ((GMAC_MAN_PHYA_Msk & ((value) << GMAC_MAN_PHYA_Pos)))
#define GMAC_MAN_OP_Pos 28
#define GMAC_MAN_OP_Msk (0x3u << GMAC_MAN_OP_Pos) /**< \brief (GMAC_MAN) Operation */
#define GMAC_MAN_OP(value) ((GMAC_MAN_OP_Msk & ((value) << GMAC_MAN_OP_Pos)))
#define GMAC_MAN_CLTTO (0x1u << 30) /**< \brief (GMAC_MAN) Clause 22 Operation */
#define GMAC_MAN_WZO (0x1u << 31) /**< \brief (GMAC_MAN) Write ZERO */
/* -------- GMAC_RPQ : (GMAC Offset: 0x038) Received Pause Quantum Register -------- */
#define GMAC_RPQ_RPQ_Pos 0
#define GMAC_RPQ_RPQ_Msk (0xffffu << GMAC_RPQ_RPQ_Pos) /**< \brief (GMAC_RPQ) Received Pause Quantum */
/* -------- GMAC_TPQ : (GMAC Offset: 0x03C) Transmit Pause Quantum Register -------- */
#define GMAC_TPQ_TPQ_Pos 0
#define GMAC_TPQ_TPQ_Msk (0xffffu << GMAC_TPQ_TPQ_Pos) /**< \brief (GMAC_TPQ) Transmit Pause Quantum */
#define GMAC_TPQ_TPQ(value) ((GMAC_TPQ_TPQ_Msk & ((value) << GMAC_TPQ_TPQ_Pos)))
/* -------- GMAC_TPSF : (GMAC Offset: 0x040) TX Partial Store and Forward Register -------- */
#define GMAC_TPSF_TPB1ADR_Pos 0
#define GMAC_TPSF_TPB1ADR_Msk (0xfffu << GMAC_TPSF_TPB1ADR_Pos) /**< \brief (GMAC_TPSF) Transmit Partial Store and Forward Address */
#define GMAC_TPSF_TPB1ADR(value) ((GMAC_TPSF_TPB1ADR_Msk & ((value) << GMAC_TPSF_TPB1ADR_Pos)))
#define GMAC_TPSF_ENTXP (0x1u << 31) /**< \brief (GMAC_TPSF) Enable TX Partial Store and Forward Operation */
/* -------- GMAC_RPSF : (GMAC Offset: 0x044) RX Partial Store and Forward Register -------- */
#define GMAC_RPSF_RPB1ADR_Pos 0
#define GMAC_RPSF_RPB1ADR_Msk (0xfffu << GMAC_RPSF_RPB1ADR_Pos) /**< \brief (GMAC_RPSF) Receive Partial Store and Forward Address */
#define GMAC_RPSF_RPB1ADR(value) ((GMAC_RPSF_RPB1ADR_Msk & ((value) << GMAC_RPSF_RPB1ADR_Pos)))
#define GMAC_RPSF_ENRXP (0x1u << 31) /**< \brief (GMAC_RPSF) Enable RX Partial Store and Forward Operation */
/* -------- GMAC_RJFML : (GMAC Offset: 0x048) RX Jumbo Frame Max Length Register -------- */
#define GMAC_RJFML_FML_Pos 0
#define GMAC_RJFML_FML_Msk (0x3fffu << GMAC_RJFML_FML_Pos) /**< \brief (GMAC_RJFML) Frame Max Length */
#define GMAC_RJFML_FML(value) ((GMAC_RJFML_FML_Msk & ((value) << GMAC_RJFML_FML_Pos)))
/* -------- GMAC_HRB : (GMAC Offset: 0x080) Hash Register Bottom -------- */
#define GMAC_HRB_ADDR_Pos 0
#define GMAC_HRB_ADDR_Msk (0xffffffffu << GMAC_HRB_ADDR_Pos) /**< \brief (GMAC_HRB) Hash Address */
#define GMAC_HRB_ADDR(value) ((GMAC_HRB_ADDR_Msk & ((value) << GMAC_HRB_ADDR_Pos)))
/* -------- GMAC_HRT : (GMAC Offset: 0x084) Hash Register Top -------- */
#define GMAC_HRT_ADDR_Pos 0
#define GMAC_HRT_ADDR_Msk (0xffffffffu << GMAC_HRT_ADDR_Pos) /**< \brief (GMAC_HRT) Hash Address */
#define GMAC_HRT_ADDR(value) ((GMAC_HRT_ADDR_Msk & ((value) << GMAC_HRT_ADDR_Pos)))
/* -------- GMAC_SAB : (GMAC Offset: N/A) Specific Address 1 Bottom Register -------- */
#define GMAC_SAB_ADDR_Pos 0
#define GMAC_SAB_ADDR_Msk (0xffffffffu << GMAC_SAB_ADDR_Pos) /**< \brief (GMAC_SAB) Specific Address 1 */
#define GMAC_SAB_ADDR(value) ((GMAC_SAB_ADDR_Msk & ((value) << GMAC_SAB_ADDR_Pos)))
/* -------- GMAC_SAT : (GMAC Offset: N/A) Specific Address 1 Top Register -------- */
#define GMAC_SAT_ADDR_Pos 0
#define GMAC_SAT_ADDR_Msk (0xffffu << GMAC_SAT_ADDR_Pos) /**< \brief (GMAC_SAT) Specific Address 1 */
#define GMAC_SAT_ADDR(value) ((GMAC_SAT_ADDR_Msk & ((value) << GMAC_SAT_ADDR_Pos)))
/* -------- GMAC_TIDM1 : (GMAC Offset: 0x0A8) Type ID Match 1 Register -------- */
#define GMAC_TIDM1_TID_Pos 0
#define GMAC_TIDM1_TID_Msk (0xffffu << GMAC_TIDM1_TID_Pos) /**< \brief (GMAC_TIDM1) Type ID Match 1 */
#define GMAC_TIDM1_TID(value) ((GMAC_TIDM1_TID_Msk & ((value) << GMAC_TIDM1_TID_Pos)))
#define GMAC_TIDM1_ENID1 (0x1u << 31) /**< \brief (GMAC_TIDM1) Enable Copying of TID Matched Frames */
/* -------- GMAC_TIDM2 : (GMAC Offset: 0x0AC) Type ID Match 2 Register -------- */
#define GMAC_TIDM2_TID_Pos 0
#define GMAC_TIDM2_TID_Msk (0xffffu << GMAC_TIDM2_TID_Pos) /**< \brief (GMAC_TIDM2) Type ID Match 2 */
#define GMAC_TIDM2_TID(value) ((GMAC_TIDM2_TID_Msk & ((value) << GMAC_TIDM2_TID_Pos)))
#define GMAC_TIDM2_ENID2 (0x1u << 31) /**< \brief (GMAC_TIDM2) Enable Copying of TID Matched Frames */
/* -------- GMAC_TIDM3 : (GMAC Offset: 0x0B0) Type ID Match 3 Register -------- */
#define GMAC_TIDM3_TID_Pos 0
#define GMAC_TIDM3_TID_Msk (0xffffu << GMAC_TIDM3_TID_Pos) /**< \brief (GMAC_TIDM3) Type ID Match 3 */
#define GMAC_TIDM3_TID(value) ((GMAC_TIDM3_TID_Msk & ((value) << GMAC_TIDM3_TID_Pos)))
#define GMAC_TIDM3_ENID3 (0x1u << 31) /**< \brief (GMAC_TIDM3) Enable Copying of TID Matched Frames */
/* -------- GMAC_TIDM4 : (GMAC Offset: 0x0B4) Type ID Match 4 Register -------- */
#define GMAC_TIDM4_TID_Pos 0
#define GMAC_TIDM4_TID_Msk (0xffffu << GMAC_TIDM4_TID_Pos) /**< \brief (GMAC_TIDM4) Type ID Match 4 */
#define GMAC_TIDM4_TID(value) ((GMAC_TIDM4_TID_Msk & ((value) << GMAC_TIDM4_TID_Pos)))
#define GMAC_TIDM4_ENID4 (0x1u << 31) /**< \brief (GMAC_TIDM4) Enable Copying of TID Matched Frames */
/* -------- GMAC_WOL : (GMAC Offset: 0x0B8) Wake on LAN Register -------- */
#define GMAC_WOL_IP_Pos 0
#define GMAC_WOL_IP_Msk (0xffffu << GMAC_WOL_IP_Pos) /**< \brief (GMAC_WOL) ARP Request IP Address */
#define GMAC_WOL_IP(value) ((GMAC_WOL_IP_Msk & ((value) << GMAC_WOL_IP_Pos)))
#define GMAC_WOL_MAG (0x1u << 16) /**< \brief (GMAC_WOL) Magic Packet Event Enable */
#define GMAC_WOL_ARP (0x1u << 17) /**< \brief (GMAC_WOL) ARP Request IP Address */
#define GMAC_WOL_SA1 (0x1u << 18) /**< \brief (GMAC_WOL) Specific Address Register 1 Event Enable */
#define GMAC_WOL_MTI (0x1u << 19) /**< \brief (GMAC_WOL) Multicast Hash Event Enable */
/* -------- GMAC_IPGS : (GMAC Offset: 0x0BC) IPG Stretch Register -------- */
#define GMAC_IPGS_FL_Pos 0
#define GMAC_IPGS_FL_Msk (0xffffu << GMAC_IPGS_FL_Pos) /**< \brief (GMAC_IPGS) Frame Length */
#define GMAC_IPGS_FL(value) ((GMAC_IPGS_FL_Msk & ((value) << GMAC_IPGS_FL_Pos)))
/* -------- GMAC_SVLAN : (GMAC Offset: 0x0C0) Stacked VLAN Register -------- */
#define GMAC_SVLAN_VLAN_TYPE_Pos 0
#define GMAC_SVLAN_VLAN_TYPE_Msk (0xffffu << GMAC_SVLAN_VLAN_TYPE_Pos) /**< \brief (GMAC_SVLAN) User Defined VLAN_TYPE Field */
#define GMAC_SVLAN_VLAN_TYPE(value) ((GMAC_SVLAN_VLAN_TYPE_Msk & ((value) << GMAC_SVLAN_VLAN_TYPE_Pos)))
#define GMAC_SVLAN_ESVLAN (0x1u << 31) /**< \brief (GMAC_SVLAN) Enable Stacked VLAN Processing Mode */
/* -------- GMAC_TPFCP : (GMAC Offset: 0x0C4) Transmit PFC Pause Register -------- */
#define GMAC_TPFCP_PEV_Pos 0
#define GMAC_TPFCP_PEV_Msk (0xffu << GMAC_TPFCP_PEV_Pos) /**< \brief (GMAC_TPFCP) Priority Enable Vector */
#define GMAC_TPFCP_PEV(value) ((GMAC_TPFCP_PEV_Msk & ((value) << GMAC_TPFCP_PEV_Pos)))
#define GMAC_TPFCP_PQ_Pos 8
#define GMAC_TPFCP_PQ_Msk (0xffu << GMAC_TPFCP_PQ_Pos) /**< \brief (GMAC_TPFCP) Pause Quantum */
#define GMAC_TPFCP_PQ(value) ((GMAC_TPFCP_PQ_Msk & ((value) << GMAC_TPFCP_PQ_Pos)))
/* -------- GMAC_SAMB1 : (GMAC Offset: 0x0C8) Specific Address 1 Mask Bottom Register -------- */
#define GMAC_SAMB1_ADDR_Pos 0
#define GMAC_SAMB1_ADDR_Msk (0xffffffffu << GMAC_SAMB1_ADDR_Pos) /**< \brief (GMAC_SAMB1) Specific Address 1 Mask */
#define GMAC_SAMB1_ADDR(value) ((GMAC_SAMB1_ADDR_Msk & ((value) << GMAC_SAMB1_ADDR_Pos)))
/* -------- GMAC_SAMT1 : (GMAC Offset: 0x0CC) Specific Address 1 Mask Top Register -------- */
#define GMAC_SAMT1_ADDR_Pos 0
#define GMAC_SAMT1_ADDR_Msk (0xffffu << GMAC_SAMT1_ADDR_Pos) /**< \brief (GMAC_SAMT1) Specific Address 1 Mask */
#define GMAC_SAMT1_ADDR(value) ((GMAC_SAMT1_ADDR_Msk & ((value) << GMAC_SAMT1_ADDR_Pos)))
/* -------- GMAC_NSC : (GMAC Offset: 0x0DC) 1588 Timer Nanosecond Comparison Register -------- */
#define GMAC_NSC_NANOSEC_Pos 0
#define GMAC_NSC_NANOSEC_Msk (0x3fffffu << GMAC_NSC_NANOSEC_Pos) /**< \brief (GMAC_NSC) 1588 Timer Nanosecond Comparison Value */
#define GMAC_NSC_NANOSEC(value) ((GMAC_NSC_NANOSEC_Msk & ((value) << GMAC_NSC_NANOSEC_Pos)))
/* -------- GMAC_SCL : (GMAC Offset: 0x0E0) 1588 Timer Second Comparison Low Register -------- */
#define GMAC_SCL_SEC_Pos 0
#define GMAC_SCL_SEC_Msk (0xffffffffu << GMAC_SCL_SEC_Pos) /**< \brief (GMAC_SCL) 1588 Timer Second Comparison Value */
#define GMAC_SCL_SEC(value) ((GMAC_SCL_SEC_Msk & ((value) << GMAC_SCL_SEC_Pos)))
/* -------- GMAC_SCH : (GMAC Offset: 0x0E4) 1588 Timer Second Comparison High Register -------- */
#define GMAC_SCH_SEC_Pos 0
#define GMAC_SCH_SEC_Msk (0xffffu << GMAC_SCH_SEC_Pos) /**< \brief (GMAC_SCH) 1588 Timer Second Comparison Value */
#define GMAC_SCH_SEC(value) ((GMAC_SCH_SEC_Msk & ((value) << GMAC_SCH_SEC_Pos)))
/* -------- GMAC_EFTSH : (GMAC Offset: 0x0E8) PTP Event Frame Transmitted Seconds High Register -------- */
#define GMAC_EFTSH_RUD_Pos 0
#define GMAC_EFTSH_RUD_Msk (0xffffu << GMAC_EFTSH_RUD_Pos) /**< \brief (GMAC_EFTSH) Register Update */
/* -------- GMAC_EFRSH : (GMAC Offset: 0x0EC) PTP Event Frame Received Seconds High Register -------- */
#define GMAC_EFRSH_RUD_Pos 0
#define GMAC_EFRSH_RUD_Msk (0xffffu << GMAC_EFRSH_RUD_Pos) /**< \brief (GMAC_EFRSH) Register Update */
/* -------- GMAC_PEFTSH : (GMAC Offset: 0x0F0) PTP Peer Event Frame Transmitted Seconds High Register -------- */
#define GMAC_PEFTSH_RUD_Pos 0
#define GMAC_PEFTSH_RUD_Msk (0xffffu << GMAC_PEFTSH_RUD_Pos) /**< \brief (GMAC_PEFTSH) Register Update */
/* -------- GMAC_PEFRSH : (GMAC Offset: 0x0F4) PTP Peer Event Frame Received Seconds High Register -------- */
#define GMAC_PEFRSH_RUD_Pos 0
#define GMAC_PEFRSH_RUD_Msk (0xffffu << GMAC_PEFRSH_RUD_Pos) /**< \brief (GMAC_PEFRSH) Register Update */
/* -------- GMAC_MID : (GMAC Offset: 0x0FC) Module ID Register -------- */
#define GMAC_MID_MREV_Pos 0
#define GMAC_MID_MREV_Msk (0xffffu << GMAC_MID_MREV_Pos) /**< \brief (GMAC_MID) Module Revision */
#define GMAC_MID_MID_Pos 16
#define GMAC_MID_MID_Msk (0xffffu << GMAC_MID_MID_Pos) /**< \brief (GMAC_MID) Module Identification Number */
/* -------- GMAC_OTLO : (GMAC Offset: 0x100) Octets Transmitted Low Register -------- */
#define GMAC_OTLO_TXO_Pos 0
#define GMAC_OTLO_TXO_Msk (0xffffffffu << GMAC_OTLO_TXO_Pos) /**< \brief (GMAC_OTLO) Transmitted Octets */
/* -------- GMAC_OTHI : (GMAC Offset: 0x104) Octets Transmitted High Register -------- */
#define GMAC_OTHI_TXO_Pos 0
#define GMAC_OTHI_TXO_Msk (0xffffu << GMAC_OTHI_TXO_Pos) /**< \brief (GMAC_OTHI) Transmitted Octets */
/* -------- GMAC_FT : (GMAC Offset: 0x108) Frames Transmitted Register -------- */
#define GMAC_FT_FTX_Pos 0
#define GMAC_FT_FTX_Msk (0xffffffffu << GMAC_FT_FTX_Pos) /**< \brief (GMAC_FT) Frames Transmitted without Error */
/* -------- GMAC_BCFT : (GMAC Offset: 0x10C) Broadcast Frames Transmitted Register -------- */
#define GMAC_BCFT_BFTX_Pos 0
#define GMAC_BCFT_BFTX_Msk (0xffffffffu << GMAC_BCFT_BFTX_Pos) /**< \brief (GMAC_BCFT) Broadcast Frames Transmitted without Error */
/* -------- GMAC_MFT : (GMAC Offset: 0x110) Multicast Frames Transmitted Register -------- */
#define GMAC_MFT_MFTX_Pos 0
#define GMAC_MFT_MFTX_Msk (0xffffffffu << GMAC_MFT_MFTX_Pos) /**< \brief (GMAC_MFT) Multicast Frames Transmitted without Error */
/* -------- GMAC_PFT : (GMAC Offset: 0x114) Pause Frames Transmitted Register -------- */
#define GMAC_PFT_PFTX_Pos 0
#define GMAC_PFT_PFTX_Msk (0xffffu << GMAC_PFT_PFTX_Pos) /**< \brief (GMAC_PFT) Pause Frames Transmitted Register */
/* -------- GMAC_BFT64 : (GMAC Offset: 0x118) 64 Byte Frames Transmitted Register -------- */
#define GMAC_BFT64_NFTX_Pos 0
#define GMAC_BFT64_NFTX_Msk (0xffffffffu << GMAC_BFT64_NFTX_Pos) /**< \brief (GMAC_BFT64) 64 Byte Frames Transmitted without Error */
/* -------- GMAC_TBFT127 : (GMAC Offset: 0x11C) 65 to 127 Byte Frames Transmitted Register -------- */
#define GMAC_TBFT127_NFTX_Pos 0
#define GMAC_TBFT127_NFTX_Msk (0xffffffffu << GMAC_TBFT127_NFTX_Pos) /**< \brief (GMAC_TBFT127) 65 to 127 Byte Frames Transmitted without Error */
/* -------- GMAC_TBFT255 : (GMAC Offset: 0x120) 128 to 255 Byte Frames Transmitted Register -------- */
#define GMAC_TBFT255_NFTX_Pos 0
#define GMAC_TBFT255_NFTX_Msk (0xffffffffu << GMAC_TBFT255_NFTX_Pos) /**< \brief (GMAC_TBFT255) 128 to 255 Byte Frames Transmitted without Error */
/* -------- GMAC_TBFT511 : (GMAC Offset: 0x124) 256 to 511 Byte Frames Transmitted Register -------- */
#define GMAC_TBFT511_NFTX_Pos 0
#define GMAC_TBFT511_NFTX_Msk (0xffffffffu << GMAC_TBFT511_NFTX_Pos) /**< \brief (GMAC_TBFT511) 256 to 511 Byte Frames Transmitted without Error */
/* -------- GMAC_TBFT1023 : (GMAC Offset: 0x128) 512 to 1023 Byte Frames Transmitted Register -------- */
#define GMAC_TBFT1023_NFTX_Pos 0
#define GMAC_TBFT1023_NFTX_Msk (0xffffffffu << GMAC_TBFT1023_NFTX_Pos) /**< \brief (GMAC_TBFT1023) 512 to 1023 Byte Frames Transmitted without Error */
/* -------- GMAC_TBFT1518 : (GMAC Offset: 0x12C) 1024 to 1518 Byte Frames Transmitted Register -------- */
#define GMAC_TBFT1518_NFTX_Pos 0
#define GMAC_TBFT1518_NFTX_Msk (0xffffffffu << GMAC_TBFT1518_NFTX_Pos) /**< \brief (GMAC_TBFT1518) 1024 to 1518 Byte Frames Transmitted without Error */
/* -------- GMAC_GTBFT1518 : (GMAC Offset: 0x130) Greater Than 1518 Byte Frames Transmitted Register -------- */
#define GMAC_GTBFT1518_NFTX_Pos 0
#define GMAC_GTBFT1518_NFTX_Msk (0xffffffffu << GMAC_GTBFT1518_NFTX_Pos) /**< \brief (GMAC_GTBFT1518) Greater than 1518 Byte Frames Transmitted without Error */
/* -------- GMAC_TUR : (GMAC Offset: 0x134) Transmit Underruns Register -------- */
#define GMAC_TUR_TXUNR_Pos 0
#define GMAC_TUR_TXUNR_Msk (0x3ffu << GMAC_TUR_TXUNR_Pos) /**< \brief (GMAC_TUR) Transmit Underruns */
/* -------- GMAC_SCF : (GMAC Offset: 0x138) Single Collision Frames Register -------- */
#define GMAC_SCF_SCOL_Pos 0
#define GMAC_SCF_SCOL_Msk (0x3ffffu << GMAC_SCF_SCOL_Pos) /**< \brief (GMAC_SCF) Single Collision */
/* -------- GMAC_MCF : (GMAC Offset: 0x13C) Multiple Collision Frames Register -------- */
#define GMAC_MCF_MCOL_Pos 0
#define GMAC_MCF_MCOL_Msk (0x3ffffu << GMAC_MCF_MCOL_Pos) /**< \brief (GMAC_MCF) Multiple Collision */
/* -------- GMAC_EC : (GMAC Offset: 0x140) Excessive Collisions Register -------- */
#define GMAC_EC_XCOL_Pos 0
#define GMAC_EC_XCOL_Msk (0x3ffu << GMAC_EC_XCOL_Pos) /**< \brief (GMAC_EC) Excessive Collisions */
/* -------- GMAC_LC : (GMAC Offset: 0x144) Late Collisions Register -------- */
#define GMAC_LC_LCOL_Pos 0
#define GMAC_LC_LCOL_Msk (0x3ffu << GMAC_LC_LCOL_Pos) /**< \brief (GMAC_LC) Late Collisions */
/* -------- GMAC_DTF : (GMAC Offset: 0x148) Deferred Transmission Frames Register -------- */
#define GMAC_DTF_DEFT_Pos 0
#define GMAC_DTF_DEFT_Msk (0x3ffffu << GMAC_DTF_DEFT_Pos) /**< \brief (GMAC_DTF) Deferred Transmission */
/* -------- GMAC_CSE : (GMAC Offset: 0x14C) Carrier Sense Errors Register -------- */
#define GMAC_CSE_CSR_Pos 0
#define GMAC_CSE_CSR_Msk (0x3ffu << GMAC_CSE_CSR_Pos) /**< \brief (GMAC_CSE) Carrier Sense Error */
/* -------- GMAC_ORLO : (GMAC Offset: 0x150) Octets Received Low Received Register -------- */
#define GMAC_ORLO_RXO_Pos 0
#define GMAC_ORLO_RXO_Msk (0xffffffffu << GMAC_ORLO_RXO_Pos) /**< \brief (GMAC_ORLO) Received Octets */
/* -------- GMAC_ORHI : (GMAC Offset: 0x154) Octets Received High Received Register -------- */
#define GMAC_ORHI_RXO_Pos 0
#define GMAC_ORHI_RXO_Msk (0xffffu << GMAC_ORHI_RXO_Pos) /**< \brief (GMAC_ORHI) Received Octets */
/* -------- GMAC_FR : (GMAC Offset: 0x158) Frames Received Register -------- */
#define GMAC_FR_FRX_Pos 0
#define GMAC_FR_FRX_Msk (0xffffffffu << GMAC_FR_FRX_Pos) /**< \brief (GMAC_FR) Frames Received without Error */
/* -------- GMAC_BCFR : (GMAC Offset: 0x15C) Broadcast Frames Received Register -------- */
#define GMAC_BCFR_BFRX_Pos 0
#define GMAC_BCFR_BFRX_Msk (0xffffffffu << GMAC_BCFR_BFRX_Pos) /**< \brief (GMAC_BCFR) Broadcast Frames Received without Error */
/* -------- GMAC_MFR : (GMAC Offset: 0x160) Multicast Frames Received Register -------- */
#define GMAC_MFR_MFRX_Pos 0
#define GMAC_MFR_MFRX_Msk (0xffffffffu << GMAC_MFR_MFRX_Pos) /**< \brief (GMAC_MFR) Multicast Frames Received without Error */
/* -------- GMAC_PFR : (GMAC Offset: 0x164) Pause Frames Received Register -------- */
#define GMAC_PFR_PFRX_Pos 0
#define GMAC_PFR_PFRX_Msk (0xffffu << GMAC_PFR_PFRX_Pos) /**< \brief (GMAC_PFR) Pause Frames Received Register */
/* -------- GMAC_BFR64 : (GMAC Offset: 0x168) 64 Byte Frames Received Register -------- */
#define GMAC_BFR64_NFRX_Pos 0
#define GMAC_BFR64_NFRX_Msk (0xffffffffu << GMAC_BFR64_NFRX_Pos) /**< \brief (GMAC_BFR64) 64 Byte Frames Received without Error */
/* -------- GMAC_TBFR127 : (GMAC Offset: 0x16C) 65 to 127 Byte Frames Received Register -------- */
#define GMAC_TBFR127_NFRX_Pos 0
#define GMAC_TBFR127_NFRX_Msk (0xffffffffu << GMAC_TBFR127_NFRX_Pos) /**< \brief (GMAC_TBFR127) 65 to 127 Byte Frames Received without Error */
/* -------- GMAC_TBFR255 : (GMAC Offset: 0x170) 128 to 255 Byte Frames Received Register -------- */
#define GMAC_TBFR255_NFRX_Pos 0
#define GMAC_TBFR255_NFRX_Msk (0xffffffffu << GMAC_TBFR255_NFRX_Pos) /**< \brief (GMAC_TBFR255) 128 to 255 Byte Frames Received without Error */
/* -------- GMAC_TBFR511 : (GMAC Offset: 0x174) 256 to 511 Byte Frames Received Register -------- */
#define GMAC_TBFR511_NFRX_Pos 0
#define GMAC_TBFR511_NFRX_Msk (0xffffffffu << GMAC_TBFR511_NFRX_Pos) /**< \brief (GMAC_TBFR511) 256 to 511 Byte Frames Received without Error */
/* -------- GMAC_TBFR1023 : (GMAC Offset: 0x178) 512 to 1023 Byte Frames Received Register -------- */
#define GMAC_TBFR1023_NFRX_Pos 0
#define GMAC_TBFR1023_NFRX_Msk (0xffffffffu << GMAC_TBFR1023_NFRX_Pos) /**< \brief (GMAC_TBFR1023) 512 to 1023 Byte Frames Received without Error */
/* -------- GMAC_TBFR1518 : (GMAC Offset: 0x17C) 1024 to 1518 Byte Frames Received Register -------- */
#define GMAC_TBFR1518_NFRX_Pos 0
#define GMAC_TBFR1518_NFRX_Msk (0xffffffffu << GMAC_TBFR1518_NFRX_Pos) /**< \brief (GMAC_TBFR1518) 1024 to 1518 Byte Frames Received without Error */
/* -------- GMAC_TMXBFR : (GMAC Offset: 0x180) 1519 to Maximum Byte Frames Received Register -------- */
#define GMAC_TMXBFR_NFRX_Pos 0
#define GMAC_TMXBFR_NFRX_Msk (0xffffffffu << GMAC_TMXBFR_NFRX_Pos) /**< \brief (GMAC_TMXBFR) 1519 to Maximum Byte Frames Received without Error */
/* -------- GMAC_UFR : (GMAC Offset: 0x184) Undersize Frames Received Register -------- */
#define GMAC_UFR_UFRX_Pos 0
#define GMAC_UFR_UFRX_Msk (0x3ffu << GMAC_UFR_UFRX_Pos) /**< \brief (GMAC_UFR) Undersize Frames Received */
/* -------- GMAC_OFR : (GMAC Offset: 0x188) Oversize Frames Received Register -------- */
#define GMAC_OFR_OFRX_Pos 0
#define GMAC_OFR_OFRX_Msk (0x3ffu << GMAC_OFR_OFRX_Pos) /**< \brief (GMAC_OFR) Oversized Frames Received */
/* -------- GMAC_JR : (GMAC Offset: 0x18C) Jabbers Received Register -------- */
#define GMAC_JR_JRX_Pos 0
#define GMAC_JR_JRX_Msk (0x3ffu << GMAC_JR_JRX_Pos) /**< \brief (GMAC_JR) Jabbers Received */
/* -------- GMAC_FCSE : (GMAC Offset: 0x190) Frame Check Sequence Errors Register -------- */
#define GMAC_FCSE_FCKR_Pos 0
#define GMAC_FCSE_FCKR_Msk (0x3ffu << GMAC_FCSE_FCKR_Pos) /**< \brief (GMAC_FCSE) Frame Check Sequence Errors */
/* -------- GMAC_LFFE : (GMAC Offset: 0x194) Length Field Frame Errors Register -------- */
#define GMAC_LFFE_LFER_Pos 0
#define GMAC_LFFE_LFER_Msk (0x3ffu << GMAC_LFFE_LFER_Pos) /**< \brief (GMAC_LFFE) Length Field Frame Errors */
/* -------- GMAC_RSE : (GMAC Offset: 0x198) Receive Symbol Errors Register -------- */
#define GMAC_RSE_RXSE_Pos 0
#define GMAC_RSE_RXSE_Msk (0x3ffu << GMAC_RSE_RXSE_Pos) /**< \brief (GMAC_RSE) Receive Symbol Errors */
/* -------- GMAC_AE : (GMAC Offset: 0x19C) Alignment Errors Register -------- */
#define GMAC_AE_AER_Pos 0
#define GMAC_AE_AER_Msk (0x3ffu << GMAC_AE_AER_Pos) /**< \brief (GMAC_AE) Alignment Errors */
/* -------- GMAC_RRE : (GMAC Offset: 0x1A0) Receive Resource Errors Register -------- */
#define GMAC_RRE_RXRER_Pos 0
#define GMAC_RRE_RXRER_Msk (0x3ffffu << GMAC_RRE_RXRER_Pos) /**< \brief (GMAC_RRE) Receive Resource Errors */
/* -------- GMAC_ROE : (GMAC Offset: 0x1A4) Receive Overrun Register -------- */
#define GMAC_ROE_RXOVR_Pos 0
#define GMAC_ROE_RXOVR_Msk (0x3ffu << GMAC_ROE_RXOVR_Pos) /**< \brief (GMAC_ROE) Receive Overruns */
/* -------- GMAC_IHCE : (GMAC Offset: 0x1A8) IP Header Checksum Errors Register -------- */
#define GMAC_IHCE_HCKER_Pos 0
#define GMAC_IHCE_HCKER_Msk (0xffu << GMAC_IHCE_HCKER_Pos) /**< \brief (GMAC_IHCE) IP Header Checksum Errors */
/* -------- GMAC_TCE : (GMAC Offset: 0x1AC) TCP Checksum Errors Register -------- */
#define GMAC_TCE_TCKER_Pos 0
#define GMAC_TCE_TCKER_Msk (0xffu << GMAC_TCE_TCKER_Pos) /**< \brief (GMAC_TCE) TCP Checksum Errors */
/* -------- GMAC_UCE : (GMAC Offset: 0x1B0) UDP Checksum Errors Register -------- */
#define GMAC_UCE_UCKER_Pos 0
#define GMAC_UCE_UCKER_Msk (0xffu << GMAC_UCE_UCKER_Pos) /**< \brief (GMAC_UCE) UDP Checksum Errors */
/* -------- GMAC_TISUBN : (GMAC Offset: 0x1BC) 1588 Timer Increment Sub-nanoseconds Register -------- */
#define GMAC_TISUBN_LSBTIR_Pos 0
#define GMAC_TISUBN_LSBTIR_Msk (0xffffu << GMAC_TISUBN_LSBTIR_Pos) /**< \brief (GMAC_TISUBN) Lower Significant Bits of Timer Increment Register */
#define GMAC_TISUBN_LSBTIR(value) ((GMAC_TISUBN_LSBTIR_Msk & ((value) << GMAC_TISUBN_LSBTIR_Pos)))
/* -------- GMAC_TSH : (GMAC Offset: 0x1C0) 1588 Timer Seconds High Register -------- */
#define GMAC_TSH_TCS_Pos 0
#define GMAC_TSH_TCS_Msk (0xffffu << GMAC_TSH_TCS_Pos) /**< \brief (GMAC_TSH) Timer Count in Seconds */
#define GMAC_TSH_TCS(value) ((GMAC_TSH_TCS_Msk & ((value) << GMAC_TSH_TCS_Pos)))
/* -------- GMAC_TSL : (GMAC Offset: 0x1D0) 1588 Timer Seconds Low Register -------- */
#define GMAC_TSL_TCS_Pos 0
#define GMAC_TSL_TCS_Msk (0xffffffffu << GMAC_TSL_TCS_Pos) /**< \brief (GMAC_TSL) Timer Count in Seconds */
#define GMAC_TSL_TCS(value) ((GMAC_TSL_TCS_Msk & ((value) << GMAC_TSL_TCS_Pos)))
/* -------- GMAC_TN : (GMAC Offset: 0x1D4) 1588 Timer Nanoseconds Register -------- */
#define GMAC_TN_TNS_Pos 0
#define GMAC_TN_TNS_Msk (0x3fffffffu << GMAC_TN_TNS_Pos) /**< \brief (GMAC_TN) Timer Count in Nanoseconds */
#define GMAC_TN_TNS(value) ((GMAC_TN_TNS_Msk & ((value) << GMAC_TN_TNS_Pos)))
/* -------- GMAC_TA : (GMAC Offset: 0x1D8) 1588 Timer Adjust Register -------- */
#define GMAC_TA_ITDT_Pos 0
#define GMAC_TA_ITDT_Msk (0x3fffffffu << GMAC_TA_ITDT_Pos) /**< \brief (GMAC_TA) Increment/Decrement */
#define GMAC_TA_ITDT(value) ((GMAC_TA_ITDT_Msk & ((value) << GMAC_TA_ITDT_Pos)))
#define GMAC_TA_ADJ (0x1u << 31) /**< \brief (GMAC_TA) Adjust 1588 Timer */
/* -------- GMAC_TI : (GMAC Offset: 0x1DC) 1588 Timer Increment Register -------- */
#define GMAC_TI_CNS_Pos 0
#define GMAC_TI_CNS_Msk (0xffu << GMAC_TI_CNS_Pos) /**< \brief (GMAC_TI) Count Nanoseconds */
#define GMAC_TI_CNS(value) ((GMAC_TI_CNS_Msk & ((value) << GMAC_TI_CNS_Pos)))
#define GMAC_TI_ACNS_Pos 8
#define GMAC_TI_ACNS_Msk (0xffu << GMAC_TI_ACNS_Pos) /**< \brief (GMAC_TI) Alternative Count Nanoseconds */
#define GMAC_TI_ACNS(value) ((GMAC_TI_ACNS_Msk & ((value) << GMAC_TI_ACNS_Pos)))
#define GMAC_TI_NIT_Pos 16
#define GMAC_TI_NIT_Msk (0xffu << GMAC_TI_NIT_Pos) /**< \brief (GMAC_TI) Number of Increments */
#define GMAC_TI_NIT(value) ((GMAC_TI_NIT_Msk & ((value) << GMAC_TI_NIT_Pos)))
/* -------- GMAC_EFTSL : (GMAC Offset: 0x1E0) PTP Event Frame Transmitted Seconds Low Register -------- */
#define GMAC_EFTSL_RUD_Pos 0
#define GMAC_EFTSL_RUD_Msk (0xffffffffu << GMAC_EFTSL_RUD_Pos) /**< \brief (GMAC_EFTSL) Register Update */
/* -------- GMAC_EFTN : (GMAC Offset: 0x1E4) PTP Event Frame Transmitted Nanoseconds Register -------- */
#define GMAC_EFTN_RUD_Pos 0
#define GMAC_EFTN_RUD_Msk (0x3fffffffu << GMAC_EFTN_RUD_Pos) /**< \brief (GMAC_EFTN) Register Update */
/* -------- GMAC_EFRSL : (GMAC Offset: 0x1E8) PTP Event Frame Received Seconds Low Register -------- */
#define GMAC_EFRSL_RUD_Pos 0
#define GMAC_EFRSL_RUD_Msk (0xffffffffu << GMAC_EFRSL_RUD_Pos) /**< \brief (GMAC_EFRSL) Register Update */
/* -------- GMAC_EFRN : (GMAC Offset: 0x1EC) PTP Event Frame Received Nanoseconds Register -------- */
#define GMAC_EFRN_RUD_Pos 0
#define GMAC_EFRN_RUD_Msk (0x3fffffffu << GMAC_EFRN_RUD_Pos) /**< \brief (GMAC_EFRN) Register Update */
/* -------- GMAC_PEFTSL : (GMAC Offset: 0x1F0) PTP Peer Event Frame Transmitted Seconds Low Register -------- */
#define GMAC_PEFTSL_RUD_Pos 0
#define GMAC_PEFTSL_RUD_Msk (0xffffffffu << GMAC_PEFTSL_RUD_Pos) /**< \brief (GMAC_PEFTSL) Register Update */
/* -------- GMAC_PEFTN : (GMAC Offset: 0x1F4) PTP Peer Event Frame Transmitted Nanoseconds Register -------- */
#define GMAC_PEFTN_RUD_Pos 0
#define GMAC_PEFTN_RUD_Msk (0x3fffffffu << GMAC_PEFTN_RUD_Pos) /**< \brief (GMAC_PEFTN) Register Update */
/* -------- GMAC_PEFRSL : (GMAC Offset: 0x1F8) PTP Peer Event Frame Received Seconds Low Register -------- */
#define GMAC_PEFRSL_RUD_Pos 0
#define GMAC_PEFRSL_RUD_Msk (0xffffffffu << GMAC_PEFRSL_RUD_Pos) /**< \brief (GMAC_PEFRSL) Register Update */
/* -------- GMAC_PEFRN : (GMAC Offset: 0x1FC) PTP Peer Event Frame Received Nanoseconds Register -------- */
#define GMAC_PEFRN_RUD_Pos 0
#define GMAC_PEFRN_RUD_Msk (0x3fffffffu << GMAC_PEFRN_RUD_Pos) /**< \brief (GMAC_PEFRN) Register Update */
/* -------- GMAC_RXLPI : (GMAC Offset: 0x270) Received LPI Transitions -------- */
#define GMAC_RXLPI_COUNT_Pos 0
#define GMAC_RXLPI_COUNT_Msk (0xffffu << GMAC_RXLPI_COUNT_Pos) /**< \brief (GMAC_RXLPI) Count of RX LPI transitions (cleared on read) */
/* -------- GMAC_RXLPITIME : (GMAC Offset: 0x274) Received LPI Time -------- */
#define GMAC_RXLPITIME_LPITIME_Pos 0
#define GMAC_RXLPITIME_LPITIME_Msk (0xffffffu << GMAC_RXLPITIME_LPITIME_Pos) /**< \brief (GMAC_RXLPITIME) Time in LPI (cleared on read) */
/* -------- GMAC_TXLPI : (GMAC Offset: 0x278) Transmit LPI Transitions -------- */
#define GMAC_TXLPI_COUNT_Pos 0
#define GMAC_TXLPI_COUNT_Msk (0xffffu << GMAC_TXLPI_COUNT_Pos) /**< \brief (GMAC_TXLPI) Count of LPI transitions (cleared on read) */
/* -------- GMAC_TXLPITIME : (GMAC Offset: 0x27C) Transmit LPI Time -------- */
#define GMAC_TXLPITIME_LPITIME_Pos 0
#define GMAC_TXLPITIME_LPITIME_Msk (0xffffffu << GMAC_TXLPITIME_LPITIME_Pos) /**< \brief (GMAC_TXLPITIME) Time in LPI (cleared on read) */
/* -------- GMAC_ISRPQ[5] : (GMAC Offset: 0x400) Interrupt Status Register Priority Queue  (index = 1) -------- */
#define GMAC_ISRPQ_RCOMP (0x1u << 1) /**< \brief (GMAC_ISRPQ[5]) Receive Complete */
#define GMAC_ISRPQ_RXUBR (0x1u << 2) /**< \brief (GMAC_ISRPQ[5]) RX Used Bit Read */
#define GMAC_ISRPQ_RLEX (0x1u << 5) /**< \brief (GMAC_ISRPQ[5]) Retry Limit Exceeded or Late Collision */
#define GMAC_ISRPQ_TFC (0x1u << 6) /**< \brief (GMAC_ISRPQ[5]) Transmit Frame Corruption Due to AHB Error */
#define GMAC_ISRPQ_TCOMP (0x1u << 7) /**< \brief (GMAC_ISRPQ[5]) Transmit Complete */
#define GMAC_ISRPQ_ROVR (0x1u << 10) /**< \brief (GMAC_ISRPQ[5]) Receive Overrun */
#define GMAC_ISRPQ_HRESP (0x1u << 11) /**< \brief (GMAC_ISRPQ[5]) HRESP Not OK */
/* -------- GMAC_TBQBAPQ[5] : (GMAC Offset: 0x440) Transmit Buffer Queue Base Address Register Priority Queue  (index = 1) -------- */
#define GMAC_TBQBAPQ_TXBQBA_Pos 2
#define GMAC_TBQBAPQ_TXBQBA_Msk (0x3fffffffu << GMAC_TBQBAPQ_TXBQBA_Pos) /**< \brief (GMAC_TBQBAPQ[5]) Transmit Buffer Queue Base Address */
#define GMAC_TBQBAPQ_TXBQBA(value) ((GMAC_TBQBAPQ_TXBQBA_Msk & ((value) << GMAC_TBQBAPQ_TXBQBA_Pos)))
/* -------- GMAC_RBQBAPQ[5] : (GMAC Offset: 0x480) Receive Buffer Queue Base Address Register Priority Queue  (index = 1) -------- */
#define GMAC_RBQBAPQ_RXBQBA_Pos 2
#define GMAC_RBQBAPQ_RXBQBA_Msk (0x3fffffffu << GMAC_RBQBAPQ_RXBQBA_Pos) /**< \brief (GMAC_RBQBAPQ[5]) Receive Buffer Queue Base Address */
#define GMAC_RBQBAPQ_RXBQBA(value) ((GMAC_RBQBAPQ_RXBQBA_Msk & ((value) << GMAC_RBQBAPQ_RXBQBA_Pos)))
/* -------- GMAC_RBSRPQ[5] : (GMAC Offset: 0x4A0) Receive Buffer Size Register Priority Queue  (index = 1) -------- */
#define GMAC_RBSRPQ_RBS_Pos 0
#define GMAC_RBSRPQ_RBS_Msk (0xffffu << GMAC_RBSRPQ_RBS_Pos) /**< \brief (GMAC_RBSRPQ[5]) Receive Buffer Size */
#define GMAC_RBSRPQ_RBS(value) ((GMAC_RBSRPQ_RBS_Msk & ((value) << GMAC_RBSRPQ_RBS_Pos)))
/* -------- GMAC_CBSCR : (GMAC Offset: 0x4BC) Credit-Based Shaping Control Register -------- */
#define GMAC_CBSCR_QBE (0x1u << 0) /**< \brief (GMAC_CBSCR) Queue B CBS Enable */
#define GMAC_CBSCR_QAE (0x1u << 1) /**< \brief (GMAC_CBSCR) Queue A CBS Enable */
/* -------- GMAC_CBSISQA : (GMAC Offset: 0x4C0) Credit-Based Shaping IdleSlope Register for Queue A -------- */
#define GMAC_CBSISQA_IS_Pos 0
#define GMAC_CBSISQA_IS_Msk (0xffffffffu << GMAC_CBSISQA_IS_Pos) /**< \brief (GMAC_CBSISQA) IdleSlope */
#define GMAC_CBSISQA_IS(value) ((GMAC_CBSISQA_IS_Msk & ((value) << GMAC_CBSISQA_IS_Pos)))
/* -------- GMAC_CBSISQB : (GMAC Offset: 0x4C4) Credit-Based Shaping IdleSlope Register for Queue B -------- */
#define GMAC_CBSISQB_IS_Pos 0
#define GMAC_CBSISQB_IS_Msk (0xffffffffu << GMAC_CBSISQB_IS_Pos) /**< \brief (GMAC_CBSISQB) IdleSlope */
#define GMAC_CBSISQB_IS(value) ((GMAC_CBSISQB_IS_Msk & ((value) << GMAC_CBSISQB_IS_Pos)))
/* -------- GMAC_ST1RPQ[4] : (GMAC Offset: 0x500) Screening Type 1 Register Priority Queue  (index = 0) -------- */
#define GMAC_ST1RPQ_QNB_Pos 0
#define GMAC_ST1RPQ_QNB_Msk (0x7u << GMAC_ST1RPQ_QNB_Pos) /**< \brief (GMAC_ST1RPQ[4]) Queue Number (0-5) */
#define GMAC_ST1RPQ_QNB(value) ((GMAC_ST1RPQ_QNB_Msk & ((value) << GMAC_ST1RPQ_QNB_Pos)))
#define GMAC_ST1RPQ_DSTCM_Pos 4
#define GMAC_ST1RPQ_DSTCM_Msk (0xffu << GMAC_ST1RPQ_DSTCM_Pos) /**< \brief (GMAC_ST1RPQ[4]) Differentiated Services or Traffic Class Match */
#define GMAC_ST1RPQ_DSTCM(value) ((GMAC_ST1RPQ_DSTCM_Msk & ((value) << GMAC_ST1RPQ_DSTCM_Pos)))
#define GMAC_ST1RPQ_UDPM_Pos 12
#define GMAC_ST1RPQ_UDPM_Msk (0xffffu << GMAC_ST1RPQ_UDPM_Pos) /**< \brief (GMAC_ST1RPQ[4]) UDP Port Match */
#define GMAC_ST1RPQ_UDPM(value) ((GMAC_ST1RPQ_UDPM_Msk & ((value) << GMAC_ST1RPQ_UDPM_Pos)))
#define GMAC_ST1RPQ_DSTCE (0x1u << 28) /**< \brief (GMAC_ST1RPQ[4]) Differentiated Services or Traffic Class Match Enable */
#define GMAC_ST1RPQ_UDPE (0x1u << 29) /**< \brief (GMAC_ST1RPQ[4]) UDP Port Match Enable */
/* -------- GMAC_ST2RPQ[8] : (GMAC Offset: 0x540) Screening Type 2 Register Priority Queue  (index = 0) -------- */
#define GMAC_ST2RPQ_QNB_Pos 0
#define GMAC_ST2RPQ_QNB_Msk (0x7u << GMAC_ST2RPQ_QNB_Pos) /**< \brief (GMAC_ST2RPQ[8]) Queue Number (0-5) */
#define GMAC_ST2RPQ_QNB(value) ((GMAC_ST2RPQ_QNB_Msk & ((value) << GMAC_ST2RPQ_QNB_Pos)))
#define GMAC_ST2RPQ_VLANP_Pos 4
#define GMAC_ST2RPQ_VLANP_Msk (0x7u << GMAC_ST2RPQ_VLANP_Pos) /**< \brief (GMAC_ST2RPQ[8]) VLAN Priority */
#define GMAC_ST2RPQ_VLANP(value) ((GMAC_ST2RPQ_VLANP_Msk & ((value) << GMAC_ST2RPQ_VLANP_Pos)))
#define GMAC_ST2RPQ_VLANE (0x1u << 8) /**< \brief (GMAC_ST2RPQ[8]) VLAN Enable */
#define GMAC_ST2RPQ_I2ETH_Pos 9
#define GMAC_ST2RPQ_I2ETH_Msk (0x7u << GMAC_ST2RPQ_I2ETH_Pos) /**< \brief (GMAC_ST2RPQ[8]) Index of Screening Type 2 EtherType register x */
#define GMAC_ST2RPQ_I2ETH(value) ((GMAC_ST2RPQ_I2ETH_Msk & ((value) << GMAC_ST2RPQ_I2ETH_Pos)))
#define GMAC_ST2RPQ_ETHE (0x1u << 12) /**< \brief (GMAC_ST2RPQ[8]) EtherType Enable */
#define GMAC_ST2RPQ_COMPA_Pos 13
#define GMAC_ST2RPQ_COMPA_Msk (0x1fu << GMAC_ST2RPQ_COMPA_Pos) /**< \brief (GMAC_ST2RPQ[8]) Index of Screening Type 2 Compare Word 0/Word 1 register x */
#define GMAC_ST2RPQ_COMPA(value) ((GMAC_ST2RPQ_COMPA_Msk & ((value) << GMAC_ST2RPQ_COMPA_Pos)))
#define GMAC_ST2RPQ_COMPAE (0x1u << 18) /**< \brief (GMAC_ST2RPQ[8]) Compare A Enable */
#define GMAC_ST2RPQ_COMPB_Pos 19
#define GMAC_ST2RPQ_COMPB_Msk (0x1fu << GMAC_ST2RPQ_COMPB_Pos) /**< \brief (GMAC_ST2RPQ[8]) Index of Screening Type 2 Compare Word 0/Word 1 register x */
#define GMAC_ST2RPQ_COMPB(value) ((GMAC_ST2RPQ_COMPB_Msk & ((value) << GMAC_ST2RPQ_COMPB_Pos)))
#define GMAC_ST2RPQ_COMPBE (0x1u << 24) /**< \brief (GMAC_ST2RPQ[8]) Compare B Enable */
#define GMAC_ST2RPQ_COMPC_Pos 25
#define GMAC_ST2RPQ_COMPC_Msk (0x1fu << GMAC_ST2RPQ_COMPC_Pos) /**< \brief (GMAC_ST2RPQ[8]) Index of Screening Type 2 Compare Word 0/Word 1 register x */
#define GMAC_ST2RPQ_COMPC(value) ((GMAC_ST2RPQ_COMPC_Msk & ((value) << GMAC_ST2RPQ_COMPC_Pos)))
#define GMAC_ST2RPQ_COMPCE (0x1u << 30) /**< \brief (GMAC_ST2RPQ[8]) Compare C Enable */
/* -------- GMAC_IERPQ[5] : (GMAC Offset: 0x5FC) Interrupt Enable Register Priority Queue  (index = 1) -------- */
#define GMAC_IERPQ_RCOMP (0x1u << 1) /**< \brief (GMAC_IERPQ[5]) Receive Complete */
#define GMAC_IERPQ_RXUBR (0x1u << 2) /**< \brief (GMAC_IERPQ[5]) RX Used Bit Read */
#define GMAC_IERPQ_RLEX (0x1u << 5) /**< \brief (GMAC_IERPQ[5]) Retry Limit Exceeded or Late Collision */
#define GMAC_IERPQ_TFC (0x1u << 6) /**< \brief (GMAC_IERPQ[5]) Transmit Frame Corruption Due to AHB Error */
#define GMAC_IERPQ_TCOMP (0x1u << 7) /**< \brief (GMAC_IERPQ[5]) Transmit Complete */
#define GMAC_IERPQ_ROVR (0x1u << 10) /**< \brief (GMAC_IERPQ[5]) Receive Overrun */
#define GMAC_IERPQ_HRESP (0x1u << 11) /**< \brief (GMAC_IERPQ[5]) HRESP Not OK */
/* -------- GMAC_IDRPQ[5] : (GMAC Offset: 0x61C) Interrupt Disable Register Priority Queue  (index = 1) -------- */
#define GMAC_IDRPQ_RCOMP (0x1u << 1) /**< \brief (GMAC_IDRPQ[5]) Receive Complete */
#define GMAC_IDRPQ_RXUBR (0x1u << 2) /**< \brief (GMAC_IDRPQ[5]) RX Used Bit Read */
#define GMAC_IDRPQ_RLEX (0x1u << 5) /**< \brief (GMAC_IDRPQ[5]) Retry Limit Exceeded or Late Collision */
#define GMAC_IDRPQ_TFC (0x1u << 6) /**< \brief (GMAC_IDRPQ[5]) Transmit Frame Corruption Due to AHB Error */
#define GMAC_IDRPQ_TCOMP (0x1u << 7) /**< \brief (GMAC_IDRPQ[5]) Transmit Complete */
#define GMAC_IDRPQ_ROVR (0x1u << 10) /**< \brief (GMAC_IDRPQ[5]) Receive Overrun */
#define GMAC_IDRPQ_HRESP (0x1u << 11) /**< \brief (GMAC_IDRPQ[5]) HRESP Not OK */
/* -------- GMAC_IMRPQ[5] : (GMAC Offset: 0x63C) Interrupt Mask Register Priority Queue  (index = 1) -------- */
#define GMAC_IMRPQ_RCOMP (0x1u << 1) /**< \brief (GMAC_IMRPQ[5]) Receive Complete */
#define GMAC_IMRPQ_RXUBR (0x1u << 2) /**< \brief (GMAC_IMRPQ[5]) RX Used Bit Read */
#define GMAC_IMRPQ_RLEX (0x1u << 5) /**< \brief (GMAC_IMRPQ[5]) Retry Limit Exceeded or Late Collision */
#define GMAC_IMRPQ_AHB (0x1u << 6) /**< \brief (GMAC_IMRPQ[5]) AHB Error */
#define GMAC_IMRPQ_TCOMP (0x1u << 7) /**< \brief (GMAC_IMRPQ[5]) Transmit Complete */
#define GMAC_IMRPQ_ROVR (0x1u << 10) /**< \brief (GMAC_IMRPQ[5]) Receive Overrun */
#define GMAC_IMRPQ_HRESP (0x1u << 11) /**< \brief (GMAC_IMRPQ[5]) HRESP Not OK */
/* -------- GMAC_ST2ER[4] : (GMAC Offset: 0x6E0) Screening Type 2 Ethertype Register  (index = 0) -------- */
#define GMAC_ST2ER_COMPVAL_Pos 0
#define GMAC_ST2ER_COMPVAL_Msk (0xffffu << GMAC_ST2ER_COMPVAL_Pos) /**< \brief (GMAC_ST2ER[4]) Ethertype Compare Value */
#define GMAC_ST2ER_COMPVAL(value) ((GMAC_ST2ER_COMPVAL_Msk & ((value) << GMAC_ST2ER_COMPVAL_Pos)))
/* -------- GMAC_ST2CW00 : (GMAC Offset: 0x700) Screening Type 2 Compare Word 0 Register  (index = 0) -------- */
#define GMAC_ST2CW00_MASKVAL_Pos 0
#define GMAC_ST2CW00_MASKVAL_Msk (0xffffu << GMAC_ST2CW00_MASKVAL_Pos) /**< \brief (GMAC_ST2CW00) Mask Value */
#define GMAC_ST2CW00_MASKVAL(value) ((GMAC_ST2CW00_MASKVAL_Msk & ((value) << GMAC_ST2CW00_MASKVAL_Pos)))
#define GMAC_ST2CW00_COMPVAL_Pos 16
#define GMAC_ST2CW00_COMPVAL_Msk (0xffffu << GMAC_ST2CW00_COMPVAL_Pos) /**< \brief (GMAC_ST2CW00) Compare Value */
#define GMAC_ST2CW00_COMPVAL(value) ((GMAC_ST2CW00_COMPVAL_Msk & ((value) << GMAC_ST2CW00_COMPVAL_Pos)))
/* -------- GMAC_ST2CW10 : (GMAC Offset: 0x704) Screening Type 2 Compare Word 1 Register  (index = 0) -------- */
#define GMAC_ST2CW10_OFFSVAL_Pos 0
#define GMAC_ST2CW10_OFFSVAL_Msk (0x7fu << GMAC_ST2CW10_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW10) Offset Value in Bytes */
#define GMAC_ST2CW10_OFFSVAL(value) ((GMAC_ST2CW10_OFFSVAL_Msk & ((value) << GMAC_ST2CW10_OFFSVAL_Pos)))
#define GMAC_ST2CW10_OFFSSTRT_Pos 7
#define GMAC_ST2CW10_OFFSSTRT_Msk (0x3u << GMAC_ST2CW10_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW10) Ethernet Frame Offset Start */
#define GMAC_ST2CW10_OFFSSTRT(value) ((GMAC_ST2CW10_OFFSSTRT_Msk & ((value) << GMAC_ST2CW10_OFFSSTRT_Pos)))
#define   GMAC_ST2CW10_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW10) Offset from the start of the frame */
#define   GMAC_ST2CW10_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW10) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW10_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW10) Offset from the byte after the IP header field */
#define   GMAC_ST2CW10_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW10) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW01 : (GMAC Offset: 0x708) Screening Type 2 Compare Word 0 Register  (index = 1) -------- */
#define GMAC_ST2CW01_MASKVAL_Pos 0
#define GMAC_ST2CW01_MASKVAL_Msk (0xffffu << GMAC_ST2CW01_MASKVAL_Pos) /**< \brief (GMAC_ST2CW01) Mask Value */
#define GMAC_ST2CW01_MASKVAL(value) ((GMAC_ST2CW01_MASKVAL_Msk & ((value) << GMAC_ST2CW01_MASKVAL_Pos)))
#define GMAC_ST2CW01_COMPVAL_Pos 16
#define GMAC_ST2CW01_COMPVAL_Msk (0xffffu << GMAC_ST2CW01_COMPVAL_Pos) /**< \brief (GMAC_ST2CW01) Compare Value */
#define GMAC_ST2CW01_COMPVAL(value) ((GMAC_ST2CW01_COMPVAL_Msk & ((value) << GMAC_ST2CW01_COMPVAL_Pos)))
/* -------- GMAC_ST2CW11 : (GMAC Offset: 0x70C) Screening Type 2 Compare Word 1 Register  (index = 1) -------- */
#define GMAC_ST2CW11_OFFSVAL_Pos 0
#define GMAC_ST2CW11_OFFSVAL_Msk (0x7fu << GMAC_ST2CW11_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW11) Offset Value in Bytes */
#define GMAC_ST2CW11_OFFSVAL(value) ((GMAC_ST2CW11_OFFSVAL_Msk & ((value) << GMAC_ST2CW11_OFFSVAL_Pos)))
#define GMAC_ST2CW11_OFFSSTRT_Pos 7
#define GMAC_ST2CW11_OFFSSTRT_Msk (0x3u << GMAC_ST2CW11_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW11) Ethernet Frame Offset Start */
#define GMAC_ST2CW11_OFFSSTRT(value) ((GMAC_ST2CW11_OFFSSTRT_Msk & ((value) << GMAC_ST2CW11_OFFSSTRT_Pos)))
#define   GMAC_ST2CW11_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW11) Offset from the start of the frame */
#define   GMAC_ST2CW11_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW11) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW11_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW11) Offset from the byte after the IP header field */
#define   GMAC_ST2CW11_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW11) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW02 : (GMAC Offset: 0x710) Screening Type 2 Compare Word 0 Register  (index = 2) -------- */
#define GMAC_ST2CW02_MASKVAL_Pos 0
#define GMAC_ST2CW02_MASKVAL_Msk (0xffffu << GMAC_ST2CW02_MASKVAL_Pos) /**< \brief (GMAC_ST2CW02) Mask Value */
#define GMAC_ST2CW02_MASKVAL(value) ((GMAC_ST2CW02_MASKVAL_Msk & ((value) << GMAC_ST2CW02_MASKVAL_Pos)))
#define GMAC_ST2CW02_COMPVAL_Pos 16
#define GMAC_ST2CW02_COMPVAL_Msk (0xffffu << GMAC_ST2CW02_COMPVAL_Pos) /**< \brief (GMAC_ST2CW02) Compare Value */
#define GMAC_ST2CW02_COMPVAL(value) ((GMAC_ST2CW02_COMPVAL_Msk & ((value) << GMAC_ST2CW02_COMPVAL_Pos)))
/* -------- GMAC_ST2CW12 : (GMAC Offset: 0x714) Screening Type 2 Compare Word 1 Register  (index = 2) -------- */
#define GMAC_ST2CW12_OFFSVAL_Pos 0
#define GMAC_ST2CW12_OFFSVAL_Msk (0x7fu << GMAC_ST2CW12_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW12) Offset Value in Bytes */
#define GMAC_ST2CW12_OFFSVAL(value) ((GMAC_ST2CW12_OFFSVAL_Msk & ((value) << GMAC_ST2CW12_OFFSVAL_Pos)))
#define GMAC_ST2CW12_OFFSSTRT_Pos 7
#define GMAC_ST2CW12_OFFSSTRT_Msk (0x3u << GMAC_ST2CW12_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW12) Ethernet Frame Offset Start */
#define GMAC_ST2CW12_OFFSSTRT(value) ((GMAC_ST2CW12_OFFSSTRT_Msk & ((value) << GMAC_ST2CW12_OFFSSTRT_Pos)))
#define   GMAC_ST2CW12_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW12) Offset from the start of the frame */
#define   GMAC_ST2CW12_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW12) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW12_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW12) Offset from the byte after the IP header field */
#define   GMAC_ST2CW12_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW12) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW03 : (GMAC Offset: 0x718) Screening Type 2 Compare Word 0 Register  (index = 3) -------- */
#define GMAC_ST2CW03_MASKVAL_Pos 0
#define GMAC_ST2CW03_MASKVAL_Msk (0xffffu << GMAC_ST2CW03_MASKVAL_Pos) /**< \brief (GMAC_ST2CW03) Mask Value */
#define GMAC_ST2CW03_MASKVAL(value) ((GMAC_ST2CW03_MASKVAL_Msk & ((value) << GMAC_ST2CW03_MASKVAL_Pos)))
#define GMAC_ST2CW03_COMPVAL_Pos 16
#define GMAC_ST2CW03_COMPVAL_Msk (0xffffu << GMAC_ST2CW03_COMPVAL_Pos) /**< \brief (GMAC_ST2CW03) Compare Value */
#define GMAC_ST2CW03_COMPVAL(value) ((GMAC_ST2CW03_COMPVAL_Msk & ((value) << GMAC_ST2CW03_COMPVAL_Pos)))
/* -------- GMAC_ST2CW13 : (GMAC Offset: 0x71C) Screening Type 2 Compare Word 1 Register  (index = 3) -------- */
#define GMAC_ST2CW13_OFFSVAL_Pos 0
#define GMAC_ST2CW13_OFFSVAL_Msk (0x7fu << GMAC_ST2CW13_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW13) Offset Value in Bytes */
#define GMAC_ST2CW13_OFFSVAL(value) ((GMAC_ST2CW13_OFFSVAL_Msk & ((value) << GMAC_ST2CW13_OFFSVAL_Pos)))
#define GMAC_ST2CW13_OFFSSTRT_Pos 7
#define GMAC_ST2CW13_OFFSSTRT_Msk (0x3u << GMAC_ST2CW13_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW13) Ethernet Frame Offset Start */
#define GMAC_ST2CW13_OFFSSTRT(value) ((GMAC_ST2CW13_OFFSSTRT_Msk & ((value) << GMAC_ST2CW13_OFFSSTRT_Pos)))
#define   GMAC_ST2CW13_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW13) Offset from the start of the frame */
#define   GMAC_ST2CW13_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW13) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW13_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW13) Offset from the byte after the IP header field */
#define   GMAC_ST2CW13_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW13) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW04 : (GMAC Offset: 0x720) Screening Type 2 Compare Word 0 Register  (index = 4) -------- */
#define GMAC_ST2CW04_MASKVAL_Pos 0
#define GMAC_ST2CW04_MASKVAL_Msk (0xffffu << GMAC_ST2CW04_MASKVAL_Pos) /**< \brief (GMAC_ST2CW04) Mask Value */
#define GMAC_ST2CW04_MASKVAL(value) ((GMAC_ST2CW04_MASKVAL_Msk & ((value) << GMAC_ST2CW04_MASKVAL_Pos)))
#define GMAC_ST2CW04_COMPVAL_Pos 16
#define GMAC_ST2CW04_COMPVAL_Msk (0xffffu << GMAC_ST2CW04_COMPVAL_Pos) /**< \brief (GMAC_ST2CW04) Compare Value */
#define GMAC_ST2CW04_COMPVAL(value) ((GMAC_ST2CW04_COMPVAL_Msk & ((value) << GMAC_ST2CW04_COMPVAL_Pos)))
/* -------- GMAC_ST2CW14 : (GMAC Offset: 0x724) Screening Type 2 Compare Word 1 Register  (index = 4) -------- */
#define GMAC_ST2CW14_OFFSVAL_Pos 0
#define GMAC_ST2CW14_OFFSVAL_Msk (0x7fu << GMAC_ST2CW14_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW14) Offset Value in Bytes */
#define GMAC_ST2CW14_OFFSVAL(value) ((GMAC_ST2CW14_OFFSVAL_Msk & ((value) << GMAC_ST2CW14_OFFSVAL_Pos)))
#define GMAC_ST2CW14_OFFSSTRT_Pos 7
#define GMAC_ST2CW14_OFFSSTRT_Msk (0x3u << GMAC_ST2CW14_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW14) Ethernet Frame Offset Start */
#define GMAC_ST2CW14_OFFSSTRT(value) ((GMAC_ST2CW14_OFFSSTRT_Msk & ((value) << GMAC_ST2CW14_OFFSSTRT_Pos)))
#define   GMAC_ST2CW14_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW14) Offset from the start of the frame */
#define   GMAC_ST2CW14_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW14) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW14_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW14) Offset from the byte after the IP header field */
#define   GMAC_ST2CW14_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW14) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW05 : (GMAC Offset: 0x728) Screening Type 2 Compare Word 0 Register  (index = 5) -------- */
#define GMAC_ST2CW05_MASKVAL_Pos 0
#define GMAC_ST2CW05_MASKVAL_Msk (0xffffu << GMAC_ST2CW05_MASKVAL_Pos) /**< \brief (GMAC_ST2CW05) Mask Value */
#define GMAC_ST2CW05_MASKVAL(value) ((GMAC_ST2CW05_MASKVAL_Msk & ((value) << GMAC_ST2CW05_MASKVAL_Pos)))
#define GMAC_ST2CW05_COMPVAL_Pos 16
#define GMAC_ST2CW05_COMPVAL_Msk (0xffffu << GMAC_ST2CW05_COMPVAL_Pos) /**< \brief (GMAC_ST2CW05) Compare Value */
#define GMAC_ST2CW05_COMPVAL(value) ((GMAC_ST2CW05_COMPVAL_Msk & ((value) << GMAC_ST2CW05_COMPVAL_Pos)))
/* -------- GMAC_ST2CW15 : (GMAC Offset: 0x72C) Screening Type 2 Compare Word 1 Register  (index = 5) -------- */
#define GMAC_ST2CW15_OFFSVAL_Pos 0
#define GMAC_ST2CW15_OFFSVAL_Msk (0x7fu << GMAC_ST2CW15_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW15) Offset Value in Bytes */
#define GMAC_ST2CW15_OFFSVAL(value) ((GMAC_ST2CW15_OFFSVAL_Msk & ((value) << GMAC_ST2CW15_OFFSVAL_Pos)))
#define GMAC_ST2CW15_OFFSSTRT_Pos 7
#define GMAC_ST2CW15_OFFSSTRT_Msk (0x3u << GMAC_ST2CW15_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW15) Ethernet Frame Offset Start */
#define GMAC_ST2CW15_OFFSSTRT(value) ((GMAC_ST2CW15_OFFSSTRT_Msk & ((value) << GMAC_ST2CW15_OFFSSTRT_Pos)))
#define   GMAC_ST2CW15_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW15) Offset from the start of the frame */
#define   GMAC_ST2CW15_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW15) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW15_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW15) Offset from the byte after the IP header field */
#define   GMAC_ST2CW15_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW15) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW06 : (GMAC Offset: 0x730) Screening Type 2 Compare Word 0 Register  (index = 6) -------- */
#define GMAC_ST2CW06_MASKVAL_Pos 0
#define GMAC_ST2CW06_MASKVAL_Msk (0xffffu << GMAC_ST2CW06_MASKVAL_Pos) /**< \brief (GMAC_ST2CW06) Mask Value */
#define GMAC_ST2CW06_MASKVAL(value) ((GMAC_ST2CW06_MASKVAL_Msk & ((value) << GMAC_ST2CW06_MASKVAL_Pos)))
#define GMAC_ST2CW06_COMPVAL_Pos 16
#define GMAC_ST2CW06_COMPVAL_Msk (0xffffu << GMAC_ST2CW06_COMPVAL_Pos) /**< \brief (GMAC_ST2CW06) Compare Value */
#define GMAC_ST2CW06_COMPVAL(value) ((GMAC_ST2CW06_COMPVAL_Msk & ((value) << GMAC_ST2CW06_COMPVAL_Pos)))
/* -------- GMAC_ST2CW16 : (GMAC Offset: 0x734) Screening Type 2 Compare Word 1 Register  (index = 6) -------- */
#define GMAC_ST2CW16_OFFSVAL_Pos 0
#define GMAC_ST2CW16_OFFSVAL_Msk (0x7fu << GMAC_ST2CW16_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW16) Offset Value in Bytes */
#define GMAC_ST2CW16_OFFSVAL(value) ((GMAC_ST2CW16_OFFSVAL_Msk & ((value) << GMAC_ST2CW16_OFFSVAL_Pos)))
#define GMAC_ST2CW16_OFFSSTRT_Pos 7
#define GMAC_ST2CW16_OFFSSTRT_Msk (0x3u << GMAC_ST2CW16_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW16) Ethernet Frame Offset Start */
#define GMAC_ST2CW16_OFFSSTRT(value) ((GMAC_ST2CW16_OFFSSTRT_Msk & ((value) << GMAC_ST2CW16_OFFSSTRT_Pos)))
#define   GMAC_ST2CW16_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW16) Offset from the start of the frame */
#define   GMAC_ST2CW16_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW16) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW16_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW16) Offset from the byte after the IP header field */
#define   GMAC_ST2CW16_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW16) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW07 : (GMAC Offset: 0x738) Screening Type 2 Compare Word 0 Register  (index = 7) -------- */
#define GMAC_ST2CW07_MASKVAL_Pos 0
#define GMAC_ST2CW07_MASKVAL_Msk (0xffffu << GMAC_ST2CW07_MASKVAL_Pos) /**< \brief (GMAC_ST2CW07) Mask Value */
#define GMAC_ST2CW07_MASKVAL(value) ((GMAC_ST2CW07_MASKVAL_Msk & ((value) << GMAC_ST2CW07_MASKVAL_Pos)))
#define GMAC_ST2CW07_COMPVAL_Pos 16
#define GMAC_ST2CW07_COMPVAL_Msk (0xffffu << GMAC_ST2CW07_COMPVAL_Pos) /**< \brief (GMAC_ST2CW07) Compare Value */
#define GMAC_ST2CW07_COMPVAL(value) ((GMAC_ST2CW07_COMPVAL_Msk & ((value) << GMAC_ST2CW07_COMPVAL_Pos)))
/* -------- GMAC_ST2CW17 : (GMAC Offset: 0x73C) Screening Type 2 Compare Word 1 Register  (index = 7) -------- */
#define GMAC_ST2CW17_OFFSVAL_Pos 0
#define GMAC_ST2CW17_OFFSVAL_Msk (0x7fu << GMAC_ST2CW17_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW17) Offset Value in Bytes */
#define GMAC_ST2CW17_OFFSVAL(value) ((GMAC_ST2CW17_OFFSVAL_Msk & ((value) << GMAC_ST2CW17_OFFSVAL_Pos)))
#define GMAC_ST2CW17_OFFSSTRT_Pos 7
#define GMAC_ST2CW17_OFFSSTRT_Msk (0x3u << GMAC_ST2CW17_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW17) Ethernet Frame Offset Start */
#define GMAC_ST2CW17_OFFSSTRT(value) ((GMAC_ST2CW17_OFFSSTRT_Msk & ((value) << GMAC_ST2CW17_OFFSSTRT_Pos)))
#define   GMAC_ST2CW17_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW17) Offset from the start of the frame */
#define   GMAC_ST2CW17_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW17) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW17_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW17) Offset from the byte after the IP header field */
#define   GMAC_ST2CW17_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW17) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW08 : (GMAC Offset: 0x740) Screening Type 2 Compare Word 0 Register  (index = 8) -------- */
#define GMAC_ST2CW08_MASKVAL_Pos 0
#define GMAC_ST2CW08_MASKVAL_Msk (0xffffu << GMAC_ST2CW08_MASKVAL_Pos) /**< \brief (GMAC_ST2CW08) Mask Value */
#define GMAC_ST2CW08_MASKVAL(value) ((GMAC_ST2CW08_MASKVAL_Msk & ((value) << GMAC_ST2CW08_MASKVAL_Pos)))
#define GMAC_ST2CW08_COMPVAL_Pos 16
#define GMAC_ST2CW08_COMPVAL_Msk (0xffffu << GMAC_ST2CW08_COMPVAL_Pos) /**< \brief (GMAC_ST2CW08) Compare Value */
#define GMAC_ST2CW08_COMPVAL(value) ((GMAC_ST2CW08_COMPVAL_Msk & ((value) << GMAC_ST2CW08_COMPVAL_Pos)))
/* -------- GMAC_ST2CW18 : (GMAC Offset: 0x744) Screening Type 2 Compare Word 1 Register  (index = 8) -------- */
#define GMAC_ST2CW18_OFFSVAL_Pos 0
#define GMAC_ST2CW18_OFFSVAL_Msk (0x7fu << GMAC_ST2CW18_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW18) Offset Value in Bytes */
#define GMAC_ST2CW18_OFFSVAL(value) ((GMAC_ST2CW18_OFFSVAL_Msk & ((value) << GMAC_ST2CW18_OFFSVAL_Pos)))
#define GMAC_ST2CW18_OFFSSTRT_Pos 7
#define GMAC_ST2CW18_OFFSSTRT_Msk (0x3u << GMAC_ST2CW18_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW18) Ethernet Frame Offset Start */
#define GMAC_ST2CW18_OFFSSTRT(value) ((GMAC_ST2CW18_OFFSSTRT_Msk & ((value) << GMAC_ST2CW18_OFFSSTRT_Pos)))
#define   GMAC_ST2CW18_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW18) Offset from the start of the frame */
#define   GMAC_ST2CW18_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW18) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW18_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW18) Offset from the byte after the IP header field */
#define   GMAC_ST2CW18_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW18) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW09 : (GMAC Offset: 0x748) Screening Type 2 Compare Word 0 Register  (index = 9) -------- */
#define GMAC_ST2CW09_MASKVAL_Pos 0
#define GMAC_ST2CW09_MASKVAL_Msk (0xffffu << GMAC_ST2CW09_MASKVAL_Pos) /**< \brief (GMAC_ST2CW09) Mask Value */
#define GMAC_ST2CW09_MASKVAL(value) ((GMAC_ST2CW09_MASKVAL_Msk & ((value) << GMAC_ST2CW09_MASKVAL_Pos)))
#define GMAC_ST2CW09_COMPVAL_Pos 16
#define GMAC_ST2CW09_COMPVAL_Msk (0xffffu << GMAC_ST2CW09_COMPVAL_Pos) /**< \brief (GMAC_ST2CW09) Compare Value */
#define GMAC_ST2CW09_COMPVAL(value) ((GMAC_ST2CW09_COMPVAL_Msk & ((value) << GMAC_ST2CW09_COMPVAL_Pos)))
/* -------- GMAC_ST2CW19 : (GMAC Offset: 0x74C) Screening Type 2 Compare Word 1 Register  (index = 9) -------- */
#define GMAC_ST2CW19_OFFSVAL_Pos 0
#define GMAC_ST2CW19_OFFSVAL_Msk (0x7fu << GMAC_ST2CW19_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW19) Offset Value in Bytes */
#define GMAC_ST2CW19_OFFSVAL(value) ((GMAC_ST2CW19_OFFSVAL_Msk & ((value) << GMAC_ST2CW19_OFFSVAL_Pos)))
#define GMAC_ST2CW19_OFFSSTRT_Pos 7
#define GMAC_ST2CW19_OFFSSTRT_Msk (0x3u << GMAC_ST2CW19_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW19) Ethernet Frame Offset Start */
#define GMAC_ST2CW19_OFFSSTRT(value) ((GMAC_ST2CW19_OFFSSTRT_Msk & ((value) << GMAC_ST2CW19_OFFSSTRT_Pos)))
#define   GMAC_ST2CW19_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW19) Offset from the start of the frame */
#define   GMAC_ST2CW19_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW19) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW19_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW19) Offset from the byte after the IP header field */
#define   GMAC_ST2CW19_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW19) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW010 : (GMAC Offset: 0x750) Screening Type 2 Compare Word 0 Register  (index = 10) -------- */
#define GMAC_ST2CW010_MASKVAL_Pos 0
#define GMAC_ST2CW010_MASKVAL_Msk (0xffffu << GMAC_ST2CW010_MASKVAL_Pos) /**< \brief (GMAC_ST2CW010) Mask Value */
#define GMAC_ST2CW010_MASKVAL(value) ((GMAC_ST2CW010_MASKVAL_Msk & ((value) << GMAC_ST2CW010_MASKVAL_Pos)))
#define GMAC_ST2CW010_COMPVAL_Pos 16
#define GMAC_ST2CW010_COMPVAL_Msk (0xffffu << GMAC_ST2CW010_COMPVAL_Pos) /**< \brief (GMAC_ST2CW010) Compare Value */
#define GMAC_ST2CW010_COMPVAL(value) ((GMAC_ST2CW010_COMPVAL_Msk & ((value) << GMAC_ST2CW010_COMPVAL_Pos)))
/* -------- GMAC_ST2CW110 : (GMAC Offset: 0x754) Screening Type 2 Compare Word 1 Register  (index = 10) -------- */
#define GMAC_ST2CW110_OFFSVAL_Pos 0
#define GMAC_ST2CW110_OFFSVAL_Msk (0x7fu << GMAC_ST2CW110_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW110) Offset Value in Bytes */
#define GMAC_ST2CW110_OFFSVAL(value) ((GMAC_ST2CW110_OFFSVAL_Msk & ((value) << GMAC_ST2CW110_OFFSVAL_Pos)))
#define GMAC_ST2CW110_OFFSSTRT_Pos 7
#define GMAC_ST2CW110_OFFSSTRT_Msk (0x3u << GMAC_ST2CW110_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW110) Ethernet Frame Offset Start */
#define GMAC_ST2CW110_OFFSSTRT(value) ((GMAC_ST2CW110_OFFSSTRT_Msk & ((value) << GMAC_ST2CW110_OFFSSTRT_Pos)))
#define   GMAC_ST2CW110_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW110) Offset from the start of the frame */
#define   GMAC_ST2CW110_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW110) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW110_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW110) Offset from the byte after the IP header field */
#define   GMAC_ST2CW110_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW110) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW011 : (GMAC Offset: 0x758) Screening Type 2 Compare Word 0 Register  (index = 11) -------- */
#define GMAC_ST2CW011_MASKVAL_Pos 0
#define GMAC_ST2CW011_MASKVAL_Msk (0xffffu << GMAC_ST2CW011_MASKVAL_Pos) /**< \brief (GMAC_ST2CW011) Mask Value */
#define GMAC_ST2CW011_MASKVAL(value) ((GMAC_ST2CW011_MASKVAL_Msk & ((value) << GMAC_ST2CW011_MASKVAL_Pos)))
#define GMAC_ST2CW011_COMPVAL_Pos 16
#define GMAC_ST2CW011_COMPVAL_Msk (0xffffu << GMAC_ST2CW011_COMPVAL_Pos) /**< \brief (GMAC_ST2CW011) Compare Value */
#define GMAC_ST2CW011_COMPVAL(value) ((GMAC_ST2CW011_COMPVAL_Msk & ((value) << GMAC_ST2CW011_COMPVAL_Pos)))
/* -------- GMAC_ST2CW111 : (GMAC Offset: 0x75C) Screening Type 2 Compare Word 1 Register  (index = 11) -------- */
#define GMAC_ST2CW111_OFFSVAL_Pos 0
#define GMAC_ST2CW111_OFFSVAL_Msk (0x7fu << GMAC_ST2CW111_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW111) Offset Value in Bytes */
#define GMAC_ST2CW111_OFFSVAL(value) ((GMAC_ST2CW111_OFFSVAL_Msk & ((value) << GMAC_ST2CW111_OFFSVAL_Pos)))
#define GMAC_ST2CW111_OFFSSTRT_Pos 7
#define GMAC_ST2CW111_OFFSSTRT_Msk (0x3u << GMAC_ST2CW111_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW111) Ethernet Frame Offset Start */
#define GMAC_ST2CW111_OFFSSTRT(value) ((GMAC_ST2CW111_OFFSSTRT_Msk & ((value) << GMAC_ST2CW111_OFFSSTRT_Pos)))
#define   GMAC_ST2CW111_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW111) Offset from the start of the frame */
#define   GMAC_ST2CW111_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW111) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW111_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW111) Offset from the byte after the IP header field */
#define   GMAC_ST2CW111_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW111) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW012 : (GMAC Offset: 0x760) Screening Type 2 Compare Word 0 Register  (index = 12) -------- */
#define GMAC_ST2CW012_MASKVAL_Pos 0
#define GMAC_ST2CW012_MASKVAL_Msk (0xffffu << GMAC_ST2CW012_MASKVAL_Pos) /**< \brief (GMAC_ST2CW012) Mask Value */
#define GMAC_ST2CW012_MASKVAL(value) ((GMAC_ST2CW012_MASKVAL_Msk & ((value) << GMAC_ST2CW012_MASKVAL_Pos)))
#define GMAC_ST2CW012_COMPVAL_Pos 16
#define GMAC_ST2CW012_COMPVAL_Msk (0xffffu << GMAC_ST2CW012_COMPVAL_Pos) /**< \brief (GMAC_ST2CW012) Compare Value */
#define GMAC_ST2CW012_COMPVAL(value) ((GMAC_ST2CW012_COMPVAL_Msk & ((value) << GMAC_ST2CW012_COMPVAL_Pos)))
/* -------- GMAC_ST2CW112 : (GMAC Offset: 0x764) Screening Type 2 Compare Word 1 Register  (index = 12) -------- */
#define GMAC_ST2CW112_OFFSVAL_Pos 0
#define GMAC_ST2CW112_OFFSVAL_Msk (0x7fu << GMAC_ST2CW112_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW112) Offset Value in Bytes */
#define GMAC_ST2CW112_OFFSVAL(value) ((GMAC_ST2CW112_OFFSVAL_Msk & ((value) << GMAC_ST2CW112_OFFSVAL_Pos)))
#define GMAC_ST2CW112_OFFSSTRT_Pos 7
#define GMAC_ST2CW112_OFFSSTRT_Msk (0x3u << GMAC_ST2CW112_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW112) Ethernet Frame Offset Start */
#define GMAC_ST2CW112_OFFSSTRT(value) ((GMAC_ST2CW112_OFFSSTRT_Msk & ((value) << GMAC_ST2CW112_OFFSSTRT_Pos)))
#define   GMAC_ST2CW112_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW112) Offset from the start of the frame */
#define   GMAC_ST2CW112_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW112) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW112_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW112) Offset from the byte after the IP header field */
#define   GMAC_ST2CW112_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW112) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW013 : (GMAC Offset: 0x768) Screening Type 2 Compare Word 0 Register  (index = 13) -------- */
#define GMAC_ST2CW013_MASKVAL_Pos 0
#define GMAC_ST2CW013_MASKVAL_Msk (0xffffu << GMAC_ST2CW013_MASKVAL_Pos) /**< \brief (GMAC_ST2CW013) Mask Value */
#define GMAC_ST2CW013_MASKVAL(value) ((GMAC_ST2CW013_MASKVAL_Msk & ((value) << GMAC_ST2CW013_MASKVAL_Pos)))
#define GMAC_ST2CW013_COMPVAL_Pos 16
#define GMAC_ST2CW013_COMPVAL_Msk (0xffffu << GMAC_ST2CW013_COMPVAL_Pos) /**< \brief (GMAC_ST2CW013) Compare Value */
#define GMAC_ST2CW013_COMPVAL(value) ((GMAC_ST2CW013_COMPVAL_Msk & ((value) << GMAC_ST2CW013_COMPVAL_Pos)))
/* -------- GMAC_ST2CW113 : (GMAC Offset: 0x76C) Screening Type 2 Compare Word 1 Register  (index = 13) -------- */
#define GMAC_ST2CW113_OFFSVAL_Pos 0
#define GMAC_ST2CW113_OFFSVAL_Msk (0x7fu << GMAC_ST2CW113_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW113) Offset Value in Bytes */
#define GMAC_ST2CW113_OFFSVAL(value) ((GMAC_ST2CW113_OFFSVAL_Msk & ((value) << GMAC_ST2CW113_OFFSVAL_Pos)))
#define GMAC_ST2CW113_OFFSSTRT_Pos 7
#define GMAC_ST2CW113_OFFSSTRT_Msk (0x3u << GMAC_ST2CW113_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW113) Ethernet Frame Offset Start */
#define GMAC_ST2CW113_OFFSSTRT(value) ((GMAC_ST2CW113_OFFSSTRT_Msk & ((value) << GMAC_ST2CW113_OFFSSTRT_Pos)))
#define   GMAC_ST2CW113_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW113) Offset from the start of the frame */
#define   GMAC_ST2CW113_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW113) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW113_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW113) Offset from the byte after the IP header field */
#define   GMAC_ST2CW113_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW113) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW014 : (GMAC Offset: 0x770) Screening Type 2 Compare Word 0 Register  (index = 14) -------- */
#define GMAC_ST2CW014_MASKVAL_Pos 0
#define GMAC_ST2CW014_MASKVAL_Msk (0xffffu << GMAC_ST2CW014_MASKVAL_Pos) /**< \brief (GMAC_ST2CW014) Mask Value */
#define GMAC_ST2CW014_MASKVAL(value) ((GMAC_ST2CW014_MASKVAL_Msk & ((value) << GMAC_ST2CW014_MASKVAL_Pos)))
#define GMAC_ST2CW014_COMPVAL_Pos 16
#define GMAC_ST2CW014_COMPVAL_Msk (0xffffu << GMAC_ST2CW014_COMPVAL_Pos) /**< \brief (GMAC_ST2CW014) Compare Value */
#define GMAC_ST2CW014_COMPVAL(value) ((GMAC_ST2CW014_COMPVAL_Msk & ((value) << GMAC_ST2CW014_COMPVAL_Pos)))
/* -------- GMAC_ST2CW114 : (GMAC Offset: 0x774) Screening Type 2 Compare Word 1 Register  (index = 14) -------- */
#define GMAC_ST2CW114_OFFSVAL_Pos 0
#define GMAC_ST2CW114_OFFSVAL_Msk (0x7fu << GMAC_ST2CW114_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW114) Offset Value in Bytes */
#define GMAC_ST2CW114_OFFSVAL(value) ((GMAC_ST2CW114_OFFSVAL_Msk & ((value) << GMAC_ST2CW114_OFFSVAL_Pos)))
#define GMAC_ST2CW114_OFFSSTRT_Pos 7
#define GMAC_ST2CW114_OFFSSTRT_Msk (0x3u << GMAC_ST2CW114_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW114) Ethernet Frame Offset Start */
#define GMAC_ST2CW114_OFFSSTRT(value) ((GMAC_ST2CW114_OFFSSTRT_Msk & ((value) << GMAC_ST2CW114_OFFSSTRT_Pos)))
#define   GMAC_ST2CW114_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW114) Offset from the start of the frame */
#define   GMAC_ST2CW114_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW114) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW114_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW114) Offset from the byte after the IP header field */
#define   GMAC_ST2CW114_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW114) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW015 : (GMAC Offset: 0x778) Screening Type 2 Compare Word 0 Register  (index = 15) -------- */
#define GMAC_ST2CW015_MASKVAL_Pos 0
#define GMAC_ST2CW015_MASKVAL_Msk (0xffffu << GMAC_ST2CW015_MASKVAL_Pos) /**< \brief (GMAC_ST2CW015) Mask Value */
#define GMAC_ST2CW015_MASKVAL(value) ((GMAC_ST2CW015_MASKVAL_Msk & ((value) << GMAC_ST2CW015_MASKVAL_Pos)))
#define GMAC_ST2CW015_COMPVAL_Pos 16
#define GMAC_ST2CW015_COMPVAL_Msk (0xffffu << GMAC_ST2CW015_COMPVAL_Pos) /**< \brief (GMAC_ST2CW015) Compare Value */
#define GMAC_ST2CW015_COMPVAL(value) ((GMAC_ST2CW015_COMPVAL_Msk & ((value) << GMAC_ST2CW015_COMPVAL_Pos)))
/* -------- GMAC_ST2CW115 : (GMAC Offset: 0x77C) Screening Type 2 Compare Word 1 Register  (index = 15) -------- */
#define GMAC_ST2CW115_OFFSVAL_Pos 0
#define GMAC_ST2CW115_OFFSVAL_Msk (0x7fu << GMAC_ST2CW115_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW115) Offset Value in Bytes */
#define GMAC_ST2CW115_OFFSVAL(value) ((GMAC_ST2CW115_OFFSVAL_Msk & ((value) << GMAC_ST2CW115_OFFSVAL_Pos)))
#define GMAC_ST2CW115_OFFSSTRT_Pos 7
#define GMAC_ST2CW115_OFFSSTRT_Msk (0x3u << GMAC_ST2CW115_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW115) Ethernet Frame Offset Start */
#define GMAC_ST2CW115_OFFSSTRT(value) ((GMAC_ST2CW115_OFFSSTRT_Msk & ((value) << GMAC_ST2CW115_OFFSSTRT_Pos)))
#define   GMAC_ST2CW115_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW115) Offset from the start of the frame */
#define   GMAC_ST2CW115_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW115) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW115_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW115) Offset from the byte after the IP header field */
#define   GMAC_ST2CW115_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW115) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW016 : (GMAC Offset: 0x780) Screening Type 2 Compare Word 0 Register  (index = 16) -------- */
#define GMAC_ST2CW016_MASKVAL_Pos 0
#define GMAC_ST2CW016_MASKVAL_Msk (0xffffu << GMAC_ST2CW016_MASKVAL_Pos) /**< \brief (GMAC_ST2CW016) Mask Value */
#define GMAC_ST2CW016_MASKVAL(value) ((GMAC_ST2CW016_MASKVAL_Msk & ((value) << GMAC_ST2CW016_MASKVAL_Pos)))
#define GMAC_ST2CW016_COMPVAL_Pos 16
#define GMAC_ST2CW016_COMPVAL_Msk (0xffffu << GMAC_ST2CW016_COMPVAL_Pos) /**< \brief (GMAC_ST2CW016) Compare Value */
#define GMAC_ST2CW016_COMPVAL(value) ((GMAC_ST2CW016_COMPVAL_Msk & ((value) << GMAC_ST2CW016_COMPVAL_Pos)))
/* -------- GMAC_ST2CW116 : (GMAC Offset: 0x784) Screening Type 2 Compare Word 1 Register  (index = 16) -------- */
#define GMAC_ST2CW116_OFFSVAL_Pos 0
#define GMAC_ST2CW116_OFFSVAL_Msk (0x7fu << GMAC_ST2CW116_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW116) Offset Value in Bytes */
#define GMAC_ST2CW116_OFFSVAL(value) ((GMAC_ST2CW116_OFFSVAL_Msk & ((value) << GMAC_ST2CW116_OFFSVAL_Pos)))
#define GMAC_ST2CW116_OFFSSTRT_Pos 7
#define GMAC_ST2CW116_OFFSSTRT_Msk (0x3u << GMAC_ST2CW116_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW116) Ethernet Frame Offset Start */
#define GMAC_ST2CW116_OFFSSTRT(value) ((GMAC_ST2CW116_OFFSSTRT_Msk & ((value) << GMAC_ST2CW116_OFFSSTRT_Pos)))
#define   GMAC_ST2CW116_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW116) Offset from the start of the frame */
#define   GMAC_ST2CW116_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW116) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW116_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW116) Offset from the byte after the IP header field */
#define   GMAC_ST2CW116_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW116) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW017 : (GMAC Offset: 0x788) Screening Type 2 Compare Word 0 Register  (index = 17) -------- */
#define GMAC_ST2CW017_MASKVAL_Pos 0
#define GMAC_ST2CW017_MASKVAL_Msk (0xffffu << GMAC_ST2CW017_MASKVAL_Pos) /**< \brief (GMAC_ST2CW017) Mask Value */
#define GMAC_ST2CW017_MASKVAL(value) ((GMAC_ST2CW017_MASKVAL_Msk & ((value) << GMAC_ST2CW017_MASKVAL_Pos)))
#define GMAC_ST2CW017_COMPVAL_Pos 16
#define GMAC_ST2CW017_COMPVAL_Msk (0xffffu << GMAC_ST2CW017_COMPVAL_Pos) /**< \brief (GMAC_ST2CW017) Compare Value */
#define GMAC_ST2CW017_COMPVAL(value) ((GMAC_ST2CW017_COMPVAL_Msk & ((value) << GMAC_ST2CW017_COMPVAL_Pos)))
/* -------- GMAC_ST2CW117 : (GMAC Offset: 0x78C) Screening Type 2 Compare Word 1 Register  (index = 17) -------- */
#define GMAC_ST2CW117_OFFSVAL_Pos 0
#define GMAC_ST2CW117_OFFSVAL_Msk (0x7fu << GMAC_ST2CW117_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW117) Offset Value in Bytes */
#define GMAC_ST2CW117_OFFSVAL(value) ((GMAC_ST2CW117_OFFSVAL_Msk & ((value) << GMAC_ST2CW117_OFFSVAL_Pos)))
#define GMAC_ST2CW117_OFFSSTRT_Pos 7
#define GMAC_ST2CW117_OFFSSTRT_Msk (0x3u << GMAC_ST2CW117_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW117) Ethernet Frame Offset Start */
#define GMAC_ST2CW117_OFFSSTRT(value) ((GMAC_ST2CW117_OFFSSTRT_Msk & ((value) << GMAC_ST2CW117_OFFSSTRT_Pos)))
#define   GMAC_ST2CW117_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW117) Offset from the start of the frame */
#define   GMAC_ST2CW117_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW117) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW117_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW117) Offset from the byte after the IP header field */
#define   GMAC_ST2CW117_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW117) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW018 : (GMAC Offset: 0x790) Screening Type 2 Compare Word 0 Register  (index = 18) -------- */
#define GMAC_ST2CW018_MASKVAL_Pos 0
#define GMAC_ST2CW018_MASKVAL_Msk (0xffffu << GMAC_ST2CW018_MASKVAL_Pos) /**< \brief (GMAC_ST2CW018) Mask Value */
#define GMAC_ST2CW018_MASKVAL(value) ((GMAC_ST2CW018_MASKVAL_Msk & ((value) << GMAC_ST2CW018_MASKVAL_Pos)))
#define GMAC_ST2CW018_COMPVAL_Pos 16
#define GMAC_ST2CW018_COMPVAL_Msk (0xffffu << GMAC_ST2CW018_COMPVAL_Pos) /**< \brief (GMAC_ST2CW018) Compare Value */
#define GMAC_ST2CW018_COMPVAL(value) ((GMAC_ST2CW018_COMPVAL_Msk & ((value) << GMAC_ST2CW018_COMPVAL_Pos)))
/* -------- GMAC_ST2CW118 : (GMAC Offset: 0x794) Screening Type 2 Compare Word 1 Register  (index = 18) -------- */
#define GMAC_ST2CW118_OFFSVAL_Pos 0
#define GMAC_ST2CW118_OFFSVAL_Msk (0x7fu << GMAC_ST2CW118_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW118) Offset Value in Bytes */
#define GMAC_ST2CW118_OFFSVAL(value) ((GMAC_ST2CW118_OFFSVAL_Msk & ((value) << GMAC_ST2CW118_OFFSVAL_Pos)))
#define GMAC_ST2CW118_OFFSSTRT_Pos 7
#define GMAC_ST2CW118_OFFSSTRT_Msk (0x3u << GMAC_ST2CW118_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW118) Ethernet Frame Offset Start */
#define GMAC_ST2CW118_OFFSSTRT(value) ((GMAC_ST2CW118_OFFSSTRT_Msk & ((value) << GMAC_ST2CW118_OFFSSTRT_Pos)))
#define   GMAC_ST2CW118_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW118) Offset from the start of the frame */
#define   GMAC_ST2CW118_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW118) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW118_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW118) Offset from the byte after the IP header field */
#define   GMAC_ST2CW118_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW118) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW019 : (GMAC Offset: 0x798) Screening Type 2 Compare Word 0 Register  (index = 19) -------- */
#define GMAC_ST2CW019_MASKVAL_Pos 0
#define GMAC_ST2CW019_MASKVAL_Msk (0xffffu << GMAC_ST2CW019_MASKVAL_Pos) /**< \brief (GMAC_ST2CW019) Mask Value */
#define GMAC_ST2CW019_MASKVAL(value) ((GMAC_ST2CW019_MASKVAL_Msk & ((value) << GMAC_ST2CW019_MASKVAL_Pos)))
#define GMAC_ST2CW019_COMPVAL_Pos 16
#define GMAC_ST2CW019_COMPVAL_Msk (0xffffu << GMAC_ST2CW019_COMPVAL_Pos) /**< \brief (GMAC_ST2CW019) Compare Value */
#define GMAC_ST2CW019_COMPVAL(value) ((GMAC_ST2CW019_COMPVAL_Msk & ((value) << GMAC_ST2CW019_COMPVAL_Pos)))
/* -------- GMAC_ST2CW119 : (GMAC Offset: 0x79C) Screening Type 2 Compare Word 1 Register  (index = 19) -------- */
#define GMAC_ST2CW119_OFFSVAL_Pos 0
#define GMAC_ST2CW119_OFFSVAL_Msk (0x7fu << GMAC_ST2CW119_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW119) Offset Value in Bytes */
#define GMAC_ST2CW119_OFFSVAL(value) ((GMAC_ST2CW119_OFFSVAL_Msk & ((value) << GMAC_ST2CW119_OFFSVAL_Pos)))
#define GMAC_ST2CW119_OFFSSTRT_Pos 7
#define GMAC_ST2CW119_OFFSSTRT_Msk (0x3u << GMAC_ST2CW119_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW119) Ethernet Frame Offset Start */
#define GMAC_ST2CW119_OFFSSTRT(value) ((GMAC_ST2CW119_OFFSSTRT_Msk & ((value) << GMAC_ST2CW119_OFFSSTRT_Pos)))
#define   GMAC_ST2CW119_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW119) Offset from the start of the frame */
#define   GMAC_ST2CW119_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW119) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW119_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW119) Offset from the byte after the IP header field */
#define   GMAC_ST2CW119_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW119) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW020 : (GMAC Offset: 0x7A0) Screening Type 2 Compare Word 0 Register  (index = 20) -------- */
#define GMAC_ST2CW020_MASKVAL_Pos 0
#define GMAC_ST2CW020_MASKVAL_Msk (0xffffu << GMAC_ST2CW020_MASKVAL_Pos) /**< \brief (GMAC_ST2CW020) Mask Value */
#define GMAC_ST2CW020_MASKVAL(value) ((GMAC_ST2CW020_MASKVAL_Msk & ((value) << GMAC_ST2CW020_MASKVAL_Pos)))
#define GMAC_ST2CW020_COMPVAL_Pos 16
#define GMAC_ST2CW020_COMPVAL_Msk (0xffffu << GMAC_ST2CW020_COMPVAL_Pos) /**< \brief (GMAC_ST2CW020) Compare Value */
#define GMAC_ST2CW020_COMPVAL(value) ((GMAC_ST2CW020_COMPVAL_Msk & ((value) << GMAC_ST2CW020_COMPVAL_Pos)))
/* -------- GMAC_ST2CW120 : (GMAC Offset: 0x7A4) Screening Type 2 Compare Word 1 Register  (index = 20) -------- */
#define GMAC_ST2CW120_OFFSVAL_Pos 0
#define GMAC_ST2CW120_OFFSVAL_Msk (0x7fu << GMAC_ST2CW120_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW120) Offset Value in Bytes */
#define GMAC_ST2CW120_OFFSVAL(value) ((GMAC_ST2CW120_OFFSVAL_Msk & ((value) << GMAC_ST2CW120_OFFSVAL_Pos)))
#define GMAC_ST2CW120_OFFSSTRT_Pos 7
#define GMAC_ST2CW120_OFFSSTRT_Msk (0x3u << GMAC_ST2CW120_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW120) Ethernet Frame Offset Start */
#define GMAC_ST2CW120_OFFSSTRT(value) ((GMAC_ST2CW120_OFFSSTRT_Msk & ((value) << GMAC_ST2CW120_OFFSSTRT_Pos)))
#define   GMAC_ST2CW120_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW120) Offset from the start of the frame */
#define   GMAC_ST2CW120_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW120) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW120_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW120) Offset from the byte after the IP header field */
#define   GMAC_ST2CW120_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW120) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW021 : (GMAC Offset: 0x7A8) Screening Type 2 Compare Word 0 Register  (index = 21) -------- */
#define GMAC_ST2CW021_MASKVAL_Pos 0
#define GMAC_ST2CW021_MASKVAL_Msk (0xffffu << GMAC_ST2CW021_MASKVAL_Pos) /**< \brief (GMAC_ST2CW021) Mask Value */
#define GMAC_ST2CW021_MASKVAL(value) ((GMAC_ST2CW021_MASKVAL_Msk & ((value) << GMAC_ST2CW021_MASKVAL_Pos)))
#define GMAC_ST2CW021_COMPVAL_Pos 16
#define GMAC_ST2CW021_COMPVAL_Msk (0xffffu << GMAC_ST2CW021_COMPVAL_Pos) /**< \brief (GMAC_ST2CW021) Compare Value */
#define GMAC_ST2CW021_COMPVAL(value) ((GMAC_ST2CW021_COMPVAL_Msk & ((value) << GMAC_ST2CW021_COMPVAL_Pos)))
/* -------- GMAC_ST2CW121 : (GMAC Offset: 0x7AC) Screening Type 2 Compare Word 1 Register  (index = 21) -------- */
#define GMAC_ST2CW121_OFFSVAL_Pos 0
#define GMAC_ST2CW121_OFFSVAL_Msk (0x7fu << GMAC_ST2CW121_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW121) Offset Value in Bytes */
#define GMAC_ST2CW121_OFFSVAL(value) ((GMAC_ST2CW121_OFFSVAL_Msk & ((value) << GMAC_ST2CW121_OFFSVAL_Pos)))
#define GMAC_ST2CW121_OFFSSTRT_Pos 7
#define GMAC_ST2CW121_OFFSSTRT_Msk (0x3u << GMAC_ST2CW121_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW121) Ethernet Frame Offset Start */
#define GMAC_ST2CW121_OFFSSTRT(value) ((GMAC_ST2CW121_OFFSSTRT_Msk & ((value) << GMAC_ST2CW121_OFFSSTRT_Pos)))
#define   GMAC_ST2CW121_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW121) Offset from the start of the frame */
#define   GMAC_ST2CW121_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW121) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW121_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW121) Offset from the byte after the IP header field */
#define   GMAC_ST2CW121_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW121) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW022 : (GMAC Offset: 0x7B0) Screening Type 2 Compare Word 0 Register  (index = 22) -------- */
#define GMAC_ST2CW022_MASKVAL_Pos 0
#define GMAC_ST2CW022_MASKVAL_Msk (0xffffu << GMAC_ST2CW022_MASKVAL_Pos) /**< \brief (GMAC_ST2CW022) Mask Value */
#define GMAC_ST2CW022_MASKVAL(value) ((GMAC_ST2CW022_MASKVAL_Msk & ((value) << GMAC_ST2CW022_MASKVAL_Pos)))
#define GMAC_ST2CW022_COMPVAL_Pos 16
#define GMAC_ST2CW022_COMPVAL_Msk (0xffffu << GMAC_ST2CW022_COMPVAL_Pos) /**< \brief (GMAC_ST2CW022) Compare Value */
#define GMAC_ST2CW022_COMPVAL(value) ((GMAC_ST2CW022_COMPVAL_Msk & ((value) << GMAC_ST2CW022_COMPVAL_Pos)))
/* -------- GMAC_ST2CW122 : (GMAC Offset: 0x7B4) Screening Type 2 Compare Word 1 Register  (index = 22) -------- */
#define GMAC_ST2CW122_OFFSVAL_Pos 0
#define GMAC_ST2CW122_OFFSVAL_Msk (0x7fu << GMAC_ST2CW122_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW122) Offset Value in Bytes */
#define GMAC_ST2CW122_OFFSVAL(value) ((GMAC_ST2CW122_OFFSVAL_Msk & ((value) << GMAC_ST2CW122_OFFSVAL_Pos)))
#define GMAC_ST2CW122_OFFSSTRT_Pos 7
#define GMAC_ST2CW122_OFFSSTRT_Msk (0x3u << GMAC_ST2CW122_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW122) Ethernet Frame Offset Start */
#define GMAC_ST2CW122_OFFSSTRT(value) ((GMAC_ST2CW122_OFFSSTRT_Msk & ((value) << GMAC_ST2CW122_OFFSSTRT_Pos)))
#define   GMAC_ST2CW122_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW122) Offset from the start of the frame */
#define   GMAC_ST2CW122_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW122) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW122_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW122) Offset from the byte after the IP header field */
#define   GMAC_ST2CW122_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW122) Offset from the byte after the TCP/UDP header field */
/* -------- GMAC_ST2CW023 : (GMAC Offset: 0x7B8) Screening Type 2 Compare Word 0 Register  (index = 23) -------- */
#define GMAC_ST2CW023_MASKVAL_Pos 0
#define GMAC_ST2CW023_MASKVAL_Msk (0xffffu << GMAC_ST2CW023_MASKVAL_Pos) /**< \brief (GMAC_ST2CW023) Mask Value */
#define GMAC_ST2CW023_MASKVAL(value) ((GMAC_ST2CW023_MASKVAL_Msk & ((value) << GMAC_ST2CW023_MASKVAL_Pos)))
#define GMAC_ST2CW023_COMPVAL_Pos 16
#define GMAC_ST2CW023_COMPVAL_Msk (0xffffu << GMAC_ST2CW023_COMPVAL_Pos) /**< \brief (GMAC_ST2CW023) Compare Value */
#define GMAC_ST2CW023_COMPVAL(value) ((GMAC_ST2CW023_COMPVAL_Msk & ((value) << GMAC_ST2CW023_COMPVAL_Pos)))
/* -------- GMAC_ST2CW123 : (GMAC Offset: 0x7BC) Screening Type 2 Compare Word 1 Register  (index = 23) -------- */
#define GMAC_ST2CW123_OFFSVAL_Pos 0
#define GMAC_ST2CW123_OFFSVAL_Msk (0x7fu << GMAC_ST2CW123_OFFSVAL_Pos) /**< \brief (GMAC_ST2CW123) Offset Value in Bytes */
#define GMAC_ST2CW123_OFFSVAL(value) ((GMAC_ST2CW123_OFFSVAL_Msk & ((value) << GMAC_ST2CW123_OFFSVAL_Pos)))
#define GMAC_ST2CW123_OFFSSTRT_Pos 7
#define GMAC_ST2CW123_OFFSSTRT_Msk (0x3u << GMAC_ST2CW123_OFFSSTRT_Pos) /**< \brief (GMAC_ST2CW123) Ethernet Frame Offset Start */
#define GMAC_ST2CW123_OFFSSTRT(value) ((GMAC_ST2CW123_OFFSSTRT_Msk & ((value) << GMAC_ST2CW123_OFFSSTRT_Pos)))
#define   GMAC_ST2CW123_OFFSSTRT_FRAMESTART (0x0u << 7) /**< \brief (GMAC_ST2CW123) Offset from the start of the frame */
#define   GMAC_ST2CW123_OFFSSTRT_ETHERTYPE (0x1u << 7) /**< \brief (GMAC_ST2CW123) Offset from the byte after the EtherType field */
#define   GMAC_ST2CW123_OFFSSTRT_IP (0x2u << 7) /**< \brief (GMAC_ST2CW123) Offset from the byte after the IP header field */
#define   GMAC_ST2CW123_OFFSSTRT_TCP_UDP (0x3u << 7) /**< \brief (GMAC_ST2CW123) Offset from the byte after the TCP/UDP header field */

/*@}*/


#endif /* _SAME70_GMAC_COMPONENT_ */
