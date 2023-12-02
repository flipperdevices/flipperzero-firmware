/**************************************************************************//**
 * @file     qei_reg.h
 * @version  V1.00
 * @brief    QEI register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __QEI_REG_H__
#define __QEI_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup QEI Quadrature Encoder Interface(QEI)
    Memory Mapped Structure for QEI Controller
@{ */

typedef struct
{


    /**
     * @var QEI_T::CNT
     * Offset: 0x00  QEI Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |Quadrature Encoder Interface Counter
     * |        |          |A 32-bit up/down counter
     * |        |          |When an effective phase pulse is detected, this counter is increased by one if the bit DIRF (QEI_STATUS[8]) is one or decreased by one if the bit DIRF(QEI_STATUS[8]) is zero
     * |        |          |This register performs an integrator which count value is proportional to the encoder position
     * |        |          |The pulse counter may be initialized to a predetermined value by one of three events occurs:
     * |        |          |1. Software is written if QEIEN (QEI_CTL[29]) = 0.
     * |        |          |2. Compare-match event if QEIEN(QEI_CTL[29])=1 and QEI is in compare-counting mode.
     * |        |          |3. Index signal change if QEIEN(QEI_CTL[29])=1 and IDXRLDEN (QEI_CTL[27])=1.
     * @var QEI_T::CNTHOLD
     * Offset: 0x04  QEI Counter Hold Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNTHOLD   |Quadrature Encoder Interface Counter Hold
     * |        |          |When bit HOLDCNT (QEI_CTL[24]) goes from low to high, the CNT(QEI_CNT[31:0]) is copied into CNTHOLD (QEI_CNTHOLD[31:0]) register.
     * @var QEI_T::CNTLATCH
     * Offset: 0x08  QEI Counter Index Latch Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNTLATCH  |Quadrature Encoder Interface Counter Index Latch
     * |        |          |When the IDXF (QEI_STATUS[0]) bit is set, the CNT(QEI_CNT[31:0]) is copied into CNTLATCH (QEI_CNTLATCH[31:0]) register.
     * @var QEI_T::CNTCMP
     * Offset: 0x0C  QEI Counter Compare Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNTCMP    |Quadrature Encoder Interface Counter Compare
     * |        |          |If the QEI controller is in the compare-counting mode CMPEN (QEI_CTL[28]) =1, when the value of CNT(QEI_CNT[31:0]) matches CNTCMP(QEI_CNTCMP[31:0]), CMPF will be set
     * |        |          |This register is software writable.
     * @var QEI_T::CNTMAX
     * Offset: 0x14  QEI Pre-set Maximum Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNTMAX    |Quadrature Encoder Interface Preset Maximum Count
     * |        |          |This register value determined by user stores the maximum value which may be the number of the QEI counter for the QEI controller compare-counting mode
     * @var QEI_T::CTL
     * Offset: 0x18  QEI Controller Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |NFCLKSEL  |Noise Filter Clock Pre-divide Selection
     * |        |          |To determine the sampling frequency of the Noise Filter clock .
     * |        |          |000 = QEI_CLK.
     * |        |          |001 = QEI_CLK/2.
     * |        |          |010 = QEI_CLK/4.
     * |        |          |011 = QEI_CLK/16.
     * |        |          |100 = QEI_CLK/32.
     * |        |          |101 = QEI_CLK/64.
     * |[3]     |NFDIS     |QEI Controller Input Noise Filter Disable Bit
     * |        |          |0 = The noise filter of QEI controller Enabled.
     * |        |          |1 = The noise filter of QEI controller Disabled.
     * |[4]     |CHAEN     |QEA Input to QEI Controller Enable Bit
     * |        |          |0 = QEA input to QEI Controller Disabled.
     * |        |          |1 = QEA input to QEI Controller Enabled.
     * |[5]     |CHBEN     |QEB Input to QEI Controller Enable Bit
     * |        |          |0 = QEB input to QEI Controller Disabled.
     * |        |          |1 = QEB input to QEI Controller Enabled.
     * |[6]     |IDXEN     |IDX Input to QEI Controller Enable Bit
     * |        |          |0 = IDX input to QEI Controller Disabled.
     * |        |          |1 = IDX input to QEI Controller Enabled.
     * |[9:8]   |MODE      |QEI Counting Mode Selection
     * |        |          |There are four quadrature encoder pulse counter operation modes.
     * |        |          |00 = X4 Free-counting Mode.
     * |        |          |01 = X2 Free-counting Mode.
     * |        |          |10 = X4 Compare-counting Mode.
     * |        |          |11 = X2 Compare-counting Mode.
     * |[12]    |CHAINV    |Inverse QEA Input Polarity
     * |        |          |0 = Not inverse QEA input polarity.
     * |        |          |1 = QEA input polarity is inversed to QEI controller.
     * |[13]    |CHBINV    |Inverse QEB Input Polarity
     * |        |          |0 = Not inverse QEB input polarity.
     * |        |          |1 = QEB input polarity is inversed to QEI controller.
     * |[14]    |IDXINV    |Inverse IDX Input Polarity
     * |        |          |0 = Not inverse IDX input polarity.
     * |        |          |1 = IDX input polarity is inversed to QEI controller.
     * |[16]    |OVUNIEN   |OVUNF Trigger QEI Interrupt Enable Bit
     * |        |          |0 = OVUNF can trigger QEI controller interrupt Disabled.
     * |        |          |1 = OVUNF can trigger QEI controller interrupt Enabled.
     * |[17]    |DIRIEN    |DIRCHGF Trigger QEI Interrupt Enable Bit
     * |        |          |0 = DIRCHGF can trigger QEI controller interrupt Disabled.
     * |        |          |1 = DIRCHGF can trigger QEI controller interrupt Enabled.
     * |[18]    |CMPIEN    |CMPF Trigger QEI Interrupt Enable Bit
     * |        |          |0 = CMPF can trigger QEI controller interrupt Disabled.
     * |        |          |1 = CMPF can trigger QEI controller interrupt Enabled.
     * |[19]    |IDXIEN    |IDXF Trigger QEI Interrupt Enable Bit
     * |        |          |0 = The IDXF can trigger QEI interrupt Disabled.
     * |        |          |1 = The IDXF can trigger QEI interrupt Enabled.
     * |[20]    |HOLDTMR0  |Hold QEI_CNT by Timer 0
     * |        |          |0 = TIF (TIMER0_INTSTS[0]) has no effect on HOLDCNT.
     * |        |          |1 = A rising edge of bit TIF(TIMER0_INTSTS[0]) in timer 0 sets HOLDCNT to 1.
     * |[21]    |HOLDTMR1  |Hold QEI_CNT by Timer 1
     * |        |          |0 = TIF(TIMER1_INTSTS[0]) has no effect on HOLDCNT.
     * |        |          |1 = A rising edge of bit TIF (TIMER1_INTSTS[0]) in timer 1 sets HOLDCNT to 1.
     * |[22]    |HOLDTMR2  |Hold QEI_CNT by Timer 2
     * |        |          |0 = TIF(TIMER2_INTSTS[0]) has no effect on HOLDCNT.
     * |        |          |1 = A rising edge of bit TIF(TIMER2_INTSTS[0]) in timer 2 sets HOLDCNT to 1.
     * |[23]    |HOLDTMR3  |Hold QEI_CNT by Timer 3
     * |        |          |0 = TIF (TIMER3_INTSTS[0]) has no effect on HOLDCNT.
     * |        |          |1 = A rising edge of bit TIF(TIMER3_INTSTS[0]) in timer 3 sets HOLDCNT to 1.
     * |[24]    |HOLDCNT   |Hold QEI_CNT Control
     * |        |          |When this bit is set from low to high, the CNT(QEI_CNT[31:0]) is copied into CNTHOLD(QEI_CNTHOLD[31:0])
     * |        |          |This bit may be set by writing 1 to it or Timer0~Timer3 interrupt flag TIF (TIMERx_INTSTS[0]).
     * |        |          |0 = No operation.
     * |        |          |1 = QEI_CNT content is captured and stored in CNTHOLD(QEI_CNTHOLD[31:0]).
     * |        |          |Note: This bit is automatically cleared after QEI_CNTHOLD holds QEI_CNT value.
     * |[25]    |IDXLATEN  |Index Latch QEI_CNT Enable Bit
     * |        |          |If this bit is set to high, the CNT(QEI_CNT[31:0]) content will be latched into CNTLATCH (QEI_CNTLATCH[31:0]) at every rising on signal CHX.
     * |        |          |0 = The index signal latch QEI counter function Disabled.
     * |        |          |1 = The index signal latch QEI counter function Enabled.
     * |[27]    |IDXRLDEN  |Index Trigger QEI_CNT Reload Enable Bit
     * |        |          |When this bit is high and a rising edge comes on signal CHX, the CNT(QEI_CNT[31:0]) will be reset to zero if the counter is in up-counting type (DIRF(QEI_STATUS[8]) = 1); while the CNT(QEI_CNT[31:0]) will be reloaded with CNTMAX (QEI_CNTMAX[31:0]) content if the counter is in down-counting type (DIRF(QEI_STATUS[8]) = 0).
     * |        |          |0 = Reload function Disabled.
     * |        |          |1 = QEI_CNT re-initialized by Index signal Enabled.
     * |[28]    |CMPEN     |The Compare Function Enable Bit
     * |        |          |The compare function in QEI controller is to compare the dynamic counting QEI_CNT with the compare register CNTCMP( QEI_CNTCMP[31:0]), if CNT(QEI_CNT[31:0]) reaches CNTCMP( QEI_CNTCMP[31:0]), the flag CMPF will be set.
     * |        |          |0 = Compare function Disabled.
     * |        |          |1 = Compare function Enabled.
     * |[29]    |QEIEN     |Quadrature Encoder Interface Controller Enable Bit
     * |        |          |0 = QEI controller function Disabled.
     * |        |          |1 = QEI controller function Enabled.
     * @var QEI_T::STATUS
     * Offset: 0x2C  QEI Controller Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |IDXF      |IDX Detected Flag
     * |        |          |When the QEI controller detects a rising edge on signal CHX it will set flag IDXF to high.
     * |        |          |0 = No rising edge detected on signal CHX.
     * |        |          |1 = A rising edge occurs on signal CHX.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[1]     |CMPF      |Compare-match Flag
     * |        |          |If the QEI compare function is enabled, the flag is set by hardware while QEI counter up or down counts and reach to the CNTCMP(QEI_CNTCMP[31:0]).
     * |        |          |0 = QEI counter does not match with CNTCMP(QEI_CNTCMP[31:0]).
     * |        |          |1 = QEI counter counts to the same as CNTCMP(QEI_CNTCMP[31:0]).
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[2]     |OVUNF     |QEI Counter Overflow or Underflow Flag
     * |        |          |Flag is set by hardware while CNT(QEI_CNT[31:0]) overflows from 0xFFFF_FFFF to zero in free-counting mode or from the CNTMAX (QEI_CNTMAX[31:0]) to zero in compare-counting mode
    * |        |          |Similarly, the flag is set while QEI counter underflows from zero to 0xFFFF_FFFF or CNTMAX (QEI_CNTMAX[31:0]).
     * |        |          |0 = No overflow or underflow occurs in QEI counter.
     * |        |          |1 = QEI counter occurs counting overflow or underflow.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[3]     |DIRCHGF   |Direction Change Flag
     * |        |          |Flag is set by hardware while QEI counter counting direction is changed.
     * |        |          |Software can clear this bit by writing 1 to it.
     * |        |          |0 = No change in QEI counter counting direction.
     * |        |          |1 = QEI counter counting direction is changed.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[8]     |DIRF      |QEI Counter Counting Direction Indication
     * |        |          |0 = QEI Counter is in down-counting.
     * |        |          |1 = QEI Counter is in up-counting.
     * |        |          |Note: This bit is set/reset by hardware according to the phase detection between CHA and CHB.
     */
    __IO uint32_t CNT;                   /*!< [0x0000] QEI Counter Register                                             */
    __IO uint32_t CNTHOLD;               /*!< [0x0004] QEI Counter Hold Register                                        */
    __IO uint32_t CNTLATCH;              /*!< [0x0008] QEI Counter Index Latch Register                                 */
    __IO uint32_t CNTCMP;                /*!< [0x000c] QEI Counter Compare Register                                     */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CNTMAX;                /*!< [0x0014] QEI Pre-set Maximum Count Register                               */
    __IO uint32_t CTL;                   /*!< [0x0018] QEI Controller Control Register                                  */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[4];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t STATUS;                /*!< [0x002c] QEI Controller Status Register                                   */

} QEI_T;

