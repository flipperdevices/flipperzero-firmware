/**************************************************************************//**
 * @file     clk_reg.h
 * @version  V1.00
 * @brief    CLK register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CLK_REG_H__
#define __CLK_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup CLK System Clock Controller(CLK)
    Memory Mapped Structure for CLK Controller
@{ */

typedef struct
{


    /**
     * @var CLK_T::PWRCTL
     * Offset: 0x00  System Power-down Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTEN     |HXT Enable Bit (Write Protect)
     * |        |          |The bit default value is set by flash controller user configuration register CONFIG0 [26]
     * |        |          |When the default clock source is from HXT, this bit is set to 1 automatically.
     * |        |          |0 = 4~24 MHz external high speed crystal (HXT) Disabled.
     * |        |          |1 = 4~24 MHz external high speed crystal (HXT) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |LXTEN     |LXT Enable Bit (Write Protect)
     * |        |          |0 = 32.768 kHz external low speed crystal (LXT) Disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal (LXT) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |HIRCEN    |HIRC Enable Bit (Write Protect)
     * |        |          |0 = 12 MHz internal high speed RC oscillator (HIRC) Disabled.
     * |        |          |1 = 12 MHz internal high speed RC oscillator (HIRC) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |LIRCEN    |LIRC Enable Bit (Write Protect)
     * |        |          |0 = 10 kHz internal low speed RC oscillator (LIRC) Disabled.
     * |        |          |1 = 10 kHz internal low speed RC oscillator (LIRC) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[4]     |PDWKDLY   |Enable the Wake-up Delay Counter (Write Protect)
     * |        |          |When the chip wakes up from Power-down mode, the clock control will delay certain clock cycles to wait system clock stable.
     * |        |          |The delayed clock cycle is 4096 clock cycles when chip works at 4~24 MHz external high speed crystal oscillator (HXT), and 256 clock cycles when chip works at 12 MHz internal high speed RC oscillator (HIRC).
     * |        |          |0 = Clock cycles delay Disabled.
     * |        |          |1 = Clock cycles delay Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[5]     |PDWKIEN   |Power-down Mode Wake-up Interrupt Enable Bit (Write Protect)
     * |        |          |0 = Power-down mode wake-up interrupt Disabled.
     * |        |          |1 = Power-down mode wake-up interrupt Enabled.
     * |        |          |Note1: The interrupt will occur when both PDWKIF and PDWKIEN are high.
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |PDWKIF    |Power-down Mode Wake-up Interrupt Status
     * |        |          |Set by "Power-down wake-up event", it indicates that resume from Power-down mode.
     * |        |          |The flag is set if any wake-up source is occurred. Refer Power Modes and Wake-up Sources chapter.
     * |        |          |Note1: Write 1 to clear the bit to 0.
     * |        |          |Note2: This bit works only if PDWKIEN (CLK_PWRCTL[5]) set to 1.
     * |[7]     |PDEN      |System Power-down Enable (Write Protect)
     * |        |          |When this bit is set to 1, Power-down mode is enabled and chip keeps active till the CPU sleep mode is also active and then the chip enters Power-down mode.
     * |        |          |When chip wakes up from Power-down mode, this bit is auto cleared
     * |        |          |Users need to set this bit again for next Power-down.
     * |        |          |In Power-down mode, HXT and the HIRC will be disabled in this mode, but LXT and LIRC are not controlled by Power-down mode.
     * |        |          |In Power-down mode, the PLL and system clock are disabled, and ignored the clock source selection
     * |        |          |The clocks of peripheral are not controlled by Power-down mode, if the peripheral clock source is from LXT or LIRC.
     * |        |          |0 = Chip will not enter Power-down mode after CPU sleep command WFI.
     * |        |          |1 = Chip enters Power-down mode after CPU sleep command WFI.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[11:10] |HXTGAIN   |HXT Gain Control Bit (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |Gain control is used to enlarge the gain of crystal to make sure crystal work normally
     * |        |          |If gain control is enabled, crystal will consume more power than gain control off.
     * |        |          |00 = HXT frequency is lower than from 8 MHz.
     * |        |          |01 = HXT frequency is from 8 MHz to 12 MHz.
     * |        |          |10 = HXT frequency is from 12 MHz to 16 MHz.
     * |        |          |11 = HXT frequency is higher than 16 MHz.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[12]    |HXTSELTYP |HXT Crystal Type Select Bit (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = Select INV type.
     * |        |          |1 = Select GM type.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[13]    |HXTTBEN   |HXT Crystal TURBO Mode (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = HXT Crystal TURBO mode disabled.
     * |        |          |1 = HXT Crystal TURBO mode enabled.
     * |[17:16] |HIRCSTBS  |HIRC Stable Count Select (Write Protect)
     * |        |          |00 = HIRC stable count is 64 clocks.
     * |        |          |01 = HIRC stable count is 24 clocks.
     * |        |          |others = Reserved.
     * |[18]    |HIRCEN    |HIRC48M Enable Bit (Write Protect)
     * |        |          |0 = 48 MHz internal high speed RC oscillator (HIRC) Disabled.
     * |        |          |1 = 48 MHz internal high speed RC oscillator (HIRC) Enabled.
     * @var CLK_T::AHBCLK
     * Offset: 0x04  AHB Devices Clock Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |PDMACKEN  |PDMA Controller Clock Enable Bit
     * |        |          |0 = PDMA peripheral clock Disabled.
     * |        |          |1 = PDMA peripheral clock Enabled.
     * |[2]     |ISPCKEN   |Flash ISP Controller Clock Enable Bit
     * |        |          |0 = Flash ISP peripheral clock Disabled.
     * |        |          |1 = Flash ISP peripheral clock Enabled.
     * |[3]     |EBICKEN   |EBI Controller Clock Enable Bit
     * |        |          |0 = EBI peripheral clock Disabled.
     * |        |          |1 = EBI peripheral clock Enabled.
     * |[5]     |EMACCKEN  |Ethernet Controller Clock Enable Bit
     * |        |          |0 = Ethernet Controller engine clock Disabled.
     * |        |          |1 = Ethernet Controller engine clock Enabled.
     * |[6]     |SDH0CKEN  |SD0 Controller Clock Enable Bit
     * |        |          |0 = SD0 engine clock Disabled.
     * |        |          |1 = SD0 engine clock Enabled.
     * |[7]     |CRCCKEN   |CRC Generator Controller Clock Enable Bit
     * |        |          |0 = CRC peripheral clock Disabled.
     * |        |          |1 = CRC peripheral clock Enabled.
     * |[10]    |HSUSBDCKEN|HSUSB Device Clock Enable Bit
     * |        |          |0 = HSUSB device controller's clock Disabled.
     * |        |          |1 = HSUSB device controller's clock Enabled.
     * |[12]    |CRPTCKEN  |Cryptographic Accelerator Clock Enable Bit
     * |        |          |0 = Cryptographic Accelerator clock Disabled.
     * |        |          |1 = Cryptographic Accelerator clock Enabled.
     * |[14]    |SPIMCKEN  |SPIM Controller Clock Enable Bit
     * |        |          |0 = SPIM controller clock Disabled.
     * |        |          |1 = SPIM controller clock Enabled.
     * |[15]    |FMCIDLE   |Flash Memory Controller Clock Enable Bit in IDLE Mode
     * |        |          |0 = FMC clock Disabled when chip is under IDLE mode.
     * |        |          |1 = FMC clock Enabled when chip is under IDLE mode.
     * |[16]    |USBHCKEN  |USB HOST Controller Clock Enable Bit
     * |        |          |0 = USB HOST peripheral clock Disabled.
     * |        |          |1 = USB HOST peripheral clock Enabled.
     * |[17]    |SDH1CKEN  |SD1 Controller Clock Enable Bit
     * |        |          |0 = SD1 engine clock Disabled.
     * |        |          |1 = SD1 engine clock Enabled.
     * @var CLK_T::APBCLK0
     * Offset: 0x08  APB Devices Clock Enable Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WDTCKEN   |Watchdog Timer Clock Enable Bit (Write Protect)
     * |        |          |0 = Watchdog timer clock Disabled.
     * |        |          |1 = Watchdog timer clock Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |RTCCKEN   |Real-time-clock APB Interface Clock Enable Bit
     * |        |          |This bit is used to control the RTC APB clock only
     * |        |          |The RTC peripheral clock source is selected from RTCSEL(CLK_CLKSEL3[8])
     * |        |          |It can be selected to 32.768 kHz external low speed crystal or 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |0 = RTC clock Disabled.
     * |        |          |1 = RTC clock Enabled.
     * |[2]     |TMR0CKEN  |Timer0 Clock Enable Bit
     * |        |          |0 = Timer0 clock Disabled.
     * |        |          |1 = Timer0 clock Enabled.
     * |[3]     |TMR1CKEN  |Timer1 Clock Enable Bit
     * |        |          |0 = Timer1 clock Disabled.
     * |        |          |1 = Timer1 clock Enabled.
     * |[4]     |TMR2CKEN  |Timer2 Clock Enable Bit
     * |        |          |0 = Timer2 clock Disabled.
     * |        |          |1 = Timer2 clock Enabled.
     * |[5]     |TMR3CKEN  |Timer3 Clock Enable Bit
     * |        |          |0 = Timer3 clock Disabled.
     * |        |          |1 = Timer3 clock Enabled.
     * |[6]     |CLKOCKEN  |CLKO Clock Enable Bit
     * |        |          |0 = CLKO clock Disabled.
     * |        |          |1 = CLKO clock Enabled.
     * |[7]     |ACMP01CKEN|Analog Comparator 0/1 Clock Enable Bit
     * |        |          |0 = Analog comparator 0/1 clock Disabled.
     * |        |          |1 = Analog comparator 0/1 clock Enabled.
     * |[8]     |I2C0CKEN  |I2C0 Clock Enable Bit
     * |        |          |0 = I2C0 clock Disabled.
     * |        |          |1 = I2C0 clock Enabled.
     * |[9]     |I2C1CKEN  |I2C1 Clock Enable Bit
     * |        |          |0 = I2C1 clock Disabled.
     * |        |          |1 = I2C1 clock Enabled.
     * |[10]    |I2C2CKEN  |I2C2 Clock Enable Bit
     * |        |          |0 = I2C2 clock Disabled.
     * |        |          |1 = I2C2 clock Enabled.
     * |[12]    |QSPI0CKEN  |QSPI0 Clock Enable Bit
     * |        |          |0 = QSPI0 clock Disabled.
     * |        |          |1 = QSPI0 clock Enabled.
     * |[13]    |SPI0CKEN  |SPI0 Clock Enable Bit
     * |        |          |0 = SPI0 clock Disabled.
     * |        |          |1 = SPI0 clock Enabled.
     * |[14]    |SPI1CKEN  |SPI1 Clock Enable Bit
     * |        |          |0 = SPI1 clock Disabled.
     * |        |          |1 = SPI1 clock Enabled.
     * |[15]    |SPI2CKEN  |SPI2 Clock Enable Bit
     * |        |          |0 = SPI2 clock Disabled.
     * |        |          |1 = SPI2 clock Enabled.
     * |[16]    |UART0CKEN |UART0 Clock Enable Bit
     * |        |          |0 = UART0 clock Disabled.
     * |        |          |1 = UART0 clock Enabled.
     * |[17]    |UART1CKEN |UART1 Clock Enable Bit
     * |        |          |0 = UART1 clock Disabled.
     * |        |          |1 = UART1 clock Enabled.
     * |[18]    |UART2CKEN |UART2 Clock Enable Bit
     * |        |          |0 = UART2 clock Disabled.
     * |        |          |1 = UART2 clock Enabled.
     * |[19]    |UART3CKEN |UART3 Clock Enable Bit
     * |        |          |0 = UART3 clock Disabled.
     * |        |          |1 = UART3 clock Enabled.
     * |[20]    |UART4CKEN |UART4 Clock Enable Bit
     * |        |          |0 = UART4 clock Disabled.
     * |        |          |1 = UART4 clock Enabled.
     * |[21]    |UART5CKEN |UART5 Clock Enable Bit
     * |        |          |0 = UART5 clock Disabled.
     * |        |          |1 = UART5 clock Enabled.
     * |[24]    |CAN0CKEN  |CAN0 Clock Enable Bit
     * |        |          |0 = CAN0 clock Disabled.
     * |        |          |1 = CAN0 clock Enabled.
     * |[25]    |CAN1CKEN  |CAN1 Clock Enable Bit
     * |        |          |0 = CAN1 clock Disabled.
     * |        |          |1 = CAN1 clock Enabled.
     * |[26]    |OTGCKEN   |USB OTG Clock Enable Bit
     * |        |          |0 = USB OTG clock Disabled.
     * |        |          |1 = USB OTG clock Enabled.
     * |[27]    |USBDCKEN  |USB Device Clock Enable Bit
     * |        |          |0 = USB Device clock Disabled.
     * |        |          |1 = USB Device clock Enabled.
     * |[28]    |EADCCKEN  |Enhanced Analog-digital-converter (EADC) Clock Enable Bit
     * |        |          |0 = EADC clock Disabled.
     * |        |          |1 = EADC clock Enabled.
     * |[29]    |I2S0CKEN  |I2S0 Clock Enable Bit
     * |        |          |0 = I2S0 Clock Disabled.
     * |        |          |1 = I2S0 Clock Enabled.
     * |[30]    |HSOTGCKEN |HSUSB OTG Clock Enable Bit
     * |        |          |0 = HSUSB OTG clock Disabled.
     * |        |          |1 = HSUSB OTG clock Enabled.
     * @var CLK_T::APBCLK1
     * Offset: 0x0C  APB Devices Clock Enable Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SC0CKEN   |SC0 Clock Enable Bit
     * |        |          |0 = SC0 clock Disabled.
     * |        |          |1 = SC0 clock Enabled.
     * |[1]     |SC1CKEN   |SC1 Clock Enable Bit
     * |        |          |0 = SC1 clock Disabled.
     * |        |          |1 = SC1 clock Enabled.
     * |[2]     |SC2CKEN   |SC2 Clock Enable Bit
     * |        |          |0 = SC2 clock Disabled.
     * |        |          |1 = SC2 clock Enabled.
     * |[6]     |SPI3CKEN  |SPI3 Clock Enable Bit
     * |        |          |0 = SPI3 clock Disabled.
     * |        |          |1 = SPI3 clock Enabled.
     * |[8]     |USCI0CKEN |USCI0 Clock Enable Bit
     * |        |          |0 = USCI0 clock Disabled.
     * |        |          |1 = USCI0 clock Enabled.
     * |[9]     |USCI1CKEN |USCI1 Clock Enable Bit
     * |        |          |0 = USCI1 clock Disabled.
     * |        |          |1 = USCI1 clock Enabled.
     * |[12]    |DACCKEN   |DAC Clock Enable Bit
     * |        |          |0 = DAC clock Disabled.
     * |        |          |1 = DAC clock Enabled.
     * |[16]    |EPWM0CKEN |EPWM0 Clock Enable Bit
     * |        |          |0 = EPWM0 clock Disabled.
     * |        |          |1 = EPWM0 clock Enabled.
     * |[17]    |EPWM1CKEN |EPWM1 Clock Enable Bit
     * |        |          |0 = EPWM1 clock Disabled.
     * |        |          |1 = EPWM1 clock Enabled.
     * |[18]    |BPWM0CKEN |BPWM0 Clock Enable Bit
     * |        |          |0 = BPWM0 clock Disabled.
     * |        |          |1 = BPWM0 clock Enabled.
     * |[19]    |BPWM1CKEN |BPWM1 Clock Enable Bit
     * |        |          |0 = BPWM1 clock Disabled.
     * |        |          |1 = BPWM1 clock Enabled.
     * |[22]    |QEI0CKEN  |QEI0 Clock Enable Bit
     * |        |          |0 = QEI0 clock Disabled.
     * |        |          |1 = QEI0 clock Enabled.
     * |[23]    |QEI1CKEN  |QEI1 Clock Enable Bit
     * |        |          |0 = QEI1 clock Disabled.
     * |        |          |1 = QEI1 clock Enabled.
     * |[26]    |ECAP0CKEN |ECAP0 Clock Enable Bit
     * |        |          |0 = ECAP0 clock Disabled.
     * |        |          |1 = ECAP0 clock Enabled.
     * |[27]    |ECAP1CKEN |ECAP1 Clock Enable Bit
     * |        |          |0 = ECAP1 clock Disabled.
     * |        |          |1 = ECAP1 clock Enabled.
     * |[30]    |OPACKEN   |OP Amplifier (OPA) Clock Enable Bit
     * |        |          |0 = OPA clock Disabled.
     * |        |          |1 = OPA clock Enabled.
     * @var CLK_T::CLKSEL0
     * Offset: 0x10  Clock Source Select Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |HCLKSEL   |HCLK Clock Source Selection (Write Protect)
     * |        |          |Before clock switching, the related clock sources (both pre-select and new-select) must be turned on.
     * |        |          |The default value is reloaded from the value of CFOSC (CONFIG0[26]) in user configuration register of Flash controller by any reset
     * |        |          |Therefore the default value is either 000b or 111b.
     * |        |          |000 = Clock source from HXT.
     * |        |          |001 = Clock source from LXT.
     * |        |          |010 = Clock source from PLL.
     * |        |          |011 = Clock source from LIRC.
     * |        |          |111 = Clock source from HIRC.
     * |        |          |Other = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[5:3]   |STCLKSEL  |Cortex-M4 SysTick Clock Source Selection (Write Protect)
     * |        |          |If SYST_CTRL[2]=0, SysTick uses listed clock source below.
     * |        |          |000 = Clock source from HXT.
     * |        |          |001 = Clock source from LXT.
     * |        |          |010 = Clock source from HXT/2.
     * |        |          |011 = Clock source from HCLK/2.
     * |        |          |111 = Clock source from HIRC/2.
     * |        |          |Note: if SysTick clock source is not from HCLK (i.e
     * |        |          |SYST_CTRL[2] = 0), SysTick clock source must less than or equal to HCLK/2.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |USBSEL    |USB Clock Source Selection (Write Protect)
     * |        |          |0 = Clock source from RC48M.
     * |        |          |1 = Clock source from PLL.
     * |[21:20] |SDH0SEL   |SD0 Engine Clock Source Selection (Write Protect)
     * |        |          |00 = Clock source from HXT clock.
     * |        |          |01 = Clock source from PLL clock.
     * |        |          |10 = Clock source from HCLK.
     * |        |          |11 = Clock source from HIRC clock.
     * |[23:22] |SDH1SEL   |SD1 Engine Clock Source Selection (Write Protect)
     * |        |          |00 = Clock source from HXT clock.
     * |        |          |01 = Clock source from PLL clock.
     * |        |          |10 = Clock source from HCLK.
     * |        |          |11 = Clock source from HIRC clock.
     * @var CLK_T::CLKSEL1
     * Offset: 0x14  Clock Source Select Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |WDTSEL    |Watchdog Timer Clock Source Selection (Write Protect)
     * |        |          |00 = Reserved.
     * |        |          |01 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |10 = Clock source from HCLK/2048.
     * |        |          |11 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[10:8]  |TMR0SEL   |TIMER0 Clock Source Selection
     * |        |          |000 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK0.
     * |        |          |011 = Clock source from external clock TM0 pin.
     * |        |          |101 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[14:12] |TMR1SEL   |TIMER1 Clock Source Selection
     * |        |          |000 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK0.
     * |        |          |011 = Clock source from external clock TM1 pin.
     * |        |          |101 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[18:16] |TMR2SEL   |TIMER2 Clock Source Selection
     * |        |          |000 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK1.
     * |        |          |011 = Clock source from external clock TM2 pin.
     * |        |          |101 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[22:20] |TMR3SEL   |TIMER3 Clock Source Selection
     * |        |          |000 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK1.
     * |        |          |011 = Clock source from external clock TM3 pin.
     * |        |          |101 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[25:24] |UART0SEL  |UART0 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[27:26] |UART1SEL  |UART1 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[29:28] |CLKOSEL   |Clock Divider Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |10 = Clock source from HCLK.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[31:30] |WWDTSEL   |Window Watchdog Timer Clock Source Selection
     * |        |          |10 = Clock source from HCLK/2048.
     * |        |          |11 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |        |          |Others = Reserved.
     * @var CLK_T::CLKSEL2
     * Offset: 0x18  Clock Source Select Control Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |EPWM0SEL  |EPWM0 Clock Source Selection
     * |        |          |The peripheral clock source of EPWM0 is defined by EPWM0SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK0.
     * |[1]     |EPWM1SEL  |EPWM1 Clock Source Selection
     * |        |          |The peripheral clock source of EPWM1 is defined by EPWM1SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK1.
     * |[3:2]   |QSPI0SEL   |QSPI0 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[5:4]   |SPI0SEL   |SPI0 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK1.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[7:6]   |SPI1SEL   |SPI1 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[8]     |BPWM0SEL  |BPWM0 Clock Source Selection
     * |        |          |The peripheral clock source of BPWM0 is defined by BPWM0SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK0.
     * |[9]     |BPWM1SEL  |BPWM1 Clock Source Selection
     * |        |          |The peripheral clock source of BPWM1 is defined by BPWM1SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK1.
     * |[11:10] |SPI2SEL   |SPI2 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK1.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[13:12] |SPI3SEL   |SPI3 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * @var CLK_T::CLKSEL3
     * Offset: 0x1C  Clock Source Select Control Register 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |SC0SEL    |SC0 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[3:2]   |SC1SEL    |SC0 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK1.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[5:4]   |SC2SEL    |SC2 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[8]     |RTCSEL    |RTC Clock Source Selection
     * |        |          |0 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |1 = Clock source from 10 kHz internal low speed RC oscillator (LIRC).
     * |[17:16] |I2S0SEL   |I2S0 Clock Source Selection
     * |        |          |00 = Clock source from HXT clock.
     * |        |          |01 = Clock source from PLL clock.
     * |        |          |10 = Clock source from PCLK.
     * |        |          |11 = Clock source from HIRC clock.
     * |[25:24] |UART2SEL  |UART2 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[27:26] |UART3SEL  |UART3 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[29:28] |UART4SEL  |UART4 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * |[31:30] |UART5SEL  |UART5 Clock Source Selection
     * |        |          |00 = Clock source from 4~24 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |11 = Clock source from 12 MHz internal high speed RC oscillator (HIRC).
     * @var CLK_T::CLKDIV0
     * Offset: 0x20  Clock Divider Number Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |HCLKDIV   |HCLK Clock Divide Number From HCLK Clock Source
     * |        |          |HCLK clock frequency = (HCLK clock source frequency) / (HCLKDIV + 1).
     * |[7:4]   |USBDIV    |USB Clock Divide Number From PLL Clock
     * |        |          |USB clock frequency = (PLL frequency) / (USBDIV + 1).
     * |[11:8]  |UART0DIV  |UART0 Clock Divide Number From UART0 Clock Source
     * |        |          |UART0 clock frequency = (UART0 clock source frequency) / (UART0DIV + 1).
     * |[15:12] |UART1DIV  |UART1 Clock Divide Number From UART1 Clock Source
     * |        |          |UART1 clock frequency = (UART1 clock source frequency) / (UART1DIV + 1).
     * |[23:16] |EADCDIV   |EADC Clock Divide Number From EADC Clock Source
     * |        |          |EADC clock frequency = (EADC clock source frequency) / (EADCDIV + 1).
     * |[31:24] |SDH0DIV   |SD0 Clock Divide Number From SD0 Clock Source
     * |        |          |SD0 clock frequency = (SD0 clock source frequency) / (SDH0DIV + 1).
     * @var CLK_T::CLKDIV1
     * Offset: 0x24  Clock Divider Number Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SC0DIV    |SC0 Clock Divide Number From SC0 Clock Source
     * |        |          |SC0 clock frequency = (SC0 clock source frequency ) / (SC0DIV + 1).
     * |[15:8]  |SC1DIV    |SC1 Clock Divide Number From SC1 Clock Source
     * |        |          |SC1 clock frequency = (SC1 clock source frequency ) / (SC1DIV + 1).
     * |[23:16] |SC2DIV    |SC2 Clock Divide Number From SC2 Clock Source
     * |        |          |SC2 clock frequency = (SC2 clock source frequency ) / (SC2DIV + 1).
     * @var CLK_T::CLKDIV3
     * Offset: 0x2C  Clock Divider Number Register 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:16] |EMACDIV   |Ethernet Clock Divide Number Form HCLK
     * |        |          |EMAC MDCLK clock frequency = (HCLK) / (EMACDIV + 1).
     * |[31:24] |SDH1DIV   |SD1 Clock Divide Number From SD1 Clock Source
     * |        |          |SD1 clock frequency = (SD1 clock source frequency) / (SDH1DIV + 1).
     * @var CLK_T::CLKDIV4
     * Offset: 0x30  Clock Divider Number Register 4
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |UART2DIV  |UART2 Clock Divide Number From UART2 Clock Source
     * |        |          |UART2 clock frequency = (UART2 clock source frequency) / (UART2DIV + 1).
     * |[7:4]   |UART3DIV  |UART3 Clock Divide Number From UART3 Clock Source
     * |        |          |UART3 clock frequency = (UART3 clock source frequency) / (UART3DIV + 1).
     * |[11:8]  |UART4DIV  |UART4 Clock Divide Number From UART4 Clock Source
     * |        |          |UART4 clock frequency = (UART4 clock source frequency) / (UART4DIV + 1).
     * |[15:12] |UART5DIV  |UART5 Clock Divide Number From UART5 Clock Source
     * |        |          |UART5 clock frequency = (UART5 clock source frequency) / (UART5DIV + 1).
     * @var CLK_T::PCLKDIV
     * Offset: 0x34  APB Clock Divider Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |APB0DIV   |APB0 Clock Divider
     * |        |          |APB0 clock can be divided from HCLK
     * |        |          |000: PCLK0 = HCLK.
     * |        |          |001: PCLK0 = 1/2 HCLK.
     * |        |          |010: PCLK0 = 1/4 HCLK.
     * |        |          |011: PCLK0 = 1/8 HCLK.
     * |        |          |100: PCLK0 = 1/16 HCLK.
     * |        |          |Others: Reserved.
     * |[6:4]   |APB1DIV   |APB1 Clock Divider
     * |        |          |APB1 clock can be divided from HCLK
     * |        |          |000: PCLK1 = HCLK.
     * |        |          |001: PCLK1 = 1/2 HCLK.
     * |        |          |010: PCLK1 = 1/4 HCLK.
     * |        |          |011: PCLK1 = 1/8 HCLK.
     * |        |          |100: PCLK1 = 1/16 HCLK.
     * |        |          |Others: Reserved.
     * @var CLK_T::PLLCTL
     * Offset: 0x40  PLL Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |FBDIV     |PLL Feedback Divider Control (Write Protect)
     * |        |          |Refer to the formulas below the table.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[13:9]  |INDIV     |PLL Input Divider Control (Write Protect)
     * |        |          |Refer to the formulas below the table.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[15:14] |OUTDIV    |PLL Output Divider Control (Write Protect)
     * |        |          |Refer to the formulas below the table.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[16]    |PD        |Power-down Mode (Write Protect)
     * |        |          |If set the PDEN bit to 1 in CLK_PWRCTL register, the PLL will enter Power-down mode, too.
     * |        |          |0 = PLL is in normal mode.
     * |        |          |1 = PLL is in Power-down mode (default).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[17]    |BP        |PLL Bypass Control (Write Protect)
     * |        |          |0 = PLL is in normal mode (default).
     * |        |          |1 = PLL clock output is same as PLL input clock FIN.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[18]    |OE        |PLL OE (FOUT Enable) Pin Control (Write Protect)
     * |        |          |0 = PLL FOUT Enabled.
     * |        |          |1 = PLL FOUT is fixed low.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[19]    |PLLSRC    |PLL Source Clock Selection (Write Protect)
     * |        |          |0 = PLL source clock from 4~24 MHz external high-speed crystal oscillator (HXT).
     * |        |          |1 = PLL source clock from 12 MHz internal high-speed oscillator (HIRC).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[23]    |STBSEL    |PLL Stable Counter Selection (Write Protect)
     * |        |          |0 = PLL stable time is 6144 PLL source clock (suitable for source clock is equal to or less than 12 MHz).
     * |        |          |1 = PLL stable time is 12288 PLL source clock (suitable for source clock is larger than 12 MHz).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var CLK_T::STATUS
     * Offset: 0x50  Clock Status Monitor Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTSTB    |HXT Clock Source Stable Flag (Read Only)
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock is not stable or disabled.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock is stable and enabled.
     * |[1]     |LXTSTB    |LXT Clock Source Stable Flag (Read Only)
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock is not stable or disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) clock is stabled and enabled.
     * |[2]     |PLLSTB    |Internal PLL Clock Source Stable Flag (Read Only)
     * |        |          |0 = Internal PLL clock is not stable or disabled.
     * |        |          |1 = Internal PLL clock is stable and enabled.
     * |[3]     |LIRCSTB   |LIRC Clock Source Stable Flag (Read Only)
     * |        |          |0 = 10 kHz internal low speed RC oscillator (LIRC) clock is not stable or disabled.
     * |        |          |1 = 10 kHz internal low speed RC oscillator (LIRC) clock is stable and enabled.
     * |[4]     |HIRCSTB   |HIRC Clock Source Stable Flag (Read Only)
     * |        |          |0 = 12 MHz internal high speed RC oscillator (HIRC) clock is not stable or disabled.
     * |        |          |1 = 12 MHz internal high speed RC oscillator (HIRC) clock is stable and enabled.
     * |        |          |Note: This bit is read only.
     * |[6]     |HIRC48MSTB|HIRC 48MHz Clock Source Stable Flag (Read Only)
     * |        |          |0 = 48 MHz internal high speed RC oscillator (HIRC) clock is not stable or disabled.
     * |        |          |1 = 48 MHz internal high speed RC oscillator (HIRC) clock is stable and enabled.
     * |        |          |Note: This bit is read only.
     * |[7]     |CLKSFAIL  |Clock Switching Fail Flag (Read Only)
     * |        |          |This bit is updated when software switches system clock source
     * |        |          |If switch target clock is stable, this bit will be set to 0
     * |        |          |If switch target clock is not stable, this bit will be set to 1.
     * |        |          |0 = Clock switching success.
     * |        |          |1 = Clock switching failure.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * @var CLK_T::CLKOCTL
     * Offset: 0x60  Clock Output Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |FREQSEL   |Clock Output Frequency Selection
     * |        |          |The formula of output frequency is
     * |        |          |Fout = Fin/2(N+1).
     * |        |          |Fin is the input clock frequency.
     * |        |          |Fout is the frequency of divider output clock.
     * |        |          |N is the 4-bit value of FREQSEL[3:0].
     * |[4]     |CLKOEN    |Clock Output Enable Bit
     * |        |          |0 = Clock Output function Disabled.
     * |        |          |1 = Clock Output function Enabled.
     * |[5]     |DIV1EN    |Clock Output Divide One Enable Bit
     * |        |          |0 = Clock Output will output clock with source frequency divided by FREQSEL.
     * |        |          |1 = Clock Output will output clock with source frequency.
     * |[6]     |CLK1HZEN  |Clock Output 1Hz Enable Bit
     * |        |          |0 = 1 Hz clock output for 32.768 kHz frequency compensation Disabled.
     * |        |          |1 = 1 Hz clock output for 32.768 kHz frequency compensation Enabled.
     * @var CLK_T::CLKDCTL
     * Offset: 0x70  Clock Fail Detector Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4]     |HXTFDEN   |HXT Clock Fail Detector Enable Bit
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock fail detector Disabled.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock fail detector Enabled.
     * |[5]     |HXTFIEN   |HXT Clock Fail Interrupt Enable Bit
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock fail interrupt Disabled.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock fail interrupt Enabled.
     * |[12]    |LXTFDEN   |LXT Clock Fail Detector Enable Bit
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail detector Disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail detector Enabled.
     * |[13]    |LXTFIEN   |LXT Clock Fail Interrupt Enable Bit
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail interrupt Disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail interrupt Enabled.
     * |[16]    |HXTFQDEN  |HXT Clock Frequency Range Detector Enable Bit
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock frequency range detector Disabled.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock frequency range detector Enabled.
     * |[17]    |HXTFQIEN  |HXT Clock Frequency Range Detector Interrupt Enable Bit
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock frequency range detector fail interrupt Disabled.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock frequency range detector fail interrupt Enabled.
     * @var CLK_T::CLKDSTS
     * Offset: 0x74  Clock Fail Detector Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTFIF    |HXT Clock Fail Interrupt Flag
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock is normal.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock stops.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * |[1]     |LXTFIF    |LXT Clock Fail Interrupt Flag
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock is normal.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) stops.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * |[8]     |HXTFQIF   |HXT Clock Frequency Range Detector Interrupt Flag
     * |        |          |0 = 4~24 MHz external high speed crystal oscillator (HXT) clock frequency is normal.
     * |        |          |1 = 4~24 MHz external high speed crystal oscillator (HXT) clock frequency is abnormal.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * @var CLK_T::CDUPB
     * Offset: 0x78  Clock Frequency Range Detector Upper Boundary Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |UPERBD    |HXT Clock Frequency Range Detector Upper Boundary Value
     * |        |          |The bits define the maximum value of frequency range detector window.
     * |        |          |When HXT frequency higher than this maximum frequency value, the HXT Clock Frequency Range Detector Interrupt Flag will set to 1.
     * @var CLK_T::CDLOWB
     * Offset: 0x7C  Clock Frequency Range Detector Lower Boundary Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |LOWERBD   |HXT Clock Frequency Range Detector Lower Boundary Value
     * |        |          |The bits define the minimum value of frequency range detector window.
     * |        |          |When HXT frequency lower than this minimum frequency value, the HXT Clock Frequency Range Detector Interrupt Flag will set to 1.
     * @var CLK_T::PMUCTL
     * Offset: 0x90  Power Manager Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |PDMSEL    |Power-down Mode Selection (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |These bits control chip power-down mode grade selection when CPU execute WFI/WFE instruction.
     * |        |          |000 = Power-down mode is selected. (PD)
     * |        |          |001 = Low leakage Power-down mode is selected (LLPD).
     * |        |          |010 =Fast wake-up Power-down mode is selected (FWPD).
     * |        |          |011 = Reserved.
     * |        |          |100 = Standby Power-down mode 0 is selected (SPD0) (SRAM retention).
     * |        |          |101 = Standby Power-down mode 1 is selected (SPD1).
     * |        |          |110 = Deep Power-down mode is selected (DPD).
     * |        |          |111 = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |DPDHOLDEN |Deep-Power-Down Mode GPIO Hold Enable
     * |        |          |0 = When GPIO enters deep power-down mode, all I/O status are tri-state.
     * |        |          |1 = When GPIO enters deep power-down mode, all I/O status are hold to keep normal operating status.
     * |        |          |    After chip was waked up from deep power-down mode, the I/O are still keep hold status until user set CLK_IOPDCTL[0]
     * |        |          |    to release I/O hold status.
     * |[8]     |WKTMREN   |Wake-up Timer Enable (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = Wake-up timer disable at DPD/SPD mode.
     * |        |          |1 = Wake-up timer enabled at DPD/SPD mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[11:9]  |WKTMRIS   |Wake-up Timer Time-out Interval Select (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |These bits control wake-up timer time-out interval when chip at DPD/SPD mode.
     * |        |          |000 = Time-out interval is 128 OSC10K clocks (12.8 ms).
     * |        |          |001 = Time-out interval is 256 OSC10K clocks (25.6 ms).
     * |        |          |010 = Time-out interval is 512 OSC10K clocks (51.2 ms).
     * |        |          |011 = Time-out interval is 1024 OSC10K clocks (102.4ms).
     * |        |          |100 = Time-out interval is 4096 OSC10K clocks (409.6ms).
     * |        |          |101 = Time-out interval is 8192 OSC10K clocks (819.2ms).
     * |        |          |110 = Time-out interval is 16384 OSC10K clocks (1638.4ms).
     * |        |          |111 = Time-out interval is 65536 OSC10K clocks (6553.6ms).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[17:16] |WKPINEN   |Wake-up Pin Enable (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |00 = Wake-up pin disable at Deep Power-down mode.
     * |        |          |01 = Wake-up pin rising edge enabled at Deep Power-down mode.
     * |        |          |10 = Wake-up pin falling edge enabled at Deep Power-down mode.
     * |        |          |11 = Wake-up pin both edge enabled at Deep Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[18]    |ACMPSPWK  |ACMP Standby Power-down Mode Wake-up Enable (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = ACMP wake-up disable at Standby Power-down mode.
     * |        |          |1 = ACMP wake-up enabled at Standby Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[23]    |RTCWKEN   |RTC Wake-up Enable (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = RTC wake-up disable at Deep Power-down mode or Standby Power-down mode.
     * |        |          |1 = RTC wake-up enabled at Deep Power-down mode or Standby Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var CLK_T::PMUSTS
     * Offset: 0x94  Power Manager Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PINWK     |Pin Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode was requested by a transition of the WAKEUP pin (GPC.0)
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[1]     |TMRWK     |Timer Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode (DPD) or Standby Power-down (SPD) mode was requested by wakeup timer time-out
     * |        |          |This flag is cleared when DPD or SPD mode is entered.
     * |[2]     |RTCWK     |RTC Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wakeup of device from Deep Power-down mode (DPD) or Standby Power-down (SPD) mode was requested with a RTC alarm, tick time or tamper happened
     * |        |          |This flag is cleared when DPD or SPD mode is entered.
     * |[8]     |GPAWK     |GPA Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Standby Power-down mode was requested by a transition of selected one GPA group pins
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[9]     |GPBWK     |GPB Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Standby Power-down mode was requested by a transition of selected one GPB group pins
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[10]    |GPCWK     |GPC Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Standby Power-down mode was requested by a transition of selected one GPC group pins
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[11]    |GPDWK     |GPD Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Standby Power-down mode was requested by a transition of selected one GPD group pins
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[12]    |LVRWK     |LVR Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wakeup of device from Standby Power-down mode was requested with a LVR happened
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[13]    |BODWK     |BOD Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wakeup of device from Standby Power-down mode (SPD) was requested with a BOD happened
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[14]    |ACMPWK    |ACMP Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wakeup of device from Standby Power-down mode (SPD) was requested with a ACMP transition
     * |        |          |This flag is cleared when SPD mode is entered.
     * |[31]    |CLRWK     |Clear Wake-up Flag
     * |        |          |0 = No clear.
     * |        |          |1 = Clear all wake-up flag.
     * @var CLK_T::LDOCTL
     * Offset: 0x98  LDO Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18]    |PDBIASEN  |Power-down Bias Enable Bit
     * |        |          |0 = Reserved.
     * |        |          |1 = Power-down bias enabled.
     * |        |          |Note: This bit should set to 1 before chip enter power-down mode.
     * @var CLK_T::SWKDBCTL
     * Offset: 0x9C  Standby Power-down Wake-up De-bounce Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |SWKDBCLKSEL|Standby Power-down Wake-up De-bounce Sampling Cycle Selection
     * |        |          |0000 = Sample wake-up input once per 1 clocks.
     * |        |          |0001 = Sample wake-up input once per 2 clocks.
     * |        |          |0010 = Sample wake-up input once per 4 clocks.
     * |        |          |0011 = Sample wake-up input once per 8 clocks.
     * |        |          |0100 = Sample wake-up input once per 16 clocks.
     * |        |          |0101 = Sample wake-up input once per 32 clocks.
     * |        |          |0110 = Sample wake-up input once per 64 clocks.
     * |        |          |0111 = Sample wake-up input once per 128 clocks.
     * |        |          |1000 = Sample wake-up input once per 256 clocks.
     * |        |          |1001 = Sample wake-up input once per 2*256 clocks.
     * |        |          |1010 = Sample wake-up input once per 4*256 clocks.
     * |        |          |1011 = Sample wake-up input once per 8*256 clocks.
     * |        |          |1100 = Sample wake-up input once per 16*256 clocks.
     * |        |          |1101 = Sample wake-up input once per 32*256 clocks.
     * |        |          |1110 = Sample wake-up input once per 64*256 clocks.
     * |        |          |1111 = Sample wake-up input once per 128*256 clocks.
     * |        |          |Note: De-bounce counter clock source is the 10 kHz internal low speed RC oscillator (LIRC).
     * @var CLK_T::PASWKCTL
     * Offset: 0xA0  GPA Standby Power-down Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |Standby Power-down Pin Wake-up Enable Bit
     * |        |          |0 = GPA group pin wake-up function disabled.
     * |        |          |1 = GPA group pin wake-up function enabled.
     * |[1]     |PRWKEN    |Pin Rising Edge Wake-up Enable Bit
     * |        |          |0 = GPA group pin rising edge wake-up function disabled.
     * |        |          |1 = GPA group pin rising edge wake-up function enabled.
     * |[2]     |PFWKEN    |Pin Falling Edge Wake-up Enable Bit
     * |        |          |0 = GPA group pin falling edge wake-up function disabled.
     * |        |          |1 = GPA group pin falling edge wake-up function enabled.
     * |[7:4]   |WKPSEL    |GPA Standby Power-down Wake-up Pin Select
     * |        |          |0000 = GPA.0 wake-up function enabled.
     * |        |          |0001 = GPA.1 wake-up function enabled.
     * |        |          |0010 = GPA.2 wake-up function enabled.
     * |        |          |0011 = GPA.3 wake-up function enabled.
     * |        |          |0100 = GPA.4 wake-up function enabled.
     * |        |          |0101 = GPA.5 wake-up function enabled.
     * |        |          |0110 = GPA.6 wake-up function enabled.
     * |        |          |0111 = GPA.7 wake-up function enabled.
     * |        |          |1000 = GPA.8 wake-up function enabled.
     * |        |          |1001 = GPA.9 wake-up function enabled.
     * |        |          |1010 = GPA.10 wake-up function enabled.
     * |        |          |1011 = GPA.11 wake-up function enabled.
     * |        |          |1100 = GPA.12 wake-up function enabled.
     * |        |          |1101 = GPA.13 wake-up function enabled.
     * |        |          |1110 = GPA.14 wake-up function enabled.
     * |        |          |1111 = GPA.15 wake-up function enabled.
     * |[8]     |DBEN      |GPA Input Signal De-bounce Enable Bit
     * |        |          |The DBEN bit is used to enable the de-bounce function for each corresponding IO
     * |        |          |If the input signal pulse width cannot be sampled by continuous two de-bounce sample cycle, the input signal transition is seen as the signal bounce and will not trigger the wake-up
     * |        |          |The de-bounce clock source is the 10 kHz internal low speed RC oscillator.
     * |        |          |0 = Standby power-down wake-up pin De-bounce function disable.
     * |        |          |1 = Standby power-down wake-up pin De-bounce function enable.
     * |        |          |The de-bounce function is valid only for edge triggered.
     * @var CLK_T::PBSWKCTL
     * Offset: 0xA4  GPB Standby Power-down Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |Standby Power-down Pin Wake-up Enable Bit
     * |        |          |0 = GPB group pin wake-up function disabled.
     * |        |          |1 = GPB group pin wake-up function enabled.
     * |[1]     |PRWKEN    |Pin Rising Edge Wake-up Enable Bit
     * |        |          |0 = GPB group pin rising edge wake-up function disabled.
     * |        |          |1 = GPB group pin rising edge wake-up function enabled.
     * |[2]     |PFWKEN    |Pin Falling Edge Wake-up Enable Bit
     * |        |          |0 = GPB group pin falling edge wake-up function disabled.
     * |        |          |1 = GPB group pin falling edge wake-up function enabled.
     * |[7:4]   |WKPSEL    |GPB Standby Power-down Wake-up Pin Select
     * |        |          |0000 = GPB.0 wake-up function enabled.
     * |        |          |0001 = GPB.1 wake-up function enabled.
     * |        |          |0010 = GPB.2 wake-up function enabled.
     * |        |          |0011 = GPB.3 wake-up function enabled.
     * |        |          |0100 = GPB.4 wake-up function enabled.
     * |        |          |0101 = GPB.5 wake-up function enabled.
     * |        |          |0110 = GPB.6 wake-up function enabled.
     * |        |          |0111 = GPB.7 wake-up function enabled.
     * |        |          |1000 = GPB.8 wake-up function enabled.
     * |        |          |1001 = GPB.9 wake-up function enabled.
     * |        |          |1010 = GPB.10 wake-up function enabled.
     * |        |          |1011 = GPB.11 wake-up function enabled.
     * |        |          |1100 = GPB.12 wake-up function enabled.
     * |        |          |1101 = GPB.13 wake-up function enabled.
     * |        |          |1110 = GPB.14 wake-up function enabled.
     * |        |          |1111 = GPB.15 wake-up function enabled.
     * |[8]     |DBEN      |GPB Input Signal De-bounce Enable Bit
     * |        |          |The DBEN bit is used to enable the de-bounce function for each corresponding IO
     * |        |          |If the input signal pulse width cannot be sampled by continuous two de-bounce sample cycle, the input signal transition is seen as the signal bounce and will not trigger the wake-up
     * |        |          |The de-bounce clock source is the 10 kHz internal low speed RC oscillator.
     * |        |          |0 = Standby power-down wake-up pin De-bounce function disable.
     * |        |          |1 = Standby power-down wake-up pin De-bounce function enable.
     * |        |          |The de-bounce function is valid only for edge triggered.
     * @var CLK_T::PCSWKCTL
     * Offset: 0xA8  GPC Standby Power-down Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |Standby Power-down Pin Wake-up Enable Bit
     * |        |          |0 = GPC group pin wake-up function disabled.
     * |        |          |1 = GPC group pin wake-up function enabled.
     * |[1]     |PRWKEN    |Pin Rising Edge Wake-up Enable Bit
     * |        |          |0 = GPC group pin rising edge wake-up function disabled.
     * |        |          |1 = GPC group pin rising edge wake-up function enabled.
     * |[2]     |PFWKEN    |Pin Falling Edge Wake-up Enable Bit
     * |        |          |0 = GPC group pin falling edge wake-up function disabled.
     * |        |          |1 = GPC group pin falling edge wake-up function enabled.
     * |[7:4]   |WKPSEL    |GPC Standby Power-down Wake-up Pin Select
     * |        |          |0000 = GPC.0 wake-up function enabled.
     * |        |          |0001 = GPC.1 wake-up function enabled.
     * |        |          |0010 = GPC.2 wake-up function enabled.
     * |        |          |0011 = GPC.3 wake-up function enabled.
     * |        |          |0100 = GPC.4 wake-up function enabled.
     * |        |          |0101 = GPC.5 wake-up function enabled.
     * |        |          |0110 = GPC.6 wake-up function enabled.
     * |        |          |0111 = GPC.7 wake-up function enabled.
     * |        |          |1000 = GPC.8 wake-up function enabled.
     * |        |          |1001 = GPC.9 wake-up function enabled.
     * |        |          |1010 = GPC.10 wake-up function enabled.
     * |        |          |1011 = GPC.11 wake-up function enabled.
     * |        |          |1100 = GPC.12 wake-up function enabled.
     * |        |          |1101 = GPC.13 wake-up function enabled.
     * |        |          |1110 = GPC.14 wake-up function enabled.
     * |        |          |1111 = GPC.15 wake-up function enabled.
     * |[8]     |DBEN      |GPC Input Signal De-bounce Enable Bit
     * |        |          |The DBEN bit is used to enable the de-bounce function for each corresponding IO
     * |        |          |If the input signal pulse width cannot be sampled by continuous two de-bounce sample cycle, the input signal transition is seen as the signal bounce and will not trigger the wake-up
     * |        |          |The de-bounce clock source is the 10 kHz internal low speed RC oscillator.
     * |        |          |0 = Standby power-down wake-up pin De-bounce function disable.
     * |        |          |1 = Standby power-down wake-up pin De-bounce function enable.
     * |        |          |The de-bounce function is valid only for edge triggered.
     * @var CLK_T::PDSWKCTL
     * Offset: 0xAC  GPD Standby Power-down Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |Standby Power-down Pin Wake-up Enable Bit
     * |        |          |0 = GPD group pin wake-up function disabled.
     * |        |          |1 = GPD group pin wake-up function enabled.
     * |[1]     |PRWKEN    |Pin Rising Edge Wake-up Enable Bit
     * |        |          |0 = GPD group pin rising edge wake-up function disabled.
     * |        |          |1 = GPD group pin rising edge wake-up function enabled.
     * |[2]     |PFWKEN    |Pin Falling Edge Wake-up Enable Bit
     * |        |          |0 = GPD group pin falling edge wake-up function disabled.
     * |        |          |1 = GPD group pin falling edge wake-up function enabled.
     * |[7:4]   |WKPSEL    |GPD Standby Power-down Wake-up Pin Select
     * |        |          |0000 = GPD.0 wake-up function enabled.
     * |        |          |0001 = GPD.1 wake-up function enabled.
     * |        |          |0010 = GPD.2 wake-up function enabled.
     * |        |          |0011 = GPD.3 wake-up function enabled.
     * |        |          |0100 = GPD.4 wake-up function enabled.
     * |        |          |0101 = GPD.5 wake-up function enabled.
     * |        |          |0110 = GPD.6 wake-up function enabled.
     * |        |          |0111 = GPD.7 wake-up function enabled.
     * |        |          |1000 = GPD.8 wake-up function enabled.
     * |        |          |1001 = GPD.9 wake-up function enabled.
     * |        |          |1010 = GPD.10 wake-up function enabled.
     * |        |          |1011 = GPD.11 wake-up function enabled.
     * |        |          |1100 = GPD.12 wake-up function enabled.
     * |        |          |1101 = GPD.13 wake-up function enabled.
     * |        |          |1110 = GPD.14 wake-up function enabled.
     * |        |          |1111 = GPD.15 wake-up function enabled.
     * |[8]     |DBEN      |GPD Input Signal De-bounce Enable Bit
     * |        |          |The DBEN bit is used to enable the de-bounce function for each corresponding IO
     * |        |          |If the input signal pulse width cannot be sampled by continuous two de-bounce sample cycle, the input signal transition is seen as the signal bounce and will not trigger the wake-up
     * |        |          |The de-bounce clock source is the 10 kHz internal low speed RC oscillator.
     * |        |          |0 = Standby power-down wake-up pin De-bounce function disable.
     * |        |          |1 = Standby power-down wake-up pin De-bounce function enable.
     * |        |          |The de-bounce function is valid only for edge triggered.
     * @var CLK_T::IOPDCTL
     * Offset: 0xB0  GPIO Standby Power-down Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |IOHR      |GPIO Hold Release
     * |        |          |When GPIO enter standby power-down mode, all I/O status are hold to keep normal operating status
     * |        |          |After chip was waked up from standby power-down mode, the I/O are still keep hold status until user set this bit to release I/O hold status.
     * |        |          |This bit is auto cleared by hardware.
     */
    __IO uint32_t PWRCTL;                /*!< [0x0000] System Power-down Control Register                               */
    __IO uint32_t AHBCLK;                /*!< [0x0004] AHB Devices Clock Enable Control Register                        */
    __IO uint32_t APBCLK0;               /*!< [0x0008] APB Devices Clock Enable Control Register 0                      */
    __IO uint32_t APBCLK1;               /*!< [0x000c] APB Devices Clock Enable Control Register 1                      */
    __IO uint32_t CLKSEL0;               /*!< [0x0010] Clock Source Select Control Register 0                           */
    __IO uint32_t CLKSEL1;               /*!< [0x0014] Clock Source Select Control Register 1                           */
    __IO uint32_t CLKSEL2;               /*!< [0x0018] Clock Source Select Control Register 2                           */
    __IO uint32_t CLKSEL3;               /*!< [0x001c] Clock Source Select Control Register 3                           */
    __IO uint32_t CLKDIV0;               /*!< [0x0020] Clock Divider Number Register 0                                  */
    __IO uint32_t CLKDIV1;               /*!< [0x0024] Clock Divider Number Register 1                                  */
    __IO uint32_t CLKDIV2;               /*!< [0x0028] Clock Divider Number Register 2                                  */
    __IO uint32_t CLKDIV3;               /*!< [0x002c] Clock Divider Number Register 3                                  */
    __IO uint32_t CLKDIV4;               /*!< [0x0030] Clock Divider Number Register 4                                  */
    __IO uint32_t PCLKDIV;               /*!< [0x0034] APB Clock Divider Register                                       */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE1[2];
    /** @endcond */
    __IO uint32_t PLLCTL;                /*!< [0x0040] PLL Control Register                                             */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE2[3];
    /** @endcond */
    __I  uint32_t STATUS;                /*!< [0x0050] Clock Status Monitor Register                                    */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE3[3];
    /** @endcond */
    __IO uint32_t CLKOCTL;               /*!< [0x0060] Clock Output Control Register                                    */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE4[3];
    /** @endcond */
    __IO uint32_t CLKDCTL;               /*!< [0x0070] Clock Fail Detector Control Register                             */
    __IO uint32_t CLKDSTS;               /*!< [0x0074] Clock Fail Detector Status Register                              */
    __IO uint32_t CDUPB;                 /*!< [0x0078] Clock Frequency Range Detector Upper Boundary Register           */
    __IO uint32_t CDLOWB;                /*!< [0x007c] Clock Frequency Range Detector Lower Boundary Register           */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE5[4];
    /** @endcond */
    __IO uint32_t PMUCTL;                /*!< [0x0090] Power Manager Control Register                                   */
    __IO uint32_t PMUSTS;                /*!< [0x0094] Power Manager Status Register                                    */
    __IO uint32_t LDOCTL;                /*!< [0x0098] LDO Control Register                                             */
    __IO uint32_t SWKDBCTL;              /*!< [0x009c] Standby Power-down Wake-up De-bounce Control Register            */
    __IO uint32_t PASWKCTL;              /*!< [0x00a0] GPA Standby Power-down Wake-up Control Register                  */
    __IO uint32_t PBSWKCTL;              /*!< [0x00a4] GPB Standby Power-down Wake-up Control Register                  */
    __IO uint32_t PCSWKCTL;              /*!< [0x00a8] GPC Standby Power-down Wake-up Control Register                  */
    __IO uint32_t PDSWKCTL;              /*!< [0x00ac] GPD Standby Power-down Wake-up Control Register                  */
    __IO uint32_t IOPDCTL;               /*!< [0x00b0] GPIO Standby Power-down Control Register                         */

} CLK_T;

