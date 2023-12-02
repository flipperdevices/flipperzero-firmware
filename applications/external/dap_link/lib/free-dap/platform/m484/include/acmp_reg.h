/**************************************************************************//**
 * @file     acmp_reg.h
 * @version  V1.00
 * @brief    ACMP register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __ACMP_REG_H__
#define __ACMP_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup ACMP Analog Comparator Controller(ACMP)
    Memory Mapped Structure for ACMP Controller
@{ */

typedef struct
{


    /**
     * @var ACMP_T::CTL
     * Offset: 0x00~0x04  Analog Comparator 0/1 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ACMPEN    |Comparator Enable Bit
     * |        |          |0 = Comparator x Disabled.
     * |        |          |1 = Comparator x Enabled.
     * |[1]     |ACMPIE    |Comparator Interrupt Enable Bit
     * |        |          |0 = Comparator x interrupt Disabled.
     * |        |          |1 = Comparator x interrupt Enabled
     * |        |          |If WKEN (ACMP_CTL0[16]) is set to 1, the wake-up interrupt function will be enabled as well.
     * |[3]     |ACMPOINV  |Comparator Output Inverse
     * |        |          |0 = Comparator x output inverse Disabled.
     * |        |          |1 = Comparator x output inverse Enabled.
     * |[5:4]   |NEGSEL    |Comparator Negative Input Selection
     * |        |          |00 = ACMPx_N pin.
     * |        |          |01 = Internal comparator reference voltage (CRV).
     * |        |          |10 = Band-gap voltage.
     * |        |          |11 = DAC output.
     * |[7:6]   |POSSEL    |Comparator Positive Input Selection
     * |        |          |00 = Input from ACMPx_P0.
     * |        |          |01 = Input from ACMPx_P1.
     * |        |          |10 = Input from ACMPx_P2.
     * |        |          |11 = Input from ACMPx_P3.
     * |[9:8]   |INTPOL    |Interrupt Condition Polarity Selection
     * |        |          |ACMPIFx will be set to 1 when comparator output edge condition is detected.
     * |        |          |00 = Rising edge or falling edge.
     * |        |          |01 = Rising edge.
     * |        |          |10 = Falling edge.
     * |        |          |11 = Reserved.
     * |[12]    |OUTSEL    |Comparator Output Select
     * |        |          |0 = Comparator x output to ACMPx_O pin is unfiltered comparator output.
     * |        |          |1 = Comparator x output to ACMPx_O pin is from filter output.
     * |[15:13] |FILTSEL   |Comparator Output Filter Count Selection
     * |        |          |000 = Filter function is Disabled.
     * |        |          |001 = ACMPx output is sampled 1 consecutive PCLK.
     * |        |          |010 = ACMPx output is sampled 2 consecutive PCLKs.
     * |        |          |011 = ACMPx output is sampled 4 consecutive PCLKs.
     * |        |          |100 = ACMPx output is sampled 8 consecutive PCLKs.
     * |        |          |101 = ACMPx output is sampled 16 consecutive PCLKs.
     * |        |          |110 = ACMPx output is sampled 32 consecutive PCLKs.
     * |        |          |111 = ACMPx output is sampled 64 consecutive PCLKs.
     * |[16]    |WKEN      |Power-down Wake-up Enable Bit
     * |        |          |0 = Wake-up function Disabled.
     * |        |          |1 = Wake-up function Enabled.
     * |[17]    |WLATEN    |Window Latch Mode Enable Bit
     * |        |          |0 = Window Latch Mode Disabled.
     * |        |          |1 = Window Latch Mode Enabled.
     * |[18]    |WCMPSEL   |Window Compare Mode Selection
     * |        |          |0 = Window Compare Mode Disabled.
     * |        |          |1 = Window Compare Mode is Selected.
     * |[25:24] |HYSSEL    |Hysteresis Mode Selection
     * |        |          |00 = Hysteresis is 0mV.
     * |        |          |01 = Hysteresis is 10mV.
     * |        |          |10 = Hysteresis is 20mV.
     * |        |          |11 = Hysteresis is 30mV.
     * |[29:28] |MODESEL   |Propagation Delay Mode Selection
     * |        |          |00 = Max propagation delay is 4.5uS, operation current is 1.2uA.
     * |        |          |01 = Max propagation delay is 2uS, operation current is 3uA.
     * |        |          |10 = Max propagation delay is 600nS, operation current is 10uA.
     * |        |          |11 = Max propagation delay is 200nS, operation current is 75uA.
     * @var ACMP_T::STATUS
     * Offset: 0x08  Analog Comparator Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ACMPIF0   |Comparator 0 Interrupt Flag
     * |        |          |This bit is set by hardware when the edge condition defined by INTPOL (ACMP_CTL0[9:8])
     * |        |          |is detected on comparator 0 output.
     * |        |          |This will generate an interrupt if ACMPIE (ACMP_CTL0[1]) is set to 1.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[1]     |ACMPIF1   |Comparator 1 Interrupt Flag
     * |        |          |This bit is set by hardware when the edge condition defined by INTPOL (ACMP_CTL1[9:8])
     * |        |          |is detected on comparator 1 output.
     * |        |          |This will cause an interrupt if ACMPIE (ACMP_CTL1[1]) is set to 1.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[4]     |ACMPO0    |Comparator 0 Output
     * |        |          |Synchronized to the PCLK to allow reading by software
     * |        |          |Cleared when the comparator 0 is disabled, i.e.
     * |        |          |ACMPEN (ACMP_CTL0[0]) is cleared to 0.
     * |[5]     |ACMPO1    |Comparator 1 Output
     * |        |          |Synchronized to the PCLK to allow reading by software.
     * |        |          |Cleared when the comparator 1 is disabled, i.e.
     * |        |          |ACMPEN (ACMP_CTL1[0]) is cleared to 0.
     * |[8]     |WKIF0     |Comparator 0 Power-down Wake-up Interrupt Flag
     * |        |          |This bit will be set to 1 when ACMP0 wake-up interrupt event occurs.
     * |        |          |0 = No power-down wake-up occurred.
     * |        |          |1 = Power-down wake-up occurred.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[9]     |WKIF1     |Comparator 1 Power-down Wake-up Interrupt Flag
     * |        |          |This bit will be set to 1 when ACMP1 wake-up interrupt event occurs.
     * |        |          |0 = No power-down wake-up occurred.
     * |        |          |1 = Power-down wake-up occurred.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[12]    |ACMPS0    |Comparator 0 Status
     * |        |          |Synchronized to the PCLK to allow reading by software
     * |        |          |Cleared when the comparator 0 is disabled, i.e.
     * |        |          |ACMPEN (ACMP_CTL0[0]) is cleared to 0.
     * |[13]    |ACMPS1    |Comparator 1 Status
     * |        |          |Synchronized to the PCLK to allow reading by software
     * |        |          |Cleared when the comparator 1 is disabled, i.e.
     * |        |          |ACMPEN (ACMP_CTL1[0]) is cleared to 0.
     * |[16]    |ACMPWO    |Comparator Window Output
     * |        |          |This bit shows the output status of window compare mode
     * |        |          |0 = The positive input voltage is outside the window.
     * |        |          |1 = The positive input voltage is in the window.
     * @var ACMP_T::VREF
     * Offset: 0x0C  Analog Comparator Reference Voltage Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |CRVCTL    |Comparator Reference Voltage Setting
     * |        |          |CRV = CRV source voltage * (1/6+CRVCTL/24).
     * |[6]     |CRVSSEL   |CRV Source Voltage Selection
     * |        |          |0 = VDDA is selected as CRV source voltage.
     * |        |          |1 = The reference voltage defined by SYS_VREFCTL register is selected as CRV source voltage.
     */
    __IO uint32_t CTL[2];                /*!< [0x0000~0x0004] Analog Comparator 0/1 Control Register                    */
    __IO uint32_t STATUS;                /*!< [0x0008] Analog Comparator Status Register                                */
    __IO uint32_t VREF;                  /*!< [0x000c] Analog Comparator Reference Voltage Control Register             */

} ACMP_T;