/**
    @addtogroup QEI_CONST QEI Bit Field Definition
    Constant Definitions for QEI Controller
@{ */

#define QEI_CNT_CNT_Pos                  (0)                                               /*!< QEI_T::CNT: CNT Position               */
#define QEI_CNT_CNT_Msk                  (0xfffffffful << QEI_CNT_CNT_Pos)                 /*!< QEI_T::CNT: CNT Mask                   */

#define QEI_CNTHOLD_CNTHOLD_Pos          (0)                                               /*!< QEI_T::CNTHOLD: CNTHOLD Position       */
#define QEI_CNTHOLD_CNTHOLD_Msk          (0xfffffffful << QEI_CNTHOLD_CNTHOLD_Pos)         /*!< QEI_T::CNTHOLD: CNTHOLD Mask           */

#define QEI_CNTLATCH_CNTLATCH_Pos        (0)                                               /*!< QEI_T::CNTLATCH: CNTLATCH Position     */
#define QEI_CNTLATCH_CNTLATCH_Msk        (0xfffffffful << QEI_CNTLATCH_CNTLATCH_Pos)       /*!< QEI_T::CNTLATCH: CNTLATCH Mask         */

#define QEI_CNTCMP_CNTCMP_Pos            (0)                                               /*!< QEI_T::CNTCMP: CNTCMP Position         */
#define QEI_CNTCMP_CNTCMP_Msk            (0xfffffffful << QEI_CNTCMP_CNTCMP_Pos)           /*!< QEI_T::CNTCMP: CNTCMP Mask             */

