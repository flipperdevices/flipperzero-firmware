/**************************************************************************//**
 * @file     ecap_reg.h
 * @version  V1.00
 * @brief    ECAP register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __ECAP_REG_H__
#define __ECAP_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup ECAP Enhanced Input Capture Timer(ECAP)
    Memory Mapped Structure for ECAP Controller
@{ */

typedef struct
{

    /**
     * @var ECAP_T::CNT
     * Offset: 0x00  Input Capture Counter (24-bit up counter)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |CNT       |Input Capture Timer/Counter
     * |        |          |The input Capture Timer/Counter is a 24-bit up-counting counter
     * |        |          |The clock source for the counter is from the clock divider
     * @var ECAP_T::HLD0
     * Offset: 0x04  Input Capture Hold Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |HOLD      |Input Capture Counter Hold Register
     * |        |          |When an active input capture channel detects a valid edge signal change, the ECAPCNT value is latched into the corresponding holding register
     * |        |          |Each input channel has its own holding register named by ECAP_HLDx where x is from 0 to 2 to indicate inputs from IC0 to IC2, respectively.
     * @var ECAP_T::HLD1
     * Offset: 0x08  Input Capture Hold Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |HOLD      |Input Capture Counter Hold Register
     * |        |          |When an active input capture channel detects a valid edge signal change, the ECAPCNT value is latched into the corresponding holding register
     * |        |          |Each input channel has its own holding register named by ECAP_HLDx where x is from 0 to 2 to indicate inputs from IC0 to IC2, respectively.
     * @var ECAP_T::HLD2
     * Offset: 0x0C  Input Capture Hold Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |HOLD      |Input Capture Counter Hold Register
     * |        |          |When an active input capture channel detects a valid edge signal change, the ECAPCNT value is latched into the corresponding holding register
     * |        |          |Each input channel has its own holding register named by ECAP_HLDx where x is from 0 to 2 to indicate inputs from IC0 to IC2, respectively.
     * @var ECAP_T::CNTCMP
     * Offset: 0x10  Input Capture Compare Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |CNTCMP    |Input Capture Counter Compare Register
     * |        |          |If the compare function is enabled (CMPEN = 1), this register (ECAP_CNTCMP) is used to compare with the capture counter (ECAP_CNT).
     * |        |          |If the reload control is enabled (RLDEN[n] = 1, n=0~3), an overflow event or capture events will trigger the hardware to load the value of this register (ECAP_CNTCMP) into ECAP_CNT.
     * @var ECAP_T::CTL0
     * Offset: 0x14  Input Capture Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |NFCLKSEL  |Noise Filter Clock Pre-divide Selection
     * |        |          |To determine the sampling frequency of the Noise Filter clock
     * |        |          |000 = CAP_CLK.
     * |        |          |001 = CAP_CLK/2.
     * |        |          |010 = CAP_CLK/4.
     * |        |          |011 = CAP_CLK/16.
     * |        |          |100 = CAP_CLK/32.
     * |        |          |101 = CAP_CLK/64.
     * |[3]     |CAPNFDIS  |Input Capture Noise Filter Disable Control
     * |        |          |0 = Noise filter of Input Capture Enabled.
     * |        |          |1 = Noise filter of Input Capture Disabled (Bypass).
     * |[4]     |IC0EN     |Port Pin IC0 Input to Input Capture Unit Enable Control
     * |        |          |0 = IC0 input to Input Capture Unit Disabled.
     * |        |          |1 = IC0 input to Input Capture Unit Enabled.
     * |[5]     |IC1EN     |Port Pin IC1 Input to Input Capture Unit Enable Control
     * |        |          |0 = IC1 input to Input Capture Unit Disabled.
     * |        |          |1 = IC1 input to Input Capture Unit Enabled.
     * |[6]     |IC2EN     |Port Pin IC2 Input to Input Capture Unit Enable Control
     * |        |          |0 = IC2 input to Input Capture Unit Disabled.
     * |        |          |1 = IC2 input to Input Capture Unit Enabled.
     * |[9:8]   |CAPSEL0   |CAP0 Input Source Selection
     * |        |          |00 = CAP0 input is from port pin ICAP0.
     * |        |          |01 = Reserved.
     * |        |          |10 = CAP0 input is from signal CHA of QEI controller unit n.
     * |        |          |11 = Reserved.
     * |        |          |Note: Input capture unit n matches QEIn, where n = 0~1.
     * |[11:10] |CAPSEL1   |CAP1 Input Source Selection
     * |        |          |00 = CAP1 input is from port pin ICAP1.
     * |        |          |01 = Reserved.
     * |        |          |10 = CAP1 input is from signal CHB of QEI controller unit n.
     * |        |          |11 = Reserved.
     * |        |          |Note: Input capture unit n matches QEIn, where n = 0~1.
     * |[13:12] |CAPSEL2   |CAP2 Input Source Selection
     * |        |          |00 = CAP2 input is from port pin ICAP2.
     * |        |          |01 = Reserved.
     * |        |          |10 = CAP2 input is from signal CHX of QEI controller unit n.
     * |        |          |11 = Reserved.
     * |        |          |Note: Input capture unit n matches QEIn, where n = 0~1.
     * |[16]    |CAPIEN0   |Input Capture Channel 0 Interrupt Enable Control
     * |        |          |0 = The flag CAPTF0 can trigger Input Capture interrupt Disabled.
     * |        |          |1 = The flag CAPTF0 can trigger Input Capture interrupt Enabled.
     * |[17]    |CAPIEN1   |Input Capture Channel 1 Interrupt Enable Control
     * |        |          |0 = The flag CAPTF1 can trigger Input Capture interrupt Disabled.
     * |        |          |1 = The flag CAPTF1 can trigger Input Capture interrupt Enabled.
     * |[18]    |CAPIEN2   |Input Capture Channel 2 Interrupt Enable Control
     * |        |          |0 = The flag CAPTF2 can trigger Input Capture interrupt Disabled.
     * |        |          |1 = The flag CAPTF2 can trigger Input Capture interrupt Enabled.
     * |[20]    |OVIEN     |CAPOVF Trigger Input Capture Interrupt Enable Control
     * |        |          |0 = The flag CAPOVF can trigger Input Capture interrupt Disabled.
     * |        |          |1 = The flag CAPOVF can trigger Input Capture interrupt Enabled.
     * |[21]    |CMPIEN    |CAPCMPF Trigger Input Capture Interrupt Enable Control
     * |        |          |0 = The flag CAPCMPF can trigger Input Capture interrupt Disabled.
     * |        |          |1 = The flag CAPCMPF can trigger Input Capture interrupt Enabled.
     * |[24]    |CNTEN     |Input Capture Counter Start Counting Control
     * |        |          |Setting this bit to 1, the capture counter (ECAP_CNT) starts up-counting synchronously with the clock from the .
     * |        |          |0 = ECAP_CNT stop counting.
     * |        |          |1 = ECAP_CNT starts up-counting.
     * |[25]    |CMPCLREN  |Input Capture Counter Cleared by Compare-match Control
     * |        |          |If this bit is set to 1, the capture counter (ECAP_CNT) will be cleared to 0 when the compare-match event (CAPCMPF = 1) occurs.
     * |        |          |0 = Compare-match event (CAPCMPF) can clear capture counter (ECAP_CNT) Disabled.
     * |        |          |1 = Compare-match event (CAPCMPF) can clear capture counter (ECAP_CNT) Enabled.
     * |[28]    |CMPEN     |Compare Function Enable Control
     * |        |          |The compare function in input capture timer/counter is to compare the dynamic counting ECAP_CNT with the compare register ECAP_CNTCMP, if ECAP_CNT value reaches ECAP_CNTCMP, the flag CAPCMPF will be set.
     * |        |          |0 = The compare function Disabled.
     * |        |          |1 = The compare function Enabled.
     * |[29]    |CAPEN     |Input Capture Timer/Counter Enable Control
     * |        |          |0 = Input Capture function Disabled.
     * |        |          |1 = Input Capture function Enabled.
     * @var ECAP_T::CTL1
     * Offset: 0x18  Input Capture Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |EDGESEL0  |Channel 0 Captured Edge Selection
     * |        |          |Input capture0 can detect falling edge change only, rising edge change only or both edge change
     * |        |          |00 = Detect rising edge only.
     * |        |          |01 = Detect falling edge only.
     * |        |          |1x = Detect both rising and falling edge.
     * |[3:2]   |EDGESEL1  |Channel 1 Captured Edge Selection
     * |        |          |Input capture1 can detect falling edge change only, rising edge change only or both edge change
     * |        |          |00 = Detect rising edge only.
     * |        |          |01 = Detect falling edge only.
     * |        |          |1x = Detect both rising and falling edge.
     * |[5:4]   |EDGESEL2  |Channel 2 Captured Edge Selection
     * |        |          |Input capture2 can detect falling edge change only, rising edge change only or both edge changes
     * |        |          |00 = Detect rising edge only.
     * |        |          |01 = Detect falling edge only.
     * |        |          |1x = Detect both rising and falling edge.
     * |[8]     |CAP0RLDEN |Capture Counteru2019s Reload Function Triggered by Event CAPTE0 Enable Bit
     * |        |          |0 = The reload triggered by Event CAPTE0 Disabled.
     * |        |          |1 = The reload triggered by Event CAPTE0 Enabled.
     * |[9]     |CAP1RLDEN |Capture Counteru2019s Reload Function Triggered by Event CAPTE1 Enable Bit
     * |        |          |0 = The reload triggered by Event CAPTE1 Disabled.
     * |        |          |1 = The reload triggered by Event CAPTE1 Enabled.
     * |[10]    |CAP2RLDEN |Capture Counteru2019s Reload Function Triggered by Event CAPTE2 Enable Bit
     * |        |          |0 = The reload triggered by Event CAPTE2 Disabled.
     * |        |          |1 = The reload triggered by Event CAPTE2 Enabled.
     * |[11]    |OVRLDEN   |Capture Counteru2019s Reload Function Triggered by Overflow Enable Bit
     * |        |          |0 = The reload triggered by CAPOV Disabled.
     * |        |          |1 = The reload triggered by CAPOV Enabled.
     * |[14:12] |CLKSEL    |Capture Timer Clock Divide Selection
     * |        |          |The capture timer clock has a pre-divider with eight divided options controlled by CLKSEL[2:0].
     * |        |          |000 = CAP_CLK/1.
     * |        |          |001 = CAP_CLK/4.
     * |        |          |010 = CAP_CLK/16.
     * |        |          |011 = CAP_CLK/32.
     * |        |          |100 = CAP_CLK/64.
     * |        |          |101 = CAP_CLK/96.
     * |        |          |110 = CAP_CLK/112.
     * |        |          |111 = CAP_CLK/128.
     * |[17:16] |CNTSRCSEL |Capture Timer/Counter Clock Source Selection
     * |        |          |Select the capture timer/counter clock source.
     * |        |          |00 = CAP_CLK (default).
     * |        |          |01 = CAP0.
     * |        |          |10 = CAP1.
     * |        |          |11 = CAP2.
     * |[20]    |CAP0CLREN |Capture Counter Cleared by Capture Event0 Control
     * |        |          |0 = Event CAPTE0 can clear capture counter (ECAP_CNT) Disabled.
     * |        |          |1 = Event CAPTE0 can clear capture counter (ECAP_CNT) Enabled.
     * |[21]    |CAP1CLREN |Capture Counter Cleared by Capture Event1 Control
     * |        |          |0 = Event CAPTE1 can clear capture counter (ECAP_CNT) Disabled.
     * |        |          |1 = Event CAPTE1 can clear capture counter (ECAP_CNT) Enabled.
     * |[22]    |CAP2CLREN |Capture Counter Cleared by Capture Event2 Control
     * |        |          |0 = Event CAPTE2 can clear capture counter (ECAP_CNT) Disabled.
     * |        |          |1 = Event CAPTE2 can clear capture counter (ECAP_CNT) Enabled.
     * @var ECAP_T::STATUS
     * Offset: 0x1C  Input Capture Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPTF0    |Input Capture Channel 0 Triggered Flag
     * |        |          |When the input capture channel 0 detects a valid edge change at CAP0 input, it will set flag CAPTF0 to high.
     * |        |          |0 = No valid edge change has been detected at CAP0 input since last clear.
     * |        |          |1 = At least a valid edge change has been detected at CAP0 input since last clear.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[1]     |CAPTF1    |Input Capture Channel 1 Triggered Flag
     * |        |          |When the input capture channel 1 detects a valid edge change at CAP1 input, it will set flag CAPTF1 to high.
     * |        |          |0 = No valid edge change has been detected at CAP1 input since last clear.
     * |        |          |1 = At least a valid edge change has been detected at CAP1 input since last clear.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[2]     |CAPTF2    |Input Capture Channel 2 Triggered Flag
     * |        |          |When the input capture channel 2 detects a valid edge change at CAP2 input, it will set flag CAPTF2 to high.
     * |        |          |0 = No valid edge change has been detected at CAP2 input since last clear.
     * |        |          |1 = At least a valid edge change has been detected at CAP2 input since last clear.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[4]     |CAPCMPF   |Input Capture Compare-match Flag
     * |        |          |If the input capture compare function is enabled, the flag is set by hardware when capture counter (ECAP_CNT) up counts and reaches the ECAP_CNTCMP value.
     * |        |          |0 = ECAP_CNT has not matched ECAP_CNTCMP value since last clear.
     * |        |          |1 = ECAP_CNT has matched ECAP_CNTCMP value at least once since last clear.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[5]     |CAPOVF    |Input Capture Counter Overflow Flag
     * |        |          |Flag is set by hardware when counter (ECAP_CNT) overflows from 0x00FF_FFFF to zero.
     * |        |          |0 = No overflow event has occurred since last clear.
     * |        |          |1 = Overflow event(s) has/have occurred since last clear.
     * |        |          |Note: This bit is only cleared by writing 1 to it.
     * |[6]     |CAP0      |Value of Input Channel 0, CAP0 (Read Only)
     * |        |          |Reflecting the value of input channel 0, CAP0
     * |        |          |(The bit is read only and write is ignored)
     * |[7]     |CAP1      |Value of Input Channel 1, CAP1 (Read Only)
     * |        |          |Reflecting the value of input channel 1, CAP1
     * |        |          |(The bit is read only and write is ignored)
     * |[8]     |CAP2      |Value of Input Channel 2, CAP2 (Read Only)
     * |        |          |Reflecting the value of input channel 2, CAP2.
     * |        |          |(The bit is read only and write is ignored)
     */
    __IO uint32_t CNT;                   /*!< [0x0000] Input Capture Counter                                            */
    __IO uint32_t HLD0;                  /*!< [0x0004] Input Capture Hold Register 0                                    */
    __IO uint32_t HLD1;                  /*!< [0x0008] Input Capture Hold Register 1                                    */
    __IO uint32_t HLD2;                  /*!< [0x000c] Input Capture Hold Register 2                                    */
    __IO uint32_t CNTCMP;                /*!< [0x0010] Input Capture Compare Register                                   */
    __IO uint32_t CTL0;                  /*!< [0x0014] Input Capture Control Register 0                                 */
    __IO uint32_t CTL1;                  /*!< [0x0018] Input Capture Control Register 1                                 */
    __IO uint32_t STATUS;                /*!< [0x001c] Input Capture Status Register                                    */

} ECAP_T;

