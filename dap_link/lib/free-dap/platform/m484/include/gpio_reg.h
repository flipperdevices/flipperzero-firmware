/**************************************************************************//**
 * @file     gpio_reg.h
 * @version  V1.00
 * @brief    GPIO register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __GPIO_REG_H__
#define __GPIO_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup GPIO General Purpose Input/Output Controller(GPIO)
    Memory Mapped Structure for GPIO Controller
@{ */


typedef struct
{

    /**
     * @var GPIO_T::MODE
     * Offset: 0x00/0x40/0x80/0xC0/0x100/0x140/0x180/0x1C0  Port A-H I/O Mode Control
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2n+1:2n]|MODEn    |Port A-H I/O Pin[n] Mode Control
     * |        |          |Determine each I/O mode of Px.n pins.
     * |        |          |00 = Px.n is in Input mode.
     * |        |          |01 = Px.n is in Push-pull Output mode.
     * |        |          |10 = Px.n is in Open-drain Output mode.
     * |        |          |11 = Px.n is in Quasi-bidirectional mode.
     * |        |          |Note1: The initial value of this field is defined by CIOINI (CONFIG0 [10]).
     * |        |          |If CIOINI is set to 0, the default value is 0xFFFF_FFFF and all pins will be quasi-bidirectional mode after chip powered on.
     * |        |          |If CIOINI is set to 1, the default value is 0x0000_0000 and all pins will be input mode after chip powered on.
     * |        |          |Note2:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::DINOFF
     * Offset: 0x04/0x44/0x84/0xC4/0x104/0x144/0x184/0x1C4  Port A-H Digital Input Path Disable Control
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n+16]  |DINOFFn   |Port A-H Pin[n] Digital Input Path Disable Control
     * |        |          |Each of these bits is used to control if the digital input path of corresponding Px.n pin is disabled.
     * |        |          |If input is analog signal, users can disable Px.n digital input path to avoid input current leakage.
     * |        |          |0 = Px.n digital input path Enabled.
     * |        |          |1 = Px.n digital input path Disabled (digital input tied to low).
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::DOUT
     * Offset: 0x08/0x48/0x88/0xC8/0x108/0x148/0x188/0x1C8  Port A-H Data Output Value
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |DOUTn     |Port A-H Pin[n] Output Value
     * |        |          |Each of these bits controls the status of a Px.n pin when the Px.n is configured as Push-pull output, Open-drain output or Quasi-bidirectional mode.
     * |        |          |0 = Px.n will drive Low if the Px.n pin is configured as Push-pull output, Open-drain output or Quasi-bidirectional mode.
     * |        |          |1 = Px.n will drive High if the Px.n pin is configured as Push-pull output or Quasi-bidirectional mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::DATMSK
     * Offset: 0x0C/0x4C/0x8C/0xCC/0x10C/0x14C/0x18C/0x1CC  Port A-H Data Output Write Mask
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |DATMSKn    |Port A-H Pin[n] Data Output Write Mask
     * |        |          |These bits are used to protect the corresponding DOUT (Px_DOUT[n]) bit.
     * |        |          |When the DATMSK (Px_DATMSK[n]) bit is set to 1, the corresponding DOUT (Px_DOUT[n]) bit is protected.
     * |        |          |If the write signal is masked, writing data to the protect bit is ignored.
     * |        |          |0 = Corresponding DOUT (Px_DOUT[n]) bit can be updated.
     * |        |          |1 = Corresponding DOUT (Px_DOUT[n]) bit protected.
     * |        |          |Note1: This function only protects the corresponding DOUT (Px_DOUT[n]) bit, and will not protect the corresponding PDIO (Pxn_PDIO[n]) bit.
     * |        |          |Note2:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::PIN
     * Offset: 0x10/0x50/0x90/0xD0/0x110/0x150/0x190/0x1D0  Port A-H Pin Value
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |PINn      |Port A-H Pin[n] Pin Value
     * |        |          |Each bit of the register reflects the actual status of the respective Px.n pin.
     * |        |          |If the bit is 1, it indicates the corresponding pin status is high; else the pin status is low.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::DBEN
     * Offset: 0x14/0x54/0x94/0xD4/0x114/0x154/0x194/0x1D4  Port A-H De-Bounce Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |DBENn     |Port A-H Pin[n] Input Signal De-Bounce Enable Bit
     * |        |          |The DBEN[n] bit is used to enable the de-bounce function for each corresponding bit.
     * |        |          |If the input signal pulse width cannot be sampled by continuous two de-bounce sample cycle, the input signal transition is seen as the signal bounce and will not trigger the interrupt.
     * |        |          |The de-bounce clock source is controlled by DBCLKSRC (GPIO_DBCTL [4]), one de-bounce sample cycle period is controlled by DBCLKSEL (GPIO_DBCTL [3:0]).
     * |        |          |0 = Px.n de-bounce function Disabled.
     * |        |          |1 = Px.n de-bounce function Enabled.
     * |        |          |The de-bounce function is valid only for edge triggered interrupt.
     * |        |          |If the interrupt mode is level triggered, the de-bounce enable bit is ignored.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::INTTYPE
     * Offset: 0x18/0x58/0x98/0xD8/0x118/0x158/0x198/0x1D8  Port A-H Interrupt Trigger Type Control
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |TYPEn     |Port A-H Pin[n] Edge or Level Detection Interrupt Trigger Type Control
     * |        |          |TYPE (Px_INTTYPE[n]) bit is used to control the triggered interrupt is by level trigger or by edge trigger.
     * |        |          |If the interrupt is by edge trigger, the trigger source can be controlled by de-bounce.
     * |        |          |If the interrupt is by level trigger, the input source is sampled by one HCLK clock and generates the interrupt.
     * |        |          |0 = Edge trigger interrupt.
     * |        |          |1 = Level trigger interrupt.
     * |        |          |If the pin is set as the level trigger interrupt, only one level can be set on the registers RHIEN (Px_INTEN[n+16])/FLIEN (Px_INTEN[n]).
     * |        |          |If both levels to trigger interrupt are set, the setting is ignored and no interrupt will occur.
     * |        |          |The de-bounce function is valid only for edge triggered interrupt.
     * |        |          |If the interrupt mode is level triggered, the de-bounce enable bit is ignored.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::INTEN
     * Offset: 0x1C/0x5C/0x9C/0xDC/0x11C/0x15C/0x19C/0x1DC  Port A-H Interrupt Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |FLIENn    |Port A-H Pin[n] Falling Edge or Low Level Interrupt Trigger Type Enable Bit
     * |        |          |The FLIEN (Px_INTEN[n]) bit is used to enable the interrupt for each of the corresponding input Px.n pin.
     * |        |          |Set bit to 1 also enable the pin wake-up function.
     * |        |          |When setting the FLIEN (Px_INTEN[n]) bit to 1 :
     * |        |          |If the interrupt is level trigger (TYPE (Px_INTTYPE[n]) bit is set to 1), the input Px.n pin will generate the interrupt while this pin state is at low level.
     * |        |          |If the interrupt is edge trigger(TYPE (Px_INTTYPE[n]) bit is set to 0), the input Px.n pin will generate the interrupt while this pin state changed from high to low.
     * |        |          |0 = Px.n level low or high to low interrupt Disabled.
     * |        |          |1 = Px.n level low or high to low interrupt Enabled.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[n+16]  |RHIENn    |Port A-H Pin[n] Rising Edge or High Level Interrupt Trigger Type Enable Bit
     * |        |          |The RHIEN (Px_INTEN[n+16]) bit is used to enable the interrupt for each of the corresponding input Px.n pin
     * |        |          |Set bit to 1 also enable the pin wake-up function.
     * |        |          |When setting the RHIEN (Px_INTEN[n+16]) bit to 1 :
     * |        |          |If the interrupt is level trigger (TYPE (Px_INTTYPE[n]) bit is set to 1), the input Px.n pin will generate the interrupt while this pin state is at high level.
     * |        |          |If the interrupt is edge trigger (TYPE (Px_INTTYPE[n]) bit is set to 0), the input Px.n pin will generate the interrupt while this pin state changed from low to high.
     * |        |          |0 = Px.n level high or low to high interrupt Disabled.
     * |        |          |1 = Px.n level high or low to high interrupt Enabled.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::INTSRC
     * Offset: 0x20/0x60/0xA0/0xE0/0x120/0x160/0x1A0/0x1E0  Port A-H Interrupt Source Flag
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |INTSRCn   |Port A-H Pin[n] Interrupt Source Flag
     * |        |          |Write Operation :
     * |        |          |0 = No action.
     * |        |          |1 = Clear the corresponding pending interrupt.
     * |        |          |Read Operation :
     * |        |          |0 = No interrupt at Px.n.
     * |        |          |1 = Px.n generates an interrupt.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::SMTEN
     * Offset: 0x24/0x64/0xA4/0xE4/0x124/0x164/0x1A4/0x1E4  Port A-H Input Schmitt Trigger Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[n]     |SMTENn    |Port A-H Pin[n] Input Schmitt Trigger Enable Bit
     * |        |          |0 = Px.n input Schmitt trigger function Disabled.
     * |        |          |1 = Px.n input Schmitt trigger function Enabled.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::SLEWCTL
     * Offset: 0x28/0x68/0xA8/0xE8/0x128/0x168/0x1A8/0x1E8  Port A-H High Slew Rate Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2n+1:2n]|HSRENn    |Port A-H Pin[n] High Slew Rate Control
     * |        |          |00 = Px.n output with normal slew rate mode (maximum 40 MHz at 2.7V).
     * |        |          |01 = Px.n output with high slew rate mode (maximum 80 MHz at 2.7V).
     * |        |          |10 = Px.n output with fast slew rate mode (maximum 100 MHz at 2.7V.
     * |        |          |11 = Reserved.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var GPIO_T::PUSEL
     * Offset: 0x30/0x70/0xB0/0xF0/0x130/0x170/0x1B0/0x1F0  Port A-H Pull-up and Pull-down Selection Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2n+1:2n]|PUSELn    |Port A-H Pin[n] Pull-up and Pull-down Enable Register
     * |        |          |Determine each I/O Pull-up/pull-down of Px.n pins.
     * |        |          |00 = Px.n pull-up and pull-up disable.
     * |        |          |01 = Px.n pull-up enable.
     * |        |          |10 = Px.n pull-down enable.
     * |        |          |11 = Reserved.
     * |        |          |Note1:
     * |        |          |Basically, the pull-up control and pull-down control has following behavior limitation
     * |        |          |The independent pull-up control register only valid when MODEn set as tri-state and open-drain mode
     * |        |          |The independent pull-down control register only valid when MODEn set as tri-state mode
     * |        |          |When both pull-up pull-down is set as 1 at tri-state mode, keep I/O in tri-state mode
     * |        |          |Note2:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     */

    __IO uint32_t MODE;          /* Offset: 0x00/0x40/0x80/0xC0/0x100/0x140/0x180/0x1C0  Port A-H I/O Mode Control                       */
    __IO uint32_t DINOFF;        /* Offset: 0x04/0x44/0x84/0xC4/0x104/0x144/0x184/0x1C4  Port A-H Digital Input Path Disable Control     */
    __IO uint32_t DOUT;          /* Offset: 0x08/0x48/0x88/0xC8/0x108/0x148/0x188/0x1C8  Port A-H Data Output Value                      */
    __IO uint32_t DATMSK;        /* Offset: 0x0C/0x4C/0x8C/0xCC/0x10C/0x14C/0x18C/0x1CC  Port A-H Data Output Write Mask                 */
    __I  uint32_t PIN;           /* Offset: 0x10/0x50/0x90/0xD0/0x110/0x150/0x190/0x1D0  Port A-H Pin Value                              */
    __IO uint32_t DBEN;          /* Offset: 0x14/0x54/0x94/0xD4/0x114/0x154/0x194/0x1D4  Port A-H De-Bounce Enable Control Register      */
    __IO uint32_t INTTYPE;       /* Offset: 0x18/0x58/0x98/0xD8/0x118/0x158/0x198/0x1D8  Port A-H Interrupt Trigger Type Control         */
    __IO uint32_t INTEN;         /* Offset: 0x1C/0x5C/0x9C/0xDC/0x11C/0x15C/0x19C/0x1DC  Port A-H Interrupt Enable Control Register      */
    __IO uint32_t INTSRC;        /* Offset: 0x20/0x60/0xA0/0xE0/0x120/0x160/0x1A0/0x1E0  Port A-H Interrupt Source Flag                  */
    __IO uint32_t SMTEN;         /* Offset: 0x24/0x64/0xA4/0xE4/0x124/0x164/0x1A4/0x1E4  Port A-H Input Schmitt Trigger Enable Register  */
    __IO uint32_t SLEWCTL;       /* Offset: 0x28/0x68/0xA8/0xE8/0x128/0x168/0x1A8/0x1E8  Port A-H High Slew Rate Control Register        */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t PUSEL;         /* Offset: 0x30/0x70/0xB0/0xF0/0x130/0x170/0x1B0/0x1F0  Port A-H Pull-up and Pull-down Enable Register  */
    __I  uint32_t RESERVE1[499];
    __IO uint32_t PDIO[16];
} GPIO_T;

