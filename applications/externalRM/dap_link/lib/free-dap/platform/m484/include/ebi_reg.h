/**************************************************************************//**
 * @file     ebi_reg.h
 * @version  V1.00
 * @brief    EBI register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __EBI_REG_H__
#define __EBI_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup EBI External Bus Interface Controller(EBI)
    Memory Mapped Structure for EBI Controller
@{ */

typedef struct
{


    /**
     * @var EBI_T::CTL0
     * Offset: 0x00  External Bus Interface Bank0 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |EN        |EBI Enable Bit
     * |        |          |This bit is the functional enable bit for EBI.
     * |        |          |0 = EBI function Disabled.
     * |        |          |1 = EBI function Enabled.
     * |[1]     |DW16      |EBI Data Width 16-bit Select
     * |        |          |This bit defines if the EBI data width is 8-bit or 16-bit.
     * |        |          |0 = EBI data width is 8-bit.
     * |        |          |1 = EBI data width is 16-bit.
     * |[2]     |CSPOLINV  |Chip Select Pin Polar Inverse
     * |        |          |This bit defines the active level of EBI chip select pin (EBI_nCS).
     * |        |          |0 = Chip select pin (EBI_nCS) is active low.
     * |        |          |1 = Chip select pin (EBI_nCS) is active high.
     * |[3]     |ADSEPEN   |EBI Address/Data Bus Separating Mode Enable Bit
     * |        |          |0 = Address/Data Bus Separating Mode Disabled.
     * |        |          |1 = Address/Data Bus Separating Mode Enabled.
     * |[4]     |CACCESS   |Continuous Data Access Mode
     * |        |          |When con tenuous access mode enabled, the tASU, tALE and tLHD cycles are bypass for continuous data transfer request.
     * |        |          |0 = Continuous data access mode Disabled.
     * |        |          |1 = Continuous data access mode Enabled.
     * |[10:8]  |MCLKDIV   |External Output Clock Divider
     * |        |          |The frequency of EBI output clock (MCLK) is controlled by MCLKDIV as follow:
     * |        |          |000 = HCLK/1.
     * |        |          |001 = HCLK/2.
     * |        |          |010 = HCLK/4.
     * |        |          |011 = HCLK/8.
     * |        |          |100 = HCLK/16.
     * |        |          |101 = HCLK/32.
     * |        |          |110 = HCLK/64.
     * |        |          |111 = HCLK/128.
     * |[18:16] |TALE      |Extend Time of ALE
     * |        |          |The EBI_ALE high pulse period (tALE) to latch the address can be controlled by TALE.
     * |        |          |tALE = (TALE+1)*EBI_MCLK.
     * |        |          |Note: This field only available in EBI_CTL0 register
     * |[24]    |WBUFEN    |EBI Write Buffer Enable Bit
     * |        |          |0 = EBI write buffer Disabled.
     * |        |          |1 = EBI write buffer Enabled.
     * |        |          |Note: This bit only available in EBI_CTL0 register
     * @var EBI_T::TCTL0
     * Offset: 0x04  External Bus Interface Bank0 Timing Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:3]   |TACC      |EBI Data Access Time
     * |        |          |TACC define data access time (tACC).
     * |        |          |tACC = (TACC +1) * EBI_MCLK.
     * |[10:8]  |TAHD      |EBI Data Access Hold Time
     * |        |          |TAHD define data access hold time (tAHD).
     * |        |          |tAHD = (TAHD +1) * EBI_MCLK.
     * |[15:12] |W2X       |Idle Cycle After Write
     * |        |          |This field defines the number of W2X idle cycle.
     * |        |          |W2X idle cycle = (W2X * EBI_MCLK).
     * |        |          |When write action is finish, W2X idle cycle is inserted and EBI_nCS return to idle state.
     * |[22]    |RAHDOFF   |Access Hold Time Disable Control When Read
     * |        |          |0 = The Data Access Hold Time (tAHD) during EBI reading is Enabled.
     * |        |          |1 = The Data Access Hold Time (tAHD) during EBI reading is Disabled.
     * |[23]    |WAHDOFF   |Access Hold Time Disable Control When Write
     * |        |          |0 = The Data Access Hold Time (tAHD) during EBI writing is Enabled.
     * |        |          |1 = The Data Access Hold Time (tAHD) during EBI writing is Disabled.
     * |[27:24] |R2R       |Idle Cycle Between Read-to-read
     * |        |          |This field defines the number of R2R idle cycle.
     * |        |          |R2R idle cycle = (R2R * EBI_MCLK).
     * |        |          |When read action is finish and next action is going to read, R2R idle cycle is inserted and EBI_nCS return to idle state.
     * @var EBI_T::CTL1
     * Offset: 0x10  External Bus Interface Bank1 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |EN        |EBI Enable Bit
     * |        |          |This bit is the functional enable bit for EBI.
     * |        |          |0 = EBI function Disabled.
     * |        |          |1 = EBI function Enabled.
     * |[1]     |DW16      |EBI Data Width 16-bit Select
     * |        |          |This bit defines if the EBI data width is 8-bit or 16-bit.
     * |        |          |0 = EBI data width is 8-bit.
     * |        |          |1 = EBI data width is 16-bit.
     * |[2]     |CSPOLINV  |Chip Select Pin Polar Inverse
     * |        |          |This bit defines the active level of EBI chip select pin (EBI_nCS).
     * |        |          |0 = Chip select pin (EBI_nCS) is active low.
     * |        |          |1 = Chip select pin (EBI_nCS) is active high.
     * |[3]     |ADSEPEN   |EBI Address/Data Bus Separating Mode Enable Bit
     * |        |          |0 = Address/Data Bus Separating Mode Disabled.
     * |        |          |1 = Address/Data Bus Separating Mode Enabled.
     * |[4]     |CACCESS   |Continuous Data Access Mode
     * |        |          |When con tenuous access mode enabled, the tASU, tALE and tLHD cycles are bypass for continuous data transfer request.
     * |        |          |0 = Continuous data access mode Disabled.
     * |        |          |1 = Continuous data access mode Enabled.
     * |[10:8]  |MCLKDIV   |External Output Clock Divider
     * |        |          |The frequency of EBI output clock (MCLK) is controlled by MCLKDIV as follow:
     * |        |          |000 = HCLK/1.
     * |        |          |001 = HCLK/2.
     * |        |          |010 = HCLK/4.
     * |        |          |011 = HCLK/8.
     * |        |          |100 = HCLK/16.
     * |        |          |101 = HCLK/32.
     * |        |          |110 = HCLK/64.
     * |        |          |111 = HCLK/128.
     * |[18:16] |TALE      |Extend Time of ALE
     * |        |          |The EBI_ALE high pulse period (tALE) to latch the address can be controlled by TALE.
     * |        |          |tALE = (TALE+1)*EBI_MCLK.
     * |        |          |Note: This field only available in EBI_CTL0 register
     * |[24]    |WBUFEN    |EBI Write Buffer Enable Bit
     * |        |          |0 = EBI write buffer Disabled.
     * |        |          |1 = EBI write buffer Enabled.
     * |        |          |Note: This bit only available in EBI_CTL0 register
     * @var EBI_T::TCTL1
     * Offset: 0x14  External Bus Interface Bank1 Timing Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:3]   |TACC      |EBI Data Access Time
     * |        |          |TACC define data access time (tACC).
     * |        |          |tACC = (TACC +1) * EBI_MCLK.
     * |[10:8]  |TAHD      |EBI Data Access Hold Time
     * |        |          |TAHD define data access hold time (tAHD).
     * |        |          |tAHD = (TAHD +1) * EBI_MCLK.
     * |[15:12] |W2X       |Idle Cycle After Write
     * |        |          |This field defines the number of W2X idle cycle.
     * |        |          |W2X idle cycle = (W2X * EBI_MCLK).
     * |        |          |When write action is finish, W2X idle cycle is inserted and EBI_nCS return to idle state.
     * |[22]    |RAHDOFF   |Access Hold Time Disable Control When Read
     * |        |          |0 = The Data Access Hold Time (tAHD) during EBI reading is Enabled.
     * |        |          |1 = The Data Access Hold Time (tAHD) during EBI reading is Disabled.
     * |[23]    |WAHDOFF   |Access Hold Time Disable Control When Write
     * |        |          |0 = The Data Access Hold Time (tAHD) during EBI writing is Enabled.
     * |        |          |1 = The Data Access Hold Time (tAHD) during EBI writing is Disabled.
     * |[27:24] |R2R       |Idle Cycle Between Read-to-read
     * |        |          |This field defines the number of R2R idle cycle.
     * |        |          |R2R idle cycle = (R2R * EBI_MCLK).
     * |        |          |When read action is finish and next action is going to read, R2R idle cycle is inserted and EBI_nCS return to idle state.
     * @var EBI_T::CTL2
     * Offset: 0x20  External Bus Interface Bank2 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |EN        |EBI Enable Bit
     * |        |          |This bit is the functional enable bit for EBI.
     * |        |          |0 = EBI function Disabled.
     * |        |          |1 = EBI function Enabled.
     * |[1]     |DW16      |EBI Data Width 16-bit Select
     * |        |          |This bit defines if the EBI data width is 8-bit or 16-bit.
     * |        |          |0 = EBI data width is 8-bit.
     * |        |          |1 = EBI data width is 16-bit.
     * |[2]     |CSPOLINV  |Chip Select Pin Polar Inverse
     * |        |          |This bit defines the active level of EBI chip select pin (EBI_nCS).
     * |        |          |0 = Chip select pin (EBI_nCS) is active low.
     * |        |          |1 = Chip select pin (EBI_nCS) is active high.
     * |[3]     |ADSEPEN   |EBI Address/Data Bus Separating Mode Enable Bit
     * |        |          |0 = Address/Data Bus Separating Mode Disabled.
     * |        |          |1 = Address/Data Bus Separating Mode Enabled.
     * |[4]     |CACCESS   |Continuous Data Access Mode
     * |        |          |When con tenuous access mode enabled, the tASU, tALE and tLHD cycles are bypass for continuous data transfer request.
     * |        |          |0 = Continuous data access mode Disabled.
     * |        |          |1 = Continuous data access mode Enabled.
     * |[10:8]  |MCLKDIV   |External Output Clock Divider
     * |        |          |The frequency of EBI output clock (MCLK) is controlled by MCLKDIV as follow:
     * |        |          |000 = HCLK/1.
     * |        |          |001 = HCLK/2.
     * |        |          |010 = HCLK/4.
     * |        |          |011 = HCLK/8.
     * |        |          |100 = HCLK/16.
     * |        |          |101 = HCLK/32.
     * |        |          |110 = HCLK/64.
     * |        |          |111 = HCLK/128.
     * |[18:16] |TALE      |Extend Time of ALE
     * |        |          |The EBI_ALE high pulse period (tALE) to latch the address can be controlled by TALE.
     * |        |          |tALE = (TALE+1)*EBI_MCLK.
     * |        |          |Note: This field only available in EBI_CTL0 register
     * |[24]    |WBUFEN    |EBI Write Buffer Enable Bit
     * |        |          |0 = EBI write buffer Disabled.
     * |        |          |1 = EBI write buffer Enabled.
     * |        |          |Note: This bit only available in EBI_CTL0 register
     * @var EBI_T::TCTL2
     * Offset: 0x24  External Bus Interface Bank2 Timing Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:3]   |TACC      |EBI Data Access Time
     * |        |          |TACC define data access time (tACC).
     * |        |          |tACC = (TACC +1) * EBI_MCLK.
     * |[10:8]  |TAHD      |EBI Data Access Hold Time
     * |        |          |TAHD define data access hold time (tAHD).
     * |        |          |tAHD = (TAHD +1) * EBI_MCLK.
     * |[15:12] |W2X       |Idle Cycle After Write
     * |        |          |This field defines the number of W2X idle cycle.
     * |        |          |W2X idle cycle = (W2X * EBI_MCLK).
     * |        |          |When write action is finish, W2X idle cycle is inserted and EBI_nCS return to idle state.
     * |[22]    |RAHDOFF   |Access Hold Time Disable Control When Read
     * |        |          |0 = The Data Access Hold Time (tAHD) during EBI reading is Enabled.
     * |        |          |1 = The Data Access Hold Time (tAHD) during EBI reading is Disabled.
     * |[23]    |WAHDOFF   |Access Hold Time Disable Control When Write
     * |        |          |0 = The Data Access Hold Time (tAHD) during EBI writing is Enabled.
     * |        |          |1 = The Data Access Hold Time (tAHD) during EBI writing is Disabled.
     * |[27:24] |R2R       |Idle Cycle Between Read-to-read
     * |        |          |This field defines the number of R2R idle cycle.
     * |        |          |R2R idle cycle = (R2R * EBI_MCLK).
     * |        |          |When read action is finish and next action is going to read, R2R idle cycle is inserted and EBI_nCS return to idle state.
     */
    __IO uint32_t CTL0;                  /*!< [0x0000] External Bus Interface Bank0 Control Register                    */
    __IO uint32_t TCTL0;                 /*!< [0x0004] External Bus Interface Bank0 Timing Control Register             */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CTL1;                  /*!< [0x0010] External Bus Interface Bank1 Control Register                    */
    __IO uint32_t TCTL1;                 /*!< [0x0014] External Bus Interface Bank1 Timing Control Register             */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CTL2;                  /*!< [0x0020] External Bus Interface Bank2 Control Register                    */
    __IO uint32_t TCTL2;                 /*!< [0x0024] External Bus Interface Bank2 Timing Control Register             */

} EBI_T;