#define QEI_CNTMAX_CNTMAX_Pos            (0)                                               /*!< QEI_T::CNTMAX: CNTMAX Position         */
#define QEI_CNTMAX_CNTMAX_Msk            (0xfffffffful << QEI_CNTMAX_CNTMAX_Pos)           /*!< QEI_T::CNTMAX: CNTMAX Mask             */

#define QEI_CTL_NFCLKSEL_Pos             (0)                                               /*!< QEI_T::CTL: NFCLKSEL Position          */
#define QEI_CTL_NFCLKSEL_Msk             (0x7ul << QEI_CTL_NFCLKSEL_Pos)                   /*!< QEI_T::CTL: NFCLKSEL Mask              */

#define QEI_CTL_NFDIS_Pos                (3)                                               /*!< QEI_T::CTL: NFDIS Position             */
#define QEI_CTL_NFDIS_Msk                (0x1ul << QEI_CTL_NFDIS_Pos)                      /*!< QEI_T::CTL: NFDIS Mask                 */

#define QEI_CTL_CHAEN_Pos                (4)                                               /*!< QEI_T::CTL: CHAEN Position             */
#define QEI_CTL_CHAEN_Msk                (0x1ul << QEI_CTL_CHAEN_Pos)                      /*!< QEI_T::CTL: CHAEN Mask                 */