typedef struct
{

    /**
     * @var GPIO_DBCTL_T::DBCTL
     * Offset: 0x440  Interrupt De-bounce Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |DBCLKSEL  |De-Bounce Sampling Cycle Selection
     * |        |          |0000 = Sample interrupt input once per 1 clocks.
     * |        |          |0001 = Sample interrupt input once per 2 clocks.
     * |        |          |0010 = Sample interrupt input once per 4 clocks.
     * |        |          |0011 = Sample interrupt input once per 8 clocks.
     * |        |          |0100 = Sample interrupt input once per 16 clocks.
     * |        |          |0101 = Sample interrupt input once per 32 clocks.
     * |        |          |0110 = Sample interrupt input once per 64 clocks.
     * |        |          |0111 = Sample interrupt input once per 128 clocks.
     * |        |          |1000 = Sample interrupt input once per 256 clocks.
     * |        |          |1001 = Sample interrupt input once per 2*256 clocks.
     * |        |          |1010 = Sample interrupt input once per 4*256 clocks.
     * |        |          |1011 = Sample interrupt input once per 8*256 clocks.
     * |        |          |1100 = Sample interrupt input once per 16*256 clocks.
     * |        |          |1101 = Sample interrupt input once per 32*256 clocks.
     * |        |          |1110 = Sample interrupt input once per 64*256 clocks.
     * |        |          |1111 = Sample interrupt input once per 128*256 clocks.
     * |[4]     |DBCLKSRC  |De-Bounce Counter Clock Source Selection
     * |        |          |0 = De-bounce counter clock source is the HCLK.
     * |        |          |1 = De-bounce counter clock source is the 10 kHz internal low speed RC oscillator (LIRC).
     * |[5]     |ICLKON    |Interrupt Clock On Mode
     * |        |          |0 = Edge detection circuit is active only if I/O pin corresponding RHIEN (Px_INTEN[n+16])/FLIEN (Px_INTEN[n]) bit is set to 1.
     * |        |          |1 = All I/O pins edge detection circuit is always active after reset.
     * |        |          |Note: It is recommended to disable this bit to save system power if no special application concern.
     */

    __IO uint32_t DBCTL;         /* Offset: 0x440  Interrupt De-bounce Control Register                              */

} GPIO_DBCTL_T;

/**
    @addtogroup GPIO_CONST GPIO Bit Field Definition
    Constant Definitions for GPIO Controller
@{ */

#define GPIO_MODE_MODE0_Pos              (0)                                               /*!< GPIO_T::MODE: MODE0 Position              */
#define GPIO_MODE_MODE0_Msk              (0x3ul << GPIO_MODE_MODE0_Pos)                    /*!< GPIO_T::MODE: MODE0 Mask                  */

#define GPIO_MODE_MODE1_Pos              (2)                                               /*!< GPIO_T::MODE: MODE1 Position              */
#define GPIO_MODE_MODE1_Msk              (0x3ul << GPIO_MODE_MODE1_Pos)                    /*!< GPIO_T::MODE: MODE1 Mask                  */

#define GPIO_MODE_MODE2_Pos              (4)                                               /*!< GPIO_T::MODE: MODE2 Position              */
#define GPIO_MODE_MODE2_Msk              (0x3ul << GPIO_MODE_MODE2_Pos)                    /*!< GPIO_T::MODE: MODE2 Mask                  */

#define GPIO_MODE_MODE3_Pos              (6)                                               /*!< GPIO_T::MODE: MODE3 Position              */
#define GPIO_MODE_MODE3_Msk              (0x3ul << GPIO_MODE_MODE3_Pos)                    /*!< GPIO_T::MODE: MODE3 Mask                  */

#define GPIO_MODE_MODE4_Pos              (8)                                               /*!< GPIO_T::MODE: MODE4 Position              */
#define GPIO_MODE_MODE4_Msk              (0x3ul << GPIO_MODE_MODE4_Pos)                    /*!< GPIO_T::MODE: MODE4 Mask                  */

#define GPIO_MODE_MODE5_Pos              (10)                                              /*!< GPIO_T::MODE: MODE5 Position              */
#define GPIO_MODE_MODE5_Msk              (0x3ul << GPIO_MODE_MODE5_Pos)                    /*!< GPIO_T::MODE: MODE5 Mask                  */

#define GPIO_MODE_MODE6_Pos              (12)                                              /*!< GPIO_T::MODE: MODE6 Position              */
#define GPIO_MODE_MODE6_Msk              (0x3ul << GPIO_MODE_MODE6_Pos)                    /*!< GPIO_T::MODE: MODE6 Mask                  */

#define GPIO_MODE_MODE7_Pos              (14)                                              /*!< GPIO_T::MODE: MODE7 Position              */
#define GPIO_MODE_MODE7_Msk              (0x3ul << GPIO_MODE_MODE7_Pos)                    /*!< GPIO_T::MODE: MODE7 Mask                  */

#define GPIO_MODE_MODE8_Pos              (16)                                              /*!< GPIO_T::MODE: MODE8 Position              */
#define GPIO_MODE_MODE8_Msk              (0x3ul << GPIO_MODE_MODE8_Pos)                    /*!< GPIO_T::MODE: MODE8 Mask                  */

#define GPIO_MODE_MODE9_Pos              (18)                                              /*!< GPIO_T::MODE: MODE9 Position              */
#define GPIO_MODE_MODE9_Msk              (0x3ul << GPIO_MODE_MODE9_Pos)                    /*!< GPIO_T::MODE: MODE9 Mask                  */

#define GPIO_MODE_MODE10_Pos             (20)                                              /*!< GPIO_T::MODE: MODE10 Position             */
#define GPIO_MODE_MODE10_Msk             (0x3ul << GPIO_MODE_MODE10_Pos)                   /*!< GPIO_T::MODE: MODE10 Mask                 */

#define GPIO_MODE_MODE11_Pos             (22)                                              /*!< GPIO_T::MODE: MODE11 Position             */
#define GPIO_MODE_MODE11_Msk             (0x3ul << GPIO_MODE_MODE11_Pos)                   /*!< GPIO_T::MODE: MODE11 Mask                 */

#define GPIO_MODE_MODE12_Pos             (24)                                              /*!< GPIO_T::MODE: MODE12 Position             */
#define GPIO_MODE_MODE12_Msk             (0x3ul << GPIO_MODE_MODE12_Pos)                   /*!< GPIO_T::MODE: MODE12 Mask                 */

#define GPIO_MODE_MODE13_Pos             (26)                                              /*!< GPIO_T::MODE: MODE13 Position             */
#define GPIO_MODE_MODE13_Msk             (0x3ul << GPIO_MODE_MODE13_Pos)                   /*!< GPIO_T::MODE: MODE13 Mask                 */

#define GPIO_MODE_MODE14_Pos             (28)                                              /*!< GPIO_T::MODE: MODE14 Position             */
#define GPIO_MODE_MODE14_Msk             (0x3ul << GPIO_MODE_MODE14_Pos)                   /*!< GPIO_T::MODE: MODE14 Mask                 */

#define GPIO_MODE_MODE15_Pos             (30)                                              /*!< GPIO_T::MODE: MODE15 Position             */
#define GPIO_MODE_MODE15_Msk             (0x3ul << GPIO_MODE_MODE15_Pos)                   /*!< GPIO_T::MODE: MODE15 Mask                 */

#define GPIO_DINOFF_DINOFF0_Pos          (16)                                              /*!< GPIO_T::DINOFF: DINOFF0 Position          */
#define GPIO_DINOFF_DINOFF0_Msk          (0x1ul << GPIO_DINOFF_DINOFF0_Pos)                /*!< GPIO_T::DINOFF: DINOFF0 Mask              */

#define GPIO_DINOFF_DINOFF1_Pos          (17)                                              /*!< GPIO_T::DINOFF: DINOFF1 Position          */
#define GPIO_DINOFF_DINOFF1_Msk          (0x1ul << GPIO_DINOFF_DINOFF1_Pos)                /*!< GPIO_T::DINOFF: DINOFF1 Mask              */

#define GPIO_DINOFF_DINOFF2_Pos          (18)                                              /*!< GPIO_T::DINOFF: DINOFF2 Position          */
#define GPIO_DINOFF_DINOFF2_Msk          (0x1ul << GPIO_DINOFF_DINOFF2_Pos)                /*!< GPIO_T::DINOFF: DINOFF2 Mask              */

#define GPIO_DINOFF_DINOFF3_Pos          (19)                                              /*!< GPIO_T::DINOFF: DINOFF3 Position          */
#define GPIO_DINOFF_DINOFF3_Msk          (0x1ul << GPIO_DINOFF_DINOFF3_Pos)                /*!< GPIO_T::DINOFF: DINOFF3 Mask              */