/**
    @addtogroup CLK_CONST CLK Bit Field Definition
    Constant Definitions for CLK Controller
@{ */

#define CLK_PWRCTL_HXTEN_Pos             (0)                                               /*!< CLK_T::PWRCTL: HXTEN Position          */
#define CLK_PWRCTL_HXTEN_Msk             (0x1ul << CLK_PWRCTL_HXTEN_Pos)                   /*!< CLK_T::PWRCTL: HXTEN Mask              */

#define CLK_PWRCTL_LXTEN_Pos             (1)                                               /*!< CLK_T::PWRCTL: LXTEN Position          */
#define CLK_PWRCTL_LXTEN_Msk             (0x1ul << CLK_PWRCTL_LXTEN_Pos)                   /*!< CLK_T::PWRCTL: LXTEN Mask              */

#define CLK_PWRCTL_HIRCEN_Pos            (2)                                               /*!< CLK_T::PWRCTL: HIRCEN Position         */
#define CLK_PWRCTL_HIRCEN_Msk            (0x1ul << CLK_PWRCTL_HIRCEN_Pos)                  /*!< CLK_T::PWRCTL: HIRCEN Mask             */

#define CLK_PWRCTL_LIRCEN_Pos            (3)                                               /*!< CLK_T::PWRCTL: LIRCEN Position         */
#define CLK_PWRCTL_LIRCEN_Msk            (0x1ul << CLK_PWRCTL_LIRCEN_Pos)                  /*!< CLK_T::PWRCTL: LIRCEN Mask             */

