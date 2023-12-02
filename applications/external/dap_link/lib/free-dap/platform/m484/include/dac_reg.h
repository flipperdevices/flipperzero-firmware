/**************************************************************************//**
 * @file     dac_reg.h
 * @version  V1.00
 * @brief    DAC register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __DAC_REG_H__
#define __DAC_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
  @{
*/

/**
    @addtogroup DAC Digital to Analog Converter(DAC)
    Memory Mapped Structure for DAC Controller
@{ */

typedef struct
{


    /**
     * @var DAC_T::CTL
     * Offset: 0x00  DAC Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |DACEN     |DAC Enable Bit
     * |        |          |0 = DAC is Disabled.
     * |        |          |1 = DAC is Enabled.
     * |[1]     |DACIEN    |DAC Interrupt Enable Bit
     * |        |          |0 = Interrupt is Disabled.
     * |        |          |1 = Interrupt is Enabled.
     * |[2]     |DMAEN     |DMA Mode Enable Bit
     * |        |          |0 = DMA mode Disabled.
     * |        |          |1 = DMA mode Enabled.
     * |[3]     |DMAURIEN  |DMA Under-run Interrupt Enable Bit
     * |        |          |0 = DMA under-run interrupt Disabled.
     * |        |          |1 = DMA under-run interrupt Enabled.
     * |[4]     |TRGEN     |Trigger Mode Enable Bit
     * |        |          |0 = DAC event trigger mode Disabled.
     * |        |          |1 = DAC event trigger mode Enabled.
     * |[7:5]   |TRGSEL    |Trigger Source Selection
     * |        |          |000 = Software trigger.
     * |        |          |001 = External pin DAC0_ST trigger.
     * |        |          |010 = Timer 0 trigger.
     * |        |          |011 = Timer 1 trigger.
     * |        |          |100 = Timer 2 trigger.
     * |        |          |101 = Timer 3 trigger.
     * |        |          |110 = EPWM0 trigger.
     * |        |          |111 = EPWM1 trigger.
     * |[8]     |BYPASS    |Bypass Buffer Mode
     * |        |          |0 = Output voltage buffer Enabled.
     * |        |          |1 = Output voltage buffer Disabled.
     * |[10]    |LALIGN    |DAC Data Left-aligned Enabled Control
     * |        |          |0 = Right alignment.
     * |        |          |1 = Left alignment.
     * |[13:12] |ETRGSEL   |External Pin Trigger Selection
     * |        |          |00 = Low level trigger.
     * |        |          |01 = High level trigger.
     * |        |          |10 = Falling edge trigger.
     * |        |          |11 = Rising edge trigger.
     * |[15:14] |BWSEL     |DAC Data Bit-width Selection
     * |        |          |00 = data is 12 bits.
     * |        |          |01 = data is 8 bits.
     * |        |          |Others = reserved.
     * |[16]    |GRPEN     |DAC Group Mode Enable Bit
     * |        |          |0 = DAC0 and DAC1 are not grouped.
     * |        |          |1 = DAC0 and DAC1 are grouped.
     * @var DAC_T::SWTRG
     * Offset: 0x04  DAC Software Trigger Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SWTRG     |Software Trigger
     * |        |          |0 = Software trigger Disabled.
     * |        |          |1 = Software trigger Enabled.
     * |        |          |User writes this bit to generate one shot pulse and it is cleared to 0 by hardware automatically; Reading this bit will always get 0.
     * @var DAC_T::DAT
     * Offset: 0x08  DAC Data Holding Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |DACDAT    |DAC 12-bit Holding Data
     * |        |          |These bits are written by user software which specifies 12-bit conversion data for DAC output
     * |        |          |The unused bits (DAC_DAT[3:0] in left-alignment mode and DAC_DAT[15:12] in right alignment mode) are ignored by DAC controller hardware.
     * |        |          |12 bit left alignment: user has to load data into DAC_DAT[15:4] bits.
     * |        |          |12 bit right alignment: user has to load data into DAC_DAT[11:0] bits.
     * @var DAC_T::DATOUT
     * Offset: 0x0C  DAC Data Output Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |DATOUT    |DAC 12-bit Output Data
     * |        |          |These bits are current digital data for DAC output conversion.
     * |        |          |It is loaded from DAC_DAT register and user cannot write it directly.
     * @var DAC_T::STATUS
     * Offset: 0x10  DAC Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FINISH    |DAC Conversion Complete Finish Flag
     * |        |          |0 = DAC is in conversion state.
     * |        |          |1 = DAC conversion finish.
     * |        |          |This bit set to 1 when conversion time counter counts to SETTLET
     * |        |          |It is cleared to 0 when DAC starts a new conversion
     * |        |          |User writes 1 to clear this bit to 0.
     * |[1]     |DMAUDR    |DMA Under-run Interrupt Flag
     * |        |          |0 = No DMA under-run error condition occurred.
     * |        |          |1 = DMA under-run error condition occurred.
     * |        |          |User writes 1 to clear this bit.
     * |[8]     |BUSY      |DAC Busy Flag (Read Only)
     * |        |          |0 = DAC is ready for next conversion.
     * |        |          |1 = DAC is busy in conversion.
     * |        |          |This is read only bit.
     * @var DAC_T::TCTL
     * Offset: 0x14  DAC Timing Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |SETTLET   |DAC Output Settling Time
     * |        |          |User software needs to write appropriate value to these bits to meet DAC conversion settling time base on PCLK (APB clock) speed.
     * |        |          |For example, DAC controller clock speed is 80MHz and DAC conversion settling time is 1 us, SETTLETvalue must be greater than 0x50.
     * |        |          |SELTTLET = DAC controller clock speed x settling time.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] DAC Control Register                                             */
    __IO uint32_t SWTRG;                 /*!< [0x0004] DAC Software Trigger Control Register                            */
    __IO uint32_t DAT;                   /*!< [0x0008] DAC Data Holding Register                                        */
    __I  uint32_t DATOUT;                /*!< [0x000c] DAC Data Output Register                                         */
    __IO uint32_t STATUS;                /*!< [0x0010] DAC Status Register                                              */
    __IO uint32_t TCTL;                  /*!< [0x0014] DAC Timing Control Register                                      */

} DAC_T;