/**
    @addtogroup ECAP_CONST ECAP Bit Field Definition
    Constant Definitions for ECAP Controller
@{ */

#define ECAP_CNT_CNT_Pos                 (0)                                               /*!< ECAP_T::CNT: CNT Position              */
#define ECAP_CNT_CNT_Msk                 (0xfffffful << ECAP_CNT_CNT_Pos)                  /*!< ECAP_T::CNT: CNT Mask                  */

#define ECAP_HLD0_HOLD_Pos               (0)                                               /*!< ECAP_T::HLD0: HOLD Position            */
#define ECAP_HLD0_HOLD_Msk               (0xfffffful << ECAP_HLD0_HOLD_Pos)                /*!< ECAP_T::HLD0: HOLD Mask                */

#define ECAP_HLD1_HOLD_Pos               (0)                                               /*!< ECAP_T::HLD1: HOLD Position            */
#define ECAP_HLD1_HOLD_Msk               (0xfffffful << ECAP_HLD1_HOLD_Pos)                /*!< ECAP_T::HLD1: HOLD Mask                */

#define ECAP_HLD2_HOLD_Pos               (0)                                               /*!< ECAP_T::HLD2: HOLD Position            */
#define ECAP_HLD2_HOLD_Msk               (0xfffffful << ECAP_HLD2_HOLD_Pos)                /*!< ECAP_T::HLD2: HOLD Mask                */

