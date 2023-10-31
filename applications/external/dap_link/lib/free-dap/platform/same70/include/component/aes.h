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

#ifndef _SAME70_AES_COMPONENT_
#define _SAME70_AES_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Advanced Encryption Standard */
/* ============================================================================= */
/** \addtogroup SAME70_AES Advanced Encryption Standard */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Aes hardware registers */
typedef struct {
  __O  uint32_t AES_CR;        /**< \brief (Aes Offset: 0x00) Control Register */
  __IO uint32_t AES_MR;        /**< \brief (Aes Offset: 0x04) Mode Register */
  __I  uint32_t Reserved1[2];
  __O  uint32_t AES_IER;       /**< \brief (Aes Offset: 0x10) Interrupt Enable Register */
  __O  uint32_t AES_IDR;       /**< \brief (Aes Offset: 0x14) Interrupt Disable Register */
  __I  uint32_t AES_IMR;       /**< \brief (Aes Offset: 0x18) Interrupt Mask Register */
  __I  uint32_t AES_ISR;       /**< \brief (Aes Offset: 0x1C) Interrupt Status Register */
  __O  uint32_t AES_KEYWR[8];  /**< \brief (Aes Offset: 0x20) Key Word Register */
  __O  uint32_t AES_IDATAR[4]; /**< \brief (Aes Offset: 0x40) Input Data Register */
  __I  uint32_t AES_ODATAR[4]; /**< \brief (Aes Offset: 0x50) Output Data Register */
  __O  uint32_t AES_IVR[4];    /**< \brief (Aes Offset: 0x60) Initialization Vector Register */
  __IO uint32_t AES_AADLENR;   /**< \brief (Aes Offset: 0x70) Additional Authenticated Data Length Register */
  __IO uint32_t AES_CLENR;     /**< \brief (Aes Offset: 0x74) Plaintext/Ciphertext Length Register */
  __IO uint32_t AES_GHASHR[4]; /**< \brief (Aes Offset: 0x78) GCM Intermediate Hash Word Register */
  __I  uint32_t AES_TAGR[4];   /**< \brief (Aes Offset: 0x88) GCM Authentication Tag Word Register */
  __I  uint32_t AES_CTRR;      /**< \brief (Aes Offset: 0x98) GCM Encryption Counter Value Register */
  __IO uint32_t AES_GCMHR[4];  /**< \brief (Aes Offset: 0x9C) GCM H Word Register */
  __I  uint32_t Reserved2[20];
  __I  uint32_t AES_VERSION;   /**< \brief (Aes Offset: 0xFC) Version Register */
} Aes;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- AES_CR : (AES Offset: 0x00) Control Register -------- */
#define AES_CR_START (0x1u << 0) /**< \brief (AES_CR) Start Processing */
#define AES_CR_SWRST (0x1u << 8) /**< \brief (AES_CR) Software Reset */
#define AES_CR_LOADSEED (0x1u << 16) /**< \brief (AES_CR) Random Number Generator Seed Loading */
/* -------- AES_MR : (AES Offset: 0x04) Mode Register -------- */
#define AES_MR_CIPHER (0x1u << 0) /**< \brief (AES_MR) Processing Mode */
#define AES_MR_GTAGEN (0x1u << 1) /**< \brief (AES_MR) GCM Automatic Tag Generation Enable */
#define AES_MR_DUALBUFF (0x1u << 3) /**< \brief (AES_MR) Dual Input Buffer */
#define   AES_MR_DUALBUFF_INACTIVE (0x0u << 3) /**< \brief (AES_MR) AES_IDATARx cannot be written during processing of previous block. */
#define   AES_MR_DUALBUFF_ACTIVE (0x1u << 3) /**< \brief (AES_MR) AES_IDATARx can be written during processing of previous block when SMOD = 0x2. It speeds up the overall runtime of large files. */
#define AES_MR_PROCDLY_Pos 4
#define AES_MR_PROCDLY_Msk (0xfu << AES_MR_PROCDLY_Pos) /**< \brief (AES_MR) Processing Delay */
#define AES_MR_PROCDLY(value) ((AES_MR_PROCDLY_Msk & ((value) << AES_MR_PROCDLY_Pos)))
#define AES_MR_SMOD_Pos 8
#define AES_MR_SMOD_Msk (0x3u << AES_MR_SMOD_Pos) /**< \brief (AES_MR) Start Mode */
#define AES_MR_SMOD(value) ((AES_MR_SMOD_Msk & ((value) << AES_MR_SMOD_Pos)))
#define   AES_MR_SMOD_MANUAL_START (0x0u << 8) /**< \brief (AES_MR) Manual Mode */
#define   AES_MR_SMOD_AUTO_START (0x1u << 8) /**< \brief (AES_MR) Auto Mode */
#define   AES_MR_SMOD_IDATAR0_START (0x2u << 8) /**< \brief (AES_MR) AES_IDATAR0 access only Auto Mode (DMA) */
#define AES_MR_KEYSIZE_Pos 10
#define AES_MR_KEYSIZE_Msk (0x3u << AES_MR_KEYSIZE_Pos) /**< \brief (AES_MR) Key Size */
#define AES_MR_KEYSIZE(value) ((AES_MR_KEYSIZE_Msk & ((value) << AES_MR_KEYSIZE_Pos)))
#define   AES_MR_KEYSIZE_AES128 (0x0u << 10) /**< \brief (AES_MR) AES Key Size is 128 bits */
#define   AES_MR_KEYSIZE_AES192 (0x1u << 10) /**< \brief (AES_MR) AES Key Size is 192 bits */
#define   AES_MR_KEYSIZE_AES256 (0x2u << 10) /**< \brief (AES_MR) AES Key Size is 256 bits */
#define AES_MR_OPMOD_Pos 12
#define AES_MR_OPMOD_Msk (0x7u << AES_MR_OPMOD_Pos) /**< \brief (AES_MR) Operation Mode */
#define AES_MR_OPMOD(value) ((AES_MR_OPMOD_Msk & ((value) << AES_MR_OPMOD_Pos)))
#define   AES_MR_OPMOD_ECB (0x0u << 12) /**< \brief (AES_MR) ECB: Electronic Code Book mode */
#define   AES_MR_OPMOD_CBC (0x1u << 12) /**< \brief (AES_MR) CBC: Cipher Block Chaining mode */
#define   AES_MR_OPMOD_OFB (0x2u << 12) /**< \brief (AES_MR) OFB: Output Feedback mode */
#define   AES_MR_OPMOD_CFB (0x3u << 12) /**< \brief (AES_MR) CFB: Cipher Feedback mode */
#define   AES_MR_OPMOD_CTR (0x4u << 12) /**< \brief (AES_MR) CTR: Counter mode (16-bit internal counter) */
#define   AES_MR_OPMOD_GCM (0x5u << 12) /**< \brief (AES_MR) GCM: Galois/Counter mode */
#define AES_MR_LOD (0x1u << 15) /**< \brief (AES_MR) Last Output Data Mode */
#define AES_MR_CFBS_Pos 16
#define AES_MR_CFBS_Msk (0x7u << AES_MR_CFBS_Pos) /**< \brief (AES_MR) Cipher Feedback Data Size */
#define AES_MR_CFBS(value) ((AES_MR_CFBS_Msk & ((value) << AES_MR_CFBS_Pos)))
#define   AES_MR_CFBS_SIZE_128BIT (0x0u << 16) /**< \brief (AES_MR) 128-bit */
#define   AES_MR_CFBS_SIZE_64BIT (0x1u << 16) /**< \brief (AES_MR) 64-bit */
#define   AES_MR_CFBS_SIZE_32BIT (0x2u << 16) /**< \brief (AES_MR) 32-bit */
#define   AES_MR_CFBS_SIZE_16BIT (0x3u << 16) /**< \brief (AES_MR) 16-bit */
#define   AES_MR_CFBS_SIZE_8BIT (0x4u << 16) /**< \brief (AES_MR) 8-bit */
#define AES_MR_CKEY_Pos 20
#define AES_MR_CKEY_Msk (0xfu << AES_MR_CKEY_Pos) /**< \brief (AES_MR) Key */
#define AES_MR_CKEY(value) ((AES_MR_CKEY_Msk & ((value) << AES_MR_CKEY_Pos)))
#define   AES_MR_CKEY_PASSWD (0xEu << 20) /**< \brief (AES_MR) This field must be written with 0xE to allow CMTYPx bit configuration changes. Any other values will abort the write operation in CMTYPx bits.Always reads as 0. */
#define AES_MR_CMTYP1 (0x1u << 24) /**< \brief (AES_MR) Countermeasure Type 1 */
#define   AES_MR_CMTYP1_NOPROT_EXTKEY (0x0u << 24) /**< \brief (AES_MR) Countermeasure type 1 is disabled. */
#define   AES_MR_CMTYP1_PROT_EXTKEY (0x1u << 24) /**< \brief (AES_MR) Countermeasure type 1 is enabled. */
#define AES_MR_CMTYP2 (0x1u << 25) /**< \brief (AES_MR) Countermeasure Type 2 */
#define   AES_MR_CMTYP2_NO_PAUSE (0x0u << 25) /**< \brief (AES_MR) Countermeasure type 2 is disabled. */
#define   AES_MR_CMTYP2_PAUSE (0x1u << 25) /**< \brief (AES_MR) Countermeasure type 2 is enabled. */
#define AES_MR_CMTYP3 (0x1u << 26) /**< \brief (AES_MR) Countermeasure Type 3 */
#define   AES_MR_CMTYP3_NO_DUMMY (0x0u << 26) /**< \brief (AES_MR) Countermeasure type 3 is disabled. */
#define   AES_MR_CMTYP3_DUMMY (0x1u << 26) /**< \brief (AES_MR) Countermeasure type 3 is enabled. */
#define AES_MR_CMTYP4 (0x1u << 27) /**< \brief (AES_MR) Countermeasure Type 4 */
#define   AES_MR_CMTYP4_NO_RESTART (0x0u << 27) /**< \brief (AES_MR) Countermeasure type 4 is disabled. */
#define   AES_MR_CMTYP4_RESTART (0x1u << 27) /**< \brief (AES_MR) Countermeasure type 4 is enabled. */
#define AES_MR_CMTYP5 (0x1u << 28) /**< \brief (AES_MR) Countermeasure Type 5 */
#define   AES_MR_CMTYP5_NO_ADDACCESS (0x0u << 28) /**< \brief (AES_MR) Countermeasure type 5 is disabled. */
#define   AES_MR_CMTYP5_ADDACCESS (0x1u << 28) /**< \brief (AES_MR) Countermeasure type 5 is enabled. */
#define AES_MR_CMTYP6 (0x1u << 29) /**< \brief (AES_MR) Countermeasure Type 6 */
#define   AES_MR_CMTYP6_NO_IDLECURRENT (0x0u << 29) /**< \brief (AES_MR) Countermeasure type 6 is disabled. */
#define   AES_MR_CMTYP6_IDLECURRENT (0x1u << 29) /**< \brief (AES_MR) Countermeasure type 6 is enabled. */
/* -------- AES_IER : (AES Offset: 0x10) Interrupt Enable Register -------- */
#define AES_IER_DATRDY (0x1u << 0) /**< \brief (AES_IER) Data Ready Interrupt Enable */
#define AES_IER_URAD (0x1u << 8) /**< \brief (AES_IER) Unspecified Register Access Detection Interrupt Enable */
#define AES_IER_TAGRDY (0x1u << 16) /**< \brief (AES_IER) GCM Tag Ready Interrupt Enable */
/* -------- AES_IDR : (AES Offset: 0x14) Interrupt Disable Register -------- */
#define AES_IDR_DATRDY (0x1u << 0) /**< \brief (AES_IDR) Data Ready Interrupt Disable */
#define AES_IDR_URAD (0x1u << 8) /**< \brief (AES_IDR) Unspecified Register Access Detection Interrupt Disable */
#define AES_IDR_TAGRDY (0x1u << 16) /**< \brief (AES_IDR) GCM Tag Ready Interrupt Disable */
/* -------- AES_IMR : (AES Offset: 0x18) Interrupt Mask Register -------- */
#define AES_IMR_DATRDY (0x1u << 0) /**< \brief (AES_IMR) Data Ready Interrupt Mask */
#define AES_IMR_URAD (0x1u << 8) /**< \brief (AES_IMR) Unspecified Register Access Detection Interrupt Mask */
#define AES_IMR_TAGRDY (0x1u << 16) /**< \brief (AES_IMR) GCM Tag Ready Interrupt Mask */
/* -------- AES_ISR : (AES Offset: 0x1C) Interrupt Status Register -------- */
#define AES_ISR_DATRDY (0x1u << 0) /**< \brief (AES_ISR) Data Ready (cleared by setting bit START or bit SWRST in AES_CR or by reading AES_ODATARx) */
#define AES_ISR_URAD (0x1u << 8) /**< \brief (AES_ISR) Unspecified Register Access Detection Status (cleared by writing SWRST in AES_CR) */
#define AES_ISR_URAT_Pos 12
#define AES_ISR_URAT_Msk (0xfu << AES_ISR_URAT_Pos) /**< \brief (AES_ISR) Unspecified Register Access (cleared by writing SWRST in AES_CR) */
#define   AES_ISR_URAT_IDR_WR_PROCESSING (0x0u << 12) /**< \brief (AES_ISR) Input Data Register written during the data processing when SMOD = 0x2 mode. */
#define   AES_ISR_URAT_ODR_RD_PROCESSING (0x1u << 12) /**< \brief (AES_ISR) Output Data Register read during the data processing. */
#define   AES_ISR_URAT_MR_WR_PROCESSING (0x2u << 12) /**< \brief (AES_ISR) Mode Register written during the data processing. */
#define   AES_ISR_URAT_ODR_RD_SUBKGEN (0x3u << 12) /**< \brief (AES_ISR) Output Data Register read during the sub-keys generation. */
#define   AES_ISR_URAT_MR_WR_SUBKGEN (0x4u << 12) /**< \brief (AES_ISR) Mode Register written during the sub-keys generation. */
#define   AES_ISR_URAT_WOR_RD_ACCESS (0x5u << 12) /**< \brief (AES_ISR) Write-only register read access. */
#define AES_ISR_TAGRDY (0x1u << 16) /**< \brief (AES_ISR) GCM Tag Ready */
/* -------- AES_KEYWR[8] : (AES Offset: 0x20) Key Word Register -------- */
#define AES_KEYWR_KEYW_Pos 0
#define AES_KEYWR_KEYW_Msk (0xffffffffu << AES_KEYWR_KEYW_Pos) /**< \brief (AES_KEYWR[8]) Key Word */
#define AES_KEYWR_KEYW(value) ((AES_KEYWR_KEYW_Msk & ((value) << AES_KEYWR_KEYW_Pos)))
/* -------- AES_IDATAR[4] : (AES Offset: 0x40) Input Data Register -------- */
#define AES_IDATAR_IDATA_Pos 0
#define AES_IDATAR_IDATA_Msk (0xffffffffu << AES_IDATAR_IDATA_Pos) /**< \brief (AES_IDATAR[4]) Input Data Word */
#define AES_IDATAR_IDATA(value) ((AES_IDATAR_IDATA_Msk & ((value) << AES_IDATAR_IDATA_Pos)))
/* -------- AES_ODATAR[4] : (AES Offset: 0x50) Output Data Register -------- */
#define AES_ODATAR_ODATA_Pos 0
#define AES_ODATAR_ODATA_Msk (0xffffffffu << AES_ODATAR_ODATA_Pos) /**< \brief (AES_ODATAR[4]) Output Data */
/* -------- AES_IVR[4] : (AES Offset: 0x60) Initialization Vector Register -------- */
#define AES_IVR_IV_Pos 0
#define AES_IVR_IV_Msk (0xffffffffu << AES_IVR_IV_Pos) /**< \brief (AES_IVR[4]) Initialization Vector */
#define AES_IVR_IV(value) ((AES_IVR_IV_Msk & ((value) << AES_IVR_IV_Pos)))
/* -------- AES_AADLENR : (AES Offset: 0x70) Additional Authenticated Data Length Register -------- */
#define AES_AADLENR_AADLEN_Pos 0
#define AES_AADLENR_AADLEN_Msk (0xffffffffu << AES_AADLENR_AADLEN_Pos) /**< \brief (AES_AADLENR) Additional Authenticated Data Length */
#define AES_AADLENR_AADLEN(value) ((AES_AADLENR_AADLEN_Msk & ((value) << AES_AADLENR_AADLEN_Pos)))
/* -------- AES_CLENR : (AES Offset: 0x74) Plaintext/Ciphertext Length Register -------- */
#define AES_CLENR_CLEN_Pos 0
#define AES_CLENR_CLEN_Msk (0xffffffffu << AES_CLENR_CLEN_Pos) /**< \brief (AES_CLENR) Plaintext/Ciphertext Length */
#define AES_CLENR_CLEN(value) ((AES_CLENR_CLEN_Msk & ((value) << AES_CLENR_CLEN_Pos)))
/* -------- AES_GHASHR[4] : (AES Offset: 0x78) GCM Intermediate Hash Word Register -------- */
#define AES_GHASHR_GHASH_Pos 0
#define AES_GHASHR_GHASH_Msk (0xffffffffu << AES_GHASHR_GHASH_Pos) /**< \brief (AES_GHASHR[4]) Intermediate GCM Hash Word x */
#define AES_GHASHR_GHASH(value) ((AES_GHASHR_GHASH_Msk & ((value) << AES_GHASHR_GHASH_Pos)))
/* -------- AES_TAGR[4] : (AES Offset: 0x88) GCM Authentication Tag Word Register -------- */
#define AES_TAGR_TAG_Pos 0
#define AES_TAGR_TAG_Msk (0xffffffffu << AES_TAGR_TAG_Pos) /**< \brief (AES_TAGR[4]) GCM Authentication Tag x */
/* -------- AES_CTRR : (AES Offset: 0x98) GCM Encryption Counter Value Register -------- */
#define AES_CTRR_CTR_Pos 0
#define AES_CTRR_CTR_Msk (0xffffffffu << AES_CTRR_CTR_Pos) /**< \brief (AES_CTRR) GCM Encryption Counter */
/* -------- AES_GCMHR[4] : (AES Offset: 0x9C) GCM H Word Register -------- */
#define AES_GCMHR_H_Pos 0
#define AES_GCMHR_H_Msk (0xffffffffu << AES_GCMHR_H_Pos) /**< \brief (AES_GCMHR[4]) GCM H Word x */
#define AES_GCMHR_H(value) ((AES_GCMHR_H_Msk & ((value) << AES_GCMHR_H_Pos)))
/* -------- AES_VERSION : (AES Offset: 0xFC) Version Register -------- */
#define AES_VERSION_VERSION_Pos 0
#define AES_VERSION_VERSION_Msk (0xfffu << AES_VERSION_VERSION_Pos) /**< \brief (AES_VERSION) Version of the Hardware Module */
#define AES_VERSION_MFN_Pos 16
#define AES_VERSION_MFN_Msk (0x7u << AES_VERSION_MFN_Pos) /**< \brief (AES_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_AES_COMPONENT_ */