/**
    @addtogroup DAC_CONST DAC Bit Field Definition
    Constant Definitions for DAC Controller
@{ */

#define DAC_CTL_DACEN_Pos                (0)                                               /*!< DAC_T::CTL: DACEN Position             */
#define DAC_CTL_DACEN_Msk                (0x1ul << DAC_CTL_DACEN_Pos)                      /*!< DAC_T::CTL: DACEN Mask                 */

#define DAC_CTL_DACIEN_Pos               (1)                                               /*!< DAC_T::CTL: DACIEN Position            */
#define DAC_CTL_DACIEN_Msk               (0x1ul << DAC_CTL_DACIEN_Pos)                     /*!< DAC_T::CTL: DACIEN Mask                */

#define DAC_CTL_DMAEN_Pos                (2)                                               /*!< DAC_T::CTL: DMAEN Position             */
#define DAC_CTL_DMAEN_Msk                (0x1ul << DAC_CTL_DMAEN_Pos)                      /*!< DAC_T::CTL: DMAEN Mask                 */

#define DAC_CTL_DMAURIEN_Pos             (3)                                               /*!< DAC_T::CTL: DMAURIEN Position          */
#define DAC_CTL_DMAURIEN_Msk             (0x1ul << DAC_CTL_DMAURIEN_Pos)                   /*!< DAC_T::CTL: DMAURIEN Mask              */

#define DAC_CTL_TRGEN_Pos                (4)                                               /*!< DAC_T::CTL: TRGEN Position             */
#define DAC_CTL_TRGEN_Msk                (0x1ul << DAC_CTL_TRGEN_Pos)                      /*!< DAC_T::CTL: TRGEN Mask                 */

#define DAC_CTL_TRGSEL_Pos               (5)                                               /*!< DAC_T::CTL: TRGSEL Position            */
#define DAC_CTL_TRGSEL_Msk               (0x7ul << DAC_CTL_TRGSEL_Pos)                     /*!< DAC_T::CTL: TRGSEL Mask                */