#define ECAP_CNTCMP_CNTCMP_Pos           (0)                                               /*!< ECAP_T::CNTCMP: CNTCMP Position        */
#define ECAP_CNTCMP_CNTCMP_Msk           (0xfffffful << ECAP_CNTCMP_CNTCMP_Pos)            /*!< ECAP_T::CNTCMP: CNTCMP Mask            */

#define ECAP_CTL0_NFCLKSEL_Pos           (0)                                               /*!< ECAP_T::CTL0: NFCLKSEL Position        */
#define ECAP_CTL0_NFCLKSEL_Msk           (0x7ul << ECAP_CTL0_NFCLKSEL_Pos)                 /*!< ECAP_T::CTL0: NFCLKSEL Mask            */

#define ECAP_CTL0_CAPNFDIS_Pos           (3)                                               /*!< ECAP_T::CTL0: CAPNFDIS Position        */
#define ECAP_CTL0_CAPNFDIS_Msk           (0x1ul << ECAP_CTL0_CAPNFDIS_Pos)                 /*!< ECAP_T::CTL0: CAPNFDIS Mask            */

#define ECAP_CTL0_IC0EN_Pos              (4)                                               /*!< ECAP_T::CTL0: IC0EN Position           */
#define ECAP_CTL0_IC0EN_Msk              (0x1ul << ECAP_CTL0_IC0EN_Pos)                    /*!< ECAP_T::CTL0: IC0EN Mask               */