#define GPIO_DINOFF_DINOFF4_Pos          (20)                                              /*!< GPIO_T::DINOFF: DINOFF4 Position          */
#define GPIO_DINOFF_DINOFF4_Msk          (0x1ul << GPIO_DINOFF_DINOFF4_Pos)                /*!< GPIO_T::DINOFF: DINOFF4 Mask              */

#define GPIO_DINOFF_DINOFF5_Pos          (21)                                              /*!< GPIO_T::DINOFF: DINOFF5 Position          */
#define GPIO_DINOFF_DINOFF5_Msk          (0x1ul << GPIO_DINOFF_DINOFF5_Pos)                /*!< GPIO_T::DINOFF: DINOFF5 Mask              */

#define GPIO_DINOFF_DINOFF6_Pos          (22)                                              /*!< GPIO_T::DINOFF: DINOFF6 Position          */
#define GPIO_DINOFF_DINOFF6_Msk          (0x1ul << GPIO_DINOFF_DINOFF6_Pos)                /*!< GPIO_T::DINOFF: DINOFF6 Mask              */

#define GPIO_DINOFF_DINOFF7_Pos          (23)                                              /*!< GPIO_T::DINOFF: DINOFF7 Position          */
#define GPIO_DINOFF_DINOFF7_Msk          (0x1ul << GPIO_DINOFF_DINOFF7_Pos)                /*!< GPIO_T::DINOFF: DINOFF7 Mask              */

#define GPIO_DINOFF_DINOFF8_Pos          (24)                                              /*!< GPIO_T::DINOFF: DINOFF8 Position          */
#define GPIO_DINOFF_DINOFF8_Msk          (0x1ul << GPIO_DINOFF_DINOFF8_Pos)                /*!< GPIO_T::DINOFF: DINOFF8 Mask              */

#define GPIO_DINOFF_DINOFF9_Pos          (25)                                              /*!< GPIO_T::DINOFF: DINOFF9 Position          */
#define GPIO_DINOFF_DINOFF9_Msk          (0x1ul << GPIO_DINOFF_DINOFF9_Pos)                /*!< GPIO_T::DINOFF: DINOFF9 Mask              */

#define GPIO_DINOFF_DINOFF10_Pos         (26)                                              /*!< GPIO_T::DINOFF: DINOFF10 Position         */
#define GPIO_DINOFF_DINOFF10_Msk         (0x1ul << GPIO_DINOFF_DINOFF10_Pos)               /*!< GPIO_T::DINOFF: DINOFF10 Mask             */

#define GPIO_DINOFF_DINOFF11_Pos         (27)                                              /*!< GPIO_T::DINOFF: DINOFF11 Position         */
#define GPIO_DINOFF_DINOFF11_Msk         (0x1ul << GPIO_DINOFF_DINOFF11_Pos)               /*!< GPIO_T::DINOFF: DINOFF11 Mask             */

#define GPIO_DINOFF_DINOFF12_Pos         (28)                                              /*!< GPIO_T::DINOFF: DINOFF12 Position         */
#define GPIO_DINOFF_DINOFF12_Msk         (0x1ul << GPIO_DINOFF_DINOFF12_Pos)               /*!< GPIO_T::DINOFF: DINOFF12 Mask             */

#define GPIO_DINOFF_DINOFF13_Pos         (29)                                              /*!< GPIO_T::DINOFF: DINOFF13 Position         */
#define GPIO_DINOFF_DINOFF13_Msk         (0x1ul << GPIO_DINOFF_DINOFF13_Pos)               /*!< GPIO_T::DINOFF: DINOFF13 Mask             */

#define GPIO_DINOFF_DINOFF14_Pos         (30)                                              /*!< GPIO_T::DINOFF: DINOFF14 Position         */
#define GPIO_DINOFF_DINOFF14_Msk         (0x1ul << GPIO_DINOFF_DINOFF14_Pos)               /*!< GPIO_T::DINOFF: DINOFF14 Mask             */

#define GPIO_DINOFF_DINOFF15_Pos         (31)                                              /*!< GPIO_T::DINOFF: DINOFF15 Position         */
#define GPIO_DINOFF_DINOFF15_Msk         (0x1ul << GPIO_DINOFF_DINOFF15_Pos)               /*!< GPIO_T::DINOFF: DINOFF15 Mask             */

#define GPIO_DOUT_DOUT0_Pos              (0)                                               /*!< GPIO_T::DOUT: DOUT0 Position              */
#define GPIO_DOUT_DOUT0_Msk              (0x1ul << GPIO_DOUT_DOUT0_Pos)                    /*!< GPIO_T::DOUT: DOUT0 Mask                  */

#define GPIO_DOUT_DOUT1_Pos              (1)                                               /*!< GPIO_T::DOUT: DOUT1 Position              */
#define GPIO_DOUT_DOUT1_Msk              (0x1ul << GPIO_DOUT_DOUT1_Pos)                    /*!< GPIO_T::DOUT: DOUT1 Mask                  */

#define GPIO_DOUT_DOUT2_Pos              (2)                                               /*!< GPIO_T::DOUT: DOUT2 Position              */
#define GPIO_DOUT_DOUT2_Msk              (0x1ul << GPIO_DOUT_DOUT2_Pos)                    /*!< GPIO_T::DOUT: DOUT2 Mask                  */

#define GPIO_DOUT_DOUT3_Pos              (3)                                               /*!< GPIO_T::DOUT: DOUT3 Position              */
#define GPIO_DOUT_DOUT3_Msk              (0x1ul << GPIO_DOUT_DOUT3_Pos)                    /*!< GPIO_T::DOUT: DOUT3 Mask                  */

#define GPIO_DOUT_DOUT4_Pos              (4)                                               /*!< GPIO_T::DOUT: DOUT4 Position              */
#define GPIO_DOUT_DOUT4_Msk              (0x1ul << GPIO_DOUT_DOUT4_Pos)                    /*!< GPIO_T::DOUT: DOUT4 Mask                  */

#define GPIO_DOUT_DOUT5_Pos              (5)                                               /*!< GPIO_T::DOUT: DOUT5 Position              */
#define GPIO_DOUT_DOUT5_Msk              (0x1ul << GPIO_DOUT_DOUT5_Pos)                    /*!< GPIO_T::DOUT: DOUT5 Mask                  */

#define GPIO_DOUT_DOUT6_Pos              (6)                                               /*!< GPIO_T::DOUT: DOUT6 Position              */
#define GPIO_DOUT_DOUT6_Msk              (0x1ul << GPIO_DOUT_DOUT6_Pos)                    /*!< GPIO_T::DOUT: DOUT6 Mask                  */

#define GPIO_DOUT_DOUT7_Pos              (7)                                               /*!< GPIO_T::DOUT: DOUT7 Position              */
#define GPIO_DOUT_DOUT7_Msk              (0x1ul << GPIO_DOUT_DOUT7_Pos)                    /*!< GPIO_T::DOUT: DOUT7 Mask                  */

#define GPIO_DOUT_DOUT8_Pos              (8)                                               /*!< GPIO_T::DOUT: DOUT8 Position              */
#define GPIO_DOUT_DOUT8_Msk              (0x1ul << GPIO_DOUT_DOUT8_Pos)                    /*!< GPIO_T::DOUT: DOUT8 Mask                  */

#define GPIO_DOUT_DOUT9_Pos              (9)                                               /*!< GPIO_T::DOUT: DOUT9 Position              */
#define GPIO_DOUT_DOUT9_Msk              (0x1ul << GPIO_DOUT_DOUT9_Pos)                    /*!< GPIO_T::DOUT: DOUT9 Mask                  */

#define GPIO_DOUT_DOUT10_Pos             (10)                                              /*!< GPIO_T::DOUT: DOUT10 Position             */
#define GPIO_DOUT_DOUT10_Msk             (0x1ul << GPIO_DOUT_DOUT10_Pos)                   /*!< GPIO_T::DOUT: DOUT10 Mask                 */

#define GPIO_DOUT_DOUT11_Pos             (11)                                              /*!< GPIO_T::DOUT: DOUT11 Position             */
#define GPIO_DOUT_DOUT11_Msk             (0x1ul << GPIO_DOUT_DOUT11_Pos)                   /*!< GPIO_T::DOUT: DOUT11 Mask                 */

#define GPIO_DOUT_DOUT12_Pos             (12)                                              /*!< GPIO_T::DOUT: DOUT12 Position             */
#define GPIO_DOUT_DOUT12_Msk             (0x1ul << GPIO_DOUT_DOUT12_Pos)                   /*!< GPIO_T::DOUT: DOUT12 Mask                 */

#define GPIO_DOUT_DOUT13_Pos             (13)                                              /*!< GPIO_T::DOUT: DOUT13 Position             */
#define GPIO_DOUT_DOUT13_Msk             (0x1ul << GPIO_DOUT_DOUT13_Pos)                   /*!< GPIO_T::DOUT: DOUT13 Mask                 */

#define GPIO_DOUT_DOUT14_Pos             (14)                                              /*!< GPIO_T::DOUT: DOUT14 Position             */
#define GPIO_DOUT_DOUT14_Msk             (0x1ul << GPIO_DOUT_DOUT14_Pos)                   /*!< GPIO_T::DOUT: DOUT14 Mask                 */

#define GPIO_DOUT_DOUT15_Pos             (15)                                              /*!< GPIO_T::DOUT: DOUT15 Position             */
#define GPIO_DOUT_DOUT15_Msk             (0x1ul << GPIO_DOUT_DOUT15_Pos)                   /*!< GPIO_T::DOUT: DOUT15 Mask                 */

#define GPIO_DATMSK_DATMSK0_Pos          (0)                                               /*!< GPIO_T::DATMSK: DATMSK0 Position       */
#define GPIO_DATMSK_DATMSK0_Msk          (0x1ul << GPIO_DATMSK_DATMSK0_Pos)                /*!< GPIO_T::DATMSK: DATMSK0 Mask           */

#define GPIO_DATMSK_DATMSK1_Pos          (1)                                               /*!< GPIO_T::DATMSK: DATMSK1 Position       */
#define GPIO_DATMSK_DATMSK1_Msk          (0x1ul << GPIO_DATMSK_DATMSK1_Pos)                /*!< GPIO_T::DATMSK: DATMSK1 Mask           */

#define GPIO_DATMSK_DATMSK2_Pos          (2)                                               /*!< GPIO_T::DATMSK: DATMSK2 Position       */
#define GPIO_DATMSK_DATMSK2_Msk          (0x1ul << GPIO_DATMSK_DATMSK2_Pos)                /*!< GPIO_T::DATMSK: DATMSK2 Mask           */

#define GPIO_DATMSK_DATMSK3_Pos          (3)                                               /*!< GPIO_T::DATMSK: DATMSK3 Position       */
#define GPIO_DATMSK_DATMSK3_Msk          (0x1ul << GPIO_DATMSK_DATMSK3_Pos)                /*!< GPIO_T::DATMSK: DATMSK3 Mask           */