#define DAC_CTL_BYPASS_Pos               (8)                                               /*!< DAC_T::CTL: BYPASS Position            */
#define DAC_CTL_BYPASS_Msk               (0x1ul << DAC_CTL_BYPASS_Pos)                     /*!< DAC_T::CTL: BYPASS Mask                */

#define DAC_CTL_LALIGN_Pos               (10)                                              /*!< DAC_T::CTL: LALIGN Position            */
#define DAC_CTL_LALIGN_Msk               (0x1ul << DAC_CTL_LALIGN_Pos)                     /*!< DAC_T::CTL: LALIGN Mask                */

#define DAC_CTL_ETRGSEL_Pos              (12)                                              /*!< DAC_T::CTL: ETRGSEL Position           */
#define DAC_CTL_ETRGSEL_Msk              (0x3ul << DAC_CTL_ETRGSEL_Pos)                    /*!< DAC_T::CTL: ETRGSEL Mask               */

#define DAC_CTL_BWSEL_Pos                (14)                                              /*!< DAC_T::CTL: BWSEL Position             */
#define DAC_CTL_BWSEL_Msk                (0x3ul << DAC_CTL_BWSEL_Pos)                      /*!< DAC_T::CTL: BWSEL Mask                 */

#define DAC_CTL_GRPEN_Pos                (16)                                              /*!< DAC_T::CTL: GRPEN Position             */
#define DAC_CTL_GRPEN_Msk                (0x1ul << DAC_CTL_GRPEN_Pos)                      /*!< DAC_T::CTL: GRPEN Mask                 */

#define DAC_SWTRG_SWTRG_Pos              (0)                                               /*!< DAC_T::SWTRG: SWTRG Position           */
#define DAC_SWTRG_SWTRG_Msk              (0x1ul << DAC_SWTRG_SWTRG_Pos)                    /*!< DAC_T::SWTRG: SWTRG Mask               */

#define DAC_DAT_DACDAT_Pos               (0)                                               /*!< DAC_T::DAT: DACDAT Position            */
#define DAC_DAT_DACDAT_Msk               (0xfffful << DAC_DAT_DACDAT_Pos)                  /*!< DAC_T::DAT: DACDAT Mask                */

#define DAC_DATOUT_DATOUT_Pos            (0)                                               /*!< DAC_T::DATOUT: DATOUT Position         */
#define DAC_DATOUT_DATOUT_Msk            (0xffful << DAC_DATOUT_DATOUT_Pos)                /*!< DAC_T::DATOUT: DATOUT Mask             */

#define DAC_STATUS_FINISH_Pos            (0)                                               /*!< DAC_T::STATUS: FINISH Position         */
#define DAC_STATUS_FINISH_Msk            (0x1ul << DAC_STATUS_FINISH_Pos)                  /*!< DAC_T::STATUS: FINISH Mask             */

#define DAC_STATUS_DMAUDR_Pos            (1)                                               /*!< DAC_T::STATUS: DMAUDR Position         */
#define DAC_STATUS_DMAUDR_Msk            (0x1ul << DAC_STATUS_DMAUDR_Pos)                  /*!< DAC_T::STATUS: DMAUDR Mask             */

#define DAC_STATUS_BUSY_Pos              (8)                                               /*!< DAC_T::STATUS: BUSY Position           */
#define DAC_STATUS_BUSY_Msk              (0x1ul << DAC_STATUS_BUSY_Pos)                    /*!< DAC_T::STATUS: BUSY Mask               */

#define DAC_TCTL_SETTLET_Pos             (0)                                               /*!< DAC_T::TCTL: SETTLET Position          */
#define DAC_TCTL_SETTLET_Msk             (0x3fful << DAC_TCTL_SETTLET_Pos)                 /*!< DAC_T::TCTL: SETTLET Mask              */

/**@}*/ /* DAC_CONST */
/**@}*/ /* end of DAC register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __DAC_REG_H__ */
