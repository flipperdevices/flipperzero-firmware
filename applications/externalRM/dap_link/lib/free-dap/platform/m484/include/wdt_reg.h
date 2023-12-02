/**************************************************************************//**
 * @file     wdt_reg.h
 * @version  V1.00
 * @brief    WDT register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __WDT_REG_H__
#define __WDT_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup WDT Watch Dog Timer Controller(WDT)
    Memory Mapped Structure for WDT Controller
@{ */

typedef struct
{


    /**
     * @var WDT_T::CTL
     * Offset: 0x00  WDT Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RSTCNT    |Reset WDT Up Counter (Write Protect)
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the internal 18-bit WDT up counter value.
     * |        |          |Note1: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note2: This bit will be automatically cleared by hardware.
     * |[1]     |RSTEN     |WDT Time-out Reset Enable Control (Write Protect)
     * |        |          |Setting this bit will enable the WDT time-out reset function If the WDT up counter value has not been cleared after the specific WDT reset delay period expires.
     * |        |          |0 = WDT time-out reset function Disabled.
     * |        |          |1 = WDT time-out reset function Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |RSTF      |WDT Time-out Reset Flag
     * |        |          |This bit indicates the system has been reset by WDT time-out reset or not.
     * |        |          |0 = WDT time-out reset did not occur.
     * |        |          |1 = WDT time-out reset occurred.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[3]     |IF        |WDT Time-out Interrupt Flag
     * |        |          |This bit will set to 1 while WDT up counter value reaches the selected WDT time-out interval
     * |        |          |0 = WDT time-out interrupt did not occur.
     * |        |          |1 = WDT time-out interrupt occurred.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[4]     |WKEN      |WDT Time-out Wake-up Function Control (Write Protect)
     * |        |          |If this bit is set to 1, while WDT time-out interrupt flag IF (WDT_CTL[3]) is generated to 1 and interrupt enable bit INTEN (WDT_CTL[6]) is enabled, the WDT time-out interrupt signal will generate a wake-up trigger event to chip.
     * |        |          |0 = Wake-up trigger event Disabled if WDT time-out interrupt signal generated.
     * |        |          |1 = Wake-up trigger event Enabled if WDT time-out interrupt signal generated.
     * |        |          |Note1: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note2: Chip can be woken-up by WDT time-out interrupt signal generated only if WDT clock source is selected to 10 kHz internal low speed RC oscillator (LIRC) or LXT.
     * |[5]     |WKF       |WDT Time-out Wake-up Flag (Write Protect)
     * |        |          |This bit indicates the interrupt wake-up flag status of WDT
     * |        |          |0 = WDT does not cause chip wake-up.
     * |        |          |1 = Chip wake-up from Idle or Power-down mode if WDT time-out interrupt signal generated.
     * |        |          |Note1: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[6]     |INTEN     |WDT Time-out Interrupt Enable Control (Write Protect)
     * |        |          |If this bit is enabled, the WDT time-out interrupt signal is generated and inform to CPU.
     * |        |          |0 = WDT time-out interrupt Disabled.
     * |        |          |1 = WDT time-out interrupt Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7]     |WDTEN     |WDT Enable Control (Write Protect)
     * |        |          |0 = WDT Disabled (This action will reset the internal up counter value).
     * |        |          |1 = WDT Enabled.
     * |        |          |Note1: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note2: If CWDTEN[2:0] (combined by Config0[31] and Config0[4:3]) bits is not configure to 111, this bit is forced as 1 and user cannot change this bit to 0.
     * |[10:8]  |TOUTSEL   |WDT Time-out Interval Selection (Write Protect)
     * |        |          |These three bits select the time-out interval period for the WDT.
     * |        |          |000 = 24 * WDT_CLK.
     * |        |          |001 = 26 * WDT_CLK.
     * |        |          |010 = 28 * WDT_CLK.
     * |        |          |011 = 210 * WDT_CLK.
     * |        |          |100 = 212 * WDT_CLK.
     * |        |          |101 = 214 * WDT_CLK.
     * |        |          |110 = 216 * WDT_CLK.
     * |        |          |111 = 218 * WDT_CLK.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[30]    |SYNC      |WDT Enable Control SYNC Flag Indicator (Read Only)
     * |        |          |If user execute enable/disable WDTEN (WDT_CTL[7]), this flag can be indicated enable/disable WDTEN function is completed or not.
     * |        |          |0 = Set WDTEN bit is completed.
     * |        |          |1 = Set WDTEN bit is synchronizing and not become active yet..
     * |        |          |Note: Perform enable or disable WDTEN bit needs 2 * WDT_CLK period to become active.
     * |[31]    |ICEDEBUG  |ICE Debug Mode Acknowledge Disable Control (Write Protect)
     * |        |          |0 = ICE debug mode acknowledgement affects WDT counting.
     * |        |          |WDT up counter will be held while CPU is held by ICE.
     * |        |          |1 = ICE debug mode acknowledgement Disabled.
     * |        |          |WDT up counter will keep going no matter CPU is held by ICE or not.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var WDT_T::ALTCTL
     * Offset: 0x04  WDT Alternative Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |RSTDSEL   |WDT Reset Delay Selection (Write Protect)
     * |        |          |When WDT time-out happened, user has a time named WDT Reset Delay Period to clear WDT counter by setting RSTCNT (WDT_CTL[0]) to prevent WDT time-out reset happened
     * |        |          |User can select a suitable setting of RSTDSEL for different WDT Reset Delay Period.
     * |        |          |00 = WDT Reset Delay Period is 1026 * WDT_CLK.
     * |        |          |01 = WDT Reset Delay Period is 130 * WDT_CLK.
     * |        |          |10 = WDT Reset Delay Period is 18 * WDT_CLK.
     * |        |          |11 = WDT Reset Delay Period is 3 * WDT_CLK.
     * |        |          |Note1: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note2: This register will be reset to 0 if WDT time-out reset happened.
     * @var WDT_T::RSTCNT
     * Offset: 0x08  WDT Reset Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |RSTCNT    |WDT Reset Counter Register
     * |        |          |Writing 0x00005AA5 to this field will reset the internal 18-bit WDT up counter value to 0.
     * |        |          |Note: Perform RSTCNT to reset counter needs 2 * WDT_CLK period to become active.
     * |        |          |Note: RSTCNT (WDT_CTL[0]) bit is a write protected bit
     * |        |          |RSTCNT (WDT_RSTCNT[31:0]) bits are not write protected.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] WDT Control Register                                             */
    __IO uint32_t ALTCTL;                /*!< [0x0004] WDT Alternative Control Register                                 */
    __O  uint32_t RSTCNT;                /*!< [0x0008] WDT Reset Counter Register                                       */

} WDT_T;