#define ECAP_CTL0_IC1EN_Pos              (5)                                               /*!< ECAP_T::CTL0: IC1EN Position           */
#define ECAP_CTL0_IC1EN_Msk              (0x1ul << ECAP_CTL0_IC1EN_Pos)                    /*!< ECAP_T::CTL0: IC1EN Mask               */

#define ECAP_CTL0_IC2EN_Pos              (6)                                               /*!< ECAP_T::CTL0: IC2EN Position           */
#define ECAP_CTL0_IC2EN_Msk              (0x1ul << ECAP_CTL0_IC2EN_Pos)                    /*!< ECAP_T::CTL0: IC2EN Mask               */

#define ECAP_CTL0_CAPSEL0_Pos            (8)                                               /*!< ECAP_T::CTL0: CAPSEL0 Position         */
#define ECAP_CTL0_CAPSEL0_Msk            (0x3ul << ECAP_CTL0_CAPSEL0_Pos)                  /*!< ECAP_T::CTL0: CAPSEL0 Mask             */

#define ECAP_CTL0_CAPSEL1_Pos            (10)                                              /*!< ECAP_T::CTL0: CAPSEL1 Position         */
#define ECAP_CTL0_CAPSEL1_Msk            (0x3ul << ECAP_CTL0_CAPSEL1_Pos)                  /*!< ECAP_T::CTL0: CAPSEL1 Mask             */