/**
    @addtogroup EBI_CONST EBI Bit Field Definition
    Constant Definitions for EBI Controller
@{ */

#define EBI_CTL_EN_Pos                   (0)                                               /*!< EBI_T::CTL: EN Position                */
#define EBI_CTL_EN_Msk                   (0x1ul << EBI_CTL_EN_Pos)                         /*!< EBI_T::CTL: EN Mask                    */

#define EBI_CTL_DW16_Pos                 (1)                                               /*!< EBI_T::CTL: DW16 Position              */
#define EBI_CTL_DW16_Msk                 (0x1ul << EBI_CTL_DW16_Pos)                       /*!< EBI_T::CTL: DW16 Mask                  */

#define EBI_CTL_CSPOLINV_Pos             (2)                                               /*!< EBI_T::CTL: CSPOLINV Position          */
#define EBI_CTL_CSPOLINV_Msk             (0x1ul << EBI_CTL_CSPOLINV_Pos)                   /*!< EBI_T::CTL: CSPOLINV Mask              */

#define EBI_CTL_ADSEPEN_Pos              (3)                                               /*!< EBI_T::CTL: ADSEPEN Position           */
#define EBI_CTL_ADSEPEN_Msk              (0x1ul << EBI_CTL_ADSEPEN_Pos)                    /*!< EBI_T::CTL: ADSEPEN Mask               */