#define GPIO_DATMSK_DATMSK4_Pos          (4)                                               /*!< GPIO_T::DATMSK: DATMSK4 Position       */
#define GPIO_DATMSK_DATMSK4_Msk          (0x1ul << GPIO_DATMSK_DATMSK4_Pos)                /*!< GPIO_T::DATMSK: DATMSK4 Mask           */

#define GPIO_DATMSK_DATMSK5_Pos          (5)                                               /*!< GPIO_T::DATMSK: DATMSK5 Position       */
#define GPIO_DATMSK_DATMSK5_Msk          (0x1ul << GPIO_DATMSK_DATMSK5_Pos)                /*!< GPIO_T::DATMSK: DATMSK5 Mask           */

#define GPIO_DATMSK_DATMSK6_Pos          (6)                                               /*!< GPIO_T::DATMSK: DATMSK6 Position       */
#define GPIO_DATMSK_DATMSK6_Msk          (0x1ul << GPIO_DATMSK_DATMSK6_Pos)                /*!< GPIO_T::DATMSK: DATMSK6 Mask           */

#define GPIO_DATMSK_DATMSK7_Pos          (7)                                               /*!< GPIO_T::DATMSK: DATMSK7 Position       */
#define GPIO_DATMSK_DATMSK7_Msk          (0x1ul << GPIO_DATMSK_DATMSK7_Pos)                /*!< GPIO_T::DATMSK: DATMSK7 Mask           */

#define GPIO_DATMSK_DATMSK8_Pos          (8)                                               /*!< GPIO_T::DATMSK: DATMSK8 Position       */
#define GPIO_DATMSK_DATMSK8_Msk          (0x1ul << GPIO_DATMSK_DATMSK8_Pos)                /*!< GPIO_T::DATMSK: DATMSK8 Mask           */

#define GPIO_DATMSK_DATMSK9_Pos          (9)                                               /*!< GPIO_T::DATMSK: DATMSK9 Position       */
#define GPIO_DATMSK_DATMSK9_Msk          (0x1ul << GPIO_DATMSK_DATMSK9_Pos)                /*!< GPIO_T::DATMSK: DATMSK9 Mask           */

#define GPIO_DATMSK_DATMSK10_Pos         (10)                                              /*!< GPIO_T::DATMSK: DATMSK10 Position      */
#define GPIO_DATMSK_DATMSK10_Msk         (0x1ul << GPIO_DATMSK_DATMSK10_Pos)               /*!< GPIO_T::DATMSK: DATMSK10 Mask          */

#define GPIO_DATMSK_DATMSK11_Pos         (11)                                              /*!< GPIO_T::DATMSK: DATMSK11 Position      */
#define GPIO_DATMSK_DATMSK11_Msk         (0x1ul << GPIO_DATMSK_DATMSK11_Pos)               /*!< GPIO_T::DATMSK: DATMSK11 Mask          */

#define GPIO_DATMSK_DATMSK12_Pos         (12)                                              /*!< GPIO_T::DATMSK: DATMSK12 Position      */
#define GPIO_DATMSK_DATMSK12_Msk         (0x1ul << GPIO_DATMSK_DATMSK12_Pos)               /*!< GPIO_T::DATMSK: DATMSK12 Mask          */

#define GPIO_DATMSK_DATMSK13_Pos         (13)                                              /*!< GPIO_T::DATMSK: DATMSK13 Position      */
#define GPIO_DATMSK_DATMSK13_Msk         (0x1ul << GPIO_DATMSK_DATMSK13_Pos)               /*!< GPIO_T::DATMSK: DATMSK13 Mask          */

#define GPIO_DATMSK_DATMSK14_Pos         (14)                                              /*!< GPIO_T::DATMSK: DATMSK14 Position      */
#define GPIO_DATMSK_DATMSK14_Msk         (0x1ul << GPIO_DATMSK_DATMSK14_Pos)               /*!< GPIO_T::DATMSK: DATMSK14 Mask          */

#define GPIO_DATMSK_DATMSK15_Pos         (15)                                              /*!< GPIO_T::DATMSK: DATMSK15 Position      */
#define GPIO_DATMSK_DATMSK15_Msk         (0x1ul << GPIO_DATMSK_DATMSK15_Pos)               /*!< GPIO_T::DATMSK: DATMSK15 Mask          */

#define GPIO_PIN_PIN0_Pos                (0)                                               /*!< GPIO_T::PIN: PIN0 Position             */
#define GPIO_PIN_PIN0_Msk                (0x1ul << GPIO_PIN_PIN0_Pos)                      /*!< GPIO_T::PIN: PIN0 Mask                 */

#define GPIO_PIN_PIN1_Pos                (1)                                               /*!< GPIO_T::PIN: PIN1 Position             */
#define GPIO_PIN_PIN1_Msk                (0x1ul << GPIO_PIN_PIN1_Pos)                      /*!< GPIO_T::PIN: PIN1 Mask                 */

#define GPIO_PIN_PIN2_Pos                (2)                                               /*!< GPIO_T::PIN: PIN2 Position             */
#define GPIO_PIN_PIN2_Msk                (0x1ul << GPIO_PIN_PIN2_Pos)                      /*!< GPIO_T::PIN: PIN2 Mask                 */

#define GPIO_PIN_PIN3_Pos                (3)                                               /*!< GPIO_T::PIN: PIN3 Position             */
#define GPIO_PIN_PIN3_Msk                (0x1ul << GPIO_PIN_PIN3_Pos)                      /*!< GPIO_T::PIN: PIN3 Mask                 */

#define GPIO_PIN_PIN4_Pos                (4)                                               /*!< GPIO_T::PIN: PIN4 Position             */
#define GPIO_PIN_PIN4_Msk                (0x1ul << GPIO_PIN_PIN4_Pos)                      /*!< GPIO_T::PIN: PIN4 Mask                 */

#define GPIO_PIN_PIN5_Pos                (5)                                               /*!< GPIO_T::PIN: PIN5 Position             */
#define GPIO_PIN_PIN5_Msk                (0x1ul << GPIO_PIN_PIN5_Pos)                      /*!< GPIO_T::PIN: PIN5 Mask                 */

#define GPIO_PIN_PIN6_Pos                (6)                                               /*!< GPIO_T::PIN: PIN6 Position             */
#define GPIO_PIN_PIN6_Msk                (0x1ul << GPIO_PIN_PIN6_Pos)                      /*!< GPIO_T::PIN: PIN6 Mask                 */

#define GPIO_PIN_PIN7_Pos                (7)                                               /*!< GPIO_T::PIN: PIN7 Position             */
#define GPIO_PIN_PIN7_Msk                (0x1ul << GPIO_PIN_PIN7_Pos)                      /*!< GPIO_T::PIN: PIN7 Mask                 */

#define GPIO_PIN_PIN8_Pos                (8)                                               /*!< GPIO_T::PIN: PIN8 Position             */
#define GPIO_PIN_PIN8_Msk                (0x1ul << GPIO_PIN_PIN8_Pos)                      /*!< GPIO_T::PIN: PIN8 Mask                 */

#define GPIO_PIN_PIN9_Pos                (9)                                               /*!< GPIO_T::PIN: PIN9 Position             */
#define GPIO_PIN_PIN9_Msk                (0x1ul << GPIO_PIN_PIN9_Pos)                      /*!< GPIO_T::PIN: PIN9 Mask                 */

#define GPIO_PIN_PIN10_Pos               (10)                                              /*!< GPIO_T::PIN: PIN10 Position            */
#define GPIO_PIN_PIN10_Msk               (0x1ul << GPIO_PIN_PIN10_Pos)                     /*!< GPIO_T::PIN: PIN10 Mask                */

#define GPIO_PIN_PIN11_Pos               (11)                                              /*!< GPIO_T::PIN: PIN11 Position            */
#define GPIO_PIN_PIN11_Msk               (0x1ul << GPIO_PIN_PIN11_Pos)                     /*!< GPIO_T::PIN: PIN11 Mask                */

#define GPIO_PIN_PIN12_Pos               (12)                                              /*!< GPIO_T::PIN: PIN12 Position            */
#define GPIO_PIN_PIN12_Msk               (0x1ul << GPIO_PIN_PIN12_Pos)                     /*!< GPIO_T::PIN: PIN12 Mask                */

#define GPIO_PIN_PIN13_Pos               (13)                                              /*!< GPIO_T::PIN: PIN13 Position            */
#define GPIO_PIN_PIN13_Msk               (0x1ul << GPIO_PIN_PIN13_Pos)                     /*!< GPIO_T::PIN: PIN13 Mask                */

#define GPIO_PIN_PIN14_Pos               (14)                                              /*!< GPIO_T::PIN: PIN14 Position            */
#define GPIO_PIN_PIN14_Msk               (0x1ul << GPIO_PIN_PIN14_Pos)                     /*!< GPIO_T::PIN: PIN14 Mask                */

#define GPIO_PIN_PIN15_Pos               (15)                                              /*!< GPIO_T::PIN: PIN15 Position            */
#define GPIO_PIN_PIN15_Msk               (0x1ul << GPIO_PIN_PIN15_Pos)                     /*!< GPIO_T::PIN: PIN15 Mask                */

#define GPIO_DBEN_DBEN0_Pos              (0)                                               /*!< GPIO_T::DBEN: DBEN0 Position           */
#define GPIO_DBEN_DBEN0_Msk              (0x1ul << GPIO_DBEN_DBEN0_Pos)                    /*!< GPIO_T::DBEN: DBEN0 Mask               */

#define GPIO_DBEN_DBEN1_Pos              (1)                                               /*!< GPIO_T::DBEN: DBEN1 Position           */
#define GPIO_DBEN_DBEN1_Msk              (0x1ul << GPIO_DBEN_DBEN1_Pos)                    /*!< GPIO_T::DBEN: DBEN1 Mask               */

#define GPIO_DBEN_DBEN2_Pos              (2)                                               /*!< GPIO_T::DBEN: DBEN2 Position           */
#define GPIO_DBEN_DBEN2_Msk              (0x1ul << GPIO_DBEN_DBEN2_Pos)                    /*!< GPIO_T::DBEN: DBEN2 Mask               */

#define GPIO_DBEN_DBEN3_Pos              (3)                                               /*!< GPIO_T::DBEN: DBEN3 Position           */
#define GPIO_DBEN_DBEN3_Msk              (0x1ul << GPIO_DBEN_DBEN3_Pos)                    /*!< GPIO_T::DBEN: DBEN3 Mask               */

#define GPIO_DBEN_DBEN4_Pos              (4)                                               /*!< GPIO_T::DBEN: DBEN4 Position           */
#define GPIO_DBEN_DBEN4_Msk              (0x1ul << GPIO_DBEN_DBEN4_Pos)                    /*!< GPIO_T::DBEN: DBEN4 Mask               */

#define GPIO_DBEN_DBEN5_Pos              (5)                                               /*!< GPIO_T::DBEN: DBEN5 Position           */
#define GPIO_DBEN_DBEN5_Msk              (0x1ul << GPIO_DBEN_DBEN5_Pos)                    /*!< GPIO_T::DBEN: DBEN5 Mask               */