/**
    @addtogroup ACMP_CONST ACMP Bit Field Definition
    Constant Definitions for ACMP Controller
@{ */

#define ACMP_CTL_ACMPEN_Pos              (0)                                               /*!< ACMP_T::CTL: ACMPEN Position           */
#define ACMP_CTL_ACMPEN_Msk              (0x1ul << ACMP_CTL_ACMPEN_Pos)                    /*!< ACMP_T::CTL: ACMPEN Mask               */

#define ACMP_CTL_ACMPIE_Pos              (1)                                               /*!< ACMP_T::CTL: ACMPIE Position           */
#define ACMP_CTL_ACMPIE_Msk              (0x1ul << ACMP_CTL_ACMPIE_Pos)                    /*!< ACMP_T::CTL: ACMPIE Mask               */

#define ACMP_CTL_ACMPOINV_Pos            (3)                                               /*!< ACMP_T::CTL: ACMPOINV Position         */
#define ACMP_CTL_ACMPOINV_Msk            (0x1ul << ACMP_CTL_ACMPOINV_Pos)                  /*!< ACMP_T::CTL: ACMPOINV Mask             */

#define ACMP_CTL_NEGSEL_Pos              (4)                                               /*!< ACMP_T::CTL: NEGSEL Position           */
#define ACMP_CTL_NEGSEL_Msk              (0x3ul << ACMP_CTL_NEGSEL_Pos)                    /*!< ACMP_T::CTL: NEGSEL Mask               */