#define CLK_PWRCTL_PDWKDLY_Pos           (4)                                               /*!< CLK_T::PWRCTL: PDWKDLY Position        */
#define CLK_PWRCTL_PDWKDLY_Msk           (0x1ul << CLK_PWRCTL_PDWKDLY_Pos)                 /*!< CLK_T::PWRCTL: PDWKDLY Mask            */

#define CLK_PWRCTL_PDWKIEN_Pos           (5)                                               /*!< CLK_T::PWRCTL: PDWKIEN Position        */
#define CLK_PWRCTL_PDWKIEN_Msk           (0x1ul << CLK_PWRCTL_PDWKIEN_Pos)                 /*!< CLK_T::PWRCTL: PDWKIEN Mask            */

#define CLK_PWRCTL_PDWKIF_Pos            (6)                                               /*!< CLK_T::PWRCTL: PDWKIF Position         */
#define CLK_PWRCTL_PDWKIF_Msk            (0x1ul << CLK_PWRCTL_PDWKIF_Pos)                  /*!< CLK_T::PWRCTL: PDWKIF Mask             */

#define CLK_PWRCTL_PDEN_Pos              (7)                                               /*!< CLK_T::PWRCTL: PDEN Position           */
#define CLK_PWRCTL_PDEN_Msk              (0x1ul << CLK_PWRCTL_PDEN_Pos)                    /*!< CLK_T::PWRCTL: PDEN Mask               */

