/**
 * \file
 *
 * \brief Component description for HMATRIXB
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD11_HMATRIXB_COMPONENT_
#define _SAMD11_HMATRIXB_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR HMATRIXB */
/* ========================================================================== */
/** \addtogroup SAMD11_HMATRIXB HSB Matrix */
/*@{*/

#define HMATRIXB_I7638
#define REV_HMATRIXB                0x212

/* -------- HMATRIXB_PRAS : (HMATRIXB Offset: 0x080) (R/W 32) PRS Priority A for Slave -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t M0PR:4;           /*!< bit:  0.. 3  Master 0 Priority                  */
    uint32_t M1PR:4;           /*!< bit:  4.. 7  Master 1 Priority                  */
    uint32_t M2PR:4;           /*!< bit:  8..11  Master 2 Priority                  */
    uint32_t M3PR:4;           /*!< bit: 12..15  Master 3 Priority                  */
    uint32_t M4PR:4;           /*!< bit: 16..19  Master 4 Priority                  */
    uint32_t M5PR:4;           /*!< bit: 20..23  Master 5 Priority                  */
    uint32_t M6PR:4;           /*!< bit: 24..27  Master 6 Priority                  */
    uint32_t M7PR:4;           /*!< bit: 28..31  Master 7 Priority                  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} HMATRIXB_PRAS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define HMATRIXB_PRAS_OFFSET        0x080        /**< \brief (HMATRIXB_PRAS offset) Priority A for Slave */
#define HMATRIXB_PRAS_RESETVALUE    _U_(0x00000000) /**< \brief (HMATRIXB_PRAS reset_value) Priority A for Slave */

#define HMATRIXB_PRAS_M0PR_Pos      0            /**< \brief (HMATRIXB_PRAS) Master 0 Priority */
#define HMATRIXB_PRAS_M0PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M0PR_Pos)
#define HMATRIXB_PRAS_M0PR(value)   (HMATRIXB_PRAS_M0PR_Msk & ((value) << HMATRIXB_PRAS_M0PR_Pos))
#define HMATRIXB_PRAS_M1PR_Pos      4            /**< \brief (HMATRIXB_PRAS) Master 1 Priority */
#define HMATRIXB_PRAS_M1PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M1PR_Pos)
#define HMATRIXB_PRAS_M1PR(value)   (HMATRIXB_PRAS_M1PR_Msk & ((value) << HMATRIXB_PRAS_M1PR_Pos))
#define HMATRIXB_PRAS_M2PR_Pos      8            /**< \brief (HMATRIXB_PRAS) Master 2 Priority */
#define HMATRIXB_PRAS_M2PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M2PR_Pos)
#define HMATRIXB_PRAS_M2PR(value)   (HMATRIXB_PRAS_M2PR_Msk & ((value) << HMATRIXB_PRAS_M2PR_Pos))
#define HMATRIXB_PRAS_M3PR_Pos      12           /**< \brief (HMATRIXB_PRAS) Master 3 Priority */
#define HMATRIXB_PRAS_M3PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M3PR_Pos)
#define HMATRIXB_PRAS_M3PR(value)   (HMATRIXB_PRAS_M3PR_Msk & ((value) << HMATRIXB_PRAS_M3PR_Pos))
#define HMATRIXB_PRAS_M4PR_Pos      16           /**< \brief (HMATRIXB_PRAS) Master 4 Priority */
#define HMATRIXB_PRAS_M4PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M4PR_Pos)
#define HMATRIXB_PRAS_M4PR(value)   (HMATRIXB_PRAS_M4PR_Msk & ((value) << HMATRIXB_PRAS_M4PR_Pos))
#define HMATRIXB_PRAS_M5PR_Pos      20           /**< \brief (HMATRIXB_PRAS) Master 5 Priority */
#define HMATRIXB_PRAS_M5PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M5PR_Pos)
#define HMATRIXB_PRAS_M5PR(value)   (HMATRIXB_PRAS_M5PR_Msk & ((value) << HMATRIXB_PRAS_M5PR_Pos))
#define HMATRIXB_PRAS_M6PR_Pos      24           /**< \brief (HMATRIXB_PRAS) Master 6 Priority */
#define HMATRIXB_PRAS_M6PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M6PR_Pos)
#define HMATRIXB_PRAS_M6PR(value)   (HMATRIXB_PRAS_M6PR_Msk & ((value) << HMATRIXB_PRAS_M6PR_Pos))
#define HMATRIXB_PRAS_M7PR_Pos      28           /**< \brief (HMATRIXB_PRAS) Master 7 Priority */
#define HMATRIXB_PRAS_M7PR_Msk      (_U_(0xF) << HMATRIXB_PRAS_M7PR_Pos)
#define HMATRIXB_PRAS_M7PR(value)   (HMATRIXB_PRAS_M7PR_Msk & ((value) << HMATRIXB_PRAS_M7PR_Pos))
#define HMATRIXB_PRAS_MASK          _U_(0xFFFFFFFF) /**< \brief (HMATRIXB_PRAS) MASK Register */