#define ACMP_CTL_POSSEL_Pos              (6)                                               /*!< ACMP_T::CTL: POSSEL Position           */
#define ACMP_CTL_POSSEL_Msk              (0x3ul << ACMP_CTL_POSSEL_Pos)                    /*!< ACMP_T::CTL: POSSEL Mask               */

#define ACMP_CTL_INTPOL_Pos              (8)                                               /*!< ACMP_T::CTL: INTPOL Position           */
#define ACMP_CTL_INTPOL_Msk              (0x3ul << ACMP_CTL_INTPOL_Pos)                    /*!< ACMP_T::CTL: INTPOL Mask               */

#define ACMP_CTL_OUTSEL_Pos              (12)                                              /*!< ACMP_T::CTL: OUTSEL Position           */
#define ACMP_CTL_OUTSEL_Msk              (0x1ul << ACMP_CTL_OUTSEL_Pos)                    /*!< ACMP_T::CTL: OUTSEL Mask               */

#define ACMP_CTL_FILTSEL_Pos             (13)                                              /*!< ACMP_T::CTL: FILTSEL Position          */
#define ACMP_CTL_FILTSEL_Msk             (0x7ul << ACMP_CTL_FILTSEL_Pos)                   /*!< ACMP_T::CTL: FILTSEL Mask              */

#define ACMP_CTL_WKEN_Pos                (16)                                              /*!< ACMP_T::CTL: WKEN Position             */
#define ACMP_CTL_WKEN_Msk                (0x1ul << ACMP_CTL_WKEN_Pos)                      /*!< ACMP_T::CTL: WKEN Mask                 */

#define ACMP_CTL_WLATEN_Pos              (17)                                              /*!< ACMP_T::CTL: WLATEN Position           */
#define ACMP_CTL_WLATEN_Msk              (0x1ul << ACMP_CTL_WLATEN_Pos)                    /*!< ACMP_T::CTL: WLATEN Mask               */

#define ACMP_CTL_WCMPSEL_Pos             (18)                                              /*!< ACMP_T::CTL: WCMPSEL Position          */
#define ACMP_CTL_WCMPSEL_Msk             (0x1ul << ACMP_CTL_WCMPSEL_Pos)                   /*!< ACMP_T::CTL: WCMPSEL Mask              */

#define ACMP_CTL_HYSSEL_Pos              (24)                                              /*!< ACMP_T::CTL: HYSSEL Position           */
#define ACMP_CTL_HYSSEL_Msk              (0x3ul << ACMP_CTL_HYSSEL_Pos)                    /*!< ACMP_T::CTL: HYSSEL Mask               */