#define CLK_PWRCTL_HXTGAIN_Pos           (10)                                              /*!< CLK_T::PWRCTL: HXTGAIN Position        */
#define CLK_PWRCTL_HXTGAIN_Msk           (0x3ul << CLK_PWRCTL_HXTGAIN_Pos)                 /*!< CLK_T::PWRCTL: HXTGAIN Mask            */

#define CLK_PWRCTL_HXTSELTYP_Pos         (12)                                              /*!< CLK_T::PWRCTL: HXTSELTYP Position      */
#define CLK_PWRCTL_HXTSELTYP_Msk         (0x1ul << CLK_PWRCTL_HXTSELTYP_Pos)               /*!< CLK_T::PWRCTL: HXTSELTYP Mask          */

#define CLK_PWRCTL_HXTTBEN_Pos           (13)                                              /*!< CLK_T::PWRCTL: HXTTBEN Position        */
#define CLK_PWRCTL_HXTTBEN_Msk           (0x1ul << CLK_PWRCTL_HXTTBEN_Pos)                 /*!< CLK_T::PWRCTL: HXTTBEN Mask            */

#define CLK_PWRCTL_HIRCSTBS_Pos          (16)                                              /*!< CLK_T::PWRCTL: HIRCSTBS Position       */
#define CLK_PWRCTL_HIRCSTBS_Msk          (0x3ul << CLK_PWRCTL_HIRCSTBS_Pos)                /*!< CLK_T::PWRCTL: HIRCSTBS Mask           */

#define CLK_PWRCTL_HIRC48MEN_Pos         (18)                                              /*!< CLK_T::PWRCTL: HIRC48MEN Position      */
#define CLK_PWRCTL_HIRC48MEN_Msk         (0x1ul << CLK_PWRCTL_HIRC48MEN_Pos)               /*!< CLK_T::PWRCTL: HIRC48MEN Mask          */

#define CLK_AHBCLK_PDMACKEN_Pos          (1)                                               /*!< CLK_T::AHBCLK: PDMACKEN Position       */
#define CLK_AHBCLK_PDMACKEN_Msk          (0x1ul << CLK_AHBCLK_PDMACKEN_Pos)                /*!< CLK_T::AHBCLK: PDMACKEN Mask           */

#define CLK_AHBCLK_ISPCKEN_Pos           (2)                                               /*!< CLK_T::AHBCLK: ISPCKEN Position        */
#define CLK_AHBCLK_ISPCKEN_Msk           (0x1ul << CLK_AHBCLK_ISPCKEN_Pos)                 /*!< CLK_T::AHBCLK: ISPCKEN Mask            */

#define CLK_AHBCLK_EBICKEN_Pos           (3)                                               /*!< CLK_T::AHBCLK: EBICKEN Position        */
#define CLK_AHBCLK_EBICKEN_Msk           (0x1ul << CLK_AHBCLK_EBICKEN_Pos)                 /*!< CLK_T::AHBCLK: EBICKEN Mask            */

#define CLK_AHBCLK_EMACCKEN_Pos          (5)                                               /*!< CLK_T::AHBCLK: EMACCKEN Position       */
#define CLK_AHBCLK_EMACCKEN_Msk          (0x1ul << CLK_AHBCLK_EMACCKEN_Pos)                /*!< CLK_T::AHBCLK: EMACCKEN Mask           */

#define CLK_AHBCLK_SDH0CKEN_Pos          (6)                                               /*!< CLK_T::AHBCLK: SDH0CKEN Position       */
#define CLK_AHBCLK_SDH0CKEN_Msk          (0x1ul << CLK_AHBCLK_SDH0CKEN_Pos)                /*!< CLK_T::AHBCLK: SDH0CKEN Mask           */

#define CLK_AHBCLK_CRCCKEN_Pos           (7)                                               /*!< CLK_T::AHBCLK: CRCCKEN Position        */
#define CLK_AHBCLK_CRCCKEN_Msk           (0x1ul << CLK_AHBCLK_CRCCKEN_Pos)                 /*!< CLK_T::AHBCLK: CRCCKEN Mask            */

#define CLK_AHBCLK_CCAPCKEN_Pos          (8)                                               /*!< CLK_T::AHBCLK: CCAPCKEN Position       */
#define CLK_AHBCLK_CCAPCKEN_Msk          (0x1ul << CLK_AHBCLK_CCAPCKEN_Pos)                /*!< CLK_T::AHBCLK: CCAPCKEN Mask           */

#define CLK_AHBCLK_SENCKEN_Pos           (9)                                               /*!< CLK_T::AHBCLK: SENCKEN Position        */
#define CLK_AHBCLK_SENCKEN_Msk           (0x1ul << CLK_AHBCLK_SENCKEN_Pos)                 /*!< CLK_T::AHBCLK: SENCKEN Mask            */

#define CLK_AHBCLK_HSUSBDCKEN_Pos        (10)                                              /*!< CLK_T::AHBCLK: HSUSBDCKEN Position     */
#define CLK_AHBCLK_HSUSBDCKEN_Msk        (0x1ul << CLK_AHBCLK_HSUSBDCKEN_Pos)              /*!< CLK_T::AHBCLK: HSUSBDCKEN Mask         */

#define CLK_AHBCLK_CRPTCKEN_Pos          (12)                                              /*!< CLK_T::AHBCLK: CRPTCKEN Position       */
#define CLK_AHBCLK_CRPTCKEN_Msk          (0x1ul << CLK_AHBCLK_CRPTCKEN_Pos)                /*!< CLK_T::AHBCLK: CRPTCKEN Mask           */

#define CLK_AHBCLK_SPIMCKEN_Pos          (14)                                              /*!< CLK_T::AHBCLK: SPIMCKEN Position       */
#define CLK_AHBCLK_SPIMCKEN_Msk          (0x1ul << CLK_AHBCLK_SPIMCKEN_Pos)                /*!< CLK_T::AHBCLK: SPIMCKEN Mask           */

#define CLK_AHBCLK_FMCIDLE_Pos           (15)                                              /*!< CLK_T::AHBCLK: FMCIDLE Position        */
#define CLK_AHBCLK_FMCIDLE_Msk           (0x1ul << CLK_AHBCLK_FMCIDLE_Pos)                 /*!< CLK_T::AHBCLK: FMCIDLE Mask            */

#define CLK_AHBCLK_USBHCKEN_Pos          (16)                                              /*!< CLK_T::AHBCLK: USBHCKEN Position       */
#define CLK_AHBCLK_USBHCKEN_Msk          (0x1ul << CLK_AHBCLK_USBHCKEN_Pos)                /*!< CLK_T::AHBCLK: USBHCKEN Mask           */

#define CLK_AHBCLK_SDH1CKEN_Pos          (17)                                              /*!< CLK_T::AHBCLK: SDH1CKEN Position       */
#define CLK_AHBCLK_SDH1CKEN_Msk          (0x1ul << CLK_AHBCLK_SDH1CKEN_Pos)                /*!< CLK_T::AHBCLK: SDH1CKEN Mask           */

#define CLK_APBCLK0_WDTCKEN_Pos          (0)                                               /*!< CLK_T::APBCLK0: WDTCKEN Position       */
#define CLK_APBCLK0_WDTCKEN_Msk          (0x1ul << CLK_APBCLK0_WDTCKEN_Pos)                /*!< CLK_T::APBCLK0: WDTCKEN Mask           */

#define CLK_APBCLK0_RTCCKEN_Pos          (1)                                               /*!< CLK_T::APBCLK0: RTCCKEN Position       */
#define CLK_APBCLK0_RTCCKEN_Msk          (0x1ul << CLK_APBCLK0_RTCCKEN_Pos)                /*!< CLK_T::APBCLK0: RTCCKEN Mask           */

#define CLK_APBCLK0_TMR0CKEN_Pos         (2)                                               /*!< CLK_T::APBCLK0: TMR0CKEN Position      */
#define CLK_APBCLK0_TMR0CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR0CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR0CKEN Mask          */

#define CLK_APBCLK0_TMR1CKEN_Pos         (3)                                               /*!< CLK_T::APBCLK0: TMR1CKEN Position      */
#define CLK_APBCLK0_TMR1CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR1CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR1CKEN Mask          */