#define GPIO_DBEN_DBEN6_Pos              (6)                                               /*!< GPIO_T::DBEN: DBEN6 Position           */
#define GPIO_DBEN_DBEN6_Msk              (0x1ul << GPIO_DBEN_DBEN6_Pos)                    /*!< GPIO_T::DBEN: DBEN6 Mask               */

#define GPIO_DBEN_DBEN7_Pos              (7)                                               /*!< GPIO_T::DBEN: DBEN7 Position           */
#define GPIO_DBEN_DBEN7_Msk              (0x1ul << GPIO_DBEN_DBEN7_Pos)                    /*!< GPIO_T::DBEN: DBEN7 Mask               */

#define GPIO_DBEN_DBEN8_Pos              (8)                                               /*!< GPIO_T::DBEN: DBEN8 Position           */
#define GPIO_DBEN_DBEN8_Msk              (0x1ul << GPIO_DBEN_DBEN8_Pos)                    /*!< GPIO_T::DBEN: DBEN8 Mask               */

#define GPIO_DBEN_DBEN9_Pos              (9)                                               /*!< GPIO_T::DBEN: DBEN9 Position           */
#define GPIO_DBEN_DBEN9_Msk              (0x1ul << GPIO_DBEN_DBEN9_Pos)                    /*!< GPIO_T::DBEN: DBEN9 Mask               */

#define GPIO_DBEN_DBEN10_Pos             (10)                                              /*!< GPIO_T::DBEN: DBEN10 Position          */
#define GPIO_DBEN_DBEN10_Msk             (0x1ul << GPIO_DBEN_DBEN10_Pos)                   /*!< GPIO_T::DBEN: DBEN10 Mask              */

#define GPIO_DBEN_DBEN11_Pos             (11)                                              /*!< GPIO_T::DBEN: DBEN11 Position          */
#define GPIO_DBEN_DBEN11_Msk             (0x1ul << GPIO_DBEN_DBEN11_Pos)                   /*!< GPIO_T::DBEN: DBEN11 Mask              */

#define GPIO_DBEN_DBEN12_Pos             (12)                                              /*!< GPIO_T::DBEN: DBEN12 Position          */
#define GPIO_DBEN_DBEN12_Msk             (0x1ul << GPIO_DBEN_DBEN12_Pos)                   /*!< GPIO_T::DBEN: DBEN12 Mask              */

#define GPIO_DBEN_DBEN13_Pos             (13)                                              /*!< GPIO_T::DBEN: DBEN13 Position          */
#define GPIO_DBEN_DBEN13_Msk             (0x1ul << GPIO_DBEN_DBEN13_Pos)                   /*!< GPIO_T::DBEN: DBEN13 Mask              */

#define GPIO_DBEN_DBEN14_Pos             (14)                                              /*!< GPIO_T::DBEN: DBEN14 Position          */
#define GPIO_DBEN_DBEN14_Msk             (0x1ul << GPIO_DBEN_DBEN14_Pos)                   /*!< GPIO_T::DBEN: DBEN14 Mask              */

#define GPIO_DBEN_DBEN15_Pos             (15)                                              /*!< GPIO_T::DBEN: DBEN15 Position          */
#define GPIO_DBEN_DBEN15_Msk             (0x1ul << GPIO_DBEN_DBEN15_Pos)                   /*!< GPIO_T::DBEN: DBEN15 Mask              */

#define GPIO_INTTYPE_TYPE0_Pos           (0)                                               /*!< GPIO_T::INTTYPE: TYPE0 Position        */
#define GPIO_INTTYPE_TYPE0_Msk           (0x1ul << GPIO_INTTYPE_TYPE0_Pos)                 /*!< GPIO_T::INTTYPE: TYPE0 Mask            */

#define GPIO_INTTYPE_TYPE1_Pos           (1)                                               /*!< GPIO_T::INTTYPE: TYPE1 Position        */
#define GPIO_INTTYPE_TYPE1_Msk           (0x1ul << GPIO_INTTYPE_TYPE1_Pos)                 /*!< GPIO_T::INTTYPE: TYPE1 Mask            */

#define GPIO_INTTYPE_TYPE2_Pos           (2)                                               /*!< GPIO_T::INTTYPE: TYPE2 Position        */
#define GPIO_INTTYPE_TYPE2_Msk           (0x1ul << GPIO_INTTYPE_TYPE2_Pos)                 /*!< GPIO_T::INTTYPE: TYPE2 Mask            */

#define GPIO_INTTYPE_TYPE3_Pos           (3)                                               /*!< GPIO_T::INTTYPE: TYPE3 Position        */
#define GPIO_INTTYPE_TYPE3_Msk           (0x1ul << GPIO_INTTYPE_TYPE3_Pos)                 /*!< GPIO_T::INTTYPE: TYPE3 Mask            */

#define GPIO_INTTYPE_TYPE4_Pos           (4)                                               /*!< GPIO_T::INTTYPE: TYPE4 Position        */
#define GPIO_INTTYPE_TYPE4_Msk           (0x1ul << GPIO_INTTYPE_TYPE4_Pos)                 /*!< GPIO_T::INTTYPE: TYPE4 Mask            */

#define GPIO_INTTYPE_TYPE5_Pos           (5)                                               /*!< GPIO_T::INTTYPE: TYPE5 Position        */
#define GPIO_INTTYPE_TYPE5_Msk           (0x1ul << GPIO_INTTYPE_TYPE5_Pos)                 /*!< GPIO_T::INTTYPE: TYPE5 Mask            */

#define GPIO_INTTYPE_TYPE6_Pos           (6)                                               /*!< GPIO_T::INTTYPE: TYPE6 Position        */
#define GPIO_INTTYPE_TYPE6_Msk           (0x1ul << GPIO_INTTYPE_TYPE6_Pos)                 /*!< GPIO_T::INTTYPE: TYPE6 Mask            */

#define GPIO_INTTYPE_TYPE7_Pos           (7)                                               /*!< GPIO_T::INTTYPE: TYPE7 Position        */
#define GPIO_INTTYPE_TYPE7_Msk           (0x1ul << GPIO_INTTYPE_TYPE7_Pos)                 /*!< GPIO_T::INTTYPE: TYPE7 Mask            */

#define GPIO_INTTYPE_TYPE8_Pos           (8)                                               /*!< GPIO_T::INTTYPE: TYPE8 Position        */
#define GPIO_INTTYPE_TYPE8_Msk           (0x1ul << GPIO_INTTYPE_TYPE8_Pos)                 /*!< GPIO_T::INTTYPE: TYPE8 Mask            */

#define GPIO_INTTYPE_TYPE9_Pos           (9)                                               /*!< GPIO_T::INTTYPE: TYPE9 Position        */
#define GPIO_INTTYPE_TYPE9_Msk           (0x1ul << GPIO_INTTYPE_TYPE9_Pos)                 /*!< GPIO_T::INTTYPE: TYPE9 Mask            */

#define GPIO_INTTYPE_TYPE10_Pos          (10)                                              /*!< GPIO_T::INTTYPE: TYPE10 Position       */
#define GPIO_INTTYPE_TYPE10_Msk          (0x1ul << GPIO_INTTYPE_TYPE10_Pos)                /*!< GPIO_T::INTTYPE: TYPE10 Mask           */

#define GPIO_INTTYPE_TYPE11_Pos          (11)                                              /*!< GPIO_T::INTTYPE: TYPE11 Position       */
#define GPIO_INTTYPE_TYPE11_Msk          (0x1ul << GPIO_INTTYPE_TYPE11_Pos)                /*!< GPIO_T::INTTYPE: TYPE11 Mask           */

#define GPIO_INTTYPE_TYPE12_Pos          (12)                                              /*!< GPIO_T::INTTYPE: TYPE12 Position       */
#define GPIO_INTTYPE_TYPE12_Msk          (0x1ul << GPIO_INTTYPE_TYPE12_Pos)                /*!< GPIO_T::INTTYPE: TYPE12 Mask           */

#define GPIO_INTTYPE_TYPE13_Pos          (13)                                              /*!< GPIO_T::INTTYPE: TYPE13 Position       */
#define GPIO_INTTYPE_TYPE13_Msk          (0x1ul << GPIO_INTTYPE_TYPE13_Pos)                /*!< GPIO_T::INTTYPE: TYPE13 Mask           */

#define GPIO_INTTYPE_TYPE14_Pos          (14)                                              /*!< GPIO_T::INTTYPE: TYPE14 Position       */
#define GPIO_INTTYPE_TYPE14_Msk          (0x1ul << GPIO_INTTYPE_TYPE14_Pos)                /*!< GPIO_T::INTTYPE: TYPE14 Mask           */

#define GPIO_INTTYPE_TYPE15_Pos          (15)                                              /*!< GPIO_T::INTTYPE: TYPE15 Position       */
#define GPIO_INTTYPE_TYPE15_Msk          (0x1ul << GPIO_INTTYPE_TYPE15_Pos)                /*!< GPIO_T::INTTYPE: TYPE15 Mask           */

#define GPIO_INTEN_FLIEN0_Pos            (0)                                               /*!< GPIO_T::INTEN: FLIEN0 Position         */
#define GPIO_INTEN_FLIEN0_Msk            (0x1ul << GPIO_INTEN_FLIEN0_Pos)                  /*!< GPIO_T::INTEN: FLIEN0 Mask             */

#define GPIO_INTEN_FLIEN1_Pos            (1)                                               /*!< GPIO_T::INTEN: FLIEN1 Position         */
#define GPIO_INTEN_FLIEN1_Msk            (0x1ul << GPIO_INTEN_FLIEN1_Pos)                  /*!< GPIO_T::INTEN: FLIEN1 Mask             */

#define GPIO_INTEN_FLIEN2_Pos            (2)                                               /*!< GPIO_T::INTEN: FLIEN2 Position         */
#define GPIO_INTEN_FLIEN2_Msk            (0x1ul << GPIO_INTEN_FLIEN2_Pos)                  /*!< GPIO_T::INTEN: FLIEN2 Mask             */

#define GPIO_INTEN_FLIEN3_Pos            (3)                                               /*!< GPIO_T::INTEN: FLIEN3 Position         */
#define GPIO_INTEN_FLIEN3_Msk            (0x1ul << GPIO_INTEN_FLIEN3_Pos)                  /*!< GPIO_T::INTEN: FLIEN3 Mask             */

#define GPIO_INTEN_FLIEN4_Pos            (4)                                               /*!< GPIO_T::INTEN: FLIEN4 Position         */
#define GPIO_INTEN_FLIEN4_Msk            (0x1ul << GPIO_INTEN_FLIEN4_Pos)                  /*!< GPIO_T::INTEN: FLIEN4 Mask             */

#define GPIO_INTEN_FLIEN5_Pos            (5)                                               /*!< GPIO_T::INTEN: FLIEN5 Position         */
#define GPIO_INTEN_FLIEN5_Msk            (0x1ul << GPIO_INTEN_FLIEN5_Pos)                  /*!< GPIO_T::INTEN: FLIEN5 Mask             */