#define ECAP_CTL0_CAPSEL2_Pos            (12)                                              /*!< ECAP_T::CTL0: CAPSEL2 Position         */
#define ECAP_CTL0_CAPSEL2_Msk            (0x3ul << ECAP_CTL0_CAPSEL2_Pos)                  /*!< ECAP_T::CTL0: CAPSEL2 Mask             */

#define ECAP_CTL0_CAPIEN0_Pos            (16)                                              /*!< ECAP_T::CTL0: CAPIEN0 Position         */
#define ECAP_CTL0_CAPIEN0_Msk            (0x1ul << ECAP_CTL0_CAPIEN0_Pos)                  /*!< ECAP_T::CTL0: CAPIEN0 Mask             */

#define ECAP_CTL0_CAPIEN1_Pos            (17)                                              /*!< ECAP_T::CTL0: CAPIEN1 Position         */
#define ECAP_CTL0_CAPIEN1_Msk            (0x1ul << ECAP_CTL0_CAPIEN1_Pos)                  /*!< ECAP_T::CTL0: CAPIEN1 Mask             */

#define ECAP_CTL0_CAPIEN2_Pos            (18)                                              /*!< ECAP_T::CTL0: CAPIEN2 Position         */
#define ECAP_CTL0_CAPIEN2_Msk            (0x1ul << ECAP_CTL0_CAPIEN2_Pos)                  /*!< ECAP_T::CTL0: CAPIEN2 Mask             */

#define ECAP_CTL0_OVIEN_Pos              (20)                                              /*!< ECAP_T::CTL0: OVIEN Position           */
#define ECAP_CTL0_OVIEN_Msk              (0x1ul << ECAP_CTL0_OVIEN_Pos)                    /*!< ECAP_T::CTL0: OVIEN Mask               */

#define ECAP_CTL0_CMPIEN_Pos             (21)                                              /*!< ECAP_T::CTL0: CMPIEN Position          */
#define ECAP_CTL0_CMPIEN_Msk             (0x1ul << ECAP_CTL0_CMPIEN_Pos)                   /*!< ECAP_T::CTL0: CMPIEN Mask              */