#define CLK_APBCLK0_TMR2CKEN_Pos         (4)                                               /*!< CLK_T::APBCLK0: TMR2CKEN Position      */
#define CLK_APBCLK0_TMR2CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR2CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR2CKEN Mask          */

#define CLK_APBCLK0_TMR3CKEN_Pos         (5)                                               /*!< CLK_T::APBCLK0: TMR3CKEN Position      */
#define CLK_APBCLK0_TMR3CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR3CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR3CKEN Mask          */

#define CLK_APBCLK0_CLKOCKEN_Pos         (6)                                               /*!< CLK_T::APBCLK0: CLKOCKEN Position      */
#define CLK_APBCLK0_CLKOCKEN_Msk         (0x1ul << CLK_APBCLK0_CLKOCKEN_Pos)               /*!< CLK_T::APBCLK0: CLKOCKEN Mask          */

#define CLK_APBCLK0_ACMP01CKEN_Pos       (7)                                               /*!< CLK_T::APBCLK0: ACMP01CKEN Position    */
#define CLK_APBCLK0_ACMP01CKEN_Msk       (0x1ul << CLK_APBCLK0_ACMP01CKEN_Pos)             /*!< CLK_T::APBCLK0: ACMP01CKEN Mask        */

#define CLK_APBCLK0_I2C0CKEN_Pos         (8)                                               /*!< CLK_T::APBCLK0: I2C0CKEN Position      */
#define CLK_APBCLK0_I2C0CKEN_Msk         (0x1ul << CLK_APBCLK0_I2C0CKEN_Pos)               /*!< CLK_T::APBCLK0: I2C0CKEN Mask          */

#define CLK_APBCLK0_I2C1CKEN_Pos         (9)                                               /*!< CLK_T::APBCLK0: I2C1CKEN Position      */
#define CLK_APBCLK0_I2C1CKEN_Msk         (0x1ul << CLK_APBCLK0_I2C1CKEN_Pos)               /*!< CLK_T::APBCLK0: I2C1CKEN Mask          */

#define CLK_APBCLK0_I2C2CKEN_Pos         (10)                                              /*!< CLK_T::APBCLK0: I2C2CKEN Position      */
#define CLK_APBCLK0_I2C2CKEN_Msk         (0x1ul << CLK_APBCLK0_I2C2CKEN_Pos)               /*!< CLK_T::APBCLK0: I2C2CKEN Mask          */

#define CLK_APBCLK0_QSPI0CKEN_Pos        (12)                                              /*!< CLK_T::APBCLK0: QSPI0CKEN Position     */
#define CLK_APBCLK0_QSPI0CKEN_Msk        (0x1ul << CLK_APBCLK0_QSPI0CKEN_Pos)              /*!< CLK_T::APBCLK0: QSPI0CKEN Mask         */

#define CLK_APBCLK0_SPI0CKEN_Pos         (13)                                              /*!< CLK_T::APBCLK0: SPI0CKEN Position      */
#define CLK_APBCLK0_SPI0CKEN_Msk         (0x1ul << CLK_APBCLK0_SPI0CKEN_Pos)               /*!< CLK_T::APBCLK0: SPI0CKEN Mask          */

#define CLK_APBCLK0_SPI1CKEN_Pos         (14)                                              /*!< CLK_T::APBCLK0: SPI1CKEN Position      */
#define CLK_APBCLK0_SPI1CKEN_Msk         (0x1ul << CLK_APBCLK0_SPI1CKEN_Pos)               /*!< CLK_T::APBCLK0: SPI1CKEN Mask          */

#define CLK_APBCLK0_SPI2CKEN_Pos         (15)                                              /*!< CLK_T::APBCLK0: SPI2CKEN Position      */
#define CLK_APBCLK0_SPI2CKEN_Msk         (0x1ul << CLK_APBCLK0_SPI2CKEN_Pos)               /*!< CLK_T::APBCLK0: SPI2CKEN Mask          */

#define CLK_APBCLK0_UART0CKEN_Pos        (16)                                              /*!< CLK_T::APBCLK0: UART0CKEN Position     */
#define CLK_APBCLK0_UART0CKEN_Msk        (0x1ul << CLK_APBCLK0_UART0CKEN_Pos)              /*!< CLK_T::APBCLK0: UART0CKEN Mask         */

#define CLK_APBCLK0_UART1CKEN_Pos        (17)                                              /*!< CLK_T::APBCLK0: UART1CKEN Position     */
#define CLK_APBCLK0_UART1CKEN_Msk        (0x1ul << CLK_APBCLK0_UART1CKEN_Pos)              /*!< CLK_T::APBCLK0: UART1CKEN Mask         */

#define CLK_APBCLK0_UART2CKEN_Pos        (18)                                              /*!< CLK_T::APBCLK0: UART2CKEN Position     */
#define CLK_APBCLK0_UART2CKEN_Msk        (0x1ul << CLK_APBCLK0_UART2CKEN_Pos)              /*!< CLK_T::APBCLK0: UART2CKEN Mask         */

#define CLK_APBCLK0_UART3CKEN_Pos        (19)                                              /*!< CLK_T::APBCLK0: UART3CKEN Position     */
#define CLK_APBCLK0_UART3CKEN_Msk        (0x1ul << CLK_APBCLK0_UART3CKEN_Pos)              /*!< CLK_T::APBCLK0: UART3CKEN Mask         */

#define CLK_APBCLK0_UART4CKEN_Pos        (20)                                              /*!< CLK_T::APBCLK0: UART4CKEN Position     */
#define CLK_APBCLK0_UART4CKEN_Msk        (0x1ul << CLK_APBCLK0_UART4CKEN_Pos)              /*!< CLK_T::APBCLK0: UART4CKEN Mask         */

#define CLK_APBCLK0_UART5CKEN_Pos        (21)                                              /*!< CLK_T::APBCLK0: UART5CKEN Position     */
#define CLK_APBCLK0_UART5CKEN_Msk        (0x1ul << CLK_APBCLK0_UART5CKEN_Pos)              /*!< CLK_T::APBCLK0: UART5CKEN Mask         */

#define CLK_APBCLK0_UART6CKEN_Pos        (22)                                              /*!< CLK_T::APBCLK0: UART6CKEN Position     */
#define CLK_APBCLK0_UART6CKEN_Msk        (0x1ul << CLK_APBCLK0_UART6CKEN_Pos)              /*!< CLK_T::APBCLK0: UART6CKEN Mask         */

#define CLK_APBCLK0_UART7CKEN_Pos        (23)                                              /*!< CLK_T::APBCLK0: UART7CKEN Position     */
#define CLK_APBCLK0_UART7CKEN_Msk        (0x1ul << CLK_APBCLK0_UART7CKEN_Pos)              /*!< CLK_T::APBCLK0: UART7CKEN Mask         */

#define CLK_APBCLK0_CAN0CKEN_Pos         (24)                                              /*!< CLK_T::APBCLK0: CAN0CKEN Position      */
#define CLK_APBCLK0_CAN0CKEN_Msk         (0x1ul << CLK_APBCLK0_CAN0CKEN_Pos)               /*!< CLK_T::APBCLK0: CAN0CKEN Mask          */

#define CLK_APBCLK0_CAN1CKEN_Pos         (25)                                              /*!< CLK_T::APBCLK0: CAN1CKEN Position      */
#define CLK_APBCLK0_CAN1CKEN_Msk         (0x1ul << CLK_APBCLK0_CAN1CKEN_Pos)               /*!< CLK_T::APBCLK0: CAN1CKEN Mask          */

#define CLK_APBCLK0_OTGCKEN_Pos          (26)                                              /*!< CLK_T::APBCLK0: OTGCKEN Position       */
#define CLK_APBCLK0_OTGCKEN_Msk          (0x1ul << CLK_APBCLK0_OTGCKEN_Pos)                /*!< CLK_T::APBCLK0: OTGCKEN Mask           */

#define CLK_APBCLK0_USBDCKEN_Pos         (27)                                              /*!< CLK_T::APBCLK0: USBDCKEN Position      */
#define CLK_APBCLK0_USBDCKEN_Msk         (0x1ul << CLK_APBCLK0_USBDCKEN_Pos)               /*!< CLK_T::APBCLK0: USBDCKEN Mask          */

#define CLK_APBCLK0_EADCCKEN_Pos         (28)                                              /*!< CLK_T::APBCLK0: EADCCKEN Position      */
#define CLK_APBCLK0_EADCCKEN_Msk         (0x1ul << CLK_APBCLK0_EADCCKEN_Pos)               /*!< CLK_T::APBCLK0: EADCCKEN Mask          */

#define CLK_APBCLK0_I2S0CKEN_Pos         (29)                                              /*!< CLK_T::APBCLK0: I2S0CKEN Position      */
#define CLK_APBCLK0_I2S0CKEN_Msk         (0x1ul << CLK_APBCLK0_I2S0CKEN_Pos)               /*!< CLK_T::APBCLK0: I2S0CKEN Mask          */

#define CLK_APBCLK0_HSOTGCKEN_Pos        (30)                                              /*!< CLK_T::APBCLK0: HSOTGCKEN Position     */
#define CLK_APBCLK0_HSOTGCKEN_Msk        (0x1ul << CLK_APBCLK0_HSOTGCKEN_Pos)              /*!< CLK_T::APBCLK0: HSOTGCKEN Mask         */

#define CLK_APBCLK1_SC0CKEN_Pos          (0)                                               /*!< CLK_T::APBCLK1: SC0CKEN Position       */
#define CLK_APBCLK1_SC0CKEN_Msk          (0x1ul << CLK_APBCLK1_SC0CKEN_Pos)                /*!< CLK_T::APBCLK1: SC0CKEN Mask           */

#define CLK_APBCLK1_SC1CKEN_Pos          (1)                                               /*!< CLK_T::APBCLK1: SC1CKEN Position       */
#define CLK_APBCLK1_SC1CKEN_Msk          (0x1ul << CLK_APBCLK1_SC1CKEN_Pos)                /*!< CLK_T::APBCLK1: SC1CKEN Mask           */

#define CLK_APBCLK1_SC2CKEN_Pos          (2)                                               /*!< CLK_T::APBCLK1: SC2CKEN Position       */
#define CLK_APBCLK1_SC2CKEN_Msk          (0x1ul << CLK_APBCLK1_SC2CKEN_Pos)                /*!< CLK_T::APBCLK1: SC2CKEN Mask           */

#define CLK_APBCLK1_QSPI1CKEN_Pos        (4)                                               /*!< CLK_T::APBCLK1: QSPI1CKEN Position     */
#define CLK_APBCLK1_QSPI1CKEN_Msk        (0x1ul << CLK_APBCLK1_QSPI1CKEN_Pos)              /*!< CLK_T::APBCLK1: QSPI1CKEN Mask         */

#define CLK_APBCLK1_SPI3CKEN_Pos         (6)                                               /*!< CLK_T::APBCLK1: SPI3CKEN Position      */
#define CLK_APBCLK1_SPI3CKEN_Msk         (0x1ul << CLK_APBCLK1_SPI3CKEN_Pos)               /*!< CLK_T::APBCLK1: SPI3CKEN Mask          */

#define CLK_APBCLK1_USCI0CKEN_Pos        (8)                                               /*!< CLK_T::APBCLK1: USCI0CKEN Position     */
#define CLK_APBCLK1_USCI0CKEN_Msk        (0x1ul << CLK_APBCLK1_USCI0CKEN_Pos)              /*!< CLK_T::APBCLK1: USCI0CKEN Mask         */

#define CLK_APBCLK1_USCI1CKEN_Pos        (9)                                               /*!< CLK_T::APBCLK1: USCI1CKEN Position     */
#define CLK_APBCLK1_USCI1CKEN_Msk        (0x1ul << CLK_APBCLK1_USCI1CKEN_Pos)              /*!< CLK_T::APBCLK1: USCI1CKEN Mask         */

#define CLK_APBCLK1_DACCKEN_Pos          (12)                                              /*!< CLK_T::APBCLK1: DACCKEN Position       */
#define CLK_APBCLK1_DACCKEN_Msk          (0x1ul << CLK_APBCLK1_DACCKEN_Pos)                /*!< CLK_T::APBCLK1: DACCKEN Mask           */

#define CLK_APBCLK1_EPWM0CKEN_Pos        (16)                                              /*!< CLK_T::APBCLK1: EPWM0CKEN Position     */
#define CLK_APBCLK1_EPWM0CKEN_Msk        (0x1ul << CLK_APBCLK1_EPWM0CKEN_Pos)              /*!< CLK_T::APBCLK1: EPWM0CKEN Mask         */

#define CLK_APBCLK1_EPWM1CKEN_Pos        (17)                                              /*!< CLK_T::APBCLK1: EPWM1CKEN Position     */
#define CLK_APBCLK1_EPWM1CKEN_Msk        (0x1ul << CLK_APBCLK1_EPWM1CKEN_Pos)              /*!< CLK_T::APBCLK1: EPWM1CKEN Mask         */

#define CLK_APBCLK1_BPWM0CKEN_Pos        (18)                                              /*!< CLK_T::APBCLK1: BPWM0CKEN Position     */
#define CLK_APBCLK1_BPWM0CKEN_Msk        (0x1ul << CLK_APBCLK1_BPWM0CKEN_Pos)              /*!< CLK_T::APBCLK1: BPWM0CKEN Mask         */

#define CLK_APBCLK1_BPWM1CKEN_Pos        (19)                                              /*!< CLK_T::APBCLK1: BPWM1CKEN Position     */
#define CLK_APBCLK1_BPWM1CKEN_Msk        (0x1ul << CLK_APBCLK1_BPWM1CKEN_Pos)              /*!< CLK_T::APBCLK1: BPWM1CKEN Mask         */

#define CLK_APBCLK1_QEI0CKEN_Pos         (22)                                              /*!< CLK_T::APBCLK1: QEI0CKEN Position      */
#define CLK_APBCLK1_QEI0CKEN_Msk         (0x1ul << CLK_APBCLK1_QEI0CKEN_Pos)               /*!< CLK_T::APBCLK1: QEI0CKEN Mask          */

#define CLK_APBCLK1_QEI1CKEN_Pos         (23)                                              /*!< CLK_T::APBCLK1: QEI1CKEN Position      */
#define CLK_APBCLK1_QEI1CKEN_Msk         (0x1ul << CLK_APBCLK1_QEI1CKEN_Pos)               /*!< CLK_T::APBCLK1: QEI1CKEN Mask          */

#define CLK_APBCLK1_TRNGCKEN_Pos         (25)                                              /*!< CLK_T::APBCLK1: TRNGCKEN Position     */
#define CLK_APBCLK1_TRNGCKEN_Msk         (0x1ul << CLK_APBCLK1_TRNGCKEN_Pos)               /*!< CLK_T::APBCLK1: TRNGCKEN Mask         */

#define CLK_APBCLK1_ECAP0CKEN_Pos        (26)                                              /*!< CLK_T::APBCLK1: ECAP0CKEN Position     */
#define CLK_APBCLK1_ECAP0CKEN_Msk        (0x1ul << CLK_APBCLK1_ECAP0CKEN_Pos)              /*!< CLK_T::APBCLK1: ECAP0CKEN Mask         */

#define CLK_APBCLK1_ECAP1CKEN_Pos        (27)                                              /*!< CLK_T::APBCLK1: ECAP1CKEN Position     */
#define CLK_APBCLK1_ECAP1CKEN_Msk        (0x1ul << CLK_APBCLK1_ECAP1CKEN_Pos)              /*!< CLK_T::APBCLK1: ECAP1CKEN Mask         */

#define CLK_APBCLK1_CAN2CKEN_Pos         (28)                                              /*!< CLK_T::APBCLK1: CAN2CKEN Position      */
#define CLK_APBCLK1_CAN2CKEN_Msk         (0x1ul << CLK_APBCLK1_CAN2CKEN_Pos)               /*!< CLK_T::APBCLK1: CAN2CKEN Mask          */

#define CLK_APBCLK1_OPACKEN_Pos          (30)                                              /*!< CLK_T::APBCLK1: OPACKEN Position       */
#define CLK_APBCLK1_OPACKEN_Msk          (0x1ul << CLK_APBCLK1_OPACKEN_Pos)                /*!< CLK_T::APBCLK1: OPACKEN Mask           */