#define QEI_CTL_CHBEN_Pos                (5)                                               /*!< QEI_T::CTL: CHBEN Position             */
#define QEI_CTL_CHBEN_Msk                (0x1ul << QEI_CTL_CHBEN_Pos)                      /*!< QEI_T::CTL: CHBEN Mask                 */

#define QEI_CTL_IDXEN_Pos                (6)                                               /*!< QEI_T::CTL: IDXEN Position             */
#define QEI_CTL_IDXEN_Msk                (0x1ul << QEI_CTL_IDXEN_Pos)                      /*!< QEI_T::CTL: IDXEN Mask                 */

#define QEI_CTL_MODE_Pos                 (8)                                               /*!< QEI_T::CTL: MODE Position              */
#define QEI_CTL_MODE_Msk                 (0x3ul << QEI_CTL_MODE_Pos)                       /*!< QEI_T::CTL: MODE Mask                  */

#define QEI_CTL_CHAINV_Pos               (12)                                              /*!< QEI_T::CTL: CHAINV Position            */
#define QEI_CTL_CHAINV_Msk               (0x1ul << QEI_CTL_CHAINV_Pos)                     /*!< QEI_T::CTL: CHAINV Mask                */

#define QEI_CTL_CHBINV_Pos               (13)                                              /*!< QEI_T::CTL: CHBINV Position            */
#define QEI_CTL_CHBINV_Msk               (0x1ul << QEI_CTL_CHBINV_Pos)                     /*!< QEI_T::CTL: CHBINV Mask                */