#define EBI_CTL_CACCESS_Pos              (4)                                               /*!< EBI_T::CTL: CACCESS Position           */
#define EBI_CTL_CACCESS_Msk              (0x1ul << EBI_CTL_CACCESS_Pos)                    /*!< EBI_T::CTL: CACCESS Mask               */

#define EBI_CTL_MCLKDIV_Pos              (8)                                               /*!< EBI_T::CTL: MCLKDIV Position           */
#define EBI_CTL_MCLKDIV_Msk              (0x7ul << EBI_CTL_MCLKDIV_Pos)                    /*!< EBI_T::CTL: MCLKDIV Mask               */

#define EBI_CTL_TALE_Pos                 (16)                                              /*!< EBI_T::CTL: TALE Position              */
#define EBI_CTL_TALE_Msk                 (0x7ul << EBI_CTL_TALE_Pos)                       /*!< EBI_T::CTL: TALE Mask                  */

#define EBI_CTL_WBUFEN_Pos               (24)                                              /*!< EBI_T::CTL: WBUFEN Position            */
#define EBI_CTL_WBUFEN_Msk               (0x1ul << EBI_CTL_WBUFEN_Pos)                     /*!< EBI_T::CTL: WBUFEN Mask                */

#define EBI_TCTL_TACC_Pos                (3)                                               /*!< EBI_T::TCTL: TACC Position             */
#define EBI_TCTL_TACC_Msk                (0x1ful << EBI_TCTL_TACC_Pos)                     /*!< EBI_T::TCTL: TACC Mask                 */

