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

#ifndef _SAME70_USBHS_INSTANCE_
#define _SAME70_USBHS_INSTANCE_

/* ========== Register definition for USBHS peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_USBHS_DEVCTRL                         (0x40038000U) /**< \brief (USBHS) Device General Control Register */
  #define REG_USBHS_DEVISR                          (0x40038004U) /**< \brief (USBHS) Device Global Interrupt Status Register */
  #define REG_USBHS_DEVICR                          (0x40038008U) /**< \brief (USBHS) Device Global Interrupt Clear Register */
  #define REG_USBHS_DEVIFR                          (0x4003800CU) /**< \brief (USBHS) Device Global Interrupt Set Register */
  #define REG_USBHS_DEVIMR                          (0x40038010U) /**< \brief (USBHS) Device Global Interrupt Mask Register */
  #define REG_USBHS_DEVIDR                          (0x40038014U) /**< \brief (USBHS) Device Global Interrupt Disable Register */
  #define REG_USBHS_DEVIER                          (0x40038018U) /**< \brief (USBHS) Device Global Interrupt Enable Register */
  #define REG_USBHS_DEVEPT                          (0x4003801CU) /**< \brief (USBHS) Device Endpoint Register */
  #define REG_USBHS_DEVFNUM                         (0x40038020U) /**< \brief (USBHS) Device Frame Number Register */
  #define REG_USBHS_DEVEPTCFG                       (0x40038100U) /**< \brief (USBHS) Device Endpoint Configuration Register (n = 0) */
  #define REG_USBHS_DEVEPTISR                       (0x40038130U) /**< \brief (USBHS) Device Endpoint Status Register (n = 0) */
  #define REG_USBHS_DEVEPTICR                       (0x40038160U) /**< \brief (USBHS) Device Endpoint Clear Register (n = 0) */
  #define REG_USBHS_DEVEPTIFR                       (0x40038190U) /**< \brief (USBHS) Device Endpoint Set Register (n = 0) */
  #define REG_USBHS_DEVEPTIMR                       (0x400381C0U) /**< \brief (USBHS) Device Endpoint Mask Register (n = 0) */
  #define REG_USBHS_DEVEPTIER                       (0x400381F0U) /**< \brief (USBHS) Device Endpoint Enable Register (n = 0) */
  #define REG_USBHS_DEVEPTIDR                       (0x40038220U) /**< \brief (USBHS) Device Endpoint Disable Register (n = 0) */
  #define REG_USBHS_DEVDMANXTDSC1                   (0x40038310U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 1) */
  #define REG_USBHS_DEVDMAADDRESS1                  (0x40038314U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 1) */
  #define REG_USBHS_DEVDMACONTROL1                  (0x40038318U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 1) */
  #define REG_USBHS_DEVDMASTATUS1                   (0x4003831CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 1) */
  #define REG_USBHS_DEVDMANXTDSC2                   (0x40038320U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 2) */
  #define REG_USBHS_DEVDMAADDRESS2                  (0x40038324U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 2) */
  #define REG_USBHS_DEVDMACONTROL2                  (0x40038328U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 2) */
  #define REG_USBHS_DEVDMASTATUS2                   (0x4003832CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 2) */
  #define REG_USBHS_DEVDMANXTDSC3                   (0x40038330U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 3) */
  #define REG_USBHS_DEVDMAADDRESS3                  (0x40038334U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 3) */
  #define REG_USBHS_DEVDMACONTROL3                  (0x40038338U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 3) */
  #define REG_USBHS_DEVDMASTATUS3                   (0x4003833CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 3) */
  #define REG_USBHS_DEVDMANXTDSC4                   (0x40038340U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 4) */
  #define REG_USBHS_DEVDMAADDRESS4                  (0x40038344U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 4) */
  #define REG_USBHS_DEVDMACONTROL4                  (0x40038348U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 4) */
  #define REG_USBHS_DEVDMASTATUS4                   (0x4003834CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 4) */
  #define REG_USBHS_DEVDMANXTDSC5                   (0x40038350U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 5) */
  #define REG_USBHS_DEVDMAADDRESS5                  (0x40038354U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 5) */
  #define REG_USBHS_DEVDMACONTROL5                  (0x40038358U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 5) */
  #define REG_USBHS_DEVDMASTATUS5                   (0x4003835CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 5) */
  #define REG_USBHS_DEVDMANXTDSC6                   (0x40038360U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 6) */
  #define REG_USBHS_DEVDMAADDRESS6                  (0x40038364U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 6) */
  #define REG_USBHS_DEVDMACONTROL6                  (0x40038368U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 6) */
  #define REG_USBHS_DEVDMASTATUS6                   (0x4003836CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 6) */
  #define REG_USBHS_DEVDMANXTDSC7                   (0x40038370U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 7) */
  #define REG_USBHS_DEVDMAADDRESS7                  (0x40038374U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 7) */
  #define REG_USBHS_DEVDMACONTROL7                  (0x40038378U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 7) */
  #define REG_USBHS_DEVDMASTATUS7                   (0x4003837CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 7) */
  #define REG_USBHS_HSTCTRL                         (0x40038400U) /**< \brief (USBHS) Host General Control Register */
  #define REG_USBHS_HSTISR                          (0x40038404U) /**< \brief (USBHS) Host Global Interrupt Status Register */
  #define REG_USBHS_HSTICR                          (0x40038408U) /**< \brief (USBHS) Host Global Interrupt Clear Register */
  #define REG_USBHS_HSTIFR                          (0x4003840CU) /**< \brief (USBHS) Host Global Interrupt Set Register */
  #define REG_USBHS_HSTIMR                          (0x40038410U) /**< \brief (USBHS) Host Global Interrupt Mask Register */
  #define REG_USBHS_HSTIDR                          (0x40038414U) /**< \brief (USBHS) Host Global Interrupt Disable Register */
  #define REG_USBHS_HSTIER                          (0x40038418U) /**< \brief (USBHS) Host Global Interrupt Enable Register */
  #define REG_USBHS_HSTPIP                          (0x4003841CU) /**< \brief (USBHS) Host Pipe Register */
  #define REG_USBHS_HSTFNUM                         (0x40038420U) /**< \brief (USBHS) Host Frame Number Register */
  #define REG_USBHS_HSTADDR1                        (0x40038424U) /**< \brief (USBHS) Host Address 1 Register */
  #define REG_USBHS_HSTADDR2                        (0x40038428U) /**< \brief (USBHS) Host Address 2 Register */
  #define REG_USBHS_HSTADDR3                        (0x4003842CU) /**< \brief (USBHS) Host Address 3 Register */
  #define REG_USBHS_HSTPIPCFG                       (0x40038500U) /**< \brief (USBHS) Host Pipe Configuration Register (n = 0) */
  #define REG_USBHS_HSTPIPISR                       (0x40038530U) /**< \brief (USBHS) Host Pipe Status Register (n = 0) */
  #define REG_USBHS_HSTPIPICR                       (0x40038560U) /**< \brief (USBHS) Host Pipe Clear Register (n = 0) */
  #define REG_USBHS_HSTPIPIFR                       (0x40038590U) /**< \brief (USBHS) Host Pipe Set Register (n = 0) */
  #define REG_USBHS_HSTPIPIMR                       (0x400385C0U) /**< \brief (USBHS) Host Pipe Mask Register (n = 0) */
  #define REG_USBHS_HSTPIPIER                       (0x400385F0U) /**< \brief (USBHS) Host Pipe Enable Register (n = 0) */
  #define REG_USBHS_HSTPIPIDR                       (0x40038620U) /**< \brief (USBHS) Host Pipe Disable Register (n = 0) */
  #define REG_USBHS_HSTPIPINRQ                      (0x40038650U) /**< \brief (USBHS) Host Pipe IN Request Register (n = 0) */
  #define REG_USBHS_HSTPIPERR                       (0x40038680U) /**< \brief (USBHS) Host Pipe Error Register (n = 0) */
  #define REG_USBHS_HSTDMANXTDSC1                   (0x40038710U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 1) */
  #define REG_USBHS_HSTDMAADDRESS1                  (0x40038714U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 1) */
  #define REG_USBHS_HSTDMACONTROL1                  (0x40038718U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 1) */
  #define REG_USBHS_HSTDMASTATUS1                   (0x4003871CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 1) */
  #define REG_USBHS_HSTDMANXTDSC2                   (0x40038720U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 2) */
  #define REG_USBHS_HSTDMAADDRESS2                  (0x40038724U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 2) */
  #define REG_USBHS_HSTDMACONTROL2                  (0x40038728U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 2) */
  #define REG_USBHS_HSTDMASTATUS2                   (0x4003872CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 2) */
  #define REG_USBHS_HSTDMANXTDSC3                   (0x40038730U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 3) */
  #define REG_USBHS_HSTDMAADDRESS3                  (0x40038734U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 3) */
  #define REG_USBHS_HSTDMACONTROL3                  (0x40038738U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 3) */
  #define REG_USBHS_HSTDMASTATUS3                   (0x4003873CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 3) */
  #define REG_USBHS_HSTDMANXTDSC4                   (0x40038740U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 4) */
  #define REG_USBHS_HSTDMAADDRESS4                  (0x40038744U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 4) */
  #define REG_USBHS_HSTDMACONTROL4                  (0x40038748U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 4) */
  #define REG_USBHS_HSTDMASTATUS4                   (0x4003874CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 4) */
  #define REG_USBHS_HSTDMANXTDSC5                   (0x40038750U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 5) */
  #define REG_USBHS_HSTDMAADDRESS5                  (0x40038754U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 5) */
  #define REG_USBHS_HSTDMACONTROL5                  (0x40038758U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 5) */
  #define REG_USBHS_HSTDMASTATUS5                   (0x4003875CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 5) */
  #define REG_USBHS_HSTDMANXTDSC6                   (0x40038760U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 6) */
  #define REG_USBHS_HSTDMAADDRESS6                  (0x40038764U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 6) */
  #define REG_USBHS_HSTDMACONTROL6                  (0x40038768U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 6) */
  #define REG_USBHS_HSTDMASTATUS6                   (0x4003876CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 6) */
  #define REG_USBHS_HSTDMANXTDSC7                   (0x40038770U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 7) */
  #define REG_USBHS_HSTDMAADDRESS7                  (0x40038774U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 7) */
  #define REG_USBHS_HSTDMACONTROL7                  (0x40038778U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 7) */
  #define REG_USBHS_HSTDMASTATUS7                   (0x4003877CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 7) */
  #define REG_USBHS_CTRL                            (0x40038800U) /**< \brief (USBHS) General Control Register */
  #define REG_USBHS_SR                              (0x40038804U) /**< \brief (USBHS) General Status Register */
  #define REG_USBHS_SCR                             (0x40038808U) /**< \brief (USBHS) General Status Clear Register */
  #define REG_USBHS_SFR                             (0x4003880CU) /**< \brief (USBHS) General Status Set Register */
  #define REG_USBHS_TSTA1                           (0x40038810U) /**< \brief (USBHS) General Test A1 Register */
  #define REG_USBHS_TSTA2                           (0x40038814U) /**< \brief (USBHS) General Test A2 Register */
  #define REG_USBHS_VERSION                         (0x40038818U) /**< \brief (USBHS) General Version Register */
  #define REG_USBHS_FSM                             (0x4003882CU) /**< \brief (USBHS) General Finite State Machine Register */
