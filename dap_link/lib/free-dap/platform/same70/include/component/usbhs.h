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

#ifndef _SAME70_USBHS_COMPONENT_
#define _SAME70_USBHS_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR USB High-Speed Interface */
/* ============================================================================= */
/** \addtogroup SAME70_USBHS USB High-Speed Interface */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief UsbhsDevdma hardware registers */
typedef struct {
  __IO uint32_t USBHS_DEVDMANXTDSC;  /**< \brief (UsbhsDevdma Offset: 0x0) Device DMA Channel Next Descriptor Address Register */
  __IO uint32_t USBHS_DEVDMAADDRESS; /**< \brief (UsbhsDevdma Offset: 0x4) Device DMA Channel Address Register */
  __IO uint32_t USBHS_DEVDMACONTROL; /**< \brief (UsbhsDevdma Offset: 0x8) Device DMA Channel Control Register */
  __IO uint32_t USBHS_DEVDMASTATUS;  /**< \brief (UsbhsDevdma Offset: 0xC) Device DMA Channel Status Register */
} UsbhsDevdma;
/** \brief UsbhsHstdma hardware registers */
typedef struct {
  __IO uint32_t USBHS_HSTDMANXTDSC;  /**< \brief (UsbhsHstdma Offset: 0x0) Host DMA Channel Next Descriptor Address Register */
  __IO uint32_t USBHS_HSTDMAADDRESS; /**< \brief (UsbhsHstdma Offset: 0x4) Host DMA Channel Address Register */
  __IO uint32_t USBHS_HSTDMACONTROL; /**< \brief (UsbhsHstdma Offset: 0x8) Host DMA Channel Control Register */
  __IO uint32_t USBHS_HSTDMASTATUS;  /**< \brief (UsbhsHstdma Offset: 0xC) Host DMA Channel Status Register */
} UsbhsHstdma;
/** \brief Usbhs hardware registers */
#define USBHSDEVDMA_NUMBER 7
#define USBHSHSTDMA_NUMBER 7
typedef struct {
  __IO uint32_t    USBHS_DEVCTRL;                    /**< \brief (Usbhs Offset: 0x0000) Device General Control Register */
  __I  uint32_t    USBHS_DEVISR;                     /**< \brief (Usbhs Offset: 0x0004) Device Global Interrupt Status Register */
  __O  uint32_t    USBHS_DEVICR;                     /**< \brief (Usbhs Offset: 0x0008) Device Global Interrupt Clear Register */
  __O  uint32_t    USBHS_DEVIFR;                     /**< \brief (Usbhs Offset: 0x000C) Device Global Interrupt Set Register */
  __I  uint32_t    USBHS_DEVIMR;                     /**< \brief (Usbhs Offset: 0x0010) Device Global Interrupt Mask Register */
  __O  uint32_t    USBHS_DEVIDR;                     /**< \brief (Usbhs Offset: 0x0014) Device Global Interrupt Disable Register */
  __O  uint32_t    USBHS_DEVIER;                     /**< \brief (Usbhs Offset: 0x0018) Device Global Interrupt Enable Register */
  __IO uint32_t    USBHS_DEVEPT;                     /**< \brief (Usbhs Offset: 0x001C) Device Endpoint Register */
  __I  uint32_t    USBHS_DEVFNUM;                    /**< \brief (Usbhs Offset: 0x0020) Device Frame Number Register */
  __I  uint32_t    Reserved1[55];
  __IO uint32_t    USBHS_DEVEPTCFG[10];              /**< \brief (Usbhs Offset: 0x100) Device Endpoint Configuration Register (n = 0) */
  __I  uint32_t    Reserved2[2];
  __I  uint32_t    USBHS_DEVEPTISR[10];              /**< \brief (Usbhs Offset: 0x130) Device Endpoint Status Register (n = 0) */
  __I  uint32_t    Reserved3[2];
  __O  uint32_t    USBHS_DEVEPTICR[10];              /**< \brief (Usbhs Offset: 0x160) Device Endpoint Clear Register (n = 0) */
  __I  uint32_t    Reserved4[2];
  __O  uint32_t    USBHS_DEVEPTIFR[10];              /**< \brief (Usbhs Offset: 0x190) Device Endpoint Set Register (n = 0) */
  __I  uint32_t    Reserved5[2];
  __I  uint32_t    USBHS_DEVEPTIMR[10];              /**< \brief (Usbhs Offset: 0x1C0) Device Endpoint Mask Register (n = 0) */
  __I  uint32_t    Reserved6[2];
  __O  uint32_t    USBHS_DEVEPTIER[10];              /**< \brief (Usbhs Offset: 0x1F0) Device Endpoint Enable Register (n = 0) */
  __I  uint32_t    Reserved7[2];
  __O  uint32_t    USBHS_DEVEPTIDR[10];              /**< \brief (Usbhs Offset: 0x220) Device Endpoint Disable Register (n = 0) */
  __I  uint32_t    Reserved8[50];
       UsbhsDevdma USBHS_DEVDMA[USBHSDEVDMA_NUMBER]; /**< \brief (Usbhs Offset: 0x310) n = 1 .. 7 */
  __I  uint32_t    Reserved9[32];
  __IO uint32_t    USBHS_HSTCTRL;                    /**< \brief (Usbhs Offset: 0x0400) Host General Control Register */
  __I  uint32_t    USBHS_HSTISR;                     /**< \brief (Usbhs Offset: 0x0404) Host Global Interrupt Status Register */
  __O  uint32_t    USBHS_HSTICR;                     /**< \brief (Usbhs Offset: 0x0408) Host Global Interrupt Clear Register */
  __O  uint32_t    USBHS_HSTIFR;                     /**< \brief (Usbhs Offset: 0x040C) Host Global Interrupt Set Register */
  __I  uint32_t    USBHS_HSTIMR;                     /**< \brief (Usbhs Offset: 0x0410) Host Global Interrupt Mask Register */
  __O  uint32_t    USBHS_HSTIDR;                     /**< \brief (Usbhs Offset: 0x0414) Host Global Interrupt Disable Register */
  __O  uint32_t    USBHS_HSTIER;                     /**< \brief (Usbhs Offset: 0x0418) Host Global Interrupt Enable Register */
  __IO uint32_t    USBHS_HSTPIP;                     /**< \brief (Usbhs Offset: 0x0041C) Host Pipe Register */
  __IO uint32_t    USBHS_HSTFNUM;                    /**< \brief (Usbhs Offset: 0x0420) Host Frame Number Register */
  __IO uint32_t    USBHS_HSTADDR1;                   /**< \brief (Usbhs Offset: 0x0424) Host Address 1 Register */
  __IO uint32_t    USBHS_HSTADDR2;                   /**< \brief (Usbhs Offset: 0x0428) Host Address 2 Register */
  __IO uint32_t    USBHS_HSTADDR3;                   /**< \brief (Usbhs Offset: 0x042C) Host Address 3 Register */
  __I  uint32_t    Reserved10[52];
  __IO uint32_t    USBHS_HSTPIPCFG[10];              /**< \brief (Usbhs Offset: 0x500) Host Pipe Configuration Register (n = 0) */
  __I  uint32_t    Reserved11[2];
  __I  uint32_t    USBHS_HSTPIPISR[10];              /**< \brief (Usbhs Offset: 0x530) Host Pipe Status Register (n = 0) */
  __I  uint32_t    Reserved12[2];
  __O  uint32_t    USBHS_HSTPIPICR[10];              /**< \brief (Usbhs Offset: 0x560) Host Pipe Clear Register (n = 0) */
  __I  uint32_t    Reserved13[2];
  __O  uint32_t    USBHS_HSTPIPIFR[10];              /**< \brief (Usbhs Offset: 0x590) Host Pipe Set Register (n = 0) */
  __I  uint32_t    Reserved14[2];
  __I  uint32_t    USBHS_HSTPIPIMR[10];              /**< \brief (Usbhs Offset: 0x5C0) Host Pipe Mask Register (n = 0) */
  __I  uint32_t    Reserved15[2];
  __O  uint32_t    USBHS_HSTPIPIER[10];              /**< \brief (Usbhs Offset: 0x5F0) Host Pipe Enable Register (n = 0) */
  __I  uint32_t    Reserved16[2];
  __O  uint32_t    USBHS_HSTPIPIDR[10];              /**< \brief (Usbhs Offset: 0x620) Host Pipe Disable Register (n = 0) */
  __I  uint32_t    Reserved17[2];
  __IO uint32_t    USBHS_HSTPIPINRQ[10];             /**< \brief (Usbhs Offset: 0x650) Host Pipe IN Request Register (n = 0) */
  __I  uint32_t    Reserved18[2];
  __IO uint32_t    USBHS_HSTPIPERR[10];              /**< \brief (Usbhs Offset: 0x680) Host Pipe Error Register (n = 0) */
  __I  uint32_t    Reserved19[26];
       UsbhsHstdma USBHS_HSTDMA[USBHSHSTDMA_NUMBER]; /**< \brief (Usbhs Offset: 0x710) n = 1 .. 7 */
  __I  uint32_t    Reserved20[32];
  __IO uint32_t    USBHS_CTRL;                       /**< \brief (Usbhs Offset: 0x0800) General Control Register */
  __I  uint32_t    USBHS_SR;                         /**< \brief (Usbhs Offset: 0x0804) General Status Register */
  __O  uint32_t    USBHS_SCR;                        /**< \brief (Usbhs Offset: 0x0808) General Status Clear Register */
  __O  uint32_t    USBHS_SFR;                        /**< \brief (Usbhs Offset: 0x080C) General Status Set Register */
  __IO uint32_t    USBHS_TSTA1;                      /**< \brief (Usbhs Offset: 0x0810) General Test A1 Register */
  __IO uint32_t    USBHS_TSTA2;                      /**< \brief (Usbhs Offset: 0x0814) General Test A2 Register */
  __I  uint32_t    USBHS_VERSION;                    /**< \brief (Usbhs Offset: 0x0818) General Version Register */
  __I  uint32_t    Reserved21[4];
  __I  uint32_t    USBHS_FSM;                        /**< \brief (Usbhs Offset: 0x082C) General Finite State Machine Register */
} Usbhs;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- USBHS_DEVCTRL : (USBHS Offset: 0x0000) Device General Control Register -------- */
#define USBHS_DEVCTRL_UADD_Pos 0
#define USBHS_DEVCTRL_UADD_Msk (0x7fu << USBHS_DEVCTRL_UADD_Pos) /**< \brief (USBHS_DEVCTRL) USB Address */
#define USBHS_DEVCTRL_UADD(value) ((USBHS_DEVCTRL_UADD_Msk & ((value) << USBHS_DEVCTRL_UADD_Pos)))
#define USBHS_DEVCTRL_ADDEN (0x1u << 7) /**< \brief (USBHS_DEVCTRL) Address Enable */
#define USBHS_DEVCTRL_DETACH (0x1u << 8) /**< \brief (USBHS_DEVCTRL) Detach */
#define USBHS_DEVCTRL_RMWKUP (0x1u << 9) /**< \brief (USBHS_DEVCTRL) Remote Wake-Up */
#define USBHS_DEVCTRL_SPDCONF_Pos 10
#define USBHS_DEVCTRL_SPDCONF_Msk (0x3u << USBHS_DEVCTRL_SPDCONF_Pos) /**< \brief (USBHS_DEVCTRL) Mode Configuration */
#define USBHS_DEVCTRL_SPDCONF(value) ((USBHS_DEVCTRL_SPDCONF_Msk & ((value) << USBHS_DEVCTRL_SPDCONF_Pos)))
#define   USBHS_DEVCTRL_SPDCONF_NORMAL (0x0u << 10) /**< \brief (USBHS_DEVCTRL) The peripheral starts in Full-speed mode and performs a high-speed reset to switch to High-speed mode if the host is high-speed-capable. */
#define   USBHS_DEVCTRL_SPDCONF_LOW_POWER (0x1u << 10) /**< \brief (USBHS_DEVCTRL) For a better consumption, if high speed is not needed. */
#define   USBHS_DEVCTRL_SPDCONF_HIGH_SPEED (0x2u << 10) /**< \brief (USBHS_DEVCTRL) Forced high speed. */
#define   USBHS_DEVCTRL_SPDCONF_FORCED_FS (0x3u << 10) /**< \brief (USBHS_DEVCTRL) The peripheral remains in Full-speed mode whatever the host speed capability. */
#define USBHS_DEVCTRL_LS (0x1u << 12) /**< \brief (USBHS_DEVCTRL) Low-Speed Mode Force */
#define USBHS_DEVCTRL_TSTJ (0x1u << 13) /**< \brief (USBHS_DEVCTRL) Test mode J */
#define USBHS_DEVCTRL_TSTK (0x1u << 14) /**< \brief (USBHS_DEVCTRL) Test mode K */
#define USBHS_DEVCTRL_TSTPCKT (0x1u << 15) /**< \brief (USBHS_DEVCTRL) Test packet mode */
#define USBHS_DEVCTRL_OPMODE2 (0x1u << 16) /**< \brief (USBHS_DEVCTRL) Specific Operational mode */
/* -------- USBHS_DEVISR : (USBHS Offset: 0x0004) Device Global Interrupt Status Register -------- */
#define USBHS_DEVISR_SUSP (0x1u << 0) /**< \brief (USBHS_DEVISR) Suspend Interrupt */
#define USBHS_DEVISR_MSOF (0x1u << 1) /**< \brief (USBHS_DEVISR) Micro Start of Frame Interrupt */
#define USBHS_DEVISR_SOF (0x1u << 2) /**< \brief (USBHS_DEVISR) Start of Frame Interrupt */
#define USBHS_DEVISR_EORST (0x1u << 3) /**< \brief (USBHS_DEVISR) End of Reset Interrupt */
#define USBHS_DEVISR_WAKEUP (0x1u << 4) /**< \brief (USBHS_DEVISR) Wake-Up Interrupt */
#define USBHS_DEVISR_EORSM (0x1u << 5) /**< \brief (USBHS_DEVISR) End of Resume Interrupt */
#define USBHS_DEVISR_UPRSM (0x1u << 6) /**< \brief (USBHS_DEVISR) Upstream Resume Interrupt */
#define USBHS_DEVISR_PEP_0 (0x1u << 12) /**< \brief (USBHS_DEVISR) Endpoint 0 Interrupt */
#define USBHS_DEVISR_PEP_1 (0x1u << 13) /**< \brief (USBHS_DEVISR) Endpoint 1 Interrupt */
#define USBHS_DEVISR_PEP_2 (0x1u << 14) /**< \brief (USBHS_DEVISR) Endpoint 2 Interrupt */
#define USBHS_DEVISR_PEP_3 (0x1u << 15) /**< \brief (USBHS_DEVISR) Endpoint 3 Interrupt */
#define USBHS_DEVISR_PEP_4 (0x1u << 16) /**< \brief (USBHS_DEVISR) Endpoint 4 Interrupt */
#define USBHS_DEVISR_PEP_5 (0x1u << 17) /**< \brief (USBHS_DEVISR) Endpoint 5 Interrupt */
#define USBHS_DEVISR_PEP_6 (0x1u << 18) /**< \brief (USBHS_DEVISR) Endpoint 6 Interrupt */
#define USBHS_DEVISR_PEP_7 (0x1u << 19) /**< \brief (USBHS_DEVISR) Endpoint 7 Interrupt */
#define USBHS_DEVISR_PEP_8 (0x1u << 20) /**< \brief (USBHS_DEVISR) Endpoint 8 Interrupt */
#define USBHS_DEVISR_PEP_9 (0x1u << 21) /**< \brief (USBHS_DEVISR) Endpoint 9 Interrupt */
#define USBHS_DEVISR_DMA_1 (0x1u << 25) /**< \brief (USBHS_DEVISR) DMA Channel 1 Interrupt */
#define USBHS_DEVISR_DMA_2 (0x1u << 26) /**< \brief (USBHS_DEVISR) DMA Channel 2 Interrupt */
#define USBHS_DEVISR_DMA_3 (0x1u << 27) /**< \brief (USBHS_DEVISR) DMA Channel 3 Interrupt */
#define USBHS_DEVISR_DMA_4 (0x1u << 28) /**< \brief (USBHS_DEVISR) DMA Channel 4 Interrupt */
#define USBHS_DEVISR_DMA_5 (0x1u << 29) /**< \brief (USBHS_DEVISR) DMA Channel 5 Interrupt */
#define USBHS_DEVISR_DMA_6 (0x1u << 30) /**< \brief (USBHS_DEVISR) DMA Channel 6 Interrupt */
#define USBHS_DEVISR_DMA_7 (0x1u << 31) /**< \brief (USBHS_DEVISR) DMA Channel 7 Interrupt */
/* -------- USBHS_DEVICR : (USBHS Offset: 0x0008) Device Global Interrupt Clear Register -------- */
#define USBHS_DEVICR_SUSPC (0x1u << 0) /**< \brief (USBHS_DEVICR) Suspend Interrupt Clear */
#define USBHS_DEVICR_MSOFC (0x1u << 1) /**< \brief (USBHS_DEVICR) Micro Start of Frame Interrupt Clear */
#define USBHS_DEVICR_SOFC (0x1u << 2) /**< \brief (USBHS_DEVICR) Start of Frame Interrupt Clear */
#define USBHS_DEVICR_EORSTC (0x1u << 3) /**< \brief (USBHS_DEVICR) End of Reset Interrupt Clear */
#define USBHS_DEVICR_WAKEUPC (0x1u << 4) /**< \brief (USBHS_DEVICR) Wake-Up Interrupt Clear */
#define USBHS_DEVICR_EORSMC (0x1u << 5) /**< \brief (USBHS_DEVICR) End of Resume Interrupt Clear */
#define USBHS_DEVICR_UPRSMC (0x1u << 6) /**< \brief (USBHS_DEVICR) Upstream Resume Interrupt Clear */
/* -------- USBHS_DEVIFR : (USBHS Offset: 0x000C) Device Global Interrupt Set Register -------- */
#define USBHS_DEVIFR_SUSPS (0x1u << 0) /**< \brief (USBHS_DEVIFR) Suspend Interrupt Set */
#define USBHS_DEVIFR_MSOFS (0x1u << 1) /**< \brief (USBHS_DEVIFR) Micro Start of Frame Interrupt Set */
#define USBHS_DEVIFR_SOFS (0x1u << 2) /**< \brief (USBHS_DEVIFR) Start of Frame Interrupt Set */
#define USBHS_DEVIFR_EORSTS (0x1u << 3) /**< \brief (USBHS_DEVIFR) End of Reset Interrupt Set */
#define USBHS_DEVIFR_WAKEUPS (0x1u << 4) /**< \brief (USBHS_DEVIFR) Wake-Up Interrupt Set */
#define USBHS_DEVIFR_EORSMS (0x1u << 5) /**< \brief (USBHS_DEVIFR) End of Resume Interrupt Set */
#define USBHS_DEVIFR_UPRSMS (0x1u << 6) /**< \brief (USBHS_DEVIFR) Upstream Resume Interrupt Set */
#define USBHS_DEVIFR_DMA_1 (0x1u << 25) /**< \brief (USBHS_DEVIFR) DMA Channel 1 Interrupt Set */
#define USBHS_DEVIFR_DMA_2 (0x1u << 26) /**< \brief (USBHS_DEVIFR) DMA Channel 2 Interrupt Set */
#define USBHS_DEVIFR_DMA_3 (0x1u << 27) /**< \brief (USBHS_DEVIFR) DMA Channel 3 Interrupt Set */
#define USBHS_DEVIFR_DMA_4 (0x1u << 28) /**< \brief (USBHS_DEVIFR) DMA Channel 4 Interrupt Set */
#define USBHS_DEVIFR_DMA_5 (0x1u << 29) /**< \brief (USBHS_DEVIFR) DMA Channel 5 Interrupt Set */
#define USBHS_DEVIFR_DMA_6 (0x1u << 30) /**< \brief (USBHS_DEVIFR) DMA Channel 6 Interrupt Set */
#define USBHS_DEVIFR_DMA_7 (0x1u << 31) /**< \brief (USBHS_DEVIFR) DMA Channel 7 Interrupt Set */
/* -------- USBHS_DEVIMR : (USBHS Offset: 0x0010) Device Global Interrupt Mask Register -------- */
#define USBHS_DEVIMR_SUSPE (0x1u << 0) /**< \brief (USBHS_DEVIMR) Suspend Interrupt Mask */
#define USBHS_DEVIMR_MSOFE (0x1u << 1) /**< \brief (USBHS_DEVIMR) Micro Start of Frame Interrupt Mask */
#define USBHS_DEVIMR_SOFE (0x1u << 2) /**< \brief (USBHS_DEVIMR) Start of Frame Interrupt Mask */
#define USBHS_DEVIMR_EORSTE (0x1u << 3) /**< \brief (USBHS_DEVIMR) End of Reset Interrupt Mask */
#define USBHS_DEVIMR_WAKEUPE (0x1u << 4) /**< \brief (USBHS_DEVIMR) Wake-Up Interrupt Mask */
#define USBHS_DEVIMR_EORSME (0x1u << 5) /**< \brief (USBHS_DEVIMR) End of Resume Interrupt Mask */
#define USBHS_DEVIMR_UPRSME (0x1u << 6) /**< \brief (USBHS_DEVIMR) Upstream Resume Interrupt Mask */
#define USBHS_DEVIMR_PEP_0 (0x1u << 12) /**< \brief (USBHS_DEVIMR) Endpoint 0 Interrupt Mask */
#define USBHS_DEVIMR_PEP_1 (0x1u << 13) /**< \brief (USBHS_DEVIMR) Endpoint 1 Interrupt Mask */
#define USBHS_DEVIMR_PEP_2 (0x1u << 14) /**< \brief (USBHS_DEVIMR) Endpoint 2 Interrupt Mask */
#define USBHS_DEVIMR_PEP_3 (0x1u << 15) /**< \brief (USBHS_DEVIMR) Endpoint 3 Interrupt Mask */
#define USBHS_DEVIMR_PEP_4 (0x1u << 16) /**< \brief (USBHS_DEVIMR) Endpoint 4 Interrupt Mask */
#define USBHS_DEVIMR_PEP_5 (0x1u << 17) /**< \brief (USBHS_DEVIMR) Endpoint 5 Interrupt Mask */
#define USBHS_DEVIMR_PEP_6 (0x1u << 18) /**< \brief (USBHS_DEVIMR) Endpoint 6 Interrupt Mask */
#define USBHS_DEVIMR_PEP_7 (0x1u << 19) /**< \brief (USBHS_DEVIMR) Endpoint 7 Interrupt Mask */
#define USBHS_DEVIMR_PEP_8 (0x1u << 20) /**< \brief (USBHS_DEVIMR) Endpoint 8 Interrupt Mask */
#define USBHS_DEVIMR_PEP_9 (0x1u << 21) /**< \brief (USBHS_DEVIMR) Endpoint 9 Interrupt Mask */
#define USBHS_DEVIMR_DMA_1 (0x1u << 25) /**< \brief (USBHS_DEVIMR) DMA Channel 1 Interrupt Mask */
#define USBHS_DEVIMR_DMA_2 (0x1u << 26) /**< \brief (USBHS_DEVIMR) DMA Channel 2 Interrupt Mask */
#define USBHS_DEVIMR_DMA_3 (0x1u << 27) /**< \brief (USBHS_DEVIMR) DMA Channel 3 Interrupt Mask */
#define USBHS_DEVIMR_DMA_4 (0x1u << 28) /**< \brief (USBHS_DEVIMR) DMA Channel 4 Interrupt Mask */
#define USBHS_DEVIMR_DMA_5 (0x1u << 29) /**< \brief (USBHS_DEVIMR) DMA Channel 5 Interrupt Mask */
#define USBHS_DEVIMR_DMA_6 (0x1u << 30) /**< \brief (USBHS_DEVIMR) DMA Channel 6 Interrupt Mask */
#define USBHS_DEVIMR_DMA_7 (0x1u << 31) /**< \brief (USBHS_DEVIMR) DMA Channel 7 Interrupt Mask */
/* -------- USBHS_DEVIDR : (USBHS Offset: 0x0014) Device Global Interrupt Disable Register -------- */
#define USBHS_DEVIDR_SUSPEC (0x1u << 0) /**< \brief (USBHS_DEVIDR) Suspend Interrupt Disable */
#define USBHS_DEVIDR_MSOFEC (0x1u << 1) /**< \brief (USBHS_DEVIDR) Micro Start of Frame Interrupt Disable */
#define USBHS_DEVIDR_SOFEC (0x1u << 2) /**< \brief (USBHS_DEVIDR) Start of Frame Interrupt Disable */
#define USBHS_DEVIDR_EORSTEC (0x1u << 3) /**< \brief (USBHS_DEVIDR) End of Reset Interrupt Disable */
#define USBHS_DEVIDR_WAKEUPEC (0x1u << 4) /**< \brief (USBHS_DEVIDR) Wake-Up Interrupt Disable */
#define USBHS_DEVIDR_EORSMEC (0x1u << 5) /**< \brief (USBHS_DEVIDR) End of Resume Interrupt Disable */
#define USBHS_DEVIDR_UPRSMEC (0x1u << 6) /**< \brief (USBHS_DEVIDR) Upstream Resume Interrupt Disable */
#define USBHS_DEVIDR_PEP_0 (0x1u << 12) /**< \brief (USBHS_DEVIDR) Endpoint 0 Interrupt Disable */
#define USBHS_DEVIDR_PEP_1 (0x1u << 13) /**< \brief (USBHS_DEVIDR) Endpoint 1 Interrupt Disable */
#define USBHS_DEVIDR_PEP_2 (0x1u << 14) /**< \brief (USBHS_DEVIDR) Endpoint 2 Interrupt Disable */
#define USBHS_DEVIDR_PEP_3 (0x1u << 15) /**< \brief (USBHS_DEVIDR) Endpoint 3 Interrupt Disable */
#define USBHS_DEVIDR_PEP_4 (0x1u << 16) /**< \brief (USBHS_DEVIDR) Endpoint 4 Interrupt Disable */
#define USBHS_DEVIDR_PEP_5 (0x1u << 17) /**< \brief (USBHS_DEVIDR) Endpoint 5 Interrupt Disable */
#define USBHS_DEVIDR_PEP_6 (0x1u << 18) /**< \brief (USBHS_DEVIDR) Endpoint 6 Interrupt Disable */
#define USBHS_DEVIDR_PEP_7 (0x1u << 19) /**< \brief (USBHS_DEVIDR) Endpoint 7 Interrupt Disable */
#define USBHS_DEVIDR_PEP_8 (0x1u << 20) /**< \brief (USBHS_DEVIDR) Endpoint 8 Interrupt Disable */
#define USBHS_DEVIDR_PEP_9 (0x1u << 21) /**< \brief (USBHS_DEVIDR) Endpoint 9 Interrupt Disable */
#define USBHS_DEVIDR_DMA_1 (0x1u << 25) /**< \brief (USBHS_DEVIDR) DMA Channel 1 Interrupt Disable */
#define USBHS_DEVIDR_DMA_2 (0x1u << 26) /**< \brief (USBHS_DEVIDR) DMA Channel 2 Interrupt Disable */
#define USBHS_DEVIDR_DMA_3 (0x1u << 27) /**< \brief (USBHS_DEVIDR) DMA Channel 3 Interrupt Disable */
#define USBHS_DEVIDR_DMA_4 (0x1u << 28) /**< \brief (USBHS_DEVIDR) DMA Channel 4 Interrupt Disable */
#define USBHS_DEVIDR_DMA_5 (0x1u << 29) /**< \brief (USBHS_DEVIDR) DMA Channel 5 Interrupt Disable */
#define USBHS_DEVIDR_DMA_6 (0x1u << 30) /**< \brief (USBHS_DEVIDR) DMA Channel 6 Interrupt Disable */
#define USBHS_DEVIDR_DMA_7 (0x1u << 31) /**< \brief (USBHS_DEVIDR) DMA Channel 7 Interrupt Disable */
/* -------- USBHS_DEVIER : (USBHS Offset: 0x0018) Device Global Interrupt Enable Register -------- */
#define USBHS_DEVIER_SUSPES (0x1u << 0) /**< \brief (USBHS_DEVIER) Suspend Interrupt Enable */
#define USBHS_DEVIER_MSOFES (0x1u << 1) /**< \brief (USBHS_DEVIER) Micro Start of Frame Interrupt Enable */
#define USBHS_DEVIER_SOFES (0x1u << 2) /**< \brief (USBHS_DEVIER) Start of Frame Interrupt Enable */
#define USBHS_DEVIER_EORSTES (0x1u << 3) /**< \brief (USBHS_DEVIER) End of Reset Interrupt Enable */
#define USBHS_DEVIER_WAKEUPES (0x1u << 4) /**< \brief (USBHS_DEVIER) Wake-Up Interrupt Enable */
#define USBHS_DEVIER_EORSMES (0x1u << 5) /**< \brief (USBHS_DEVIER) End of Resume Interrupt Enable */
#define USBHS_DEVIER_UPRSMES (0x1u << 6) /**< \brief (USBHS_DEVIER) Upstream Resume Interrupt Enable */
#define USBHS_DEVIER_PEP_0 (0x1u << 12) /**< \brief (USBHS_DEVIER) Endpoint 0 Interrupt Enable */
#define USBHS_DEVIER_PEP_1 (0x1u << 13) /**< \brief (USBHS_DEVIER) Endpoint 1 Interrupt Enable */
#define USBHS_DEVIER_PEP_2 (0x1u << 14) /**< \brief (USBHS_DEVIER) Endpoint 2 Interrupt Enable */
#define USBHS_DEVIER_PEP_3 (0x1u << 15) /**< \brief (USBHS_DEVIER) Endpoint 3 Interrupt Enable */
#define USBHS_DEVIER_PEP_4 (0x1u << 16) /**< \brief (USBHS_DEVIER) Endpoint 4 Interrupt Enable */
#define USBHS_DEVIER_PEP_5 (0x1u << 17) /**< \brief (USBHS_DEVIER) Endpoint 5 Interrupt Enable */
#define USBHS_DEVIER_PEP_6 (0x1u << 18) /**< \brief (USBHS_DEVIER) Endpoint 6 Interrupt Enable */
#define USBHS_DEVIER_PEP_7 (0x1u << 19) /**< \brief (USBHS_DEVIER) Endpoint 7 Interrupt Enable */
#define USBHS_DEVIER_PEP_8 (0x1u << 20) /**< \brief (USBHS_DEVIER) Endpoint 8 Interrupt Enable */
#define USBHS_DEVIER_PEP_9 (0x1u << 21) /**< \brief (USBHS_DEVIER) Endpoint 9 Interrupt Enable */
#define USBHS_DEVIER_DMA_1 (0x1u << 25) /**< \brief (USBHS_DEVIER) DMA Channel 1 Interrupt Enable */
#define USBHS_DEVIER_DMA_2 (0x1u << 26) /**< \brief (USBHS_DEVIER) DMA Channel 2 Interrupt Enable */
#define USBHS_DEVIER_DMA_3 (0x1u << 27) /**< \brief (USBHS_DEVIER) DMA Channel 3 Interrupt Enable */
#define USBHS_DEVIER_DMA_4 (0x1u << 28) /**< \brief (USBHS_DEVIER) DMA Channel 4 Interrupt Enable */
#define USBHS_DEVIER_DMA_5 (0x1u << 29) /**< \brief (USBHS_DEVIER) DMA Channel 5 Interrupt Enable */
#define USBHS_DEVIER_DMA_6 (0x1u << 30) /**< \brief (USBHS_DEVIER) DMA Channel 6 Interrupt Enable */
#define USBHS_DEVIER_DMA_7 (0x1u << 31) /**< \brief (USBHS_DEVIER) DMA Channel 7 Interrupt Enable */
/* -------- USBHS_DEVEPT : (USBHS Offset: 0x001C) Device Endpoint Register -------- */
#define USBHS_DEVEPT_EPEN0 (0x1u << 0) /**< \brief (USBHS_DEVEPT) Endpoint 0 Enable */
#define USBHS_DEVEPT_EPEN1 (0x1u << 1) /**< \brief (USBHS_DEVEPT) Endpoint 1 Enable */
#define USBHS_DEVEPT_EPEN2 (0x1u << 2) /**< \brief (USBHS_DEVEPT) Endpoint 2 Enable */
#define USBHS_DEVEPT_EPEN3 (0x1u << 3) /**< \brief (USBHS_DEVEPT) Endpoint 3 Enable */
#define USBHS_DEVEPT_EPEN4 (0x1u << 4) /**< \brief (USBHS_DEVEPT) Endpoint 4 Enable */
#define USBHS_DEVEPT_EPEN5 (0x1u << 5) /**< \brief (USBHS_DEVEPT) Endpoint 5 Enable */
#define USBHS_DEVEPT_EPEN6 (0x1u << 6) /**< \brief (USBHS_DEVEPT) Endpoint 6 Enable */
#define USBHS_DEVEPT_EPEN7 (0x1u << 7) /**< \brief (USBHS_DEVEPT) Endpoint 7 Enable */
#define USBHS_DEVEPT_EPEN8 (0x1u << 8) /**< \brief (USBHS_DEVEPT) Endpoint 8 Enable */
#define USBHS_DEVEPT_EPEN9 (0x1u << 9) /**< \brief (USBHS_DEVEPT) Endpoint 9 Enable */
#define USBHS_DEVEPT_EPRST0 (0x1u << 16) /**< \brief (USBHS_DEVEPT) Endpoint 0 Reset */
#define USBHS_DEVEPT_EPRST1 (0x1u << 17) /**< \brief (USBHS_DEVEPT) Endpoint 1 Reset */
#define USBHS_DEVEPT_EPRST2 (0x1u << 18) /**< \brief (USBHS_DEVEPT) Endpoint 2 Reset */
#define USBHS_DEVEPT_EPRST3 (0x1u << 19) /**< \brief (USBHS_DEVEPT) Endpoint 3 Reset */
#define USBHS_DEVEPT_EPRST4 (0x1u << 20) /**< \brief (USBHS_DEVEPT) Endpoint 4 Reset */
#define USBHS_DEVEPT_EPRST5 (0x1u << 21) /**< \brief (USBHS_DEVEPT) Endpoint 5 Reset */
#define USBHS_DEVEPT_EPRST6 (0x1u << 22) /**< \brief (USBHS_DEVEPT) Endpoint 6 Reset */
#define USBHS_DEVEPT_EPRST7 (0x1u << 23) /**< \brief (USBHS_DEVEPT) Endpoint 7 Reset */
#define USBHS_DEVEPT_EPRST8 (0x1u << 24) /**< \brief (USBHS_DEVEPT) Endpoint 8 Reset */
#define USBHS_DEVEPT_EPRST9 (0x1u << 25) /**< \brief (USBHS_DEVEPT) Endpoint 9 Reset */
/* -------- USBHS_DEVFNUM : (USBHS Offset: 0x0020) Device Frame Number Register -------- */
#define USBHS_DEVFNUM_MFNUM_Pos 0
#define USBHS_DEVFNUM_MFNUM_Msk (0x7u << USBHS_DEVFNUM_MFNUM_Pos) /**< \brief (USBHS_DEVFNUM) Micro Frame Number */
#define USBHS_DEVFNUM_FNUM_Pos 3
#define USBHS_DEVFNUM_FNUM_Msk (0x7ffu << USBHS_DEVFNUM_FNUM_Pos) /**< \brief (USBHS_DEVFNUM) Frame Number */
#define USBHS_DEVFNUM_FNCERR (0x1u << 15) /**< \brief (USBHS_DEVFNUM) Frame Number CRC Error */
/* -------- USBHS_DEVEPTCFG[10] : (USBHS Offset: 0x100) Device Endpoint Configuration Register (n = 0) -------- */
#define USBHS_DEVEPTCFG_ALLOC (0x1u << 1) /**< \brief (USBHS_DEVEPTCFG[10]) Endpoint Memory Allocate */
#define USBHS_DEVEPTCFG_EPBK_Pos 2
#define USBHS_DEVEPTCFG_EPBK_Msk (0x3u << USBHS_DEVEPTCFG_EPBK_Pos) /**< \brief (USBHS_DEVEPTCFG[10]) Endpoint Banks */
#define USBHS_DEVEPTCFG_EPBK(value) ((USBHS_DEVEPTCFG_EPBK_Msk & ((value) << USBHS_DEVEPTCFG_EPBK_Pos)))
#define   USBHS_DEVEPTCFG_EPBK_1_BANK (0x0u << 2) /**< \brief (USBHS_DEVEPTCFG[10]) Single-bank endpoint */
#define   USBHS_DEVEPTCFG_EPBK_2_BANK (0x1u << 2) /**< \brief (USBHS_DEVEPTCFG[10]) Double-bank endpoint */
#define   USBHS_DEVEPTCFG_EPBK_3_BANK (0x2u << 2) /**< \brief (USBHS_DEVEPTCFG[10]) Triple-bank endpoint */
#define USBHS_DEVEPTCFG_EPSIZE_Pos 4
#define USBHS_DEVEPTCFG_EPSIZE_Msk (0x7u << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< \brief (USBHS_DEVEPTCFG[10]) Endpoint Size */
#define USBHS_DEVEPTCFG_EPSIZE(value) ((USBHS_DEVEPTCFG_EPSIZE_Msk & ((value) << USBHS_DEVEPTCFG_EPSIZE_Pos)))
#define   USBHS_DEVEPTCFG_EPSIZE_8_BYTE (0x0u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 8 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_16_BYTE (0x1u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 16 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_32_BYTE (0x2u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 32 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_64_BYTE (0x3u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 64 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_128_BYTE (0x4u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 128 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_256_BYTE (0x5u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 256 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_512_BYTE (0x6u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 512 bytes */
#define   USBHS_DEVEPTCFG_EPSIZE_1024_BYTE (0x7u << 4) /**< \brief (USBHS_DEVEPTCFG[10]) 1024 bytes */
#define USBHS_DEVEPTCFG_EPDIR (0x1u << 8) /**< \brief (USBHS_DEVEPTCFG[10]) Endpoint Direction */
#define   USBHS_DEVEPTCFG_EPDIR_OUT (0x0u << 8) /**< \brief (USBHS_DEVEPTCFG[10]) The endpoint direction is OUT. */
#define   USBHS_DEVEPTCFG_EPDIR_IN (0x1u << 8) /**< \brief (USBHS_DEVEPTCFG[10]) The endpoint direction is IN (nor for control endpoints). */
#define USBHS_DEVEPTCFG_AUTOSW (0x1u << 9) /**< \brief (USBHS_DEVEPTCFG[10]) Automatic Switch */
#define USBHS_DEVEPTCFG_EPTYPE_Pos 11
#define USBHS_DEVEPTCFG_EPTYPE_Msk (0x3u << USBHS_DEVEPTCFG_EPTYPE_Pos) /**< \brief (USBHS_DEVEPTCFG[10]) Endpoint Type */
#define USBHS_DEVEPTCFG_EPTYPE(value) ((USBHS_DEVEPTCFG_EPTYPE_Msk & ((value) << USBHS_DEVEPTCFG_EPTYPE_Pos)))
#define   USBHS_DEVEPTCFG_EPTYPE_CTRL (0x0u << 11) /**< \brief (USBHS_DEVEPTCFG[10]) Control */
#define   USBHS_DEVEPTCFG_EPTYPE_ISO (0x1u << 11) /**< \brief (USBHS_DEVEPTCFG[10]) Isochronous */
#define   USBHS_DEVEPTCFG_EPTYPE_BLK (0x2u << 11) /**< \brief (USBHS_DEVEPTCFG[10]) Bulk */
#define   USBHS_DEVEPTCFG_EPTYPE_INTRPT (0x3u << 11) /**< \brief (USBHS_DEVEPTCFG[10]) Interrupt */
#define USBHS_DEVEPTCFG_NBTRANS_Pos 13
#define USBHS_DEVEPTCFG_NBTRANS_Msk (0x3u << USBHS_DEVEPTCFG_NBTRANS_Pos) /**< \brief (USBHS_DEVEPTCFG[10]) Number of transactions per microframe for isochronous endpoint */
#define USBHS_DEVEPTCFG_NBTRANS(value) ((USBHS_DEVEPTCFG_NBTRANS_Msk & ((value) << USBHS_DEVEPTCFG_NBTRANS_Pos)))
#define   USBHS_DEVEPTCFG_NBTRANS_0_TRANS (0x0u << 13) /**< \brief (USBHS_DEVEPTCFG[10]) Reserved to endpoint that does not have the high-bandwidth isochronous capability. */
#define   USBHS_DEVEPTCFG_NBTRANS_1_TRANS (0x1u << 13) /**< \brief (USBHS_DEVEPTCFG[10]) Default value: one transaction per microframe. */
#define   USBHS_DEVEPTCFG_NBTRANS_2_TRANS (0x2u << 13) /**< \brief (USBHS_DEVEPTCFG[10]) Two transactions per microframe. This endpoint should be configured as double-bank. */
#define   USBHS_DEVEPTCFG_NBTRANS_3_TRANS (0x3u << 13) /**< \brief (USBHS_DEVEPTCFG[10]) Three transactions per microframe. This endpoint should be configured as triple-bank. */
/* -------- USBHS_DEVEPTISR[10] : (USBHS Offset: 0x130) Device Endpoint Status Register (n = 0) -------- */
#define USBHS_DEVEPTISR_TXINI (0x1u << 0) /**< \brief (USBHS_DEVEPTISR[10]) Transmitted IN Data Interrupt */
#define USBHS_DEVEPTISR_RXOUTI (0x1u << 1) /**< \brief (USBHS_DEVEPTISR[10]) Received OUT Data Interrupt */
#define USBHS_DEVEPTISR_RXSTPI (0x1u << 2) /**< \brief (USBHS_DEVEPTISR[10]) Received SETUP Interrupt */
#define USBHS_DEVEPTISR_NAKOUTI (0x1u << 3) /**< \brief (USBHS_DEVEPTISR[10]) NAKed OUT Interrupt */
#define USBHS_DEVEPTISR_NAKINI (0x1u << 4) /**< \brief (USBHS_DEVEPTISR[10]) NAKed IN Interrupt */
#define USBHS_DEVEPTISR_OVERFI (0x1u << 5) /**< \brief (USBHS_DEVEPTISR[10]) Overflow Interrupt */
#define USBHS_DEVEPTISR_STALLEDI (0x1u << 6) /**< \brief (USBHS_DEVEPTISR[10]) STALLed Interrupt */
#define USBHS_DEVEPTISR_SHORTPACKET (0x1u << 7) /**< \brief (USBHS_DEVEPTISR[10]) Short Packet Interrupt */
#define USBHS_DEVEPTISR_DTSEQ_Pos 8
#define USBHS_DEVEPTISR_DTSEQ_Msk (0x3u << USBHS_DEVEPTISR_DTSEQ_Pos) /**< \brief (USBHS_DEVEPTISR[10]) Data Toggle Sequence */
#define   USBHS_DEVEPTISR_DTSEQ_DATA0 (0x0u << 8) /**< \brief (USBHS_DEVEPTISR[10]) Data0 toggle sequence */
#define   USBHS_DEVEPTISR_DTSEQ_DATA1 (0x1u << 8) /**< \brief (USBHS_DEVEPTISR[10]) Data1 toggle sequence */
#define   USBHS_DEVEPTISR_DTSEQ_DATA2 (0x2u << 8) /**< \brief (USBHS_DEVEPTISR[10]) Reserved for high-bandwidth isochronous endpoint */
#define   USBHS_DEVEPTISR_DTSEQ_MDATA (0x3u << 8) /**< \brief (USBHS_DEVEPTISR[10]) Reserved for high-bandwidth isochronous endpoint */
#define USBHS_DEVEPTISR_NBUSYBK_Pos 12
#define USBHS_DEVEPTISR_NBUSYBK_Msk (0x3u << USBHS_DEVEPTISR_NBUSYBK_Pos) /**< \brief (USBHS_DEVEPTISR[10]) Number of Busy Banks */
#define   USBHS_DEVEPTISR_NBUSYBK_0_BUSY (0x0u << 12) /**< \brief (USBHS_DEVEPTISR[10]) 0 busy bank (all banks free) */
#define   USBHS_DEVEPTISR_NBUSYBK_1_BUSY (0x1u << 12) /**< \brief (USBHS_DEVEPTISR[10]) 1 busy bank */
#define   USBHS_DEVEPTISR_NBUSYBK_2_BUSY (0x2u << 12) /**< \brief (USBHS_DEVEPTISR[10]) 2 busy banks */
#define   USBHS_DEVEPTISR_NBUSYBK_3_BUSY (0x3u << 12) /**< \brief (USBHS_DEVEPTISR[10]) 3 busy banks */
#define USBHS_DEVEPTISR_CURRBK_Pos 14
#define USBHS_DEVEPTISR_CURRBK_Msk (0x3u << USBHS_DEVEPTISR_CURRBK_Pos) /**< \brief (USBHS_DEVEPTISR[10]) Current Bank */
#define   USBHS_DEVEPTISR_CURRBK_BANK0 (0x0u << 14) /**< \brief (USBHS_DEVEPTISR[10]) Current bank is bank0 */
#define   USBHS_DEVEPTISR_CURRBK_BANK1 (0x1u << 14) /**< \brief (USBHS_DEVEPTISR[10]) Current bank is bank1 */
#define   USBHS_DEVEPTISR_CURRBK_BANK2 (0x2u << 14) /**< \brief (USBHS_DEVEPTISR[10]) Current bank is bank2 */
#define USBHS_DEVEPTISR_RWALL (0x1u << 16) /**< \brief (USBHS_DEVEPTISR[10]) Read/Write Allowed */
#define USBHS_DEVEPTISR_CTRLDIR (0x1u << 17) /**< \brief (USBHS_DEVEPTISR[10]) Control Direction */
#define USBHS_DEVEPTISR_CFGOK (0x1u << 18) /**< \brief (USBHS_DEVEPTISR[10]) Configuration OK Status */
#define USBHS_DEVEPTISR_BYCT_Pos 20
#define USBHS_DEVEPTISR_BYCT_Msk (0x7ffu << USBHS_DEVEPTISR_BYCT_Pos) /**< \brief (USBHS_DEVEPTISR[10]) Byte Count */
#define USBHS_DEVEPTISR_UNDERFI (0x1u << 2) /**< \brief (USBHS_DEVEPTISR[10]) Underflow Interrupt */
#define USBHS_DEVEPTISR_HBISOINERRI (0x1u << 3) /**< \brief (USBHS_DEVEPTISR[10]) High Bandwidth Isochronous IN Underflow Error Interrupt */
#define USBHS_DEVEPTISR_HBISOFLUSHI (0x1u << 4) /**< \brief (USBHS_DEVEPTISR[10]) High Bandwidth Isochronous IN Flush Interrupt */
#define USBHS_DEVEPTISR_CRCERRI (0x1u << 6) /**< \brief (USBHS_DEVEPTISR[10]) CRC Error Interrupt */
#define USBHS_DEVEPTISR_ERRORTRANS (0x1u << 10) /**< \brief (USBHS_DEVEPTISR[10]) High-bandwidth Isochronous OUT Endpoint Transaction Error Interrupt */
/* -------- USBHS_DEVEPTICR[10] : (USBHS Offset: 0x160) Device Endpoint Clear Register (n = 0) -------- */
#define USBHS_DEVEPTICR_TXINIC (0x1u << 0) /**< \brief (USBHS_DEVEPTICR[10]) Transmitted IN Data Interrupt Clear */
#define USBHS_DEVEPTICR_RXOUTIC (0x1u << 1) /**< \brief (USBHS_DEVEPTICR[10]) Received OUT Data Interrupt Clear */
#define USBHS_DEVEPTICR_RXSTPIC (0x1u << 2) /**< \brief (USBHS_DEVEPTICR[10]) Received SETUP Interrupt Clear */
#define USBHS_DEVEPTICR_NAKOUTIC (0x1u << 3) /**< \brief (USBHS_DEVEPTICR[10]) NAKed OUT Interrupt Clear */
#define USBHS_DEVEPTICR_NAKINIC (0x1u << 4) /**< \brief (USBHS_DEVEPTICR[10]) NAKed IN Interrupt Clear */
#define USBHS_DEVEPTICR_OVERFIC (0x1u << 5) /**< \brief (USBHS_DEVEPTICR[10]) Overflow Interrupt Clear */
#define USBHS_DEVEPTICR_STALLEDIC (0x1u << 6) /**< \brief (USBHS_DEVEPTICR[10]) STALLed Interrupt Clear */
#define USBHS_DEVEPTICR_SHORTPACKETC (0x1u << 7) /**< \brief (USBHS_DEVEPTICR[10]) Short Packet Interrupt Clear */
#define USBHS_DEVEPTICR_UNDERFIC (0x1u << 2) /**< \brief (USBHS_DEVEPTICR[10]) Underflow Interrupt Clear */
#define USBHS_DEVEPTICR_HBISOINERRIC (0x1u << 3) /**< \brief (USBHS_DEVEPTICR[10]) High Bandwidth Isochronous IN Underflow Error Interrupt Clear */
#define USBHS_DEVEPTICR_HBISOFLUSHIC (0x1u << 4) /**< \brief (USBHS_DEVEPTICR[10]) High Bandwidth Isochronous IN Flush Interrupt Clear */
#define USBHS_DEVEPTICR_CRCERRIC (0x1u << 6) /**< \brief (USBHS_DEVEPTICR[10]) CRC Error Interrupt Clear */
/* -------- USBHS_DEVEPTIFR[10] : (USBHS Offset: 0x190) Device Endpoint Set Register (n = 0) -------- */
#define USBHS_DEVEPTIFR_TXINIS (0x1u << 0) /**< \brief (USBHS_DEVEPTIFR[10]) Transmitted IN Data Interrupt Set */
#define USBHS_DEVEPTIFR_RXOUTIS (0x1u << 1) /**< \brief (USBHS_DEVEPTIFR[10]) Received OUT Data Interrupt Set */
#define USBHS_DEVEPTIFR_RXSTPIS (0x1u << 2) /**< \brief (USBHS_DEVEPTIFR[10]) Received SETUP Interrupt Set */
#define USBHS_DEVEPTIFR_NAKOUTIS (0x1u << 3) /**< \brief (USBHS_DEVEPTIFR[10]) NAKed OUT Interrupt Set */
#define USBHS_DEVEPTIFR_NAKINIS (0x1u << 4) /**< \brief (USBHS_DEVEPTIFR[10]) NAKed IN Interrupt Set */
#define USBHS_DEVEPTIFR_OVERFIS (0x1u << 5) /**< \brief (USBHS_DEVEPTIFR[10]) Overflow Interrupt Set */
#define USBHS_DEVEPTIFR_STALLEDIS (0x1u << 6) /**< \brief (USBHS_DEVEPTIFR[10]) STALLed Interrupt Set */
#define USBHS_DEVEPTIFR_SHORTPACKETS (0x1u << 7) /**< \brief (USBHS_DEVEPTIFR[10]) Short Packet Interrupt Set */
#define USBHS_DEVEPTIFR_NBUSYBKS (0x1u << 12) /**< \brief (USBHS_DEVEPTIFR[10]) Number of Busy Banks Interrupt Set */
#define USBHS_DEVEPTIFR_UNDERFIS (0x1u << 2) /**< \brief (USBHS_DEVEPTIFR[10]) Underflow Interrupt Set */
#define USBHS_DEVEPTIFR_HBISOINERRIS (0x1u << 3) /**< \brief (USBHS_DEVEPTIFR[10]) High Bandwidth Isochronous IN Underflow Error Interrupt Set */
#define USBHS_DEVEPTIFR_HBISOFLUSHIS (0x1u << 4) /**< \brief (USBHS_DEVEPTIFR[10]) High Bandwidth Isochronous IN Flush Interrupt Set */
#define USBHS_DEVEPTIFR_CRCERRIS (0x1u << 6) /**< \brief (USBHS_DEVEPTIFR[10]) CRC Error Interrupt Set */
/* -------- USBHS_DEVEPTIMR[10] : (USBHS Offset: 0x1C0) Device Endpoint Mask Register (n = 0) -------- */
#define USBHS_DEVEPTIMR_TXINE (0x1u << 0) /**< \brief (USBHS_DEVEPTIMR[10]) Transmitted IN Data Interrupt */
#define USBHS_DEVEPTIMR_RXOUTE (0x1u << 1) /**< \brief (USBHS_DEVEPTIMR[10]) Received OUT Data Interrupt */
#define USBHS_DEVEPTIMR_RXSTPE (0x1u << 2) /**< \brief (USBHS_DEVEPTIMR[10]) Received SETUP Interrupt */
#define USBHS_DEVEPTIMR_NAKOUTE (0x1u << 3) /**< \brief (USBHS_DEVEPTIMR[10]) NAKed OUT Interrupt */
#define USBHS_DEVEPTIMR_NAKINE (0x1u << 4) /**< \brief (USBHS_DEVEPTIMR[10]) NAKed IN Interrupt */
#define USBHS_DEVEPTIMR_OVERFE (0x1u << 5) /**< \brief (USBHS_DEVEPTIMR[10]) Overflow Interrupt */
#define USBHS_DEVEPTIMR_STALLEDE (0x1u << 6) /**< \brief (USBHS_DEVEPTIMR[10]) STALLed Interrupt */
#define USBHS_DEVEPTIMR_SHORTPACKETE (0x1u << 7) /**< \brief (USBHS_DEVEPTIMR[10]) Short Packet Interrupt */
#define USBHS_DEVEPTIMR_NBUSYBKE (0x1u << 12) /**< \brief (USBHS_DEVEPTIMR[10]) Number of Busy Banks Interrupt */
#define USBHS_DEVEPTIMR_KILLBK (0x1u << 13) /**< \brief (USBHS_DEVEPTIMR[10]) Kill IN Bank */
#define USBHS_DEVEPTIMR_FIFOCON (0x1u << 14) /**< \brief (USBHS_DEVEPTIMR[10]) FIFO Control */
#define USBHS_DEVEPTIMR_EPDISHDMA (0x1u << 16) /**< \brief (USBHS_DEVEPTIMR[10]) Endpoint Interrupts Disable HDMA Request */
#define USBHS_DEVEPTIMR_NYETDIS (0x1u << 17) /**< \brief (USBHS_DEVEPTIMR[10]) NYET Token Disable */
#define USBHS_DEVEPTIMR_RSTDT (0x1u << 18) /**< \brief (USBHS_DEVEPTIMR[10]) Reset Data Toggle */
#define USBHS_DEVEPTIMR_STALLRQ (0x1u << 19) /**< \brief (USBHS_DEVEPTIMR[10]) STALL Request */
#define USBHS_DEVEPTIMR_UNDERFE (0x1u << 2) /**< \brief (USBHS_DEVEPTIMR[10]) Underflow Interrupt */
#define USBHS_DEVEPTIMR_HBISOINERRE (0x1u << 3) /**< \brief (USBHS_DEVEPTIMR[10]) High Bandwidth Isochronous IN Error Interrupt */
#define USBHS_DEVEPTIMR_HBISOFLUSHE (0x1u << 4) /**< \brief (USBHS_DEVEPTIMR[10]) High Bandwidth Isochronous IN Flush Interrupt */
#define USBHS_DEVEPTIMR_CRCERRE (0x1u << 6) /**< \brief (USBHS_DEVEPTIMR[10]) CRC Error Interrupt */
#define USBHS_DEVEPTIMR_MDATAE (0x1u << 8) /**< \brief (USBHS_DEVEPTIMR[10]) MData Interrupt */
#define USBHS_DEVEPTIMR_DATAXE (0x1u << 9) /**< \brief (USBHS_DEVEPTIMR[10]) DataX Interrupt */
#define USBHS_DEVEPTIMR_ERRORTRANSE (0x1u << 10) /**< \brief (USBHS_DEVEPTIMR[10]) Transaction Error Interrupt */
/* -------- USBHS_DEVEPTIER[10] : (USBHS Offset: 0x1F0) Device Endpoint Enable Register (n = 0) -------- */
#define USBHS_DEVEPTIER_TXINES (0x1u << 0) /**< \brief (USBHS_DEVEPTIER[10]) Transmitted IN Data Interrupt Enable */
#define USBHS_DEVEPTIER_RXOUTES (0x1u << 1) /**< \brief (USBHS_DEVEPTIER[10]) Received OUT Data Interrupt Enable */
#define USBHS_DEVEPTIER_RXSTPES (0x1u << 2) /**< \brief (USBHS_DEVEPTIER[10]) Received SETUP Interrupt Enable */
#define USBHS_DEVEPTIER_NAKOUTES (0x1u << 3) /**< \brief (USBHS_DEVEPTIER[10]) NAKed OUT Interrupt Enable */
#define USBHS_DEVEPTIER_NAKINES (0x1u << 4) /**< \brief (USBHS_DEVEPTIER[10]) NAKed IN Interrupt Enable */
#define USBHS_DEVEPTIER_OVERFES (0x1u << 5) /**< \brief (USBHS_DEVEPTIER[10]) Overflow Interrupt Enable */
#define USBHS_DEVEPTIER_STALLEDES (0x1u << 6) /**< \brief (USBHS_DEVEPTIER[10]) STALLed Interrupt Enable */
#define USBHS_DEVEPTIER_SHORTPACKETES (0x1u << 7) /**< \brief (USBHS_DEVEPTIER[10]) Short Packet Interrupt Enable */
#define USBHS_DEVEPTIER_NBUSYBKES (0x1u << 12) /**< \brief (USBHS_DEVEPTIER[10]) Number of Busy Banks Interrupt Enable */
#define USBHS_DEVEPTIER_KILLBKS (0x1u << 13) /**< \brief (USBHS_DEVEPTIER[10]) Kill IN Bank */
#define USBHS_DEVEPTIER_FIFOCONS (0x1u << 14) /**< \brief (USBHS_DEVEPTIER[10]) FIFO Control */
#define USBHS_DEVEPTIER_EPDISHDMAS (0x1u << 16) /**< \brief (USBHS_DEVEPTIER[10]) Endpoint Interrupts Disable HDMA Request Enable */
#define USBHS_DEVEPTIER_NYETDISS (0x1u << 17) /**< \brief (USBHS_DEVEPTIER[10]) NYET Token Disable Enable */
#define USBHS_DEVEPTIER_RSTDTS (0x1u << 18) /**< \brief (USBHS_DEVEPTIER[10]) Reset Data Toggle Enable */
#define USBHS_DEVEPTIER_STALLRQS (0x1u << 19) /**< \brief (USBHS_DEVEPTIER[10]) STALL Request Enable */
#define USBHS_DEVEPTIER_UNDERFES (0x1u << 2) /**< \brief (USBHS_DEVEPTIER[10]) Underflow Interrupt Enable */
#define USBHS_DEVEPTIER_HBISOINERRES (0x1u << 3) /**< \brief (USBHS_DEVEPTIER[10]) High Bandwidth Isochronous IN Error Interrupt Enable */
#define USBHS_DEVEPTIER_HBISOFLUSHES (0x1u << 4) /**< \brief (USBHS_DEVEPTIER[10]) High Bandwidth Isochronous IN Flush Interrupt Enable */
#define USBHS_DEVEPTIER_CRCERRES (0x1u << 6) /**< \brief (USBHS_DEVEPTIER[10]) CRC Error Interrupt Enable */
#define USBHS_DEVEPTIER_MDATAES (0x1u << 8) /**< \brief (USBHS_DEVEPTIER[10]) MData Interrupt Enable */
#define USBHS_DEVEPTIER_DATAXES (0x1u << 9) /**< \brief (USBHS_DEVEPTIER[10]) DataX Interrupt Enable */
#define USBHS_DEVEPTIER_ERRORTRANSES (0x1u << 10) /**< \brief (USBHS_DEVEPTIER[10]) Transaction Error Interrupt Enable */
/* -------- USBHS_DEVEPTIDR[10] : (USBHS Offset: 0x220) Device Endpoint Disable Register (n = 0) -------- */
#define USBHS_DEVEPTIDR_TXINEC (0x1u << 0) /**< \brief (USBHS_DEVEPTIDR[10]) Transmitted IN Interrupt Clear */
#define USBHS_DEVEPTIDR_RXOUTEC (0x1u << 1) /**< \brief (USBHS_DEVEPTIDR[10]) Received OUT Data Interrupt Clear */
#define USBHS_DEVEPTIDR_RXSTPEC (0x1u << 2) /**< \brief (USBHS_DEVEPTIDR[10]) Received SETUP Interrupt Clear */
#define USBHS_DEVEPTIDR_NAKOUTEC (0x1u << 3) /**< \brief (USBHS_DEVEPTIDR[10]) NAKed OUT Interrupt Clear */
#define USBHS_DEVEPTIDR_NAKINEC (0x1u << 4) /**< \brief (USBHS_DEVEPTIDR[10]) NAKed IN Interrupt Clear */
#define USBHS_DEVEPTIDR_OVERFEC (0x1u << 5) /**< \brief (USBHS_DEVEPTIDR[10]) Overflow Interrupt Clear */
#define USBHS_DEVEPTIDR_STALLEDEC (0x1u << 6) /**< \brief (USBHS_DEVEPTIDR[10]) STALLed Interrupt Clear */
#define USBHS_DEVEPTIDR_SHORTPACKETEC (0x1u << 7) /**< \brief (USBHS_DEVEPTIDR[10]) Shortpacket Interrupt Clear */
#define USBHS_DEVEPTIDR_NBUSYBKEC (0x1u << 12) /**< \brief (USBHS_DEVEPTIDR[10]) Number of Busy Banks Interrupt Clear */
#define USBHS_DEVEPTIDR_FIFOCONC (0x1u << 14) /**< \brief (USBHS_DEVEPTIDR[10]) FIFO Control Clear */
#define USBHS_DEVEPTIDR_EPDISHDMAC (0x1u << 16) /**< \brief (USBHS_DEVEPTIDR[10]) Endpoint Interrupts Disable HDMA Request Clear */
#define USBHS_DEVEPTIDR_NYETDISC (0x1u << 17) /**< \brief (USBHS_DEVEPTIDR[10]) NYET Token Disable Clear */
#define USBHS_DEVEPTIDR_STALLRQC (0x1u << 19) /**< \brief (USBHS_DEVEPTIDR[10]) STALL Request Clear */
#define USBHS_DEVEPTIDR_UNDERFEC (0x1u << 2) /**< \brief (USBHS_DEVEPTIDR[10]) Underflow Interrupt Clear */
#define USBHS_DEVEPTIDR_HBISOINERREC (0x1u << 3) /**< \brief (USBHS_DEVEPTIDR[10]) High Bandwidth Isochronous IN Error Interrupt Clear */
#define USBHS_DEVEPTIDR_HBISOFLUSHEC (0x1u << 4) /**< \brief (USBHS_DEVEPTIDR[10]) High Bandwidth Isochronous IN Flush Interrupt Clear */
#define USBHS_DEVEPTIDR_CRCERREC (0x1u << 6) /**< \brief (USBHS_DEVEPTIDR[10]) CRC Error Interrupt Clear */
#define USBHS_DEVEPTIDR_MDATEC (0x1u << 8) /**< \brief (USBHS_DEVEPTIDR[10]) MData Interrupt Clear */
#define USBHS_DEVEPTIDR_DATAXEC (0x1u << 9) /**< \brief (USBHS_DEVEPTIDR[10]) DataX Interrupt Clear */
#define USBHS_DEVEPTIDR_ERRORTRANSEC (0x1u << 10) /**< \brief (USBHS_DEVEPTIDR[10]) Transaction Error Interrupt Clear */
/* -------- USBHS_DEVDMANXTDSC : (USBHS Offset: N/A) Device DMA Channel Next Descriptor Address Register -------- */
#define USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Pos 0
#define USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Msk (0xffffffffu << USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Pos) /**< \brief (USBHS_DEVDMANXTDSC) Next Descriptor Address */
#define USBHS_DEVDMANXTDSC_NXT_DSC_ADD(value) ((USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Msk & ((value) << USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Pos)))
/* -------- USBHS_DEVDMAADDRESS : (USBHS Offset: N/A) Device DMA Channel Address Register -------- */
#define USBHS_DEVDMAADDRESS_BUFF_ADD_Pos 0
#define USBHS_DEVDMAADDRESS_BUFF_ADD_Msk (0xffffffffu << USBHS_DEVDMAADDRESS_BUFF_ADD_Pos) /**< \brief (USBHS_DEVDMAADDRESS) Buffer Address */
#define USBHS_DEVDMAADDRESS_BUFF_ADD(value) ((USBHS_DEVDMAADDRESS_BUFF_ADD_Msk & ((value) << USBHS_DEVDMAADDRESS_BUFF_ADD_Pos)))
/* -------- USBHS_DEVDMACONTROL : (USBHS Offset: N/A) Device DMA Channel Control Register -------- */
#define USBHS_DEVDMACONTROL_CHANN_ENB (0x1u << 0) /**< \brief (USBHS_DEVDMACONTROL) Channel Enable Command */
#define USBHS_DEVDMACONTROL_LDNXT_DSC (0x1u << 1) /**< \brief (USBHS_DEVDMACONTROL) Load Next Channel Transfer Descriptor Enable Command */
#define USBHS_DEVDMACONTROL_END_TR_EN (0x1u << 2) /**< \brief (USBHS_DEVDMACONTROL) End of Transfer Enable Control (OUT transfers only) */
#define USBHS_DEVDMACONTROL_END_B_EN (0x1u << 3) /**< \brief (USBHS_DEVDMACONTROL) End of Buffer Enable Control */
#define USBHS_DEVDMACONTROL_END_TR_IT (0x1u << 4) /**< \brief (USBHS_DEVDMACONTROL) End of Transfer Interrupt Enable */
#define USBHS_DEVDMACONTROL_END_BUFFIT (0x1u << 5) /**< \brief (USBHS_DEVDMACONTROL) End of Buffer Interrupt Enable */
#define USBHS_DEVDMACONTROL_DESC_LD_IT (0x1u << 6) /**< \brief (USBHS_DEVDMACONTROL) Descriptor Loaded Interrupt Enable */
#define USBHS_DEVDMACONTROL_BURST_LCK (0x1u << 7) /**< \brief (USBHS_DEVDMACONTROL) Burst Lock Enable */
#define USBHS_DEVDMACONTROL_BUFF_LENGTH_Pos 16
#define USBHS_DEVDMACONTROL_BUFF_LENGTH_Msk (0xffffu << USBHS_DEVDMACONTROL_BUFF_LENGTH_Pos) /**< \brief (USBHS_DEVDMACONTROL) Buffer Byte Length (Write-only) */
#define USBHS_DEVDMACONTROL_BUFF_LENGTH(value) ((USBHS_DEVDMACONTROL_BUFF_LENGTH_Msk & ((value) << USBHS_DEVDMACONTROL_BUFF_LENGTH_Pos)))
/* -------- USBHS_DEVDMASTATUS : (USBHS Offset: N/A) Device DMA Channel Status Register -------- */
#define USBHS_DEVDMASTATUS_CHANN_ENB (0x1u << 0) /**< \brief (USBHS_DEVDMASTATUS) Channel Enable Status */
#define USBHS_DEVDMASTATUS_CHANN_ACT (0x1u << 1) /**< \brief (USBHS_DEVDMASTATUS) Channel Active Status */
#define USBHS_DEVDMASTATUS_END_TR_ST (0x1u << 4) /**< \brief (USBHS_DEVDMASTATUS) End of Channel Transfer Status */
#define USBHS_DEVDMASTATUS_END_BF_ST (0x1u << 5) /**< \brief (USBHS_DEVDMASTATUS) End of Channel Buffer Status */
#define USBHS_DEVDMASTATUS_DESC_LDST (0x1u << 6) /**< \brief (USBHS_DEVDMASTATUS) Descriptor Loaded Status */
#define USBHS_DEVDMASTATUS_BUFF_COUNT_Pos 16
#define USBHS_DEVDMASTATUS_BUFF_COUNT_Msk (0xffffu << USBHS_DEVDMASTATUS_BUFF_COUNT_Pos) /**< \brief (USBHS_DEVDMASTATUS) Buffer Byte Count */
#define USBHS_DEVDMASTATUS_BUFF_COUNT(value) ((USBHS_DEVDMASTATUS_BUFF_COUNT_Msk & ((value) << USBHS_DEVDMASTATUS_BUFF_COUNT_Pos)))
/* -------- USBHS_HSTCTRL : (USBHS Offset: 0x0400) Host General Control Register -------- */
#define USBHS_HSTCTRL_SOFE (0x1u << 8) /**< \brief (USBHS_HSTCTRL) Start of Frame Generation Enable */
#define USBHS_HSTCTRL_RESET (0x1u << 9) /**< \brief (USBHS_HSTCTRL) Send USB Reset */
#define USBHS_HSTCTRL_RESUME (0x1u << 10) /**< \brief (USBHS_HSTCTRL) Send USB Resume */
#define USBHS_HSTCTRL_SPDCONF_Pos 12
#define USBHS_HSTCTRL_SPDCONF_Msk (0x3u << USBHS_HSTCTRL_SPDCONF_Pos) /**< \brief (USBHS_HSTCTRL) Mode Configuration */
#define USBHS_HSTCTRL_SPDCONF(value) ((USBHS_HSTCTRL_SPDCONF_Msk & ((value) << USBHS_HSTCTRL_SPDCONF_Pos)))
#define   USBHS_HSTCTRL_SPDCONF_NORMAL (0x0u << 12) /**< \brief (USBHS_HSTCTRL) The host starts in Full-speed mode and performs a high-speed reset to switch to High-speed mode if the downstream peripheral is high-speed capable. */
#define   USBHS_HSTCTRL_SPDCONF_LOW_POWER (0x1u << 12) /**< \brief (USBHS_HSTCTRL) For a better consumption, if high speed is not needed. */
#define   USBHS_HSTCTRL_SPDCONF_HIGH_SPEED (0x2u << 12) /**< \brief (USBHS_HSTCTRL) Forced high speed. */
#define   USBHS_HSTCTRL_SPDCONF_FORCED_FS (0x3u << 12) /**< \brief (USBHS_HSTCTRL) The host remains in Full-speed mode whatever the peripheral speed capability. */
/* -------- USBHS_HSTISR : (USBHS Offset: 0x0404) Host Global Interrupt Status Register -------- */
#define USBHS_HSTISR_DCONNI (0x1u << 0) /**< \brief (USBHS_HSTISR) Device Connection Interrupt */
#define USBHS_HSTISR_DDISCI (0x1u << 1) /**< \brief (USBHS_HSTISR) Device Disconnection Interrupt */
#define USBHS_HSTISR_RSTI (0x1u << 2) /**< \brief (USBHS_HSTISR) USB Reset Sent Interrupt */
#define USBHS_HSTISR_RSMEDI (0x1u << 3) /**< \brief (USBHS_HSTISR) Downstream Resume Sent Interrupt */
#define USBHS_HSTISR_RXRSMI (0x1u << 4) /**< \brief (USBHS_HSTISR) Upstream Resume Received Interrupt */
#define USBHS_HSTISR_HSOFI (0x1u << 5) /**< \brief (USBHS_HSTISR) Host Start of Frame Interrupt */
#define USBHS_HSTISR_HWUPI (0x1u << 6) /**< \brief (USBHS_HSTISR) Host Wake-Up Interrupt */
#define USBHS_HSTISR_PEP_0 (0x1u << 8) /**< \brief (USBHS_HSTISR) Pipe 0 Interrupt */
#define USBHS_HSTISR_PEP_1 (0x1u << 9) /**< \brief (USBHS_HSTISR) Pipe 1 Interrupt */
#define USBHS_HSTISR_PEP_2 (0x1u << 10) /**< \brief (USBHS_HSTISR) Pipe 2 Interrupt */
#define USBHS_HSTISR_PEP_3 (0x1u << 11) /**< \brief (USBHS_HSTISR) Pipe 3 Interrupt */
#define USBHS_HSTISR_PEP_4 (0x1u << 12) /**< \brief (USBHS_HSTISR) Pipe 4 Interrupt */
#define USBHS_HSTISR_PEP_5 (0x1u << 13) /**< \brief (USBHS_HSTISR) Pipe 5 Interrupt */
#define USBHS_HSTISR_PEP_6 (0x1u << 14) /**< \brief (USBHS_HSTISR) Pipe 6 Interrupt */
#define USBHS_HSTISR_PEP_7 (0x1u << 15) /**< \brief (USBHS_HSTISR) Pipe 7 Interrupt */
#define USBHS_HSTISR_PEP_8 (0x1u << 16) /**< \brief (USBHS_HSTISR) Pipe 8 Interrupt */
#define USBHS_HSTISR_PEP_9 (0x1u << 17) /**< \brief (USBHS_HSTISR) Pipe 9 Interrupt */
#define USBHS_HSTISR_DMA_1 (0x1u << 25) /**< \brief (USBHS_HSTISR) DMA Channel 1 Interrupt */
#define USBHS_HSTISR_DMA_2 (0x1u << 26) /**< \brief (USBHS_HSTISR) DMA Channel 2 Interrupt */
#define USBHS_HSTISR_DMA_3 (0x1u << 27) /**< \brief (USBHS_HSTISR) DMA Channel 3 Interrupt */
#define USBHS_HSTISR_DMA_4 (0x1u << 28) /**< \brief (USBHS_HSTISR) DMA Channel 4 Interrupt */
#define USBHS_HSTISR_DMA_5 (0x1u << 29) /**< \brief (USBHS_HSTISR) DMA Channel 5 Interrupt */
#define USBHS_HSTISR_DMA_6 (0x1u << 30) /**< \brief (USBHS_HSTISR) DMA Channel 6 Interrupt */
#define USBHS_HSTISR_DMA_7 (0x1u << 31) /**< \brief (USBHS_HSTISR) DMA Channel 7 Interrupt */
/* -------- USBHS_HSTICR : (USBHS Offset: 0x0408) Host Global Interrupt Clear Register -------- */
#define USBHS_HSTICR_DCONNIC (0x1u << 0) /**< \brief (USBHS_HSTICR) Device Connection Interrupt Clear */
#define USBHS_HSTICR_DDISCIC (0x1u << 1) /**< \brief (USBHS_HSTICR) Device Disconnection Interrupt Clear */
#define USBHS_HSTICR_RSTIC (0x1u << 2) /**< \brief (USBHS_HSTICR) USB Reset Sent Interrupt Clear */
#define USBHS_HSTICR_RSMEDIC (0x1u << 3) /**< \brief (USBHS_HSTICR) Downstream Resume Sent Interrupt Clear */
#define USBHS_HSTICR_RXRSMIC (0x1u << 4) /**< \brief (USBHS_HSTICR) Upstream Resume Received Interrupt Clear */
#define USBHS_HSTICR_HSOFIC (0x1u << 5) /**< \brief (USBHS_HSTICR) Host Start of Frame Interrupt Clear */
#define USBHS_HSTICR_HWUPIC (0x1u << 6) /**< \brief (USBHS_HSTICR) Host Wake-Up Interrupt Clear */
/* -------- USBHS_HSTIFR : (USBHS Offset: 0x040C) Host Global Interrupt Set Register -------- */
#define USBHS_HSTIFR_DCONNIS (0x1u << 0) /**< \brief (USBHS_HSTIFR) Device Connection Interrupt Set */
#define USBHS_HSTIFR_DDISCIS (0x1u << 1) /**< \brief (USBHS_HSTIFR) Device Disconnection Interrupt Set */
#define USBHS_HSTIFR_RSTIS (0x1u << 2) /**< \brief (USBHS_HSTIFR) USB Reset Sent Interrupt Set */
#define USBHS_HSTIFR_RSMEDIS (0x1u << 3) /**< \brief (USBHS_HSTIFR) Downstream Resume Sent Interrupt Set */
#define USBHS_HSTIFR_RXRSMIS (0x1u << 4) /**< \brief (USBHS_HSTIFR) Upstream Resume Received Interrupt Set */
#define USBHS_HSTIFR_HSOFIS (0x1u << 5) /**< \brief (USBHS_HSTIFR) Host Start of Frame Interrupt Set */
#define USBHS_HSTIFR_HWUPIS (0x1u << 6) /**< \brief (USBHS_HSTIFR) Host Wake-Up Interrupt Set */
#define USBHS_HSTIFR_DMA_1 (0x1u << 25) /**< \brief (USBHS_HSTIFR) DMA Channel 1 Interrupt Set */
#define USBHS_HSTIFR_DMA_2 (0x1u << 26) /**< \brief (USBHS_HSTIFR) DMA Channel 2 Interrupt Set */
#define USBHS_HSTIFR_DMA_3 (0x1u << 27) /**< \brief (USBHS_HSTIFR) DMA Channel 3 Interrupt Set */
#define USBHS_HSTIFR_DMA_4 (0x1u << 28) /**< \brief (USBHS_HSTIFR) DMA Channel 4 Interrupt Set */
#define USBHS_HSTIFR_DMA_5 (0x1u << 29) /**< \brief (USBHS_HSTIFR) DMA Channel 5 Interrupt Set */
#define USBHS_HSTIFR_DMA_6 (0x1u << 30) /**< \brief (USBHS_HSTIFR) DMA Channel 6 Interrupt Set */
#define USBHS_HSTIFR_DMA_7 (0x1u << 31) /**< \brief (USBHS_HSTIFR) DMA Channel 7 Interrupt Set */
/* -------- USBHS_HSTIMR : (USBHS Offset: 0x0410) Host Global Interrupt Mask Register -------- */
#define USBHS_HSTIMR_DCONNIE (0x1u << 0) /**< \brief (USBHS_HSTIMR) Device Connection Interrupt Enable */
#define USBHS_HSTIMR_DDISCIE (0x1u << 1) /**< \brief (USBHS_HSTIMR) Device Disconnection Interrupt Enable */
#define USBHS_HSTIMR_RSTIE (0x1u << 2) /**< \brief (USBHS_HSTIMR) USB Reset Sent Interrupt Enable */
#define USBHS_HSTIMR_RSMEDIE (0x1u << 3) /**< \brief (USBHS_HSTIMR) Downstream Resume Sent Interrupt Enable */
#define USBHS_HSTIMR_RXRSMIE (0x1u << 4) /**< \brief (USBHS_HSTIMR) Upstream Resume Received Interrupt Enable */
#define USBHS_HSTIMR_HSOFIE (0x1u << 5) /**< \brief (USBHS_HSTIMR) Host Start of Frame Interrupt Enable */
#define USBHS_HSTIMR_HWUPIE (0x1u << 6) /**< \brief (USBHS_HSTIMR) Host Wake-Up Interrupt Enable */
#define USBHS_HSTIMR_PEP_0 (0x1u << 8) /**< \brief (USBHS_HSTIMR) Pipe 0 Interrupt Enable */
#define USBHS_HSTIMR_PEP_1 (0x1u << 9) /**< \brief (USBHS_HSTIMR) Pipe 1 Interrupt Enable */
#define USBHS_HSTIMR_PEP_2 (0x1u << 10) /**< \brief (USBHS_HSTIMR) Pipe 2 Interrupt Enable */
#define USBHS_HSTIMR_PEP_3 (0x1u << 11) /**< \brief (USBHS_HSTIMR) Pipe 3 Interrupt Enable */
#define USBHS_HSTIMR_PEP_4 (0x1u << 12) /**< \brief (USBHS_HSTIMR) Pipe 4 Interrupt Enable */
#define USBHS_HSTIMR_PEP_5 (0x1u << 13) /**< \brief (USBHS_HSTIMR) Pipe 5 Interrupt Enable */
#define USBHS_HSTIMR_PEP_6 (0x1u << 14) /**< \brief (USBHS_HSTIMR) Pipe 6 Interrupt Enable */
#define USBHS_HSTIMR_PEP_7 (0x1u << 15) /**< \brief (USBHS_HSTIMR) Pipe 7 Interrupt Enable */
#define USBHS_HSTIMR_PEP_8 (0x1u << 16) /**< \brief (USBHS_HSTIMR) Pipe 8 Interrupt Enable */
#define USBHS_HSTIMR_PEP_9 (0x1u << 17) /**< \brief (USBHS_HSTIMR) Pipe 9 Interrupt Enable */
#define USBHS_HSTIMR_DMA_1 (0x1u << 25) /**< \brief (USBHS_HSTIMR) DMA Channel 1 Interrupt Enable */
#define USBHS_HSTIMR_DMA_2 (0x1u << 26) /**< \brief (USBHS_HSTIMR) DMA Channel 2 Interrupt Enable */
#define USBHS_HSTIMR_DMA_3 (0x1u << 27) /**< \brief (USBHS_HSTIMR) DMA Channel 3 Interrupt Enable */
#define USBHS_HSTIMR_DMA_4 (0x1u << 28) /**< \brief (USBHS_HSTIMR) DMA Channel 4 Interrupt Enable */
#define USBHS_HSTIMR_DMA_5 (0x1u << 29) /**< \brief (USBHS_HSTIMR) DMA Channel 5 Interrupt Enable */
#define USBHS_HSTIMR_DMA_6 (0x1u << 30) /**< \brief (USBHS_HSTIMR) DMA Channel 6 Interrupt Enable */
#define USBHS_HSTIMR_DMA_7 (0x1u << 31) /**< \brief (USBHS_HSTIMR) DMA Channel 7 Interrupt Enable */
/* -------- USBHS_HSTIDR : (USBHS Offset: 0x0414) Host Global Interrupt Disable Register -------- */
#define USBHS_HSTIDR_DCONNIEC (0x1u << 0) /**< \brief (USBHS_HSTIDR) Device Connection Interrupt Disable */
#define USBHS_HSTIDR_DDISCIEC (0x1u << 1) /**< \brief (USBHS_HSTIDR) Device Disconnection Interrupt Disable */
#define USBHS_HSTIDR_RSTIEC (0x1u << 2) /**< \brief (USBHS_HSTIDR) USB Reset Sent Interrupt Disable */
#define USBHS_HSTIDR_RSMEDIEC (0x1u << 3) /**< \brief (USBHS_HSTIDR) Downstream Resume Sent Interrupt Disable */
#define USBHS_HSTIDR_RXRSMIEC (0x1u << 4) /**< \brief (USBHS_HSTIDR) Upstream Resume Received Interrupt Disable */
#define USBHS_HSTIDR_HSOFIEC (0x1u << 5) /**< \brief (USBHS_HSTIDR) Host Start of Frame Interrupt Disable */
#define USBHS_HSTIDR_HWUPIEC (0x1u << 6) /**< \brief (USBHS_HSTIDR) Host Wake-Up Interrupt Disable */
#define USBHS_HSTIDR_PEP_0 (0x1u << 8) /**< \brief (USBHS_HSTIDR) Pipe 0 Interrupt Disable */
#define USBHS_HSTIDR_PEP_1 (0x1u << 9) /**< \brief (USBHS_HSTIDR) Pipe 1 Interrupt Disable */
#define USBHS_HSTIDR_PEP_2 (0x1u << 10) /**< \brief (USBHS_HSTIDR) Pipe 2 Interrupt Disable */
#define USBHS_HSTIDR_PEP_3 (0x1u << 11) /**< \brief (USBHS_HSTIDR) Pipe 3 Interrupt Disable */
#define USBHS_HSTIDR_PEP_4 (0x1u << 12) /**< \brief (USBHS_HSTIDR) Pipe 4 Interrupt Disable */
#define USBHS_HSTIDR_PEP_5 (0x1u << 13) /**< \brief (USBHS_HSTIDR) Pipe 5 Interrupt Disable */
#define USBHS_HSTIDR_PEP_6 (0x1u << 14) /**< \brief (USBHS_HSTIDR) Pipe 6 Interrupt Disable */
#define USBHS_HSTIDR_PEP_7 (0x1u << 15) /**< \brief (USBHS_HSTIDR) Pipe 7 Interrupt Disable */
#define USBHS_HSTIDR_PEP_8 (0x1u << 16) /**< \brief (USBHS_HSTIDR) Pipe 8 Interrupt Disable */
#define USBHS_HSTIDR_PEP_9 (0x1u << 17) /**< \brief (USBHS_HSTIDR) Pipe 9 Interrupt Disable */
#define USBHS_HSTIDR_DMA_1 (0x1u << 25) /**< \brief (USBHS_HSTIDR) DMA Channel 1 Interrupt Disable */
#define USBHS_HSTIDR_DMA_2 (0x1u << 26) /**< \brief (USBHS_HSTIDR) DMA Channel 2 Interrupt Disable */
#define USBHS_HSTIDR_DMA_3 (0x1u << 27) /**< \brief (USBHS_HSTIDR) DMA Channel 3 Interrupt Disable */
#define USBHS_HSTIDR_DMA_4 (0x1u << 28) /**< \brief (USBHS_HSTIDR) DMA Channel 4 Interrupt Disable */
#define USBHS_HSTIDR_DMA_5 (0x1u << 29) /**< \brief (USBHS_HSTIDR) DMA Channel 5 Interrupt Disable */
#define USBHS_HSTIDR_DMA_6 (0x1u << 30) /**< \brief (USBHS_HSTIDR) DMA Channel 6 Interrupt Disable */
#define USBHS_HSTIDR_DMA_7 (0x1u << 31) /**< \brief (USBHS_HSTIDR) DMA Channel 7 Interrupt Disable */
/* -------- USBHS_HSTIER : (USBHS Offset: 0x0418) Host Global Interrupt Enable Register -------- */
#define USBHS_HSTIER_DCONNIES (0x1u << 0) /**< \brief (USBHS_HSTIER) Device Connection Interrupt Enable */
#define USBHS_HSTIER_DDISCIES (0x1u << 1) /**< \brief (USBHS_HSTIER) Device Disconnection Interrupt Enable */
#define USBHS_HSTIER_RSTIES (0x1u << 2) /**< \brief (USBHS_HSTIER) USB Reset Sent Interrupt Enable */
#define USBHS_HSTIER_RSMEDIES (0x1u << 3) /**< \brief (USBHS_HSTIER) Downstream Resume Sent Interrupt Enable */
#define USBHS_HSTIER_RXRSMIES (0x1u << 4) /**< \brief (USBHS_HSTIER) Upstream Resume Received Interrupt Enable */
#define USBHS_HSTIER_HSOFIES (0x1u << 5) /**< \brief (USBHS_HSTIER) Host Start of Frame Interrupt Enable */
#define USBHS_HSTIER_HWUPIES (0x1u << 6) /**< \brief (USBHS_HSTIER) Host Wake-Up Interrupt Enable */
#define USBHS_HSTIER_PEP_0 (0x1u << 8) /**< \brief (USBHS_HSTIER) Pipe 0 Interrupt Enable */
#define USBHS_HSTIER_PEP_1 (0x1u << 9) /**< \brief (USBHS_HSTIER) Pipe 1 Interrupt Enable */
#define USBHS_HSTIER_PEP_2 (0x1u << 10) /**< \brief (USBHS_HSTIER) Pipe 2 Interrupt Enable */
#define USBHS_HSTIER_PEP_3 (0x1u << 11) /**< \brief (USBHS_HSTIER) Pipe 3 Interrupt Enable */
#define USBHS_HSTIER_PEP_4 (0x1u << 12) /**< \brief (USBHS_HSTIER) Pipe 4 Interrupt Enable */
#define USBHS_HSTIER_PEP_5 (0x1u << 13) /**< \brief (USBHS_HSTIER) Pipe 5 Interrupt Enable */
#define USBHS_HSTIER_PEP_6 (0x1u << 14) /**< \brief (USBHS_HSTIER) Pipe 6 Interrupt Enable */
#define USBHS_HSTIER_PEP_7 (0x1u << 15) /**< \brief (USBHS_HSTIER) Pipe 7 Interrupt Enable */
#define USBHS_HSTIER_PEP_8 (0x1u << 16) /**< \brief (USBHS_HSTIER) Pipe 8 Interrupt Enable */
#define USBHS_HSTIER_PEP_9 (0x1u << 17) /**< \brief (USBHS_HSTIER) Pipe 9 Interrupt Enable */
#define USBHS_HSTIER_DMA_1 (0x1u << 25) /**< \brief (USBHS_HSTIER) DMA Channel 1 Interrupt Enable */
#define USBHS_HSTIER_DMA_2 (0x1u << 26) /**< \brief (USBHS_HSTIER) DMA Channel 2 Interrupt Enable */
#define USBHS_HSTIER_DMA_3 (0x1u << 27) /**< \brief (USBHS_HSTIER) DMA Channel 3 Interrupt Enable */
#define USBHS_HSTIER_DMA_4 (0x1u << 28) /**< \brief (USBHS_HSTIER) DMA Channel 4 Interrupt Enable */
#define USBHS_HSTIER_DMA_5 (0x1u << 29) /**< \brief (USBHS_HSTIER) DMA Channel 5 Interrupt Enable */
#define USBHS_HSTIER_DMA_6 (0x1u << 30) /**< \brief (USBHS_HSTIER) DMA Channel 6 Interrupt Enable */
#define USBHS_HSTIER_DMA_7 (0x1u << 31) /**< \brief (USBHS_HSTIER) DMA Channel 7 Interrupt Enable */
/* -------- USBHS_HSTPIP : (USBHS Offset: 0x0041C) Host Pipe Register -------- */
#define USBHS_HSTPIP_PEN0 (0x1u << 0) /**< \brief (USBHS_HSTPIP) Pipe 0 Enable */
#define USBHS_HSTPIP_PEN1 (0x1u << 1) /**< \brief (USBHS_HSTPIP) Pipe 1 Enable */
#define USBHS_HSTPIP_PEN2 (0x1u << 2) /**< \brief (USBHS_HSTPIP) Pipe 2 Enable */
#define USBHS_HSTPIP_PEN3 (0x1u << 3) /**< \brief (USBHS_HSTPIP) Pipe 3 Enable */
#define USBHS_HSTPIP_PEN4 (0x1u << 4) /**< \brief (USBHS_HSTPIP) Pipe 4 Enable */
#define USBHS_HSTPIP_PEN5 (0x1u << 5) /**< \brief (USBHS_HSTPIP) Pipe 5 Enable */
#define USBHS_HSTPIP_PEN6 (0x1u << 6) /**< \brief (USBHS_HSTPIP) Pipe 6 Enable */
#define USBHS_HSTPIP_PEN7 (0x1u << 7) /**< \brief (USBHS_HSTPIP) Pipe 7 Enable */
#define USBHS_HSTPIP_PEN8 (0x1u << 8) /**< \brief (USBHS_HSTPIP) Pipe 8 Enable */
#define USBHS_HSTPIP_PRST0 (0x1u << 16) /**< \brief (USBHS_HSTPIP) Pipe 0 Reset */
#define USBHS_HSTPIP_PRST1 (0x1u << 17) /**< \brief (USBHS_HSTPIP) Pipe 1 Reset */
#define USBHS_HSTPIP_PRST2 (0x1u << 18) /**< \brief (USBHS_HSTPIP) Pipe 2 Reset */
#define USBHS_HSTPIP_PRST3 (0x1u << 19) /**< \brief (USBHS_HSTPIP) Pipe 3 Reset */
#define USBHS_HSTPIP_PRST4 (0x1u << 20) /**< \brief (USBHS_HSTPIP) Pipe 4 Reset */
#define USBHS_HSTPIP_PRST5 (0x1u << 21) /**< \brief (USBHS_HSTPIP) Pipe 5 Reset */
#define USBHS_HSTPIP_PRST6 (0x1u << 22) /**< \brief (USBHS_HSTPIP) Pipe 6 Reset */
#define USBHS_HSTPIP_PRST7 (0x1u << 23) /**< \brief (USBHS_HSTPIP) Pipe 7 Reset */
#define USBHS_HSTPIP_PRST8 (0x1u << 24) /**< \brief (USBHS_HSTPIP) Pipe 8 Reset */
/* -------- USBHS_HSTFNUM : (USBHS Offset: 0x0420) Host Frame Number Register -------- */
#define USBHS_HSTFNUM_MFNUM_Pos 0
#define USBHS_HSTFNUM_MFNUM_Msk (0x7u << USBHS_HSTFNUM_MFNUM_Pos) /**< \brief (USBHS_HSTFNUM) Micro Frame Number */
#define USBHS_HSTFNUM_MFNUM(value) ((USBHS_HSTFNUM_MFNUM_Msk & ((value) << USBHS_HSTFNUM_MFNUM_Pos)))
#define USBHS_HSTFNUM_FNUM_Pos 3
#define USBHS_HSTFNUM_FNUM_Msk (0x7ffu << USBHS_HSTFNUM_FNUM_Pos) /**< \brief (USBHS_HSTFNUM) Frame Number */
#define USBHS_HSTFNUM_FNUM(value) ((USBHS_HSTFNUM_FNUM_Msk & ((value) << USBHS_HSTFNUM_FNUM_Pos)))
#define USBHS_HSTFNUM_FLENHIGH_Pos 16
#define USBHS_HSTFNUM_FLENHIGH_Msk (0xffu << USBHS_HSTFNUM_FLENHIGH_Pos) /**< \brief (USBHS_HSTFNUM) Frame Length */
#define USBHS_HSTFNUM_FLENHIGH(value) ((USBHS_HSTFNUM_FLENHIGH_Msk & ((value) << USBHS_HSTFNUM_FLENHIGH_Pos)))
/* -------- USBHS_HSTADDR1 : (USBHS Offset: 0x0424) Host Address 1 Register -------- */
#define USBHS_HSTADDR1_HSTADDRP0_Pos 0
#define USBHS_HSTADDR1_HSTADDRP0_Msk (0x7fu << USBHS_HSTADDR1_HSTADDRP0_Pos) /**< \brief (USBHS_HSTADDR1) USB Host Address */
#define USBHS_HSTADDR1_HSTADDRP0(value) ((USBHS_HSTADDR1_HSTADDRP0_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP0_Pos)))
#define USBHS_HSTADDR1_HSTADDRP1_Pos 8
#define USBHS_HSTADDR1_HSTADDRP1_Msk (0x7fu << USBHS_HSTADDR1_HSTADDRP1_Pos) /**< \brief (USBHS_HSTADDR1) USB Host Address */
#define USBHS_HSTADDR1_HSTADDRP1(value) ((USBHS_HSTADDR1_HSTADDRP1_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP1_Pos)))
#define USBHS_HSTADDR1_HSTADDRP2_Pos 16
#define USBHS_HSTADDR1_HSTADDRP2_Msk (0x7fu << USBHS_HSTADDR1_HSTADDRP2_Pos) /**< \brief (USBHS_HSTADDR1) USB Host Address */
#define USBHS_HSTADDR1_HSTADDRP2(value) ((USBHS_HSTADDR1_HSTADDRP2_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP2_Pos)))
#define USBHS_HSTADDR1_HSTADDRP3_Pos 24
#define USBHS_HSTADDR1_HSTADDRP3_Msk (0x7fu << USBHS_HSTADDR1_HSTADDRP3_Pos) /**< \brief (USBHS_HSTADDR1) USB Host Address */
#define USBHS_HSTADDR1_HSTADDRP3(value) ((USBHS_HSTADDR1_HSTADDRP3_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP3_Pos)))
/* -------- USBHS_HSTADDR2 : (USBHS Offset: 0x0428) Host Address 2 Register -------- */
#define USBHS_HSTADDR2_HSTADDRP4_Pos 0
#define USBHS_HSTADDR2_HSTADDRP4_Msk (0x7fu << USBHS_HSTADDR2_HSTADDRP4_Pos) /**< \brief (USBHS_HSTADDR2) USB Host Address */
#define USBHS_HSTADDR2_HSTADDRP4(value) ((USBHS_HSTADDR2_HSTADDRP4_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP4_Pos)))
#define USBHS_HSTADDR2_HSTADDRP5_Pos 8
#define USBHS_HSTADDR2_HSTADDRP5_Msk (0x7fu << USBHS_HSTADDR2_HSTADDRP5_Pos) /**< \brief (USBHS_HSTADDR2) USB Host Address */
#define USBHS_HSTADDR2_HSTADDRP5(value) ((USBHS_HSTADDR2_HSTADDRP5_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP5_Pos)))
#define USBHS_HSTADDR2_HSTADDRP6_Pos 16
#define USBHS_HSTADDR2_HSTADDRP6_Msk (0x7fu << USBHS_HSTADDR2_HSTADDRP6_Pos) /**< \brief (USBHS_HSTADDR2) USB Host Address */
#define USBHS_HSTADDR2_HSTADDRP6(value) ((USBHS_HSTADDR2_HSTADDRP6_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP6_Pos)))
#define USBHS_HSTADDR2_HSTADDRP7_Pos 24
#define USBHS_HSTADDR2_HSTADDRP7_Msk (0x7fu << USBHS_HSTADDR2_HSTADDRP7_Pos) /**< \brief (USBHS_HSTADDR2) USB Host Address */
#define USBHS_HSTADDR2_HSTADDRP7(value) ((USBHS_HSTADDR2_HSTADDRP7_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP7_Pos)))
/* -------- USBHS_HSTADDR3 : (USBHS Offset: 0x042C) Host Address 3 Register -------- */
#define USBHS_HSTADDR3_HSTADDRP8_Pos 0
#define USBHS_HSTADDR3_HSTADDRP8_Msk (0x7fu << USBHS_HSTADDR3_HSTADDRP8_Pos) /**< \brief (USBHS_HSTADDR3) USB Host Address */
#define USBHS_HSTADDR3_HSTADDRP8(value) ((USBHS_HSTADDR3_HSTADDRP8_Msk & ((value) << USBHS_HSTADDR3_HSTADDRP8_Pos)))
#define USBHS_HSTADDR3_HSTADDRP9_Pos 8
#define USBHS_HSTADDR3_HSTADDRP9_Msk (0x7fu << USBHS_HSTADDR3_HSTADDRP9_Pos) /**< \brief (USBHS_HSTADDR3) USB Host Address */
#define USBHS_HSTADDR3_HSTADDRP9(value) ((USBHS_HSTADDR3_HSTADDRP9_Msk & ((value) << USBHS_HSTADDR3_HSTADDRP9_Pos)))
/* -------- USBHS_HSTPIPCFG[10] : (USBHS Offset: 0x500) Host Pipe Configuration Register (n = 0) -------- */
#define USBHS_HSTPIPCFG_ALLOC (0x1u << 1) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Memory Allocate */
#define USBHS_HSTPIPCFG_PBK_Pos 2
#define USBHS_HSTPIPCFG_PBK_Msk (0x3u << USBHS_HSTPIPCFG_PBK_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Banks */
#define USBHS_HSTPIPCFG_PBK(value) ((USBHS_HSTPIPCFG_PBK_Msk & ((value) << USBHS_HSTPIPCFG_PBK_Pos)))
#define   USBHS_HSTPIPCFG_PBK_1_BANK (0x0u << 2) /**< \brief (USBHS_HSTPIPCFG[10]) Single-bank pipe */
#define   USBHS_HSTPIPCFG_PBK_2_BANK (0x1u << 2) /**< \brief (USBHS_HSTPIPCFG[10]) Double-bank pipe */
#define   USBHS_HSTPIPCFG_PBK_3_BANK (0x2u << 2) /**< \brief (USBHS_HSTPIPCFG[10]) Triple-bank pipe */
#define USBHS_HSTPIPCFG_PSIZE_Pos 4
#define USBHS_HSTPIPCFG_PSIZE_Msk (0x7u << USBHS_HSTPIPCFG_PSIZE_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Size */
#define USBHS_HSTPIPCFG_PSIZE(value) ((USBHS_HSTPIPCFG_PSIZE_Msk & ((value) << USBHS_HSTPIPCFG_PSIZE_Pos)))
#define   USBHS_HSTPIPCFG_PSIZE_8_BYTE (0x0u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 8 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_16_BYTE (0x1u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 16 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_32_BYTE (0x2u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 32 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_64_BYTE (0x3u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 64 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_128_BYTE (0x4u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 128 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_256_BYTE (0x5u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 256 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_512_BYTE (0x6u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 512 bytes */
#define   USBHS_HSTPIPCFG_PSIZE_1024_BYTE (0x7u << 4) /**< \brief (USBHS_HSTPIPCFG[10]) 1024 bytes */
#define USBHS_HSTPIPCFG_PTOKEN_Pos 8
#define USBHS_HSTPIPCFG_PTOKEN_Msk (0x3u << USBHS_HSTPIPCFG_PTOKEN_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Token */
#define USBHS_HSTPIPCFG_PTOKEN(value) ((USBHS_HSTPIPCFG_PTOKEN_Msk & ((value) << USBHS_HSTPIPCFG_PTOKEN_Pos)))
#define   USBHS_HSTPIPCFG_PTOKEN_SETUP (0x0u << 8) /**< \brief (USBHS_HSTPIPCFG[10]) SETUP */
#define   USBHS_HSTPIPCFG_PTOKEN_IN (0x1u << 8) /**< \brief (USBHS_HSTPIPCFG[10]) IN */
#define   USBHS_HSTPIPCFG_PTOKEN_OUT (0x2u << 8) /**< \brief (USBHS_HSTPIPCFG[10]) OUT */
#define USBHS_HSTPIPCFG_AUTOSW (0x1u << 10) /**< \brief (USBHS_HSTPIPCFG[10]) Automatic Switch */
#define USBHS_HSTPIPCFG_PTYPE_Pos 12
#define USBHS_HSTPIPCFG_PTYPE_Msk (0x3u << USBHS_HSTPIPCFG_PTYPE_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Type */
#define USBHS_HSTPIPCFG_PTYPE(value) ((USBHS_HSTPIPCFG_PTYPE_Msk & ((value) << USBHS_HSTPIPCFG_PTYPE_Pos)))
#define   USBHS_HSTPIPCFG_PTYPE_CTRL (0x0u << 12) /**< \brief (USBHS_HSTPIPCFG[10]) Control */
#define   USBHS_HSTPIPCFG_PTYPE_ISO (0x1u << 12) /**< \brief (USBHS_HSTPIPCFG[10]) Isochronous */
#define   USBHS_HSTPIPCFG_PTYPE_BLK (0x2u << 12) /**< \brief (USBHS_HSTPIPCFG[10]) Bulk */
#define   USBHS_HSTPIPCFG_PTYPE_INTRPT (0x3u << 12) /**< \brief (USBHS_HSTPIPCFG[10]) Interrupt */
#define USBHS_HSTPIPCFG_PEPNUM_Pos 16
#define USBHS_HSTPIPCFG_PEPNUM_Msk (0xfu << USBHS_HSTPIPCFG_PEPNUM_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Endpoint Number */
#define USBHS_HSTPIPCFG_PEPNUM(value) ((USBHS_HSTPIPCFG_PEPNUM_Msk & ((value) << USBHS_HSTPIPCFG_PEPNUM_Pos)))
#define USBHS_HSTPIPCFG_INTFRQ_Pos 24
#define USBHS_HSTPIPCFG_INTFRQ_Msk (0xffu << USBHS_HSTPIPCFG_INTFRQ_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Pipe Interrupt Request Frequency */
#define USBHS_HSTPIPCFG_INTFRQ(value) ((USBHS_HSTPIPCFG_INTFRQ_Msk & ((value) << USBHS_HSTPIPCFG_INTFRQ_Pos)))
#define USBHS_HSTPIPCFG_PINGEN (0x1u << 20) /**< \brief (USBHS_HSTPIPCFG[10]) Ping Enable */
#define USBHS_HSTPIPCFG_BINTERVAL_Pos 24
#define USBHS_HSTPIPCFG_BINTERVAL_Msk (0xffu << USBHS_HSTPIPCFG_BINTERVAL_Pos) /**< \brief (USBHS_HSTPIPCFG[10]) Binterval Parameter for the Bulk-Out/Ping Transaction */
#define USBHS_HSTPIPCFG_BINTERVAL(value) ((USBHS_HSTPIPCFG_BINTERVAL_Msk & ((value) << USBHS_HSTPIPCFG_BINTERVAL_Pos)))
/* -------- USBHS_HSTPIPISR[10] : (USBHS Offset: 0x530) Host Pipe Status Register (n = 0) -------- */
#define USBHS_HSTPIPISR_RXINI (0x1u << 0) /**< \brief (USBHS_HSTPIPISR[10]) Received IN Data Interrupt */
#define USBHS_HSTPIPISR_TXOUTI (0x1u << 1) /**< \brief (USBHS_HSTPIPISR[10]) Transmitted OUT Data Interrupt */
#define USBHS_HSTPIPISR_TXSTPI (0x1u << 2) /**< \brief (USBHS_HSTPIPISR[10]) Transmitted SETUP Interrupt */
#define USBHS_HSTPIPISR_PERRI (0x1u << 3) /**< \brief (USBHS_HSTPIPISR[10]) Pipe Error Interrupt */
#define USBHS_HSTPIPISR_NAKEDI (0x1u << 4) /**< \brief (USBHS_HSTPIPISR[10]) NAKed Interrupt */
#define USBHS_HSTPIPISR_OVERFI (0x1u << 5) /**< \brief (USBHS_HSTPIPISR[10]) Overflow Interrupt */
#define USBHS_HSTPIPISR_RXSTALLDI (0x1u << 6) /**< \brief (USBHS_HSTPIPISR[10]) Received STALLed Interrupt */
#define USBHS_HSTPIPISR_SHORTPACKETI (0x1u << 7) /**< \brief (USBHS_HSTPIPISR[10]) Short Packet Interrupt */
#define USBHS_HSTPIPISR_DTSEQ_Pos 8
#define USBHS_HSTPIPISR_DTSEQ_Msk (0x3u << USBHS_HSTPIPISR_DTSEQ_Pos) /**< \brief (USBHS_HSTPIPISR[10]) Data Toggle Sequence */
#define   USBHS_HSTPIPISR_DTSEQ_DATA0 (0x0u << 8) /**< \brief (USBHS_HSTPIPISR[10]) Data0 toggle sequence */
#define   USBHS_HSTPIPISR_DTSEQ_DATA1 (0x1u << 8) /**< \brief (USBHS_HSTPIPISR[10]) Data1 toggle sequence */
#define USBHS_HSTPIPISR_NBUSYBK_Pos 12
#define USBHS_HSTPIPISR_NBUSYBK_Msk (0x3u << USBHS_HSTPIPISR_NBUSYBK_Pos) /**< \brief (USBHS_HSTPIPISR[10]) Number of Busy Banks */
#define   USBHS_HSTPIPISR_NBUSYBK_0_BUSY (0x0u << 12) /**< \brief (USBHS_HSTPIPISR[10]) 0 busy bank (all banks free) */
#define   USBHS_HSTPIPISR_NBUSYBK_1_BUSY (0x1u << 12) /**< \brief (USBHS_HSTPIPISR[10]) 1 busy bank */
#define   USBHS_HSTPIPISR_NBUSYBK_2_BUSY (0x2u << 12) /**< \brief (USBHS_HSTPIPISR[10]) 2 busy banks */
#define   USBHS_HSTPIPISR_NBUSYBK_3_BUSY (0x3u << 12) /**< \brief (USBHS_HSTPIPISR[10]) 3 busy banks */
#define USBHS_HSTPIPISR_CURRBK_Pos 14
#define USBHS_HSTPIPISR_CURRBK_Msk (0x3u << USBHS_HSTPIPISR_CURRBK_Pos) /**< \brief (USBHS_HSTPIPISR[10]) Current Bank */
#define   USBHS_HSTPIPISR_CURRBK_BANK0 (0x0u << 14) /**< \brief (USBHS_HSTPIPISR[10]) Current bank is bank0 */
#define   USBHS_HSTPIPISR_CURRBK_BANK1 (0x1u << 14) /**< \brief (USBHS_HSTPIPISR[10]) Current bank is bank1 */
#define   USBHS_HSTPIPISR_CURRBK_BANK2 (0x2u << 14) /**< \brief (USBHS_HSTPIPISR[10]) Current bank is bank2 */
#define USBHS_HSTPIPISR_RWALL (0x1u << 16) /**< \brief (USBHS_HSTPIPISR[10]) Read/Write Allowed */
#define USBHS_HSTPIPISR_CFGOK (0x1u << 18) /**< \brief (USBHS_HSTPIPISR[10]) Configuration OK Status */
#define USBHS_HSTPIPISR_PBYCT_Pos 20
#define USBHS_HSTPIPISR_PBYCT_Msk (0x7ffu << USBHS_HSTPIPISR_PBYCT_Pos) /**< \brief (USBHS_HSTPIPISR[10]) Pipe Byte Count */
#define USBHS_HSTPIPISR_UNDERFI (0x1u << 2) /**< \brief (USBHS_HSTPIPISR[10]) Underflow Interrupt */
#define USBHS_HSTPIPISR_CRCERRI (0x1u << 6) /**< \brief (USBHS_HSTPIPISR[10]) CRC Error Interrupt */
/* -------- USBHS_HSTPIPICR[10] : (USBHS Offset: 0x560) Host Pipe Clear Register (n = 0) -------- */
#define USBHS_HSTPIPICR_RXINIC (0x1u << 0) /**< \brief (USBHS_HSTPIPICR[10]) Received IN Data Interrupt Clear */
#define USBHS_HSTPIPICR_TXOUTIC (0x1u << 1) /**< \brief (USBHS_HSTPIPICR[10]) Transmitted OUT Data Interrupt Clear */
#define USBHS_HSTPIPICR_TXSTPIC (0x1u << 2) /**< \brief (USBHS_HSTPIPICR[10]) Transmitted SETUP Interrupt Clear */
#define USBHS_HSTPIPICR_NAKEDIC (0x1u << 4) /**< \brief (USBHS_HSTPIPICR[10]) NAKed Interrupt Clear */
#define USBHS_HSTPIPICR_OVERFIC (0x1u << 5) /**< \brief (USBHS_HSTPIPICR[10]) Overflow Interrupt Clear */
#define USBHS_HSTPIPICR_RXSTALLDIC (0x1u << 6) /**< \brief (USBHS_HSTPIPICR[10]) Received STALLed Interrupt Clear */
#define USBHS_HSTPIPICR_SHORTPACKETIC (0x1u << 7) /**< \brief (USBHS_HSTPIPICR[10]) Short Packet Interrupt Clear */
#define USBHS_HSTPIPICR_UNDERFIC (0x1u << 2) /**< \brief (USBHS_HSTPIPICR[10]) Underflow Interrupt Clear */
#define USBHS_HSTPIPICR_CRCERRIC (0x1u << 6) /**< \brief (USBHS_HSTPIPICR[10]) CRC Error Interrupt Clear */
/* -------- USBHS_HSTPIPIFR[10] : (USBHS Offset: 0x590) Host Pipe Set Register (n = 0) -------- */
#define USBHS_HSTPIPIFR_RXINIS (0x1u << 0) /**< \brief (USBHS_HSTPIPIFR[10]) Received IN Data Interrupt Set */
#define USBHS_HSTPIPIFR_TXOUTIS (0x1u << 1) /**< \brief (USBHS_HSTPIPIFR[10]) Transmitted OUT Data Interrupt Set */
#define USBHS_HSTPIPIFR_TXSTPIS (0x1u << 2) /**< \brief (USBHS_HSTPIPIFR[10]) Transmitted SETUP Interrupt Set */
#define USBHS_HSTPIPIFR_PERRIS (0x1u << 3) /**< \brief (USBHS_HSTPIPIFR[10]) Pipe Error Interrupt Set */
#define USBHS_HSTPIPIFR_NAKEDIS (0x1u << 4) /**< \brief (USBHS_HSTPIPIFR[10]) NAKed Interrupt Set */
#define USBHS_HSTPIPIFR_OVERFIS (0x1u << 5) /**< \brief (USBHS_HSTPIPIFR[10]) Overflow Interrupt Set */
#define USBHS_HSTPIPIFR_RXSTALLDIS (0x1u << 6) /**< \brief (USBHS_HSTPIPIFR[10]) Received STALLed Interrupt Set */
#define USBHS_HSTPIPIFR_SHORTPACKETIS (0x1u << 7) /**< \brief (USBHS_HSTPIPIFR[10]) Short Packet Interrupt Set */
#define USBHS_HSTPIPIFR_NBUSYBKS (0x1u << 12) /**< \brief (USBHS_HSTPIPIFR[10]) Number of Busy Banks Set */
#define USBHS_HSTPIPIFR_UNDERFIS (0x1u << 2) /**< \brief (USBHS_HSTPIPIFR[10]) Underflow Interrupt Set */
#define USBHS_HSTPIPIFR_CRCERRIS (0x1u << 6) /**< \brief (USBHS_HSTPIPIFR[10]) CRC Error Interrupt Set */
/* -------- USBHS_HSTPIPIMR[10] : (USBHS Offset: 0x5C0) Host Pipe Mask Register (n = 0) -------- */
#define USBHS_HSTPIPIMR_RXINE (0x1u << 0) /**< \brief (USBHS_HSTPIPIMR[10]) Received IN Data Interrupt Enable */
#define USBHS_HSTPIPIMR_TXOUTE (0x1u << 1) /**< \brief (USBHS_HSTPIPIMR[10]) Transmitted OUT Data Interrupt Enable */
#define USBHS_HSTPIPIMR_TXSTPE (0x1u << 2) /**< \brief (USBHS_HSTPIPIMR[10]) Transmitted SETUP Interrupt Enable */
#define USBHS_HSTPIPIMR_PERRE (0x1u << 3) /**< \brief (USBHS_HSTPIPIMR[10]) Pipe Error Interrupt Enable */
#define USBHS_HSTPIPIMR_NAKEDE (0x1u << 4) /**< \brief (USBHS_HSTPIPIMR[10]) NAKed Interrupt Enable */
#define USBHS_HSTPIPIMR_OVERFIE (0x1u << 5) /**< \brief (USBHS_HSTPIPIMR[10]) Overflow Interrupt Enable */
#define USBHS_HSTPIPIMR_RXSTALLDE (0x1u << 6) /**< \brief (USBHS_HSTPIPIMR[10]) Received STALLed Interrupt Enable */
#define USBHS_HSTPIPIMR_SHORTPACKETIE (0x1u << 7) /**< \brief (USBHS_HSTPIPIMR[10]) Short Packet Interrupt Enable */
#define USBHS_HSTPIPIMR_NBUSYBKE (0x1u << 12) /**< \brief (USBHS_HSTPIPIMR[10]) Number of Busy Banks Interrupt Enable */
#define USBHS_HSTPIPIMR_FIFOCON (0x1u << 14) /**< \brief (USBHS_HSTPIPIMR[10]) FIFO Control */
#define USBHS_HSTPIPIMR_PDISHDMA (0x1u << 16) /**< \brief (USBHS_HSTPIPIMR[10]) Pipe Interrupts Disable HDMA Request Enable */
#define USBHS_HSTPIPIMR_PFREEZE (0x1u << 17) /**< \brief (USBHS_HSTPIPIMR[10]) Pipe Freeze */
#define USBHS_HSTPIPIMR_RSTDT (0x1u << 18) /**< \brief (USBHS_HSTPIPIMR[10]) Reset Data Toggle */
#define USBHS_HSTPIPIMR_UNDERFIE (0x1u << 2) /**< \brief (USBHS_HSTPIPIMR[10]) Underflow Interrupt Enable */
#define USBHS_HSTPIPIMR_CRCERRE (0x1u << 6) /**< \brief (USBHS_HSTPIPIMR[10]) CRC Error Interrupt Enable */
/* -------- USBHS_HSTPIPIER[10] : (USBHS Offset: 0x5F0) Host Pipe Enable Register (n = 0) -------- */
#define USBHS_HSTPIPIER_RXINES (0x1u << 0) /**< \brief (USBHS_HSTPIPIER[10]) Received IN Data Interrupt Enable */
#define USBHS_HSTPIPIER_TXOUTES (0x1u << 1) /**< \brief (USBHS_HSTPIPIER[10]) Transmitted OUT Data Interrupt Enable */
#define USBHS_HSTPIPIER_TXSTPES (0x1u << 2) /**< \brief (USBHS_HSTPIPIER[10]) Transmitted SETUP Interrupt Enable */
#define USBHS_HSTPIPIER_PERRES (0x1u << 3) /**< \brief (USBHS_HSTPIPIER[10]) Pipe Error Interrupt Enable */
#define USBHS_HSTPIPIER_NAKEDES (0x1u << 4) /**< \brief (USBHS_HSTPIPIER[10]) NAKed Interrupt Enable */
#define USBHS_HSTPIPIER_OVERFIES (0x1u << 5) /**< \brief (USBHS_HSTPIPIER[10]) Overflow Interrupt Enable */
#define USBHS_HSTPIPIER_RXSTALLDES (0x1u << 6) /**< \brief (USBHS_HSTPIPIER[10]) Received STALLed Interrupt Enable */
#define USBHS_HSTPIPIER_SHORTPACKETIES (0x1u << 7) /**< \brief (USBHS_HSTPIPIER[10]) Short Packet Interrupt Enable */
#define USBHS_HSTPIPIER_NBUSYBKES (0x1u << 12) /**< \brief (USBHS_HSTPIPIER[10]) Number of Busy Banks Enable */
#define USBHS_HSTPIPIER_PDISHDMAS (0x1u << 16) /**< \brief (USBHS_HSTPIPIER[10]) Pipe Interrupts Disable HDMA Request Enable */
#define USBHS_HSTPIPIER_PFREEZES (0x1u << 17) /**< \brief (USBHS_HSTPIPIER[10]) Pipe Freeze Enable */
#define USBHS_HSTPIPIER_RSTDTS (0x1u << 18) /**< \brief (USBHS_HSTPIPIER[10]) Reset Data Toggle Enable */
#define USBHS_HSTPIPIER_UNDERFIES (0x1u << 2) /**< \brief (USBHS_HSTPIPIER[10]) Underflow Interrupt Enable */
#define USBHS_HSTPIPIER_CRCERRES (0x1u << 6) /**< \brief (USBHS_HSTPIPIER[10]) CRC Error Interrupt Enable */
/* -------- USBHS_HSTPIPIDR[10] : (USBHS Offset: 0x620) Host Pipe Disable Register (n = 0) -------- */
#define USBHS_HSTPIPIDR_RXINEC (0x1u << 0) /**< \brief (USBHS_HSTPIPIDR[10]) Received IN Data Interrupt Disable */
#define USBHS_HSTPIPIDR_TXOUTEC (0x1u << 1) /**< \brief (USBHS_HSTPIPIDR[10]) Transmitted OUT Data Interrupt Disable */
#define USBHS_HSTPIPIDR_TXSTPEC (0x1u << 2) /**< \brief (USBHS_HSTPIPIDR[10]) Transmitted SETUP Interrupt Disable */
#define USBHS_HSTPIPIDR_PERREC (0x1u << 3) /**< \brief (USBHS_HSTPIPIDR[10]) Pipe Error Interrupt Disable */
#define USBHS_HSTPIPIDR_NAKEDEC (0x1u << 4) /**< \brief (USBHS_HSTPIPIDR[10]) NAKed Interrupt Disable */
#define USBHS_HSTPIPIDR_OVERFIEC (0x1u << 5) /**< \brief (USBHS_HSTPIPIDR[10]) Overflow Interrupt Disable */
#define USBHS_HSTPIPIDR_RXSTALLDEC (0x1u << 6) /**< \brief (USBHS_HSTPIPIDR[10]) Received STALLed Interrupt Disable */
#define USBHS_HSTPIPIDR_SHORTPACKETIEC (0x1u << 7) /**< \brief (USBHS_HSTPIPIDR[10]) Short Packet Interrupt Disable */
#define USBHS_HSTPIPIDR_NBUSYBKEC (0x1u << 12) /**< \brief (USBHS_HSTPIPIDR[10]) Number of Busy Banks Disable */
#define USBHS_HSTPIPIDR_FIFOCONC (0x1u << 14) /**< \brief (USBHS_HSTPIPIDR[10]) FIFO Control Disable */
#define USBHS_HSTPIPIDR_PDISHDMAC (0x1u << 16) /**< \brief (USBHS_HSTPIPIDR[10]) Pipe Interrupts Disable HDMA Request Disable */
#define USBHS_HSTPIPIDR_PFREEZEC (0x1u << 17) /**< \brief (USBHS_HSTPIPIDR[10]) Pipe Freeze Disable */
#define USBHS_HSTPIPIDR_UNDERFIEC (0x1u << 2) /**< \brief (USBHS_HSTPIPIDR[10]) Underflow Interrupt Disable */
#define USBHS_HSTPIPIDR_CRCERREC (0x1u << 6) /**< \brief (USBHS_HSTPIPIDR[10]) CRC Error Interrupt Disable */
/* -------- USBHS_HSTPIPINRQ[10] : (USBHS Offset: 0x650) Host Pipe IN Request Register (n = 0) -------- */
#define USBHS_HSTPIPINRQ_INRQ_Pos 0
#define USBHS_HSTPIPINRQ_INRQ_Msk (0xffu << USBHS_HSTPIPINRQ_INRQ_Pos) /**< \brief (USBHS_HSTPIPINRQ[10]) IN Request Number before Freeze */
#define USBHS_HSTPIPINRQ_INRQ(value) ((USBHS_HSTPIPINRQ_INRQ_Msk & ((value) << USBHS_HSTPIPINRQ_INRQ_Pos)))
#define USBHS_HSTPIPINRQ_INMODE (0x1u << 8) /**< \brief (USBHS_HSTPIPINRQ[10]) IN Request Mode */
/* -------- USBHS_HSTPIPERR[10] : (USBHS Offset: 0x680) Host Pipe Error Register (n = 0) -------- */
#define USBHS_HSTPIPERR_DATATGL (0x1u << 0) /**< \brief (USBHS_HSTPIPERR[10]) Data Toggle Error */
#define USBHS_HSTPIPERR_DATAPID (0x1u << 1) /**< \brief (USBHS_HSTPIPERR[10]) Data PID Error */
#define USBHS_HSTPIPERR_PID (0x1u << 2) /**< \brief (USBHS_HSTPIPERR[10]) Data PID Error */
#define USBHS_HSTPIPERR_TIMEOUT (0x1u << 3) /**< \brief (USBHS_HSTPIPERR[10]) Time-Out Error */
#define USBHS_HSTPIPERR_CRC16 (0x1u << 4) /**< \brief (USBHS_HSTPIPERR[10]) CRC16 Error */
#define USBHS_HSTPIPERR_COUNTER_Pos 5
#define USBHS_HSTPIPERR_COUNTER_Msk (0x3u << USBHS_HSTPIPERR_COUNTER_Pos) /**< \brief (USBHS_HSTPIPERR[10]) Error Counter */
#define USBHS_HSTPIPERR_COUNTER(value) ((USBHS_HSTPIPERR_COUNTER_Msk & ((value) << USBHS_HSTPIPERR_COUNTER_Pos)))
/* -------- USBHS_HSTDMANXTDSC : (USBHS Offset: N/A) Host DMA Channel Next Descriptor Address Register -------- */
#define USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Pos 0
#define USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Msk (0xffffffffu << USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Pos) /**< \brief (USBHS_HSTDMANXTDSC) Next Descriptor Address */
#define USBHS_HSTDMANXTDSC_NXT_DSC_ADD(value) ((USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Msk & ((value) << USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Pos)))
/* -------- USBHS_HSTDMAADDRESS : (USBHS Offset: N/A) Host DMA Channel Address Register -------- */
#define USBHS_HSTDMAADDRESS_BUFF_ADD_Pos 0
#define USBHS_HSTDMAADDRESS_BUFF_ADD_Msk (0xffffffffu << USBHS_HSTDMAADDRESS_BUFF_ADD_Pos) /**< \brief (USBHS_HSTDMAADDRESS) Buffer Address */
#define USBHS_HSTDMAADDRESS_BUFF_ADD(value) ((USBHS_HSTDMAADDRESS_BUFF_ADD_Msk & ((value) << USBHS_HSTDMAADDRESS_BUFF_ADD_Pos)))
/* -------- USBHS_HSTDMACONTROL : (USBHS Offset: N/A) Host DMA Channel Control Register -------- */
#define USBHS_HSTDMACONTROL_CHANN_ENB (0x1u << 0) /**< \brief (USBHS_HSTDMACONTROL) Channel Enable Command */
#define USBHS_HSTDMACONTROL_LDNXT_DSC (0x1u << 1) /**< \brief (USBHS_HSTDMACONTROL) Load Next Channel Transfer Descriptor Enable Command */
#define USBHS_HSTDMACONTROL_END_TR_EN (0x1u << 2) /**< \brief (USBHS_HSTDMACONTROL) End of Transfer Enable Control (OUT transfers only) */
#define USBHS_HSTDMACONTROL_END_B_EN (0x1u << 3) /**< \brief (USBHS_HSTDMACONTROL) End of Buffer Enable Control */
#define USBHS_HSTDMACONTROL_END_TR_IT (0x1u << 4) /**< \brief (USBHS_HSTDMACONTROL) End of Transfer Interrupt Enable */
#define USBHS_HSTDMACONTROL_END_BUFFIT (0x1u << 5) /**< \brief (USBHS_HSTDMACONTROL) End of Buffer Interrupt Enable */
#define USBHS_HSTDMACONTROL_DESC_LD_IT (0x1u << 6) /**< \brief (USBHS_HSTDMACONTROL) Descriptor Loaded Interrupt Enable */
#define USBHS_HSTDMACONTROL_BURST_LCK (0x1u << 7) /**< \brief (USBHS_HSTDMACONTROL) Burst Lock Enable */
#define USBHS_HSTDMACONTROL_BUFF_LENGTH_Pos 16
#define USBHS_HSTDMACONTROL_BUFF_LENGTH_Msk (0xffffu << USBHS_HSTDMACONTROL_BUFF_LENGTH_Pos) /**< \brief (USBHS_HSTDMACONTROL) Buffer Byte Length (Write-only) */
#define USBHS_HSTDMACONTROL_BUFF_LENGTH(value) ((USBHS_HSTDMACONTROL_BUFF_LENGTH_Msk & ((value) << USBHS_HSTDMACONTROL_BUFF_LENGTH_Pos)))
/* -------- USBHS_HSTDMASTATUS : (USBHS Offset: N/A) Host DMA Channel Status Register -------- */
#define USBHS_HSTDMASTATUS_CHANN_ENB (0x1u << 0) /**< \brief (USBHS_HSTDMASTATUS) Channel Enable Status */
#define USBHS_HSTDMASTATUS_CHANN_ACT (0x1u << 1) /**< \brief (USBHS_HSTDMASTATUS) Channel Active Status */
#define USBHS_HSTDMASTATUS_END_TR_ST (0x1u << 4) /**< \brief (USBHS_HSTDMASTATUS) End of Channel Transfer Status */
#define USBHS_HSTDMASTATUS_END_BF_ST (0x1u << 5) /**< \brief (USBHS_HSTDMASTATUS) End of Channel Buffer Status */
#define USBHS_HSTDMASTATUS_DESC_LDST (0x1u << 6) /**< \brief (USBHS_HSTDMASTATUS) Descriptor Loaded Status */
#define USBHS_HSTDMASTATUS_BUFF_COUNT_Pos 16
#define USBHS_HSTDMASTATUS_BUFF_COUNT_Msk (0xffffu << USBHS_HSTDMASTATUS_BUFF_COUNT_Pos) /**< \brief (USBHS_HSTDMASTATUS) Buffer Byte Count */
#define USBHS_HSTDMASTATUS_BUFF_COUNT(value) ((USBHS_HSTDMASTATUS_BUFF_COUNT_Msk & ((value) << USBHS_HSTDMASTATUS_BUFF_COUNT_Pos)))
/* -------- USBHS_CTRL : (USBHS Offset: 0x0800) General Control Register -------- */
#define USBHS_CTRL_RDERRE (0x1u << 4) /**< \brief (USBHS_CTRL) Remote Device Connection Error Interrupt Enable */
#define USBHS_CTRL_VBUSHWC (0x1u << 8) /**< \brief (USBHS_CTRL) VBUS Hardware Control */
#define USBHS_CTRL_FRZCLK (0x1u << 14) /**< \brief (USBHS_CTRL) Freeze USB Clock */
#define USBHS_CTRL_USBE (0x1u << 15) /**< \brief (USBHS_CTRL) USBHS Enable */
#define USBHS_CTRL_UIMOD (0x1u << 25) /**< \brief (USBHS_CTRL) USBHS Mode */
#define   USBHS_CTRL_UIMOD_HOST (0x0u << 25) /**< \brief (USBHS_CTRL) The module is in USB Host mode. */
#define   USBHS_CTRL_UIMOD_DEVICE (0x1u << 25) /**< \brief (USBHS_CTRL) The module is in USB Device mode. */
/* -------- USBHS_SR : (USBHS Offset: 0x0804) General Status Register -------- */
#define USBHS_SR_RDERRI (0x1u << 4) /**< \brief (USBHS_SR) Remote Device Connection Error Interrupt (Host mode only) */
#define USBHS_SR_VBUSRQ (0x1u << 9) /**< \brief (USBHS_SR) VBus Request (Host mode only) */
#define USBHS_SR_SPEED_Pos 12
#define USBHS_SR_SPEED_Msk (0x3u << USBHS_SR_SPEED_Pos) /**< \brief (USBHS_SR) Speed Status (Device mode only) */
#define   USBHS_SR_SPEED_FULL_SPEED (0x0u << 12) /**< \brief (USBHS_SR) Full-Speed mode */
#define   USBHS_SR_SPEED_HIGH_SPEED (0x1u << 12) /**< \brief (USBHS_SR) High-Speed mode */
#define   USBHS_SR_SPEED_LOW_SPEED (0x2u << 12) /**< \brief (USBHS_SR) Low-Speed mode */
#define USBHS_SR_CLKUSABLE (0x1u << 14) /**< \brief (USBHS_SR) UTMI Clock Usable */
/* -------- USBHS_SCR : (USBHS Offset: 0x0808) General Status Clear Register -------- */
#define USBHS_SCR_RDERRIC (0x1u << 4) /**< \brief (USBHS_SCR) Remote Device Connection Error Interrupt Clear */
#define USBHS_SCR_VBUSRQC (0x1u << 9) /**< \brief (USBHS_SCR) VBus Request Clear */
/* -------- USBHS_SFR : (USBHS Offset: 0x080C) General Status Set Register -------- */
#define USBHS_SFR_RDERRIS (0x1u << 4) /**< \brief (USBHS_SFR) Remote Device Connection Error Interrupt Set */
#define USBHS_SFR_VBUSRQS (0x1u << 9) /**< \brief (USBHS_SFR) VBus Request Set */
/* -------- USBHS_TSTA1 : (USBHS Offset: 0x0810) General Test A1 Register -------- */
#define USBHS_TSTA1_CounterA_Pos 0
#define USBHS_TSTA1_CounterA_Msk (0x7fffu << USBHS_TSTA1_CounterA_Pos) /**< \brief (USBHS_TSTA1) Counter A */
#define USBHS_TSTA1_CounterA(value) ((USBHS_TSTA1_CounterA_Msk & ((value) << USBHS_TSTA1_CounterA_Pos)))
#define USBHS_TSTA1_LoadCntA (0x1u << 15) /**< \brief (USBHS_TSTA1) Load CounterA */
#define USBHS_TSTA1_CounterB_Pos 16
#define USBHS_TSTA1_CounterB_Msk (0x3fu << USBHS_TSTA1_CounterB_Pos) /**< \brief (USBHS_TSTA1) Counter B */
#define USBHS_TSTA1_CounterB(value) ((USBHS_TSTA1_CounterB_Msk & ((value) << USBHS_TSTA1_CounterB_Pos)))
#define USBHS_TSTA1_LoadCntB (0x1u << 23) /**< \brief (USBHS_TSTA1) Load CounterB */
#define USBHS_TSTA1_SOFCntMa1_Pos 24
#define USBHS_TSTA1_SOFCntMa1_Msk (0x7fu << USBHS_TSTA1_SOFCntMa1_Pos) /**< \brief (USBHS_TSTA1) SOF Counter Max */
#define USBHS_TSTA1_SOFCntMa1(value) ((USBHS_TSTA1_SOFCntMa1_Msk & ((value) << USBHS_TSTA1_SOFCntMa1_Pos)))
#define USBHS_TSTA1_LoadSOFCnt (0x1u << 31) /**< \brief (USBHS_TSTA1) Load SOF Counter */
/* -------- USBHS_TSTA2 : (USBHS Offset: 0x0814) General Test A2 Register -------- */
#define USBHS_TSTA2_FullDetachEn (0x1u << 0) /**< \brief (USBHS_TSTA2) Full Detach Enable */
#define USBHS_TSTA2_HSSerialMode (0x1u << 1) /**< \brief (USBHS_TSTA2) HS Serial Mode */
#define USBHS_TSTA2_LoopBackMode (0x1u << 2) /**< \brief (USBHS_TSTA2) Loop-back Mode */
#define USBHS_TSTA2_DisableGatedClock (0x1u << 3) /**< \brief (USBHS_TSTA2) Disable Gated Clock */
#define USBHS_TSTA2_ForceSuspendMTo1 (0x1u << 4) /**< \brief (USBHS_TSTA2) Force SuspendM to 1 */
#define USBHS_TSTA2_ByPassDpll (0x1u << 5) /**< \brief (USBHS_TSTA2) Bypass DPLL */
#define USBHS_TSTA2_HostHSDisconnectDisable (0x1u << 6) /**< \brief (USBHS_TSTA2) Host HS Disconnect Disable */
#define USBHS_TSTA2_ForceHSRst_50ms (0x1u << 7) /**< \brief (USBHS_TSTA2) Force HS Reset to 50 ms */
#define USBHS_TSTA2_RemovePUWhenTX (0x1u << 9) /**< \brief (USBHS_TSTA2) Remove Pull-up When TX */
/* -------- USBHS_VERSION : (USBHS Offset: 0x0818) General Version Register -------- */
#define USBHS_VERSION_VERSION_Pos 0
#define USBHS_VERSION_VERSION_Msk (0xfffu << USBHS_VERSION_VERSION_Pos) /**< \brief (USBHS_VERSION) Version Number */
#define USBHS_VERSION_MFN_Pos 16
#define USBHS_VERSION_MFN_Msk (0xfu << USBHS_VERSION_MFN_Pos) /**< \brief (USBHS_VERSION) Metal Fix Number */
/* -------- USBHS_FSM : (USBHS Offset: 0x082C) General Finite State Machine Register -------- */
#define USBHS_FSM_DRDSTATE_Pos 0
#define USBHS_FSM_DRDSTATE_Msk (0xfu << USBHS_FSM_DRDSTATE_Pos) /**< \brief (USBHS_FSM) Dual Role Device State */
#define   USBHS_FSM_DRDSTATE_A_IDLESTATE (0x0u << 0) /**< \brief (USBHS_FSM) This is the start state for A-devices (when the ID pin is 0) */
#define   USBHS_FSM_DRDSTATE_A_WAIT_VRISE (0x1u << 0) /**< \brief (USBHS_FSM) In this state, the A-device waits for the voltage on VBus to rise above the A-device VBus Valid threshold (4.4 V). */
#define   USBHS_FSM_DRDSTATE_A_WAIT_BCON (0x2u << 0) /**< \brief (USBHS_FSM) In this state, the A-device waits for the B-device to signal a connection. */
#define   USBHS_FSM_DRDSTATE_A_HOST (0x3u << 0) /**< \brief (USBHS_FSM) In this state, the A-device that operates in Host mode is operational. */
#define   USBHS_FSM_DRDSTATE_A_SUSPEND (0x4u << 0) /**< \brief (USBHS_FSM) The A-device operating as a host is in the Suspend mode. */
#define   USBHS_FSM_DRDSTATE_A_PERIPHERAL (0x5u << 0) /**< \brief (USBHS_FSM) The A-device operates as a peripheral. */
#define   USBHS_FSM_DRDSTATE_A_WAIT_VFALL (0x6u << 0) /**< \brief (USBHS_FSM) In this state, the A-device waits for the voltage on VBus to drop below the A-device Session Valid threshold (1.4 V). */
#define   USBHS_FSM_DRDSTATE_A_VBUS_ERR (0x7u << 0) /**< \brief (USBHS_FSM) In this state, the A-device waits for recovery of the over-current condition that caused it to enter this state. */
#define   USBHS_FSM_DRDSTATE_A_WAIT_DISCHARGE (0x8u << 0) /**< \brief (USBHS_FSM) In this state, the A-device waits for the data USB line to discharge (100 us). */
#define   USBHS_FSM_DRDSTATE_B_IDLE (0x9u << 0) /**< \brief (USBHS_FSM) This is the start state for B-device (when the ID pin is 1). */
#define   USBHS_FSM_DRDSTATE_B_PERIPHERAL (0xAu << 0) /**< \brief (USBHS_FSM) In this state, the B-device acts as the peripheral. */
#define   USBHS_FSM_DRDSTATE_B_WAIT_BEGIN_HNP (0xBu << 0) /**< \brief (USBHS_FSM) In this state, the B-device is in Suspend mode and waits until 3 ms before initiating the HNP protocol if requested. */
#define   USBHS_FSM_DRDSTATE_B_WAIT_DISCHARGE (0xCu << 0) /**< \brief (USBHS_FSM) In this state, the B-device waits for the data USB line to discharge (100 us)) before becoming Host. */
#define   USBHS_FSM_DRDSTATE_B_WAIT_ACON (0xDu << 0) /**< \brief (USBHS_FSM) In this state, the B-device waits for the A-device to signal a connect before becoming B-Host. */
#define   USBHS_FSM_DRDSTATE_B_HOST (0xEu << 0) /**< \brief (USBHS_FSM) In this state, the B-device acts as the Host. */
#define   USBHS_FSM_DRDSTATE_B_SRP_INIT (0xFu << 0) /**< \brief (USBHS_FSM) In this state, the B-device attempts to start a session using the SRP protocol. */

/*@}*/


#endif /* _SAME70_USBHS_COMPONENT_ */