#define CLK_APBCLK1_EADC1CKEN_Pos        (31)                                              /*!< CLK_T::APBCLK1: EADC1CKEN Position     */
#define CLK_APBCLK1_EADC1CKEN_Msk        (0x1ul << CLK_APBCLK1_EADC1CKEN_Pos)              /*!< CLK_T::APBCLK1: EADC1CKEN Mask         */

#define CLK_CLKSEL0_HCLKSEL_Pos          (0)                                               /*!< CLK_T::CLKSEL0: HCLKSEL Position       */
#define CLK_CLKSEL0_HCLKSEL_Msk          (0x7ul << CLK_CLKSEL0_HCLKSEL_Pos)                /*!< CLK_T::CLKSEL0: HCLKSEL Mask           */

#define CLK_CLKSEL0_STCLKSEL_Pos         (3)                                               /*!< CLK_T::CLKSEL0: STCLKSEL Position      */
#define CLK_CLKSEL0_STCLKSEL_Msk         (0x7ul << CLK_CLKSEL0_STCLKSEL_Pos)               /*!< CLK_T::CLKSEL0: STCLKSEL Mask          */

#define CLK_CLKSEL0_USBSEL_Pos           (8)                                               /*!< CLK_T::CLKSEL0: PCLK0SEL Position      */
#define CLK_CLKSEL0_USBSEL_Msk           (0x1ul << CLK_CLKSEL0_USBSEL_Pos)                 /*!< CLK_T::CLKSEL0: PCLK0SEL Mask          */

#define CLK_CLKSEL0_CCAPSEL_Pos          (16)                                              /*!< CLK_T::CLKSEL0: CCAPSEL Position      */
#define CLK_CLKSEL0_CCAPSEL_Msk          (0x3ul << CLK_CLKSEL0_CCAPSEL_Pos)                /*!< CLK_T::CLKSEL0: CCAPSEL Mask          */

#define CLK_CLKSEL0_SDH0SEL_Pos          (20)                                              /*!< CLK_T::CLKSEL0: SDH0SEL Position       */
#define CLK_CLKSEL0_SDH0SEL_Msk          (0x3ul << CLK_CLKSEL0_SDH0SEL_Pos)                /*!< CLK_T::CLKSEL0: SDH0SEL Mask           */

#define CLK_CLKSEL0_SDH1SEL_Pos          (22)                                              /*!< CLK_T::CLKSEL0: SDH1SEL Position       */
#define CLK_CLKSEL0_SDH1SEL_Msk          (0x3ul << CLK_CLKSEL0_SDH1SEL_Pos)                /*!< CLK_T::CLKSEL0: SDH1SEL Mask           */

#define CLK_CLKSEL1_WDTSEL_Pos           (0)                                               /*!< CLK_T::CLKSEL1: WDTSEL Position        */
#define CLK_CLKSEL1_WDTSEL_Msk           (0x3ul << CLK_CLKSEL1_WDTSEL_Pos)                 /*!< CLK_T::CLKSEL1: WDTSEL Mask            */

#define CLK_CLKSEL1_TMR0SEL_Pos          (8)                                               /*!< CLK_T::CLKSEL1: TMR0SEL Position       */
#define CLK_CLKSEL1_TMR0SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR0SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR0SEL Mask           */

#define CLK_CLKSEL1_TMR1SEL_Pos          (12)                                              /*!< CLK_T::CLKSEL1: TMR1SEL Position       */
#define CLK_CLKSEL1_TMR1SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR1SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR1SEL Mask           */

#define CLK_CLKSEL1_TMR2SEL_Pos          (16)                                              /*!< CLK_T::CLKSEL1: TMR2SEL Position       */
#define CLK_CLKSEL1_TMR2SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR2SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR2SEL Mask           */

#define CLK_CLKSEL1_TMR3SEL_Pos          (20)                                              /*!< CLK_T::CLKSEL1: TMR3SEL Position       */
#define CLK_CLKSEL1_TMR3SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR3SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR3SEL Mask           */

#define CLK_CLKSEL1_UART0SEL_Pos         (24)                                              /*!< CLK_T::CLKSEL1: UART0SEL Position      */
#define CLK_CLKSEL1_UART0SEL_Msk         (0x3ul << CLK_CLKSEL1_UART0SEL_Pos)               /*!< CLK_T::CLKSEL1: UART0SEL Mask          */

#define CLK_CLKSEL1_UART1SEL_Pos         (26)                                              /*!< CLK_T::CLKSEL1: UART1SEL Position      */
#define CLK_CLKSEL1_UART1SEL_Msk         (0x3ul << CLK_CLKSEL1_UART1SEL_Pos)               /*!< CLK_T::CLKSEL1: UART1SEL Mask          */

#define CLK_CLKSEL1_CLKOSEL_Pos          (28)                                              /*!< CLK_T::CLKSEL1: CLKOSEL Position       */
#define CLK_CLKSEL1_CLKOSEL_Msk          (0x3ul << CLK_CLKSEL1_CLKOSEL_Pos)                /*!< CLK_T::CLKSEL1: CLKOSEL Mask           */

#define CLK_CLKSEL1_WWDTSEL_Pos          (30)                                              /*!< CLK_T::CLKSEL1: WWDTSEL Position       */
#define CLK_CLKSEL1_WWDTSEL_Msk          (0x3ul << CLK_CLKSEL1_WWDTSEL_Pos)                /*!< CLK_T::CLKSEL1: WWDTSEL Mask           */

#define CLK_CLKSEL2_EPWM0SEL_Pos         (0)                                               /*!< CLK_T::CLKSEL2: EPWM0SEL Position      */
#define CLK_CLKSEL2_EPWM0SEL_Msk         (0x1ul << CLK_CLKSEL2_EPWM0SEL_Pos)               /*!< CLK_T::CLKSEL2: EPWM0SEL Mask          */

#define CLK_CLKSEL2_EPWM1SEL_Pos         (1)                                               /*!< CLK_T::CLKSEL2: EPWM1SEL Position      */
#define CLK_CLKSEL2_EPWM1SEL_Msk         (0x1ul << CLK_CLKSEL2_EPWM1SEL_Pos)               /*!< CLK_T::CLKSEL2: EPWM1SEL Mask          */

#define CLK_CLKSEL2_QSPI0SEL_Pos         (2)                                               /*!< CLK_T::CLKSEL2: QSPI0SEL Position      */
#define CLK_CLKSEL2_QSPI0SEL_Msk         (0x3ul << CLK_CLKSEL2_QSPI0SEL_Pos)               /*!< CLK_T::CLKSEL2: QSPI0SEL Mask          */

#define CLK_CLKSEL2_SPI0SEL_Pos          (4)                                               /*!< CLK_T::CLKSEL2: SPI0SEL Position       */
#define CLK_CLKSEL2_SPI0SEL_Msk          (0x3ul << CLK_CLKSEL2_SPI0SEL_Pos)                /*!< CLK_T::CLKSEL2: SPI0SEL Mask           */

#define CLK_CLKSEL2_SPI1SEL_Pos          (6)                                               /*!< CLK_T::CLKSEL2: SPI1SEL Position       */
#define CLK_CLKSEL2_SPI1SEL_Msk          (0x3ul << CLK_CLKSEL2_SPI1SEL_Pos)                /*!< CLK_T::CLKSEL2: SPI1SEL Mask           */

#define CLK_CLKSEL2_BPWM0SEL_Pos         (8)                                               /*!< CLK_T::CLKSEL2: BPWM0SEL Position      */
#define CLK_CLKSEL2_BPWM0SEL_Msk         (0x1ul << CLK_CLKSEL2_BPWM0SEL_Pos)               /*!< CLK_T::CLKSEL2: BPWM0SEL Mask          */

#define CLK_CLKSEL2_BPWM1SEL_Pos         (9)                                               /*!< CLK_T::CLKSEL2: BPWM1SEL Position      */
#define CLK_CLKSEL2_BPWM1SEL_Msk         (0x1ul << CLK_CLKSEL2_BPWM1SEL_Pos)               /*!< CLK_T::CLKSEL2: BPWM1SEL Mask          */

#define CLK_CLKSEL2_SPI2SEL_Pos          (10)                                              /*!< CLK_T::CLKSEL2: SPI2SEL Position       */
#define CLK_CLKSEL2_SPI2SEL_Msk          (0x3ul << CLK_CLKSEL2_SPI2SEL_Pos)                /*!< CLK_T::CLKSEL2: SPI2SEL Mask           */

#define CLK_CLKSEL2_SPI3SEL_Pos          (12)                                              /*!< CLK_T::CLKSEL2: SPI3SEL Position       */
#define CLK_CLKSEL2_SPI3SEL_Msk          (0x3ul << CLK_CLKSEL2_SPI3SEL_Pos)                /*!< CLK_T::CLKSEL2: SPI3SEL Mask           */

#define CLK_CLKSEL3_SC0SEL_Pos           (0)                                               /*!< CLK_T::CLKSEL3: SC0SEL Position        */
#define CLK_CLKSEL3_SC0SEL_Msk           (0x3ul << CLK_CLKSEL3_SC0SEL_Pos)                 /*!< CLK_T::CLKSEL3: SC0SEL Mask            */

#define CLK_CLKSEL3_SC1SEL_Pos           (2)                                               /*!< CLK_T::CLKSEL3: SC1SEL Position        */
#define CLK_CLKSEL3_SC1SEL_Msk           (0x3ul << CLK_CLKSEL3_SC1SEL_Pos)                 /*!< CLK_T::CLKSEL3: SC1SEL Mask            */

#define CLK_CLKSEL3_SC2SEL_Pos           (4)                                               /*!< CLK_T::CLKSEL3: SC2SEL Position        */
#define CLK_CLKSEL3_SC2SEL_Msk           (0x3ul << CLK_CLKSEL3_SC2SEL_Pos)                 /*!< CLK_T::CLKSEL3: SC2SEL Mask            */

#define CLK_CLKSEL3_RTCSEL_Pos           (8)                                               /*!< CLK_T::CLKSEL3: RTCSEL Position        */
#define CLK_CLKSEL3_RTCSEL_Msk           (0x1ul << CLK_CLKSEL3_RTCSEL_Pos)                 /*!< CLK_T::CLKSEL3: RTCSEL Mask            */

#define CLK_CLKSEL3_QSPI1SEL_Pos         (12)                                              /*!< CLK_T::CLKSEL3: QSPI1SEL Position      */
#define CLK_CLKSEL3_QSPI1SEL_Msk         (0x3ul << CLK_CLKSEL3_QSPI1SEL_Pos)               /*!< CLK_T::CLKSEL3: QSPI1SEL Mask          */

#define CLK_CLKSEL3_I2S0SEL_Pos          (16)                                              /*!< CLK_T::CLKSEL3: I2S0SEL Position       */
#define CLK_CLKSEL3_I2S0SEL_Msk          (0x3ul << CLK_CLKSEL3_I2S0SEL_Pos)                /*!< CLK_T::CLKSEL3: I2S0SEL Mask           */

#define CLK_CLKSEL3_UART6SEL_Pos         (20)                                              /*!< CLK_T::CLKSEL3: UART6SEL Position      */
#define CLK_CLKSEL3_UART6SEL_Msk         (0x3ul << CLK_CLKSEL3_UART6SEL_Pos)               /*!< CLK_T::CLKSEL3: UART6SEL Mask          */

#define CLK_CLKSEL3_UART7SEL_Pos         (22)                                              /*!< CLK_T::CLKSEL3: UART7SEL Position      */
#define CLK_CLKSEL3_UART7SEL_Msk         (0x3ul << CLK_CLKSEL3_UART7SEL_Pos)               /*!< CLK_T::CLKSEL3: UART7SEL Mask          */

#define CLK_CLKSEL3_UART2SEL_Pos         (24)                                              /*!< CLK_T::CLKSEL3: UART2SEL Position      */
#define CLK_CLKSEL3_UART2SEL_Msk         (0x3ul << CLK_CLKSEL3_UART2SEL_Pos)               /*!< CLK_T::CLKSEL3: UART2SEL Mask          */

#define CLK_CLKSEL3_UART3SEL_Pos         (26)                                              /*!< CLK_T::CLKSEL3: UART3SEL Position      */
#define CLK_CLKSEL3_UART3SEL_Msk         (0x3ul << CLK_CLKSEL3_UART3SEL_Pos)               /*!< CLK_T::CLKSEL3: UART3SEL Mask          */

#define CLK_CLKSEL3_UART4SEL_Pos         (28)                                              /*!< CLK_T::CLKSEL3: UART4SEL Position      */
#define CLK_CLKSEL3_UART4SEL_Msk         (0x3ul << CLK_CLKSEL3_UART4SEL_Pos)               /*!< CLK_T::CLKSEL3: UART4SEL Mask          */

#define CLK_CLKSEL3_UART5SEL_Pos         (30)                                              /*!< CLK_T::CLKSEL3: UART5SEL Position      */
#define CLK_CLKSEL3_UART5SEL_Msk         (0x3ul << CLK_CLKSEL3_UART5SEL_Pos)               /*!< CLK_T::CLKSEL3: UART5SEL Mask          */

#define CLK_CLKDIV0_HCLKDIV_Pos          (0)                                               /*!< CLK_T::CLKDIV0: HCLKDIV Position       */
#define CLK_CLKDIV0_HCLKDIV_Msk          (0xful << CLK_CLKDIV0_HCLKDIV_Pos)                /*!< CLK_T::CLKDIV0: HCLKDIV Mask           */

#define CLK_CLKDIV0_USBDIV_Pos           (4)                                               /*!< CLK_T::CLKDIV0: USBDIV Position        */
#define CLK_CLKDIV0_USBDIV_Msk           (0xful << CLK_CLKDIV0_USBDIV_Pos)                 /*!< CLK_T::CLKDIV0: USBDIV Mask            */

#define CLK_CLKDIV0_UART0DIV_Pos         (8)                                               /*!< CLK_T::CLKDIV0: UART0DIV Position      */
#define CLK_CLKDIV0_UART0DIV_Msk         (0xful << CLK_CLKDIV0_UART0DIV_Pos)               /*!< CLK_T::CLKDIV0: UART0DIV Mask          */

#define CLK_CLKDIV0_UART1DIV_Pos         (12)                                              /*!< CLK_T::CLKDIV0: UART1DIV Position      */
#define CLK_CLKDIV0_UART1DIV_Msk         (0xful << CLK_CLKDIV0_UART1DIV_Pos)               /*!< CLK_T::CLKDIV0: UART1DIV Mask          */

#define CLK_CLKDIV0_EADCDIV_Pos          (16)                                              /*!< CLK_T::CLKDIV0: EADCDIV Position       */
#define CLK_CLKDIV0_EADCDIV_Msk          (0xfful << CLK_CLKDIV0_EADCDIV_Pos)               /*!< CLK_T::CLKDIV0: EADCDIV Mask           */

#define CLK_CLKDIV0_SDH0DIV_Pos          (24)                                              /*!< CLK_T::CLKDIV0: SDH0DIV Position       */
#define CLK_CLKDIV0_SDH0DIV_Msk          (0xfful << CLK_CLKDIV0_SDH0DIV_Pos)               /*!< CLK_T::CLKDIV0: SDH0DIV Mask           */

#define CLK_CLKDIV1_SC0DIV_Pos           (0)                                               /*!< CLK_T::CLKDIV1: SC0DIV Position        */
#define CLK_CLKDIV1_SC0DIV_Msk           (0xfful << CLK_CLKDIV1_SC0DIV_Pos)                /*!< CLK_T::CLKDIV1: SC0DIV Mask            */

#define CLK_CLKDIV1_SC1DIV_Pos           (8)                                               /*!< CLK_T::CLKDIV1: SC1DIV Position        */
#define CLK_CLKDIV1_SC1DIV_Msk           (0xfful << CLK_CLKDIV1_SC1DIV_Pos)                /*!< CLK_T::CLKDIV1: SC1DIV Mask            */

#define CLK_CLKDIV1_SC2DIV_Pos           (16)                                              /*!< CLK_T::CLKDIV1: SC2DIV Position        */
#define CLK_CLKDIV1_SC2DIV_Msk           (0xfful << CLK_CLKDIV1_SC2DIV_Pos)                /*!< CLK_T::CLKDIV1: SC2DIV Mask            */

#define CLK_CLKDIV2_I2SDIV_Pos           (0)                                               /*!< CLK_T::CLKDIV2: I2SDIV Position        */
#define CLK_CLKDIV2_I2SDIV_Msk           (0xful << CLK_CLKDIV2_I2SDIV_Pos)                 /*!< CLK_T::CLKDIV2: I2SDIV Mask            */