#define GPIO_INTEN_FLIEN6_Pos            (6)                                               /*!< GPIO_T::INTEN: FLIEN6 Position         */
#define GPIO_INTEN_FLIEN6_Msk            (0x1ul << GPIO_INTEN_FLIEN6_Pos)                  /*!< GPIO_T::INTEN: FLIEN6 Mask             */

#define GPIO_INTEN_FLIEN7_Pos            (7)                                               /*!< GPIO_T::INTEN: FLIEN7 Position         */
#define GPIO_INTEN_FLIEN7_Msk            (0x1ul << GPIO_INTEN_FLIEN7_Pos)                  /*!< GPIO_T::INTEN: FLIEN7 Mask             */

#define GPIO_INTEN_FLIEN8_Pos            (8)                                               /*!< GPIO_T::INTEN: FLIEN8 Position         */
#define GPIO_INTEN_FLIEN8_Msk            (0x1ul << GPIO_INTEN_FLIEN8_Pos)                  /*!< GPIO_T::INTEN: FLIEN8 Mask             */

#define GPIO_INTEN_FLIEN9_Pos            (9)                                               /*!< GPIO_T::INTEN: FLIEN9 Position         */
#define GPIO_INTEN_FLIEN9_Msk            (0x1ul << GPIO_INTEN_FLIEN9_Pos)                  /*!< GPIO_T::INTEN: FLIEN9 Mask             */

#define GPIO_INTEN_FLIEN10_Pos           (10)                                              /*!< GPIO_T::INTEN: FLIEN10 Position        */
#define GPIO_INTEN_FLIEN10_Msk           (0x1ul << GPIO_INTEN_FLIEN10_Pos)                 /*!< GPIO_T::INTEN: FLIEN10 Mask            */

#define GPIO_INTEN_FLIEN11_Pos           (11)                                              /*!< GPIO_T::INTEN: FLIEN11 Position        */
#define GPIO_INTEN_FLIEN11_Msk           (0x1ul << GPIO_INTEN_FLIEN11_Pos)                 /*!< GPIO_T::INTEN: FLIEN11 Mask            */

#define GPIO_INTEN_FLIEN12_Pos           (12)                                              /*!< GPIO_T::INTEN: FLIEN12 Position        */
#define GPIO_INTEN_FLIEN12_Msk           (0x1ul << GPIO_INTEN_FLIEN12_Pos)                 /*!< GPIO_T::INTEN: FLIEN12 Mask            */

#define GPIO_INTEN_FLIEN13_Pos           (13)                                              /*!< GPIO_T::INTEN: FLIEN13 Position        */
#define GPIO_INTEN_FLIEN13_Msk           (0x1ul << GPIO_INTEN_FLIEN13_Pos)                 /*!< GPIO_T::INTEN: FLIEN13 Mask            */

#define GPIO_INTEN_FLIEN14_Pos           (14)                                              /*!< GPIO_T::INTEN: FLIEN14 Position        */
#define GPIO_INTEN_FLIEN14_Msk           (0x1ul << GPIO_INTEN_FLIEN14_Pos)                 /*!< GPIO_T::INTEN: FLIEN14 Mask            */

#define GPIO_INTEN_FLIEN15_Pos           (15)                                              /*!< GPIO_T::INTEN: FLIEN15 Position        */
#define GPIO_INTEN_FLIEN15_Msk           (0x1ul << GPIO_INTEN_FLIEN15_Pos)                 /*!< GPIO_T::INTEN: FLIEN15 Mask            */

#define GPIO_INTEN_RHIEN0_Pos            (16)                                              /*!< GPIO_T::INTEN: RHIEN0 Position         */
#define GPIO_INTEN_RHIEN0_Msk            (0x1ul << GPIO_INTEN_RHIEN0_Pos)                  /*!< GPIO_T::INTEN: RHIEN0 Mask             */

#define GPIO_INTEN_RHIEN1_Pos            (17)                                              /*!< GPIO_T::INTEN: RHIEN1 Position         */
#define GPIO_INTEN_RHIEN1_Msk            (0x1ul << GPIO_INTEN_RHIEN1_Pos)                  /*!< GPIO_T::INTEN: RHIEN1 Mask             */

#define GPIO_INTEN_RHIEN2_Pos            (18)                                              /*!< GPIO_T::INTEN: RHIEN2 Position         */
#define GPIO_INTEN_RHIEN2_Msk            (0x1ul << GPIO_INTEN_RHIEN2_Pos)                  /*!< GPIO_T::INTEN: RHIEN2 Mask             */

#define GPIO_INTEN_RHIEN3_Pos            (19)                                              /*!< GPIO_T::INTEN: RHIEN3 Position         */
#define GPIO_INTEN_RHIEN3_Msk            (0x1ul << GPIO_INTEN_RHIEN3_Pos)                  /*!< GPIO_T::INTEN: RHIEN3 Mask             */

#define GPIO_INTEN_RHIEN4_Pos            (20)                                              /*!< GPIO_T::INTEN: RHIEN4 Position         */
#define GPIO_INTEN_RHIEN4_Msk            (0x1ul << GPIO_INTEN_RHIEN4_Pos)                  /*!< GPIO_T::INTEN: RHIEN4 Mask             */

#define GPIO_INTEN_RHIEN5_Pos            (21)                                              /*!< GPIO_T::INTEN: RHIEN5 Position         */
#define GPIO_INTEN_RHIEN5_Msk            (0x1ul << GPIO_INTEN_RHIEN5_Pos)                  /*!< GPIO_T::INTEN: RHIEN5 Mask             */

#define GPIO_INTEN_RHIEN6_Pos            (22)                                              /*!< GPIO_T::INTEN: RHIEN6 Position         */
#define GPIO_INTEN_RHIEN6_Msk            (0x1ul << GPIO_INTEN_RHIEN6_Pos)                  /*!< GPIO_T::INTEN: RHIEN6 Mask             */

#define GPIO_INTEN_RHIEN7_Pos            (23)                                              /*!< GPIO_T::INTEN: RHIEN7 Position         */
#define GPIO_INTEN_RHIEN7_Msk            (0x1ul << GPIO_INTEN_RHIEN7_Pos)                  /*!< GPIO_T::INTEN: RHIEN7 Mask             */

#define GPIO_INTEN_RHIEN8_Pos            (24)                                              /*!< GPIO_T::INTEN: RHIEN8 Position         */
#define GPIO_INTEN_RHIEN8_Msk            (0x1ul << GPIO_INTEN_RHIEN8_Pos)                  /*!< GPIO_T::INTEN: RHIEN8 Mask             */

#define GPIO_INTEN_RHIEN9_Pos            (25)                                              /*!< GPIO_T::INTEN: RHIEN9 Position         */
#define GPIO_INTEN_RHIEN9_Msk            (0x1ul << GPIO_INTEN_RHIEN9_Pos)                  /*!< GPIO_T::INTEN: RHIEN9 Mask             */

#define GPIO_INTEN_RHIEN10_Pos           (26)                                              /*!< GPIO_T::INTEN: RHIEN10 Position        */
#define GPIO_INTEN_RHIEN10_Msk           (0x1ul << GPIO_INTEN_RHIEN10_Pos)                 /*!< GPIO_T::INTEN: RHIEN10 Mask            */

#define GPIO_INTEN_RHIEN11_Pos           (27)                                              /*!< GPIO_T::INTEN: RHIEN11 Position        */
#define GPIO_INTEN_RHIEN11_Msk           (0x1ul << GPIO_INTEN_RHIEN11_Pos)                 /*!< GPIO_T::INTEN: RHIEN11 Mask            */

#define GPIO_INTEN_RHIEN12_Pos           (28)                                              /*!< GPIO_T::INTEN: RHIEN12 Position        */
#define GPIO_INTEN_RHIEN12_Msk           (0x1ul << GPIO_INTEN_RHIEN12_Pos)                 /*!< GPIO_T::INTEN: RHIEN12 Mask            */

#define GPIO_INTEN_RHIEN13_Pos           (29)                                              /*!< GPIO_T::INTEN: RHIEN13 Position        */
#define GPIO_INTEN_RHIEN13_Msk           (0x1ul << GPIO_INTEN_RHIEN13_Pos)                 /*!< GPIO_T::INTEN: RHIEN13 Mask            */

#define GPIO_INTEN_RHIEN14_Pos           (30)                                              /*!< GPIO_T::INTEN: RHIEN14 Position        */
#define GPIO_INTEN_RHIEN14_Msk           (0x1ul << GPIO_INTEN_RHIEN14_Pos)                 /*!< GPIO_T::INTEN: RHIEN14 Mask            */

#define GPIO_INTEN_RHIEN15_Pos           (31)                                              /*!< GPIO_T::INTEN: RHIEN15 Position        */
#define GPIO_INTEN_RHIEN15_Msk           (0x1ul << GPIO_INTEN_RHIEN15_Pos)                 /*!< GPIO_T::INTEN: RHIEN15 Mask            */

#define GPIO_INTSRC_INTSRC0_Pos          (0)                                               /*!< GPIO_T::INTSRC: INTSRC0 Position       */
#define GPIO_INTSRC_INTSRC0_Msk          (0x1ul << GPIO_INTSRC_INTSRC0_Pos)                /*!< GPIO_T::INTSRC: INTSRC0 Mask           */

#define GPIO_INTSRC_INTSRC1_Pos          (1)                                               /*!< GPIO_T::INTSRC: INTSRC1 Position       */
#define GPIO_INTSRC_INTSRC1_Msk          (0x1ul << GPIO_INTSRC_INTSRC1_Pos)                /*!< GPIO_T::INTSRC: INTSRC1 Mask           */

#define GPIO_INTSRC_INTSRC2_Pos          (2)                                               /*!< GPIO_T::INTSRC: INTSRC2 Position       */
#define GPIO_INTSRC_INTSRC2_Msk          (0x1ul << GPIO_INTSRC_INTSRC2_Pos)                /*!< GPIO_T::INTSRC: INTSRC2 Mask           */

#define GPIO_INTSRC_INTSRC3_Pos          (3)                                               /*!< GPIO_T::INTSRC: INTSRC3 Position       */
#define GPIO_INTSRC_INTSRC3_Msk          (0x1ul << GPIO_INTSRC_INTSRC3_Pos)                /*!< GPIO_T::INTSRC: INTSRC3 Mask           */

#define GPIO_INTSRC_INTSRC4_Pos          (4)                                               /*!< GPIO_T::INTSRC: INTSRC4 Position       */
#define GPIO_INTSRC_INTSRC4_Msk          (0x1ul << GPIO_INTSRC_INTSRC4_Pos)                /*!< GPIO_T::INTSRC: INTSRC4 Mask           */

#define GPIO_INTSRC_INTSRC5_Pos          (5)                                               /*!< GPIO_T::INTSRC: INTSRC5 Position       */
#define GPIO_INTSRC_INTSRC5_Msk          (0x1ul << GPIO_INTSRC_INTSRC5_Pos)                /*!< GPIO_T::INTSRC: INTSRC5 Mask           */