#define ECAP_CTL0_CNTEN_Pos              (24)                                              /*!< ECAP_T::CTL0: CNTEN Position           */
#define ECAP_CTL0_CNTEN_Msk              (0x1ul << ECAP_CTL0_CNTEN_Pos)                    /*!< ECAP_T::CTL0: CNTEN Mask               */

#define ECAP_CTL0_CMPCLREN_Pos           (25)                                              /*!< ECAP_T::CTL0: CMPCLREN Position        */
#define ECAP_CTL0_CMPCLREN_Msk           (0x1ul << ECAP_CTL0_CMPCLREN_Pos)                 /*!< ECAP_T::CTL0: CMPCLREN Mask            */

#define ECAP_CTL0_CMPEN_Pos              (28)                                              /*!< ECAP_T::CTL0: CMPEN Position           */
#define ECAP_CTL0_CMPEN_Msk              (0x1ul << ECAP_CTL0_CMPEN_Pos)                    /*!< ECAP_T::CTL0: CMPEN Mask               */

#define ECAP_CTL0_CAPEN_Pos              (29)                                              /*!< ECAP_T::CTL0: CAPEN Position           */
#define ECAP_CTL0_CAPEN_Msk              (0x1ul << ECAP_CTL0_CAPEN_Pos)                    /*!< ECAP_T::CTL0: CAPEN Mask               */

#define ECAP_CTL1_EDGESEL0_Pos           (0)                                               /*!< ECAP_T::CTL1: EDGESEL0 Position        */
#define ECAP_CTL1_EDGESEL0_Msk           (0x3ul << ECAP_CTL1_EDGESEL0_Pos)                 /*!< ECAP_T::CTL1: EDGESEL0 Mask            */

#define ECAP_CTL1_EDGESEL1_Pos           (2)                                               /*!< ECAP_T::CTL1: EDGESEL1 Position        */
#define ECAP_CTL1_EDGESEL1_Msk           (0x3ul << ECAP_CTL1_EDGESEL1_Pos)                 /*!< ECAP_T::CTL1: EDGESEL1 Mask            */

#define ECAP_CTL1_EDGESEL2_Pos           (4)                                               /*!< ECAP_T::CTL1: EDGESEL2 Position        */
#define ECAP_CTL1_EDGESEL2_Msk           (0x3ul << ECAP_CTL1_EDGESEL2_Pos)                 /*!< ECAP_T::CTL1: EDGESEL2 Mask            */

#define ECAP_CTL1_CAP0RLDEN_Pos          (8)                                               /*!< ECAP_T::CTL1: CAP0RLDEN Position       */
#define ECAP_CTL1_CAP0RLDEN_Msk          (0x1ul << ECAP_CTL1_CAP0RLDEN_Pos)                /*!< ECAP_T::CTL1: CAP0RLDEN Mask           */

#define ECAP_CTL1_CAP1RLDEN_Pos          (9)                                               /*!< ECAP_T::CTL1: CAP1RLDEN Position       */
#define ECAP_CTL1_CAP1RLDEN_Msk          (0x1ul << ECAP_CTL1_CAP1RLDEN_Pos)                /*!< ECAP_T::CTL1: CAP1RLDEN Mask           */

#define ECAP_CTL1_CAP2RLDEN_Pos          (10)                                              /*!< ECAP_T::CTL1: CAP2RLDEN Position       */
#define ECAP_CTL1_CAP2RLDEN_Msk          (0x1ul << ECAP_CTL1_CAP2RLDEN_Pos)                /*!< ECAP_T::CTL1: CAP2RLDEN Mask           */

#define ECAP_CTL1_OVRLDEN_Pos            (11)                                              /*!< ECAP_T::CTL1: OVRLDEN Position         */
#define ECAP_CTL1_OVRLDEN_Msk            (0x1ul << ECAP_CTL1_OVRLDEN_Pos)                  /*!< ECAP_T::CTL1: OVRLDEN Mask             */

#define ECAP_CTL1_CLKSEL_Pos             (12)                                              /*!< ECAP_T::CTL1: CLKSEL Position          */
#define ECAP_CTL1_CLKSEL_Msk             (0x7ul << ECAP_CTL1_CLKSEL_Pos)                   /*!< ECAP_T::CTL1: CLKSEL Mask              */