#define EBI_TCTL_TAHD_Pos                (8)                                               /*!< EBI_T::TCTL: TAHD Position             */
#define EBI_TCTL_TAHD_Msk                (0x7ul << EBI_TCTL_TAHD_Pos)                      /*!< EBI_T::TCTL: TAHD Mask                 */

#define EBI_TCTL_W2X_Pos                 (12)                                              /*!< EBI_T::TCTL: W2X Position              */
#define EBI_TCTL_W2X_Msk                 (0xful << EBI_TCTL_W2X_Pos)                       /*!< EBI_T::TCTL: W2X Mask                  */

#define EBI_TCTL_RAHDOFF_Pos             (22)                                              /*!< EBI_T::TCTL: RAHDOFF Position          */
#define EBI_TCTL_RAHDOFF_Msk             (0x1ul << EBI_TCTL_RAHDOFF_Pos)                   /*!< EBI_T::TCTL: RAHDOFF Mask              */

#define EBI_TCTL_WAHDOFF_Pos             (23)                                              /*!< EBI_T::TCTL: WAHDOFF Position          */
#define EBI_TCTL_WAHDOFF_Msk             (0x1ul << EBI_TCTL_WAHDOFF_Pos)                   /*!< EBI_T::TCTL: WAHDOFF Mask              */