#define GPIO_INTSRC_INTSRC6_Pos          (6)                                               /*!< GPIO_T::INTSRC: INTSRC6 Position       */
#define GPIO_INTSRC_INTSRC6_Msk          (0x1ul << GPIO_INTSRC_INTSRC6_Pos)                /*!< GPIO_T::INTSRC: INTSRC6 Mask           */

#define GPIO_INTSRC_INTSRC7_Pos          (7)                                               /*!< GPIO_T::INTSRC: INTSRC7 Position       */
#define GPIO_INTSRC_INTSRC7_Msk          (0x1ul << GPIO_INTSRC_INTSRC7_Pos)                /*!< GPIO_T::INTSRC: INTSRC7 Mask           */

#define GPIO_INTSRC_INTSRC8_Pos          (8)                                               /*!< GPIO_T::INTSRC: INTSRC8 Position       */
#define GPIO_INTSRC_INTSRC8_Msk          (0x1ul << GPIO_INTSRC_INTSRC8_Pos)                /*!< GPIO_T::INTSRC: INTSRC8 Mask           */

#define GPIO_INTSRC_INTSRC9_Pos          (9)                                               /*!< GPIO_T::INTSRC: INTSRC9 Position       */
#define GPIO_INTSRC_INTSRC9_Msk          (0x1ul << GPIO_INTSRC_INTSRC9_Pos)                /*!< GPIO_T::INTSRC: INTSRC9 Mask           */

#define GPIO_INTSRC_INTSRC10_Pos         (10)                                              /*!< GPIO_T::INTSRC: INTSRC10 Position      */
#define GPIO_INTSRC_INTSRC10_Msk         (0x1ul << GPIO_INTSRC_INTSRC10_Pos)               /*!< GPIO_T::INTSRC: INTSRC10 Mask          */

#define GPIO_INTSRC_INTSRC11_Pos         (11)                                              /*!< GPIO_T::INTSRC: INTSRC11 Position      */
#define GPIO_INTSRC_INTSRC11_Msk         (0x1ul << GPIO_INTSRC_INTSRC11_Pos)               /*!< GPIO_T::INTSRC: INTSRC11 Mask          */

#define GPIO_INTSRC_INTSRC12_Pos         (12)                                              /*!< GPIO_T::INTSRC: INTSRC12 Position      */
#define GPIO_INTSRC_INTSRC12_Msk         (0x1ul << GPIO_INTSRC_INTSRC12_Pos)               /*!< GPIO_T::INTSRC: INTSRC12 Mask          */

#define GPIO_INTSRC_INTSRC13_Pos         (13)                                              /*!< GPIO_T::INTSRC: INTSRC13 Position      */
#define GPIO_INTSRC_INTSRC13_Msk         (0x1ul << GPIO_INTSRC_INTSRC13_Pos)               /*!< GPIO_T::INTSRC: INTSRC13 Mask          */

#define GPIO_INTSRC_INTSRC14_Pos         (14)                                              /*!< GPIO_T::INTSRC: INTSRC14 Position      */
#define GPIO_INTSRC_INTSRC14_Msk         (0x1ul << GPIO_INTSRC_INTSRC14_Pos)               /*!< GPIO_T::INTSRC: INTSRC14 Mask          */

#define GPIO_INTSRC_INTSRC15_Pos         (15)                                              /*!< GPIO_T::INTSRC: INTSRC15 Position      */
#define GPIO_INTSRC_INTSRC15_Msk         (0x1ul << GPIO_INTSRC_INTSRC15_Pos)               /*!< GPIO_T::INTSRC: INTSRC15 Mask          */

#define GPIO_SMTEN_SMTEN0_Pos            (0)                                               /*!< GPIO_T::SMTEN: SMTEN0 Position         */
#define GPIO_SMTEN_SMTEN0_Msk            (0x1ul << GPIO_SMTEN_SMTEN0_Pos)                  /*!< GPIO_T::SMTEN: SMTEN0 Mask             */

#define GPIO_SMTEN_SMTEN1_Pos            (1)                                               /*!< GPIO_T::SMTEN: SMTEN1 Position         */
#define GPIO_SMTEN_SMTEN1_Msk            (0x1ul << GPIO_SMTEN_SMTEN1_Pos)                  /*!< GPIO_T::SMTEN: SMTEN1 Mask             */

#define GPIO_SMTEN_SMTEN2_Pos            (2)                                               /*!< GPIO_T::SMTEN: SMTEN2 Position         */
#define GPIO_SMTEN_SMTEN2_Msk            (0x1ul << GPIO_SMTEN_SMTEN2_Pos)                  /*!< GPIO_T::SMTEN: SMTEN2 Mask             */

#define GPIO_SMTEN_SMTEN3_Pos            (3)                                               /*!< GPIO_T::SMTEN: SMTEN3 Position         */
#define GPIO_SMTEN_SMTEN3_Msk            (0x1ul << GPIO_SMTEN_SMTEN3_Pos)                  /*!< GPIO_T::SMTEN: SMTEN3 Mask             */

#define GPIO_SMTEN_SMTEN4_Pos            (4)                                               /*!< GPIO_T::SMTEN: SMTEN4 Position         */
#define GPIO_SMTEN_SMTEN4_Msk            (0x1ul << GPIO_SMTEN_SMTEN4_Pos)                  /*!< GPIO_T::SMTEN: SMTEN4 Mask             */

#define GPIO_SMTEN_SMTEN5_Pos            (5)                                               /*!< GPIO_T::SMTEN: SMTEN5 Position         */
#define GPIO_SMTEN_SMTEN5_Msk            (0x1ul << GPIO_SMTEN_SMTEN5_Pos)                  /*!< GPIO_T::SMTEN: SMTEN5 Mask             */

#define GPIO_SMTEN_SMTEN6_Pos            (6)                                               /*!< GPIO_T::SMTEN: SMTEN6 Position         */
#define GPIO_SMTEN_SMTEN6_Msk            (0x1ul << GPIO_SMTEN_SMTEN6_Pos)                  /*!< GPIO_T::SMTEN: SMTEN6 Mask             */

#define GPIO_SMTEN_SMTEN7_Pos            (7)                                               /*!< GPIO_T::SMTEN: SMTEN7 Position         */
#define GPIO_SMTEN_SMTEN7_Msk            (0x1ul << GPIO_SMTEN_SMTEN7_Pos)                  /*!< GPIO_T::SMTEN: SMTEN7 Mask             */

#define GPIO_SMTEN_SMTEN8_Pos            (8)                                               /*!< GPIO_T::SMTEN: SMTEN8 Position         */
#define GPIO_SMTEN_SMTEN8_Msk            (0x1ul << GPIO_SMTEN_SMTEN8_Pos)                  /*!< GPIO_T::SMTEN: SMTEN8 Mask             */

#define GPIO_SMTEN_SMTEN9_Pos            (9)                                               /*!< GPIO_T::SMTEN: SMTEN9 Position         */
#define GPIO_SMTEN_SMTEN9_Msk            (0x1ul << GPIO_SMTEN_SMTEN9_Pos)                  /*!< GPIO_T::SMTEN: SMTEN9 Mask             */

#define GPIO_SMTEN_SMTEN10_Pos           (10)                                              /*!< GPIO_T::SMTEN: SMTEN10 Position        */
#define GPIO_SMTEN_SMTEN10_Msk           (0x1ul << GPIO_SMTEN_SMTEN10_Pos)                 /*!< GPIO_T::SMTEN: SMTEN10 Mask            */

#define GPIO_SMTEN_SMTEN11_Pos           (11)                                              /*!< GPIO_T::SMTEN: SMTEN11 Position        */
#define GPIO_SMTEN_SMTEN11_Msk           (0x1ul << GPIO_SMTEN_SMTEN11_Pos)                 /*!< GPIO_T::SMTEN: SMTEN11 Mask            */

#define GPIO_SMTEN_SMTEN12_Pos           (12)                                              /*!< GPIO_T::SMTEN: SMTEN12 Position        */
#define GPIO_SMTEN_SMTEN12_Msk           (0x1ul << GPIO_SMTEN_SMTEN12_Pos)                 /*!< GPIO_T::SMTEN: SMTEN12 Mask            */

#define GPIO_SMTEN_SMTEN13_Pos           (13)                                              /*!< GPIO_T::SMTEN: SMTEN13 Position        */
#define GPIO_SMTEN_SMTEN13_Msk           (0x1ul << GPIO_SMTEN_SMTEN13_Pos)                 /*!< GPIO_T::SMTEN: SMTEN13 Mask            */

#define GPIO_SMTEN_SMTEN14_Pos           (14)                                              /*!< GPIO_T::SMTEN: SMTEN14 Position        */
#define GPIO_SMTEN_SMTEN14_Msk           (0x1ul << GPIO_SMTEN_SMTEN14_Pos)                 /*!< GPIO_T::SMTEN: SMTEN14 Mask            */

#define GPIO_SMTEN_SMTEN15_Pos           (15)                                              /*!< GPIO_T::SMTEN: SMTEN15 Position        */
#define GPIO_SMTEN_SMTEN15_Msk           (0x1ul << GPIO_SMTEN_SMTEN15_Pos)                 /*!< GPIO_T::SMTEN: SMTEN15 Mask            */

#define GPIO_SLEWCTL_HSREN0_Pos          (0)                                               /*!< GPIO_T::SLEWCTL: HSREN0 Position       */
#define GPIO_SLEWCTL_HSREN0_Msk          (0x3ul << GPIO_SLEWCTL_HSREN0_Pos)                /*!< GPIO_T::SLEWCTL: HSREN0 Mask           */

#define GPIO_SLEWCTL_HSREN1_Pos          (2)                                               /*!< GPIO_T::SLEWCTL: HSREN1 Position       */
#define GPIO_SLEWCTL_HSREN1_Msk          (0x3ul << GPIO_SLEWCTL_HSREN1_Pos)                /*!< GPIO_T::SLEWCTL: HSREN1 Mask           */

#define GPIO_SLEWCTL_HSREN2_Pos          (4)                                               /*!< GPIO_T::SLEWCTL: HSREN2 Position       */
#define GPIO_SLEWCTL_HSREN2_Msk          (0x3ul << GPIO_SLEWCTL_HSREN2_Pos)                /*!< GPIO_T::SLEWCTL: HSREN2 Mask           */

#define GPIO_SLEWCTL_HSREN3_Pos          (6)                                               /*!< GPIO_T::SLEWCTL: HSREN3 Position       */
#define GPIO_SLEWCTL_HSREN3_Msk          (0x3ul << GPIO_SLEWCTL_HSREN3_Pos)                /*!< GPIO_T::SLEWCTL: HSREN3 Mask           */

#define GPIO_SLEWCTL_HSREN4_Pos          (8)                                               /*!< GPIO_T::SLEWCTL: HSREN4 Position       */
#define GPIO_SLEWCTL_HSREN4_Msk          (0x3ul << GPIO_SLEWCTL_HSREN4_Pos)                /*!< GPIO_T::SLEWCTL: HSREN4 Mask           */