/* -------- HMATRIXB_PRBS : (HMATRIXB Offset: 0x084) (R/W 32) PRS Priority B for Slave -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t M8PR:4;           /*!< bit:  0.. 3  Master 8 Priority                  */
    uint32_t M9PR:4;           /*!< bit:  4.. 7  Master 9 Priority                  */
    uint32_t M10PR:4;          /*!< bit:  8..11  Master 10 Priority                 */
    uint32_t M11PR:4;          /*!< bit: 12..15  Master 11 Priority                 */
    uint32_t M12PR:4;          /*!< bit: 16..19  Master 12 Priority                 */
    uint32_t M13PR:4;          /*!< bit: 20..23  Master 13 Priority                 */
    uint32_t M14PR:4;          /*!< bit: 24..27  Master 14 Priority                 */
    uint32_t M15PR:4;          /*!< bit: 28..31  Master 15 Priority                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} HMATRIXB_PRBS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define HMATRIXB_PRBS_OFFSET        0x084        /**< \brief (HMATRIXB_PRBS offset) Priority B for Slave */
#define HMATRIXB_PRBS_RESETVALUE    _U_(0x00000000) /**< \brief (HMATRIXB_PRBS reset_value) Priority B for Slave */

#define HMATRIXB_PRBS_M8PR_Pos      0            /**< \brief (HMATRIXB_PRBS) Master 8 Priority */
#define HMATRIXB_PRBS_M8PR_Msk      (_U_(0xF) << HMATRIXB_PRBS_M8PR_Pos)
#define HMATRIXB_PRBS_M8PR(value)   (HMATRIXB_PRBS_M8PR_Msk & ((value) << HMATRIXB_PRBS_M8PR_Pos))
#define HMATRIXB_PRBS_M9PR_Pos      4            /**< \brief (HMATRIXB_PRBS) Master 9 Priority */
#define HMATRIXB_PRBS_M9PR_Msk      (_U_(0xF) << HMATRIXB_PRBS_M9PR_Pos)
#define HMATRIXB_PRBS_M9PR(value)   (HMATRIXB_PRBS_M9PR_Msk & ((value) << HMATRIXB_PRBS_M9PR_Pos))
#define HMATRIXB_PRBS_M10PR_Pos     8            /**< \brief (HMATRIXB_PRBS) Master 10 Priority */
#define HMATRIXB_PRBS_M10PR_Msk     (_U_(0xF) << HMATRIXB_PRBS_M10PR_Pos)
#define HMATRIXB_PRBS_M10PR(value)  (HMATRIXB_PRBS_M10PR_Msk & ((value) << HMATRIXB_PRBS_M10PR_Pos))
#define HMATRIXB_PRBS_M11PR_Pos     12           /**< \brief (HMATRIXB_PRBS) Master 11 Priority */
#define HMATRIXB_PRBS_M11PR_Msk     (_U_(0xF) << HMATRIXB_PRBS_M11PR_Pos)
#define HMATRIXB_PRBS_M11PR(value)  (HMATRIXB_PRBS_M11PR_Msk & ((value) << HMATRIXB_PRBS_M11PR_Pos))
#define HMATRIXB_PRBS_M12PR_Pos     16           /**< \brief (HMATRIXB_PRBS) Master 12 Priority */
#define HMATRIXB_PRBS_M12PR_Msk     (_U_(0xF) << HMATRIXB_PRBS_M12PR_Pos)
#define HMATRIXB_PRBS_M12PR(value)  (HMATRIXB_PRBS_M12PR_Msk & ((value) << HMATRIXB_PRBS_M12PR_Pos))
#define HMATRIXB_PRBS_M13PR_Pos     20           /**< \brief (HMATRIXB_PRBS) Master 13 Priority */
#define HMATRIXB_PRBS_M13PR_Msk     (_U_(0xF) << HMATRIXB_PRBS_M13PR_Pos)
#define HMATRIXB_PRBS_M13PR(value)  (HMATRIXB_PRBS_M13PR_Msk & ((value) << HMATRIXB_PRBS_M13PR_Pos))
#define HMATRIXB_PRBS_M14PR_Pos     24           /**< \brief (HMATRIXB_PRBS) Master 14 Priority */
#define HMATRIXB_PRBS_M14PR_Msk     (_U_(0xF) << HMATRIXB_PRBS_M14PR_Pos)
#define HMATRIXB_PRBS_M14PR(value)  (HMATRIXB_PRBS_M14PR_Msk & ((value) << HMATRIXB_PRBS_M14PR_Pos))
#define HMATRIXB_PRBS_M15PR_Pos     28           /**< \brief (HMATRIXB_PRBS) Master 15 Priority */
#define HMATRIXB_PRBS_M15PR_Msk     (_U_(0xF) << HMATRIXB_PRBS_M15PR_Pos)
#define HMATRIXB_PRBS_M15PR(value)  (HMATRIXB_PRBS_M15PR_Msk & ((value) << HMATRIXB_PRBS_M15PR_Pos))
#define HMATRIXB_PRBS_MASK          _U_(0xFFFFFFFF) /**< \brief (HMATRIXB_PRBS) MASK Register */