#define EBI_TCTL_R2R_Pos                 (24)                                              /*!< EBI_T::TCTL: R2R Position              */
#define EBI_TCTL_R2R_Msk                 (0xful << EBI_TCTL_R2R_Pos)                       /*!< EBI_T::TCTL: R2R Mask                  */

#define EBI_CTL0_EN_Pos                  (0)                                               /*!< EBI_T::CTL0: EN Position               */
#define EBI_CTL0_EN_Msk                  (0x1ul << EBI_CTL0_EN_Pos)                        /*!< EBI_T::CTL0: EN Mask                   */

#define EBI_CTL0_DW16_Pos                (1)                                               /*!< EBI_T::CTL0: DW16 Position             */
#define EBI_CTL0_DW16_Msk                (0x1ul << EBI_CTL0_DW16_Pos)                      /*!< EBI_T::CTL0: DW16 Mask                 */

#define EBI_CTL0_CSPOLINV_Pos            (2)                                               /*!< EBI_T::CTL0: CSPOLINV Position         */
#define EBI_CTL0_CSPOLINV_Msk            (0x1ul << EBI_CTL0_CSPOLINV_Pos)                  /*!< EBI_T::CTL0: CSPOLINV Mask             */

#define EBI_CTL0_ADSEPEN_Pos             (3)                                               /*!< EBI_T::CTL0: ADSEPEN Position          */
#define EBI_CTL0_ADSEPEN_Msk             (0x1ul << EBI_CTL0_ADSEPEN_Pos)                   /*!< EBI_T::CTL0: ADSEPEN Mask              */

#define EBI_CTL0_CACCESS_Pos             (4)                                               /*!< EBI_T::CTL0: CACCESS Position          */
#define EBI_CTL0_CACCESS_Msk             (0x1ul << EBI_CTL0_CACCESS_Pos)                   /*!< EBI_T::CTL0: CACCESS Mask              */