#define CLK_CLKDIV2_EADC1DIV_Pos         (24)                                              /*!< CLK_T::CLKDIV2: EADC1DIV Position      */
#define CLK_CLKDIV2_EADC1DIV_Msk         (0xfful << CLK_CLKDIV2_EADC1DIV_Pos)              /*!< CLK_T::CLKDIV2: EADC1DIV Mask          */

#define CLK_CLKDIV3_CCAPDIV_Pos          (0)                                               /*!< CLK_T::CLKDIV3: CCAPDIV Position       */
#define CLK_CLKDIV3_CCAPDIV_Msk          (0xfful << CLK_CLKDIV3_CCAPDIV_Pos)               /*!< CLK_T::CLKDIV3: CCAPDIV Mask           */

#define CLK_CLKDIV3_VSENSEDIV_Pos        (8)                                               /*!< CLK_T::CLKDIV3: VSENSEDIV Position     */
#define CLK_CLKDIV3_VSENSEDIV_Msk        (0xfful << CLK_CLKDIV3_VSENSEDIV_Pos)             /*!< CLK_T::CLKDIV3: VSENSEDIV Mask         */

#define CLK_CLKDIV3_EMACDIV_Pos          (16)                                              /*!< CLK_T::CLKDIV3: EMACDIV Position       */
#define CLK_CLKDIV3_EMACDIV_Msk          (0xfful << CLK_CLKDIV3_EMACDIV_Pos)               /*!< CLK_T::CLKDIV3: EMACDIV Mask           */

#define CLK_CLKDIV3_SDH1DIV_Pos          (24)                                              /*!< CLK_T::CLKDIV3: SDH1DIV Position       */
#define CLK_CLKDIV3_SDH1DIV_Msk          (0xfful << CLK_CLKDIV3_SDH1DIV_Pos)               /*!< CLK_T::CLKDIV3: SDH1DIV Mask           */

#define CLK_CLKDIV4_UART2DIV_Pos         (0)                                               /*!< CLK_T::CLKDIV4: UART2DIV Position      */
#define CLK_CLKDIV4_UART2DIV_Msk         (0xful << CLK_CLKDIV4_UART2DIV_Pos)               /*!< CLK_T::CLKDIV4: UART2DIV Mask          */

#define CLK_CLKDIV4_UART3DIV_Pos         (4)                                               /*!< CLK_T::CLKDIV4: UART3DIV Position      */
#define CLK_CLKDIV4_UART3DIV_Msk         (0xful << CLK_CLKDIV4_UART3DIV_Pos)               /*!< CLK_T::CLKDIV4: UART3DIV Mask          */

#define CLK_CLKDIV4_UART4DIV_Pos         (8)                                               /*!< CLK_T::CLKDIV4: UART4DIV Position      */
#define CLK_CLKDIV4_UART4DIV_Msk         (0xful << CLK_CLKDIV4_UART4DIV_Pos)               /*!< CLK_T::CLKDIV4: UART4DIV Mask          */

#define CLK_CLKDIV4_UART5DIV_Pos         (12)                                              /*!< CLK_T::CLKDIV4: UART5DIV Position      */
#define CLK_CLKDIV4_UART5DIV_Msk         (0xful << CLK_CLKDIV4_UART5DIV_Pos)               /*!< CLK_T::CLKDIV4: UART5DIV Mask          */

#define CLK_CLKDIV4_UART6DIV_Pos         (16)                                              /*!< CLK_T::CLKDIV4: UART6DIV Position      */
#define CLK_CLKDIV4_UART6DIV_Msk         (0xful << CLK_CLKDIV4_UART6DIV_Pos)               /*!< CLK_T::CLKDIV4: UART6DIV Mask          */

#define CLK_CLKDIV4_UART7DIV_Pos         (20)                                              /*!< CLK_T::CLKDIV4: UART7DIV Position      */
#define CLK_CLKDIV4_UART7DIV_Msk         (0xful << CLK_CLKDIV4_UART7DIV_Pos)               /*!< CLK_T::CLKDIV4: UART7DIV Mask          */

#define CLK_PCLKDIV_APB0DIV_Pos          (0)                                               /*!< CLK_T::PCLKDIV: APB0DIV Position       */
#define CLK_PCLKDIV_APB0DIV_Msk          (0x7ul << CLK_PCLKDIV_APB0DIV_Pos)                /*!< CLK_T::PCLKDIV: APB0DIV Mask           */

#define CLK_PCLKDIV_APB1DIV_Pos          (4)                                               /*!< CLK_T::PCLKDIV: APB1DIV Position       */
#define CLK_PCLKDIV_APB1DIV_Msk          (0x7ul << CLK_PCLKDIV_APB1DIV_Pos)                /*!< CLK_T::PCLKDIV: APB1DIV Mask           */

#define CLK_PLLCTL_FBDIV_Pos             (0)                                               /*!< CLK_T::PLLCTL: FBDIV Position          */
#define CLK_PLLCTL_FBDIV_Msk             (0x1fful << CLK_PLLCTL_FBDIV_Pos)                 /*!< CLK_T::PLLCTL: FBDIV Mask              */

#define CLK_PLLCTL_INDIV_Pos             (9)                                               /*!< CLK_T::PLLCTL: INDIV Position          */
#define CLK_PLLCTL_INDIV_Msk             (0x1ful << CLK_PLLCTL_INDIV_Pos)                  /*!< CLK_T::PLLCTL: INDIV Mask              */

#define CLK_PLLCTL_OUTDIV_Pos            (14)                                              /*!< CLK_T::PLLCTL: OUTDIV Position         */
#define CLK_PLLCTL_OUTDIV_Msk            (0x3ul << CLK_PLLCTL_OUTDIV_Pos)                  /*!< CLK_T::PLLCTL: OUTDIV Mask             */

#define CLK_PLLCTL_PD_Pos                (16)                                              /*!< CLK_T::PLLCTL: PD Position             */
#define CLK_PLLCTL_PD_Msk                (0x1ul << CLK_PLLCTL_PD_Pos)                      /*!< CLK_T::PLLCTL: PD Mask                 */

#define CLK_PLLCTL_BP_Pos                (17)                                              /*!< CLK_T::PLLCTL: BP Position             */
#define CLK_PLLCTL_BP_Msk                (0x1ul << CLK_PLLCTL_BP_Pos)                      /*!< CLK_T::PLLCTL: BP Mask                 */

#define CLK_PLLCTL_OE_Pos                (18)                                              /*!< CLK_T::PLLCTL: OE Position             */
#define CLK_PLLCTL_OE_Msk                (0x1ul << CLK_PLLCTL_OE_Pos)                      /*!< CLK_T::PLLCTL: OE Mask                 */

#define CLK_PLLCTL_PLLSRC_Pos            (19)                                              /*!< CLK_T::PLLCTL: PLLSRC Position         */
#define CLK_PLLCTL_PLLSRC_Msk            (0x1ul << CLK_PLLCTL_PLLSRC_Pos)                  /*!< CLK_T::PLLCTL: PLLSRC Mask             */

#define CLK_PLLCTL_STBSEL_Pos            (23)                                              /*!< CLK_T::PLLCTL: STBSEL Position         */
#define CLK_PLLCTL_STBSEL_Msk            (0x1ul << CLK_PLLCTL_STBSEL_Pos)                  /*!< CLK_T::PLLCTL: STBSEL Mask             */

#define CLK_STATUS_HXTSTB_Pos            (0)                                               /*!< CLK_T::STATUS: HXTSTB Position         */
#define CLK_STATUS_HXTSTB_Msk            (0x1ul << CLK_STATUS_HXTSTB_Pos)                  /*!< CLK_T::STATUS: HXTSTB Mask             */

#define CLK_STATUS_LXTSTB_Pos            (1)                                               /*!< CLK_T::STATUS: LXTSTB Position         */
#define CLK_STATUS_LXTSTB_Msk            (0x1ul << CLK_STATUS_LXTSTB_Pos)                  /*!< CLK_T::STATUS: LXTSTB Mask             */

#define CLK_STATUS_PLLSTB_Pos            (2)                                               /*!< CLK_T::STATUS: PLLSTB Position         */
#define CLK_STATUS_PLLSTB_Msk            (0x1ul << CLK_STATUS_PLLSTB_Pos)                  /*!< CLK_T::STATUS: PLLSTB Mask             */

#define CLK_STATUS_LIRCSTB_Pos           (3)                                               /*!< CLK_T::STATUS: LIRCSTB Position        */
#define CLK_STATUS_LIRCSTB_Msk           (0x1ul << CLK_STATUS_LIRCSTB_Pos)                 /*!< CLK_T::STATUS: LIRCSTB Mask            */

#define CLK_STATUS_HIRCSTB_Pos           (4)                                               /*!< CLK_T::STATUS: HIRCSTB Position        */
#define CLK_STATUS_HIRCSTB_Msk           (0x1ul << CLK_STATUS_HIRCSTB_Pos)                 /*!< CLK_T::STATUS: HIRCSTB Mask            */

#define CLK_STATUS_HIRC48MSTB_Pos        (6)                                               /*!< CLK_T::STATUS: HIRC48MSTB Position     */
#define CLK_STATUS_HIRC48MSTB_Msk        (0x1ul << CLK_STATUS_HIRC48MSTB_Pos)              /*!< CLK_T::STATUS: HIRC48MSTB Mask         */

#define CLK_STATUS_CLKSFAIL_Pos          (7)                                               /*!< CLK_T::STATUS: CLKSFAIL Position       */
#define CLK_STATUS_CLKSFAIL_Msk          (0x1ul << CLK_STATUS_CLKSFAIL_Pos)                /*!< CLK_T::STATUS: CLKSFAIL Mask           */

#define CLK_CLKOCTL_FREQSEL_Pos          (0)                                               /*!< CLK_T::CLKOCTL: FREQSEL Position       */
#define CLK_CLKOCTL_FREQSEL_Msk          (0xful << CLK_CLKOCTL_FREQSEL_Pos)                /*!< CLK_T::CLKOCTL: FREQSEL Mask           */

#define CLK_CLKOCTL_CLKOEN_Pos           (4)                                               /*!< CLK_T::CLKOCTL: CLKOEN Position        */
#define CLK_CLKOCTL_CLKOEN_Msk           (0x1ul << CLK_CLKOCTL_CLKOEN_Pos)                 /*!< CLK_T::CLKOCTL: CLKOEN Mask            */

#define CLK_CLKOCTL_DIV1EN_Pos           (5)                                               /*!< CLK_T::CLKOCTL: DIV1EN Position        */
#define CLK_CLKOCTL_DIV1EN_Msk           (0x1ul << CLK_CLKOCTL_DIV1EN_Pos)                 /*!< CLK_T::CLKOCTL: DIV1EN Mask            */

#define CLK_CLKOCTL_CLK1HZEN_Pos         (6)                                               /*!< CLK_T::CLKOCTL: CLK1HZEN Position      */
#define CLK_CLKOCTL_CLK1HZEN_Msk         (0x1ul << CLK_CLKOCTL_CLK1HZEN_Pos)               /*!< CLK_T::CLKOCTL: CLK1HZEN Mask          */

#define CLK_CLKDCTL_HXTFDEN_Pos          (4)                                               /*!< CLK_T::CLKDCTL: HXTFDEN Position       */
#define CLK_CLKDCTL_HXTFDEN_Msk          (0x1ul << CLK_CLKDCTL_HXTFDEN_Pos)                /*!< CLK_T::CLKDCTL: HXTFDEN Mask           */

#define CLK_CLKDCTL_HXTFIEN_Pos          (5)                                               /*!< CLK_T::CLKDCTL: HXTFIEN Position       */
#define CLK_CLKDCTL_HXTFIEN_Msk          (0x1ul << CLK_CLKDCTL_HXTFIEN_Pos)                /*!< CLK_T::CLKDCTL: HXTFIEN Mask           */

#define CLK_CLKDCTL_LXTFDEN_Pos          (12)                                              /*!< CLK_T::CLKDCTL: LXTFDEN Position       */
#define CLK_CLKDCTL_LXTFDEN_Msk          (0x1ul << CLK_CLKDCTL_LXTFDEN_Pos)                /*!< CLK_T::CLKDCTL: LXTFDEN Mask           */

#define CLK_CLKDCTL_LXTFIEN_Pos          (13)                                              /*!< CLK_T::CLKDCTL: LXTFIEN Position       */
#define CLK_CLKDCTL_LXTFIEN_Msk          (0x1ul << CLK_CLKDCTL_LXTFIEN_Pos)                /*!< CLK_T::CLKDCTL: LXTFIEN Mask           */

#define CLK_CLKDCTL_HXTFQDEN_Pos         (16)                                              /*!< CLK_T::CLKDCTL: HXTFQDEN Position      */
#define CLK_CLKDCTL_HXTFQDEN_Msk         (0x1ul << CLK_CLKDCTL_HXTFQDEN_Pos)               /*!< CLK_T::CLKDCTL: HXTFQDEN Mask          */

#define CLK_CLKDCTL_HXTFQIEN_Pos         (17)                                              /*!< CLK_T::CLKDCTL: HXTFQIEN Position      */
#define CLK_CLKDCTL_HXTFQIEN_Msk         (0x1ul << CLK_CLKDCTL_HXTFQIEN_Pos)               /*!< CLK_T::CLKDCTL: HXTFQIEN Mask          */

#define CLK_CLKDSTS_HXTFIF_Pos           (0)                                               /*!< CLK_T::CLKDSTS: HXTFIF Position        */
#define CLK_CLKDSTS_HXTFIF_Msk           (0x1ul << CLK_CLKDSTS_HXTFIF_Pos)                 /*!< CLK_T::CLKDSTS: HXTFIF Mask            */

#define CLK_CLKDSTS_LXTFIF_Pos           (1)                                               /*!< CLK_T::CLKDSTS: LXTFIF Position        */
#define CLK_CLKDSTS_LXTFIF_Msk           (0x1ul << CLK_CLKDSTS_LXTFIF_Pos)                 /*!< CLK_T::CLKDSTS: LXTFIF Mask            */

#define CLK_CLKDSTS_HXTFQIF_Pos          (8)                                               /*!< CLK_T::CLKDSTS: HXTFQIF Position       */
#define CLK_CLKDSTS_HXTFQIF_Msk          (0x1ul << CLK_CLKDSTS_HXTFQIF_Pos)                /*!< CLK_T::CLKDSTS: HXTFQIF Mask           */

#define CLK_CDUPB_UPERBD_Pos             (0)                                               /*!< CLK_T::CDUPB: UPERBD Position          */
#define CLK_CDUPB_UPERBD_Msk             (0x3fful << CLK_CDUPB_UPERBD_Pos)                 /*!< CLK_T::CDUPB: UPERBD Mask              */

#define CLK_CDLOWB_LOWERBD_Pos           (0)                                               /*!< CLK_T::CDLOWB: LOWERBD Position        */
#define CLK_CDLOWB_LOWERBD_Msk           (0x3fful << CLK_CDLOWB_LOWERBD_Pos)               /*!< CLK_T::CDLOWB: LOWERBD Mask            */

#define CLK_PMUCTL_PDMSEL_Pos            (0)                                               /*!< CLK_T::PMUCTL: PDMSEL Position         */
#define CLK_PMUCTL_PDMSEL_Msk            (0x7ul << CLK_PMUCTL_PDMSEL_Pos)                  /*!< CLK_T::PMUCTL: PDMSEL Mask             */

#define CLK_PMUCTL_DPDHOLDEN_Pos         (3)                                               /*!< CLK_T::PMUCTL: DPDHOLDEN Position      */
#define CLK_PMUCTL_DPDHOLDEN_Msk         (0x1ul << CLK_PMUCTL_DPDHOLDEN_Pos)               /*!< CLK_T::PMUCTL: DPDHOLDEN Mask          */

#define CLK_PMUCTL_SRETSEL_Pos           (4)                                               /*!< CLK_T::PMUCTL: SRETSEL Position        */
#define CLK_PMUCTL_SRETSEL_Msk           (0x7ul << CLK_PMUCTL_SRETSEL_Pos)                 /*!< CLK_T::PMUCTL: SRETSEL Mask            */

#define CLK_PMUCTL_WKTMREN_Pos           (8)                                               /*!< CLK_T::PMUCTL: WKTMREN Position        */
#define CLK_PMUCTL_WKTMREN_Msk           (0x1ul << CLK_PMUCTL_WKTMREN_Pos)                 /*!< CLK_T::PMUCTL: WKTMREN Mask            */