#define ECAP_CTL1_CNTSRCSEL_Pos          (16)                                              /*!< ECAP_T::CTL1: CNTSRCSEL Position       */
#define ECAP_CTL1_CNTSRCSEL_Msk          (0x3ul << ECAP_CTL1_CNTSRCSEL_Pos)                /*!< ECAP_T::CTL1: CNTSRCSEL Mask           */

#define ECAP_CTL1_CAP0CLREN_Pos          (20)                                              /*!< ECAP_T::CTL1: CAP0CLREN Position       */
#define ECAP_CTL1_CAP0CLREN_Msk          (0x1ul << ECAP_CTL1_CAP0CLREN_Pos)                /*!< ECAP_T::CTL1: CAP0CLREN Mask           */

#define ECAP_CTL1_CAP1CLREN_Pos          (21)                                              /*!< ECAP_T::CTL1: CAP1CLREN Position       */
#define ECAP_CTL1_CAP1CLREN_Msk          (0x1ul << ECAP_CTL1_CAP1CLREN_Pos)                /*!< ECAP_T::CTL1: CAP1CLREN Mask           */

#define ECAP_CTL1_CAP2CLREN_Pos          (22)                                              /*!< ECAP_T::CTL1: CAP2CLREN Position       */
#define ECAP_CTL1_CAP2CLREN_Msk          (0x1ul << ECAP_CTL1_CAP2CLREN_Pos)                /*!< ECAP_T::CTL1: CAP2CLREN Mask           */

#define ECAP_STATUS_CAPTF0_Pos           (0)                                               /*!< ECAP_T::STATUS: CAPTF0 Position        */
#define ECAP_STATUS_CAPTF0_Msk           (0x1ul << ECAP_STATUS_CAPTF0_Pos)                 /*!< ECAP_T::STATUS: CAPTF0 Mask            */

#define ECAP_STATUS_CAPTF1_Pos           (1)                                               /*!< ECAP_T::STATUS: CAPTF1 Position        */
#define ECAP_STATUS_CAPTF1_Msk           (0x1ul << ECAP_STATUS_CAPTF1_Pos)                 /*!< ECAP_T::STATUS: CAPTF1 Mask            */

#define ECAP_STATUS_CAPTF2_Pos           (2)                                               /*!< ECAP_T::STATUS: CAPTF2 Position        */
#define ECAP_STATUS_CAPTF2_Msk           (0x1ul << ECAP_STATUS_CAPTF2_Pos)                 /*!< ECAP_T::STATUS: CAPTF2 Mask            */

#define ECAP_STATUS_CAPCMPF_Pos          (4)                                               /*!< ECAP_T::STATUS: CAPCMPF Position       */
#define ECAP_STATUS_CAPCMPF_Msk          (0x1ul << ECAP_STATUS_CAPCMPF_Pos)                /*!< ECAP_T::STATUS: CAPCMPF Mask           */

#define ECAP_STATUS_CAPOVF_Pos           (5)                                               /*!< ECAP_T::STATUS: CAPOVF Position        */
#define ECAP_STATUS_CAPOVF_Msk           (0x1ul << ECAP_STATUS_CAPOVF_Pos)                 /*!< ECAP_T::STATUS: CAPOVF Mask            */

#define ECAP_STATUS_CAP0_Pos             (8)                                               /*!< ECAP_T::STATUS: CAP0 Position          */
#define ECAP_STATUS_CAP0_Msk             (0x1ul << ECAP_STATUS_CAP0_Pos)                   /*!< ECAP_T::STATUS: CAP0 Mask              */

#define ECAP_STATUS_CAP1_Pos             (9)                                               /*!< ECAP_T::STATUS: CAP1 Position          */
#define ECAP_STATUS_CAP1_Msk             (0x1ul << ECAP_STATUS_CAP1_Pos)                   /*!< ECAP_T::STATUS: CAP1 Mask              */

#define ECAP_STATUS_CAP2_Pos             (10)                                               /*!< ECAP_T::STATUS: CAP2 Position          */
#define ECAP_STATUS_CAP2_Msk             (0x1ul << ECAP_STATUS_CAP2_Pos)                   /*!< ECAP_T::STATUS: CAP2 Mask              */

/**@}*/ /* ECAP_CONST */
/**@}*/ /* end of ECAP register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __ECAP_REG_H__ */