#define EBI_CTL0_MCLKDIV_Pos             (8)                                               /*!< EBI_T::CTL0: MCLKDIV Position          */
#define EBI_CTL0_MCLKDIV_Msk             (0x7ul << EBI_CTL0_MCLKDIV_Pos)                   /*!< EBI_T::CTL0: MCLKDIV Mask              */

#define EBI_CTL0_TALE_Pos                (16)                                              /*!< EBI_T::CTL0: TALE Position             */
#define EBI_CTL0_TALE_Msk                (0x7ul << EBI_CTL0_TALE_Pos)                      /*!< EBI_T::CTL0: TALE Mask                 */

#define EBI_CTL0_WBUFEN_Pos              (24)                                              /*!< EBI_T::CTL0: WBUFEN Position           */
#define EBI_CTL0_WBUFEN_Msk              (0x1ul << EBI_CTL0_WBUFEN_Pos)                    /*!< EBI_T::CTL0: WBUFEN Mask               */

#define EBI_TCTL0_TACC_Pos               (3)                                               /*!< EBI_T::TCTL0: TACC Position            */
#define EBI_TCTL0_TACC_Msk               (0x1ful << EBI_TCTL0_TACC_Pos)                    /*!< EBI_T::TCTL0: TACC Mask                */

#define EBI_TCTL0_TAHD_Pos               (8)                                               /*!< EBI_T::TCTL0: TAHD Position            */
#define EBI_TCTL0_TAHD_Msk               (0x7ul << EBI_TCTL0_TAHD_Pos)                     /*!< EBI_T::TCTL0: TAHD Mask                */

#define EBI_TCTL0_W2X_Pos                (12)                                              /*!< EBI_T::TCTL0: W2X Position             */
#define EBI_TCTL0_W2X_Msk                (0xful << EBI_TCTL0_W2X_Pos)                      /*!< EBI_T::TCTL0: W2X Mask                 */

#define EBI_TCTL0_RAHDOFF_Pos            (22)                                              /*!< EBI_T::TCTL0: RAHDOFF Position         */
#define EBI_TCTL0_RAHDOFF_Msk            (0x1ul << EBI_TCTL0_RAHDOFF_Pos)                  /*!< EBI_T::TCTL0: RAHDOFF Mask             */

#define EBI_TCTL0_WAHDOFF_Pos            (23)                                              /*!< EBI_T::TCTL0: WAHDOFF Position         */
#define EBI_TCTL0_WAHDOFF_Msk            (0x1ul << EBI_TCTL0_WAHDOFF_Pos)                  /*!< EBI_T::TCTL0: WAHDOFF Mask             */

#define EBI_TCTL0_R2R_Pos                (24)                                              /*!< EBI_T::TCTL0: R2R Position             */
#define EBI_TCTL0_R2R_Msk                (0xful << EBI_TCTL0_R2R_Pos)                      /*!< EBI_T::TCTL0: R2R Mask                 */

#define EBI_CTL1_EN_Pos                  (0)                                               /*!< EBI_T::CTL1: EN Position               */
#define EBI_CTL1_EN_Msk                  (0x1ul << EBI_CTL1_EN_Pos)                        /*!< EBI_T::CTL1: EN Mask                   */

#define EBI_CTL1_DW16_Pos                (1)                                               /*!< EBI_T::CTL1: DW16 Position             */
#define EBI_CTL1_DW16_Msk                (0x1ul << EBI_CTL1_DW16_Pos)                      /*!< EBI_T::CTL1: DW16 Mask                 */

#define EBI_CTL1_CSPOLINV_Pos            (2)                                               /*!< EBI_T::CTL1: CSPOLINV Position         */
#define EBI_CTL1_CSPOLINV_Msk            (0x1ul << EBI_CTL1_CSPOLINV_Pos)                  /*!< EBI_T::CTL1: CSPOLINV Mask             */

#define EBI_CTL1_ADSEPEN_Pos             (3)                                               /*!< EBI_T::CTL1: ADSEPEN Position          */
#define EBI_CTL1_ADSEPEN_Msk             (0x1ul << EBI_CTL1_ADSEPEN_Pos)                   /*!< EBI_T::CTL1: ADSEPEN Mask              */