#define CLK_PMUCTL_WKTMRIS_Pos           (9)                                               /*!< CLK_T::PMUCTL: WKTMRIS Position        */
#define CLK_PMUCTL_WKTMRIS_Msk           (0xful << CLK_PMUCTL_WKTMRIS_Pos)                 /*!< CLK_T::PMUCTL: WKTMRIS Mask            */

#define CLK_PMUCTL_WKPINEN_Pos           (16)                                              /*!< CLK_T::PMUCTL: WKPINEN Position        */
#define CLK_PMUCTL_WKPINEN_Msk           (0x3ul << CLK_PMUCTL_WKPINEN_Pos)                 /*!< CLK_T::PMUCTL: WKPINEN Mask            */

#define CLK_PMUCTL_ACMPSPWK_Pos          (18)                                              /*!< CLK_T::PMUCTL: ACMPSPWK Position       */
#define CLK_PMUCTL_ACMPSPWK_Msk          (0x1ul << CLK_PMUCTL_ACMPSPWK_Pos)                /*!< CLK_T::PMUCTL: ACMPSPWK Mask           */

#define CLK_PMUCTL_RTCWKEN_Pos           (23)                                              /*!< CLK_T::PMUCTL: RTCWKEN Position        */
#define CLK_PMUCTL_RTCWKEN_Msk           (0x1ul << CLK_PMUCTL_RTCWKEN_Pos)                 /*!< CLK_T::PMUCTL: RTCWKEN Mask            */

#define CLK_PMUCTL_WKPINEN1_Pos          (24)                                              /*!< CLK_T::PMUCTL: WKPINEN1 Position       */
#define CLK_PMUCTL_WKPINEN1_Msk          (0x3ul << CLK_PMUCTL_WKPINEN1_Pos)                /*!< CLK_T::PMUCTL: WKPINEN1 Mask           */

#define CLK_PMUCTL_WKPINEN2_Pos          (26)                                              /*!< CLK_T::PMUCTL: WKPINEN2 Position       */
#define CLK_PMUCTL_WKPINEN2_Msk          (0x3ul << CLK_PMUCTL_WKPINEN2_Pos)                /*!< CLK_T::PMUCTL: WKPINEN2 Mask           */

#define CLK_PMUCTL_WKPINEN3_Pos          (28)                                              /*!< CLK_T::PMUCTL: WKPINEN3 Position       */
#define CLK_PMUCTL_WKPINEN3_Msk          (0x3ul << CLK_PMUCTL_WKPINEN3_Pos)                /*!< CLK_T::PMUCTL: WKPINEN3 Mask           */

#define CLK_PMUCTL_WKPINEN4_Pos          (30)                                              /*!< CLK_T::PMUCTL: WKPINEN4 Position       */
#define CLK_PMUCTL_WKPINEN4_Msk          (0x3ul << CLK_PMUCTL_WKPINEN4_Pos)                /*!< CLK_T::PMUCTL: WKPINEN4 Mask           */

#define CLK_PMUSTS_PINWK_Pos             (0)                                               /*!< CLK_T::PMUSTS: PINWK Position          */
#define CLK_PMUSTS_PINWK_Msk             (0x1ul << CLK_PMUSTS_PINWK_Pos)                   /*!< CLK_T::PMUSTS: PINWK Mask              */

#define CLK_PMUSTS_TMRWK_Pos             (1)                                               /*!< CLK_T::PMUSTS: TMRWK Position          */
#define CLK_PMUSTS_TMRWK_Msk             (0x1ul << CLK_PMUSTS_TMRWK_Pos)                   /*!< CLK_T::PMUSTS: TMRWK Mask              */

#define CLK_PMUSTS_RTCWK_Pos             (2)                                               /*!< CLK_T::PMUSTS: RTCWK Position          */
#define CLK_PMUSTS_RTCWK_Msk             (0x1ul << CLK_PMUSTS_RTCWK_Pos)                   /*!< CLK_T::PMUSTS: RTCWK Mask              */

#define CLK_PMUSTS_PINWK1_Pos            (3)                                               /*!< CLK_T::PMUSTS: PINWK1 Position         */
#define CLK_PMUSTS_PINWK1_Msk            (0x1ul << CLK_PMUSTS_PINWK1_Pos)                  /*!< CLK_T::PMUSTS: PINWK1 Mask             */

#define CLK_PMUSTS_PINWK2_Pos            (4)                                               /*!< CLK_T::PMUSTS: PINWK2 Position         */
#define CLK_PMUSTS_PINWK2_Msk            (0x1ul << CLK_PMUSTS_PINWK2_Pos)                  /*!< CLK_T::PMUSTS: PINWK2 Mask             */

#define CLK_PMUSTS_PINWK3_Pos            (5)                                               /*!< CLK_T::PMUSTS: PINWK3 Position         */
#define CLK_PMUSTS_PINWK3_Msk            (0x1ul << CLK_PMUSTS_PINWK3_Pos)                  /*!< CLK_T::PMUSTS: PINWK3 Mask             */

#define CLK_PMUSTS_PINWK4_Pos            (6)                                               /*!< CLK_T::PMUSTS: PINWK4 Position         */
#define CLK_PMUSTS_PINWK4_Msk            (0x1ul << CLK_PMUSTS_PINWK4_Pos)                  /*!< CLK_T::PMUSTS: PINWK4 Mask             */

#define CLK_PMUSTS_GPAWK_Pos             (8)                                               /*!< CLK_T::PMUSTS: GPAWK Position          */
#define CLK_PMUSTS_GPAWK_Msk             (0x1ul << CLK_PMUSTS_GPAWK_Pos)                   /*!< CLK_T::PMUSTS: GPAWK Mask              */

#define CLK_PMUSTS_GPBWK_Pos             (9)                                               /*!< CLK_T::PMUSTS: GPBWK Position          */
#define CLK_PMUSTS_GPBWK_Msk             (0x1ul << CLK_PMUSTS_GPBWK_Pos)                   /*!< CLK_T::PMUSTS: GPBWK Mask              */

#define CLK_PMUSTS_GPCWK_Pos             (10)                                              /*!< CLK_T::PMUSTS: GPCWK Position          */
#define CLK_PMUSTS_GPCWK_Msk             (0x1ul << CLK_PMUSTS_GPCWK_Pos)                   /*!< CLK_T::PMUSTS: GPCWK Mask              */

#define CLK_PMUSTS_GPDWK_Pos             (11)                                              /*!< CLK_T::PMUSTS: GPDWK Position          */
#define CLK_PMUSTS_GPDWK_Msk             (0x1ul << CLK_PMUSTS_GPDWK_Pos)                   /*!< CLK_T::PMUSTS: GPDWK Mask              */

#define CLK_PMUSTS_LVRWK_Pos             (12)                                              /*!< CLK_T::PMUSTS: LVRWK Position          */
#define CLK_PMUSTS_LVRWK_Msk             (0x1ul << CLK_PMUSTS_LVRWK_Pos)                   /*!< CLK_T::PMUSTS: LVRWK Mask              */

#define CLK_PMUSTS_BODWK_Pos             (13)                                              /*!< CLK_T::PMUSTS: BODWK Position          */
#define CLK_PMUSTS_BODWK_Msk             (0x1ul << CLK_PMUSTS_BODWK_Pos)                   /*!< CLK_T::PMUSTS: BODWK Mask              */

#define CLK_PMUSTS_ACMPWK_Pos            (14)                                              /*!< CLK_T::PMUSTS: ACMPWK Position         */
#define CLK_PMUSTS_ACMPWK_Msk            (0x1ul << CLK_PMUSTS_ACMPWK_Pos)                  /*!< CLK_T::PMUSTS: ACMPWK Mask             */

#define CLK_PMUSTS_CLRWK_Pos             (31)                                              /*!< CLK_T::PMUSTS: CLRWK Position          */
#define CLK_PMUSTS_CLRWK_Msk             (0x1ul << CLK_PMUSTS_CLRWK_Pos)                   /*!< CLK_T::PMUSTS: CLRWK Mask              */

#define CLK_LDOCTL_PDBIASEN_Pos          (18)                                              /*!< CLK_T::LDOCTL: PDBIASEN Position       */
#define CLK_LDOCTL_PDBIASEN_Msk          (0x1ul << CLK_LDOCTL_PDBIASEN_Pos)                /*!< CLK_T::LDOCTL: PDBIASEN Mask           */

#define CLK_SWKDBCTL_SWKDBCLKSEL_Pos     (0)                                               /*!< CLK_T::SWKDBCTL: SWKDBCLKSEL Position  */
#define CLK_SWKDBCTL_SWKDBCLKSEL_Msk     (0xful << CLK_SWKDBCTL_SWKDBCLKSEL_Pos)           /*!< CLK_T::SWKDBCTL: SWKDBCLKSEL Mask      */

#define CLK_PASWKCTL_WKEN_Pos            (0)                                               /*!< CLK_T::PASWKCTL: WKEN Position         */
#define CLK_PASWKCTL_WKEN_Msk            (0x1ul << CLK_PASWKCTL_WKEN_Pos)                  /*!< CLK_T::PASWKCTL: WKEN Mask             */

#define CLK_PASWKCTL_PRWKEN_Pos          (1)                                               /*!< CLK_T::PASWKCTL: PRWKEN Position       */
#define CLK_PASWKCTL_PRWKEN_Msk          (0x1ul << CLK_PASWKCTL_PRWKEN_Pos)                /*!< CLK_T::PASWKCTL: PRWKEN Mask           */

#define CLK_PASWKCTL_PFWKEN_Pos          (2)                                               /*!< CLK_T::PASWKCTL: PFWKEN Position       */
#define CLK_PASWKCTL_PFWKEN_Msk          (0x1ul << CLK_PASWKCTL_PFWKEN_Pos)                /*!< CLK_T::PASWKCTL: PFWKEN Mask           */

#define CLK_PASWKCTL_WKPSEL_Pos          (4)                                               /*!< CLK_T::PASWKCTL: WKPSEL Position       */
#define CLK_PASWKCTL_WKPSEL_Msk          (0xful << CLK_PASWKCTL_WKPSEL_Pos)                /*!< CLK_T::PASWKCTL: WKPSEL Mask           */

#define CLK_PASWKCTL_DBEN_Pos            (8)                                               /*!< CLK_T::PASWKCTL: DBEN Position         */
#define CLK_PASWKCTL_DBEN_Msk            (0x1ul << CLK_PASWKCTL_DBEN_Pos)                  /*!< CLK_T::PASWKCTL: DBEN Mask             */

#define CLK_PBSWKCTL_WKEN_Pos            (0)                                               /*!< CLK_T::PBSWKCTL: WKEN Position         */
#define CLK_PBSWKCTL_WKEN_Msk            (0x1ul << CLK_PBSWKCTL_WKEN_Pos)                  /*!< CLK_T::PBSWKCTL: WKEN Mask             */

#define CLK_PBSWKCTL_PRWKEN_Pos          (1)                                               /*!< CLK_T::PBSWKCTL: PRWKEN Position       */
#define CLK_PBSWKCTL_PRWKEN_Msk          (0x1ul << CLK_PBSWKCTL_PRWKEN_Pos)                /*!< CLK_T::PBSWKCTL: PRWKEN Mask           */

#define CLK_PBSWKCTL_PFWKEN_Pos          (2)                                               /*!< CLK_T::PBSWKCTL: PFWKEN Position       */
#define CLK_PBSWKCTL_PFWKEN_Msk          (0x1ul << CLK_PBSWKCTL_PFWKEN_Pos)                /*!< CLK_T::PBSWKCTL: PFWKEN Mask           */

#define CLK_PBSWKCTL_WKPSEL_Pos          (4)                                               /*!< CLK_T::PBSWKCTL: WKPSEL Position       */
#define CLK_PBSWKCTL_WKPSEL_Msk          (0xful << CLK_PBSWKCTL_WKPSEL_Pos)                /*!< CLK_T::PBSWKCTL: WKPSEL Mask           */

#define CLK_PBSWKCTL_DBEN_Pos            (8)                                               /*!< CLK_T::PBSWKCTL: DBEN Position         */
#define CLK_PBSWKCTL_DBEN_Msk            (0x1ul << CLK_PBSWKCTL_DBEN_Pos)                  /*!< CLK_T::PBSWKCTL: DBEN Mask             */

#define CLK_PCSWKCTL_WKEN_Pos            (0)                                               /*!< CLK_T::PCSWKCTL: WKEN Position         */
#define CLK_PCSWKCTL_WKEN_Msk            (0x1ul << CLK_PCSWKCTL_WKEN_Pos)                  /*!< CLK_T::PCSWKCTL: WKEN Mask             */

#define CLK_PCSWKCTL_PRWKEN_Pos          (1)                                               /*!< CLK_T::PCSWKCTL: PRWKEN Position       */
#define CLK_PCSWKCTL_PRWKEN_Msk          (0x1ul << CLK_PCSWKCTL_PRWKEN_Pos)                /*!< CLK_T::PCSWKCTL: PRWKEN Mask           */

#define CLK_PCSWKCTL_PFWKEN_Pos          (2)                                               /*!< CLK_T::PCSWKCTL: PFWKEN Position       */
#define CLK_PCSWKCTL_PFWKEN_Msk          (0x1ul << CLK_PCSWKCTL_PFWKEN_Pos)                /*!< CLK_T::PCSWKCTL: PFWKEN Mask           */

#define CLK_PCSWKCTL_WKPSEL_Pos          (4)                                               /*!< CLK_T::PCSWKCTL: WKPSEL Position       */
#define CLK_PCSWKCTL_WKPSEL_Msk          (0xful << CLK_PCSWKCTL_WKPSEL_Pos)                /*!< CLK_T::PCSWKCTL: WKPSEL Mask           */

#define CLK_PCSWKCTL_DBEN_Pos            (8)                                               /*!< CLK_T::PCSWKCTL: DBEN Position         */
#define CLK_PCSWKCTL_DBEN_Msk            (0x1ul << CLK_PCSWKCTL_DBEN_Pos)                  /*!< CLK_T::PCSWKCTL: DBEN Mask             */

#define CLK_PDSWKCTL_WKEN_Pos            (0)                                               /*!< CLK_T::PDSWKCTL: WKEN Position         */
#define CLK_PDSWKCTL_WKEN_Msk            (0x1ul << CLK_PDSWKCTL_WKEN_Pos)                  /*!< CLK_T::PDSWKCTL: WKEN Mask             */

#define CLK_PDSWKCTL_PRWKEN_Pos          (1)                                               /*!< CLK_T::PDSWKCTL: PRWKEN Position       */
#define CLK_PDSWKCTL_PRWKEN_Msk          (0x1ul << CLK_PDSWKCTL_PRWKEN_Pos)                /*!< CLK_T::PDSWKCTL: PRWKEN Mask           */

#define CLK_PDSWKCTL_PFWKEN_Pos          (2)                                               /*!< CLK_T::PDSWKCTL: PFWKEN Position       */
#define CLK_PDSWKCTL_PFWKEN_Msk          (0x1ul << CLK_PDSWKCTL_PFWKEN_Pos)                /*!< CLK_T::PDSWKCTL: PFWKEN Mask           */

#define CLK_PDSWKCTL_WKPSEL_Pos          (4)                                               /*!< CLK_T::PDSWKCTL: WKPSEL Position       */
#define CLK_PDSWKCTL_WKPSEL_Msk          (0xful << CLK_PDSWKCTL_WKPSEL_Pos)                /*!< CLK_T::PDSWKCTL: WKPSEL Mask           */

#define CLK_PDSWKCTL_DBEN_Pos            (8)                                               /*!< CLK_T::PDSWKCTL: DBEN Position         */
#define CLK_PDSWKCTL_DBEN_Msk            (0x1ul << CLK_PDSWKCTL_DBEN_Pos)                  /*!< CLK_T::PDSWKCTL: DBEN Mask             */

#define CLK_IOPDCTL_IOHR_Pos             (0)                                               /*!< CLK_T::IOPDCTL: IOHR Position          */
#define CLK_IOPDCTL_IOHR_Msk             (0x1ul << CLK_IOPDCTL_IOHR_Pos)                   /*!< CLK_T::IOPDCTL: IOHR Mask              */

/**@}*/ /* CLK_CONST */
/**@}*/ /* end of CLK register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __CLK_REG_H__ */