/**
    @addtogroup WDT_CONST WDT Bit Field Definition
    Constant Definitions for WDT Controller
@{ */

#define WDT_CTL_RSTCNT_Pos               (0)                                               /*!< WDT_T::CTL: RSTCNT Position            */
#define WDT_CTL_RSTCNT_Msk               (0x1ul << WDT_CTL_RSTCNT_Pos)                     /*!< WDT_T::CTL: RSTCNT Mask                */

#define WDT_CTL_RSTEN_Pos                (1)                                               /*!< WDT_T::CTL: RSTEN Position             */
#define WDT_CTL_RSTEN_Msk                (0x1ul << WDT_CTL_RSTEN_Pos)                      /*!< WDT_T::CTL: RSTEN Mask                 */

#define WDT_CTL_RSTF_Pos                 (2)                                               /*!< WDT_T::CTL: RSTF Position              */
#define WDT_CTL_RSTF_Msk                 (0x1ul << WDT_CTL_RSTF_Pos)                       /*!< WDT_T::CTL: RSTF Mask                  */

#define WDT_CTL_IF_Pos                   (3)                                               /*!< WDT_T::CTL: IF Position                */
#define WDT_CTL_IF_Msk                   (0x1ul << WDT_CTL_IF_Pos)                         /*!< WDT_T::CTL: IF Mask                    */

#define WDT_CTL_WKEN_Pos                 (4)                                               /*!< WDT_T::CTL: WKEN Position              */
#define WDT_CTL_WKEN_Msk                 (0x1ul << WDT_CTL_WKEN_Pos)                       /*!< WDT_T::CTL: WKEN Mask                  */

#define WDT_CTL_WKF_Pos                  (5)                                               /*!< WDT_T::CTL: WKF Position               */
#define WDT_CTL_WKF_Msk                  (0x1ul << WDT_CTL_WKF_Pos)                        /*!< WDT_T::CTL: WKF Mask                   */

#define WDT_CTL_INTEN_Pos                (6)                                               /*!< WDT_T::CTL: INTEN Position             */
#define WDT_CTL_INTEN_Msk                (0x1ul << WDT_CTL_INTEN_Pos)                      /*!< WDT_T::CTL: INTEN Mask                 */

#define WDT_CTL_WDTEN_Pos                (7)                                               /*!< WDT_T::CTL: WDTEN Position             */
#define WDT_CTL_WDTEN_Msk                (0x1ul << WDT_CTL_WDTEN_Pos)                      /*!< WDT_T::CTL: WDTEN Mask                 */

#define WDT_CTL_TOUTSEL_Pos              (8)                                               /*!< WDT_T::CTL: TOUTSEL Position           */
#define WDT_CTL_TOUTSEL_Msk              (0x7ul << WDT_CTL_TOUTSEL_Pos)                    /*!< WDT_T::CTL: TOUTSEL Mask               */

#define WDT_CTL_SYNC_Pos                 (30)                                              /*!< WDT_T::CTL: SYNC Position              */
#define WDT_CTL_SYNC_Msk                 (0x1ul << WDT_CTL_SYNC_Pos)                       /*!< WDT_T::CTL: SYNC Mask                  */

#define WDT_CTL_ICEDEBUG_Pos             (31)                                              /*!< WDT_T::CTL: ICEDEBUG Position          */
#define WDT_CTL_ICEDEBUG_Msk             (0x1ul << WDT_CTL_ICEDEBUG_Pos)                   /*!< WDT_T::CTL: ICEDEBUG Mask              */

#define WDT_ALTCTL_RSTDSEL_Pos           (0)                                               /*!< WDT_T::ALTCTL: RSTDSEL Position        */
#define WDT_ALTCTL_RSTDSEL_Msk           (0x3ul << WDT_ALTCTL_RSTDSEL_Pos)                 /*!< WDT_T::ALTCTL: RSTDSEL Mask            */

#define WDT_RSTCNT_RSTCNT_Pos            (0)                                               /*!< WDT_T::RSTCNT: RSTCNT Position         */
#define WDT_RSTCNT_RSTCNT_Msk            (0xfffffffful << WDT_RSTCNT_RSTCNT_Pos)           /*!< WDT_T::RSTCNT: RSTCNT Mask             */

/**@}*/ /* WDT_CONST */
/**@}*/ /* end of WDT register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __WDT_REG_H__ */