#define EBI_CTL1_CACCESS_Pos             (4)                                               /*!< EBI_T::CTL1: CACCESS Position          */
#define EBI_CTL1_CACCESS_Msk             (0x1ul << EBI_CTL1_CACCESS_Pos)                   /*!< EBI_T::CTL1: CACCESS Mask              */

#define EBI_CTL1_MCLKDIV_Pos             (8)                                               /*!< EBI_T::CTL1: MCLKDIV Position          */
#define EBI_CTL1_MCLKDIV_Msk             (0x7ul << EBI_CTL1_MCLKDIV_Pos)                   /*!< EBI_T::CTL1: MCLKDIV Mask              */

#define EBI_CTL1_TALE_Pos                (16)                                              /*!< EBI_T::CTL1: TALE Position             */
#define EBI_CTL1_TALE_Msk                (0x7ul << EBI_CTL1_TALE_Pos)                      /*!< EBI_T::CTL1: TALE Mask                 */

#define EBI_CTL1_WBUFEN_Pos              (24)                                              /*!< EBI_T::CTL1: WBUFEN Position           */
#define EBI_CTL1_WBUFEN_Msk              (0x1ul << EBI_CTL1_WBUFEN_Pos)                    /*!< EBI_T::CTL1: WBUFEN Mask               */

#define EBI_TCTL1_TACC_Pos               (3)                                               /*!< EBI_T::TCTL1: TACC Position            */
#define EBI_TCTL1_TACC_Msk               (0x1ful << EBI_TCTL1_TACC_Pos)                    /*!< EBI_T::TCTL1: TACC Mask                */

#define EBI_TCTL1_TAHD_Pos               (8)                                               /*!< EBI_T::TCTL1: TAHD Position            */
#define EBI_TCTL1_TAHD_Msk               (0x7ul << EBI_TCTL1_TAHD_Pos)                     /*!< EBI_T::TCTL1: TAHD Mask                */

#define EBI_TCTL1_W2X_Pos                (12)                                              /*!< EBI_T::TCTL1: W2X Position             */
#define EBI_TCTL1_W2X_Msk                (0xful << EBI_TCTL1_W2X_Pos)                      /*!< EBI_T::TCTL1: W2X Mask                 */

#define EBI_TCTL1_RAHDOFF_Pos            (22)                                              /*!< EBI_T::TCTL1: RAHDOFF Position         */
#define EBI_TCTL1_RAHDOFF_Msk            (0x1ul << EBI_TCTL1_RAHDOFF_Pos)                  /*!< EBI_T::TCTL1: RAHDOFF Mask             */

#define EBI_TCTL1_WAHDOFF_Pos            (23)                                              /*!< EBI_T::TCTL1: WAHDOFF Position         */
#define EBI_TCTL1_WAHDOFF_Msk            (0x1ul << EBI_TCTL1_WAHDOFF_Pos)                  /*!< EBI_T::TCTL1: WAHDOFF Mask             */

#define EBI_TCTL1_R2R_Pos                (24)                                              /*!< EBI_T::TCTL1: R2R Position             */
#define EBI_TCTL1_R2R_Msk                (0xful << EBI_TCTL1_R2R_Pos)                      /*!< EBI_T::TCTL1: R2R Mask                 */

#define EBI_CTL2_EN_Pos                  (0)                                               /*!< EBI_T::CTL2: EN Position               */
#define EBI_CTL2_EN_Msk                  (0x1ul << EBI_CTL2_EN_Pos)                        /*!< EBI_T::CTL2: EN Mask                   */

#define EBI_CTL2_DW16_Pos                (1)                                               /*!< EBI_T::CTL2: DW16 Position             */
#define EBI_CTL2_DW16_Msk                (0x1ul << EBI_CTL2_DW16_Pos)                      /*!< EBI_T::CTL2: DW16 Mask                 */