#define ACMP_CTL_MODESEL_Pos             (28)                                              /*!< ACMP_T::CTL: MODESEL Position          */
#define ACMP_CTL_MODESEL_Msk             (0x3ul << ACMP_CTL_MODESEL_Pos)                   /*!< ACMP_T::CTL: MODESEL Mask              */

#define ACMP_STATUS_ACMPIF0_Pos          (0)                                               /*!< ACMP_T::STATUS: ACMPIF0 Position       */
#define ACMP_STATUS_ACMPIF0_Msk          (0x1ul << ACMP_STATUS_ACMPIF0_Pos)                /*!< ACMP_T::STATUS: ACMPIF0 Mask           */

#define ACMP_STATUS_ACMPIF1_Pos          (1)                                               /*!< ACMP_T::STATUS: ACMPIF1 Position       */
#define ACMP_STATUS_ACMPIF1_Msk          (0x1ul << ACMP_STATUS_ACMPIF1_Pos)                /*!< ACMP_T::STATUS: ACMPIF1 Mask           */

#define ACMP_STATUS_ACMPO0_Pos           (4)                                               /*!< ACMP_T::STATUS: ACMPO0 Position        */
#define ACMP_STATUS_ACMPO0_Msk           (0x1ul << ACMP_STATUS_ACMPO0_Pos)                 /*!< ACMP_T::STATUS: ACMPO0 Mask            */

#define ACMP_STATUS_ACMPO1_Pos           (5)                                               /*!< ACMP_T::STATUS: ACMPO1 Position        */
#define ACMP_STATUS_ACMPO1_Msk           (0x1ul << ACMP_STATUS_ACMPO1_Pos)                 /*!< ACMP_T::STATUS: ACMPO1 Mask            */

#define ACMP_STATUS_WKIF0_Pos            (8)                                               /*!< ACMP_T::STATUS: WKIF0 Position         */
#define ACMP_STATUS_WKIF0_Msk            (0x1ul << ACMP_STATUS_WKIF0_Pos)                  /*!< ACMP_T::STATUS: WKIF0 Mask             */

#define ACMP_STATUS_WKIF1_Pos            (9)                                               /*!< ACMP_T::STATUS: WKIF1 Position         */
#define ACMP_STATUS_WKIF1_Msk            (0x1ul << ACMP_STATUS_WKIF1_Pos)                  /*!< ACMP_T::STATUS: WKIF1 Mask             */

#define ACMP_STATUS_ACMPS0_Pos           (12)                                              /*!< ACMP_T::STATUS: ACMPS0 Position        */
#define ACMP_STATUS_ACMPS0_Msk           (0x1ul << ACMP_STATUS_ACMPS0_Pos)                 /*!< ACMP_T::STATUS: ACMPS0 Mask            */

#define ACMP_STATUS_ACMPS1_Pos           (13)                                              /*!< ACMP_T::STATUS: ACMPS1 Position        */
#define ACMP_STATUS_ACMPS1_Msk           (0x1ul << ACMP_STATUS_ACMPS1_Pos)                 /*!< ACMP_T::STATUS: ACMPS1 Mask            */

#define ACMP_STATUS_ACMPWO_Pos           (16)                                              /*!< ACMP_T::STATUS: ACMPWO Position        */
#define ACMP_STATUS_ACMPWO_Msk           (0x1ul << ACMP_STATUS_ACMPWO_Pos)                 /*!< ACMP_T::STATUS: ACMPWO Mask            */

#define ACMP_VREF_CRVCTL_Pos             (0)                                               /*!< ACMP_T::VREF: CRVCTL Position          */
#define ACMP_VREF_CRVCTL_Msk             (0xful << ACMP_VREF_CRVCTL_Pos)                   /*!< ACMP_T::VREF: CRVCTL Mask              */

#define ACMP_VREF_CRVSSEL_Pos            (6)                                               /*!< ACMP_T::VREF: CRVSSEL Position         */
#define ACMP_VREF_CRVSSEL_Msk            (0x1ul << ACMP_VREF_CRVSSEL_Pos)                  /*!< ACMP_T::VREF: CRVSSEL Mask             */

/**@}*/ /* ACMP_CONST */
/**@}*/ /* end of ACMP register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __ACMP_REG_H__ */
