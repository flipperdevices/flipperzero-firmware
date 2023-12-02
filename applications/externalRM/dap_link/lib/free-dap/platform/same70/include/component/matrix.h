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

#ifndef _SAME70_MATRIX_COMPONENT_
#define _SAME70_MATRIX_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR AHB Bus Matrix */
/* ============================================================================= */
/** \addtogroup SAME70_MATRIX AHB Bus Matrix */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief MatrixPr hardware registers */
typedef struct {
  __IO uint32_t MATRIX_PRAS; /**< \brief (MatrixPr Offset: 0x0) Priority Register A for Slave 0 */
  __IO uint32_t MATRIX_PRBS; /**< \brief (MatrixPr Offset: 0x4) Priority Register B for Slave 0 */
} MatrixPr;
/** \brief Matrix hardware registers */
#define MATRIXPR_NUMBER 9
typedef struct {
  __IO uint32_t MATRIX_MCFG0;               /**< \brief (Matrix Offset: 0x0000) Master Configuration Register 0 */
  __IO uint32_t MATRIX_MCFG1;               /**< \brief (Matrix Offset: 0x0004) Master Configuration Register 1 */
  __IO uint32_t MATRIX_MCFG2;               /**< \brief (Matrix Offset: 0x0008) Master Configuration Register 2 */
  __IO uint32_t MATRIX_MCFG3;               /**< \brief (Matrix Offset: 0x000C) Master Configuration Register 3 */
  __IO uint32_t MATRIX_MCFG4;               /**< \brief (Matrix Offset: 0x0010) Master Configuration Register 4 */
  __IO uint32_t MATRIX_MCFG5;               /**< \brief (Matrix Offset: 0x0014) Master Configuration Register 5 */
  __IO uint32_t MATRIX_MCFG6;               /**< \brief (Matrix Offset: 0x0018) Master Configuration Register 6 */
  __I  uint32_t Reserved1[1];
  __IO uint32_t MATRIX_MCFG8;               /**< \brief (Matrix Offset: 0x0020) Master Configuration Register 8 */
  __IO uint32_t MATRIX_MCFG9;               /**< \brief (Matrix Offset: 0x0024) Master Configuration Register 9 */
  __IO uint32_t MATRIX_MCFG10;              /**< \brief (Matrix Offset: 0x0028) Master Configuration Register 10 */
  __IO uint32_t MATRIX_MCFG11;              /**< \brief (Matrix Offset: 0x002C) Master Configuration Register 11 */
  __IO uint32_t MATRIX_MCFG12;              /**< \brief (Matrix Offset: 0x0030) Master Configuration Register 12 */
  __I  uint32_t Reserved2[3];
  __IO uint32_t MATRIX_SCFG[9];             /**< \brief (Matrix Offset: 0x0040) Slave Configuration Register */
  __I  uint32_t Reserved3[7];
       MatrixPr MATRIX_PR[MATRIXPR_NUMBER]; /**< \brief (Matrix Offset: 0x0080) 0 .. 8 */
  __I  uint32_t Reserved4[14];
  __IO uint32_t MATRIX_MRCR;                /**< \brief (Matrix Offset: 0x0100) Master Remap Control Register */
  __I  uint32_t Reserved5[3];
  __IO uint32_t CCFG_CAN0;                  /**< \brief (Matrix Offset: 0x0110) CAN0 Configuration Register */
  __IO uint32_t CCFG_SYSIO;                 /**< \brief (Matrix Offset: 0x0114) System I/O and CAN1 Configuration Register */
  __IO uint32_t CCFG_PCCR;                  /**< \brief (Matrix Offset: 0x0118) Peripheral Clock Configuration Register */
  __IO uint32_t CCFG_DYNCFG;                /**< \brief (Matrix Offset: 0x011C) Dynamic Clock Gating Register */
  __I  uint32_t Reserved6[1];
  __IO uint32_t CCFG_SMCNFCS;               /**< \brief (Matrix Offset: 0x0124) SMC NAND Flash Chip Select Configuration Register */
  __I  uint32_t Reserved7[47];
  __IO uint32_t MATRIX_WPMR;                /**< \brief (Matrix Offset: 0x01E4) Write Protection Mode Register */
  __I  uint32_t MATRIX_WPSR;                /**< \brief (Matrix Offset: 0x01E8) Write Protection Status Register */
  __I  uint32_t Reserved8[4];
  __I  uint32_t MATRIX_VERSION;             /**< \brief (Matrix Offset: 0x01FC) Version Register */
} Matrix;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- MATRIX_MCFG0 : (MATRIX Offset: 0x0000) Master Configuration Register 0 -------- */
#define MATRIX_MCFG0_ULBT_Pos 0
#define MATRIX_MCFG0_ULBT_Msk (0x7u << MATRIX_MCFG0_ULBT_Pos) /**< \brief (MATRIX_MCFG0) Undefined Length Burst Type */
#define MATRIX_MCFG0_ULBT(value) ((MATRIX_MCFG0_ULBT_Msk & ((value) << MATRIX_MCFG0_ULBT_Pos)))
#define   MATRIX_MCFG0_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG0) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG0_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG0) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG0_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG0) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG0_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG0) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG0_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG0) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG0_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG0) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG0_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG0) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG0_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG0) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG1 : (MATRIX Offset: 0x0004) Master Configuration Register 1 -------- */
#define MATRIX_MCFG1_ULBT_Pos 0
#define MATRIX_MCFG1_ULBT_Msk (0x7u << MATRIX_MCFG1_ULBT_Pos) /**< \brief (MATRIX_MCFG1) Undefined Length Burst Type */
#define MATRIX_MCFG1_ULBT(value) ((MATRIX_MCFG1_ULBT_Msk & ((value) << MATRIX_MCFG1_ULBT_Pos)))
#define   MATRIX_MCFG1_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG1) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG1_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG1) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG1_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG1) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG1_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG1) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG1_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG1) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG1_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG1) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG1_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG1) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG1_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG1) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG2 : (MATRIX Offset: 0x0008) Master Configuration Register 2 -------- */
#define MATRIX_MCFG2_ULBT_Pos 0
#define MATRIX_MCFG2_ULBT_Msk (0x7u << MATRIX_MCFG2_ULBT_Pos) /**< \brief (MATRIX_MCFG2) Undefined Length Burst Type */
#define MATRIX_MCFG2_ULBT(value) ((MATRIX_MCFG2_ULBT_Msk & ((value) << MATRIX_MCFG2_ULBT_Pos)))
#define   MATRIX_MCFG2_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG2) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG2_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG2) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG2_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG2) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG2_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG2) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG2_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG2) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG2_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG2) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG2_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG2) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG2_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG2) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG3 : (MATRIX Offset: 0x000C) Master Configuration Register 3 -------- */
#define MATRIX_MCFG3_ULBT_Pos 0
#define MATRIX_MCFG3_ULBT_Msk (0x7u << MATRIX_MCFG3_ULBT_Pos) /**< \brief (MATRIX_MCFG3) Undefined Length Burst Type */
#define MATRIX_MCFG3_ULBT(value) ((MATRIX_MCFG3_ULBT_Msk & ((value) << MATRIX_MCFG3_ULBT_Pos)))
#define   MATRIX_MCFG3_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG3) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG3_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG3) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG3_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG3) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG3_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG3) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG3_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG3) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG3_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG3) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG3_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG3) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG3_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG3) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG4 : (MATRIX Offset: 0x0010) Master Configuration Register 4 -------- */
#define MATRIX_MCFG4_ULBT_Pos 0
#define MATRIX_MCFG4_ULBT_Msk (0x7u << MATRIX_MCFG4_ULBT_Pos) /**< \brief (MATRIX_MCFG4) Undefined Length Burst Type */
#define MATRIX_MCFG4_ULBT(value) ((MATRIX_MCFG4_ULBT_Msk & ((value) << MATRIX_MCFG4_ULBT_Pos)))
#define   MATRIX_MCFG4_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG4) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG4_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG4) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG4_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG4) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG4_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG4) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG4_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG4) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG4_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG4) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG4_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG4) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG4_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG4) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG5 : (MATRIX Offset: 0x0014) Master Configuration Register 5 -------- */
#define MATRIX_MCFG5_ULBT_Pos 0
#define MATRIX_MCFG5_ULBT_Msk (0x7u << MATRIX_MCFG5_ULBT_Pos) /**< \brief (MATRIX_MCFG5) Undefined Length Burst Type */
#define MATRIX_MCFG5_ULBT(value) ((MATRIX_MCFG5_ULBT_Msk & ((value) << MATRIX_MCFG5_ULBT_Pos)))
#define   MATRIX_MCFG5_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG5) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG5_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG5) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG5_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG5) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG5_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG5) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG5_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG5) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG5_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG5) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG5_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG5) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG5_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG5) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG6 : (MATRIX Offset: 0x0018) Master Configuration Register 6 -------- */
#define MATRIX_MCFG6_ULBT_Pos 0
#define MATRIX_MCFG6_ULBT_Msk (0x7u << MATRIX_MCFG6_ULBT_Pos) /**< \brief (MATRIX_MCFG6) Undefined Length Burst Type */
#define MATRIX_MCFG6_ULBT(value) ((MATRIX_MCFG6_ULBT_Msk & ((value) << MATRIX_MCFG6_ULBT_Pos)))
#define   MATRIX_MCFG6_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG6) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG6_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG6) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG6_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG6) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG6_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG6) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG6_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG6) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG6_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG6) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG6_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG6) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG6_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG6) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG8 : (MATRIX Offset: 0x0020) Master Configuration Register 8 -------- */
#define MATRIX_MCFG8_ULBT_Pos 0
#define MATRIX_MCFG8_ULBT_Msk (0x7u << MATRIX_MCFG8_ULBT_Pos) /**< \brief (MATRIX_MCFG8) Undefined Length Burst Type */
#define MATRIX_MCFG8_ULBT(value) ((MATRIX_MCFG8_ULBT_Msk & ((value) << MATRIX_MCFG8_ULBT_Pos)))
#define   MATRIX_MCFG8_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG8) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG8_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG8) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG8_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG8) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG8_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG8) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG8_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG8) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG8_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG8) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG8_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG8) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG8_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG8) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG9 : (MATRIX Offset: 0x0024) Master Configuration Register 9 -------- */
#define MATRIX_MCFG9_ULBT_Pos 0
#define MATRIX_MCFG9_ULBT_Msk (0x7u << MATRIX_MCFG9_ULBT_Pos) /**< \brief (MATRIX_MCFG9) Undefined Length Burst Type */
#define MATRIX_MCFG9_ULBT(value) ((MATRIX_MCFG9_ULBT_Msk & ((value) << MATRIX_MCFG9_ULBT_Pos)))
#define   MATRIX_MCFG9_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG9) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG9_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG9) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG9_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG9) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG9_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG9) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG9_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG9) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG9_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG9) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG9_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG9) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG9_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG9) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG10 : (MATRIX Offset: 0x0028) Master Configuration Register 10 -------- */
#define MATRIX_MCFG10_ULBT_Pos 0
#define MATRIX_MCFG10_ULBT_Msk (0x7u << MATRIX_MCFG10_ULBT_Pos) /**< \brief (MATRIX_MCFG10) Undefined Length Burst Type */
#define MATRIX_MCFG10_ULBT(value) ((MATRIX_MCFG10_ULBT_Msk & ((value) << MATRIX_MCFG10_ULBT_Pos)))
#define   MATRIX_MCFG10_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG10) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG10_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG10) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG10_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG10) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG10_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG10) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG10_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG10) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG10_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG10) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG10_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG10) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG10_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG10) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_MCFG11 : (MATRIX Offset: 0x002C) Master Configuration Register 11 -------- */
#define MATRIX_MCFG11_ULBT_Pos 0
#define MATRIX_MCFG11_ULBT_Msk (0x7u << MATRIX_MCFG11_ULBT_Pos) /**< \brief (MATRIX_MCFG11) Undefined Length Burst Type */
#define MATRIX_MCFG11_ULBT(value) ((MATRIX_MCFG11_ULBT_Msk & ((value) << MATRIX_MCFG11_ULBT_Pos)))
#define   MATRIX_MCFG11_ULBT_UNLTD_LENGTH (0x0u << 0) /**< \brief (MATRIX_MCFG11) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. */
#define   MATRIX_MCFG11_ULBT_SINGLE_ACCESS (0x1u << 0) /**< \brief (MATRIX_MCFG11) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. */
#define   MATRIX_MCFG11_ULBT_4BEAT_BURST (0x2u << 0) /**< \brief (MATRIX_MCFG11) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. */
#define   MATRIX_MCFG11_ULBT_8BEAT_BURST (0x3u << 0) /**< \brief (MATRIX_MCFG11) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. */
#define   MATRIX_MCFG11_ULBT_16BEAT_BURST (0x4u << 0) /**< \brief (MATRIX_MCFG11) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. */
#define   MATRIX_MCFG11_ULBT_32BEAT_BURST (0x5u << 0) /**< \brief (MATRIX_MCFG11) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. */
#define   MATRIX_MCFG11_ULBT_64BEAT_BURST (0x6u << 0) /**< \brief (MATRIX_MCFG11) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. */
#define   MATRIX_MCFG11_ULBT_128BEAT_BURST (0x7u << 0) /**< \brief (MATRIX_MCFG11) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. */
/* -------- MATRIX_SCFG[9] : (MATRIX Offset: 0x0040) Slave Configuration Register -------- */
#define MATRIX_SCFG_SLOT_CYCLE_Pos 0
#define MATRIX_SCFG_SLOT_CYCLE_Msk (0x1ffu << MATRIX_SCFG_SLOT_CYCLE_Pos) /**< \brief (MATRIX_SCFG[9]) Maximum Bus Grant Duration for Masters */
#define MATRIX_SCFG_SLOT_CYCLE(value) ((MATRIX_SCFG_SLOT_CYCLE_Msk & ((value) << MATRIX_SCFG_SLOT_CYCLE_Pos)))
#define MATRIX_SCFG_DEFMSTR_TYPE_Pos 16
#define MATRIX_SCFG_DEFMSTR_TYPE_Msk (0x3u << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< \brief (MATRIX_SCFG[9]) Default Master Type */
#define MATRIX_SCFG_DEFMSTR_TYPE(value) ((MATRIX_SCFG_DEFMSTR_TYPE_Msk & ((value) << MATRIX_SCFG_DEFMSTR_TYPE_Pos)))
#define   MATRIX_SCFG_DEFMSTR_TYPE_NONE (0x0u << 16) /**< \brief (MATRIX_SCFG[9]) No Default Master-At the end of the current slave access, if no other master request is pending, the slave is disconnected from all masters.This results in a one clock cycle latency for the first access of a burst transfer or for a single access. */
#define   MATRIX_SCFG_DEFMSTR_TYPE_LAST (0x1u << 16) /**< \brief (MATRIX_SCFG[9]) Last Default Master-At the end of the current slave access, if no other master request is pending, the slave stays connected to the last master having accessed it.This results in not having one clock cycle latency when the last master tries to access the slave again. */
#define   MATRIX_SCFG_DEFMSTR_TYPE_FIXED (0x2u << 16) /**< \brief (MATRIX_SCFG[9]) Fixed Default Master-At the end of the current slave access, if no other master request is pending, the slave connects to the fixed master the number that has been written in the FIXED_DEFMSTR field.This results in not having one clock cycle latency when the fixed master tries to access the slave again. */
#define MATRIX_SCFG_FIXED_DEFMSTR_Pos 18
#define MATRIX_SCFG_FIXED_DEFMSTR_Msk (0xfu << MATRIX_SCFG_FIXED_DEFMSTR_Pos) /**< \brief (MATRIX_SCFG[9]) Fixed Default Master */
#define MATRIX_SCFG_FIXED_DEFMSTR(value) ((MATRIX_SCFG_FIXED_DEFMSTR_Msk & ((value) << MATRIX_SCFG_FIXED_DEFMSTR_Pos)))
/* -------- MATRIX_PRAS : (MATRIX Offset: N/A) Priority Register A for Slave 0 -------- */
#define MATRIX_PRAS_M0PR_Pos 0
#define MATRIX_PRAS_M0PR_Msk (0x3u << MATRIX_PRAS_M0PR_Pos) /**< \brief (MATRIX_PRAS) Master 0 Priority */
#define MATRIX_PRAS_M0PR(value) ((MATRIX_PRAS_M0PR_Msk & ((value) << MATRIX_PRAS_M0PR_Pos)))
#define MATRIX_PRAS_M1PR_Pos 4
#define MATRIX_PRAS_M1PR_Msk (0x3u << MATRIX_PRAS_M1PR_Pos) /**< \brief (MATRIX_PRAS) Master 1 Priority */
#define MATRIX_PRAS_M1PR(value) ((MATRIX_PRAS_M1PR_Msk & ((value) << MATRIX_PRAS_M1PR_Pos)))
#define MATRIX_PRAS_M2PR_Pos 8
#define MATRIX_PRAS_M2PR_Msk (0x3u << MATRIX_PRAS_M2PR_Pos) /**< \brief (MATRIX_PRAS) Master 2 Priority */
#define MATRIX_PRAS_M2PR(value) ((MATRIX_PRAS_M2PR_Msk & ((value) << MATRIX_PRAS_M2PR_Pos)))
#define MATRIX_PRAS_M3PR_Pos 12
#define MATRIX_PRAS_M3PR_Msk (0x3u << MATRIX_PRAS_M3PR_Pos) /**< \brief (MATRIX_PRAS) Master 3 Priority */
#define MATRIX_PRAS_M3PR(value) ((MATRIX_PRAS_M3PR_Msk & ((value) << MATRIX_PRAS_M3PR_Pos)))
#define MATRIX_PRAS_M4PR_Pos 16
#define MATRIX_PRAS_M4PR_Msk (0x3u << MATRIX_PRAS_M4PR_Pos) /**< \brief (MATRIX_PRAS) Master 4 Priority */
#define MATRIX_PRAS_M4PR(value) ((MATRIX_PRAS_M4PR_Msk & ((value) << MATRIX_PRAS_M4PR_Pos)))
#define MATRIX_PRAS_M5PR_Pos 20
#define MATRIX_PRAS_M5PR_Msk (0x3u << MATRIX_PRAS_M5PR_Pos) /**< \brief (MATRIX_PRAS) Master 5 Priority */
#define MATRIX_PRAS_M5PR(value) ((MATRIX_PRAS_M5PR_Msk & ((value) << MATRIX_PRAS_M5PR_Pos)))
#define MATRIX_PRAS_M6PR_Pos 24
#define MATRIX_PRAS_M6PR_Msk (0x3u << MATRIX_PRAS_M6PR_Pos) /**< \brief (MATRIX_PRAS) Master 6 Priority */
#define MATRIX_PRAS_M6PR(value) ((MATRIX_PRAS_M6PR_Msk & ((value) << MATRIX_PRAS_M6PR_Pos)))
/* -------- MATRIX_PRBS : (MATRIX Offset: N/A) Priority Register B for Slave 0 -------- */
#define MATRIX_PRBS_M8PR_Pos 0
#define MATRIX_PRBS_M8PR_Msk (0x3u << MATRIX_PRBS_M8PR_Pos) /**< \brief (MATRIX_PRBS) Master 8 Priority */
#define MATRIX_PRBS_M8PR(value) ((MATRIX_PRBS_M8PR_Msk & ((value) << MATRIX_PRBS_M8PR_Pos)))
#define MATRIX_PRBS_M9PR_Pos 4
#define MATRIX_PRBS_M9PR_Msk (0x3u << MATRIX_PRBS_M9PR_Pos) /**< \brief (MATRIX_PRBS) Master 9 Priority */
#define MATRIX_PRBS_M9PR(value) ((MATRIX_PRBS_M9PR_Msk & ((value) << MATRIX_PRBS_M9PR_Pos)))
#define MATRIX_PRBS_M10PR_Pos 8
#define MATRIX_PRBS_M10PR_Msk (0x3u << MATRIX_PRBS_M10PR_Pos) /**< \brief (MATRIX_PRBS) Master 10 Priority */
#define MATRIX_PRBS_M10PR(value) ((MATRIX_PRBS_M10PR_Msk & ((value) << MATRIX_PRBS_M10PR_Pos)))
#define MATRIX_PRBS_M11PR_Pos 12
#define MATRIX_PRBS_M11PR_Msk (0x3u << MATRIX_PRBS_M11PR_Pos) /**< \brief (MATRIX_PRBS) Master 11 Priority */
#define MATRIX_PRBS_M11PR(value) ((MATRIX_PRBS_M11PR_Msk & ((value) << MATRIX_PRBS_M11PR_Pos)))
#define MATRIX_PRBS_M12PR_Pos 16
#define MATRIX_PRBS_M12PR_Msk (0x3u << MATRIX_PRBS_M12PR_Pos) /**< \brief (MATRIX_PRBS) Master 12 Priority */
#define MATRIX_PRBS_M12PR(value) ((MATRIX_PRBS_M12PR_Msk & ((value) << MATRIX_PRBS_M12PR_Pos)))
/* -------- MATRIX_MRCR : (MATRIX Offset: 0x0100) Master Remap Control Register -------- */
#define MATRIX_MRCR_RCB0 (0x1u << 0) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 0 */
#define MATRIX_MRCR_RCB1 (0x1u << 1) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 1 */
#define MATRIX_MRCR_RCB2 (0x1u << 2) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 2 */
#define MATRIX_MRCR_RCB3 (0x1u << 3) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 3 */
#define MATRIX_MRCR_RCB4 (0x1u << 4) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 4 */
#define MATRIX_MRCR_RCB5 (0x1u << 5) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 5 */
#define MATRIX_MRCR_RCB6 (0x1u << 6) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 6 */
#define MATRIX_MRCR_RCB8 (0x1u << 8) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 8 */
#define MATRIX_MRCR_RCB9 (0x1u << 9) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 9 */
#define MATRIX_MRCR_RCB10 (0x1u << 10) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 10 */
#define MATRIX_MRCR_RCB11 (0x1u << 11) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 11 */
#define MATRIX_MRCR_RCB12 (0x1u << 12) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 12 */
/* -------- CCFG_CAN0 : (MATRIX Offset: 0x0110) CAN0 Configuration Register -------- */
#define CCFG_CAN0_CAN0DMABA_Pos 16
#define CCFG_CAN0_CAN0DMABA_Msk (0xffffu << CCFG_CAN0_CAN0DMABA_Pos) /**< \brief (CCFG_CAN0) CAN0 DMA Base Address */
#define CCFG_CAN0_CAN0DMABA(value) ((CCFG_CAN0_CAN0DMABA_Msk & ((value) << CCFG_CAN0_CAN0DMABA_Pos)))
/* -------- CCFG_SYSIO : (MATRIX Offset: 0x0114) System I/O and CAN1 Configuration Register -------- */
#define CCFG_SYSIO_SYSIO4 (0x1u << 4) /**< \brief (CCFG_SYSIO) PB4 or TDI Assignment */
#define CCFG_SYSIO_SYSIO5 (0x1u << 5) /**< \brief (CCFG_SYSIO) PB5 or TDO/TRACESWO Assignment */
#define CCFG_SYSIO_SYSIO6 (0x1u << 6) /**< \brief (CCFG_SYSIO) PB6 or TMS/SWDIO Assignment */
#define CCFG_SYSIO_SYSIO7 (0x1u << 7) /**< \brief (CCFG_SYSIO) PB7 or TCK/SWCLK Assignment */
#define CCFG_SYSIO_SYSIO12 (0x1u << 12) /**< \brief (CCFG_SYSIO) PB12 or ERASE Assignment */
#define CCFG_SYSIO_CAN1DMABA_Pos 16
#define CCFG_SYSIO_CAN1DMABA_Msk (0xffffu << CCFG_SYSIO_CAN1DMABA_Pos) /**< \brief (CCFG_SYSIO) CAN1 DMA Base Address */
#define CCFG_SYSIO_CAN1DMABA(value) ((CCFG_SYSIO_CAN1DMABA_Msk & ((value) << CCFG_SYSIO_CAN1DMABA_Pos)))
/* -------- CCFG_PCCR : (MATRIX Offset: 0x0118) Peripheral Clock Configuration Register -------- */
#define CCFG_PCCR_TC0CC (0x1u << 20) /**< \brief (CCFG_PCCR) TC0 Clock Configuration */
#define CCFG_PCCR_I2SC0CC (0x1u << 21) /**< \brief (CCFG_PCCR) I2SC0 Clock Configuration */
#define CCFG_PCCR_I2SC1CC (0x1u << 22) /**< \brief (CCFG_PCCR) I2SC1 Clock Configuration */
/* -------- CCFG_DYNCKG : (MATRIX Offset: 0x11c) (R/W 32) Dynamic Clock Gating Register -------- */
#define CCFG_DYNCKG_MATCKG  (0x1u << 0) /**< \brief (CCFG_DYNCKG) MATRIX Dynamic Clock Gating */
#define CCFG_DYNCKG_BRIDCKG (0x1u << 1) /**< \brief (CCFG_DYNCKG) Bridge Dynamic Clock Gating Enable */
#define CCFG_DYNCKG_EFCCKG  (0x1u << 2) /**< \brief (CCFG_DYNCKG) EFC Dynamic Clock Gating Enable */
/* -------- CCFG_SMCNFCS : (MATRIX Offset: 0x0124) SMC NAND Flash Chip Select Configuration Register -------- */
#define CCFG_SMCNFCS_SMC_NFCS0 (0x1u << 0) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 0 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS1 (0x1u << 1) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 1 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS2 (0x1u << 2) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 2 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS3 (0x1u << 3) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 3 Assignment */
#define CCFG_SMCNFCS_SDRAMEN (0x1u << 4) /**< \brief (CCFG_SMCNFCS) SDRAM Enable */
/* -------- MATRIX_WPMR : (MATRIX Offset: 0x01E4) Write Protection Mode Register -------- */
#define MATRIX_WPMR_WPEN (0x1u << 0) /**< \brief (MATRIX_WPMR) Write Protection Enable */
#define MATRIX_WPMR_WPKEY_Pos 8
#define MATRIX_WPMR_WPKEY_Msk (0xffffffu << MATRIX_WPMR_WPKEY_Pos) /**< \brief (MATRIX_WPMR) Write Protection Key */
#define MATRIX_WPMR_WPKEY(value) ((MATRIX_WPMR_WPKEY_Msk & ((value) << MATRIX_WPMR_WPKEY_Pos)))
#define   MATRIX_WPMR_WPKEY_PASSWD (0x4D4154u << 8) /**< \brief (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. */
/* -------- MATRIX_WPSR : (MATRIX Offset: 0x01E8) Write Protection Status Register -------- */
#define MATRIX_WPSR_WPVS (0x1u << 0) /**< \brief (MATRIX_WPSR) Write Protection Violation Status */
#define MATRIX_WPSR_WPVSRC_Pos 8
#define MATRIX_WPSR_WPVSRC_Msk (0xffffu << MATRIX_WPSR_WPVSRC_Pos) /**< \brief (MATRIX_WPSR) Write Protection Violation Source */
/* -------- MATRIX_VERSION : (MATRIX Offset: 0x01FC) Version Register -------- */
#define MATRIX_VERSION_VERSION_Pos 0
#define MATRIX_VERSION_VERSION_Msk (0xfffu << MATRIX_VERSION_VERSION_Pos) /**< \brief (MATRIX_VERSION) Version of the Hardware Module */
#define MATRIX_VERSION_MFN_Pos 16
#define MATRIX_VERSION_MFN_Msk (0x7u << MATRIX_VERSION_MFN_Pos) /**< \brief (MATRIX_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_MATRIX_COMPONENT_ */