#define EBI_CTL2_CSPOLINV_Pos            (2)                                               /*!< EBI_T::CTL2: CSPOLINV Position         */
#define EBI_CTL2_CSPOLINV_Msk            (0x1ul << EBI_CTL2_CSPOLINV_Pos)                  /*!< EBI_T::CTL2: CSPOLINV Mask             */

#define EBI_CTL2_ADSEPEN_Pos             (3)                                               /*!< EBI_T::CTL2: ADSEPEN Position          */
#define EBI_CTL2_ADSEPEN_Msk             (0x1ul << EBI_CTL2_ADSEPEN_Pos)                   /*!< EBI_T::CTL2: ADSEPEN Mask              */

#define EBI_CTL2_CACCESS_Pos             (4)                                               /*!< EBI_T::CTL2: CACCESS Position          */
#define EBI_CTL2_CACCESS_Msk             (0x1ul << EBI_CTL2_CACCESS_Pos)                   /*!< EBI_T::CTL2: CACCESS Mask              */

#define EBI_CTL2_MCLKDIV_Pos             (8)                                               /*!< EBI_T::CTL2: MCLKDIV Position          */
#define EBI_CTL2_MCLKDIV_Msk             (0x7ul << EBI_CTL2_MCLKDIV_Pos)                   /*!< EBI_T::CTL2: MCLKDIV Mask              */

#define EBI_CTL2_TALE_Pos                (16)                                              /*!< EBI_T::CTL2: TALE Position             */
#define EBI_CTL2_TALE_Msk                (0x7ul << EBI_CTL2_TALE_Pos)                      /*!< EBI_T::CTL2: TALE Mask                 */

#define EBI_CTL2_WBUFEN_Pos              (24)                                              /*!< EBI_T::CTL2: WBUFEN Position           */
#define EBI_CTL2_WBUFEN_Msk              (0x1ul << EBI_CTL2_WBUFEN_Pos)                    /*!< EBI_T::CTL2: WBUFEN Mask               */

#define EBI_TCTL2_TACC_Pos               (3)                                               /*!< EBI_T::TCTL2: TACC Position            */
#define EBI_TCTL2_TACC_Msk               (0x1ful << EBI_TCTL2_TACC_Pos)                    /*!< EBI_T::TCTL2: TACC Mask                */

#define EBI_TCTL2_TAHD_Pos               (8)                                               /*!< EBI_T::TCTL2: TAHD Position            */
#define EBI_TCTL2_TAHD_Msk               (0x7ul << EBI_TCTL2_TAHD_Pos)                     /*!< EBI_T::TCTL2: TAHD Mask                */

#define EBI_TCTL2_W2X_Pos                (12)                                              /*!< EBI_T::TCTL2: W2X Position             */
#define EBI_TCTL2_W2X_Msk                (0xful << EBI_TCTL2_W2X_Pos)                      /*!< EBI_T::TCTL2: W2X Mask                 */

#define EBI_TCTL2_RAHDOFF_Pos            (22)                                              /*!< EBI_T::TCTL2: RAHDOFF Position         */
#define EBI_TCTL2_RAHDOFF_Msk            (0x1ul << EBI_TCTL2_RAHDOFF_Pos)                  /*!< EBI_T::TCTL2: RAHDOFF Mask             */

#define EBI_TCTL2_WAHDOFF_Pos            (23)                                              /*!< EBI_T::TCTL2: WAHDOFF Position         */
#define EBI_TCTL2_WAHDOFF_Msk            (0x1ul << EBI_TCTL2_WAHDOFF_Pos)                  /*!< EBI_T::TCTL2: WAHDOFF Mask             */

#define EBI_TCTL2_R2R_Pos                (24)                                              /*!< EBI_T::TCTL2: R2R Position             */
#define EBI_TCTL2_R2R_Msk                (0xful << EBI_TCTL2_R2R_Pos)                      /*!< EBI_T::TCTL2: R2R Mask                 */

/**@}*/ /* EBI_CONST */
/**@}*/ /* end of EBI register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __EBI_REG_H__ */
