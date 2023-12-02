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

#ifndef _SAME70_MCAN_COMPONENT_
#define _SAME70_MCAN_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Controller Area Network */
/* ============================================================================= */
/** \addtogroup SAME70_MCAN Controller Area Network */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Mcan hardware registers */
typedef struct {
  __I  uint32_t MCAN_CREL;    /**< \brief (Mcan Offset: 0x00) Core Release Register */
  __I  uint32_t MCAN_ENDN;    /**< \brief (Mcan Offset: 0x04) Endian Register */
  __IO uint32_t MCAN_CUST;    /**< \brief (Mcan Offset: 0x08) Customer Register */
#if (__SAM_M7_REVB == 1)
  __IO uint32_t MCAN_DBTP;    /**< \brief (Mcan Offset: 0x0C) Data Bit Timing and Prescaler Register */	  
#else
  __IO uint32_t MCAN_FBTP;    /**< \brief (Mcan Offset: 0x0C) Fast Bit Timing and Prescaler Register */
#endif
  __IO uint32_t MCAN_TEST;    /**< \brief (Mcan Offset: 0x10) Test Register */
  __IO uint32_t MCAN_RWD;     /**< \brief (Mcan Offset: 0x14) RAM Watchdog Register */
  __IO uint32_t MCAN_CCCR;    /**< \brief (Mcan Offset: 0x18) CC Control Register */
#if (__SAM_M7_REVB == 1)
  __IO uint32_t MCAN_NBTP;     /**< \brief (Mcan Offset: 0x1C) Nominal Bit Timing and Prescaler Register */
#else
  __IO uint32_t MCAN_BTP;     /**< \brief (Mcan Offset: 0x1C) Bit Timing and Prescaler Register */
#endif
  __IO uint32_t MCAN_TSCC;    /**< \brief (Mcan Offset: 0x20) Timestamp Counter Configuration Register */
  __IO uint32_t MCAN_TSCV;    /**< \brief (Mcan Offset: 0x24) Timestamp Counter Value Register */
  __IO uint32_t MCAN_TOCC;    /**< \brief (Mcan Offset: 0x28) Timeout Counter Configuration Register */
  __IO uint32_t MCAN_TOCV;    /**< \brief (Mcan Offset: 0x2C) Timeout Counter Value Register */
  __I  uint32_t Reserved1[4];
  __I  uint32_t MCAN_ECR;     /**< \brief (Mcan Offset: 0x40) Error Counter Register */
  __I  uint32_t MCAN_PSR;     /**< \brief (Mcan Offset: 0x44) Protocol Status Register */
#if (__SAM_M7_REVB == 1)
  __IO uint32_t MCAN_TDCR;    /**< \brief (Mcan Offset: 0x48) Transmit Delay Compensation Register */
  __I  uint32_t Reserved2[1];
#else
  __I  uint32_t Reserved2[2];
#endif
  __IO uint32_t MCAN_IR;      /**< \brief (Mcan Offset: 0x50) Interrupt Register */
  __IO uint32_t MCAN_IE;      /**< \brief (Mcan Offset: 0x54) Interrupt Enable Register */
  __IO uint32_t MCAN_ILS;     /**< \brief (Mcan Offset: 0x58) Interrupt Line Select Register */
  __IO uint32_t MCAN_ILE;     /**< \brief (Mcan Offset: 0x5C) Interrupt Line Enable Register */
  __I  uint32_t Reserved3[8];
  __IO uint32_t MCAN_GFC;     /**< \brief (Mcan Offset: 0x80) Global Filter Configuration Register */
  __IO uint32_t MCAN_SIDFC;   /**< \brief (Mcan Offset: 0x84) Standard ID Filter Configuration Register */
  __IO uint32_t MCAN_XIDFC;   /**< \brief (Mcan Offset: 0x88) Extended ID Filter Configuration Register */
  __I  uint32_t Reserved4[1];
  __IO uint32_t MCAN_XIDAM;   /**< \brief (Mcan Offset: 0x90) Extended ID AND Mask Register */
  __I  uint32_t MCAN_HPMS;    /**< \brief (Mcan Offset: 0x94) High Priority Message Status Register */
  __IO uint32_t MCAN_NDAT1;   /**< \brief (Mcan Offset: 0x98) New Data 1 Register */
  __IO uint32_t MCAN_NDAT2;   /**< \brief (Mcan Offset: 0x9C) New Data 2 Register */
  __IO uint32_t MCAN_RXF0C;   /**< \brief (Mcan Offset: 0xA0) Receive FIFO 0 Configuration Register */
  __I  uint32_t MCAN_RXF0S;   /**< \brief (Mcan Offset: 0xA4) Receive FIFO 0 Status Register */
  __IO uint32_t MCAN_RXF0A;   /**< \brief (Mcan Offset: 0xA8) Receive FIFO 0 Acknowledge Register */
  __IO uint32_t MCAN_RXBC;    /**< \brief (Mcan Offset: 0xAC) Receive Rx Buffer Configuration Register */
  __IO uint32_t MCAN_RXF1C;   /**< \brief (Mcan Offset: 0xB0) Receive FIFO 1 Configuration Register */
  __I  uint32_t MCAN_RXF1S;   /**< \brief (Mcan Offset: 0xB4) Receive FIFO 1 Status Register */
  __IO uint32_t MCAN_RXF1A;   /**< \brief (Mcan Offset: 0xB8) Receive FIFO 1 Acknowledge Register */
  __IO uint32_t MCAN_RXESC;   /**< \brief (Mcan Offset: 0xBC) Receive Buffer / FIFO Element Size Configuration Register */
  __IO uint32_t MCAN_TXBC;    /**< \brief (Mcan Offset: 0xC0) Transmit Buffer Configuration Register */
  __I  uint32_t MCAN_TXFQS;   /**< \brief (Mcan Offset: 0xC4) Transmit FIFO/Queue Status Register */
  __IO uint32_t MCAN_TXESC;   /**< \brief (Mcan Offset: 0xC8) Transmit Buffer Element Size Configuration Register */
  __I  uint32_t MCAN_TXBRP;   /**< \brief (Mcan Offset: 0xCC) Transmit Buffer Request Pending Register */
  __IO uint32_t MCAN_TXBAR;   /**< \brief (Mcan Offset: 0xD0) Transmit Buffer Add Request Register */
  __IO uint32_t MCAN_TXBCR;   /**< \brief (Mcan Offset: 0xD4) Transmit Buffer Cancellation Request Register */
  __I  uint32_t MCAN_TXBTO;   /**< \brief (Mcan Offset: 0xD8) Transmit Buffer Transmission Occurred Register */
  __I  uint32_t MCAN_TXBCF;   /**< \brief (Mcan Offset: 0xDC) Transmit Buffer Cancellation Finished Register */
  __IO uint32_t MCAN_TXBTIE;  /**< \brief (Mcan Offset: 0xE0) Transmit Buffer Transmission Interrupt Enable Register */
  __IO uint32_t MCAN_TXBCIE;  /**< \brief (Mcan Offset: 0xE4) Transmit Buffer Cancellation Finished Interrupt Enable Register */
  __I  uint32_t Reserved5[2];
  __IO uint32_t MCAN_TXEFC;   /**< \brief (Mcan Offset: 0xF0) Transmit Event FIFO Configuration Register */
  __I  uint32_t MCAN_TXEFS;   /**< \brief (Mcan Offset: 0xF4) Transmit Event FIFO Status Register */
  __IO uint32_t MCAN_TXEFA;   /**< \brief (Mcan Offset: 0xF8) Transmit Event FIFO Acknowledge Register */
} Mcan;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- MCAN_CREL : (MCAN Offset: 0x00) Core Release Register -------- */
#define MCAN_CREL_DAY_Pos 0
#define MCAN_CREL_DAY_Msk (0xffu << MCAN_CREL_DAY_Pos) /**< \brief (MCAN_CREL) Timestamp Day */
#define MCAN_CREL_MON_Pos 8
#define MCAN_CREL_MON_Msk (0xffu << MCAN_CREL_MON_Pos) /**< \brief (MCAN_CREL) Timestamp Month */
#define MCAN_CREL_YEAR_Pos 16
#define MCAN_CREL_YEAR_Msk (0xfu << MCAN_CREL_YEAR_Pos) /**< \brief (MCAN_CREL) Timestamp Year */
#define MCAN_CREL_SUBSTEP_Pos 20
#define MCAN_CREL_SUBSTEP_Msk (0xfu << MCAN_CREL_SUBSTEP_Pos) /**< \brief (MCAN_CREL) Sub-step of Core Release */
#define MCAN_CREL_STEP_Pos 24
#define MCAN_CREL_STEP_Msk (0xfu << MCAN_CREL_STEP_Pos) /**< \brief (MCAN_CREL) Step of Core Release */
#define MCAN_CREL_REL_Pos 28
#define MCAN_CREL_REL_Msk (0xfu << MCAN_CREL_REL_Pos) /**< \brief (MCAN_CREL) Core Release */
/* -------- MCAN_ENDN : (MCAN Offset: 0x04) Endian Register -------- */
#define MCAN_ENDN_ETV_Pos 0
#define MCAN_ENDN_ETV_Msk (0xffffffffu << MCAN_ENDN_ETV_Pos) /**< \brief (MCAN_ENDN) Endianness Test Value */
/* -------- MCAN_CUST : (MCAN Offset: 0x08) Customer Register -------- */
#define MCAN_CUST_CSV_Pos 0
#define MCAN_CUST_CSV_Msk (0xffffffffu << MCAN_CUST_CSV_Pos) /**< \brief (MCAN_CUST) Customer-specific Value */
#define MCAN_CUST_CSV(value) ((MCAN_CUST_CSV_Msk & ((value) << MCAN_CUST_CSV_Pos)))
/* -------- MCAN_FBTP : (MCAN Offset: 0x0C) Fast Bit Timing and Prescaler Register -------- */
#define MCAN_FBTP_FSJW_Pos 0
#define MCAN_FBTP_FSJW_Msk (0x3u << MCAN_FBTP_FSJW_Pos) /**< \brief (MCAN_FBTP) Fast (Re) Synchronization Jump Width */
#define MCAN_FBTP_FSJW(value) ((MCAN_FBTP_FSJW_Msk & ((value) << MCAN_FBTP_FSJW_Pos)))
#define MCAN_FBTP_FTSEG2_Pos 4
#define MCAN_FBTP_FTSEG2_Msk (0x7u << MCAN_FBTP_FTSEG2_Pos) /**< \brief (MCAN_FBTP) Fast Time Segment After Sample Point */
#define MCAN_FBTP_FTSEG2(value) ((MCAN_FBTP_FTSEG2_Msk & ((value) << MCAN_FBTP_FTSEG2_Pos)))
#define MCAN_FBTP_FTSEG1_Pos 8
#define MCAN_FBTP_FTSEG1_Msk (0xfu << MCAN_FBTP_FTSEG1_Pos) /**< \brief (MCAN_FBTP) Fast Time Segment Before Sample Point */
#define MCAN_FBTP_FTSEG1(value) ((MCAN_FBTP_FTSEG1_Msk & ((value) << MCAN_FBTP_FTSEG1_Pos)))
#define MCAN_FBTP_FBRP_Pos 16
#define MCAN_FBTP_FBRP_Msk (0x1fu << MCAN_FBTP_FBRP_Pos) /**< \brief (MCAN_FBTP) Fast Baud Rate Prescaler */
#define MCAN_FBTP_FBRP(value) ((MCAN_FBTP_FBRP_Msk & ((value) << MCAN_FBTP_FBRP_Pos)))
#define MCAN_FBTP_TDC (0x1u << 23) /**< \brief (MCAN_FBTP) Transceiver Delay Compensation */
#define   MCAN_FBTP_TDC_DISABLED (0x0u << 23) /**< \brief (MCAN_FBTP) Transceiver Delay Compensation disabled. */
#define   MCAN_FBTP_TDC_ENABLED (0x1u << 23) /**< \brief (MCAN_FBTP) Transceiver Delay Compensation enabled. */
#define MCAN_FBTP_TDCO_Pos 24
#define MCAN_FBTP_TDCO_Msk (0x1fu << MCAN_FBTP_TDCO_Pos) /**< \brief (MCAN_FBTP) Transce iver Delay Compensation Offset */
#define MCAN_FBTP_TDCO(value) ((MCAN_FBTP_TDCO_Msk & ((value) << MCAN_FBTP_TDCO_Pos)))
/* -------- MCAN_DBTP : (MCAN Offset: 0x0C) Data Bit Timing and Prescaler Register -------- */
#define MCAN_DBTP_DSJW_Pos 0
#define MCAN_DBTP_DSJW_Msk (0x7u << MCAN_DBTP_DSJW_Pos) /**< \brief (MCAN_DBTP) Data (Re) Synchronization Jump Width */
#define MCAN_DBTP_DSJW(value) ((MCAN_DBTP_DSJW_Msk & ((value) << MCAN_DBTP_DSJW_Pos)))
#define MCAN_DBTP_DTSEG2_Pos 4
#define MCAN_DBTP_DTSEG2_Msk (0xfu << MCAN_DBTP_DTSEG2_Pos) /**< \brief (MCAN_DBTP) Data Time Segment After Sample Point */
#define MCAN_DBTP_DTSEG2(value) ((MCAN_DBTP_DTSEG2_Msk & ((value) << MCAN_DBTP_DTSEG2_Pos)))
#define MCAN_DBTP_DTSEG1_Pos 8
#define MCAN_DBTP_DTSEG1_Msk (0x1fu << MCAN_DBTP_DTSEG1_Pos) /**< \brief (MCAN_DBTP) Data Time Segment Before Sample Point */
#define MCAN_DBTP_DTSEG1(value) ((MCAN_DBTP_DTSEG1_Msk & ((value) << MCAN_DBTP_DTSEG1_Pos)))
#define MCAN_DBTP_DBRP_Pos 16
#define MCAN_DBTP_DBRP_Msk (0x1fu << MCAN_DBTP_DBRP_Pos) /**< \brief (MCAN_DBTP) Data Bit Rate Prescaler */
#define MCAN_DBTP_DBRP(value) ((MCAN_DBTP_DBRP_Msk & ((value) << MCAN_DBTP_DBRP_Pos)))
#define MCAN_DBTP_TDC (0x1u << 23) /**< \brief (MCAN_DBTP) Transmitter Delay Compensation */
#define   MCAN_DBTP_TDC_DISABLED (0x0u << 23) /**< \brief (MCAN_DBTP) Transmitter Delay Compensation disabled. */
#define   MCAN_DBTP_TDC_ENABLED (0x1u << 23) /**< \brief (MCAN_DBTP) Transmitter Delay Compensation enabled. */
/* -------- MCAN_TEST : (MCAN Offset: 0x10) Test Register -------- */
#define MCAN_TEST_LBCK (0x1u << 4) /**< \brief (MCAN_TEST) Loop Back Mode (read/write) */
#define   MCAN_TEST_LBCK_DISABLED (0x0u << 4) /**< \brief (MCAN_TEST) Reset value. Loop Back mode is disabled. */
#define   MCAN_TEST_LBCK_ENABLED (0x1u << 4) /**< \brief (MCAN_TEST) Loop Back mode is enabled (see Section 6.1.9). */
#define MCAN_TEST_TX_Pos 5
#define MCAN_TEST_TX_Msk (0x3u << MCAN_TEST_TX_Pos) /**< \brief (MCAN_TEST) Control of Transmit Pin (read/write) */
#define MCAN_TEST_TX(value) ((MCAN_TEST_TX_Msk & ((value) << MCAN_TEST_TX_Pos)))
#define   MCAN_TEST_TX_RESET (0x0u << 5) /**< \brief (MCAN_TEST) Reset value, CANTX controlled by the CAN Core, updated at the end of the CAN bit time. */
#define   MCAN_TEST_TX_SAMPLE_POINT_MONITORING (0x1u << 5) /**< \brief (MCAN_TEST) Sample Point can be monitored at pin CANTX. */
#define   MCAN_TEST_TX_DOMINANT (0x2u << 5) /**< \brief (MCAN_TEST) Dominant ('0') level at pin CANTX. */
#define   MCAN_TEST_TX_RECESSIVE (0x3u << 5) /**< \brief (MCAN_TEST) Recessive ('1') at pin CANTX. */
#define MCAN_TEST_RX (0x1u << 7) /**< \brief (MCAN_TEST) Receive Pin (read-only) */
#define MCAN_TEST_TDCV_Pos 8
#define MCAN_TEST_TDCV_Msk (0x3fu << MCAN_TEST_TDCV_Pos) /**< \brief (MCAN_TEST) Transceiver Delay Compensation Value (read-only) */
#define MCAN_TEST_TDCV(value) ((MCAN_TEST_TDCV_Msk & ((value) << MCAN_TEST_TDCV_Pos)))
/* -------- MCAN_RWD : (MCAN Offset: 0x14) RAM Watchdog Register -------- */
#define MCAN_RWD_WDC_Pos 0
#define MCAN_RWD_WDC_Msk (0xffu << MCAN_RWD_WDC_Pos) /**< \brief (MCAN_RWD) Watchdog Configuration (read/write) */
#define MCAN_RWD_WDC(value) ((MCAN_RWD_WDC_Msk & ((value) << MCAN_RWD_WDC_Pos)))
#define MCAN_RWD_WDV_Pos 8
#define MCAN_RWD_WDV_Msk (0xffu << MCAN_RWD_WDV_Pos) /**< \brief (MCAN_RWD) Watchdog Value (read-only) */
#define MCAN_RWD_WDV(value) ((MCAN_RWD_WDV_Msk & ((value) << MCAN_RWD_WDV_Pos)))
/* -------- MCAN_CCCR : (MCAN Offset: 0x18) CC Control Register -------- */
#define MCAN_CCCR_INIT (0x1u << 0) /**< \brief (MCAN_CCCR) Initialization (read/write) */
#define   MCAN_CCCR_INIT_DISABLED (0x0u << 0) /**< \brief (MCAN_CCCR) Normal operation. */
#define   MCAN_CCCR_INIT_ENABLED (0x1u << 0) /**< \brief (MCAN_CCCR) Initialization is started. */
#define MCAN_CCCR_CCE (0x1u << 1) /**< \brief (MCAN_CCCR) Configuration Change Enable (read/write, write protection) */
#define   MCAN_CCCR_CCE_PROTECTED (0x0u << 1) /**< \brief (MCAN_CCCR) The processor has no write access to the protected configuration registers. */
#define   MCAN_CCCR_CCE_CONFIGURABLE (0x1u << 1) /**< \brief (MCAN_CCCR) The processor has write access to the protected configuration registers (while MCAN_CCCR.INIT = '1'). */
#define MCAN_CCCR_ASM (0x1u << 2) /**< \brief (MCAN_CCCR) Restricted Operation Mode (read/write, write protection against '1') */
#define   MCAN_CCCR_ASM_NORMAL (0x0u << 2) /**< \brief (MCAN_CCCR) Normal CAN operation. */
#define   MCAN_CCCR_ASM_RESTRICTED (0x1u << 2) /**< \brief (MCAN_CCCR) Restricted operation mode active. */
#define MCAN_CCCR_CSA (0x1u << 3) /**< \brief (MCAN_CCCR) Clock Stop Acknowledge (read-only) */
#define MCAN_CCCR_CSR (0x1u << 4) /**< \brief (MCAN_CCCR) Clock Stop Request (read/write) */
#define   MCAN_CCCR_CSR_NO_CLOCK_STOP (0x0u << 4) /**< \brief (MCAN_CCCR) No clock stop is requested. */
#define   MCAN_CCCR_CSR_CLOCK_STOP (0x1u << 4) /**< \brief (MCAN_CCCR) Clock stop requested. When clock stop is requested, first INIT and then CSA will be set after all pend-ing transfer requests have been completed and the CAN bus reached idle. */
#define MCAN_CCCR_MON (0x1u << 5) /**< \brief (MCAN_CCCR) Bus Monitoring Mode (read/write, write protection against '1') */
#define   MCAN_CCCR_MON_DISABLED (0x0u << 5) /**< \brief (MCAN_CCCR) Bus Monitoring mode is disabled. */
#define   MCAN_CCCR_MON_ENABLED (0x1u << 5) /**< \brief (MCAN_CCCR) Bus Monitoring mode is enabled. */
#define MCAN_CCCR_DAR (0x1u << 6) /**< \brief (MCAN_CCCR) Disable Automatic Retransmission (read/write, write protection) */
#define   MCAN_CCCR_DAR_AUTO_RETX (0x0u << 6) /**< \brief (MCAN_CCCR) Automatic retransmission of messages not transmitted successfully enabled. */
#define   MCAN_CCCR_DAR_NO_AUTO_RETX (0x1u << 6) /**< \brief (MCAN_CCCR) Automatic retransmission disabled. */
#define MCAN_CCCR_TEST (0x1u << 7) /**< \brief (MCAN_CCCR) Test Mode Enable (read/write, write protection against '1') */
#define   MCAN_CCCR_TEST_DISABLED (0x0u << 7) /**< \brief (MCAN_CCCR) Normal operation, MCAN_TEST register holds reset values. */
#define   MCAN_CCCR_TEST_ENABLED (0x1u << 7) /**< \brief (MCAN_CCCR) Test mode, write access to MCAN_TEST register enabled. */
#define MCAN_CCCR_CME_Pos 8
#define MCAN_CCCR_CME_Msk (0x3u << MCAN_CCCR_CME_Pos) /**< \brief (MCAN_CCCR) CAN Mode Enable (read/write, write protection) */
#define MCAN_CCCR_CME(value) ((MCAN_CCCR_CME_Msk & ((value) << MCAN_CCCR_CME_Pos)))
#define   MCAN_CCCR_CME_ISO11898_1 (0x0u << 8) /**< \brief (MCAN_CCCR) CAN operation according to ISO11898-1 enabled */
#define   MCAN_CCCR_CME_FD (0x1u << 8) /**< \brief (MCAN_CCCR) CAN FD operation enabled */
#define MCAN_CCCR_CMR_Pos 10
#define MCAN_CCCR_CMR_Msk (0x3u << MCAN_CCCR_CMR_Pos) /**< \brief (MCAN_CCCR) CAN Mode Request (read/write) */
#define MCAN_CCCR_CMR(value) ((MCAN_CCCR_CMR_Msk & ((value) << MCAN_CCCR_CMR_Pos)))
#define   MCAN_CCCR_CMR_NO_CHANGE (0x0u << 10) /**< \brief (MCAN_CCCR) No mode change */
#define   MCAN_CCCR_CMR_FD (0x1u << 10) /**< \brief (MCAN_CCCR) Request CAN FD operation */
#define   MCAN_CCCR_CMR_FD_BITRATE_SWITCH (0x2u << 10) /**< \brief (MCAN_CCCR) Request CAN FD operation with bit rate switching */
#define   MCAN_CCCR_CMR_ISO11898_1 (0x3u << 10) /**< \brief (MCAN_CCCR) Request CAN operation according ISO11898-1 */
#define MCAN_CCCR_FDO (0x1u << 12) /**< \brief (MCAN_CCCR) CAN FD Operation (read-only) */
#define MCAN_CCCR_FDBS (0x1u << 13) /**< \brief (MCAN_CCCR) CAN FD Bit Rate Switching (read-only) */
#define MCAN_CCCR_FDOE (0x1u << 8) /**< \brief (MCAN_CCCR) CAN FD Operation Enable (read/write, write protection) */
#define   MCAN_CCCR_FDOE_DISABLED (0x0u << 8) /**< \brief (MCAN_CCCR) FD operation disabled. */
#define   MCAN_CCCR_FDOE_ENABLED (0x1u << 8) /**< \brief (MCAN_CCCR) FD operation enabled. */
#define MCAN_CCCR_BRSE (0x1u << 9) /**< \brief (MCAN_CCCR) Bit Rate Switching Enable (read/write, write protection) */
#define   MCAN_CCCR_BRSE_DISABLED (0x0u << 9) /**< \brief (MCAN_CCCR) Bit rate switching for transmissions disabled. */
#define   MCAN_CCCR_BRSE_ENABLED (0x1u << 9) /**< \brief (MCAN_CCCR) Bit rate switching for transmissions enabled. */
#define MCAN_CCCR_PXHD (0x1u << 12) /**< \brief (MCAN_CCCR) Protocol Exception Event Handling (read/write, write protection) */
#define MCAN_CCCR_EFBI (0x1u << 13) /**< \brief (MCAN_CCCR) Edge Filtering during Bus Integration (read/write, write protection) */
#define MCAN_CCCR_TXP (0x1u << 14) /**< \brief (MCAN_CCCR) Transmit Pause (read/write, write protection) */
#define MCAN_CCCR_NISO (0x1u << 15) /**< \brief (MCAN_CCCR) Non-ISO Operation */
/* -------- MCAN_BTP : (MCAN Offset: 0x1C) Bit Timing and Prescaler Register -------- */
#define MCAN_BTP_SJW_Pos 0
#define MCAN_BTP_SJW_Msk (0xfu << MCAN_BTP_SJW_Pos) /**< \brief (MCAN_BTP) (Re) Synchronization Jump Width */
#define MCAN_BTP_SJW(value) ((MCAN_BTP_SJW_Msk & ((value) << MCAN_BTP_SJW_Pos)))
#define MCAN_BTP_TSEG2_Pos 4
#define MCAN_BTP_TSEG2_Msk (0xfu << MCAN_BTP_TSEG2_Pos) /**< \brief (MCAN_BTP) Time Segment After Sample Point */
#define MCAN_BTP_TSEG2(value) ((MCAN_BTP_TSEG2_Msk & ((value) << MCAN_BTP_TSEG2_Pos)))
#define MCAN_BTP_TSEG1_Pos 8
#define MCAN_BTP_TSEG1_Msk (0x3fu << MCAN_BTP_TSEG1_Pos) /**< \brief (MCAN_BTP) Time Segment Before Sample Point */
#define MCAN_BTP_TSEG1(value) ((MCAN_BTP_TSEG1_Msk & ((value) << MCAN_BTP_TSEG1_Pos)))
#define MCAN_BTP_BRP_Pos 16
#define MCAN_BTP_BRP_Msk (0x3ffu << MCAN_BTP_BRP_Pos) /**< \brief (MCAN_BTP) Baud Rate Prescaler */
#define MCAN_BTP_BRP(value) ((MCAN_BTP_BRP_Msk & ((value) << MCAN_BTP_BRP_Pos)))
/* -------- MCAN_NBTP : (MCAN Offset: 0x1C) Nominal Bit Timing and Prescaler Register -------- */
#define MCAN_NBTP_NTSEG2_Pos 0
#define MCAN_NBTP_NTSEG2_Msk (0x7fu << MCAN_NBTP_NTSEG2_Pos) /**< \brief (MCAN_NBTP) Nominal Time Segment After Sample Point */
#define MCAN_NBTP_NTSEG2(value) ((MCAN_NBTP_NTSEG2_Msk & ((value) << MCAN_NBTP_NTSEG2_Pos)))
#define MCAN_NBTP_NTSEG1_Pos 8
#define MCAN_NBTP_NTSEG1_Msk (0xffu << MCAN_NBTP_NTSEG1_Pos) /**< \brief (MCAN_NBTP) Nominal Time Segment Before Sample Point */
#define MCAN_NBTP_NTSEG1(value) ((MCAN_NBTP_NTSEG1_Msk & ((value) << MCAN_NBTP_NTSEG1_Pos)))
#define MCAN_NBTP_NBRP_Pos 16
#define MCAN_NBTP_NBRP_Msk (0x1ffu << MCAN_NBTP_NBRP_Pos) /**< \brief (MCAN_NBTP) Nominal Bit Rate Prescaler */
#define MCAN_NBTP_NBRP(value) ((MCAN_NBTP_NBRP_Msk & ((value) << MCAN_NBTP_NBRP_Pos)))
#define MCAN_NBTP_NSJW_Pos 25
#define MCAN_NBTP_NSJW_Msk (0x7fu << MCAN_NBTP_NSJW_Pos) /**< \brief (MCAN_NBTP) Nominal (Re) Synchronization Jump Width */
#define MCAN_NBTP_NSJW(value) ((MCAN_NBTP_NSJW_Msk & ((value) << MCAN_NBTP_NSJW_Pos)))
/* -------- MCAN_TSCC : (MCAN Offset: 0x20) Timestamp Counter Configuration Register -------- */
#define MCAN_TSCC_TSS_Pos 0
#define MCAN_TSCC_TSS_Msk (0x3u << MCAN_TSCC_TSS_Pos) /**< \brief (MCAN_TSCC) Timestamp Select */
#define MCAN_TSCC_TSS(value) ((MCAN_TSCC_TSS_Msk & ((value) << MCAN_TSCC_TSS_Pos)))
#define   MCAN_TSCC_TSS_ALWAYS_0 (0x0u << 0) /**< \brief (MCAN_TSCC) Timestamp counter value always 0x0000 */
#define   MCAN_TSCC_TSS_TCP_INC (0x1u << 0) /**< \brief (MCAN_TSCC) Timestamp counter value incremented according to TCP */
#define   MCAN_TSCC_TSS_EXT_TIMESTAMP (0x2u << 0) /**< \brief (MCAN_TSCC) External timestamp counter value used */
#define MCAN_TSCC_TCP_Pos 16
#define MCAN_TSCC_TCP_Msk (0xfu << MCAN_TSCC_TCP_Pos) /**< \brief (MCAN_TSCC) Timestamp Counter Prescaler */
#define MCAN_TSCC_TCP(value) ((MCAN_TSCC_TCP_Msk & ((value) << MCAN_TSCC_TCP_Pos)))
/* -------- MCAN_TSCV : (MCAN Offset: 0x24) Timestamp Counter Value Register -------- */
#define MCAN_TSCV_TSC_Pos 0
#define MCAN_TSCV_TSC_Msk (0xffffu << MCAN_TSCV_TSC_Pos) /**< \brief (MCAN_TSCV) Timestamp Counter (cleared on write) */
#define MCAN_TSCV_TSC(value) ((MCAN_TSCV_TSC_Msk & ((value) << MCAN_TSCV_TSC_Pos)))
/* -------- MCAN_TOCC : (MCAN Offset: 0x28) Timeout Counter Configuration Register -------- */
#define MCAN_TOCC_ETOC (0x1u << 0) /**< \brief (MCAN_TOCC) Enable Timeout Counter */
#define   MCAN_TOCC_ETOC_NO_TIMEOUT (0x0u << 0) /**< \brief (MCAN_TOCC) Timeout Counter disabled. */
#define   MCAN_TOCC_ETOC_TOS_CONTROLLED (0x1u << 0) /**< \brief (MCAN_TOCC) Timeout Counter enabled. */
#define MCAN_TOCC_TOS_Pos 1
#define MCAN_TOCC_TOS_Msk (0x3u << MCAN_TOCC_TOS_Pos) /**< \brief (MCAN_TOCC) Timeout Select */
#define MCAN_TOCC_TOS(value) ((MCAN_TOCC_TOS_Msk & ((value) << MCAN_TOCC_TOS_Pos)))
#define   MCAN_TOCC_TOS_CONTINUOUS (0x0u << 1) /**< \brief (MCAN_TOCC) Continuous operation */
#define   MCAN_TOCC_TOS_TX_EV_TIMEOUT (0x1u << 1) /**< \brief (MCAN_TOCC) Timeout controlled by Tx Event FIFO */
#define   MCAN_TOCC_TOS_RX0_EV_TIMEOUT (0x2u << 1) /**< \brief (MCAN_TOCC) Timeout controlled by Receive FIFO 0 */
#define   MCAN_TOCC_TOS_RX1_EV_TIMEOUT (0x3u << 1) /**< \brief (MCAN_TOCC) Timeout controlled by Receive FIFO 1 */
#define MCAN_TOCC_TOP_Pos 16
#define MCAN_TOCC_TOP_Msk (0xffffu << MCAN_TOCC_TOP_Pos) /**< \brief (MCAN_TOCC) Timeout Period */
#define MCAN_TOCC_TOP(value) ((MCAN_TOCC_TOP_Msk & ((value) << MCAN_TOCC_TOP_Pos)))
/* -------- MCAN_TOCV : (MCAN Offset: 0x2C) Timeout Counter Value Register -------- */
#define MCAN_TOCV_TOC_Pos 0
#define MCAN_TOCV_TOC_Msk (0xffffu << MCAN_TOCV_TOC_Pos) /**< \brief (MCAN_TOCV) Timeout Counter (cleared on write) */
#define MCAN_TOCV_TOC(value) ((MCAN_TOCV_TOC_Msk & ((value) << MCAN_TOCV_TOC_Pos)))
/* -------- MCAN_ECR : (MCAN Offset: 0x40) Error Counter Register -------- */
#define MCAN_ECR_TEC_Pos 0
#define MCAN_ECR_TEC_Msk (0xffu << MCAN_ECR_TEC_Pos) /**< \brief (MCAN_ECR) Transmit Error Counter */
#define MCAN_ECR_REC_Pos 8
#define MCAN_ECR_REC_Msk (0x7fu << MCAN_ECR_REC_Pos) /**< \brief (MCAN_ECR) Receive Error Counter */
#define MCAN_ECR_RP (0x1u << 15) /**< \brief (MCAN_ECR) Receive Error Passive */
#define MCAN_ECR_CEL_Pos 16
#define MCAN_ECR_CEL_Msk (0xffu << MCAN_ECR_CEL_Pos) /**< \brief (MCAN_ECR) CAN Error Logging (cleared on read) */
/* -------- MCAN_PSR : (MCAN Offset: 0x44) Protocol Status Register -------- */
#define MCAN_PSR_LEC_Pos 0
#define MCAN_PSR_LEC_Msk (0x7u << MCAN_PSR_LEC_Pos) /**< \brief (MCAN_PSR) Last Error Code (set to 111 on read) */
#define   MCAN_PSR_LEC_NO_ERROR (0x0u << 0) /**< \brief (MCAN_PSR) No error occurred since LEC has been reset by successful reception or transmission. */
#define   MCAN_PSR_LEC_STUFF_ERROR (0x1u << 0) /**< \brief (MCAN_PSR) More than 5 equal bits in a sequence have occurred in a part of a received meSsage where this is not allowed. */
#define   MCAN_PSR_LEC_FORM_ERROR (0x2u << 0) /**< \brief (MCAN_PSR) A fixed format part of a received frame has the wrong format. */
#define   MCAN_PSR_LEC_ACK_ERROR (0x3u << 0) /**< \brief (MCAN_PSR) The message transmitted by the MCAN was not acknowledged by another node. */
#define   MCAN_PSR_LEC_BIT1_ERROR (0x4u << 0) /**< \brief (MCAN_PSR) During the transmission of a message (with the exception of the arbitration field), the device wanted to send a recessive level (bit of logical value '1'), but the monitored bus value was dominant. */
#define   MCAN_PSR_LEC_BIT0_ERROR (0x5u << 0) /**< \brief (MCAN_PSR) During the transmission of a message (or acknowledge bit, or active error flag, or overload flag), the device wanted to send a dominant level (data or identifier bit logical value '0'), but the monitored bus value was recessive. During Bus_Off recovery this status is set each time a sequence of 11 recessive bits has been monitored. This enables the processor to monitor the proceeding of the Bus_Off recovery sequence (indicating the bus is not stuck at dominant or continuously disturbed). */
#define   MCAN_PSR_LEC_CRC_ERROR (0x6u << 0) /**< \brief (MCAN_PSR) The CRC check sum of a received message was incorrect. The CRC of an incoming message does not match with the CRC calculated from the received data. */
#define   MCAN_PSR_LEC_NO_CHANGE (0x7u << 0) /**< \brief (MCAN_PSR) Any read access to the Protocol Status Register re-initializes the LEC to '7'. When the LEC shows the value '7', no CAN bus event was detected since the last processor read access to the Protocol Status Register. */
#define MCAN_PSR_ACT_Pos 3
#define MCAN_PSR_ACT_Msk (0x3u << MCAN_PSR_ACT_Pos) /**< \brief (MCAN_PSR) Activity */
#define   MCAN_PSR_ACT_SYNCHRONIZING (0x0u << 3) /**< \brief (MCAN_PSR) Node is synchronizing on CAN communication */
#define   MCAN_PSR_ACT_IDLE (0x1u << 3) /**< \brief (MCAN_PSR) Node is neither receiver nor transmitter */
#define   MCAN_PSR_ACT_RECEIVER (0x2u << 3) /**< \brief (MCAN_PSR) Node is operating as receiver */
#define   MCAN_PSR_ACT_TRANSMITTER (0x3u << 3) /**< \brief (MCAN_PSR) Node is operating as transmitter */
#define MCAN_PSR_EP (0x1u << 5) /**< \brief (MCAN_PSR) Error Passive */
#define MCAN_PSR_EW (0x1u << 6) /**< \brief (MCAN_PSR) Warning Status */
#define MCAN_PSR_BO (0x1u << 7) /**< \brief (MCAN_PSR) Bus_Off Status */
#define MCAN_PSR_FLEC_Pos 8
#define MCAN_PSR_FLEC_Msk (0x7u << MCAN_PSR_FLEC_Pos) /**< \brief (MCAN_PSR) Fast Last Error Code (set to 111 on read) */
#define MCAN_PSR_DLEC_Pos 8
#define MCAN_PSR_DLEC_Msk (0x7u << MCAN_PSR_DLEC_Pos) /**< \brief (MCAN_PSR) Data Phase Last Error Code (set to 111 on read) */
#define MCAN_PSR_RESI (0x1u << 11) /**< \brief (MCAN_PSR) ESI Flag of Last Received CAN FD Message (cleared on read) */
#define MCAN_PSR_RBRS (0x1u << 12) /**< \brief (MCAN_PSR) BRS Flag of Last Received CAN FD Message (cleared on read) */
#define MCAN_PSR_REDL (0x1u << 13) /**< \brief (MCAN_PSR) Received a CAN FD Message (cleared on read) */
#define MCAN_PSR_RFDF (0x1u << 13) /**< \brief (MCAN_PSR) Received a CAN FD Message (cleared on read) */
#define MCAN_PSR_PXE (0x1u << 14) /**< \brief (MCAN_PSR) Protocol Exception Event (cleared on read) */
#define MCAN_PSR_TDCV_Pos 16
#define MCAN_PSR_TDCV_Msk (0x7fu << MCAN_PSR_TDCV_Pos) /**< \brief (MCAN_PSR) Transmitter Delay Compensation Value */
/* -------- MCAN_TDCR : (MCAN Offset: 0x48) Transmit Delay Compensation Register -------- */
#define MCAN_TDCR_TDCF_Pos 0
#define MCAN_TDCR_TDCF_Msk (0x7fu << MCAN_TDCR_TDCF_Pos) /**< \brief (MCAN_TDCR) Transmitter Delay Compensation Filter */
#define MCAN_TDCR_TDCF(value) ((MCAN_TDCR_TDCF_Msk & ((value) << MCAN_TDCR_TDCF_Pos)))
#define MCAN_TDCR_TDCO_Pos 8
#define MCAN_TDCR_TDCO_Msk (0x7fu << MCAN_TDCR_TDCO_Pos) /**< \brief (MCAN_TDCR) Transmitter Delay Compensation Offset */
#define MCAN_TDCR_TDCO(value) ((MCAN_TDCR_TDCO_Msk & ((value) << MCAN_TDCR_TDCO_Pos)))
/* -------- MCAN_IR : (MCAN Offset: 0x50) Interrupt Register -------- */
#define MCAN_IR_RF0N (0x1u << 0) /**< \brief (MCAN_IR) Receive FIFO 0 New Message */
#define MCAN_IR_RF0W (0x1u << 1) /**< \brief (MCAN_IR) Receive FIFO 0 Watermark Reached */
#define MCAN_IR_RF0F (0x1u << 2) /**< \brief (MCAN_IR) Receive FIFO 0 Full */
#define MCAN_IR_RF0L (0x1u << 3) /**< \brief (MCAN_IR) Receive FIFO 0 Message Lost */
#define MCAN_IR_RF1N (0x1u << 4) /**< \brief (MCAN_IR) Receive FIFO 1 New Message */
#define MCAN_IR_RF1W (0x1u << 5) /**< \brief (MCAN_IR) Receive FIFO 1 Watermark Reached */
#define MCAN_IR_RF1F (0x1u << 6) /**< \brief (MCAN_IR) Receive FIFO 1 Full */
#define MCAN_IR_RF1L (0x1u << 7) /**< \brief (MCAN_IR) Receive FIFO 1 Message Lost */
#define MCAN_IR_HPM (0x1u << 8) /**< \brief (MCAN_IR) High Priority Message */
#define MCAN_IR_TC (0x1u << 9) /**< \brief (MCAN_IR) Transmission Completed */
#define MCAN_IR_TCF (0x1u << 10) /**< \brief (MCAN_IR) Transmission Cancellation Finished */
#define MCAN_IR_TFE (0x1u << 11) /**< \brief (MCAN_IR) Tx FIFO Empty */
#define MCAN_IR_TEFN (0x1u << 12) /**< \brief (MCAN_IR) Tx Event FIFO New Entry */
#define MCAN_IR_TEFW (0x1u << 13) /**< \brief (MCAN_IR) Tx Event FIFO Watermark Reached */
#define MCAN_IR_TEFF (0x1u << 14) /**< \brief (MCAN_IR) Tx Event FIFO Full */
#define MCAN_IR_TEFL (0x1u << 15) /**< \brief (MCAN_IR) Tx Event FIFO Element Lost */
#define MCAN_IR_TSW (0x1u << 16) /**< \brief (MCAN_IR) Timestamp Wraparound */
#define MCAN_IR_MRAF (0x1u << 17) /**< \brief (MCAN_IR) Message RAM Access Failure */
#define MCAN_IR_TOO (0x1u << 18) /**< \brief (MCAN_IR) Timeout Occurred */
#define MCAN_IR_DRX (0x1u << 19) /**< \brief (MCAN_IR) Message stored to Dedicated Receive Buffer */
#define MCAN_IR_ELO (0x1u << 22) /**< \brief (MCAN_IR) Error Logging Overflow */
#define MCAN_IR_EP (0x1u << 23) /**< \brief (MCAN_IR) Error Passive */
#define MCAN_IR_EW (0x1u << 24) /**< \brief (MCAN_IR) Warning Status */
#define MCAN_IR_BO (0x1u << 25) /**< \brief (MCAN_IR) Bus_Off Status */
#define MCAN_IR_WDI (0x1u << 26) /**< \brief (MCAN_IR) Watchdog Interrupt */
#define MCAN_IR_CRCE (0x1u << 27) /**< \brief (MCAN_IR) CRC Error */
#define MCAN_IR_BE (0x1u << 28) /**< \brief (MCAN_IR) Bit Error */
#define MCAN_IR_ACKE (0x1u << 29) /**< \brief (MCAN_IR) Acknowledge Error */
#define MCAN_IR_FOE (0x1u << 30) /**< \brief (MCAN_IR) Format Error */
#define MCAN_IR_STE (0x1u << 31) /**< \brief (MCAN_IR) Stuff Error */
#define MCAN_IR_PEA (0x1u << 27) /**< \brief (MCAN_IR) Protocol Error in Arbitration Phase */
#define MCAN_IR_PED (0x1u << 28) /**< \brief (MCAN_IR) Protocol Error in Data Phase */
#define MCAN_IR_ARA (0x1u << 29) /**< \brief (MCAN_IR) Access to Reserved Address */
/* -------- MCAN_IE : (MCAN Offset: 0x54) Interrupt Enable Register -------- */
#define MCAN_IE_RF0NE (0x1u << 0) /**< \brief (MCAN_IE) Receive FIFO 0 New Message Interrupt Enable */
#define MCAN_IE_RF0WE (0x1u << 1) /**< \brief (MCAN_IE) Receive FIFO 0 Watermark Reached Interrupt Enable */
#define MCAN_IE_RF0FE (0x1u << 2) /**< \brief (MCAN_IE) Receive FIFO 0 Full Interrupt Enable */
#define MCAN_IE_RF0LE (0x1u << 3) /**< \brief (MCAN_IE) Receive FIFO 0 Message Lost Interrupt Enable */
#define MCAN_IE_RF1NE (0x1u << 4) /**< \brief (MCAN_IE) Receive FIFO 1 New Message Interrupt Enable */
#define MCAN_IE_RF1WE (0x1u << 5) /**< \brief (MCAN_IE) Receive FIFO 1 Watermark Reached Interrupt Enable */
#define MCAN_IE_RF1FE (0x1u << 6) /**< \brief (MCAN_IE) Receive FIFO 1 Full Interrupt Enable */
#define MCAN_IE_RF1LE (0x1u << 7) /**< \brief (MCAN_IE) Receive FIFO 1 Message Lost Interrupt Enable */
#define MCAN_IE_HPME (0x1u << 8) /**< \brief (MCAN_IE) High Priority Message Interrupt Enable */
#define MCAN_IE_TCE (0x1u << 9) /**< \brief (MCAN_IE) Transmission Completed Interrupt Enable */
#define MCAN_IE_TCFE (0x1u << 10) /**< \brief (MCAN_IE) Transmission Cancellation Finished Interrupt Enable */
#define MCAN_IE_TFEE (0x1u << 11) /**< \brief (MCAN_IE) Tx FIFO Empty Interrupt Enable */
#define MCAN_IE_TEFNE (0x1u << 12) /**< \brief (MCAN_IE) Tx Event FIFO New Entry Interrupt Enable */
#define MCAN_IE_TEFWE (0x1u << 13) /**< \brief (MCAN_IE) Tx Event FIFO Watermark Reached Interrupt Enable */
#define MCAN_IE_TEFFE (0x1u << 14) /**< \brief (MCAN_IE) Tx Event FIFO Full Interrupt Enable */
#define MCAN_IE_TEFLE (0x1u << 15) /**< \brief (MCAN_IE) Tx Event FIFO Event Lost Interrupt Enable */
#define MCAN_IE_TSWE (0x1u << 16) /**< \brief (MCAN_IE) Timestamp Wraparound Interrupt Enable */
#define MCAN_IE_MRAFE (0x1u << 17) /**< \brief (MCAN_IE) Message RAM Access Failure Interrupt Enable */
#define MCAN_IE_TOOE (0x1u << 18) /**< \brief (MCAN_IE) Timeout Occurred Interrupt Enable */
#define MCAN_IE_DRXE (0x1u << 19) /**< \brief (MCAN_IE) Message stored to Dedicated Receive Buffer Interrupt Enable */
#define MCAN_IE_ELOE (0x1u << 22) /**< \brief (MCAN_IE) Error Logging Overflow Interrupt Enable */
#define MCAN_IE_EPE (0x1u << 23) /**< \brief (MCAN_IE) Error Passive Interrupt Enable */
#define MCAN_IE_EWE (0x1u << 24) /**< \brief (MCAN_IE) Warning Status Interrupt Enable */
#define MCAN_IE_BOE (0x1u << 25) /**< \brief (MCAN_IE) Bus_Off Status Interrupt Enable */
#define MCAN_IE_WDIE (0x1u << 26) /**< \brief (MCAN_IE) Watchdog Interrupt Enable */
#define MCAN_IE_CRCEE (0x1u << 27) /**< \brief (MCAN_IE) CRC Error Interrupt Enable */
#define MCAN_IE_BEE (0x1u << 28) /**< \brief (MCAN_IE) Bit Error Interrupt Enable */
#define MCAN_IE_ACKEE (0x1u << 29) /**< \brief (MCAN_IE) Acknowledge Error Interrupt Enable */
#define MCAN_IE_FOEE (0x1u << 30) /**< \brief (MCAN_IE) Format Error Interrupt Enable */
#define MCAN_IE_STEE (0x1u << 31) /**< \brief (MCAN_IE) Stuff Error Interrupt Enable */
#define MCAN_IE_PEAE (0x1u << 27) /**< \brief (MCAN_IE) Protocol Error in Arbitration Phase Enable */
#define MCAN_IE_PEDE (0x1u << 28) /**< \brief (MCAN_IE) Protocol Error in Data Phase Enable */
#define MCAN_IE_ARAE (0x1u << 29) /**< \brief (MCAN_IE) Access to Reserved Address Enable */
/* -------- MCAN_ILS : (MCAN Offset: 0x58) Interrupt Line Select Register -------- */
#define MCAN_ILS_RF0NL (0x1u << 0) /**< \brief (MCAN_ILS) Receive FIFO 0 New Message Interrupt Line */
#define MCAN_ILS_RF0WL (0x1u << 1) /**< \brief (MCAN_ILS) Receive FIFO 0 Watermark Reached Interrupt Line */
#define MCAN_ILS_RF0FL (0x1u << 2) /**< \brief (MCAN_ILS) Receive FIFO 0 Full Interrupt Line */
#define MCAN_ILS_RF0LL (0x1u << 3) /**< \brief (MCAN_ILS) Receive FIFO 0 Message Lost Interrupt Line */
#define MCAN_ILS_RF1NL (0x1u << 4) /**< \brief (MCAN_ILS) Receive FIFO 1 New Message Interrupt Line */
#define MCAN_ILS_RF1WL (0x1u << 5) /**< \brief (MCAN_ILS) Receive FIFO 1 Watermark Reached Interrupt Line */
#define MCAN_ILS_RF1FL (0x1u << 6) /**< \brief (MCAN_ILS) Receive FIFO 1 Full Interrupt Line */
#define MCAN_ILS_RF1LL (0x1u << 7) /**< \brief (MCAN_ILS) Receive FIFO 1 Message Lost Interrupt Line */
#define MCAN_ILS_HPML (0x1u << 8) /**< \brief (MCAN_ILS) High Priority Message Interrupt Line */
#define MCAN_ILS_TCL (0x1u << 9) /**< \brief (MCAN_ILS) Transmission Completed Interrupt Line */
#define MCAN_ILS_TCFL (0x1u << 10) /**< \brief (MCAN_ILS) Transmission Cancellation Finished Interrupt Line */
#define MCAN_ILS_TFEL (0x1u << 11) /**< \brief (MCAN_ILS) Tx FIFO Empty Interrupt Line */
#define MCAN_ILS_TEFNL (0x1u << 12) /**< \brief (MCAN_ILS) Tx Event FIFO New Entry Interrupt Line */
#define MCAN_ILS_TEFWL (0x1u << 13) /**< \brief (MCAN_ILS) Tx Event FIFO Watermark Reached Interrupt Line */
#define MCAN_ILS_TEFFL (0x1u << 14) /**< \brief (MCAN_ILS) Tx Event FIFO Full Interrupt Line */
#define MCAN_ILS_TEFLL (0x1u << 15) /**< \brief (MCAN_ILS) Tx Event FIFO Event Lost Interrupt Line */
#define MCAN_ILS_TSWL (0x1u << 16) /**< \brief (MCAN_ILS) Timestamp Wraparound Interrupt Line */
#define MCAN_ILS_MRAFL (0x1u << 17) /**< \brief (MCAN_ILS) Message RAM Access Failure Interrupt Line */
#define MCAN_ILS_TOOL (0x1u << 18) /**< \brief (MCAN_ILS) Timeout Occurred Interrupt Line */
#define MCAN_ILS_DRXL (0x1u << 19) /**< \brief (MCAN_ILS) Message stored to Dedicated Receive Buffer Interrupt Line */
#define MCAN_ILS_ELOL (0x1u << 22) /**< \brief (MCAN_ILS) Error Logging Overflow Interrupt Line */
#define MCAN_ILS_EPL (0x1u << 23) /**< \brief (MCAN_ILS) Error Passive Interrupt Line */
#define MCAN_ILS_EWL (0x1u << 24) /**< \brief (MCAN_ILS) Warning Status Interrupt Line */
#define MCAN_ILS_BOL (0x1u << 25) /**< \brief (MCAN_ILS) Bus_Off Status Interrupt Line */
#define MCAN_ILS_WDIL (0x1u << 26) /**< \brief (MCAN_ILS) Watchdog Interrupt Line */
#define MCAN_ILS_CRCEL (0x1u << 27) /**< \brief (MCAN_ILS) CRC Error Interrupt Line */
#define MCAN_ILS_BEL (0x1u << 28) /**< \brief (MCAN_ILS) Bit Error Interrupt Line */
#define MCAN_ILS_ACKEL (0x1u << 29) /**< \brief (MCAN_ILS) Acknowledge Error Interrupt Line */
#define MCAN_ILS_FOEL (0x1u << 30) /**< \brief (MCAN_ILS) Format Error Interrupt Line */
#define MCAN_ILS_STEL (0x1u << 31) /**< \brief (MCAN_ILS) Stuff Error Interrupt Line */
#define MCAN_ILS_PEAL (0x1u << 27) /**< \brief (MCAN_ILS) Protocol Error in Arbitration Phase Line */
#define MCAN_ILS_PEDL (0x1u << 28) /**< \brief (MCAN_ILS) Protocol Error in Data Phase Line */
#define MCAN_ILS_ARAL (0x1u << 29) /**< \brief (MCAN_ILS) Access to Reserved Address Line */
/* -------- MCAN_ILE : (MCAN Offset: 0x5C) Interrupt Line Enable Register -------- */
#define MCAN_ILE_EINT0 (0x1u << 0) /**< \brief (MCAN_ILE) Enable Interrupt Line 0 */
#define MCAN_ILE_EINT1 (0x1u << 1) /**< \brief (MCAN_ILE) Enable Interrupt Line 1 */
/* -------- MCAN_GFC : (MCAN Offset: 0x80) Global Filter Configuration Register -------- */
#define MCAN_GFC_RRFE (0x1u << 0) /**< \brief (MCAN_GFC) Reject Remote Frames Extended */
#define   MCAN_GFC_RRFE_FILTER (0x0u << 0) /**< \brief (MCAN_GFC) Filter remote frames with 29-bit extended IDs. */
#define   MCAN_GFC_RRFE_REJECT (0x1u << 0) /**< \brief (MCAN_GFC) Reject all remote frames with 29-bit extended IDs. */
#define MCAN_GFC_RRFS (0x1u << 1) /**< \brief (MCAN_GFC) Reject Remote Frames Standard */
#define   MCAN_GFC_RRFS_FILTER (0x0u << 1) /**< \brief (MCAN_GFC) Filter remote frames with 11-bit standard IDs. */
#define   MCAN_GFC_RRFS_REJECT (0x1u << 1) /**< \brief (MCAN_GFC) Reject all remote frames with 11-bit standard IDs. */
#define MCAN_GFC_ANFE_Pos 2
#define MCAN_GFC_ANFE_Msk (0x3u << MCAN_GFC_ANFE_Pos) /**< \brief (MCAN_GFC) Accept Non-matching Frames Extended */
#define MCAN_GFC_ANFE(value) ((MCAN_GFC_ANFE_Msk & ((value) << MCAN_GFC_ANFE_Pos)))
#define   MCAN_GFC_ANFE_RX_FIFO_0 (0x0u << 2) /**< \brief (MCAN_GFC) Message stored in Receive FIFO 0 */
#define   MCAN_GFC_ANFE_RX_FIFO_1 (0x1u << 2) /**< \brief (MCAN_GFC) Message stored in Receive FIFO 1 */
#define MCAN_GFC_ANFS_Pos 4
#define MCAN_GFC_ANFS_Msk (0x3u << MCAN_GFC_ANFS_Pos) /**< \brief (MCAN_GFC) Accept Non-matching Frames Standard */
#define MCAN_GFC_ANFS(value) ((MCAN_GFC_ANFS_Msk & ((value) << MCAN_GFC_ANFS_Pos)))
#define   MCAN_GFC_ANFS_RX_FIFO_0 (0x0u << 4) /**< \brief (MCAN_GFC) Message stored in Receive FIFO 0 */
#define   MCAN_GFC_ANFS_RX_FIFO_1 (0x1u << 4) /**< \brief (MCAN_GFC) Message stored in Receive FIFO 1 */
/* -------- MCAN_SIDFC : (MCAN Offset: 0x84) Standard ID Filter Configuration Register -------- */
#define MCAN_SIDFC_FLSSA_Pos 2
#define MCAN_SIDFC_FLSSA_Msk (0x3fffu << MCAN_SIDFC_FLSSA_Pos) /**< \brief (MCAN_SIDFC) Filter List Standard Start Address */
#define MCAN_SIDFC_FLSSA(value) ((MCAN_SIDFC_FLSSA_Msk & ((value) << MCAN_SIDFC_FLSSA_Pos)))
#define MCAN_SIDFC_LSS_Pos 16
#define MCAN_SIDFC_LSS_Msk (0xffu << MCAN_SIDFC_LSS_Pos) /**< \brief (MCAN_SIDFC) List Size Standard */
#define MCAN_SIDFC_LSS(value) ((MCAN_SIDFC_LSS_Msk & ((value) << MCAN_SIDFC_LSS_Pos)))
/* -------- MCAN_XIDFC : (MCAN Offset: 0x88) Extended ID Filter Configuration Register -------- */
#define MCAN_XIDFC_FLESA_Pos 2
#define MCAN_XIDFC_FLESA_Msk (0x3fffu << MCAN_XIDFC_FLESA_Pos) /**< \brief (MCAN_XIDFC) Filter List Extended Start Address */
#define MCAN_XIDFC_FLESA(value) ((MCAN_XIDFC_FLESA_Msk & ((value) << MCAN_XIDFC_FLESA_Pos)))
#define MCAN_XIDFC_LSE_Pos 16
#define MCAN_XIDFC_LSE_Msk (0x7fu << MCAN_XIDFC_LSE_Pos) /**< \brief (MCAN_XIDFC) List Size Extended */
#define MCAN_XIDFC_LSE(value) ((MCAN_XIDFC_LSE_Msk & ((value) << MCAN_XIDFC_LSE_Pos)))
/* -------- MCAN_XIDAM : (MCAN Offset: 0x90) Extended ID AND Mask Register -------- */
#define MCAN_XIDAM_EIDM_Pos 0
#define MCAN_XIDAM_EIDM_Msk (0x1fffffffu << MCAN_XIDAM_EIDM_Pos) /**< \brief (MCAN_XIDAM) Extended ID Mask */
#define MCAN_XIDAM_EIDM(value) ((MCAN_XIDAM_EIDM_Msk & ((value) << MCAN_XIDAM_EIDM_Pos)))
/* -------- MCAN_HPMS : (MCAN Offset: 0x94) High Priority Message Status Register -------- */
#define MCAN_HPMS_BIDX_Pos 0
#define MCAN_HPMS_BIDX_Msk (0x3fu << MCAN_HPMS_BIDX_Pos) /**< \brief (MCAN_HPMS) Buffer Index */
#define MCAN_HPMS_MSI_Pos 6
#define MCAN_HPMS_MSI_Msk (0x3u << MCAN_HPMS_MSI_Pos) /**< \brief (MCAN_HPMS) Message Storage Indicator */
#define   MCAN_HPMS_MSI_NO_FIFO_SEL (0x0u << 6) /**< \brief (MCAN_HPMS) No FIFO selected. */
#define   MCAN_HPMS_MSI_LOST (0x1u << 6) /**< \brief (MCAN_HPMS) FIFO message lost. */
#define   MCAN_HPMS_MSI_FIFO_0 (0x2u << 6) /**< \brief (MCAN_HPMS) Message stored in FIFO 0. */
#define   MCAN_HPMS_MSI_FIFO_1 (0x3u << 6) /**< \brief (MCAN_HPMS) Message stored in FIFO 1. */
#define MCAN_HPMS_FIDX_Pos 8
#define MCAN_HPMS_FIDX_Msk (0x7fu << MCAN_HPMS_FIDX_Pos) /**< \brief (MCAN_HPMS) Filter Index */
#define MCAN_HPMS_FLST (0x1u << 15) /**< \brief (MCAN_HPMS) Filter List */
/* -------- MCAN_NDAT1 : (MCAN Offset: 0x98) New Data 1 Register -------- */
#define MCAN_NDAT1_ND0 (0x1u << 0) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND1 (0x1u << 1) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND2 (0x1u << 2) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND3 (0x1u << 3) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND4 (0x1u << 4) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND5 (0x1u << 5) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND6 (0x1u << 6) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND7 (0x1u << 7) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND8 (0x1u << 8) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND9 (0x1u << 9) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND10 (0x1u << 10) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND11 (0x1u << 11) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND12 (0x1u << 12) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND13 (0x1u << 13) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND14 (0x1u << 14) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND15 (0x1u << 15) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND16 (0x1u << 16) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND17 (0x1u << 17) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND18 (0x1u << 18) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND19 (0x1u << 19) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND20 (0x1u << 20) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND21 (0x1u << 21) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND22 (0x1u << 22) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND23 (0x1u << 23) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND24 (0x1u << 24) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND25 (0x1u << 25) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND26 (0x1u << 26) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND27 (0x1u << 27) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND28 (0x1u << 28) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND29 (0x1u << 29) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND30 (0x1u << 30) /**< \brief (MCAN_NDAT1) New Data */
#define MCAN_NDAT1_ND31 (0x1u << 31) /**< \brief (MCAN_NDAT1) New Data */
/* -------- MCAN_NDAT2 : (MCAN Offset: 0x9C) New Data 2 Register -------- */
#define MCAN_NDAT2_ND32 (0x1u << 0) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND33 (0x1u << 1) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND34 (0x1u << 2) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND35 (0x1u << 3) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND36 (0x1u << 4) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND37 (0x1u << 5) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND38 (0x1u << 6) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND39 (0x1u << 7) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND40 (0x1u << 8) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND41 (0x1u << 9) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND42 (0x1u << 10) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND43 (0x1u << 11) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND44 (0x1u << 12) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND45 (0x1u << 13) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND46 (0x1u << 14) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND47 (0x1u << 15) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND48 (0x1u << 16) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND49 (0x1u << 17) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND50 (0x1u << 18) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND51 (0x1u << 19) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND52 (0x1u << 20) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND53 (0x1u << 21) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND54 (0x1u << 22) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND55 (0x1u << 23) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND56 (0x1u << 24) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND57 (0x1u << 25) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND58 (0x1u << 26) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND59 (0x1u << 27) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND60 (0x1u << 28) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND61 (0x1u << 29) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND62 (0x1u << 30) /**< \brief (MCAN_NDAT2) New Data */
#define MCAN_NDAT2_ND63 (0x1u << 31) /**< \brief (MCAN_NDAT2) New Data */
/* -------- MCAN_RXF0C : (MCAN Offset: 0xA0) Receive FIFO 0 Configuration Register -------- */
#define MCAN_RXF0C_F0SA_Pos 2
#define MCAN_RXF0C_F0SA_Msk (0x3fffu << MCAN_RXF0C_F0SA_Pos) /**< \brief (MCAN_RXF0C) Receive FIFO 0 Start Address */
#define MCAN_RXF0C_F0SA(value) ((MCAN_RXF0C_F0SA_Msk & ((value) << MCAN_RXF0C_F0SA_Pos)))
#define MCAN_RXF0C_F0S_Pos 16
#define MCAN_RXF0C_F0S_Msk (0x7fu << MCAN_RXF0C_F0S_Pos) /**< \brief (MCAN_RXF0C) Receive FIFO 0 Start Address */
#define MCAN_RXF0C_F0S(value) ((MCAN_RXF0C_F0S_Msk & ((value) << MCAN_RXF0C_F0S_Pos)))
#define MCAN_RXF0C_F0WM_Pos 24
#define MCAN_RXF0C_F0WM_Msk (0x7fu << MCAN_RXF0C_F0WM_Pos) /**< \brief (MCAN_RXF0C) Receive FIFO 0 Watermark */
#define MCAN_RXF0C_F0WM(value) ((MCAN_RXF0C_F0WM_Msk & ((value) << MCAN_RXF0C_F0WM_Pos)))
#define MCAN_RXF0C_F0OM (0x1u << 31) /**< \brief (MCAN_RXF0C) FIFO 0 Operation Mode */
/* -------- MCAN_RXF0S : (MCAN Offset: 0xA4) Receive FIFO 0 Status Register -------- */
#define MCAN_RXF0S_F0FL_Pos 0
#define MCAN_RXF0S_F0FL_Msk (0x7fu << MCAN_RXF0S_F0FL_Pos) /**< \brief (MCAN_RXF0S) Receive FIFO 0 Fill Level */
#define MCAN_RXF0S_F0GI_Pos 8
#define MCAN_RXF0S_F0GI_Msk (0x3fu << MCAN_RXF0S_F0GI_Pos) /**< \brief (MCAN_RXF0S) Receive FIFO 0 Get Index */
#define MCAN_RXF0S_F0PI_Pos 16
#define MCAN_RXF0S_F0PI_Msk (0x3fu << MCAN_RXF0S_F0PI_Pos) /**< \brief (MCAN_RXF0S) Receive FIFO 0 Put Index */
#define MCAN_RXF0S_F0F (0x1u << 24) /**< \brief (MCAN_RXF0S) Receive FIFO 0 Fill Level */
#define MCAN_RXF0S_RF0L (0x1u << 25) /**< \brief (MCAN_RXF0S) Receive FIFO 0 Message Lost */
/* -------- MCAN_RXF0A : (MCAN Offset: 0xA8) Receive FIFO 0 Acknowledge Register -------- */
#define MCAN_RXF0A_F0AI_Pos 0
#define MCAN_RXF0A_F0AI_Msk (0x3fu << MCAN_RXF0A_F0AI_Pos) /**< \brief (MCAN_RXF0A) Receive FIFO 0 Acknowledge Index */
#define MCAN_RXF0A_F0AI(value) ((MCAN_RXF0A_F0AI_Msk & ((value) << MCAN_RXF0A_F0AI_Pos)))
/* -------- MCAN_RXBC : (MCAN Offset: 0xAC) Receive Rx Buffer Configuration Register -------- */
#define MCAN_RXBC_RBSA_Pos 2
#define MCAN_RXBC_RBSA_Msk (0x3fffu << MCAN_RXBC_RBSA_Pos) /**< \brief (MCAN_RXBC) Receive Buffer Start Address */
#define MCAN_RXBC_RBSA(value) ((MCAN_RXBC_RBSA_Msk & ((value) << MCAN_RXBC_RBSA_Pos)))
/* -------- MCAN_RXF1C : (MCAN Offset: 0xB0) Receive FIFO 1 Configuration Register -------- */
#define MCAN_RXF1C_F1SA_Pos 2
#define MCAN_RXF1C_F1SA_Msk (0x3fffu << MCAN_RXF1C_F1SA_Pos) /**< \brief (MCAN_RXF1C) Receive FIFO 1 Start Address */
#define MCAN_RXF1C_F1SA(value) ((MCAN_RXF1C_F1SA_Msk & ((value) << MCAN_RXF1C_F1SA_Pos)))
#define MCAN_RXF1C_F1S_Pos 16
#define MCAN_RXF1C_F1S_Msk (0x7fu << MCAN_RXF1C_F1S_Pos) /**< \brief (MCAN_RXF1C) Receive FIFO 1 Start Address */
#define MCAN_RXF1C_F1S(value) ((MCAN_RXF1C_F1S_Msk & ((value) << MCAN_RXF1C_F1S_Pos)))
#define MCAN_RXF1C_F1WM_Pos 24
#define MCAN_RXF1C_F1WM_Msk (0x7fu << MCAN_RXF1C_F1WM_Pos) /**< \brief (MCAN_RXF1C) Receive FIFO 1 Watermark */
#define MCAN_RXF1C_F1WM(value) ((MCAN_RXF1C_F1WM_Msk & ((value) << MCAN_RXF1C_F1WM_Pos)))
#define MCAN_RXF1C_F1OM (0x1u << 31) /**< \brief (MCAN_RXF1C) FIFO 1 Operation Mode */
/* -------- MCAN_RXF1S : (MCAN Offset: 0xB4) Receive FIFO 1 Status Register -------- */
#define MCAN_RXF1S_F1FL_Pos 0
#define MCAN_RXF1S_F1FL_Msk (0x7fu << MCAN_RXF1S_F1FL_Pos) /**< \brief (MCAN_RXF1S) Receive FIFO 1 Fill Level */
#define MCAN_RXF1S_F1GI_Pos 8
#define MCAN_RXF1S_F1GI_Msk (0x3fu << MCAN_RXF1S_F1GI_Pos) /**< \brief (MCAN_RXF1S) Receive FIFO 1 Get Index */
#define MCAN_RXF1S_F1PI_Pos 16
#define MCAN_RXF1S_F1PI_Msk (0x3fu << MCAN_RXF1S_F1PI_Pos) /**< \brief (MCAN_RXF1S) Receive FIFO 1 Put Index */
#define MCAN_RXF1S_F1F (0x1u << 24) /**< \brief (MCAN_RXF1S) Receive FIFO 1 Fill Level */
#define MCAN_RXF1S_RF1L (0x1u << 25) /**< \brief (MCAN_RXF1S) Receive FIFO 1 Message Lost */
#define MCAN_RXF1S_DMS_Pos 30
#define MCAN_RXF1S_DMS_Msk (0x3u << MCAN_RXF1S_DMS_Pos) /**< \brief (MCAN_RXF1S) Debug Message Status */
#define   MCAN_RXF1S_DMS_IDLE (0x0u << 30) /**< \brief (MCAN_RXF1S) Idle state, wait for reception of debug messages, DMA request is cleared. */
#define   MCAN_RXF1S_DMS_MSG_A (0x1u << 30) /**< \brief (MCAN_RXF1S) Debug message A received. */
#define   MCAN_RXF1S_DMS_MSG_AB (0x2u << 30) /**< \brief (MCAN_RXF1S) Debug messages A, B received. */
#define   MCAN_RXF1S_DMS_MSG_ABC (0x3u << 30) /**< \brief (MCAN_RXF1S) Debug messages A, B, C received, DMA request is set. */
/* -------- MCAN_RXF1A : (MCAN Offset: 0xB8) Receive FIFO 1 Acknowledge Register -------- */
#define MCAN_RXF1A_F1AI_Pos 0
#define MCAN_RXF1A_F1AI_Msk (0x3fu << MCAN_RXF1A_F1AI_Pos) /**< \brief (MCAN_RXF1A) Receive FIFO 1 Acknowledge Index */
#define MCAN_RXF1A_F1AI(value) ((MCAN_RXF1A_F1AI_Msk & ((value) << MCAN_RXF1A_F1AI_Pos)))
/* -------- MCAN_RXESC : (MCAN Offset: 0xBC) Receive Buffer / FIFO Element Size Configuration Register -------- */
#define MCAN_RXESC_F0DS_Pos 0
#define MCAN_RXESC_F0DS_Msk (0x7u << MCAN_RXESC_F0DS_Pos) /**< \brief (MCAN_RXESC) Receive FIFO 0 Data Field Size */
#define MCAN_RXESC_F0DS(value) ((MCAN_RXESC_F0DS_Msk & ((value) << MCAN_RXESC_F0DS_Pos)))
#define   MCAN_RXESC_F0DS_8_BYTE (0x0u << 0) /**< \brief (MCAN_RXESC) 8-byte data field */
#define   MCAN_RXESC_F0DS_12_BYTE (0x1u << 0) /**< \brief (MCAN_RXESC) 12-byte data field */
#define   MCAN_RXESC_F0DS_16_BYTE (0x2u << 0) /**< \brief (MCAN_RXESC) 16-byte data field */
#define   MCAN_RXESC_F0DS_20_BYTE (0x3u << 0) /**< \brief (MCAN_RXESC) 20-byte data field */
#define   MCAN_RXESC_F0DS_24_BYTE (0x4u << 0) /**< \brief (MCAN_RXESC) 24-byte data field */
#define   MCAN_RXESC_F0DS_32_BYTE (0x5u << 0) /**< \brief (MCAN_RXESC) 32-byte data field */
#define   MCAN_RXESC_F0DS_48_BYTE (0x6u << 0) /**< \brief (MCAN_RXESC) 48-byte data field */
#define   MCAN_RXESC_F0DS_64_BYTE (0x7u << 0) /**< \brief (MCAN_RXESC) 64-byte data field */
#define MCAN_RXESC_F1DS_Pos 4
#define MCAN_RXESC_F1DS_Msk (0x7u << MCAN_RXESC_F1DS_Pos) /**< \brief (MCAN_RXESC) Receive FIFO 1 Data Field Size */
#define MCAN_RXESC_F1DS(value) ((MCAN_RXESC_F1DS_Msk & ((value) << MCAN_RXESC_F1DS_Pos)))
#define   MCAN_RXESC_F1DS_8_BYTE (0x0u << 4) /**< \brief (MCAN_RXESC) 8-byte data field */
#define   MCAN_RXESC_F1DS_12_BYTE (0x1u << 4) /**< \brief (MCAN_RXESC) 12-byte data field */
#define   MCAN_RXESC_F1DS_16_BYTE (0x2u << 4) /**< \brief (MCAN_RXESC) 16-byte data field */
#define   MCAN_RXESC_F1DS_20_BYTE (0x3u << 4) /**< \brief (MCAN_RXESC) 20-byte data field */
#define   MCAN_RXESC_F1DS_24_BYTE (0x4u << 4) /**< \brief (MCAN_RXESC) 24-byte data field */
#define   MCAN_RXESC_F1DS_32_BYTE (0x5u << 4) /**< \brief (MCAN_RXESC) 32-byte data field */
#define   MCAN_RXESC_F1DS_48_BYTE (0x6u << 4) /**< \brief (MCAN_RXESC) 48-byte data field */
#define   MCAN_RXESC_F1DS_64_BYTE (0x7u << 4) /**< \brief (MCAN_RXESC) 64-byte data field */
#define MCAN_RXESC_RBDS_Pos 8
#define MCAN_RXESC_RBDS_Msk (0x7u << MCAN_RXESC_RBDS_Pos) /**< \brief (MCAN_RXESC) Receive Buffer Data Field Size */
#define MCAN_RXESC_RBDS(value) ((MCAN_RXESC_RBDS_Msk & ((value) << MCAN_RXESC_RBDS_Pos)))
#define   MCAN_RXESC_RBDS_8_BYTE (0x0u << 8) /**< \brief (MCAN_RXESC) 8-byte data field */
#define   MCAN_RXESC_RBDS_12_BYTE (0x1u << 8) /**< \brief (MCAN_RXESC) 12-byte data field */
#define   MCAN_RXESC_RBDS_16_BYTE (0x2u << 8) /**< \brief (MCAN_RXESC) 16-byte data field */
#define   MCAN_RXESC_RBDS_20_BYTE (0x3u << 8) /**< \brief (MCAN_RXESC) 20-byte data field */
#define   MCAN_RXESC_RBDS_24_BYTE (0x4u << 8) /**< \brief (MCAN_RXESC) 24-byte data field */
#define   MCAN_RXESC_RBDS_32_BYTE (0x5u << 8) /**< \brief (MCAN_RXESC) 32-byte data field */
#define   MCAN_RXESC_RBDS_48_BYTE (0x6u << 8) /**< \brief (MCAN_RXESC) 48-byte data field */
#define   MCAN_RXESC_RBDS_64_BYTE (0x7u << 8) /**< \brief (MCAN_RXESC) 64-byte data field */
/* -------- MCAN_TXBC : (MCAN Offset: 0xC0) Transmit Buffer Configuration Register -------- */
#define MCAN_TXBC_TBSA_Pos 2
#define MCAN_TXBC_TBSA_Msk (0x3fffu << MCAN_TXBC_TBSA_Pos) /**< \brief (MCAN_TXBC) Tx Buffers Start Address */
#define MCAN_TXBC_TBSA(value) ((MCAN_TXBC_TBSA_Msk & ((value) << MCAN_TXBC_TBSA_Pos)))
#define MCAN_TXBC_NDTB_Pos 16
#define MCAN_TXBC_NDTB_Msk (0x3fu << MCAN_TXBC_NDTB_Pos) /**< \brief (MCAN_TXBC) Number of Dedicated Transmit Buffers */
#define MCAN_TXBC_NDTB(value) ((MCAN_TXBC_NDTB_Msk & ((value) << MCAN_TXBC_NDTB_Pos)))
#define MCAN_TXBC_TFQS_Pos 24
#define MCAN_TXBC_TFQS_Msk (0x3fu << MCAN_TXBC_TFQS_Pos) /**< \brief (MCAN_TXBC) Transmit FIFO/Queue Size */
#define MCAN_TXBC_TFQS(value) ((MCAN_TXBC_TFQS_Msk & ((value) << MCAN_TXBC_TFQS_Pos)))
#define MCAN_TXBC_TFQM (0x1u << 30) /**< \brief (MCAN_TXBC) Tx FIFO/Queue Mode */
/* -------- MCAN_TXFQS : (MCAN Offset: 0xC4) Transmit FIFO/Queue Status Register -------- */
#define MCAN_TXFQS_TFFL_Pos 0
#define MCAN_TXFQS_TFFL_Msk (0x3fu << MCAN_TXFQS_TFFL_Pos) /**< \brief (MCAN_TXFQS) Tx FIFO Free Level */
#define MCAN_TXFQS_TFGI_Pos 8
#define MCAN_TXFQS_TFGI_Msk (0x1fu << MCAN_TXFQS_TFGI_Pos) /**< \brief (MCAN_TXFQS) Tx FIFO Get Index */
#define MCAN_TXFQS_TFQPI_Pos 16
#define MCAN_TXFQS_TFQPI_Msk (0x1fu << MCAN_TXFQS_TFQPI_Pos) /**< \brief (MCAN_TXFQS) Tx FIFO/Queue Put Index */
#define MCAN_TXFQS_TFQF (0x1u << 21) /**< \brief (MCAN_TXFQS) Tx FIFO/Queue Full */
/* -------- MCAN_TXESC : (MCAN Offset: 0xC8) Transmit Buffer Element Size Configuration Register -------- */
#define MCAN_TXESC_TBDS_Pos 0
#define MCAN_TXESC_TBDS_Msk (0x7u << MCAN_TXESC_TBDS_Pos) /**< \brief (MCAN_TXESC) Tx Buffer Data Field Size */
#define MCAN_TXESC_TBDS(value) ((MCAN_TXESC_TBDS_Msk & ((value) << MCAN_TXESC_TBDS_Pos)))
#define   MCAN_TXESC_TBDS_8_BYTE (0x0u << 0) /**< \brief (MCAN_TXESC) 8-byte data field */
#define   MCAN_TXESC_TBDS_12_BYTE (0x1u << 0) /**< \brief (MCAN_TXESC) 12-byte data field */
#define   MCAN_TXESC_TBDS_16_BYTE (0x2u << 0) /**< \brief (MCAN_TXESC) 16-byte data field */
#define   MCAN_TXESC_TBDS_20_BYTE (0x3u << 0) /**< \brief (MCAN_TXESC) 20-byte data field */
#define   MCAN_TXESC_TBDS_24_BYTE (0x4u << 0) /**< \brief (MCAN_TXESC) 24-byte data field */
#define   MCAN_TXESC_TBDS_32_BYTE (0x5u << 0) /**< \brief (MCAN_TXESC) 32-byte data field */
#define   MCAN_TXESC_TBDS_48_BYTE (0x6u << 0) /**< \brief (MCAN_TXESC) 48- byte data field */
#define   MCAN_TXESC_TBDS_64_BYTE (0x7u << 0) /**< \brief (MCAN_TXESC) 64-byte data field */
/* -------- MCAN_TXBRP : (MCAN Offset: 0xCC) Transmit Buffer Request Pending Register -------- */
#define MCAN_TXBRP_TRP0 (0x1u << 0) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 0 */
#define MCAN_TXBRP_TRP1 (0x1u << 1) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 1 */
#define MCAN_TXBRP_TRP2 (0x1u << 2) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 2 */
#define MCAN_TXBRP_TRP3 (0x1u << 3) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 3 */
#define MCAN_TXBRP_TRP4 (0x1u << 4) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 4 */
#define MCAN_TXBRP_TRP5 (0x1u << 5) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 5 */
#define MCAN_TXBRP_TRP6 (0x1u << 6) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 6 */
#define MCAN_TXBRP_TRP7 (0x1u << 7) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 7 */
#define MCAN_TXBRP_TRP8 (0x1u << 8) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 8 */
#define MCAN_TXBRP_TRP9 (0x1u << 9) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 9 */
#define MCAN_TXBRP_TRP10 (0x1u << 10) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 10 */
#define MCAN_TXBRP_TRP11 (0x1u << 11) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 11 */
#define MCAN_TXBRP_TRP12 (0x1u << 12) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 12 */
#define MCAN_TXBRP_TRP13 (0x1u << 13) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 13 */
#define MCAN_TXBRP_TRP14 (0x1u << 14) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 14 */
#define MCAN_TXBRP_TRP15 (0x1u << 15) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 15 */
#define MCAN_TXBRP_TRP16 (0x1u << 16) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 16 */
#define MCAN_TXBRP_TRP17 (0x1u << 17) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 17 */
#define MCAN_TXBRP_TRP18 (0x1u << 18) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 18 */
#define MCAN_TXBRP_TRP19 (0x1u << 19) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 19 */
#define MCAN_TXBRP_TRP20 (0x1u << 20) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 20 */
#define MCAN_TXBRP_TRP21 (0x1u << 21) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 21 */
#define MCAN_TXBRP_TRP22 (0x1u << 22) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 22 */
#define MCAN_TXBRP_TRP23 (0x1u << 23) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 23 */
#define MCAN_TXBRP_TRP24 (0x1u << 24) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 24 */
#define MCAN_TXBRP_TRP25 (0x1u << 25) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 25 */
#define MCAN_TXBRP_TRP26 (0x1u << 26) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 26 */
#define MCAN_TXBRP_TRP27 (0x1u << 27) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 27 */
#define MCAN_TXBRP_TRP28 (0x1u << 28) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 28 */
#define MCAN_TXBRP_TRP29 (0x1u << 29) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 29 */
#define MCAN_TXBRP_TRP30 (0x1u << 30) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 30 */
#define MCAN_TXBRP_TRP31 (0x1u << 31) /**< \brief (MCAN_TXBRP) Transmission Request Pending for Buffer 31 */
/* -------- MCAN_TXBAR : (MCAN Offset: 0xD0) Transmit Buffer Add Request Register -------- */
#define MCAN_TXBAR_AR0 (0x1u << 0) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 0 */
#define MCAN_TXBAR_AR1 (0x1u << 1) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 1 */
#define MCAN_TXBAR_AR2 (0x1u << 2) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 2 */
#define MCAN_TXBAR_AR3 (0x1u << 3) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 3 */
#define MCAN_TXBAR_AR4 (0x1u << 4) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 4 */
#define MCAN_TXBAR_AR5 (0x1u << 5) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 5 */
#define MCAN_TXBAR_AR6 (0x1u << 6) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 6 */
#define MCAN_TXBAR_AR7 (0x1u << 7) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 7 */
#define MCAN_TXBAR_AR8 (0x1u << 8) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 8 */
#define MCAN_TXBAR_AR9 (0x1u << 9) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 9 */
#define MCAN_TXBAR_AR10 (0x1u << 10) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 10 */
#define MCAN_TXBAR_AR11 (0x1u << 11) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 11 */
#define MCAN_TXBAR_AR12 (0x1u << 12) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 12 */
#define MCAN_TXBAR_AR13 (0x1u << 13) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 13 */
#define MCAN_TXBAR_AR14 (0x1u << 14) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 14 */
#define MCAN_TXBAR_AR15 (0x1u << 15) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 15 */
#define MCAN_TXBAR_AR16 (0x1u << 16) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 16 */
#define MCAN_TXBAR_AR17 (0x1u << 17) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 17 */
#define MCAN_TXBAR_AR18 (0x1u << 18) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 18 */
#define MCAN_TXBAR_AR19 (0x1u << 19) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 19 */
#define MCAN_TXBAR_AR20 (0x1u << 20) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 20 */
#define MCAN_TXBAR_AR21 (0x1u << 21) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 21 */
#define MCAN_TXBAR_AR22 (0x1u << 22) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 22 */
#define MCAN_TXBAR_AR23 (0x1u << 23) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 23 */
#define MCAN_TXBAR_AR24 (0x1u << 24) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 24 */
#define MCAN_TXBAR_AR25 (0x1u << 25) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 25 */
#define MCAN_TXBAR_AR26 (0x1u << 26) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 26 */
#define MCAN_TXBAR_AR27 (0x1u << 27) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 27 */
#define MCAN_TXBAR_AR28 (0x1u << 28) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 28 */
#define MCAN_TXBAR_AR29 (0x1u << 29) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 29 */
#define MCAN_TXBAR_AR30 (0x1u << 30) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 30 */
#define MCAN_TXBAR_AR31 (0x1u << 31) /**< \brief (MCAN_TXBAR) Add Request for Transmit Buffer 31 */
/* -------- MCAN_TXBCR : (MCAN Offset: 0xD4) Transmit Buffer Cancellation Request Register -------- */
#define MCAN_TXBCR_CR0 (0x1u << 0) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 0 */
#define MCAN_TXBCR_CR1 (0x1u << 1) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 1 */
#define MCAN_TXBCR_CR2 (0x1u << 2) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 2 */
#define MCAN_TXBCR_CR3 (0x1u << 3) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 3 */
#define MCAN_TXBCR_CR4 (0x1u << 4) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 4 */
#define MCAN_TXBCR_CR5 (0x1u << 5) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 5 */
#define MCAN_TXBCR_CR6 (0x1u << 6) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 6 */
#define MCAN_TXBCR_CR7 (0x1u << 7) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 7 */
#define MCAN_TXBCR_CR8 (0x1u << 8) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 8 */
#define MCAN_TXBCR_CR9 (0x1u << 9) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 9 */
#define MCAN_TXBCR_CR10 (0x1u << 10) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 10 */
#define MCAN_TXBCR_CR11 (0x1u << 11) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 11 */
#define MCAN_TXBCR_CR12 (0x1u << 12) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 12 */
#define MCAN_TXBCR_CR13 (0x1u << 13) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 13 */
#define MCAN_TXBCR_CR14 (0x1u << 14) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 14 */
#define MCAN_TXBCR_CR15 (0x1u << 15) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 15 */
#define MCAN_TXBCR_CR16 (0x1u << 16) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 16 */
#define MCAN_TXBCR_CR17 (0x1u << 17) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 17 */
#define MCAN_TXBCR_CR18 (0x1u << 18) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 18 */
#define MCAN_TXBCR_CR19 (0x1u << 19) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 19 */
#define MCAN_TXBCR_CR20 (0x1u << 20) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 20 */
#define MCAN_TXBCR_CR21 (0x1u << 21) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 21 */
#define MCAN_TXBCR_CR22 (0x1u << 22) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 22 */
#define MCAN_TXBCR_CR23 (0x1u << 23) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 23 */
#define MCAN_TXBCR_CR24 (0x1u << 24) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 24 */
#define MCAN_TXBCR_CR25 (0x1u << 25) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 25 */
#define MCAN_TXBCR_CR26 (0x1u << 26) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 26 */
#define MCAN_TXBCR_CR27 (0x1u << 27) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 27 */
#define MCAN_TXBCR_CR28 (0x1u << 28) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 28 */
#define MCAN_TXBCR_CR29 (0x1u << 29) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 29 */
#define MCAN_TXBCR_CR30 (0x1u << 30) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 30 */
#define MCAN_TXBCR_CR31 (0x1u << 31) /**< \brief (MCAN_TXBCR) Cancellation Request for Transmit Buffer 31 */
/* -------- MCAN_TXBTO : (MCAN Offset: 0xD8) Transmit Buffer Transmission Occurred Register -------- */
#define MCAN_TXBTO_TO0 (0x1u << 0) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 0 */
#define MCAN_TXBTO_TO1 (0x1u << 1) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 1 */
#define MCAN_TXBTO_TO2 (0x1u << 2) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 2 */
#define MCAN_TXBTO_TO3 (0x1u << 3) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 3 */
#define MCAN_TXBTO_TO4 (0x1u << 4) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 4 */
#define MCAN_TXBTO_TO5 (0x1u << 5) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 5 */
#define MCAN_TXBTO_TO6 (0x1u << 6) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 6 */
#define MCAN_TXBTO_TO7 (0x1u << 7) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 7 */
#define MCAN_TXBTO_TO8 (0x1u << 8) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 8 */
#define MCAN_TXBTO_TO9 (0x1u << 9) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 9 */
#define MCAN_TXBTO_TO10 (0x1u << 10) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 10 */
#define MCAN_TXBTO_TO11 (0x1u << 11) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 11 */
#define MCAN_TXBTO_TO12 (0x1u << 12) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 12 */
#define MCAN_TXBTO_TO13 (0x1u << 13) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 13 */
#define MCAN_TXBTO_TO14 (0x1u << 14) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 14 */
#define MCAN_TXBTO_TO15 (0x1u << 15) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 15 */
#define MCAN_TXBTO_TO16 (0x1u << 16) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 16 */
#define MCAN_TXBTO_TO17 (0x1u << 17) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 17 */
#define MCAN_TXBTO_TO18 (0x1u << 18) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 18 */
#define MCAN_TXBTO_TO19 (0x1u << 19) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 19 */
#define MCAN_TXBTO_TO20 (0x1u << 20) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 20 */
#define MCAN_TXBTO_TO21 (0x1u << 21) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 21 */
#define MCAN_TXBTO_TO22 (0x1u << 22) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 22 */
#define MCAN_TXBTO_TO23 (0x1u << 23) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 23 */
#define MCAN_TXBTO_TO24 (0x1u << 24) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 24 */
#define MCAN_TXBTO_TO25 (0x1u << 25) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 25 */
#define MCAN_TXBTO_TO26 (0x1u << 26) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 26 */
#define MCAN_TXBTO_TO27 (0x1u << 27) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 27 */
#define MCAN_TXBTO_TO28 (0x1u << 28) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 28 */
#define MCAN_TXBTO_TO29 (0x1u << 29) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 29 */
#define MCAN_TXBTO_TO30 (0x1u << 30) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 30 */
#define MCAN_TXBTO_TO31 (0x1u << 31) /**< \brief (MCAN_TXBTO) Transmission Occurred for Buffer 31 */
/* -------- MCAN_TXBCF : (MCAN Offset: 0xDC) Transmit Buffer Cancellation Finished Register -------- */
#define MCAN_TXBCF_CF0 (0x1u << 0) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 0 */
#define MCAN_TXBCF_CF1 (0x1u << 1) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 1 */
#define MCAN_TXBCF_CF2 (0x1u << 2) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 2 */
#define MCAN_TXBCF_CF3 (0x1u << 3) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 3 */
#define MCAN_TXBCF_CF4 (0x1u << 4) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 4 */
#define MCAN_TXBCF_CF5 (0x1u << 5) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 5 */
#define MCAN_TXBCF_CF6 (0x1u << 6) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 6 */
#define MCAN_TXBCF_CF7 (0x1u << 7) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 7 */
#define MCAN_TXBCF_CF8 (0x1u << 8) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 8 */
#define MCAN_TXBCF_CF9 (0x1u << 9) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 9 */
#define MCAN_TXBCF_CF10 (0x1u << 10) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 10 */
#define MCAN_TXBCF_CF11 (0x1u << 11) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 11 */
#define MCAN_TXBCF_CF12 (0x1u << 12) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 12 */
#define MCAN_TXBCF_CF13 (0x1u << 13) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 13 */
#define MCAN_TXBCF_CF14 (0x1u << 14) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 14 */
#define MCAN_TXBCF_CF15 (0x1u << 15) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 15 */
#define MCAN_TXBCF_CF16 (0x1u << 16) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 16 */
#define MCAN_TXBCF_CF17 (0x1u << 17) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 17 */
#define MCAN_TXBCF_CF18 (0x1u << 18) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 18 */
#define MCAN_TXBCF_CF19 (0x1u << 19) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 19 */
#define MCAN_TXBCF_CF20 (0x1u << 20) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 20 */
#define MCAN_TXBCF_CF21 (0x1u << 21) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 21 */
#define MCAN_TXBCF_CF22 (0x1u << 22) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 22 */
#define MCAN_TXBCF_CF23 (0x1u << 23) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 23 */
#define MCAN_TXBCF_CF24 (0x1u << 24) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 24 */
#define MCAN_TXBCF_CF25 (0x1u << 25) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 25 */
#define MCAN_TXBCF_CF26 (0x1u << 26) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 26 */
#define MCAN_TXBCF_CF27 (0x1u << 27) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 27 */
#define MCAN_TXBCF_CF28 (0x1u << 28) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 28 */
#define MCAN_TXBCF_CF29 (0x1u << 29) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 29 */
#define MCAN_TXBCF_CF30 (0x1u << 30) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 30 */
#define MCAN_TXBCF_CF31 (0x1u << 31) /**< \brief (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 31 */
/* -------- MCAN_TXBTIE : (MCAN Offset: 0xE0) Transmit Buffer Transmission Interrupt Enable Register -------- */
#define MCAN_TXBTIE_TIE0 (0x1u << 0) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 0 */
#define MCAN_TXBTIE_TIE1 (0x1u << 1) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 1 */
#define MCAN_TXBTIE_TIE2 (0x1u << 2) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 2 */
#define MCAN_TXBTIE_TIE3 (0x1u << 3) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 3 */
#define MCAN_TXBTIE_TIE4 (0x1u << 4) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 4 */
#define MCAN_TXBTIE_TIE5 (0x1u << 5) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 5 */
#define MCAN_TXBTIE_TIE6 (0x1u << 6) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 6 */
#define MCAN_TXBTIE_TIE7 (0x1u << 7) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 7 */
#define MCAN_TXBTIE_TIE8 (0x1u << 8) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 8 */
#define MCAN_TXBTIE_TIE9 (0x1u << 9) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 9 */
#define MCAN_TXBTIE_TIE10 (0x1u << 10) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 10 */
#define MCAN_TXBTIE_TIE11 (0x1u << 11) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 11 */
#define MCAN_TXBTIE_TIE12 (0x1u << 12) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 12 */
#define MCAN_TXBTIE_TIE13 (0x1u << 13) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 13 */
#define MCAN_TXBTIE_TIE14 (0x1u << 14) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 14 */
#define MCAN_TXBTIE_TIE15 (0x1u << 15) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 15 */
#define MCAN_TXBTIE_TIE16 (0x1u << 16) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 16 */
#define MCAN_TXBTIE_TIE17 (0x1u << 17) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 17 */
#define MCAN_TXBTIE_TIE18 (0x1u << 18) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 18 */
#define MCAN_TXBTIE_TIE19 (0x1u << 19) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 19 */
#define MCAN_TXBTIE_TIE20 (0x1u << 20) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 20 */
#define MCAN_TXBTIE_TIE21 (0x1u << 21) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 21 */
#define MCAN_TXBTIE_TIE22 (0x1u << 22) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 22 */
#define MCAN_TXBTIE_TIE23 (0x1u << 23) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 23 */
#define MCAN_TXBTIE_TIE24 (0x1u << 24) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 24 */
#define MCAN_TXBTIE_TIE25 (0x1u << 25) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 25 */
#define MCAN_TXBTIE_TIE26 (0x1u << 26) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 26 */
#define MCAN_TXBTIE_TIE27 (0x1u << 27) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 27 */
#define MCAN_TXBTIE_TIE28 (0x1u << 28) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 28 */
#define MCAN_TXBTIE_TIE29 (0x1u << 29) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 29 */
#define MCAN_TXBTIE_TIE30 (0x1u << 30) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 30 */
#define MCAN_TXBTIE_TIE31 (0x1u << 31) /**< \brief (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 31 */
/* -------- MCAN_TXBCIE : (MCAN Offset: 0xE4) Transmit Buffer Cancellation Finished Interrupt Enable Register -------- */
#define MCAN_TXBCIE_CFIE0 (0x1u << 0) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 0 */
#define MCAN_TXBCIE_CFIE1 (0x1u << 1) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 1 */
#define MCAN_TXBCIE_CFIE2 (0x1u << 2) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 2 */
#define MCAN_TXBCIE_CFIE3 (0x1u << 3) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 3 */
#define MCAN_TXBCIE_CFIE4 (0x1u << 4) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 4 */
#define MCAN_TXBCIE_CFIE5 (0x1u << 5) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 5 */
#define MCAN_TXBCIE_CFIE6 (0x1u << 6) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 6 */
#define MCAN_TXBCIE_CFIE7 (0x1u << 7) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 7 */
#define MCAN_TXBCIE_CFIE8 (0x1u << 8) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 8 */
#define MCAN_TXBCIE_CFIE9 (0x1u << 9) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 9 */
#define MCAN_TXBCIE_CFIE10 (0x1u << 10) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 10 */
#define MCAN_TXBCIE_CFIE11 (0x1u << 11) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 11 */
#define MCAN_TXBCIE_CFIE12 (0x1u << 12) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 12 */
#define MCAN_TXBCIE_CFIE13 (0x1u << 13) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 13 */
#define MCAN_TXBCIE_CFIE14 (0x1u << 14) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 14 */
#define MCAN_TXBCIE_CFIE15 (0x1u << 15) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 15 */
#define MCAN_TXBCIE_CFIE16 (0x1u << 16) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 16 */
#define MCAN_TXBCIE_CFIE17 (0x1u << 17) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 17 */
#define MCAN_TXBCIE_CFIE18 (0x1u << 18) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 18 */
#define MCAN_TXBCIE_CFIE19 (0x1u << 19) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 19 */
#define MCAN_TXBCIE_CFIE20 (0x1u << 20) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 20 */
#define MCAN_TXBCIE_CFIE21 (0x1u << 21) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 21 */
#define MCAN_TXBCIE_CFIE22 (0x1u << 22) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 22 */
#define MCAN_TXBCIE_CFIE23 (0x1u << 23) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 23 */
#define MCAN_TXBCIE_CFIE24 (0x1u << 24) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 24 */
#define MCAN_TXBCIE_CFIE25 (0x1u << 25) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 25 */
#define MCAN_TXBCIE_CFIE26 (0x1u << 26) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 26 */
#define MCAN_TXBCIE_CFIE27 (0x1u << 27) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 27 */
#define MCAN_TXBCIE_CFIE28 (0x1u << 28) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 28 */
#define MCAN_TXBCIE_CFIE29 (0x1u << 29) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 29 */
#define MCAN_TXBCIE_CFIE30 (0x1u << 30) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 30 */
#define MCAN_TXBCIE_CFIE31 (0x1u << 31) /**< \brief (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 31 */
/* -------- MCAN_TXEFC : (MCAN Offset: 0xF0) Transmit Event FIFO Configuration Register -------- */
#define MCAN_TXEFC_EFSA_Pos 2
#define MCAN_TXEFC_EFSA_Msk (0x3fffu << MCAN_TXEFC_EFSA_Pos) /**< \brief (MCAN_TXEFC) Event FIFO Start Address */
#define MCAN_TXEFC_EFSA(value) ((MCAN_TXEFC_EFSA_Msk & ((value) << MCAN_TXEFC_EFSA_Pos)))
#define MCAN_TXEFC_EFS_Pos 16
#define MCAN_TXEFC_EFS_Msk (0x3fu << MCAN_TXEFC_EFS_Pos) /**< \brief (MCAN_TXEFC) Event FIFO Size */
#define MCAN_TXEFC_EFS(value) ((MCAN_TXEFC_EFS_Msk & ((value) << MCAN_TXEFC_EFS_Pos)))
#define MCAN_TXEFC_EFWM_Pos 24
#define MCAN_TXEFC_EFWM_Msk (0x3fu << MCAN_TXEFC_EFWM_Pos) /**< \brief (MCAN_TXEFC) Event FIFO Watermark */
#define MCAN_TXEFC_EFWM(value) ((MCAN_TXEFC_EFWM_Msk & ((value) << MCAN_TXEFC_EFWM_Pos)))
/* -------- MCAN_TXEFS : (MCAN Offset: 0xF4) Transmit Event FIFO Status Register -------- */
#define MCAN_TXEFS_EFFL_Pos 0
#define MCAN_TXEFS_EFFL_Msk (0x3fu << MCAN_TXEFS_EFFL_Pos) /**< \brief (MCAN_TXEFS) Event FIFO Fill Level */
#define MCAN_TXEFS_EFGI_Pos 8
#define MCAN_TXEFS_EFGI_Msk (0x1fu << MCAN_TXEFS_EFGI_Pos) /**< \brief (MCAN_TXEFS) Event FIFO Get Index */
#define MCAN_TXEFS_EFPI_Pos 16
#define MCAN_TXEFS_EFPI_Msk (0x1fu << MCAN_TXEFS_EFPI_Pos) /**< \brief (MCAN_TXEFS) Event FIFO Put Index */
#define MCAN_TXEFS_EFF (0x1u << 24) /**< \brief (MCAN_TXEFS) Event FIFO Full */
#define MCAN_TXEFS_TEFL (0x1u << 25) /**< \brief (MCAN_TXEFS) Tx Event FIFO Element Lost */
/* -------- MCAN_TXEFA : (MCAN Offset: 0xF8) Transmit Event FIFO Acknowledge Register -------- */
#define MCAN_TXEFA_EFAI_Pos 0
#define MCAN_TXEFA_EFAI_Msk (0x1fu << MCAN_TXEFA_EFAI_Pos) /**< \brief (MCAN_TXEFA) Event FIFO Acknowledge Index */
#define MCAN_TXEFA_EFAI(value) ((MCAN_TXEFA_EFAI_Msk & ((value) << MCAN_TXEFA_EFAI_Pos)))

/*@}*/


#endif /* _SAME70_MCAN_COMPONENT_ */