#else
  #define REG_USBHS_DEVCTRL        (*(__IO uint32_t*)0x40038000U) /**< \brief (USBHS) Device General Control Register */
  #define REG_USBHS_DEVISR         (*(__I  uint32_t*)0x40038004U) /**< \brief (USBHS) Device Global Interrupt Status Register */
  #define REG_USBHS_DEVICR         (*(__O  uint32_t*)0x40038008U) /**< \brief (USBHS) Device Global Interrupt Clear Register */
  #define REG_USBHS_DEVIFR         (*(__O  uint32_t*)0x4003800CU) /**< \brief (USBHS) Device Global Interrupt Set Register */
  #define REG_USBHS_DEVIMR         (*(__I  uint32_t*)0x40038010U) /**< \brief (USBHS) Device Global Interrupt Mask Register */
  #define REG_USBHS_DEVIDR         (*(__O  uint32_t*)0x40038014U) /**< \brief (USBHS) Device Global Interrupt Disable Register */
  #define REG_USBHS_DEVIER         (*(__O  uint32_t*)0x40038018U) /**< \brief (USBHS) Device Global Interrupt Enable Register */
  #define REG_USBHS_DEVEPT         (*(__IO uint32_t*)0x4003801CU) /**< \brief (USBHS) Device Endpoint Register */
  #define REG_USBHS_DEVFNUM        (*(__I  uint32_t*)0x40038020U) /**< \brief (USBHS) Device Frame Number Register */
  #define REG_USBHS_DEVEPTCFG      (*(__IO uint32_t*)0x40038100U) /**< \brief (USBHS) Device Endpoint Configuration Register (n = 0) */
  #define REG_USBHS_DEVEPTISR      (*(__I  uint32_t*)0x40038130U) /**< \brief (USBHS) Device Endpoint Status Register (n = 0) */
  #define REG_USBHS_DEVEPTICR      (*(__O  uint32_t*)0x40038160U) /**< \brief (USBHS) Device Endpoint Clear Register (n = 0) */
  #define REG_USBHS_DEVEPTIFR      (*(__O  uint32_t*)0x40038190U) /**< \brief (USBHS) Device Endpoint Set Register (n = 0) */
  #define REG_USBHS_DEVEPTIMR      (*(__I  uint32_t*)0x400381C0U) /**< \brief (USBHS) Device Endpoint Mask Register (n = 0) */
  #define REG_USBHS_DEVEPTIER      (*(__O  uint32_t*)0x400381F0U) /**< \brief (USBHS) Device Endpoint Enable Register (n = 0) */
  #define REG_USBHS_DEVEPTIDR      (*(__O  uint32_t*)0x40038220U) /**< \brief (USBHS) Device Endpoint Disable Register (n = 0) */
  #define REG_USBHS_DEVDMANXTDSC1  (*(__IO uint32_t*)0x40038310U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 1) */
  #define REG_USBHS_DEVDMAADDRESS1 (*(__IO uint32_t*)0x40038314U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 1) */
  #define REG_USBHS_DEVDMACONTROL1 (*(__IO uint32_t*)0x40038318U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 1) */
  #define REG_USBHS_DEVDMASTATUS1  (*(__IO uint32_t*)0x4003831CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 1) */
  #define REG_USBHS_DEVDMANXTDSC2  (*(__IO uint32_t*)0x40038320U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 2) */
  #define REG_USBHS_DEVDMAADDRESS2 (*(__IO uint32_t*)0x40038324U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 2) */
  #define REG_USBHS_DEVDMACONTROL2 (*(__IO uint32_t*)0x40038328U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 2) */
  #define REG_USBHS_DEVDMASTATUS2  (*(__IO uint32_t*)0x4003832CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 2) */
  #define REG_USBHS_DEVDMANXTDSC3  (*(__IO uint32_t*)0x40038330U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 3) */
  #define REG_USBHS_DEVDMAADDRESS3 (*(__IO uint32_t*)0x40038334U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 3) */
  #define REG_USBHS_DEVDMACONTROL3 (*(__IO uint32_t*)0x40038338U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 3) */
  #define REG_USBHS_DEVDMASTATUS3  (*(__IO uint32_t*)0x4003833CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 3) */
  #define REG_USBHS_DEVDMANXTDSC4  (*(__IO uint32_t*)0x40038340U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 4) */
  #define REG_USBHS_DEVDMAADDRESS4 (*(__IO uint32_t*)0x40038344U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 4) */
  #define REG_USBHS_DEVDMACONTROL4 (*(__IO uint32_t*)0x40038348U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 4) */
  #define REG_USBHS_DEVDMASTATUS4  (*(__IO uint32_t*)0x4003834CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 4) */
  #define REG_USBHS_DEVDMANXTDSC5  (*(__IO uint32_t*)0x40038350U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 5) */
  #define REG_USBHS_DEVDMAADDRESS5 (*(__IO uint32_t*)0x40038354U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 5) */
  #define REG_USBHS_DEVDMACONTROL5 (*(__IO uint32_t*)0x40038358U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 5) */
  #define REG_USBHS_DEVDMASTATUS5  (*(__IO uint32_t*)0x4003835CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 5) */
  #define REG_USBHS_DEVDMANXTDSC6  (*(__IO uint32_t*)0x40038360U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 6) */
  #define REG_USBHS_DEVDMAADDRESS6 (*(__IO uint32_t*)0x40038364U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 6) */
  #define REG_USBHS_DEVDMACONTROL6 (*(__IO uint32_t*)0x40038368U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 6) */
  #define REG_USBHS_DEVDMASTATUS6  (*(__IO uint32_t*)0x4003836CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 6) */
  #define REG_USBHS_DEVDMANXTDSC7  (*(__IO uint32_t*)0x40038370U) /**< \brief (USBHS) Device DMA Channel Next Descriptor Address Register (n = 7) */
  #define REG_USBHS_DEVDMAADDRESS7 (*(__IO uint32_t*)0x40038374U) /**< \brief (USBHS) Device DMA Channel Address Register (n = 7) */
  #define REG_USBHS_DEVDMACONTROL7 (*(__IO uint32_t*)0x40038378U) /**< \brief (USBHS) Device DMA Channel Control Register (n = 7) */
  #define REG_USBHS_DEVDMASTATUS7  (*(__IO uint32_t*)0x4003837CU) /**< \brief (USBHS) Device DMA Channel Status Register (n = 7) */
  #define REG_USBHS_HSTCTRL        (*(__IO uint32_t*)0x40038400U) /**< \brief (USBHS) Host General Control Register */
  #define REG_USBHS_HSTISR         (*(__I  uint32_t*)0x40038404U) /**< \brief (USBHS) Host Global Interrupt Status Register */
  #define REG_USBHS_HSTICR         (*(__O  uint32_t*)0x40038408U) /**< \brief (USBHS) Host Global Interrupt Clear Register */
  #define REG_USBHS_HSTIFR         (*(__O  uint32_t*)0x4003840CU) /**< \brief (USBHS) Host Global Interrupt Set Register */
  #define REG_USBHS_HSTIMR         (*(__I  uint32_t*)0x40038410U) /**< \brief (USBHS) Host Global Interrupt Mask Register */
  #define REG_USBHS_HSTIDR         (*(__O  uint32_t*)0x40038414U) /**< \brief (USBHS) Host Global Interrupt Disable Register */
  #define REG_USBHS_HSTIER         (*(__O  uint32_t*)0x40038418U) /**< \brief (USBHS) Host Global Interrupt Enable Register */
  #define REG_USBHS_HSTPIP         (*(__IO uint32_t*)0x4003841CU) /**< \brief (USBHS) Host Pipe Register */
  #define REG_USBHS_HSTFNUM        (*(__IO uint32_t*)0x40038420U) /**< \brief (USBHS) Host Frame Number Register */
  #define REG_USBHS_HSTADDR1       (*(__IO uint32_t*)0x40038424U) /**< \brief (USBHS) Host Address 1 Register */
  #define REG_USBHS_HSTADDR2       (*(__IO uint32_t*)0x40038428U) /**< \brief (USBHS) Host Address 2 Register */
  #define REG_USBHS_HSTADDR3       (*(__IO uint32_t*)0x4003842CU) /**< \brief (USBHS) Host Address 3 Register */
  #define REG_USBHS_HSTPIPCFG      (*(__IO uint32_t*)0x40038500U) /**< \brief (USBHS) Host Pipe Configuration Register (n = 0) */
  #define REG_USBHS_HSTPIPISR      (*(__I  uint32_t*)0x40038530U) /**< \brief (USBHS) Host Pipe Status Register (n = 0) */
  #define REG_USBHS_HSTPIPICR      (*(__O  uint32_t*)0x40038560U) /**< \brief (USBHS) Host Pipe Clear Register (n = 0) */
  #define REG_USBHS_HSTPIPIFR      (*(__O  uint32_t*)0x40038590U) /**< \brief (USBHS) Host Pipe Set Register (n = 0) */
  #define REG_USBHS_HSTPIPIMR      (*(__I  uint32_t*)0x400385C0U) /**< \brief (USBHS) Host Pipe Mask Register (n = 0) */
  #define REG_USBHS_HSTPIPIER      (*(__O  uint32_t*)0x400385F0U) /**< \brief (USBHS) Host Pipe Enable Register (n = 0) */
  #define REG_USBHS_HSTPIPIDR      (*(__O  uint32_t*)0x40038620U) /**< \brief (USBHS) Host Pipe Disable Register (n = 0) */
  #define REG_USBHS_HSTPIPINRQ     (*(__IO uint32_t*)0x40038650U) /**< \brief (USBHS) Host Pipe IN Request Register (n = 0) */
  #define REG_USBHS_HSTPIPERR      (*(__IO uint32_t*)0x40038680U) /**< \brief (USBHS) Host Pipe Error Register (n = 0) */
  #define REG_USBHS_HSTDMANXTDSC1  (*(__IO uint32_t*)0x40038710U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 1) */
  #define REG_USBHS_HSTDMAADDRESS1 (*(__IO uint32_t*)0x40038714U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 1) */
  #define REG_USBHS_HSTDMACONTROL1 (*(__IO uint32_t*)0x40038718U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 1) */
  #define REG_USBHS_HSTDMASTATUS1  (*(__IO uint32_t*)0x4003871CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 1) */
  #define REG_USBHS_HSTDMANXTDSC2  (*(__IO uint32_t*)0x40038720U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 2) */
  #define REG_USBHS_HSTDMAADDRESS2 (*(__IO uint32_t*)0x40038724U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 2) */
  #define REG_USBHS_HSTDMACONTROL2 (*(__IO uint32_t*)0x40038728U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 2) */
  #define REG_USBHS_HSTDMASTATUS2  (*(__IO uint32_t*)0x4003872CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 2) */
  #define REG_USBHS_HSTDMANXTDSC3  (*(__IO uint32_t*)0x40038730U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 3) */
  #define REG_USBHS_HSTDMAADDRESS3 (*(__IO uint32_t*)0x40038734U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 3) */
  #define REG_USBHS_HSTDMACONTROL3 (*(__IO uint32_t*)0x40038738U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 3) */
  #define REG_USBHS_HSTDMASTATUS3  (*(__IO uint32_t*)0x4003873CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 3) */
  #define REG_USBHS_HSTDMANXTDSC4  (*(__IO uint32_t*)0x40038740U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 4) */
  #define REG_USBHS_HSTDMAADDRESS4 (*(__IO uint32_t*)0x40038744U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 4) */
  #define REG_USBHS_HSTDMACONTROL4 (*(__IO uint32_t*)0x40038748U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 4) */
  #define REG_USBHS_HSTDMASTATUS4  (*(__IO uint32_t*)0x4003874CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 4) */
  #define REG_USBHS_HSTDMANXTDSC5  (*(__IO uint32_t*)0x40038750U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 5) */
  #define REG_USBHS_HSTDMAADDRESS5 (*(__IO uint32_t*)0x40038754U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 5) */
  #define REG_USBHS_HSTDMACONTROL5 (*(__IO uint32_t*)0x40038758U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 5) */
  #define REG_USBHS_HSTDMASTATUS5  (*(__IO uint32_t*)0x4003875CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 5) */
  #define REG_USBHS_HSTDMANXTDSC6  (*(__IO uint32_t*)0x40038760U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 6) */
  #define REG_USBHS_HSTDMAADDRESS6 (*(__IO uint32_t*)0x40038764U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 6) */
  #define REG_USBHS_HSTDMACONTROL6 (*(__IO uint32_t*)0x40038768U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 6) */
  #define REG_USBHS_HSTDMASTATUS6  (*(__IO uint32_t*)0x4003876CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 6) */
  #define REG_USBHS_HSTDMANXTDSC7  (*(__IO uint32_t*)0x40038770U) /**< \brief (USBHS) Host DMA Channel Next Descriptor Address Register (n = 7) */
  #define REG_USBHS_HSTDMAADDRESS7 (*(__IO uint32_t*)0x40038774U) /**< \brief (USBHS) Host DMA Channel Address Register (n = 7) */
  #define REG_USBHS_HSTDMACONTROL7 (*(__IO uint32_t*)0x40038778U) /**< \brief (USBHS) Host DMA Channel Control Register (n = 7) */
  #define REG_USBHS_HSTDMASTATUS7  (*(__IO uint32_t*)0x4003877CU) /**< \brief (USBHS) Host DMA Channel Status Register (n = 7) */
  #define REG_USBHS_CTRL           (*(__IO uint32_t*)0x40038800U) /**< \brief (USBHS) General Control Register */
  #define REG_USBHS_SR             (*(__I  uint32_t*)0x40038804U) /**< \brief (USBHS) General Status Register */
  #define REG_USBHS_SCR            (*(__O  uint32_t*)0x40038808U) /**< \brief (USBHS) General Status Clear Register */
  #define REG_USBHS_SFR            (*(__O  uint32_t*)0x4003880CU) /**< \brief (USBHS) General Status Set Register */
  #define REG_USBHS_TSTA1          (*(__IO uint32_t*)0x40038810U) /**< \brief (USBHS) General Test A1 Register */
  #define REG_USBHS_TSTA2          (*(__IO uint32_t*)0x40038814U) /**< \brief (USBHS) General Test A2 Register */
  #define REG_USBHS_VERSION        (*(__I  uint32_t*)0x40038818U) /**< \brief (USBHS) General Version Register */
  #define REG_USBHS_FSM            (*(__I  uint32_t*)0x4003882CU) /**< \brief (USBHS) General Finite State Machine Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_USBHS_INSTANCE_ */
