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

#ifndef _SAME70_ISI_COMPONENT_
#define _SAME70_ISI_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Image Sensor Interface */
/* ============================================================================= */
/** \addtogroup SAME70_ISI Image Sensor Interface */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Isi hardware registers */
typedef struct {
  __IO uint32_t ISI_CFG1;       /**< \brief (Isi Offset: 0x00) ISI Configuration 1 Register */
  __IO uint32_t ISI_CFG2;       /**< \brief (Isi Offset: 0x04) ISI Configuration 2 Register */
  __IO uint32_t ISI_PSIZE;      /**< \brief (Isi Offset: 0x08) ISI Preview Size Register */
  __IO uint32_t ISI_PDECF;      /**< \brief (Isi Offset: 0x0C) ISI Preview Decimation Factor Register */
  __IO uint32_t ISI_Y2R_SET0;   /**< \brief (Isi Offset: 0x10) ISI Color Space Conversion YCrCb To RGB Set 0 Register */
  __IO uint32_t ISI_Y2R_SET1;   /**< \brief (Isi Offset: 0x14) ISI Color Space Conversion YCrCb To RGB Set 1 Register */
  __IO uint32_t ISI_R2Y_SET0;   /**< \brief (Isi Offset: 0x18) ISI Color Space Conversion RGB To YCrCb Set 0 Register */
  __IO uint32_t ISI_R2Y_SET1;   /**< \brief (Isi Offset: 0x1C) ISI Color Space Conversion RGB To YCrCb Set 1 Register */
  __IO uint32_t ISI_R2Y_SET2;   /**< \brief (Isi Offset: 0x20) ISI Color Space Conversion RGB To YCrCb Set 2 Register */
  __O  uint32_t ISI_CR;         /**< \brief (Isi Offset: 0x24) ISI Control Register */
  __I  uint32_t ISI_SR;         /**< \brief (Isi Offset: 0x28) ISI Status Register */
  __O  uint32_t ISI_IER;        /**< \brief (Isi Offset: 0x2C) ISI Interrupt Enable Register */
  __O  uint32_t ISI_IDR;        /**< \brief (Isi Offset: 0x30) ISI Interrupt Disable Register */
  __I  uint32_t ISI_IMR;        /**< \brief (Isi Offset: 0x34) ISI Interrupt Mask Register */
  __O  uint32_t ISI_DMA_CHER;   /**< \brief (Isi Offset: 0x38) DMA Channel Enable Register */
  __O  uint32_t ISI_DMA_CHDR;   /**< \brief (Isi Offset: 0x3C) DMA Channel Disable Register */
  __I  uint32_t ISI_DMA_CHSR;   /**< \brief (Isi Offset: 0x40) DMA Channel Status Register */
  __IO uint32_t ISI_DMA_P_ADDR; /**< \brief (Isi Offset: 0x44) DMA Preview Base Address Register */
  __IO uint32_t ISI_DMA_P_CTRL; /**< \brief (Isi Offset: 0x48) DMA Preview Control Register */
  __IO uint32_t ISI_DMA_P_DSCR; /**< \brief (Isi Offset: 0x4C) DMA Preview Descriptor Address Register */
  __IO uint32_t ISI_DMA_C_ADDR; /**< \brief (Isi Offset: 0x50) DMA Codec Base Address Register */
  __IO uint32_t ISI_DMA_C_CTRL; /**< \brief (Isi Offset: 0x54) DMA Codec Control Register */
  __IO uint32_t ISI_DMA_C_DSCR; /**< \brief (Isi Offset: 0x58) DMA Codec Descriptor Address Register */
  __I  uint32_t Reserved1[34];
  __IO uint32_t ISI_WPMR;       /**< \brief (Isi Offset: 0xE4) Write Protection Mode Register */
  __I  uint32_t ISI_WPSR;       /**< \brief (Isi Offset: 0xE8) Write Protection Status Register */
  __I  uint32_t Reserved2[4];
  __I  uint32_t ISI_VERSION;    /**< \brief (Isi Offset: 0xFC) Version Register */
} Isi;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- ISI_CFG1 : (ISI Offset: 0x00) ISI Configuration 1 Register -------- */
#define ISI_CFG1_HSYNC_POL (0x1u << 2) /**< \brief (ISI_CFG1) Horizontal Synchronization Polarity */
#define ISI_CFG1_VSYNC_POL (0x1u << 3) /**< \brief (ISI_CFG1) Vertical Synchronization Polarity */
#define ISI_CFG1_PIXCLK_POL (0x1u << 4) /**< \brief (ISI_CFG1) Pixel Clock Polarity */
#define ISI_CFG1_GRAYLE    (0x1u << 5) /**< \brief (ISI_CFG1) Grayscale Little Endian */
#define ISI_CFG1_EMB_SYNC (0x1u << 6) /**< \brief (ISI_CFG1) Embedded Synchronization */
#define ISI_CFG1_CRC_SYNC (0x1u << 7) /**< \brief (ISI_CFG1) Embedded Synchronization Correction */
#define ISI_CFG1_FRATE_Pos 8
#define ISI_CFG1_FRATE_Msk (0x7u << ISI_CFG1_FRATE_Pos) /**< \brief (ISI_CFG1) Frame Rate [0..7] */
#define ISI_CFG1_FRATE(value) ((ISI_CFG1_FRATE_Msk & ((value) << ISI_CFG1_FRATE_Pos)))
#define ISI_CFG1_DISCR (0x1u << 11) /**< \brief (ISI_CFG1) Disable Codec Request */
#define ISI_CFG1_FULL (0x1u << 12) /**< \brief (ISI_CFG1) Full Mode is Allowed */
#define ISI_CFG1_THMASK_Pos 13
#define ISI_CFG1_THMASK_Msk (0x3u << ISI_CFG1_THMASK_Pos) /**< \brief (ISI_CFG1) Threshold Mask */
#define ISI_CFG1_THMASK(value) ((ISI_CFG1_THMASK_Msk & ((value) << ISI_CFG1_THMASK_Pos)))
#define   ISI_CFG1_THMASK_BEATS_4 (0x0u << 13) /**< \brief (ISI_CFG1) Only 4 beats AHB burst allowed */
#define   ISI_CFG1_THMASK_BEATS_8 (0x1u << 13) /**< \brief (ISI_CFG1) Only 4 and 8 beats AHB burst allowed */
#define   ISI_CFG1_THMASK_BEATS_16 (0x2u << 13) /**< \brief (ISI_CFG1) 4, 8 and 16 beats AHB burst allowed */
#define ISI_CFG1_SLD_Pos 16
#define ISI_CFG1_SLD_Msk (0xffu << ISI_CFG1_SLD_Pos) /**< \brief (ISI_CFG1) Start of Line Delay */
#define ISI_CFG1_SLD(value) ((ISI_CFG1_SLD_Msk & ((value) << ISI_CFG1_SLD_Pos)))
#define ISI_CFG1_SFD_Pos 24
#define ISI_CFG1_SFD_Msk (0xffu << ISI_CFG1_SFD_Pos) /**< \brief (ISI_CFG1) Start of Frame Delay */
#define ISI_CFG1_SFD(value) ((ISI_CFG1_SFD_Msk & ((value) << ISI_CFG1_SFD_Pos)))
/* -------- ISI_CFG2 : (ISI Offset: 0x04) ISI Configuration 2 Register -------- */
#define ISI_CFG2_IM_VSIZE_Pos 0
#define ISI_CFG2_IM_VSIZE_Msk (0x7ffu << ISI_CFG2_IM_VSIZE_Pos) /**< \brief (ISI_CFG2) Vertical Size of the Image Sensor [0..2047] */
#define ISI_CFG2_IM_VSIZE(value) ((ISI_CFG2_IM_VSIZE_Msk & ((value) << ISI_CFG2_IM_VSIZE_Pos)))
#define ISI_CFG2_GS_MODE (0x1u << 11) /**< \brief (ISI_CFG2) Grayscale Pixel Format Mode */
#define ISI_CFG2_RGB_MODE (0x1u << 12) /**< \brief (ISI_CFG2) RGB Input Mode */
#define ISI_CFG2_GRAYSCALE (0x1u << 13) /**< \brief (ISI_CFG2) Grayscale Mode Format Enable */
#define ISI_CFG2_RGB_SWAP (0x1u << 14) /**< \brief (ISI_CFG2) RGB Format Swap Mode */
#define ISI_CFG2_COL_SPACE (0x1u << 15) /**< \brief (ISI_CFG2) Color Space for the Image Data */
#define ISI_CFG2_IM_HSIZE_Pos 16
#define ISI_CFG2_IM_HSIZE_Msk (0x7ffu << ISI_CFG2_IM_HSIZE_Pos) /**< \brief (ISI_CFG2) Horizontal Size of the Image Sensor [0..2047] */
#define ISI_CFG2_IM_HSIZE(value) ((ISI_CFG2_IM_HSIZE_Msk & ((value) << ISI_CFG2_IM_HSIZE_Pos)))
#define ISI_CFG2_YCC_SWAP_Pos 28
#define ISI_CFG2_YCC_SWAP_Msk (0x3u << ISI_CFG2_YCC_SWAP_Pos) /**< \brief (ISI_CFG2) YCrCb Format Swap Mode */
#define ISI_CFG2_YCC_SWAP(value) ((ISI_CFG2_YCC_SWAP_Msk & ((value) << ISI_CFG2_YCC_SWAP_Pos)))
#define   ISI_CFG2_YCC_SWAP_DEFAULT (0x0u << 28) /**< \brief (ISI_CFG2) Byte 0 Cb(i)Byte 1 Y(i)Byte 2 Cr(i)Byte 3 Y(i+1) */
#define   ISI_CFG2_YCC_SWAP_MODE1 (0x1u << 28) /**< \brief (ISI_CFG2) Byte 0 Cr(i)Byte 1 Y(i)Byte 2 Cb(i)Byte 3 Y(i+1) */
#define   ISI_CFG2_YCC_SWAP_MODE2 (0x2u << 28) /**< \brief (ISI_CFG2) Byte 0 Y(i)Byte 1 Cb(i)Byte 2 Y(i+1)Byte 3 Cr(i) */
#define   ISI_CFG2_YCC_SWAP_MODE3 (0x3u << 28) /**< \brief (ISI_CFG2) Byte 0 Y(i)Byte 1 Cr(i)Byte 2 Y(i+1)Byte 3 Cb(i) */
#define ISI_CFG2_RGB_CFG_Pos 30
#define ISI_CFG2_RGB_CFG_Msk (0x3u << ISI_CFG2_RGB_CFG_Pos) /**< \brief (ISI_CFG2) RGB Pixel Mapping Configuration */
#define ISI_CFG2_RGB_CFG(value) ((ISI_CFG2_RGB_CFG_Msk & ((value) << ISI_CFG2_RGB_CFG_Pos)))
#define   ISI_CFG2_RGB_CFG_DEFAULT (0x0u << 30) /**< \brief (ISI_CFG2) Byte 0 R/G(MSB)Byte 1 G(LSB)/BByte 2 R/G(MSB)Byte 3 G(LSB)/B */
#define   ISI_CFG2_RGB_CFG_MODE1 (0x1u << 30) /**< \brief (ISI_CFG2) Byte 0 B/G(MSB)Byte 1 G(LSB)/RByte 2 B/G(MSB)Byte 3 G(LSB)/R */
#define   ISI_CFG2_RGB_CFG_MODE2 (0x2u << 30) /**< \brief (ISI_CFG2) Byte 0 G(LSB)/RByte 1 B/G(MSB)Byte 2 G(LSB)/RByte 3 B/G(MSB) */
#define   ISI_CFG2_RGB_CFG_MODE3 (0x3u << 30) /**< \brief (ISI_CFG2) Byte 0 G(LSB)/BByte 1 R/G(MSB)Byte 2 G(LSB)/BByte 3 R/G(MSB) */
/* -------- ISI_PSIZE : (ISI Offset: 0x08) ISI Preview Size Register -------- */
#define ISI_PSIZE_PREV_VSIZE_Pos 0
#define ISI_PSIZE_PREV_VSIZE_Msk (0x3ffu << ISI_PSIZE_PREV_VSIZE_Pos) /**< \brief (ISI_PSIZE) Vertical Size for the Preview Path */
#define ISI_PSIZE_PREV_VSIZE(value) ((ISI_PSIZE_PREV_VSIZE_Msk & ((value) << ISI_PSIZE_PREV_VSIZE_Pos)))
#define ISI_PSIZE_PREV_HSIZE_Pos 16
#define ISI_PSIZE_PREV_HSIZE_Msk (0x3ffu << ISI_PSIZE_PREV_HSIZE_Pos) /**< \brief (ISI_PSIZE) Horizontal Size for the Preview Path */
#define ISI_PSIZE_PREV_HSIZE(value) ((ISI_PSIZE_PREV_HSIZE_Msk & ((value) << ISI_PSIZE_PREV_HSIZE_Pos)))
/* -------- ISI_PDECF : (ISI Offset: 0x0C) ISI Preview Decimation Factor Register -------- */
#define ISI_PDECF_DEC_FACTOR_Pos 0
#define ISI_PDECF_DEC_FACTOR_Msk (0xffu << ISI_PDECF_DEC_FACTOR_Pos) /**< \brief (ISI_PDECF) Decimation Factor */
#define ISI_PDECF_DEC_FACTOR(value) ((ISI_PDECF_DEC_FACTOR_Msk & ((value) << ISI_PDECF_DEC_FACTOR_Pos)))
/* -------- ISI_Y2R_SET0 : (ISI Offset: 0x10) ISI Color Space Conversion YCrCb To RGB Set 0 Register -------- */
#define ISI_Y2R_SET0_C0_Pos 0
#define ISI_Y2R_SET0_C0_Msk (0xffu << ISI_Y2R_SET0_C0_Pos) /**< \brief (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C0 */
#define ISI_Y2R_SET0_C0(value) ((ISI_Y2R_SET0_C0_Msk & ((value) << ISI_Y2R_SET0_C0_Pos)))
#define ISI_Y2R_SET0_C1_Pos 8
#define ISI_Y2R_SET0_C1_Msk (0xffu << ISI_Y2R_SET0_C1_Pos) /**< \brief (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C1 */
#define ISI_Y2R_SET0_C1(value) ((ISI_Y2R_SET0_C1_Msk & ((value) << ISI_Y2R_SET0_C1_Pos)))
#define ISI_Y2R_SET0_C2_Pos 16
#define ISI_Y2R_SET0_C2_Msk (0xffu << ISI_Y2R_SET0_C2_Pos) /**< \brief (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C2 */
#define ISI_Y2R_SET0_C2(value) ((ISI_Y2R_SET0_C2_Msk & ((value) << ISI_Y2R_SET0_C2_Pos)))
#define ISI_Y2R_SET0_C3_Pos 24
#define ISI_Y2R_SET0_C3_Msk (0xffu << ISI_Y2R_SET0_C3_Pos) /**< \brief (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C3 */
#define ISI_Y2R_SET0_C3(value) ((ISI_Y2R_SET0_C3_Msk & ((value) << ISI_Y2R_SET0_C3_Pos)))
/* -------- ISI_Y2R_SET1 : (ISI Offset: 0x14) ISI Color Space Conversion YCrCb To RGB Set 1 Register -------- */
#define ISI_Y2R_SET1_C4_Pos 0
#define ISI_Y2R_SET1_C4_Msk (0x1ffu << ISI_Y2R_SET1_C4_Pos) /**< \brief (ISI_Y2R_SET1) Color Space Conversion Matrix Coefficient C4 */
#define ISI_Y2R_SET1_C4(value) ((ISI_Y2R_SET1_C4_Msk & ((value) << ISI_Y2R_SET1_C4_Pos)))
#define ISI_Y2R_SET1_Yoff (0x1u << 12) /**< \brief (ISI_Y2R_SET1) Color Space Conversion Luminance Default Offset */
#define ISI_Y2R_SET1_Croff (0x1u << 13) /**< \brief (ISI_Y2R_SET1) Color Space Conversion Red Chrominance Default Offset */
#define ISI_Y2R_SET1_Cboff (0x1u << 14) /**< \brief (ISI_Y2R_SET1) Color Space Conversion Blue Chrominance Default Offset */
/* -------- ISI_R2Y_SET0 : (ISI Offset: 0x18) ISI Color Space Conversion RGB To YCrCb Set 0 Register -------- */
#define ISI_R2Y_SET0_C0_Pos 0
#define ISI_R2Y_SET0_C0_Msk (0x7fu << ISI_R2Y_SET0_C0_Pos) /**< \brief (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C0 */
#define ISI_R2Y_SET0_C0(value) ((ISI_R2Y_SET0_C0_Msk & ((value) << ISI_R2Y_SET0_C0_Pos)))
#define ISI_R2Y_SET0_C1_Pos 8
#define ISI_R2Y_SET0_C1_Msk (0x7fu << ISI_R2Y_SET0_C1_Pos) /**< \brief (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C1 */
#define ISI_R2Y_SET0_C1(value) ((ISI_R2Y_SET0_C1_Msk & ((value) << ISI_R2Y_SET0_C1_Pos)))
#define ISI_R2Y_SET0_C2_Pos 16
#define ISI_R2Y_SET0_C2_Msk (0x7fu << ISI_R2Y_SET0_C2_Pos) /**< \brief (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C2 */
#define ISI_R2Y_SET0_C2(value) ((ISI_R2Y_SET0_C2_Msk & ((value) << ISI_R2Y_SET0_C2_Pos)))
#define ISI_R2Y_SET0_Roff (0x1u << 24) /**< \brief (ISI_R2Y_SET0) Color Space Conversion Red Component Offset */
/* -------- ISI_R2Y_SET1 : (ISI Offset: 0x1C) ISI Color Space Conversion RGB To YCrCb Set 1 Register -------- */
#define ISI_R2Y_SET1_C3_Pos 0
#define ISI_R2Y_SET1_C3_Msk (0x7fu << ISI_R2Y_SET1_C3_Pos) /**< \brief (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C3 */
#define ISI_R2Y_SET1_C3(value) ((ISI_R2Y_SET1_C3_Msk & ((value) << ISI_R2Y_SET1_C3_Pos)))
#define ISI_R2Y_SET1_C4_Pos 8
#define ISI_R2Y_SET1_C4_Msk (0x7fu << ISI_R2Y_SET1_C4_Pos) /**< \brief (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C4 */
#define ISI_R2Y_SET1_C4(value) ((ISI_R2Y_SET1_C4_Msk & ((value) << ISI_R2Y_SET1_C4_Pos)))
#define ISI_R2Y_SET1_C5_Pos 16
#define ISI_R2Y_SET1_C5_Msk (0x7fu << ISI_R2Y_SET1_C5_Pos) /**< \brief (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C5 */
#define ISI_R2Y_SET1_C5(value) ((ISI_R2Y_SET1_C5_Msk & ((value) << ISI_R2Y_SET1_C5_Pos)))
#define ISI_R2Y_SET1_Goff (0x1u << 24) /**< \brief (ISI_R2Y_SET1) Color Space Conversion Green Component Offset */
/* -------- ISI_R2Y_SET2 : (ISI Offset: 0x20) ISI Color Space Conversion RGB To YCrCb Set 2 Register -------- */
#define ISI_R2Y_SET2_C6_Pos 0
#define ISI_R2Y_SET2_C6_Msk (0x7fu << ISI_R2Y_SET2_C6_Pos) /**< \brief (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C6 */
#define ISI_R2Y_SET2_C6(value) ((ISI_R2Y_SET2_C6_Msk & ((value) << ISI_R2Y_SET2_C6_Pos)))
#define ISI_R2Y_SET2_C7_Pos 8
#define ISI_R2Y_SET2_C7_Msk (0x7fu << ISI_R2Y_SET2_C7_Pos) /**< \brief (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C7 */
#define ISI_R2Y_SET2_C7(value) ((ISI_R2Y_SET2_C7_Msk & ((value) << ISI_R2Y_SET2_C7_Pos)))
#define ISI_R2Y_SET2_C8_Pos 16
#define ISI_R2Y_SET2_C8_Msk (0x7fu << ISI_R2Y_SET2_C8_Pos) /**< \brief (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C8 */
#define ISI_R2Y_SET2_C8(value) ((ISI_R2Y_SET2_C8_Msk & ((value) << ISI_R2Y_SET2_C8_Pos)))
#define ISI_R2Y_SET2_Boff (0x1u << 24) /**< \brief (ISI_R2Y_SET2) Color Space Conversion Blue Component Offset */
/* -------- ISI_CR : (ISI Offset: 0x24) ISI Control Register -------- */
#define ISI_CR_ISI_EN (0x1u << 0) /**< \brief (ISI_CR) ISI Module Enable Request */
#define ISI_CR_ISI_DIS (0x1u << 1) /**< \brief (ISI_CR) ISI Module Disable Request */
#define ISI_CR_ISI_SRST (0x1u << 2) /**< \brief (ISI_CR) ISI Software Reset Request */
#define ISI_CR_ISI_CDC (0x1u << 8) /**< \brief (ISI_CR) ISI Codec Request */
/* -------- ISI_SR : (ISI Offset: 0x28) ISI Status Register -------- */
#define ISI_SR_ENABLE (0x1u << 0) /**< \brief (ISI_SR) Module Enable */
#define ISI_SR_DIS_DONE (0x1u << 1) /**< \brief (ISI_SR) Module Disable Request has Terminated (cleared on read) */
#define ISI_SR_SRST (0x1u << 2) /**< \brief (ISI_SR) Module Software Reset Request has Terminated (cleared on read) */
#define ISI_SR_CDC_PND (0x1u << 8) /**< \brief (ISI_SR) Pending Codec Request */
#define ISI_SR_VSYNC (0x1u << 10) /**< \brief (ISI_SR) Vertical Synchronization (cleared on read) */
#define ISI_SR_PXFR_DONE (0x1u << 16) /**< \brief (ISI_SR) Preview DMA Transfer has Terminated (cleared on read) */
#define ISI_SR_CXFR_DONE (0x1u << 17) /**< \brief (ISI_SR) Codec DMA Transfer has Terminated (cleared on read) */
#define ISI_SR_SIP (0x1u << 19) /**< \brief (ISI_SR) Synchronization in Progress */
#define ISI_SR_P_OVR (0x1u << 24) /**< \brief (ISI_SR) Preview Datapath Overflow (cleared on read) */
#define ISI_SR_C_OVR (0x1u << 25) /**< \brief (ISI_SR) Codec Datapath Overflow (cleared on read) */
#define ISI_SR_CRC_ERR (0x1u << 26) /**< \brief (ISI_SR) CRC Synchronization Error (cleared on read) */
#define ISI_SR_FR_OVR (0x1u << 27) /**< \brief (ISI_SR) Frame Rate Overrun (cleared on read) */
/* -------- ISI_IER : (ISI Offset: 0x2C) ISI Interrupt Enable Register -------- */
#define ISI_IER_DIS_DONE (0x1u << 1) /**< \brief (ISI_IER) Disable Done Interrupt Enable */
#define ISI_IER_SRST (0x1u << 2) /**< \brief (ISI_IER) Software Reset Interrupt Enable */
#define ISI_IER_VSYNC (0x1u << 10) /**< \brief (ISI_IER) Vertical Synchronization Interrupt Enable */
#define ISI_IER_PXFR_DONE (0x1u << 16) /**< \brief (ISI_IER) Preview DMA Transfer Done Interrupt Enable */
#define ISI_IER_CXFR_DONE (0x1u << 17) /**< \brief (ISI_IER) Codec DMA Transfer Done Interrupt Enable */
#define ISI_IER_P_OVR (0x1u << 24) /**< \brief (ISI_IER) Preview Datapath Overflow Interrupt Enable */
#define ISI_IER_C_OVR (0x1u << 25) /**< \brief (ISI_IER) Codec Datapath Overflow Interrupt Enable */
#define ISI_IER_CRC_ERR (0x1u << 26) /**< \brief (ISI_IER) Embedded Synchronization CRC Error Interrupt Enable */
#define ISI_IER_FR_OVR (0x1u << 27) /**< \brief (ISI_IER) Frame Rate Overflow Interrupt Enable */
/* -------- ISI_IDR : (ISI Offset: 0x30) ISI Interrupt Disable Register -------- */
#define ISI_IDR_DIS_DONE (0x1u << 1) /**< \brief (ISI_IDR) Disable Done Interrupt Disable */
#define ISI_IDR_SRST (0x1u << 2) /**< \brief (ISI_IDR) Software Reset Interrupt Disable */
#define ISI_IDR_VSYNC (0x1u << 10) /**< \brief (ISI_IDR) Vertical Synchronization Interrupt Disable */
#define ISI_IDR_PXFR_DONE (0x1u << 16) /**< \brief (ISI_IDR) Preview DMA Transfer Done Interrupt Disable */
#define ISI_IDR_CXFR_DONE (0x1u << 17) /**< \brief (ISI_IDR) Codec DMA Transfer Done Interrupt Disable */
#define ISI_IDR_P_OVR (0x1u << 24) /**< \brief (ISI_IDR) Preview Datapath Overflow Interrupt Disable */
#define ISI_IDR_C_OVR (0x1u << 25) /**< \brief (ISI_IDR) Codec Datapath Overflow Interrupt Disable */
#define ISI_IDR_CRC_ERR (0x1u << 26) /**< \brief (ISI_IDR) Embedded Synchronization CRC Error Interrupt Disable */
#define ISI_IDR_FR_OVR (0x1u << 27) /**< \brief (ISI_IDR) Frame Rate Overflow Interrupt Disable */
/* -------- ISI_IMR : (ISI Offset: 0x34) ISI Interrupt Mask Register -------- */
#define ISI_IMR_DIS_DONE (0x1u << 1) /**< \brief (ISI_IMR) Module Disable Operation Completed */
#define ISI_IMR_SRST (0x1u << 2) /**< \brief (ISI_IMR) Software Reset Completed */
#define ISI_IMR_VSYNC (0x1u << 10) /**< \brief (ISI_IMR) Vertical Synchronization */
#define ISI_IMR_PXFR_DONE (0x1u << 16) /**< \brief (ISI_IMR) Preview DMA Transfer Completed */
#define ISI_IMR_CXFR_DONE (0x1u << 17) /**< \brief (ISI_IMR) Codec DMA Transfer Completed */
#define ISI_IMR_P_OVR (0x1u << 24) /**< \brief (ISI_IMR) Preview FIFO Overflow */
#define ISI_IMR_C_OVR (0x1u << 25) /**< \brief (ISI_IMR) Codec FIFO Overflow */
#define ISI_IMR_CRC_ERR (0x1u << 26) /**< \brief (ISI_IMR) CRC Synchronization Error */
#define ISI_IMR_FR_OVR (0x1u << 27) /**< \brief (ISI_IMR) Frame Rate Overrun */
/* -------- ISI_DMA_CHER : (ISI Offset: 0x38) DMA Channel Enable Register -------- */
#define ISI_DMA_CHER_P_CH_EN (0x1u << 0) /**< \brief (ISI_DMA_CHER) Preview Channel Enable */
#define ISI_DMA_CHER_C_CH_EN (0x1u << 1) /**< \brief (ISI_DMA_CHER) Codec Channel Enable */
/* -------- ISI_DMA_CHDR : (ISI Offset: 0x3C) DMA Channel Disable Register -------- */
#define ISI_DMA_CHDR_P_CH_DIS (0x1u << 0) /**< \brief (ISI_DMA_CHDR) Preview Channel Disable Request */
#define ISI_DMA_CHDR_C_CH_DIS (0x1u << 1) /**< \brief (ISI_DMA_CHDR) Codec Channel Disable Request */
/* -------- ISI_DMA_CHSR : (ISI Offset: 0x40) DMA Channel Status Register -------- */
#define ISI_DMA_CHSR_P_CH_S (0x1u << 0) /**< \brief (ISI_DMA_CHSR) Preview DMA Channel Status */
#define ISI_DMA_CHSR_C_CH_S (0x1u << 1) /**< \brief (ISI_DMA_CHSR) Code DMA Channel Status */
/* -------- ISI_DMA_P_ADDR : (ISI Offset: 0x44) DMA Preview Base Address Register -------- */
#define ISI_DMA_P_ADDR_P_ADDR_Pos 2
#define ISI_DMA_P_ADDR_P_ADDR_Msk (0x3fffffffu << ISI_DMA_P_ADDR_P_ADDR_Pos) /**< \brief (ISI_DMA_P_ADDR) Preview Image Base Address */
#define ISI_DMA_P_ADDR_P_ADDR(value) ((ISI_DMA_P_ADDR_P_ADDR_Msk & ((value) << ISI_DMA_P_ADDR_P_ADDR_Pos)))
/* -------- ISI_DMA_P_CTRL : (ISI Offset: 0x48) DMA Preview Control Register -------- */
#define ISI_DMA_P_CTRL_P_FETCH (0x1u << 0) /**< \brief (ISI_DMA_P_CTRL) Descriptor Fetch Control Bit */
#define ISI_DMA_P_CTRL_P_WB (0x1u << 1) /**< \brief (ISI_DMA_P_CTRL) Descriptor Writeback Control Bit */
#define ISI_DMA_P_CTRL_P_IEN (0x1u << 2) /**< \brief (ISI_DMA_P_CTRL) Transfer Done Flag Control */
#define ISI_DMA_P_CTRL_P_DONE (0x1u << 3) /**< \brief (ISI_DMA_P_CTRL) Preview Transfer Done */
/* -------- ISI_DMA_P_DSCR : (ISI Offset: 0x4C) DMA Preview Descriptor Address Register -------- */
#define ISI_DMA_P_DSCR_P_DSCR_Pos 2
#define ISI_DMA_P_DSCR_P_DSCR_Msk (0x3fffffffu << ISI_DMA_P_DSCR_P_DSCR_Pos) /**< \brief (ISI_DMA_P_DSCR) Preview Descriptor Base Address */
#define ISI_DMA_P_DSCR_P_DSCR(value) ((ISI_DMA_P_DSCR_P_DSCR_Msk & ((value) << ISI_DMA_P_DSCR_P_DSCR_Pos)))
/* -------- ISI_DMA_C_ADDR : (ISI Offset: 0x50) DMA Codec Base Address Register -------- */
#define ISI_DMA_C_ADDR_C_ADDR_Pos 2
#define ISI_DMA_C_ADDR_C_ADDR_Msk (0x3fffffffu << ISI_DMA_C_ADDR_C_ADDR_Pos) /**< \brief (ISI_DMA_C_ADDR) Codec Image Base Address */
#define ISI_DMA_C_ADDR_C_ADDR(value) ((ISI_DMA_C_ADDR_C_ADDR_Msk & ((value) << ISI_DMA_C_ADDR_C_ADDR_Pos)))
/* -------- ISI_DMA_C_CTRL : (ISI Offset: 0x54) DMA Codec Control Register -------- */
#define ISI_DMA_C_CTRL_C_FETCH (0x1u << 0) /**< \brief (ISI_DMA_C_CTRL) Descriptor Fetch Control Bit */
#define ISI_DMA_C_CTRL_C_WB (0x1u << 1) /**< \brief (ISI_DMA_C_CTRL) Descriptor Writeback Control Bit */
#define ISI_DMA_C_CTRL_C_IEN (0x1u << 2) /**< \brief (ISI_DMA_C_CTRL) Transfer Done Flag Control */
#define ISI_DMA_C_CTRL_C_DONE (0x1u << 3) /**< \brief (ISI_DMA_C_CTRL) Codec Transfer Done */
/* -------- ISI_DMA_C_DSCR : (ISI Offset: 0x58) DMA Codec Descriptor Address Register -------- */
#define ISI_DMA_C_DSCR_C_DSCR_Pos 2
#define ISI_DMA_C_DSCR_C_DSCR_Msk (0x3fffffffu << ISI_DMA_C_DSCR_C_DSCR_Pos) /**< \brief (ISI_DMA_C_DSCR) Codec Descriptor Base Address */
#define ISI_DMA_C_DSCR_C_DSCR(value) ((ISI_DMA_C_DSCR_C_DSCR_Msk & ((value) << ISI_DMA_C_DSCR_C_DSCR_Pos)))
/* -------- ISI_WPMR : (ISI Offset: 0xE4) Write Protection Mode Register -------- */
#define ISI_WPMR_WPEN (0x1u << 0) /**< \brief (ISI_WPMR) Write Protection Enable */
#define ISI_WPMR_WPKEY_Pos 8
#define ISI_WPMR_WPKEY_Msk (0xffffffu << ISI_WPMR_WPKEY_Pos) /**< \brief (ISI_WPMR) Write Protection Key Password */
#define ISI_WPMR_WPKEY(value) ((ISI_WPMR_WPKEY_Msk & ((value) << ISI_WPMR_WPKEY_Pos)))
#define   ISI_WPMR_WPKEY_PASSWD (0x495349u << 8) /**< \brief (ISI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. */
/* -------- ISI_WPSR : (ISI Offset: 0xE8) Write Protection Status Register -------- */
#define ISI_WPSR_WPVS (0x1u << 0) /**< \brief (ISI_WPSR) Write Protection Violation Status */
#define ISI_WPSR_WPVSRC_Pos 8
#define ISI_WPSR_WPVSRC_Msk (0xffffu << ISI_WPSR_WPVSRC_Pos) /**< \brief (ISI_WPSR) Write Protection Violation Source */
/* -------- ISI_VERSION : (ISI Offset: 0xFC) Version Register -------- */
#define ISI_VERSION_VERSION_Pos 0
#define ISI_VERSION_VERSION_Msk (0xfffu << ISI_VERSION_VERSION_Pos) /**< \brief (ISI_VERSION) Version of the Hardware Module */
#define ISI_VERSION_MFN_Pos 16
#define ISI_VERSION_MFN_Msk (0x7u << ISI_VERSION_MFN_Pos) /**< \brief (ISI_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_ISI_COMPONENT_ */