#define QEI_CTL_IDXINV_Pos               (14)                                              /*!< QEI_T::CTL: IDXINV Position            */
#define QEI_CTL_IDXINV_Msk               (0x1ul << QEI_CTL_IDXINV_Pos)                     /*!< QEI_T::CTL: IDXINV Mask                */

#define QEI_CTL_OVUNIEN_Pos              (16)                                              /*!< QEI_T::CTL: OVUNIEN Position           */
#define QEI_CTL_OVUNIEN_Msk              (0x1ul << QEI_CTL_OVUNIEN_Pos)                    /*!< QEI_T::CTL: OVUNIEN Mask               */

#define QEI_CTL_DIRIEN_Pos               (17)                                              /*!< QEI_T::CTL: DIRIEN Position            */
#define QEI_CTL_DIRIEN_Msk               (0x1ul << QEI_CTL_DIRIEN_Pos)                     /*!< QEI_T::CTL: DIRIEN Mask                */

#define QEI_CTL_CMPIEN_Pos               (18)                                              /*!< QEI_T::CTL: CMPIEN Position            */
#define QEI_CTL_CMPIEN_Msk               (0x1ul << QEI_CTL_CMPIEN_Pos)                     /*!< QEI_T::CTL: CMPIEN Mask                */

#define QEI_CTL_IDXIEN_Pos               (19)                                              /*!< QEI_T::CTL: IDXIEN Position            */
#define QEI_CTL_IDXIEN_Msk               (0x1ul << QEI_CTL_IDXIEN_Pos)                     /*!< QEI_T::CTL: IDXIEN Mask                */

#define QEI_CTL_HOLDTMR0_Pos             (20)                                              /*!< QEI_T::CTL: HOLDTMR0 Position          */
#define QEI_CTL_HOLDTMR0_Msk             (0x1ul << QEI_CTL_HOLDTMR0_Pos)                   /*!< QEI_T::CTL: HOLDTMR0 Mask              */

#define QEI_CTL_HOLDTMR1_Pos             (21)                                              /*!< QEI_T::CTL: HOLDTMR1 Position          */
#define QEI_CTL_HOLDTMR1_Msk             (0x1ul << QEI_CTL_HOLDTMR1_Pos)                   /*!< QEI_T::CTL: HOLDTMR1 Mask              */