/* -------- HMATRIXB_SFR : (HMATRIXB Offset: 0x110) (R/W 32) Special Function -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SFR:32;           /*!< bit:  0..31  Special Function Register          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} HMATRIXB_SFR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define HMATRIXB_SFR_OFFSET         0x110        /**< \brief (HMATRIXB_SFR offset) Special Function */
#define HMATRIXB_SFR_RESETVALUE     _U_(0x00000000) /**< \brief (HMATRIXB_SFR reset_value) Special Function */

#define HMATRIXB_SFR_SFR_Pos        0            /**< \brief (HMATRIXB_SFR) Special Function Register */
#define HMATRIXB_SFR_SFR_Msk        (_U_(0xFFFFFFFF) << HMATRIXB_SFR_SFR_Pos)
#define HMATRIXB_SFR_SFR(value)     (HMATRIXB_SFR_SFR_Msk & ((value) << HMATRIXB_SFR_SFR_Pos))
#define HMATRIXB_SFR_MASK           _U_(0xFFFFFFFF) /**< \brief (HMATRIXB_SFR) MASK Register */

/** \brief HmatrixbPrs hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO HMATRIXB_PRAS_Type        PRAS;        /**< \brief Offset: 0x000 (R/W 32) Priority A for Slave */
  __IO HMATRIXB_PRBS_Type        PRBS;        /**< \brief Offset: 0x004 (R/W 32) Priority B for Slave */
} HmatrixbPrs;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief HMATRIXB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
       RoReg8                    Reserved1[0x80];
       HmatrixbPrs               Prs[16];     /**< \brief Offset: 0x080 HmatrixbPrs groups */
       RoReg8                    Reserved2[0x10];
  __IO HMATRIXB_SFR_Type         SFR[16];     /**< \brief Offset: 0x110 (R/W 32) Special Function */
} Hmatrixb;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD11_HMATRIXB_COMPONENT_ */