#define GPIO_SLEWCTL_HSREN5_Pos          (10)                                              /*!< GPIO_T::SLEWCTL: HSREN5 Position       */
#define GPIO_SLEWCTL_HSREN5_Msk          (0x3ul << GPIO_SLEWCTL_HSREN5_Pos)                /*!< GPIO_T::SLEWCTL: HSREN5 Mask           */

#define GPIO_SLEWCTL_HSREN6_Pos          (12)                                              /*!< GPIO_T::SLEWCTL: HSREN6 Position       */
#define GPIO_SLEWCTL_HSREN6_Msk          (0x3ul << GPIO_SLEWCTL_HSREN6_Pos)                /*!< GPIO_T::SLEWCTL: HSREN6 Mask           */

#define GPIO_SLEWCTL_HSREN7_Pos          (14)                                              /*!< GPIO_T::SLEWCTL: HSREN7 Position       */
#define GPIO_SLEWCTL_HSREN7_Msk          (0x3ul << GPIO_SLEWCTL_HSREN7_Pos)                /*!< GPIO_T::SLEWCTL: HSREN7 Mask           */

#define GPIO_SLEWCTL_HSREN8_Pos          (16)                                              /*!< GPIO_T::SLEWCTL: HSREN8 Position       */
#define GPIO_SLEWCTL_HSREN8_Msk          (0x3ul << GPIO_SLEWCTL_HSREN8_Pos)                /*!< GPIO_T::SLEWCTL: HSREN8 Mask           */

#define GPIO_SLEWCTL_HSREN9_Pos          (18)                                              /*!< GPIO_T::SLEWCTL: HSREN9 Position       */
#define GPIO_SLEWCTL_HSREN9_Msk          (0x3ul << GPIO_SLEWCTL_HSREN9_Pos)                /*!< GPIO_T::SLEWCTL: HSREN9 Mask           */

#define GPIO_SLEWCTL_HSREN10_Pos         (20)                                              /*!< GPIO_T::SLEWCTL: HSREN10 Position      */
#define GPIO_SLEWCTL_HSREN10_Msk         (0x3ul << GPIO_SLEWCTL_HSREN10_Pos)               /*!< GPIO_T::SLEWCTL: HSREN10 Mask          */

#define GPIO_SLEWCTL_HSREN11_Pos         (22)                                              /*!< GPIO_T::SLEWCTL: HSREN11 Position      */
#define GPIO_SLEWCTL_HSREN11_Msk         (0x3ul << GPIO_SLEWCTL_HSREN11_Pos)               /*!< GPIO_T::SLEWCTL: HSREN11 Mask          */

#define GPIO_SLEWCTL_HSREN12_Pos         (24)                                              /*!< GPIO_T::SLEWCTL: HSREN12 Position      */
#define GPIO_SLEWCTL_HSREN12_Msk         (0x3ul << GPIO_SLEWCTL_HSREN12_Pos)               /*!< GPIO_T::SLEWCTL: HSREN12 Mask          */

#define GPIO_SLEWCTL_HSREN13_Pos         (26)                                              /*!< GPIO_T::SLEWCTL: HSREN13 Position      */
#define GPIO_SLEWCTL_HSREN13_Msk         (0x3ul << GPIO_SLEWCTL_HSREN13_Pos)               /*!< GPIO_T::SLEWCTL: HSREN13 Mask          */

#define GPIO_SLEWCTL_HSREN14_Pos         (28)                                              /*!< GPIO_T::SLEWCTL: HSREN14 Position      */
#define GPIO_SLEWCTL_HSREN14_Msk         (0x3ul << GPIO_SLEWCTL_HSREN14_Pos)               /*!< GPIO_T::SLEWCTL: HSREN14 Mask          */

#define GPIO_SLEWCTL_HSREN15_Pos         (30)                                              /*!< GPIO_T::SLEWCTL: HSREN15 Position      */
#define GPIO_SLEWCTL_HSREN15_Msk         (0x3ul << GPIO_SLEWCTL_HSREN15_Pos)               /*!< GPIO_T::SLEWCTL: HSREN15 Mask          */

#define GPIO_PUSEL_PUSEL0_Pos            (0)                                               /*!< GPIO_T::PUSEL: PUSEL0 Position         */
#define GPIO_PUSEL_PUSEL0_Msk            (0x3ul << GPIO_PUSEL_PUSEL0_Pos)                  /*!< GPIO_T::PUSEL: PUSEL0 Mask             */

#define GPIO_PUSEL_PUSEL1_Pos            (2)                                               /*!< GPIO_T::PUSEL: PUSEL1 Position         */
#define GPIO_PUSEL_PUSEL1_Msk            (0x3ul << GPIO_PUSEL_PUSEL1_Pos)                  /*!< GPIO_T::PUSEL: PUSEL1 Mask             */

#define GPIO_PUSEL_PUSEL2_Pos            (4)                                               /*!< GPIO_T::PUSEL: PUSEL2 Position         */
#define GPIO_PUSEL_PUSEL2_Msk            (0x3ul << GPIO_PUSEL_PUSEL2_Pos)                  /*!< GPIO_T::PUSEL: PUSEL2 Mask             */

#define GPIO_PUSEL_PUSEL3_Pos            (6)                                               /*!< GPIO_T::PUSEL: PUSEL3 Position         */
#define GPIO_PUSEL_PUSEL3_Msk            (0x3ul << GPIO_PUSEL_PUSEL3_Pos)                  /*!< GPIO_T::PUSEL: PUSEL3 Mask             */

#define GPIO_PUSEL_PUSEL4_Pos            (8)                                               /*!< GPIO_T::PUSEL: PUSEL4 Position         */
#define GPIO_PUSEL_PUSEL4_Msk            (0x3ul << GPIO_PUSEL_PUSEL4_Pos)                  /*!< GPIO_T::PUSEL: PUSEL4 Mask             */

#define GPIO_PUSEL_PUSEL5_Pos            (10)                                              /*!< GPIO_T::PUSEL: PUSEL5 Position         */
#define GPIO_PUSEL_PUSEL5_Msk            (0x3ul << GPIO_PUSEL_PUSEL5_Pos)                  /*!< GPIO_T::PUSEL: PUSEL5 Mask             */

#define GPIO_PUSEL_PUSEL6_Pos            (12)                                              /*!< GPIO_T::PUSEL: PUSEL6 Position         */
#define GPIO_PUSEL_PUSEL6_Msk            (0x3ul << GPIO_PUSEL_PUSEL6_Pos)                  /*!< GPIO_T::PUSEL: PUSEL6 Mask             */

#define GPIO_PUSEL_PUSEL7_Pos            (14)                                              /*!< GPIO_T::PUSEL: PUSEL7 Position         */
#define GPIO_PUSEL_PUSEL7_Msk            (0x3ul << GPIO_PUSEL_PUSEL7_Pos)                  /*!< GPIO_T::PUSEL: PUSEL7 Mask             */

#define GPIO_PUSEL_PUSEL8_Pos            (16)                                              /*!< GPIO_T::PUSEL: PUSEL8 Position         */
#define GPIO_PUSEL_PUSEL8_Msk            (0x3ul << GPIO_PUSEL_PUSEL8_Pos)                  /*!< GPIO_T::PUSEL: PUSEL8 Mask             */

#define GPIO_PUSEL_PUSEL9_Pos            (18)                                              /*!< GPIO_T::PUSEL: PUSEL9 Position         */
#define GPIO_PUSEL_PUSEL9_Msk            (0x3ul << GPIO_PUSEL_PUSEL9_Pos)                  /*!< GPIO_T::PUSEL: PUSEL9 Mask             */

#define GPIO_PUSEL_PUSEL10_Pos           (20)                                              /*!< GPIO_T::PUSEL: PUSEL10 Position        */
#define GPIO_PUSEL_PUSEL10_Msk           (0x3ul << GPIO_PUSEL_PUSEL10_Pos)                 /*!< GPIO_T::PUSEL: PUSEL10 Mask            */

#define GPIO_PUSEL_PUSEL11_Pos           (22)                                              /*!< GPIO_T::PUSEL: PUSEL11 Position        */
#define GPIO_PUSEL_PUSEL11_Msk           (0x3ul << GPIO_PUSEL_PUSEL11_Pos)                 /*!< GPIO_T::PUSEL: PUSEL11 Mask            */

#define GPIO_PUSEL_PUSEL12_Pos           (24)                                              /*!< GPIO_T::PUSEL: PUSEL12 Position        */
#define GPIO_PUSEL_PUSEL12_Msk           (0x3ul << GPIO_PUSEL_PUSEL12_Pos)                 /*!< GPIO_T::PUSEL: PUSEL12 Mask            */

#define GPIO_PUSEL_PUSEL13_Pos           (26)                                              /*!< GPIO_T::PUSEL: PUSEL13 Position        */
#define GPIO_PUSEL_PUSEL13_Msk           (0x3ul << GPIO_PUSEL_PUSEL13_Pos)                 /*!< GPIO_T::PUSEL: PUSEL13 Mask            */

#define GPIO_PUSEL_PUSEL14_Pos           (28)                                              /*!< GPIO_T::PUSEL: PUSEL14 Position        */
#define GPIO_PUSEL_PUSEL14_Msk           (0x3ul << GPIO_PUSEL_PUSEL14_Pos)                 /*!< GPIO_T::PUSEL: PUSEL14 Mask            */

#define GPIO_PUSEL_PUSEL15_Pos           (30)                                              /*!< GPIO_T::PUSEL: PUSEL15 Position        */
#define GPIO_PUSEL_PUSEL15_Msk           (0x3ul << GPIO_PUSEL_PUSEL15_Pos)                 /*!< GPIO_T::PUSEL: PUSEL15 Mask            */

#define GPIO_DBCTL_DBCLKSEL_Pos          (0)                                               /*!< GPIO_T::DBCTL: DBCLKSEL Position          */
#define GPIO_DBCTL_DBCLKSEL_Msk          (0xFul << GPIO_DBCTL_DBCLKSEL_Pos)                /*!< GPIO_T::DBCTL: DBCLKSEL Mask              */

#define GPIO_DBCTL_DBCLKSRC_Pos          (4)                                               /*!< GPIO_T::DBCTL: DBCLKSRC Position          */
#define GPIO_DBCTL_DBCLKSRC_Msk          (1ul << GPIO_DBCTL_DBCLKSRC_Pos)                  /*!< GPIO_T::DBCTL: DBCLKSRC Mask              */

#define GPIO_DBCTL_ICLKON_Pos            (5)                                               /*!< GPIO_T::DBCTL: ICLKON Position            */
#define GPIO_DBCTL_ICLKON_Msk            (1ul << GPIO_DBCTL_ICLKON_Pos)                    /*!< GPIO_T::DBCTL: ICLKON Mask                */

/**@}*/ /* GPIO_CONST */
/**@}*/ /* end of GPIO register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __GPIO_REG_H__ */