#define QEI_CTL_HOLDTMR2_Pos             (22)                                              /*!< QEI_T::CTL: HOLDTMR2 Position          */
#define QEI_CTL_HOLDTMR2_Msk             (0x1ul << QEI_CTL_HOLDTMR2_Pos)                   /*!< QEI_T::CTL: HOLDTMR2 Mask              */

#define QEI_CTL_HOLDTMR3_Pos             (23)                                              /*!< QEI_T::CTL: HOLDTMR3 Position          */
#define QEI_CTL_HOLDTMR3_Msk             (0x1ul << QEI_CTL_HOLDTMR3_Pos)                   /*!< QEI_T::CTL: HOLDTMR3 Mask              */

#define QEI_CTL_HOLDCNT_Pos              (24)                                              /*!< QEI_T::CTL: HOLDCNT Position           */
#define QEI_CTL_HOLDCNT_Msk              (0x1ul << QEI_CTL_HOLDCNT_Pos)                    /*!< QEI_T::CTL: HOLDCNT Mask               */

#define QEI_CTL_IDXLATEN_Pos             (25)                                              /*!< QEI_T::CTL: IDXLATEN Position          */
#define QEI_CTL_IDXLATEN_Msk             (0x1ul << QEI_CTL_IDXLATEN_Pos)                   /*!< QEI_T::CTL: IDXLATEN Mask              */

#define QEI_CTL_IDXRLDEN_Pos             (27)                                              /*!< QEI_T::CTL: IDXRLDEN Position          */
#define QEI_CTL_IDXRLDEN_Msk             (0x1ul << QEI_CTL_IDXRLDEN_Pos)                   /*!< QEI_T::CTL: IDXRLDEN Mask              */

#define QEI_CTL_CMPEN_Pos                (28)                                              /*!< QEI_T::CTL: CMPEN Position             */
#define QEI_CTL_CMPEN_Msk                (0x1ul << QEI_CTL_CMPEN_Pos)                      /*!< QEI_T::CTL: CMPEN Mask                 */

#define QEI_CTL_QEIEN_Pos                (29)                                              /*!< QEI_T::CTL: QEIEN Position             */
#define QEI_CTL_QEIEN_Msk                (0x1ul << QEI_CTL_QEIEN_Pos)                      /*!< QEI_T::CTL: QEIEN Mask                 */

#define QEI_STATUS_IDXF_Pos              (0)                                               /*!< QEI_T::STATUS: IDXF Position           */
#define QEI_STATUS_IDXF_Msk              (0x1ul << QEI_STATUS_IDXF_Pos)                    /*!< QEI_T::STATUS: IDXF Mask               */

#define QEI_STATUS_CMPF_Pos              (1)                                               /*!< QEI_T::STATUS: CMPF Position           */
#define QEI_STATUS_CMPF_Msk              (0x1ul << QEI_STATUS_CMPF_Pos)                    /*!< QEI_T::STATUS: CMPF Mask               */

#define QEI_STATUS_OVUNF_Pos             (2)                                               /*!< QEI_T::STATUS: OVUNF Position          */
#define QEI_STATUS_OVUNF_Msk             (0x1ul << QEI_STATUS_OVUNF_Pos)                   /*!< QEI_T::STATUS: OVUNF Mask              */

#define QEI_STATUS_DIRCHGF_Pos           (3)                                               /*!< QEI_T::STATUS: DIRCHGF Position        */
#define QEI_STATUS_DIRCHGF_Msk           (0x1ul << QEI_STATUS_DIRCHGF_Pos)                 /*!< QEI_T::STATUS: DIRCHGF Mask            */

#define QEI_STATUS_DIRF_Pos              (8)                                               /*!< QEI_T::STATUS: DIRF Position           */
#define QEI_STATUS_DIRF_Msk              (0x1ul << QEI_STATUS_DIRF_Pos)                    /*!< QEI_T::STATUS: DIRF Mask               */

/**@}*/ /* QEI_CONST */
/**@}*/ /* end of QEI register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __QEI_REG_H__ */
