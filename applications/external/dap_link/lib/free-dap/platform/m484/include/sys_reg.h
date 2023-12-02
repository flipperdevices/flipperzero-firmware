/**************************************************************************//**
 * @file     sys_reg.h
 * @version  V1.00
 * @brief    SYS register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SYS_REG_H__
#define __SYS_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup SYS System Manger Controller(SYS)
    Memory Mapped Structure for SYS Controller
@{ */

typedef struct
{


    /**
     * @var SYS_T::PDID
     * Offset: 0x00  Part Device Identification Number Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |PDID      |Part Device Identification Number (Read Only)
     * |        |          |This register reflects device part number code
     * |        |          |Software can read this register to identify which device is used.
     * @var SYS_T::RSTSTS
     * Offset: 0x04  System Reset Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PORF      |POR Reset Flag
     * |        |          |The POR reset flag is set by the "Reset Signal" from the Power-on Reset (POR) Controller or bit CHIPRST (SYS_IPRST0[0]) to indicate the previous reset source.
     * |        |          |0 = No reset from POR or CHIPRST.
     * |        |          |1 = Power-on Reset (POR) or CHIPRST had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[1]     |PINRF     |NRESET Pin Reset Flag
     * |        |          |The nRESET pin reset flag is set by the "Reset Signal" from the nRESET Pin to indicate the previous reset source.
     * |        |          |0 = No reset from nRESET pin.
     * |        |          |1 = Pin nRESET had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[2]     |WDTRF     |WDT Reset Flag
     * |        |          |The WDT reset flag is set by the "Reset Signal" from the Watchdog Timer or Window Watchdog Timer to indicate the previous reset source.
     * |        |          |0 = No reset from watchdog timer or window watchdog timer.
     * |        |          |1 = The watchdog timer or window watchdog timer had issued the reset signal to reset the system.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: Watchdog Timer register RSTF(WDT_CTL[2]) bit is set if the system has been reset by WDT time-out reset
     * |        |          |Window Watchdog Timer register WWDTRF(WWDT_STATUS[1]) bit is set if the system has been reset by WWDT time-out reset.
     * |[3]     |LVRF      |LVR Reset Flag
     * |        |          |The LVR reset flag is set by the "Reset Signal" from the Low Voltage Reset Controller to indicate the previous reset source.
     * |        |          |0 = No reset from LVR.
     * |        |          |1 = LVR controller had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[4]     |BODRF     |BOD Reset Flag
     * |        |          |The BOD reset flag is set by the "Reset Signal" from the Brown-Out Detector to indicate the previous reset source.
     * |        |          |0 = No reset from BOD.
     * |        |          |1 = The BOD had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |SYSRF     |System Reset Flag
     * |        |          |The system reset flag is set by the "Reset Signal" from the Cortex-M4 Core to indicate the previous reset source.
     * |        |          |0 = No reset from Cortex-M4.
     * |        |          |1 = The Cortex-M4 had issued the reset signal to reset the system by writing 1 to the bit SYSRESETREQ(AIRCR[2], Application Interrupt and Reset Control Register, address = 0xE000ED0C) in system control registers of Cortex-M4 core.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[7]     |CPURF     |CPU Reset Flag
     * |        |          |The CPU reset flag is set by hardware if software writes CPURST (SYS_IPRST0[1]) 1 to reset Cortex-M4 Core and Flash Memory Controller (FMC).
     * |        |          |0 = No reset from CPU.
     * |        |          |1 = The Cortex-M4 Core and FMC are reset by software setting CPURST to 1.
     * |        |          |Note: Write to clear this bit to 0.
     * |[8]     |CPULKRF   |CPU Lock-up Reset Flag
     * |        |          |0 = No reset from CPU lock-up happened.
     * |        |          |1 = The Cortex-M4 lock-up happened and chip is reset.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |        |          |Note2: When CPU lock-up happened under ICE is connected, This flag will set to 1 but chip will not reset.
     * @var SYS_T::IPRST0
     * Offset: 0x08  Peripheral  Reset Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CHIPRST   |Chip One-shot Reset (Write Protect)
     * |        |          |Setting this bit will reset the whole chip, including Processor core and all peripherals, and this bit will automatically return to 0 after the 2 clock cycles.
     * |        |          |The CHIPRST is same as the POR reset, all the chip controllers is reset and the chip setting from flash are also reload.
     * |        |          |About the difference between CHIPRST and SYSRESETREQ(AIRCR[2]), please refer to section 6.2.2
     * |        |          |0 = Chip normal operation.
     * |        |          |1 = Chip one-shot reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |CPURST    |Processor Core One-shot Reset (Write Protect)
     * |        |          |Setting this bit will only reset the processor core and Flash Memory Controller(FMC), and this bit will automatically return to 0 after the 2 clock cycles.
     * |        |          |0 = Processor core normal operation.
     * |        |          |1 = Processor core one-shot reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |PDMARST   |PDMA Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the PDMA
     * |        |          |User needs to set this bit to 0 to release from reset state.
     * |        |          |0 = PDMA controller normal operation.
     * |        |          |1 = PDMA controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |EBIRST    |EBI Controller Reset (Write Protect)
     * |        |          |Set this bit to 1 will generate a reset signal to the EBI
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = EBI controller normal operation.
     * |        |          |1 = EBI controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[5]     |EMACRST   |EMAC Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the EMAC controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = EMAC controller normal operation.
     * |        |          |1 = EMAC controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |SDH0RST   |SDHOST0 Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the SDHOST0 controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = SDHOST0 controller normal operation.
     * |        |          |1 = SDHOST0 controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7]     |CRCRST    |CRC Calculation Controller Reset (Write Protect)
     * |        |          |Set this bit to 1 will generate a reset signal to the CRC calculation controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = CRC calculation controller normal operation.
     * |        |          |1 = CRC calculation controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |CCAPRST   |CCAP Controller Reset (Write Protect)
     * |        |          |Set this bit to 1 will generate a reset signal to the CCAP controller.
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = CCAP controller normal operation.
     * |        |          |1 = CCAP controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[10]    |HSUSBDRST |HSUSBD Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the HSUSBD controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = HSUSBD controller normal operation.
     * |        |          |1 = HSUSBD controller reset.
     * |[12]    |CRPTRST   |CRYPTO Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the CRYPTO controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = CRYPTO controller normal operation.
     * |        |          |1 = CRYPTO controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[14]    |SPIMRST   |SPIM Controller Reset
     * |        |          |Setting this bit to 1 will generate a reset signal to the SPIM controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = SPIM controller normal operation.
     * |        |          |1 = SPIM controller reset.
     * |[16]    |USBHRST   |USBH Controller Reset (Write Protect)
     * |        |          |Set this bit to 1 will generate a reset signal to the USBH controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = USBH controller normal operation.
     * |        |          |1 = USBH controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[17]    |SDH1RST   |SDHOST1 Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the SDHOST1 controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = SDHOST1 controller normal operation.
     * |        |          |1 = SDHOST1 controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::IPRST1
     * Offset: 0x0C  Peripheral Reset Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |GPIORST   |GPIO Controller Reset
     * |        |          |0 = GPIO controller normal operation.
     * |        |          |1 = GPIO controller reset.
     * |[2]     |TMR0RST   |Timer0 Controller Reset
     * |        |          |0 = Timer0 controller normal operation.
     * |        |          |1 = Timer0 controller reset.
     * |[3]     |TMR1RST   |Timer1 Controller Reset
     * |        |          |0 = Timer1 controller normal operation.
     * |        |          |1 = Timer1 controller reset.
     * |[4]     |TMR2RST   |Timer2 Controller Reset
     * |        |          |0 = Timer2 controller normal operation.
     * |        |          |1 = Timer2 controller reset.
     * |[5]     |TMR3RST   |Timer3 Controller Reset
     * |        |          |0 = Timer3 controller normal operation.
     * |        |          |1 = Timer3 controller reset.
     * |[7]     |ACMP01RST |Analog Comparator 0/1 Controller Reset
     * |        |          |0 = Analog Comparator 0/1 controller normal operation.
     * |        |          |1 = Analog Comparator 0/1 controller reset.
     * |[8]     |I2C0RST   |I2C0 Controller Reset
     * |        |          |0 = I2C0 controller normal operation.
     * |        |          |1 = I2C0 controller reset.
     * |[9]     |I2C1RST   |I2C1 Controller Reset
     * |        |          |0 = I2C1 controller normal operation.
     * |        |          |1 = I2C1 controller reset.
     * |[10]    |I2C2RST   |I2C2 Controller Reset
     * |        |          |0 = I2C2 controller normal operation.
     * |        |          |1 = I2C2 controller reset.
     * |[12]    |QSPI0RST   |QSPI0 Controller Reset
     * |        |          |0 = QSPI0 controller normal operation.
     * |        |          |1 = QSPI0 controller reset.
     * |[13]    |SPI0RST   |SPI0 Controller Reset
     * |        |          |0 = SPI0 controller normal operation.
     * |        |          |1 = SPI0 controller reset.
     * |[14]    |SPI1RST   |SPI1 Controller Reset
     * |        |          |0 = SPI1 controller normal operation.
     * |        |          |1 = SPI1 controller reset.
     * |[15]    |SPI2RST   |SPI2 Controller Reset
     * |        |          |0 = SPI2 controller normal operation.
     * |        |          |1 = SPI2 controller reset.
     * |[16]    |UART0RST  |UART0 Controller Reset
     * |        |          |0 = UART0 controller normal operation.
     * |        |          |1 = UART0 controller reset.
     * |[17]    |UART1RST  |UART1 Controller Reset
     * |        |          |0 = UART1 controller normal operation.
     * |        |          |1 = UART1 controller reset.
     * |[18]    |UART2RST  |UART2 Controller Reset
     * |        |          |0 = UART2 controller normal operation.
     * |        |          |1 = UART2 controller reset.
     * |[19]    |UART3RST  |UART3 Controller Reset
     * |        |          |0 = UART3 controller normal operation.
     * |        |          |1 = UART3 controller reset.
     * |[20]    |UART4RST  |UART4 Controller Reset
     * |        |          |0 = UART4 controller normal operation.
     * |        |          |1 = UART4 controller reset.
     * |[21]    |UART5RST  |UART5 Controller Reset
     * |        |          |0 = UART5 controller normal operation.
     * |        |          |1 = UART5 controller reset.
     * |[24]    |CAN0RST   |CAN0 Controller Reset
     * |        |          |0 = CAN0 controller normal operation.
     * |        |          |1 = CAN0 controller reset.
     * |[25]    |CAN1RST   |CAN1 Controller Reset
     * |        |          |0 = CAN1 controller normal operation.
     * |        |          |1 = CAN1 controller reset.
     * |[27]    |USBDRST   |USBD Controller Reset
     * |        |          |0 = USBD controller normal operation.
     * |        |          |1 = USBD controller reset.
     * |[28]    |EADCRST   |EADC Controller Reset
     * |        |          |0 = EADC controller normal operation.
     * |        |          |1 = EADC controller reset.
     * |[29]    |I2S0RST   |I2S0 Controller Reset
     * |        |          |0 = I2S0 controller normal operation.
     * |        |          |1 = I2S0 controller reset.
     * @var SYS_T::IPRST2
     * Offset: 0x10  Peripheral Reset Control Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SC0RST    |SC0 Controller Reset
     * |        |          |0 = SC0 controller normal operation.
     * |        |          |1 = SC0 controller reset.
     * |[1]     |SC1RST    |SC1 Controller Reset
     * |        |          |0 = SC1 controller normal operation.
     * |        |          |1 = SC1 controller reset.
     * |[2]     |SC2RST    |SC2 Controller Reset
     * |        |          |0 = SC2 controller normal operation.
     * |        |          |1 = SC2 controller reset.
     * |[6]     |SPI3RST   |SPI3 Controller Reset
     * |        |          |0 = SPI3 controller normal operation.
     * |        |          |1 = SPI3 controller reset.
     * |[8]     |USCI0RST  |USCI0 Controller Reset
     * |        |          |0 = USCI0 controller normal operation.
     * |        |          |1 = USCI0 controller reset.
     * |[9]     |USCI1RST  |USCI1 Controller Reset
     * |        |          |0 = USCI1 controller normal operation.
     * |        |          |1 = USCI1 controller reset.
     * |[12]    |DACRST    |DAC Controller Reset
     * |        |          |0 = DAC controller normal operation.
     * |        |          |1 = DAC controller reset.
     * |[16]    |EPWM0RST   |EPWM0 Controller Reset
     * |        |          |0 = EPWM0 controller normal operation.
     * |        |          |1 = EPWM0 controller reset.
     * |[17]    |EPWM1RST   |EPWM1 Controller Reset
     * |        |          |0 = EPWM1 controller normal operation.
     * |        |          |1 = EPWM1 controller reset.
     * |[18]    |BPWM0RST  |BPWM0 Controller Reset
     * |        |          |0 = BPWM0 controller normal operation.
     * |        |          |1 = BPWM0 controller reset.
     * |[19]    |BPWM1RST  |BPWM1 Controller Reset
     * |        |          |0 = BPWM1 controller normal operation.
     * |        |          |1 = BPWM1 controller reset.
     * |[22]    |QEI0RST   |QEI0 Controller Reset
     * |        |          |0 = QEI0 controller normal operation.
     * |        |          |1 = QEI0 controller reset.
     * |[23]    |QEI1RST   |QEI1 Controller Reset
     * |        |          |0 = QEI1 controller normal operation.
     * |        |          |1 = QEI1 controller reset.
     * |[26]    |ECAP0RST  |ECAP0 Controller Reset
     * |        |          |0 = ECAP0 controller normal operation.
     * |        |          |1 = ECAP0 controller reset.
     * |[27]    |ECAP1RST  |ECAP1 Controller Reset
     * |        |          |0 = ECAP1 controller normal operation.
     * |        |          |1 = ECAP1 controller reset.
     * |[28]    |CAN2RST   |CAN2 Controller Reset
     * |        |          |0 = CAN2 controller normal operation.
     * |        |          |1 = CAN2 controller reset.
     * |[30]    |OPARST    |OP Amplifier (OPA) Controller Reset
     * |        |          |0 = OPA controller normal operation.
     * |        |          |1 = OPA controller reset.
     * @var SYS_T::BODCTL
     * Offset: 0x18  Brown-Out Detector Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BODEN     |Brown-out Detector Enable Bit (Write Protect)
     * |        |          |The default value is set by flash controller user configuration register CBODEN(CONFIG0 [19]).
     * |        |          |0 = Brown-out Detector function Disabled.
     * |        |          |1 = Brown-out Detector function Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |BODRSTEN  |Brown-out Reset Enable Bit (Write Protect)
     * |        |          |The default value is set by flash controller user configuration register CBORST(CONFIG0[20]) bit .
     * |        |          |0 = Brown-out INTERRUPT function Enabled.
     * |        |          |1 = Brown-out RESET function Enabled.
     * |        |          |Note1:
     * |        |          |While the Brown-out Detector function is enabled (BODEN high) and BOD reset function is enabled (BODRSTEN high), BOD will assert a signal to reset chip when the detected voltage is lower than the threshold (BODOUT high).
     * |        |          |While the BOD function is enabled (BODEN high) and BOD interrupt function is enabled (BODRSTEN low), BOD will assert an interrupt if BODOUT is high
     * |        |          |BOD interrupt will keep till to the BODEN set to 0
     * |        |          |BOD interrupt can be blocked by disabling the NVIC BOD interrupt or disabling BOD function (set BODEN low).
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[4]     |BODIF     |Brown-out Detector Interrupt Flag
     * |        |          |0 = Brown-out Detector does not detect any voltage draft at VDD down through or up through the voltage of BODVL setting.
     * |        |          |1 = When Brown-out Detector detects the VDD is dropped down through the voltage of BODVL setting or the VDD is raised up through the voltage of BODVL setting, this bit is set to 1 and the brown-out interrupt is requested if brown-out interrupt is enabled.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |BODLPM    |Brown-out Detector Low Power Mode (Write Protect)
     * |        |          |0 = BOD operate in normal mode (default).
     * |        |          |1 = BOD Low Power mode Enabled.
     * |        |          |Note1: The BOD consumes about 100uA in normal mode, the low power mode can reduce the current to about 1/10 but slow the BOD response.
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |BODOUT    |Brown-out Detector Output Status
     * |        |          |0 = Brown-out Detector output status is 0.
     * |        |          |It means the detected voltage is higher than BODVL setting or BODEN is 0.
     * |        |          |1 = Brown-out Detector output status is 1.
     * |        |          |It means the detected voltage is lower than BODVL setting
     * |        |          |If the BODEN is 0, BOD function disabled , this bit always responds 0000.
     * |[7]     |LVREN     |Low Voltage Reset Enable Bit (Write Protect)
     * |        |          |The LVR function resets the chip when the input power voltage is lower than LVR circuit setting
     * |        |          |LVR function is enabled by default.
     * |        |          |0 = Low Voltage Reset function Disabled.
     * |        |          |1 = Low Voltage Reset function Enabled.
     * |        |          |Note1: After enabling the bit, the LVR function will be active with 100us delay for LVR output stable (default).
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[10:8]  |BODDGSEL  |Brown-out Detector Output De-glitch Time Select (Write Protect)
     * |        |          |000 = BOD output is sampled by RC10K clock.
     * |        |          |001 = 4 system clock (HCLK).
     * |        |          |010 = 8 system clock (HCLK).
     * |        |          |011 = 16 system clock (HCLK).
     * |        |          |100 = 32 system clock (HCLK).
     * |        |          |101 = 64 system clock (HCLK).
     * |        |          |110 = 128 system clock (HCLK).
     * |        |          |111 = 256 system clock (HCLK).
     * |        |          |Note: These bits are write protected. Refer to the SYS_REGLCTL register.
     * |[14:12] |LVRDGSEL  |LVR Output De-glitch Time Select (Write Protect)
     * |        |          |000 = Without de-glitch function.
     * |        |          |001 = 4 system clock (HCLK).
     * |        |          |010 = 8 system clock (HCLK).
     * |        |          |011 = 16 system clock (HCLK).
     * |        |          |100 = 32 system clock (HCLK).
     * |        |          |101 = 64 system clock (HCLK).
     * |        |          |110 = 128 system clock (HCLK).
     * |        |          |111 = 256 system clock (HCLK).
     * |        |          |Note: These bits are write protected. Refer to the SYS_REGLCTL register.
     * |[18:16] |BODVL     |Brown-out Detector Threshold Voltage Selection (Write Protect)
     * |        |          |The default value is set by flash controller user configuration register CBOV (CONFIG0 [23:21]).
     * |        |          |000 = Brown-Out Detector threshold voltage is 1.6V.
     * |        |          |001 = Brown-Out Detector threshold voltage is 1.8V.
     * |        |          |010 = Brown-Out Detector threshold voltage is 2.0V.
     * |        |          |011 = Brown-Out Detector threshold voltage is 2.2V.
     * |        |          |100 = Brown-Out Detector threshold voltage is 2.4V.
     * |        |          |101 = Brown-Out Detector threshold voltage is 2.6V.
     * |        |          |110 = Brown-Out Detector threshold voltage is 2.8V.
     * |        |          |111 = Brown-Out Detector threshold voltage is 3.0V.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::IVSCTL
     * Offset: 0x1C  Internal Voltage Source Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |VTEMPEN   |Temperature Sensor Enable Bit
     * |        |          |This bit is used to enable/disable temperature sensor function.
     * |        |          |0 = Temperature sensor function Disabled (default).
     * |        |          |1 = Temperature sensor function Enabled.
     * |        |          |Note: After this bit is set to 1, the value of temperature sensor output can be obtained through GPC.9.
     * |[1]     |VBATUGEN  |VBAT Unity Gain Buffer Enable Bit
     * |        |          |This bit is used to enable/disable VBAT unity gain buffer function.
     * |        |          |0 = VBAT unity gain buffer function Disabled (default).
     * |        |          |1 = VBAT unity gain buffer function Enabled.
     * |        |          |Note: After this bit is set to 1, the value of VBAT unity gain buffer output voltage can be obtained from ADC conversion result
     * @var SYS_T::PORCTL
     * Offset: 0x24  Power-On-Reset Controller Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |POROFF    |Power-on Reset Enable Bit (Write Protect)
     * |        |          |When powered on, the POR circuit generates a reset signal to reset the whole chip function, but noise on the power may cause the POR active again
     * |        |          |User can disable internal POR circuit to avoid unpredictable noise to cause chip reset by writing 0x5AA5 to this field.
     * |        |          |The POR function will be active again when this field is set to another value or chip is reset by other reset source, including:
     * |        |          |nRESET, Watchdog, LVR reset, BOD reset, ICE reset command and the software-chip reset function.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::VREFCTL
     * Offset: 0x28  VREF Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4:0]   |VREFCTL   |VREF Control Bits (Write Protect)
     * |        |          |00000 = VREF is from external pin.
     * |        |          |00011 = VREF is internal 1.6V.
     * |        |          |00111 = VREF is internal 2.0V.
     * |        |          |01011 = VREF is internal 2.5V.
     * |        |          |01111 = VREF is internal 3.0V.
     * |        |          |Others = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7:6]   |PRELOAD_SEL|Pre-load Timing Selection.
     * |        |          |00 = pre-load time is 60us for 0.1uF Capacitor.
     * |        |          |01 = pre-load time is 310us for 1uF Capacitor.
     * |        |          |10 = pre-load time is 1270us for 4.7uF Capacitor.
     * |        |          |11 = pre-load time is 2650us for 10uF Capacitor.
     * @var SYS_T::USBPHY
     * Offset: 0x2C  USB PHY Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |USBROLE   |USB Role Option (Write Protect)
     * |        |          |These two bits are used to select the role of USB.
     * |        |          |00 = Standard USB Device mode.
     * |        |          |01 = Standard USB Host mode.
     * |        |          |10 = ID dependent mode.
     * |        |          |11 = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |SBO       |Note: This bit must always be kept 1. If set to 0, the result is unpredictable
     * |[8]     |USBEN     |USB PHY Enable (Write Protect)
     * |        |          |This bit is used to enable/disable USB PHY.
     * |        |          |0 = USB PHY Disabled.
     * |        |          |1 = USB PHY Enabled.
     * |[17:16] |HSUSBROLE |HSUSB Role Option (Write Protect)
     * |        |          |These two bits are used to select the role of HSUSB
     * |        |          |00 = Standard HSUSB Device mode.
     * |        |          |01 = Standard HSUSB Host mode.
     * |        |          |10 = ID dependent mode.
     * |        |          |11 = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[24]    |HSUSBEN   |HSUSB PHY Enable (Write Protect)
     * |        |          |This bit is used to enable/disable HSUSB PHY.
     * |        |          |0 = HSUSB PHY Disabled.
     * |        |          |1 = HSUSB PHY Enabled.
     * |[25]    |HSUSBACT  |HSUSB PHY Active Control
     * |        |          |This bit is used to control HSUSB PHY at reset state or active state.
     * |        |          |0 = HSUSB PHY at reset state.
     * |        |          |1 = HSUSB PHY at active state.
     * |        |          |Note: After set HSUSBEN (SYS_USBPHY[24]) to enable HSUSB PHY, user should keep HSUSB PHY at reset mode at lease 10uS before changing to active mode.
     * @var SYS_T::GPA_MFPL
     * Offset: 0x30  GPIOA Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PA0MFP    |PA.0 Multi-function Pin Selection
     * |[7:4]   |PA1MFP    |PA.1 Multi-function Pin Selection
     * |[11:8]  |PA2MFP    |PA.2 Multi-function Pin Selection
     * |[15:12] |PA3MFP    |PA.3 Multi-function Pin Selection
     * |[19:16] |PA4MFP    |PA.4 Multi-function Pin Selection
     * |[23:20] |PA5MFP    |PA.5 Multi-function Pin Selection
     * |[27:24] |PA6MFP    |PA.6 Multi-function Pin Selection
     * |[31:28] |PA7MFP    |PA.7 Multi-function Pin Selection
     * @var SYS_T::GPA_MFPH
     * Offset: 0x34  GPIOA High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PA8MFP    |PA.8 Multi-function Pin Selection
     * |[7:4]   |PA9MFP    |PA.9 Multi-function Pin Selection
     * |[11:8]  |PA10MFP   |PA.10 Multi-function Pin Selection
     * |[15:12] |PA11MFP   |PA.11 Multi-function Pin Selection
     * |[19:16] |PA12MFP   |PA.12 Multi-function Pin Selection
     * |[23:20] |PA13MFP   |PA.13 Multi-function Pin Selection
     * |[27:24] |PA14MFP   |PA.14 Multi-function Pin Selection
     * |[31:28] |PA15MFP   |PA.15 Multi-function Pin Selection
     * @var SYS_T::GPB_MFPL
     * Offset: 0x38  GPIOB Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PB0MFP    |PB.0 Multi-function Pin Selection
     * |[7:4]   |PB1MFP    |PB.1 Multi-function Pin Selection
     * |[11:8]  |PB2MFP    |PB.2 Multi-function Pin Selection
     * |[15:12] |PB3MFP    |PB.3 Multi-function Pin Selection
     * |[19:16] |PB4MFP    |PB.4 Multi-function Pin Selection
     * |[23:20] |PB5MFP    |PB.5 Multi-function Pin Selection
     * |[27:24] |PB6MFP    |PB.6 Multi-function Pin Selection
     * |[31:28] |PB7MFP    |PB.7 Multi-function Pin Selection
     * @var SYS_T::GPB_MFPH
     * Offset: 0x3C  GPIOB High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PB8MFP    |PB.8 Multi-function Pin Selection
     * |[7:4]   |PB9MFP    |PB.9 Multi-function Pin Selection
     * |[11:8]  |PB10MFP   |PB.10 Multi-function Pin Selection
     * |[15:12] |PB11MFP   |PB.11 Multi-function Pin Selection
     * |[19:16] |PB12MFP   |PB.12 Multi-function Pin Selection
     * |[23:20] |PB13MFP   |PB.13 Multi-function Pin Selection
     * |[27:24] |PB14MFP   |PB.14 Multi-function Pin Selection
     * |[31:28] |PB15MFP   |PB.15 Multi-function Pin Selection
     * @var SYS_T::GPC_MFPL
     * Offset: 0x40  GPIOC Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PC0MFP    |PC.0 Multi-function Pin Selection
     * |[7:4]   |PC1MFP    |PC.1 Multi-function Pin Selection
     * |[11:8]  |PC2MFP    |PC.2 Multi-function Pin Selection
     * |[15:12] |PC3MFP    |PC.3 Multi-function Pin Selection
     * |[19:16] |PC4MFP    |PC.4 Multi-function Pin Selection
     * |[23:20] |PC5MFP    |PC.5 Multi-function Pin Selection
     * |[27:24] |PC6MFP    |PC.6 Multi-function Pin Selection
     * |[31:28] |PC7MFP    |PC.7 Multi-function Pin Selection
     * @var SYS_T::GPC_MFPH
     * Offset: 0x44  GPIOC High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PC8MFP    |PC.8 Multi-function Pin Selection
     * |[7:4]   |PC9MFP    |PC.9 Multi-function Pin Selection
     * |[11:8]  |PC10MFP   |PC.10 Multi-function Pin Selection
     * |[15:12] |PC11MFP   |PC.11 Multi-function Pin Selection
     * |[19:16] |PC12MFP   |PC.12 Multi-function Pin Selection
     * |[23:20] |PC13MFP   |PC.13 Multi-function Pin Selection
     * |[27:24] |PC14MFP   |PC.14 Multi-function Pin Selection
     * |[31:28] |PC15MFP   |PC.15 Multi-function Pin Selection
     * @var SYS_T::GPD_MFPL
     * Offset: 0x48  GPIOD Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PD0MFP    |PD.0 Multi-function Pin Selection
     * |[7:4]   |PD1MFP    |PD.1 Multi-function Pin Selection
     * |[11:8]  |PD2MFP    |PD.2 Multi-function Pin Selection
     * |[15:12] |PD3MFP    |PD.3 Multi-function Pin Selection
     * |[19:16] |PD4MFP    |PD.4 Multi-function Pin Selection
     * |[23:20] |PD5MFP    |PD.5 Multi-function Pin Selection
     * |[27:24] |PD6MFP    |PD.6 Multi-function Pin Selection
     * |[31:28] |PD7MFP    |PD.7 Multi-function Pin Selection
     * @var SYS_T::GPD_MFPH
     * Offset: 0x4C  GPIOD High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PD8MFP    |PD.8 Multi-function Pin Selection
     * |[7:4]   |PD9MFP    |PD.9 Multi-function Pin Selection
     * |[11:8]  |PD10MFP   |PD.10 Multi-function Pin Selection
     * |[15:12] |PD11MFP   |PD.11 Multi-function Pin Selection
     * |[19:16] |PD12MFP   |PD.12 Multi-function Pin Selection
     * |[23:20] |PD13MFP   |PD.13 Multi-function Pin Selection
     * |[27:24] |PD14MFP   |PD.14 Multi-function Pin Selection
     * |[31:28] |PD15MFP   |PD.15 Multi-function Pin Selection
     * @var SYS_T::GPE_MFPL
     * Offset: 0x50  GPIOE Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PE0MFP    |PE.0 Multi-function Pin Selection
     * |[7:4]   |PE1MFP    |PE.1 Multi-function Pin Selection
     * |[11:8]  |PE2MFP    |PE.2 Multi-function Pin Selection
     * |[15:12] |PE3MFP    |PE.3 Multi-function Pin Selection
     * |[19:16] |PE4MFP    |PE.4 Multi-function Pin Selection
     * |[23:20] |PE5MFP    |PE.5 Multi-function Pin Selection
     * |[27:24] |PE6MFP    |PE.6 Multi-function Pin Selection
     * |[31:28] |PE7MFP    |PE.7 Multi-function Pin Selection
     * @var SYS_T::GPE_MFPH
     * Offset: 0x54  GPIOE High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PE8MFP    |PE.8 Multi-function Pin Selection
     * |[7:4]   |PE9MFP    |PE.9 Multi-function Pin Selection
     * |[11:8]  |PE10MFP   |PE.10 Multi-function Pin Selection
     * |[15:12] |PE11MFP   |PE.11 Multi-function Pin Selection
     * |[19:16] |PE12MFP   |PE.12 Multi-function Pin Selection
     * |[23:20] |PE13MFP   |PE.13 Multi-function Pin Selection
     * |[27:24] |PE14MFP   |PE.14 Multi-function Pin Selection
     * |[31:28] |PE15MFP   |PE.15 Multi-function Pin Selection
     * @var SYS_T::GPF_MFPL
     * Offset: 0x58  GPIOF Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PF0MFP    |PF.0 Multi-function Pin Selection
     * |[7:4]   |PF1MFP    |PF.1 Multi-function Pin Selection
     * |[11:8]  |PF2MFP    |PF.2 Multi-function Pin Selection
     * |[15:12] |PF3MFP    |PF.3 Multi-function Pin Selection
     * |[19:16] |PF4MFP    |PF.4 Multi-function Pin Selection
     * |[23:20] |PF5MFP    |PF.5 Multi-function Pin Selection
     * |[27:24] |PF6MFP    |PF.6 Multi-function Pin Selection
     * |[31:28] |PF7MFP    |PF.7 Multi-function Pin Selection
     * @var SYS_T::GPF_MFPH
     * Offset: 0x5C  GPIOF High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PF8MFP    |PF.8 Multi-function Pin Selection
     * |[7:4]   |PF9MFP    |PF.9 Multi-function Pin Selection
     * |[11:8]  |PF10MFP   |PF.10 Multi-function Pin Selection
     * |[15:12] |PF11MFP   |PF.11 Multi-function Pin Selection
     * |[19:16] |PF12MFP   |PF.12 Multi-function Pin Selection
     * |[23:20] |PF13MFP   |PF.13 Multi-function Pin Selection
     * |[27:24] |PF14MFP   |PF.14 Multi-function Pin Selection
     * |[31:28] |PF15MFP   |PF.15 Multi-function Pin Selection
     * @var SYS_T::GPG_MFPL
     * Offset: 0x60  GPIOG Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PG0MFP    |PG.0 Multi-function Pin Selection
     * |[7:4]   |PG1MFP    |PG.1 Multi-function Pin Selection
     * |[11:8]  |PG2MFP    |PG.2 Multi-function Pin Selection
     * |[15:12] |PG3MFP    |PG.3 Multi-function Pin Selection
     * |[19:16] |PG4MFP    |PG.4 Multi-function Pin Selection
     * |[23:20] |PG5MFP    |PG.5 Multi-function Pin Selection
     * |[27:24] |PG6MFP    |PG.6 Multi-function Pin Selection
     * |[31:28] |PG7MFP    |PG.7 Multi-function Pin Selection
     * @var SYS_T::GPG_MFPH
     * Offset: 0x64  GPIOG High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PG8MFP    |PG.8 Multi-function Pin Selection
     * |[7:4]   |PG9MFP    |PG.9 Multi-function Pin Selection
     * |[11:8]  |PG10MFP   |PG.10 Multi-function Pin Selection
     * |[15:12] |PG11MFP   |PG.11 Multi-function Pin Selection
     * |[19:16] |PG12MFP   |PG.12 Multi-function Pin Selection
     * |[23:20] |PG13MFP   |PG.13 Multi-function Pin Selection
     * |[27:24] |PG14MFP   |PG.14 Multi-function Pin Selection
     * |[31:28] |PG15MFP   |PG.15 Multi-function Pin Selection
     * @var SYS_T::GPH_MFPL
     * Offset: 0x68  GPIOH Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PH0MFP    |PH.0 Multi-function Pin Selection
     * |[7:4]   |PH1MFP    |PH.1 Multi-function Pin Selection
     * |[11:8]  |PH2MFP    |PH.2 Multi-function Pin Selection
     * |[15:12] |PH3MFP    |PH.3 Multi-function Pin Selection
     * |[19:16] |PH4MFP    |PH.4 Multi-function Pin Selection
     * |[23:20] |PH5MFP    |PH.5 Multi-function Pin Selection
     * |[27:24] |PH6MFP    |PH.6 Multi-function Pin Selection
     * |[31:28] |PH7MFP    |PH.7 Multi-function Pin Selection
     * @var SYS_T::GPH_MFPH
     * Offset: 0x6C  GPIOH High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PH8MFP    |PH.8 Multi-function Pin Selection
     * |[7:4]   |PH9MFP    |PH.9 Multi-function Pin Selection
     * |[11:8]  |PH10MFP   |PH.10 Multi-function Pin Selection
     * |[15:12] |PH11MFP   |PH.11 Multi-function Pin Selection
     * |[19:16] |PH12MFP   |PH.12 Multi-function Pin Selection
     * |[23:20] |PH13MFP   |PH.13 Multi-function Pin Selection
     * |[27:24] |PH14MFP   |PH.14 Multi-function Pin Selection
     * |[31:28] |PH15MFP   |PH.15 Multi-function Pin Selection
     * @var SYS_T::GPA_MFOS
     * Offset: 0x80  GPIOA Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPB_MFOS
     * Offset: 0x84  GPIOB Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPC_MFOS
     * Offset: 0x88  GPIOC Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPD_MFOS
     * Offset: 0x8C  GPIOD Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPE_MFOS
     * Offset: 0x90  GPIOE Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPF_MFOS
     * Offset: 0x94  GPIOF Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPG_MFOS
     * Offset: 0x98  GPIOG Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::GPH_MFOS
     * Offset: 0x9C  GPIOH Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[1]     |MFOS1     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[2]     |MFOS2     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[3]     |MFOS3     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[4]     |MFOS4     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[5]     |MFOS5     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[6]     |MFOS6     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[7]     |MFOS7     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[8]     |MFOS8     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[9]     |MFOS9     |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[10]    |MFOS10    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[11]    |MFOS11    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[12]    |MFOS12    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[13]    |MFOS13    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[14]    |MFOS14    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * |[15]    |MFOS15    |GPIOA-H Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple function pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple function pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15 for port A/B/E/G.
     * |        |          |Max. n=14 for port C/D.
     * |        |          |Max. n=11 for port F/H.
     * @var SYS_T::SRAM_INTCTL
     * Offset: 0xC0  System SRAM Interrupt Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PERRIEN   |SRAM Parity Check Error Interrupt Enable Bit
     * |        |          |0 = SRAM parity check error interrupt Disabled.
     * |        |          |1 = SRAM parity check error interrupt Enabled.
     * @var SYS_T::SRAM_STATUS
     * Offset: 0xC4  System SRAM Parity Error Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PERRIF    |SRAM Parity Check Error Flag
     * |        |          |This bit indicates the System SRAM parity error occurred. Write 1 to clear this to 0.
     * |        |          |0 = No System SRAM parity error.
     * |        |          |1 = System SRAM parity error occur.
     * @var SYS_T::SRAM_ERRADDR
     * Offset: 0xC8  System SRAM Parity Check Error Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ERRADDR   |System SRAM Parity Error Address
     * |        |          |This register shows system SRAM parity error byte address.
     * @var SYS_T::SRAM_BISTCTL
     * Offset: 0xD0  System SRAM BIST Test Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SRBIST0   |SRAM Bank0 BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for SRAM bank0.
     * |        |          |0 = system SRAM bank0 BIST Disabled.
     * |        |          |1 = system SRAM bank0 BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |SRBIST1   |SRAM Bank1 BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for SRAM bank1.
     * |        |          |0 = system SRAM bank1 BIST Disabled.
     * |        |          |1 = system SRAM bank1 BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |CRBIST    |CACHE BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for CACHE RAM
     * |        |          |0 = system CACHE BIST Disabled.
     * |        |          |1 = system CACHE BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |CANBIST   |CAN BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for CAN RAM
     * |        |          |0 = system CAN BIST Disabled.
     * |        |          |1 = system CAN BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[4]     |USBBIST   |USB BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for USB RAM
     * |        |          |0 = system USB BIST Disabled.
     * |        |          |1 = system USB BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[5]     |SPIMBIST  |SPIM BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for SPIM RAM
     * |        |          |0 = system SPIM BIST Disabled.
     * |        |          |1 = system SPIM BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |EMCBIST   |EMC BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for EMC RAM
     * |        |          |0 = system EMC BIST Disabled.
     * |        |          |1 = system EMC BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7]     |PDMABIST  |PDMA BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for PDMA RAM
     * |        |          |0 = system PDMA BIST Disabled.
     * |        |          |1 = system PDMA BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |HSUSBDBIST|HSUSBD BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for HSUSBD RAM
     * |        |          |0 = system HSUSBD BIST Disabled.
     * |        |          |1 = system HSUSBD BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[9]     |HSUSBHBIST|HSUSBH BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for HSUSBH RAM
     * |        |          |0 = system HSUSBH BIST Disabled.
     * |        |          |1 = system HSUSBH BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[16]    |SRB0S0    |SRAM Bank0 Section 0 BIST Select (Write Protect)
     * |        |          |This bit define if the first 16KB section of SRAM bank0 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank0 section 0 is deselected when doing bist test.
     * |        |          |1 = SRAM bank0 section 0 is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank0 should be selected when doing SRAM bank0 bist test.
     * |[17]    |SRB0S1    |SRAM Bank0 Section 1 BIST Select (Write Protect)
     * |        |          |This bit define if the second 16KB section of SRAM bank0 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank0 section 1 is deselected when doing bist test.
     * |        |          |1 = SRAM bank0 section 1 is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank0 should be selected when doing SRAM bank0 bist test.
     * |[18]    |SRB1S0    |SRAM Bank1 Section 0 BIST Select (Write Protect)
     * |        |          |This bit define if the first 16KB section of SRAM bank1 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank1 first 16KB section is deselected when doing bist test.
     * |        |          |1 = SRAM bank1 first 16KB section is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank1 should be selected when doing SRAM bank1 bist test.
     * |[19]    |SRB1S1    |SRAM Bank1 Section 1 BIST Select (Write Protect)
     * |        |          |This bit define if the second 16KB section of SRAM bank1 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank1 second 16KB section is deselected when doing bist test.
     * |        |          |1 = SRAM bank1 second 16KB section is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank1 should be selected when doing SRAM bank1 bist test.
     * |[20]    |SRB1S2    |SRAM Bank1 Section 0 BIST Select (Write Protect)
     * |        |          |This bit define if the third 16KB section of SRAM bank1 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank1 third 16KB section is deselected when doing bist test.
     * |        |          |1 = SRAM bank1 third 16KB section is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank1 should be selected when doing SRAM bank1 bist test.
     * |[21]    |SRB1S3    |SRAM Bank1 Section 1 BIST Select (Write Protect)
     * |        |          |This bit define if the fourth 16KB section of SRAM bank1 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank1 fourth 16KB section is deselected when doing bist test.
     * |        |          |1 = SRAM bank1 fourth 16KB section is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank1 should be selected when doing SRAM bank1 bist test.
     * |[22]    |SRB1S4    |SRAM Bank1 Section 0 BIST Select (Write Protect)
     * |        |          |This bit define if the fifth 16KB section of SRAM bank1 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank1 fifth 16KB section is deselected when doing bist test.
     * |        |          |1 = SRAM bank1 fifth 16KB section is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank1 should be selected when doing SRAM bank1 bist test.
     * |[23]    |SRB1S5    |SRAM Bank1 Section 1 BIST Select (Write Protect)
     * |        |          |This bit define if the sixth 16KB section of SRAM bank1 is selected or not when doing bist test.
     * |        |          |0 = SRAM bank1 sixth 16KB section is deselected when doing bist test.
     * |        |          |1 = SRAM bank1 sixth 16KB section is selected when doing bist test.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: At least one section of SRAM bank1 should be selected when doing SRAM bank1 bist test.
     * @var SYS_T::SRAM_BISTSTS
     * Offset: 0xD4  System SRAM BIST Test Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SRBISTEF0 |1st System SRAM BIST Fail Flag
     * |        |          |0 = 1st system SRAM BIST test pass.
     * |        |          |1 = 1st system SRAM BIST test fail.
     * |[1]     |SRBISTEF1 |2nd System SRAM BIST Fail Flag
     * |        |          |0 = 2nd system SRAM BIST test pass.
     * |        |          |1 = 2nd system SRAM BIST test fail.
     * |[2]     |CRBISTEF  |CACHE SRAM BIST Fail Flag
     * |        |          |0 = System CACHE RAM BIST test pass.
     * |        |          |1 = System CACHE RAM BIST test fail.
     * |[3]     |CANBEF    |CAN SRAM BIST Fail Flag
     * |        |          |0 = CAN SRAM BIST test pass.
     * |        |          |1 = CAN SRAM BIST test fail.
     * |[4]     |USBBEF    |USB SRAM BIST Fail Flag
     * |        |          |0 = USB SRAM BIST test pass.
     * |        |          |1 = USB SRAM BIST test fail.
     * |[16]    |SRBEND0   |1st SRAM BIST Test Finish
     * |        |          |0 = 1st system SRAM BIST active.
     * |        |          |1 =1st system SRAM BIST finish.
     * |[17]    |SRBEND1   |2nd SRAM BIST Test Finish
     * |        |          |0 = 2nd system SRAM BIST is active.
     * |        |          |1 = 2nd system SRAM BIST finish.
     * |[18]    |CRBEND    |CACHE SRAM BIST Test Finish
     * |        |          |0 = System CACHE RAM BIST is active.
     * |        |          |1 = System CACHE RAM BIST test finish.
     * |[19]    |CANBEND   |CAN SRAM BIST Test Finish
     * |        |          |0 = CAN SRAM BIST is active.
     * |        |          |1 = CAN SRAM BIST test finish.
     * |[20]    |USBBEND   |USB SRAM BIST Test Finish
     * |        |          |0 = USB SRAM BIST is active.
     * |        |          |1 = USB SRAM BIST test finish.
     * @var SYS_T::HIRCTCTL
     * Offset: 0xE4  HIRC48M Trim Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |FREQSEL   |Trim Frequency Selection
     * |        |          |This field indicates the target frequency of 48 MHz internal high speed RC oscillator (HIRC) auto trim.
     * |        |          |During auto trim operation, if clock error detected with CESTOPEN is set to 1 or trim retry limitation count reached, this field will be cleared to 00 automatically.
     * |        |          |00 = Disable HIRC auto trim function.
     * |        |          |01 = Enable HIRC auto trim function and trim HIRC to 48 MHz.
     * |        |          |10 = Reserved..
     * |        |          |11 = Reserved.
     * |[5:4]   |LOOPSEL   |Trim Calculation Loop Selection
     * |        |          |This field defines that trim value calculation is based on how many reference clocks.
     * |        |          |00 = Trim value calculation is based on average difference in 4 clocks of reference clock.
     * |        |          |01 = Trim value calculation is based on average difference in 8 clocks of reference clock.
     * |        |          |10 = Trim value calculation is based on average difference in 16 clocks of reference clock.
     * |        |          |11 = Trim value calculation is based on average difference in 32 clocks of reference clock.
     * |        |          |Note: For example, if LOOPSEL is set as 00, auto trim circuit will calculate trim value based on the average frequency difference in 4 clocks of reference clock.
     * |[7:6]   |RETRYCNT  |Trim Value Update Limitation Count
     * |        |          |This field defines that how many times the auto trim circuit will try to update the HIRC trim value before the frequency of HIRC locked.
     * |        |          |Once the HIRC locked, the internal trim value update counter will be reset.
     * |        |          |If the trim value update counter reached this limitation value and frequency of HIRC still doesn't lock, the auto trim operation will be disabled and FREQSEL will be cleared to 00.
     * |        |          |00 = Trim retry count limitation is 64 loops.
     * |        |          |01 = Trim retry count limitation is 128 loops.
     * |        |          |10 = Trim retry count limitation is 256 loops.
     * |        |          |11 = Trim retry count limitation is 512 loops.
     * |[8]     |CESTOPEN  |Clock Error Stop Enable Bit
     * |        |          |0 = The trim operation is keep going if clock is inaccuracy.
     * |        |          |1 = The trim operation is stopped if clock is inaccuracy.
     * |[9]     |BOUNDEN   |Boundary Enable Bit
     * |        |          |0 = Boundary function is disable.
     * |        |          |1 = Boundary function is enable.
     * |[10]    |REFCKSEL  |Reference Clock Selection
     * |        |          |0 = HIRC trim reference from external 32.768 kHz crystal oscillator.
     * |        |          |1 = HIRC trim reference from internal USB synchronous mode.
     * |        |          |Note: HIRC trim reference clock is 20Khz in test mode.
     * |[20:16  |BOUNDARY  |Boundary Selection
     * |        |          |Fill the boundary range from 0x1 to 0x31, 0x0 is reserved.
     * |        |          |Note1: This field is effective only when the BOUNDEN(SYS_HIRCTRIMCTL[9]) is enable.
     * @var SYS_T::HIRCTIEN
     * Offset: 0xE8  HIRC48M Trim Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TFAILIEN  |Trim Failure Interrupt Enable Bit
     * |        |          |This bit controls if an interrupt will be triggered while HIRC trim value update limitation count reached and HIRC frequency still not locked on target frequency set by FREQSEL(SYS_HIRCTCTL[1:0]).
     * |        |          |If this bit is high and TFAILIF(SYS_HIRCTISTS[1]) is set during auto trim operation, an interrupt will be triggered to notify that HIRC trim value update limitation count was reached.
     * |        |          |0 = Disable TFAILIF(SYS_HIRCTISTS[1]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable TFAILIF(SYS_HIRCTISTS[1]) status to trigger an interrupt to CPU.
     * |[2]     |CLKEIEN   |Clock Error Interrupt Enable Bit
     * |        |          |This bit controls if CPU would get an interrupt while clock is inaccuracy during auto trim operation.
     * |        |          |If this bit is set to1, and CLKERRIF(SYS_HIRCTISTS[2]) is set during auto trim operation, an interrupt will be triggered to notify the clock frequency is inaccuracy.
     * |        |          |0 = Disable CLKERRIF(SYS_HIRCTISTS[2]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable CLKERRIF(SYS_HIRCTISTS[2]) status to trigger an interrupt to CPU.
     * @var SYS_T::HIRCTISTS
     * Offset: 0xEC  HIRC48M Trim Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FREQLOCK  |HIRC Frequency Lock Status
     * |        |          |This bit indicates the HIRC frequency is locked.
     * |        |          |This is a status bit and doesn't trigger any interrupt
     * |        |          |Write 1 to clear this to 0
     * |        |          |This bit will be set automatically, if the frequency is lock and the RC_TRIM is enabled.
     * |        |          |0 = The internal high-speed oscillator frequency doesn't lock at 48 MHz yet.
     * |        |          |1 = The internal high-speed oscillator frequency locked at 48 MHz.
     * |[1]     |TFAILIF   |Trim Failure Interrupt Status
     * |        |          |This bit indicates that HIRC trim value update limitation count reached and the HIRC clock frequency still doesn't be locked
     * |        |          |Once this bit is set, the auto trim operation stopped and FREQSEL(SYS_HIRCTCTL[1:0]) will be cleared to 00 by hardware automatically.
     * |        |          |If this bit is set and TFAILIEN(SYS_HIRCTIEN[1]) is high, an interrupt will be triggered to notify that HIRC trim value update limitation count was reached
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Trim value update limitation count does not reach.
     * |        |          |1 = Trim value update limitation count reached and HIRC frequency still not locked.
     * |[2]     |CLKERRIF  |Clock Error Interrupt Status
     * |        |          |When the frequency of 32.768 kHz external low speed crystal oscillator (LXT) or 48MHz internal high speed RC oscillator (HIRC) is shift larger to unreasonable value, this bit will be set and to be an indicate that clock frequency is inaccuracy.
     * |        |          |Once this bit is set to 1, the auto trim operation stopped and FREQSEL(SYS_HIRCTCL[1:0]) will be cleared to 00 by hardware automatically if CESTOPEN(SYS_HIRCTCTL[8]) is set to 1.
     * |        |          |If this bit is set and CLKEIEN(SYS_HIRCTIEN[2]) is high, an interrupt will be triggered to notify the clock frequency is inaccuracy.
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Clock frequency is accurate.
     * |        |          |1 = Clock frequency is inaccurate.
     * |[3]     |OVBDIF    |Over Boundary Status
     * |        |          |When the over boundary function is set, if there occurs the over boundary condition, this flag will be set.
     * |        |          |Note1: Write 1 to clear this flag.
     * |        |          |Note2: This function is only supported in M48xGC/M48xG8.
     * |        |          |0 = Over boundary condition did not occur.
     * |        |          |1 = Over boundary condition occurred.
     * @var SYS_T::IRCTCTL
     * Offset: 0xF0  HIRC Trim Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |FREQSEL   |Trim Frequency Selection
     * |        |          |This field indicates the target frequency of 12 MHz internal high speed RC oscillator (HIRC) auto trim.
     * |        |          |During auto trim operation, if clock error detected with CESTOPEN is set to 1 or trim retry limitation count reached, this field will be cleared to 00 automatically.
     * |        |          |00 = Disable HIRC auto trim function.
     * |        |          |01 = Enable HIRC auto trim function and trim HIRC to 12 MHz.
     * |        |          |10 = Reserved..
     * |        |          |11 = Reserved.
     * |[5:4]   |LOOPSEL   |Trim Calculation Loop Selection
     * |        |          |This field defines that trim value calculation is based on how many reference clocks.
     * |        |          |00 = Trim value calculation is based on average difference in 4 clocks of reference clock.
     * |        |          |01 = Trim value calculation is based on average difference in 8 clocks of reference clock.
     * |        |          |10 = Trim value calculation is based on average difference in 16 clocks of reference clock.
     * |        |          |11 = Trim value calculation is based on average difference in 32 clocks of reference clock.
     * |        |          |Note: For example, if LOOPSEL is set as 00, auto trim circuit will calculate trim value based on the average frequency difference in 4 clocks of reference clock.
     * |[7:6]   |RETRYCNT  |Trim Value Update Limitation Count
     * |        |          |This field defines that how many times the auto trim circuit will try to update the HIRC trim value before the frequency of HIRC locked.
     * |        |          |Once the HIRC locked, the internal trim value update counter will be reset.
     * |        |          |If the trim value update counter reached this limitation value and frequency of HIRC still doesn't lock, the auto trim operation will be disabled and FREQSEL will be cleared to 00.
     * |        |          |00 = Trim retry count limitation is 64 loops.
     * |        |          |01 = Trim retry count limitation is 128 loops.
     * |        |          |10 = Trim retry count limitation is 256 loops.
     * |        |          |11 = Trim retry count limitation is 512 loops.
     * |[8]     |CESTOPEN  |Clock Error Stop Enable Bit
     * |        |          |0 = The trim operation is keep going if clock is inaccuracy.
     * |        |          |1 = The trim operation is stopped if clock is inaccuracy.
     * |[10]    |REFCKSEL  |Reference Clock Selection
     * |        |          |0 = HIRC trim reference from external 32.768 kHz crystal oscillator.
     * |        |          |1 = HIRC trim reference from internal USB synchronous mode.
     * |        |          |Note: HIRC trim reference clock is 20Khz in test mode.
     * @var SYS_T::IRCTIEN
     * Offset: 0xF4  HIRC Trim Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TFAILIEN  |Trim Failure Interrupt Enable Bit
     * |        |          |This bit controls if an interrupt will be triggered while HIRC trim value update limitation count reached and HIRC frequency still not locked on target frequency set by FREQSEL(SYS_IRCTCTL[1:0]).
     * |        |          |If this bit is high and TFAILIF(SYS_IRCTISTS[1]) is set during auto trim operation, an interrupt will be triggered to notify that HIRC trim value update limitation count was reached.
     * |        |          |0 = Disable TFAILIF(SYS_IRCTISTS[1]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable TFAILIF(SYS_IRCTISTS[1]) status to trigger an interrupt to CPU.
     * |[2]     |CLKEIEN   |Clock Error Interrupt Enable Bit
     * |        |          |This bit controls if CPU would get an interrupt while clock is inaccuracy during auto trim operation.
     * |        |          |If this bit is set to1, and CLKERRIF(SYS_IRCTISTS[2]) is set during auto trim operation, an interrupt will be triggered to notify the clock frequency is inaccuracy.
     * |        |          |0 = Disable CLKERRIF(SYS_IRCTISTS[2]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable CLKERRIF(SYS_IRCTISTS[2]) status to trigger an interrupt to CPU.
     * @var SYS_T::IRCTISTS
     * Offset: 0xF8  HIRC Trim Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FREQLOCK  |HIRC Frequency Lock Status
     * |        |          |This bit indicates the HIRC frequency is locked.
     * |        |          |This is a status bit and doesn't trigger any interrupt
     * |        |          |Write 1 to clear this to 0
     * |        |          |This bit will be set automatically, if the frequency is lock and the RC_TRIM is enabled.
     * |        |          |0 = The internal high-speed oscillator frequency doesn't lock at 12 MHz yet.
     * |        |          |1 = The internal high-speed oscillator frequency locked at 12 MHz.
     * |[1]     |TFAILIF   |Trim Failure Interrupt Status
     * |        |          |This bit indicates that HIRC trim value update limitation count reached and the HIRC clock frequency still doesn't be locked
     * |        |          |Once this bit is set, the auto trim operation stopped and FREQSEL(SYS_IRCTCTL[1:0]) will be cleared to 00 by hardware automatically.
     * |        |          |If this bit is set and TFAILIEN(SYS_IRCTIEN[1]) is high, an interrupt will be triggered to notify that HIRC trim value update limitation count was reached
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Trim value update limitation count does not reach.
     * |        |          |1 = Trim value update limitation count reached and HIRC frequency still not locked.
     * |[2]     |CLKERRIF  |Clock Error Interrupt Status
     * |        |          |When the frequency of 32.768 kHz external low speed crystal oscillator (LXT) or 12MHz internal high speed RC oscillator (HIRC) is shift larger to unreasonable value, this bit will be set and to be an indicate that clock frequency is inaccuracy.
     * |        |          |Once this bit is set to 1, the auto trim operation stopped and FREQSEL(SYS_IRCTCL[1:0]) will be cleared to 00 by hardware automatically if CESTOPEN(SYS_IRCTCTL[8]) is set to 1.
     * |        |          |If this bit is set and CLKEIEN(SYS_IRCTIEN[2]) is high, an interrupt will be triggered to notify the clock frequency is inaccuracy.
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Clock frequency is accurate.
     * |        |          |1 = Clock frequency is inaccurate.
     * @var SYS_T::REGLCTL
     * Offset: 0x100  Register Lock Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |REGLCTL   |Register Lock Control Code
     * |        |          |Some registers have write-protection function
     * |        |          |Writing these registers have to disable the protected function by writing the sequence value "59h", "16h", "88h" to this field.
     * |        |          |After this sequence is completed, the REGLCTL bit will be set to 1 and write-protection registers can be normal write.
     * |        |          |Register Lock Control Code
     * |        |          |0 = Write-protection Enabled for writing protected registers
     * |        |          |Any write to the protected register is ignored.
     * |        |          |1 = Write-protection Disabled for writing protected registers.
     * @var SYS_T::PORDISAN
     * Offset: 0x1EC  Analog POR Disable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |POROFFAN  |Power-on Reset Enable Bit (Write Protect)
     * |        |          |After powered on, User can turn off internal analog POR circuit to save power by writing 0x5AA5 to this field.
     * |        |          |The analog POR circuit will be active again when  this field is set to another value or chip is reset by other reset source, including:
     * |        |          |nRESET, Watchdog, LVR reset, BOD reset, ICE reset command and the software-chip reset function.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::PLCTL
     * Offset: 0x1F8  Power Level Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |PLSEL     |Power Level Select(Write Protect)
     * |        |          |00 = Power level is PL0.
     * |        |          |01 = Power level is PL1.
     * |        |          |Others = Reserved.
     * |[21:16] |LVSSTEP   |LDO Voltage Scaling Step(Write Protect)
     * |        |          |The LVSSTEP value is LDO voltage rising step.
     * |        |          |Core voltage scaling voltage step = (LVSSTEP + 1) * 10mV.
     * |[31:24] |LVSPRD    |LDO Voltage Scaling Period(Write Protect)
     * |        |          |The LVSPRD value is the period of each LDO voltage rising step.
     * |        |          |LDO voltage scaling period = (LVSPRD + 1) * 1us.
     * @var SYS_T::PLSTS
     * Offset: 0x1FC  Power Level Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PLCBUSY   |Power Level Change Busy Bit (Read Only)
     * |        |          |This bit is set by hardware when core voltage is changing
     * |        |          |After core voltage change is completed, this bit will be cleared automatically by hardware.
     * |        |          |0 = Core voltage change is completed.
     * |        |          |1 = Core voltage change is ongoing.
     * |[9:8]   |PLSTATUS  |Power Level Status (Read Only)
     * |        |          |00 = Power level is PL0.
     * |        |          |01 = Power level is PL1.
     * |        |          |Others = Reserved.
     * @var SYS_T::AHBMCTL
     * Offset: 0x400  AHB Bus Matrix Priority Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |INTACTEN  |Highest AHB Bus Priority of Cortex M4 Core Enable Bit (Write Protect)
     * |        |          |Enable Cortex-M4 Core With Highest AHB Bus Priority In AHB Bus Matrix
     * |        |          |0 = Run robin mode.
     * |        |          |1 = Cortex-M4 CPU with highest bus priority when interrupt occurred.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     */
    __I  uint32_t PDID;                  /*!< [0x0000] Part Device Identification Number Register                       */
    __IO uint32_t RSTSTS;                /*!< [0x0004] System Reset Status Register                                     */
    __IO uint32_t IPRST0;                /*!< [0x0008] Peripheral  Reset Control Register 0                             */
    __IO uint32_t IPRST1;                /*!< [0x000c] Peripheral Reset Control Register 1                              */
    __IO uint32_t IPRST2;                /*!< [0x0010] Peripheral Reset Control Register 2                              */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE0[1];
    /** @endcond */
    __IO uint32_t BODCTL;                /*!< [0x0018] Brown-Out Detector Control Register                              */
    __IO uint32_t IVSCTL;                /*!< [0x001c] Internal Voltage Source Control Register                         */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE1[1];
    /** @endcond */
    __IO uint32_t PORCTL;                /*!< [0x0024] Power-On-Reset Controller Register                               */
    __IO uint32_t VREFCTL;               /*!< [0x0028] VREF Control Register                                            */
    __IO uint32_t USBPHY;                /*!< [0x002c] USB PHY Control Register                                         */
    __IO uint32_t GPA_MFPL;              /*!< [0x0030] GPIOA Low Byte Multiple Function Control Register                */
    __IO uint32_t GPA_MFPH;              /*!< [0x0034] GPIOA High Byte Multiple Function Control Register               */
    __IO uint32_t GPB_MFPL;              /*!< [0x0038] GPIOB Low Byte Multiple Function Control Register                */
    __IO uint32_t GPB_MFPH;              /*!< [0x003c] GPIOB High Byte Multiple Function Control Register               */
    __IO uint32_t GPC_MFPL;              /*!< [0x0040] GPIOC Low Byte Multiple Function Control Register                */
    __IO uint32_t GPC_MFPH;              /*!< [0x0044] GPIOC High Byte Multiple Function Control Register               */
    __IO uint32_t GPD_MFPL;              /*!< [0x0048] GPIOD Low Byte Multiple Function Control Register                */
    __IO uint32_t GPD_MFPH;              /*!< [0x004c] GPIOD High Byte Multiple Function Control Register               */
    __IO uint32_t GPE_MFPL;              /*!< [0x0050] GPIOE Low Byte Multiple Function Control Register                */
    __IO uint32_t GPE_MFPH;              /*!< [0x0054] GPIOE High Byte Multiple Function Control Register               */
    __IO uint32_t GPF_MFPL;              /*!< [0x0058] GPIOF Low Byte Multiple Function Control Register                */
    __IO uint32_t GPF_MFPH;              /*!< [0x005c] GPIOF High Byte Multiple Function Control Register               */
    __IO uint32_t GPG_MFPL;              /*!< [0x0060] GPIOG Low Byte Multiple Function Control Register                */
    __IO uint32_t GPG_MFPH;              /*!< [0x0064] GPIOG High Byte Multiple Function Control Register               */
    __IO uint32_t GPH_MFPL;              /*!< [0x0068] GPIOH Low Byte Multiple Function Control Register                */
    __IO uint32_t GPH_MFPH;              /*!< [0x006c] GPIOH High Byte Multiple Function Control Register               */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE2[4];
    /** @endcond */
    __IO uint32_t GPA_MFOS;              /*!< [0x0080] GPIOA Multiple Function Output Select Register                   */
    __IO uint32_t GPB_MFOS;              /*!< [0x0084] GPIOB Multiple Function Output Select Register                   */
    __IO uint32_t GPC_MFOS;              /*!< [0x0088] GPIOC Multiple Function Output Select Register                   */
    __IO uint32_t GPD_MFOS;              /*!< [0x008c] GPIOD Multiple Function Output Select Register                   */
    __IO uint32_t GPE_MFOS;              /*!< [0x0090] GPIOE Multiple Function Output Select Register                   */
    __IO uint32_t GPF_MFOS;              /*!< [0x0094] GPIOF Multiple Function Output Select Register                   */
    __IO uint32_t GPG_MFOS;              /*!< [0x0098] GPIOG Multiple Function Output Select Register                   */
    __IO uint32_t GPH_MFOS;              /*!< [0x009c] GPIOH Multiple Function Output Select Register                   */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE3[8];
    /** @endcond */
    __IO uint32_t SRAM_INTCTL;           /*!< [0x00c0] System SRAM Interrupt Enable Control Register                    */
    __IO uint32_t SRAM_STATUS;           /*!< [0x00c4] System SRAM Parity Error Status Register                         */
    __I  uint32_t SRAM_ERRADDR;          /*!< [0x00c8] System SRAM Parity Check Error Address Register                  */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE4[1];
    /** @endcond */
    __IO uint32_t SRAM_BISTCTL;          /*!< [0x00d0] System SRAM BIST Test Control Register                           */
    __I  uint32_t SRAM_BISTSTS;          /*!< [0x00d4] System SRAM BIST Test Status Register                            */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE5[3];
    /** @endcond */
    __IO uint32_t HIRCTCTL;              /*!< [0x00e4] HIRC48M Trim Control Register                                    */
    __IO uint32_t HIRCTIEN;              /*!< [0x00e8] HIRC48M Trim Interrupt Enable Register                           */
    __IO uint32_t HIRCTISTS;             /*!< [0x00ec] HIRC48M Trim Interrupt Status Register                           */
    __IO uint32_t IRCTCTL;               /*!< [0x00f0] HIRC Trim Control Register                                       */
    __IO uint32_t IRCTIEN;               /*!< [0x00f4] HIRC Trim Interrupt Enable Register                              */
    __IO uint32_t IRCTISTS;              /*!< [0x00f8] HIRC Trim Interrupt Status Register                              */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE6[1];
    /** @endcond */
    __IO uint32_t REGLCTL;               /*!< [0x0100] Register Lock Control Register                                   */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE7[58];
    /** @endcond */
    __IO uint32_t PORDISAN;              /*!< [0x01ec] Analog POR Disable Control Register                              */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE8;
    /** @endcond */
    __I  uint32_t CSERVER;               /*!< [0x01f4] Chip Series Version Register                                     */
    __IO uint32_t PLCTL;                 /*!< [0x01f8] Power Level Control Register                                     */
    __I  uint32_t PLSTS;                 /*!< [0x01fc] Power Level Status Register                                      */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE9[128];
    /** @endcond */
    __IO uint32_t AHBMCTL;               /*!< [0x0400] AHB Bus Matrix Priority Control Register                         */

} SYS_T;

/**
    @addtogroup SYS_CONST SYS Bit Field Definition
    Constant Definitions for SYS Controller
@{ */

#define SYS_PDID_PDID_Pos                (0)                                               /*!< SYS_T::PDID: PDID Position             */
#define SYS_PDID_PDID_Msk                (0xfffffffful << SYS_PDID_PDID_Pos)               /*!< SYS_T::PDID: PDID Mask                 */

#define SYS_RSTSTS_PORF_Pos              (0)                                               /*!< SYS_T::RSTSTS: PORF Position           */
#define SYS_RSTSTS_PORF_Msk              (0x1ul << SYS_RSTSTS_PORF_Pos)                    /*!< SYS_T::RSTSTS: PORF Mask               */

#define SYS_RSTSTS_PINRF_Pos             (1)                                               /*!< SYS_T::RSTSTS: PINRF Position          */
#define SYS_RSTSTS_PINRF_Msk             (0x1ul << SYS_RSTSTS_PINRF_Pos)                   /*!< SYS_T::RSTSTS: PINRF Mask              */

#define SYS_RSTSTS_WDTRF_Pos             (2)                                               /*!< SYS_T::RSTSTS: WDTRF Position          */
#define SYS_RSTSTS_WDTRF_Msk             (0x1ul << SYS_RSTSTS_WDTRF_Pos)                   /*!< SYS_T::RSTSTS: WDTRF Mask              */

#define SYS_RSTSTS_LVRF_Pos              (3)                                               /*!< SYS_T::RSTSTS: LVRF Position           */
#define SYS_RSTSTS_LVRF_Msk              (0x1ul << SYS_RSTSTS_LVRF_Pos)                    /*!< SYS_T::RSTSTS: LVRF Mask               */

#define SYS_RSTSTS_BODRF_Pos             (4)                                               /*!< SYS_T::RSTSTS: BODRF Position          */
#define SYS_RSTSTS_BODRF_Msk             (0x1ul << SYS_RSTSTS_BODRF_Pos)                   /*!< SYS_T::RSTSTS: BODRF Mask              */

#define SYS_RSTSTS_SYSRF_Pos             (5)                                               /*!< SYS_T::RSTSTS: SYSRF Position          */
#define SYS_RSTSTS_SYSRF_Msk             (0x1ul << SYS_RSTSTS_SYSRF_Pos)                   /*!< SYS_T::RSTSTS: SYSRF Mask              */

#define SYS_RSTSTS_CPURF_Pos             (7)                                               /*!< SYS_T::RSTSTS: CPURF Position          */
#define SYS_RSTSTS_CPURF_Msk             (0x1ul << SYS_RSTSTS_CPURF_Pos)                   /*!< SYS_T::RSTSTS: CPURF Mask              */

#define SYS_RSTSTS_CPULKRF_Pos           (8)                                               /*!< SYS_T::RSTSTS: CPULKRF Position        */
#define SYS_RSTSTS_CPULKRF_Msk           (0x1ul << SYS_RSTSTS_CPULKRF_Pos)                 /*!< SYS_T::RSTSTS: CPULKRF Mask            */

#define SYS_IPRST0_CHIPRST_Pos           (0)                                               /*!< SYS_T::IPRST0: CHIPRST Position        */
#define SYS_IPRST0_CHIPRST_Msk           (0x1ul << SYS_IPRST0_CHIPRST_Pos)                 /*!< SYS_T::IPRST0: CHIPRST Mask            */

#define SYS_IPRST0_CPURST_Pos            (1)                                               /*!< SYS_T::IPRST0: CPURST Position         */
#define SYS_IPRST0_CPURST_Msk            (0x1ul << SYS_IPRST0_CPURST_Pos)                  /*!< SYS_T::IPRST0: CPURST Mask             */

#define SYS_IPRST0_PDMARST_Pos           (2)                                               /*!< SYS_T::IPRST0: PDMARST Position        */
#define SYS_IPRST0_PDMARST_Msk           (0x1ul << SYS_IPRST0_PDMARST_Pos)                 /*!< SYS_T::IPRST0: PDMARST Mask            */

#define SYS_IPRST0_EBIRST_Pos            (3)                                               /*!< SYS_T::IPRST0: EBIRST Position         */
#define SYS_IPRST0_EBIRST_Msk            (0x1ul << SYS_IPRST0_EBIRST_Pos)                  /*!< SYS_T::IPRST0: EBIRST Mask             */

#define SYS_IPRST0_EMACRST_Pos           (5)                                               /*!< SYS_T::IPRST0: EMACRST Position        */
#define SYS_IPRST0_EMACRST_Msk           (0x1ul << SYS_IPRST0_EMACRST_Pos)                 /*!< SYS_T::IPRST0: EMACRST Mask            */

#define SYS_IPRST0_SDH0RST_Pos           (6)                                               /*!< SYS_T::IPRST0: SDH0RST Position        */
#define SYS_IPRST0_SDH0RST_Msk           (0x1ul << SYS_IPRST0_SDH0RST_Pos)                 /*!< SYS_T::IPRST0: SDH0RST Mask            */

#define SYS_IPRST0_CRCRST_Pos            (7)                                               /*!< SYS_T::IPRST0: CRCRST Position         */
#define SYS_IPRST0_CRCRST_Msk            (0x1ul << SYS_IPRST0_CRCRST_Pos)                  /*!< SYS_T::IPRST0: CRCRST Mask             */

#define SYS_IPRST0_CCAPRST_Pos           (8)                                               /*!< SYS_T::IPRST0: CCAPRST Position        */
#define SYS_IPRST0_CCAPRST_Msk           (0x1ul << SYS_IPRST0_CCAPRST_Pos)                 /*!< SYS_T::IPRST0: CCAPRST Mask            */

#define SYS_IPRST0_HSUSBDRST_Pos         (10)                                              /*!< SYS_T::IPRST0: HSUSBDRST Position      */
#define SYS_IPRST0_HSUSBDRST_Msk         (0x1ul << SYS_IPRST0_HSUSBDRST_Pos)               /*!< SYS_T::IPRST0: HSUSBDRST Mask          */

#define SYS_IPRST0_CRPTRST_Pos           (12)                                              /*!< SYS_T::IPRST0: CRPTRST Position        */
#define SYS_IPRST0_CRPTRST_Msk           (0x1ul << SYS_IPRST0_CRPTRST_Pos)                 /*!< SYS_T::IPRST0: CRPTRST Mask            */

#define SYS_IPRST0_SPIMRST_Pos           (14)                                              /*!< SYS_T::IPRST0: SPIMRST Position        */
#define SYS_IPRST0_SPIMRST_Msk           (0x1ul << SYS_IPRST0_SPIMRST_Pos)                 /*!< SYS_T::IPRST0: SPIMRST Mask            */

#define SYS_IPRST0_USBHRST_Pos           (16)                                              /*!< SYS_T::IPRST0: USBHRST Position        */
#define SYS_IPRST0_USBHRST_Msk           (0x1ul << SYS_IPRST0_USBHRST_Pos)                 /*!< SYS_T::IPRST0: USBHRST Mask            */

#define SYS_IPRST0_SDH1RST_Pos           (17)                                              /*!< SYS_T::IPRST0: SDH1RST Position        */
#define SYS_IPRST0_SDH1RST_Msk           (0x1ul << SYS_IPRST0_SDH1RST_Pos)                 /*!< SYS_T::IPRST0: SDH1RST Mask            */

#define SYS_IPRST1_GPIORST_Pos           (1)                                               /*!< SYS_T::IPRST1: GPIORST Position        */
#define SYS_IPRST1_GPIORST_Msk           (0x1ul << SYS_IPRST1_GPIORST_Pos)                 /*!< SYS_T::IPRST1: GPIORST Mask            */

#define SYS_IPRST1_TMR0RST_Pos           (2)                                               /*!< SYS_T::IPRST1: TMR0RST Position        */
#define SYS_IPRST1_TMR0RST_Msk           (0x1ul << SYS_IPRST1_TMR0RST_Pos)                 /*!< SYS_T::IPRST1: TMR0RST Mask            */

#define SYS_IPRST1_TMR1RST_Pos           (3)                                               /*!< SYS_T::IPRST1: TMR1RST Position        */
#define SYS_IPRST1_TMR1RST_Msk           (0x1ul << SYS_IPRST1_TMR1RST_Pos)                 /*!< SYS_T::IPRST1: TMR1RST Mask            */

#define SYS_IPRST1_TMR2RST_Pos           (4)                                               /*!< SYS_T::IPRST1: TMR2RST Position        */
#define SYS_IPRST1_TMR2RST_Msk           (0x1ul << SYS_IPRST1_TMR2RST_Pos)                 /*!< SYS_T::IPRST1: TMR2RST Mask            */

#define SYS_IPRST1_TMR3RST_Pos           (5)                                               /*!< SYS_T::IPRST1: TMR3RST Position        */
#define SYS_IPRST1_TMR3RST_Msk           (0x1ul << SYS_IPRST1_TMR3RST_Pos)                 /*!< SYS_T::IPRST1: TMR3RST Mask            */

#define SYS_IPRST1_ACMP01RST_Pos         (7)                                               /*!< SYS_T::IPRST1: ACMP01RST Position      */
#define SYS_IPRST1_ACMP01RST_Msk         (0x1ul << SYS_IPRST1_ACMP01RST_Pos)               /*!< SYS_T::IPRST1: ACMP01RST Mask          */

#define SYS_IPRST1_I2C0RST_Pos           (8)                                               /*!< SYS_T::IPRST1: I2C0RST Position        */
#define SYS_IPRST1_I2C0RST_Msk           (0x1ul << SYS_IPRST1_I2C0RST_Pos)                 /*!< SYS_T::IPRST1: I2C0RST Mask            */

#define SYS_IPRST1_I2C1RST_Pos           (9)                                               /*!< SYS_T::IPRST1: I2C1RST Position        */
#define SYS_IPRST1_I2C1RST_Msk           (0x1ul << SYS_IPRST1_I2C1RST_Pos)                 /*!< SYS_T::IPRST1: I2C1RST Mask            */

#define SYS_IPRST1_I2C2RST_Pos           (10)                                              /*!< SYS_T::IPRST1: I2C2RST Position        */
#define SYS_IPRST1_I2C2RST_Msk           (0x1ul << SYS_IPRST1_I2C2RST_Pos)                 /*!< SYS_T::IPRST1: I2C2RST Mask            */

#define SYS_IPRST1_QSPI0RST_Pos          (12)                                              /*!< SYS_T::IPRST1: QSPI0RST Position       */
#define SYS_IPRST1_QSPI0RST_Msk          (0x1ul << SYS_IPRST1_QSPI0RST_Pos)                /*!< SYS_T::IPRST1: QSPI0RST Mask           */

#define SYS_IPRST1_SPI0RST_Pos           (13)                                              /*!< SYS_T::IPRST1: SPI0RST Position        */
#define SYS_IPRST1_SPI0RST_Msk           (0x1ul << SYS_IPRST1_SPI0RST_Pos)                 /*!< SYS_T::IPRST1: SPI0RST Mask            */

#define SYS_IPRST1_SPI1RST_Pos           (14)                                              /*!< SYS_T::IPRST1: SPI1RST Position        */
#define SYS_IPRST1_SPI1RST_Msk           (0x1ul << SYS_IPRST1_SPI1RST_Pos)                 /*!< SYS_T::IPRST1: SPI1RST Mask            */

#define SYS_IPRST1_SPI2RST_Pos           (15)                                              /*!< SYS_T::IPRST1: SPI2RST Position        */
#define SYS_IPRST1_SPI2RST_Msk           (0x1ul << SYS_IPRST1_SPI2RST_Pos)                 /*!< SYS_T::IPRST1: SPI2RST Mask            */

#define SYS_IPRST1_UART0RST_Pos          (16)                                              /*!< SYS_T::IPRST1: UART0RST Position       */
#define SYS_IPRST1_UART0RST_Msk          (0x1ul << SYS_IPRST1_UART0RST_Pos)                /*!< SYS_T::IPRST1: UART0RST Mask           */

#define SYS_IPRST1_UART1RST_Pos          (17)                                              /*!< SYS_T::IPRST1: UART1RST Position       */
#define SYS_IPRST1_UART1RST_Msk          (0x1ul << SYS_IPRST1_UART1RST_Pos)                /*!< SYS_T::IPRST1: UART1RST Mask           */

#define SYS_IPRST1_UART2RST_Pos          (18)                                              /*!< SYS_T::IPRST1: UART2RST Position       */
#define SYS_IPRST1_UART2RST_Msk          (0x1ul << SYS_IPRST1_UART2RST_Pos)                /*!< SYS_T::IPRST1: UART2RST Mask           */

#define SYS_IPRST1_UART3RST_Pos          (19)                                              /*!< SYS_T::IPRST1: UART3RST Position       */
#define SYS_IPRST1_UART3RST_Msk          (0x1ul << SYS_IPRST1_UART3RST_Pos)                /*!< SYS_T::IPRST1: UART3RST Mask           */

#define SYS_IPRST1_UART4RST_Pos          (20)                                              /*!< SYS_T::IPRST1: UART4RST Position       */
#define SYS_IPRST1_UART4RST_Msk          (0x1ul << SYS_IPRST1_UART4RST_Pos)                /*!< SYS_T::IPRST1: UART4RST Mask           */

#define SYS_IPRST1_UART5RST_Pos          (21)                                              /*!< SYS_T::IPRST1: UART5RST Position       */
#define SYS_IPRST1_UART5RST_Msk          (0x1ul << SYS_IPRST1_UART5RST_Pos)                /*!< SYS_T::IPRST1: UART5RST Mask           */

#define SYS_IPRST1_UART6RST_Pos          (22)                                              /*!< SYS_T::IPRST1: UART6RST Position       */
#define SYS_IPRST1_UART6RST_Msk          (0x1ul << SYS_IPRST1_UART6RST_Pos)                /*!< SYS_T::IPRST1: UART6RST Mask           */

#define SYS_IPRST1_UART7RST_Pos          (23)                                              /*!< SYS_T::IPRST1: UART7RST Position       */
#define SYS_IPRST1_UART7RST_Msk          (0x1ul << SYS_IPRST1_UART7RST_Pos)                /*!< SYS_T::IPRST1: UART7RST Mask           */

#define SYS_IPRST1_CAN0RST_Pos           (24)                                              /*!< SYS_T::IPRST1: CAN0RST Position        */
#define SYS_IPRST1_CAN0RST_Msk           (0x1ul << SYS_IPRST1_CAN0RST_Pos)                 /*!< SYS_T::IPRST1: CAN0RST Mask            */

#define SYS_IPRST1_CAN1RST_Pos           (25)                                              /*!< SYS_T::IPRST1: CAN1RST Position        */
#define SYS_IPRST1_CAN1RST_Msk           (0x1ul << SYS_IPRST1_CAN1RST_Pos)                 /*!< SYS_T::IPRST1: CAN1RST Mask            */

#define SYS_IPRST1_OTGRST_Pos            (26)                                              /*!< SYS_T::IPRST1: OTGRST Position         */
#define SYS_IPRST1_OTGRST_Msk            (0x1ul << SYS_IPRST1_OTGRST_Pos)                  /*!< SYS_T::IPRST1: OTGRST Mask             */

#define SYS_IPRST1_USBDRST_Pos           (27)                                              /*!< SYS_T::IPRST1: USBDRST Position        */
#define SYS_IPRST1_USBDRST_Msk           (0x1ul << SYS_IPRST1_USBDRST_Pos)                 /*!< SYS_T::IPRST1: USBDRST Mask            */

#define SYS_IPRST1_EADCRST_Pos           (28)                                              /*!< SYS_T::IPRST1: EADCRST Position        */
#define SYS_IPRST1_EADCRST_Msk           (0x1ul << SYS_IPRST1_EADCRST_Pos)                 /*!< SYS_T::IPRST1: EADCRST Mask            */

#define SYS_IPRST1_I2S0RST_Pos           (29)                                              /*!< SYS_T::IPRST1: I2S0RST Position        */
#define SYS_IPRST1_I2S0RST_Msk           (0x1ul << SYS_IPRST1_I2S0RST_Pos)                 /*!< SYS_T::IPRST1: I2S0RST Mask            */

#define SYS_IPRST1_HSOTGRST_Pos          (30)                                              /*!< SYS_T::IPRST1: HSOTGRST Position       */
#define SYS_IPRST1_HSOTGRST_Msk          (0x1ul << SYS_IPRST1_HSOTGRST_Pos)                /*!< SYS_T::IPRST1: HSOTGRST Mask           */

#define SYS_IPRST1_TRNGRST_Pos           (31)                                              /*!< SYS_T::IPRST1: TRNGRST Position        */
#define SYS_IPRST1_TRNGRST_Msk           (0x1ul << SYS_IPRST1_TRNGRST_Pos)                 /*!< SYS_T::IPRST1: TRNGRST Mask            */

#define SYS_IPRST2_SC0RST_Pos            (0)                                               /*!< SYS_T::IPRST2: SC0RST Position         */
#define SYS_IPRST2_SC0RST_Msk            (0x1ul << SYS_IPRST2_SC0RST_Pos)                  /*!< SYS_T::IPRST2: SC0RST Mask             */

#define SYS_IPRST2_SC1RST_Pos            (1)                                               /*!< SYS_T::IPRST2: SC1RST Position         */
#define SYS_IPRST2_SC1RST_Msk            (0x1ul << SYS_IPRST2_SC1RST_Pos)                  /*!< SYS_T::IPRST2: SC1RST Mask             */

#define SYS_IPRST2_SC2RST_Pos            (2)                                               /*!< SYS_T::IPRST2: SC2RST Position         */
#define SYS_IPRST2_SC2RST_Msk            (0x1ul << SYS_IPRST2_SC2RST_Pos)                  /*!< SYS_T::IPRST2: SC2RST Mask             */

#define SYS_IPRST2_QSPI1RST_Pos          (4)                                               /*!< SYS_T::IPRST2: QSPI1RST Position       */
#define SYS_IPRST2_QSPI1RST_Msk          (0x1ul << SYS_IPRST2_QSPI1RST_Pos)                /*!< SYS_T::IPRST2: QSPI1RST Mask           */

#define SYS_IPRST2_SPI3RST_Pos           (6)                                               /*!< SYS_T::IPRST2: SPI3RST Position        */
#define SYS_IPRST2_SPI3RST_Msk           (0x1ul << SYS_IPRST2_SPI3RST_Pos)                 /*!< SYS_T::IPRST2: SPI3RST Mask            */

#define SYS_IPRST2_USCI0RST_Pos          (8)                                               /*!< SYS_T::IPRST2: USCI0RST Position       */
#define SYS_IPRST2_USCI0RST_Msk          (0x1ul << SYS_IPRST2_USCI0RST_Pos)                /*!< SYS_T::IPRST2: USCI0RST Mask           */

#define SYS_IPRST2_USCI1RST_Pos          (9)                                               /*!< SYS_T::IPRST2: USCI1RST Position       */
#define SYS_IPRST2_USCI1RST_Msk          (0x1ul << SYS_IPRST2_USCI1RST_Pos)                /*!< SYS_T::IPRST2: USCI1RST Mask           */

#define SYS_IPRST2_DACRST_Pos            (12)                                              /*!< SYS_T::IPRST2: DACRST Position         */
#define SYS_IPRST2_DACRST_Msk            (0x1ul << SYS_IPRST2_DACRST_Pos)                  /*!< SYS_T::IPRST2: DACRST Mask             */

#define SYS_IPRST2_EPWM0RST_Pos          (16)                                              /*!< SYS_T::IPRST2: EPWM0RST Position       */
#define SYS_IPRST2_EPWM0RST_Msk          (0x1ul << SYS_IPRST2_EPWM0RST_Pos)                /*!< SYS_T::IPRST2: EPWM0RST Mask           */

#define SYS_IPRST2_EPWM1RST_Pos          (17)                                              /*!< SYS_T::IPRST2: EPWM1RST Position       */
#define SYS_IPRST2_EPWM1RST_Msk          (0x1ul << SYS_IPRST2_EPWM1RST_Pos)                /*!< SYS_T::IPRST2: EPWM1RST Mask           */

#define SYS_IPRST2_BPWM0RST_Pos          (18)                                              /*!< SYS_T::IPRST2: BPWM0RST Position       */
#define SYS_IPRST2_BPWM0RST_Msk          (0x1ul << SYS_IPRST2_BPWM0RST_Pos)                /*!< SYS_T::IPRST2: BPWM0RST Mask           */

#define SYS_IPRST2_BPWM1RST_Pos          (19)                                              /*!< SYS_T::IPRST2: BPWM1RST Position       */
#define SYS_IPRST2_BPWM1RST_Msk          (0x1ul << SYS_IPRST2_BPWM1RST_Pos)                /*!< SYS_T::IPRST2: BPWM1RST Mask           */

#define SYS_IPRST2_QEI0RST_Pos           (22)                                              /*!< SYS_T::IPRST2: QEI0RST Position        */
#define SYS_IPRST2_QEI0RST_Msk           (0x1ul << SYS_IPRST2_QEI0RST_Pos)                 /*!< SYS_T::IPRST2: QEI0RST Mask            */

#define SYS_IPRST2_QEI1RST_Pos           (23)                                              /*!< SYS_T::IPRST2: QEI1RST Position        */
#define SYS_IPRST2_QEI1RST_Msk           (0x1ul << SYS_IPRST2_QEI1RST_Pos)                 /*!< SYS_T::IPRST2: QEI1RST Mask            */

#define SYS_IPRST2_ECAP0RST_Pos          (26)                                              /*!< SYS_T::IPRST2: ECAP0RST Position       */
#define SYS_IPRST2_ECAP0RST_Msk          (0x1ul << SYS_IPRST2_ECAP0RST_Pos)                /*!< SYS_T::IPRST2: ECAP0RST Mask           */

#define SYS_IPRST2_ECAP1RST_Pos          (27)                                              /*!< SYS_T::IPRST2: ECAP1RST Position       */
#define SYS_IPRST2_ECAP1RST_Msk          (0x1ul << SYS_IPRST2_ECAP1RST_Pos)                /*!< SYS_T::IPRST2: ECAP1RST Mask           */

#define SYS_IPRST2_CAN2RST_Pos           (28)                                              /*!< SYS_T::IPRST2: CAN2RST Position        */
#define SYS_IPRST2_CAN2RST_Msk           (0x1ul << SYS_IPRST2_CAN2RST_Pos)                 /*!< SYS_T::IPRST2: CAN2RST Mask            */

#define SYS_IPRST2_OPARST_Pos            (30)                                              /*!< SYS_T::IPRST2: OPARST Position         */
#define SYS_IPRST2_OPARST_Msk            (0x1ul << SYS_IPRST2_OPARST_Pos)                  /*!< SYS_T::IPRST2: OPARST Mask             */

#define SYS_IPRST2_EADC1RST_Pos          (31)                                              /*!< SYS_T::IPRST2: EADC1RST Position       */
#define SYS_IPRST2_EADC1RST_Msk          (0x1ul << SYS_IPRST2_EADC1RST_Pos)                /*!< SYS_T::IPRST2: EADC1RST Mask           */

#define SYS_BODCTL_BODEN_Pos             (0)                                               /*!< SYS_T::BODCTL: BODEN Position          */
#define SYS_BODCTL_BODEN_Msk             (0x1ul << SYS_BODCTL_BODEN_Pos)                   /*!< SYS_T::BODCTL: BODEN Mask              */

#define SYS_BODCTL_BODRSTEN_Pos          (3)                                               /*!< SYS_T::BODCTL: BODRSTEN Position       */
#define SYS_BODCTL_BODRSTEN_Msk          (0x1ul << SYS_BODCTL_BODRSTEN_Pos)                /*!< SYS_T::BODCTL: BODRSTEN Mask           */

#define SYS_BODCTL_BODIF_Pos             (4)                                               /*!< SYS_T::BODCTL: BODIF Position          */
#define SYS_BODCTL_BODIF_Msk             (0x1ul << SYS_BODCTL_BODIF_Pos)                   /*!< SYS_T::BODCTL: BODIF Mask              */

#define SYS_BODCTL_BODLPM_Pos            (5)                                               /*!< SYS_T::BODCTL: BODLPM Position         */
#define SYS_BODCTL_BODLPM_Msk            (0x1ul << SYS_BODCTL_BODLPM_Pos)                  /*!< SYS_T::BODCTL: BODLPM Mask             */

#define SYS_BODCTL_BODOUT_Pos            (6)                                               /*!< SYS_T::BODCTL: BODOUT Position         */
#define SYS_BODCTL_BODOUT_Msk            (0x1ul << SYS_BODCTL_BODOUT_Pos)                  /*!< SYS_T::BODCTL: BODOUT Mask             */

#define SYS_BODCTL_LVREN_Pos             (7)                                               /*!< SYS_T::BODCTL: LVREN Position          */
#define SYS_BODCTL_LVREN_Msk             (0x1ul << SYS_BODCTL_LVREN_Pos)                   /*!< SYS_T::BODCTL: LVREN Mask              */

#define SYS_BODCTL_BODDGSEL_Pos          (8)                                               /*!< SYS_T::BODCTL: BODDGSEL Position       */
#define SYS_BODCTL_BODDGSEL_Msk          (0x7ul << SYS_BODCTL_BODDGSEL_Pos)                /*!< SYS_T::BODCTL: BODDGSEL Mask           */

#define SYS_BODCTL_LVRDGSEL_Pos          (12)                                              /*!< SYS_T::BODCTL: LVRDGSEL Position       */
#define SYS_BODCTL_LVRDGSEL_Msk          (0x7ul << SYS_BODCTL_LVRDGSEL_Pos)                /*!< SYS_T::BODCTL: LVRDGSEL Mask           */

#define SYS_BODCTL_BODVL_Pos             (16)                                              /*!< SYS_T::BODCTL: BODVL Position          */
#define SYS_BODCTL_BODVL_Msk             (0x7ul << SYS_BODCTL_BODVL_Pos)                   /*!< SYS_T::BODCTL: BODVL Mask              */

#define SYS_IVSCTL_VTEMPEN_Pos           (0)                                               /*!< SYS_T::IVSCTL: VTEMPEN Position        */
#define SYS_IVSCTL_VTEMPEN_Msk           (0x1ul << SYS_IVSCTL_VTEMPEN_Pos)                 /*!< SYS_T::IVSCTL: VTEMPEN Mask            */

#define SYS_IVSCTL_VBATUGEN_Pos          (1)                                               /*!< SYS_T::IVSCTL: VBATUGEN Position       */
#define SYS_IVSCTL_VBATUGEN_Msk          (0x1ul << SYS_IVSCTL_VBATUGEN_Pos)                /*!< SYS_T::IVSCTL: VBATUGEN Mask           */

#define SYS_PORCTL_POROFF_Pos            (0)                                               /*!< SYS_T::PORCTL: POROFF Position         */
#define SYS_PORCTL_POROFF_Msk            (0xfffful << SYS_PORCTL_POROFF_Pos)               /*!< SYS_T::PORCTL: POROFF Mask             */

#define SYS_VREFCTL_VREFCTL_Pos          (0)                                               /*!< SYS_T::VREFCTL: VREFCTL Position       */
#define SYS_VREFCTL_VREFCTL_Msk          (0x1ful << SYS_VREFCTL_VREFCTL_Pos)               /*!< SYS_T::VREFCTL: VREFCTL Mask           */

#define SYS_VREFCTL_PRELOAD_SEL_Pos      (6)                                               /*!< SYS_T::VREFCTL: PRELOAD_SEL Position   */
#define SYS_VREFCTL_PRELOAD_SEL_Msk      (0x3ul << SYS_VREFCTL_PRELOAD_SEL_Pos)            /*!< SYS_T::VREFCTL: PRELOAD_SEL Mask       */

#define SYS_USBPHY_USBROLE_Pos           (0)                                               /*!< SYS_T::USBPHY: USBROLE Position        */
#define SYS_USBPHY_USBROLE_Msk           (0x3ul << SYS_USBPHY_USBROLE_Pos)                 /*!< SYS_T::USBPHY: USBROLE Mask            */

#define SYS_USBPHY_SBO_Pos               (2)                                               /*!< SYS_T::USBPHY: SBO Position            */
#define SYS_USBPHY_SBO_Msk               (0x1ul << SYS_USBPHY_SBO_Pos)                     /*!< SYS_T::USBPHY: SBO Mask                */

#define SYS_USBPHY_USBEN_Pos             (8)                                               /*!< SYS_T::USBPHY: USBEN Position          */
#define SYS_USBPHY_USBEN_Msk             (0x1ul << SYS_USBPHY_USBEN_Pos)                   /*!< SYS_T::USBPHY: USBEN Mask              */

#define SYS_USBPHY_HSUSBROLE_Pos         (16)                                              /*!< SYS_T::USBPHY: HSUSBROLE Position      */
#define SYS_USBPHY_HSUSBROLE_Msk         (0x3ul << SYS_USBPHY_HSUSBROLE_Pos)               /*!< SYS_T::USBPHY: HSUSBROLE Mask          */

#define SYS_USBPHY_HSUSBEN_Pos           (24)                                              /*!< SYS_T::USBPHY: HSUSBEN Position        */
#define SYS_USBPHY_HSUSBEN_Msk           (0x1ul << SYS_USBPHY_HSUSBEN_Pos)                 /*!< SYS_T::USBPHY: HSUSBEN Mask            */

#define SYS_USBPHY_HSUSBACT_Pos          (25)                                              /*!< SYS_T::USBPHY: HSUSBACT Position       */
#define SYS_USBPHY_HSUSBACT_Msk          (0x1ul << SYS_USBPHY_HSUSBACT_Pos)                /*!< SYS_T::USBPHY: HSUSBACT Mask           */

#define SYS_GPA_MFPL_PA0MFP_Pos          (0)                                               /*!< SYS_T::GPA_MFPL: PA0MFP Position       */
#define SYS_GPA_MFPL_PA0MFP_Msk          (0xful << SYS_GPA_MFPL_PA0MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA0MFP Mask           */

#define SYS_GPA_MFPL_PA1MFP_Pos          (4)                                               /*!< SYS_T::GPA_MFPL: PA1MFP Position       */
#define SYS_GPA_MFPL_PA1MFP_Msk          (0xful << SYS_GPA_MFPL_PA1MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA1MFP Mask           */

#define SYS_GPA_MFPL_PA2MFP_Pos          (8)                                               /*!< SYS_T::GPA_MFPL: PA2MFP Position       */
#define SYS_GPA_MFPL_PA2MFP_Msk          (0xful << SYS_GPA_MFPL_PA2MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA2MFP Mask           */

#define SYS_GPA_MFPL_PA3MFP_Pos          (12)                                              /*!< SYS_T::GPA_MFPL: PA3MFP Position       */
#define SYS_GPA_MFPL_PA3MFP_Msk          (0xful << SYS_GPA_MFPL_PA3MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA3MFP Mask           */

#define SYS_GPA_MFPL_PA4MFP_Pos          (16)                                              /*!< SYS_T::GPA_MFPL: PA4MFP Position       */
#define SYS_GPA_MFPL_PA4MFP_Msk          (0xful << SYS_GPA_MFPL_PA4MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA4MFP Mask           */

#define SYS_GPA_MFPL_PA5MFP_Pos          (20)                                              /*!< SYS_T::GPA_MFPL: PA5MFP Position       */
#define SYS_GPA_MFPL_PA5MFP_Msk          (0xful << SYS_GPA_MFPL_PA5MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA5MFP Mask           */

#define SYS_GPA_MFPL_PA6MFP_Pos          (24)                                              /*!< SYS_T::GPA_MFPL: PA6MFP Position       */
#define SYS_GPA_MFPL_PA6MFP_Msk          (0xful << SYS_GPA_MFPL_PA6MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA6MFP Mask           */

#define SYS_GPA_MFPL_PA7MFP_Pos          (28)                                              /*!< SYS_T::GPA_MFPL: PA7MFP Position       */
#define SYS_GPA_MFPL_PA7MFP_Msk          (0xful << SYS_GPA_MFPL_PA7MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA7MFP Mask           */

#define SYS_GPA_MFPH_PA8MFP_Pos          (0)                                               /*!< SYS_T::GPA_MFPH: PA8MFP Position       */
#define SYS_GPA_MFPH_PA8MFP_Msk          (0xful << SYS_GPA_MFPH_PA8MFP_Pos)                /*!< SYS_T::GPA_MFPH: PA8MFP Mask           */

#define SYS_GPA_MFPH_PA9MFP_Pos          (4)                                               /*!< SYS_T::GPA_MFPH: PA9MFP Position       */
#define SYS_GPA_MFPH_PA9MFP_Msk          (0xful << SYS_GPA_MFPH_PA9MFP_Pos)                /*!< SYS_T::GPA_MFPH: PA9MFP Mask           */

#define SYS_GPA_MFPH_PA10MFP_Pos         (8)                                               /*!< SYS_T::GPA_MFPH: PA10MFP Position      */
#define SYS_GPA_MFPH_PA10MFP_Msk         (0xful << SYS_GPA_MFPH_PA10MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA10MFP Mask          */

#define SYS_GPA_MFPH_PA11MFP_Pos         (12)                                              /*!< SYS_T::GPA_MFPH: PA11MFP Position      */
#define SYS_GPA_MFPH_PA11MFP_Msk         (0xful << SYS_GPA_MFPH_PA11MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA11MFP Mask          */

#define SYS_GPA_MFPH_PA12MFP_Pos         (16)                                              /*!< SYS_T::GPA_MFPH: PA12MFP Position      */
#define SYS_GPA_MFPH_PA12MFP_Msk         (0xful << SYS_GPA_MFPH_PA12MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA12MFP Mask          */

#define SYS_GPA_MFPH_PA13MFP_Pos         (20)                                              /*!< SYS_T::GPA_MFPH: PA13MFP Position      */
#define SYS_GPA_MFPH_PA13MFP_Msk         (0xful << SYS_GPA_MFPH_PA13MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA13MFP Mask          */

#define SYS_GPA_MFPH_PA14MFP_Pos         (24)                                              /*!< SYS_T::GPA_MFPH: PA14MFP Position      */
#define SYS_GPA_MFPH_PA14MFP_Msk         (0xful << SYS_GPA_MFPH_PA14MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA14MFP Mask          */

#define SYS_GPA_MFPH_PA15MFP_Pos         (28)                                              /*!< SYS_T::GPA_MFPH: PA15MFP Position      */
#define SYS_GPA_MFPH_PA15MFP_Msk         (0xful << SYS_GPA_MFPH_PA15MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA15MFP Mask          */

#define SYS_GPB_MFPL_PB0MFP_Pos          (0)                                               /*!< SYS_T::GPB_MFPL: PB0MFP Position       */
#define SYS_GPB_MFPL_PB0MFP_Msk          (0xful << SYS_GPB_MFPL_PB0MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB0MFP Mask           */

#define SYS_GPB_MFPL_PB1MFP_Pos          (4)                                               /*!< SYS_T::GPB_MFPL: PB1MFP Position       */
#define SYS_GPB_MFPL_PB1MFP_Msk          (0xful << SYS_GPB_MFPL_PB1MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB1MFP Mask           */

#define SYS_GPB_MFPL_PB2MFP_Pos          (8)                                               /*!< SYS_T::GPB_MFPL: PB2MFP Position       */
#define SYS_GPB_MFPL_PB2MFP_Msk          (0xful << SYS_GPB_MFPL_PB2MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB2MFP Mask           */

#define SYS_GPB_MFPL_PB3MFP_Pos          (12)                                              /*!< SYS_T::GPB_MFPL: PB3MFP Position       */
#define SYS_GPB_MFPL_PB3MFP_Msk          (0xful << SYS_GPB_MFPL_PB3MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB3MFP Mask           */

#define SYS_GPB_MFPL_PB4MFP_Pos          (16)                                              /*!< SYS_T::GPB_MFPL: PB4MFP Position       */
#define SYS_GPB_MFPL_PB4MFP_Msk          (0xful << SYS_GPB_MFPL_PB4MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB4MFP Mask           */

#define SYS_GPB_MFPL_PB5MFP_Pos          (20)                                              /*!< SYS_T::GPB_MFPL: PB5MFP Position       */
#define SYS_GPB_MFPL_PB5MFP_Msk          (0xful << SYS_GPB_MFPL_PB5MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB5MFP Mask           */

#define SYS_GPB_MFPL_PB6MFP_Pos          (24)                                              /*!< SYS_T::GPB_MFPL: PB6MFP Position       */
#define SYS_GPB_MFPL_PB6MFP_Msk          (0xful << SYS_GPB_MFPL_PB6MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB6MFP Mask           */

#define SYS_GPB_MFPL_PB7MFP_Pos          (28)                                              /*!< SYS_T::GPB_MFPL: PB7MFP Position       */
#define SYS_GPB_MFPL_PB7MFP_Msk          (0xful << SYS_GPB_MFPL_PB7MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB7MFP Mask           */

#define SYS_GPB_MFPH_PB8MFP_Pos          (0)                                               /*!< SYS_T::GPB_MFPH: PB8MFP Position       */
#define SYS_GPB_MFPH_PB8MFP_Msk          (0xful << SYS_GPB_MFPH_PB8MFP_Pos)                /*!< SYS_T::GPB_MFPH: PB8MFP Mask           */

#define SYS_GPB_MFPH_PB9MFP_Pos          (4)                                               /*!< SYS_T::GPB_MFPH: PB9MFP Position       */
#define SYS_GPB_MFPH_PB9MFP_Msk          (0xful << SYS_GPB_MFPH_PB9MFP_Pos)                /*!< SYS_T::GPB_MFPH: PB9MFP Mask           */

#define SYS_GPB_MFPH_PB10MFP_Pos         (8)                                               /*!< SYS_T::GPB_MFPH: PB10MFP Position      */
#define SYS_GPB_MFPH_PB10MFP_Msk         (0xful << SYS_GPB_MFPH_PB10MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB10MFP Mask          */

#define SYS_GPB_MFPH_PB11MFP_Pos         (12)                                              /*!< SYS_T::GPB_MFPH: PB11MFP Position      */
#define SYS_GPB_MFPH_PB11MFP_Msk         (0xful << SYS_GPB_MFPH_PB11MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB11MFP Mask          */

#define SYS_GPB_MFPH_PB12MFP_Pos         (16)                                              /*!< SYS_T::GPB_MFPH: PB12MFP Position      */
#define SYS_GPB_MFPH_PB12MFP_Msk         (0xful << SYS_GPB_MFPH_PB12MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB12MFP Mask          */

#define SYS_GPB_MFPH_PB13MFP_Pos         (20)                                              /*!< SYS_T::GPB_MFPH: PB13MFP Position      */
#define SYS_GPB_MFPH_PB13MFP_Msk         (0xful << SYS_GPB_MFPH_PB13MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB13MFP Mask          */

#define SYS_GPB_MFPH_PB14MFP_Pos         (24)                                              /*!< SYS_T::GPB_MFPH: PB14MFP Position      */
#define SYS_GPB_MFPH_PB14MFP_Msk         (0xful << SYS_GPB_MFPH_PB14MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB14MFP Mask          */

#define SYS_GPB_MFPH_PB15MFP_Pos         (28)                                              /*!< SYS_T::GPB_MFPH: PB15MFP Position      */
#define SYS_GPB_MFPH_PB15MFP_Msk         (0xful << SYS_GPB_MFPH_PB15MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB15MFP Mask          */

#define SYS_GPC_MFPL_PC0MFP_Pos          (0)                                               /*!< SYS_T::GPC_MFPL: PC0MFP Position       */
#define SYS_GPC_MFPL_PC0MFP_Msk          (0xful << SYS_GPC_MFPL_PC0MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC0MFP Mask           */

#define SYS_GPC_MFPL_PC1MFP_Pos          (4)                                               /*!< SYS_T::GPC_MFPL: PC1MFP Position       */
#define SYS_GPC_MFPL_PC1MFP_Msk          (0xful << SYS_GPC_MFPL_PC1MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC1MFP Mask           */

#define SYS_GPC_MFPL_PC2MFP_Pos          (8)                                               /*!< SYS_T::GPC_MFPL: PC2MFP Position       */
#define SYS_GPC_MFPL_PC2MFP_Msk          (0xful << SYS_GPC_MFPL_PC2MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC2MFP Mask           */

#define SYS_GPC_MFPL_PC3MFP_Pos          (12)                                              /*!< SYS_T::GPC_MFPL: PC3MFP Position       */
#define SYS_GPC_MFPL_PC3MFP_Msk          (0xful << SYS_GPC_MFPL_PC3MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC3MFP Mask           */

#define SYS_GPC_MFPL_PC4MFP_Pos          (16)                                              /*!< SYS_T::GPC_MFPL: PC4MFP Position       */
#define SYS_GPC_MFPL_PC4MFP_Msk          (0xful << SYS_GPC_MFPL_PC4MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC4MFP Mask           */

#define SYS_GPC_MFPL_PC5MFP_Pos          (20)                                              /*!< SYS_T::GPC_MFPL: PC5MFP Position       */
#define SYS_GPC_MFPL_PC5MFP_Msk          (0xful << SYS_GPC_MFPL_PC5MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC5MFP Mask           */

#define SYS_GPC_MFPL_PC6MFP_Pos          (24)                                              /*!< SYS_T::GPC_MFPL: PC6MFP Position       */
#define SYS_GPC_MFPL_PC6MFP_Msk          (0xful << SYS_GPC_MFPL_PC6MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC6MFP Mask           */

#define SYS_GPC_MFPL_PC7MFP_Pos          (28)                                              /*!< SYS_T::GPC_MFPL: PC7MFP Position       */
#define SYS_GPC_MFPL_PC7MFP_Msk          (0xful << SYS_GPC_MFPL_PC7MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC7MFP Mask           */

#define SYS_GPC_MFPH_PC8MFP_Pos          (0)                                               /*!< SYS_T::GPC_MFPH: PC8MFP Position       */
#define SYS_GPC_MFPH_PC8MFP_Msk          (0xful << SYS_GPC_MFPH_PC8MFP_Pos)                /*!< SYS_T::GPC_MFPH: PC8MFP Mask           */

#define SYS_GPC_MFPH_PC9MFP_Pos          (4)                                               /*!< SYS_T::GPC_MFPH: PC9MFP Position       */
#define SYS_GPC_MFPH_PC9MFP_Msk          (0xful << SYS_GPC_MFPH_PC9MFP_Pos)                /*!< SYS_T::GPC_MFPH: PC9MFP Mask           */

#define SYS_GPC_MFPH_PC10MFP_Pos         (8)                                               /*!< SYS_T::GPC_MFPH: PC10MFP Position      */
#define SYS_GPC_MFPH_PC10MFP_Msk         (0xful << SYS_GPC_MFPH_PC10MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC10MFP Mask          */

#define SYS_GPC_MFPH_PC11MFP_Pos         (12)                                              /*!< SYS_T::GPC_MFPH: PC11MFP Position      */
#define SYS_GPC_MFPH_PC11MFP_Msk         (0xful << SYS_GPC_MFPH_PC11MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC11MFP Mask          */

#define SYS_GPC_MFPH_PC12MFP_Pos         (16)                                              /*!< SYS_T::GPC_MFPH: PC12MFP Position      */
#define SYS_GPC_MFPH_PC12MFP_Msk         (0xful << SYS_GPC_MFPH_PC12MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC12MFP Mask          */

#define SYS_GPC_MFPH_PC13MFP_Pos         (20)                                              /*!< SYS_T::GPC_MFPH: PC13MFP Position      */
#define SYS_GPC_MFPH_PC13MFP_Msk         (0xful << SYS_GPC_MFPH_PC13MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC13MFP Mask          */

#define SYS_GPC_MFPH_PC14MFP_Pos         (24)                                              /*!< SYS_T::GPC_MFPH: PC14MFP Position      */
#define SYS_GPC_MFPH_PC14MFP_Msk         (0xful << SYS_GPC_MFPH_PC14MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC14MFP Mask          */

#define SYS_GPC_MFPH_PC15MFP_Pos         (28)                                              /*!< SYS_T::GPC_MFPH: PC15MFP Position      */
#define SYS_GPC_MFPH_PC15MFP_Msk         (0xful << SYS_GPC_MFPH_PC15MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC15MFP Mask          */

#define SYS_GPD_MFPL_PD0MFP_Pos          (0)                                               /*!< SYS_T::GPD_MFPL: PD0MFP Position       */
#define SYS_GPD_MFPL_PD0MFP_Msk          (0xful << SYS_GPD_MFPL_PD0MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD0MFP Mask           */

#define SYS_GPD_MFPL_PD1MFP_Pos          (4)                                               /*!< SYS_T::GPD_MFPL: PD1MFP Position       */
#define SYS_GPD_MFPL_PD1MFP_Msk          (0xful << SYS_GPD_MFPL_PD1MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD1MFP Mask           */

#define SYS_GPD_MFPL_PD2MFP_Pos          (8)                                               /*!< SYS_T::GPD_MFPL: PD2MFP Position       */
#define SYS_GPD_MFPL_PD2MFP_Msk          (0xful << SYS_GPD_MFPL_PD2MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD2MFP Mask           */

#define SYS_GPD_MFPL_PD3MFP_Pos          (12)                                              /*!< SYS_T::GPD_MFPL: PD3MFP Position       */
#define SYS_GPD_MFPL_PD3MFP_Msk          (0xful << SYS_GPD_MFPL_PD3MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD3MFP Mask           */

#define SYS_GPD_MFPL_PD4MFP_Pos          (16)                                              /*!< SYS_T::GPD_MFPL: PD4MFP Position       */
#define SYS_GPD_MFPL_PD4MFP_Msk          (0xful << SYS_GPD_MFPL_PD4MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD4MFP Mask           */

#define SYS_GPD_MFPL_PD5MFP_Pos          (20)                                              /*!< SYS_T::GPD_MFPL: PD5MFP Position       */
#define SYS_GPD_MFPL_PD5MFP_Msk          (0xful << SYS_GPD_MFPL_PD5MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD5MFP Mask           */

#define SYS_GPD_MFPL_PD6MFP_Pos          (24)                                              /*!< SYS_T::GPD_MFPL: PD6MFP Position       */
#define SYS_GPD_MFPL_PD6MFP_Msk          (0xful << SYS_GPD_MFPL_PD6MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD6MFP Mask           */

#define SYS_GPD_MFPL_PD7MFP_Pos          (28)                                              /*!< SYS_T::GPD_MFPL: PD7MFP Position       */
#define SYS_GPD_MFPL_PD7MFP_Msk          (0xful << SYS_GPD_MFPL_PD7MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD7MFP Mask           */

#define SYS_GPD_MFPH_PD8MFP_Pos          (0)                                               /*!< SYS_T::GPD_MFPH: PD8MFP Position       */
#define SYS_GPD_MFPH_PD8MFP_Msk          (0xful << SYS_GPD_MFPH_PD8MFP_Pos)                /*!< SYS_T::GPD_MFPH: PD8MFP Mask           */

#define SYS_GPD_MFPH_PD9MFP_Pos          (4)                                               /*!< SYS_T::GPD_MFPH: PD9MFP Position       */
#define SYS_GPD_MFPH_PD9MFP_Msk          (0xful << SYS_GPD_MFPH_PD9MFP_Pos)                /*!< SYS_T::GPD_MFPH: PD9MFP Mask           */

#define SYS_GPD_MFPH_PD10MFP_Pos         (8)                                               /*!< SYS_T::GPD_MFPH: PD10MFP Position      */
#define SYS_GPD_MFPH_PD10MFP_Msk         (0xful << SYS_GPD_MFPH_PD10MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD10MFP Mask          */

#define SYS_GPD_MFPH_PD11MFP_Pos         (12)                                              /*!< SYS_T::GPD_MFPH: PD11MFP Position      */
#define SYS_GPD_MFPH_PD11MFP_Msk         (0xful << SYS_GPD_MFPH_PD11MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD11MFP Mask          */

#define SYS_GPD_MFPH_PD12MFP_Pos         (16)                                              /*!< SYS_T::GPD_MFPH: PD12MFP Position      */
#define SYS_GPD_MFPH_PD12MFP_Msk         (0xful << SYS_GPD_MFPH_PD12MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD12MFP Mask          */

#define SYS_GPD_MFPH_PD13MFP_Pos         (20)                                              /*!< SYS_T::GPD_MFPH: PD13MFP Position      */
#define SYS_GPD_MFPH_PD13MFP_Msk         (0xful << SYS_GPD_MFPH_PD13MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD13MFP Mask          */

#define SYS_GPD_MFPH_PD14MFP_Pos         (24)                                              /*!< SYS_T::GPD_MFPH: PD14MFP Position      */
#define SYS_GPD_MFPH_PD14MFP_Msk         (0xful << SYS_GPD_MFPH_PD14MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD14MFP Mask          */

#define SYS_GPD_MFPH_PD15MFP_Pos         (28)                                              /*!< SYS_T::GPD_MFPH: PD15MFP Position      */
#define SYS_GPD_MFPH_PD15MFP_Msk         (0xful << SYS_GPD_MFPH_PD15MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD15MFP Mask          */

#define SYS_GPE_MFPL_PE0MFP_Pos          (0)                                               /*!< SYS_T::GPE_MFPL: PE0MFP Position       */
#define SYS_GPE_MFPL_PE0MFP_Msk          (0xful << SYS_GPE_MFPL_PE0MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE0MFP Mask           */

#define SYS_GPE_MFPL_PE1MFP_Pos          (4)                                               /*!< SYS_T::GPE_MFPL: PE1MFP Position       */
#define SYS_GPE_MFPL_PE1MFP_Msk          (0xful << SYS_GPE_MFPL_PE1MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE1MFP Mask           */

#define SYS_GPE_MFPL_PE2MFP_Pos          (8)                                               /*!< SYS_T::GPE_MFPL: PE2MFP Position       */
#define SYS_GPE_MFPL_PE2MFP_Msk          (0xful << SYS_GPE_MFPL_PE2MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE2MFP Mask           */

#define SYS_GPE_MFPL_PE3MFP_Pos          (12)                                              /*!< SYS_T::GPE_MFPL: PE3MFP Position       */
#define SYS_GPE_MFPL_PE3MFP_Msk          (0xful << SYS_GPE_MFPL_PE3MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE3MFP Mask           */

#define SYS_GPE_MFPL_PE4MFP_Pos          (16)                                              /*!< SYS_T::GPE_MFPL: PE4MFP Position       */
#define SYS_GPE_MFPL_PE4MFP_Msk          (0xful << SYS_GPE_MFPL_PE4MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE4MFP Mask           */

#define SYS_GPE_MFPL_PE5MFP_Pos          (20)                                              /*!< SYS_T::GPE_MFPL: PE5MFP Position       */
#define SYS_GPE_MFPL_PE5MFP_Msk          (0xful << SYS_GPE_MFPL_PE5MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE5MFP Mask           */

#define SYS_GPE_MFPL_PE6MFP_Pos          (24)                                              /*!< SYS_T::GPE_MFPL: PE6MFP Position       */
#define SYS_GPE_MFPL_PE6MFP_Msk          (0xful << SYS_GPE_MFPL_PE6MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE6MFP Mask           */

#define SYS_GPE_MFPL_PE7MFP_Pos          (28)                                              /*!< SYS_T::GPE_MFPL: PE7MFP Position       */
#define SYS_GPE_MFPL_PE7MFP_Msk          (0xful << SYS_GPE_MFPL_PE7MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE7MFP Mask           */

#define SYS_GPE_MFPH_PE8MFP_Pos          (0)                                               /*!< SYS_T::GPE_MFPH: PE8MFP Position       */
#define SYS_GPE_MFPH_PE8MFP_Msk          (0xful << SYS_GPE_MFPH_PE8MFP_Pos)                /*!< SYS_T::GPE_MFPH: PE8MFP Mask           */

#define SYS_GPE_MFPH_PE9MFP_Pos          (4)                                               /*!< SYS_T::GPE_MFPH: PE9MFP Position       */
#define SYS_GPE_MFPH_PE9MFP_Msk          (0xful << SYS_GPE_MFPH_PE9MFP_Pos)                /*!< SYS_T::GPE_MFPH: PE9MFP Mask           */

#define SYS_GPE_MFPH_PE10MFP_Pos         (8)                                               /*!< SYS_T::GPE_MFPH: PE10MFP Position      */
#define SYS_GPE_MFPH_PE10MFP_Msk         (0xful << SYS_GPE_MFPH_PE10MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE10MFP Mask          */

#define SYS_GPE_MFPH_PE11MFP_Pos         (12)                                              /*!< SYS_T::GPE_MFPH: PE11MFP Position      */
#define SYS_GPE_MFPH_PE11MFP_Msk         (0xful << SYS_GPE_MFPH_PE11MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE11MFP Mask          */

#define SYS_GPE_MFPH_PE12MFP_Pos         (16)                                              /*!< SYS_T::GPE_MFPH: PE12MFP Position      */
#define SYS_GPE_MFPH_PE12MFP_Msk         (0xful << SYS_GPE_MFPH_PE12MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE12MFP Mask          */

#define SYS_GPE_MFPH_PE13MFP_Pos         (20)                                              /*!< SYS_T::GPE_MFPH: PE13MFP Position      */
#define SYS_GPE_MFPH_PE13MFP_Msk         (0xful << SYS_GPE_MFPH_PE13MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE13MFP Mask          */

#define SYS_GPE_MFPH_PE14MFP_Pos         (24)                                              /*!< SYS_T::GPE_MFPH: PE14MFP Position      */
#define SYS_GPE_MFPH_PE14MFP_Msk         (0xful << SYS_GPE_MFPH_PE14MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE14MFP Mask          */

#define SYS_GPE_MFPH_PE15MFP_Pos         (28)                                              /*!< SYS_T::GPE_MFPH: PE15MFP Position      */
#define SYS_GPE_MFPH_PE15MFP_Msk         (0xful << SYS_GPE_MFPH_PE15MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE15MFP Mask          */

#define SYS_GPF_MFPL_PF0MFP_Pos          (0)                                               /*!< SYS_T::GPF_MFPL: PF0MFP Position       */
#define SYS_GPF_MFPL_PF0MFP_Msk          (0xful << SYS_GPF_MFPL_PF0MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF0MFP Mask           */

#define SYS_GPF_MFPL_PF1MFP_Pos          (4)                                               /*!< SYS_T::GPF_MFPL: PF1MFP Position       */
#define SYS_GPF_MFPL_PF1MFP_Msk          (0xful << SYS_GPF_MFPL_PF1MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF1MFP Mask           */

#define SYS_GPF_MFPL_PF2MFP_Pos          (8)                                               /*!< SYS_T::GPF_MFPL: PF2MFP Position       */
#define SYS_GPF_MFPL_PF2MFP_Msk          (0xful << SYS_GPF_MFPL_PF2MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF2MFP Mask           */

#define SYS_GPF_MFPL_PF3MFP_Pos          (12)                                              /*!< SYS_T::GPF_MFPL: PF3MFP Position       */
#define SYS_GPF_MFPL_PF3MFP_Msk          (0xful << SYS_GPF_MFPL_PF3MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF3MFP Mask           */

#define SYS_GPF_MFPL_PF4MFP_Pos          (16)                                              /*!< SYS_T::GPF_MFPL: PF4MFP Position       */
#define SYS_GPF_MFPL_PF4MFP_Msk          (0xful << SYS_GPF_MFPL_PF4MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF4MFP Mask           */

#define SYS_GPF_MFPL_PF5MFP_Pos          (20)                                              /*!< SYS_T::GPF_MFPL: PF5MFP Position       */
#define SYS_GPF_MFPL_PF5MFP_Msk          (0xful << SYS_GPF_MFPL_PF5MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF5MFP Mask           */

#define SYS_GPF_MFPL_PF6MFP_Pos          (24)                                              /*!< SYS_T::GPF_MFPL: PF6MFP Position       */
#define SYS_GPF_MFPL_PF6MFP_Msk          (0xful << SYS_GPF_MFPL_PF6MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF6MFP Mask           */

#define SYS_GPF_MFPL_PF7MFP_Pos          (28)                                              /*!< SYS_T::GPF_MFPL: PF7MFP Position       */
#define SYS_GPF_MFPL_PF7MFP_Msk          (0xful << SYS_GPF_MFPL_PF7MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF7MFP Mask           */

#define SYS_GPF_MFPH_PF8MFP_Pos          (0)                                               /*!< SYS_T::GPF_MFPH: PF8MFP Position       */
#define SYS_GPF_MFPH_PF8MFP_Msk          (0xful << SYS_GPF_MFPH_PF8MFP_Pos)                /*!< SYS_T::GPF_MFPH: PF8MFP Mask           */

#define SYS_GPF_MFPH_PF9MFP_Pos          (4)                                               /*!< SYS_T::GPF_MFPH: PF9MFP Position       */
#define SYS_GPF_MFPH_PF9MFP_Msk          (0xful << SYS_GPF_MFPH_PF9MFP_Pos)                /*!< SYS_T::GPF_MFPH: PF9MFP Mask           */

#define SYS_GPF_MFPH_PF10MFP_Pos         (8)                                               /*!< SYS_T::GPF_MFPH: PF10MFP Position      */
#define SYS_GPF_MFPH_PF10MFP_Msk         (0xful << SYS_GPF_MFPH_PF10MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF10MFP Mask          */

#define SYS_GPF_MFPH_PF11MFP_Pos         (12)                                              /*!< SYS_T::GPF_MFPH: PF11MFP Position      */
#define SYS_GPF_MFPH_PF11MFP_Msk         (0xful << SYS_GPF_MFPH_PF11MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF11MFP Mask          */

#define SYS_GPF_MFPH_PF12MFP_Pos         (16)                                              /*!< SYS_T::GPF_MFPH: PF12MFP Position      */
#define SYS_GPF_MFPH_PF12MFP_Msk         (0xful << SYS_GPF_MFPH_PF12MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF12MFP Mask          */

#define SYS_GPF_MFPH_PF13MFP_Pos         (20)                                              /*!< SYS_T::GPF_MFPH: PF13MFP Position      */
#define SYS_GPF_MFPH_PF13MFP_Msk         (0xful << SYS_GPF_MFPH_PF13MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF13MFP Mask          */

#define SYS_GPF_MFPH_PF14MFP_Pos         (24)                                              /*!< SYS_T::GPF_MFPH: PF14MFP Position      */
#define SYS_GPF_MFPH_PF14MFP_Msk         (0xful << SYS_GPF_MFPH_PF14MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF14MFP Mask          */

#define SYS_GPF_MFPH_PF15MFP_Pos         (28)                                              /*!< SYS_T::GPF_MFPH: PF15MFP Position      */
#define SYS_GPF_MFPH_PF15MFP_Msk         (0xful << SYS_GPF_MFPH_PF15MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF15MFP Mask          */

#define SYS_GPG_MFPL_PG0MFP_Pos          (0)                                               /*!< SYS_T::GPG_MFPL: PG0MFP Position       */
#define SYS_GPG_MFPL_PG0MFP_Msk          (0xful << SYS_GPG_MFPL_PG0MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG0MFP Mask           */

#define SYS_GPG_MFPL_PG1MFP_Pos          (4)                                               /*!< SYS_T::GPG_MFPL: PG1MFP Position       */
#define SYS_GPG_MFPL_PG1MFP_Msk          (0xful << SYS_GPG_MFPL_PG1MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG1MFP Mask           */

#define SYS_GPG_MFPL_PG2MFP_Pos          (8)                                               /*!< SYS_T::GPG_MFPL: PG2MFP Position       */
#define SYS_GPG_MFPL_PG2MFP_Msk          (0xful << SYS_GPG_MFPL_PG2MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG2MFP Mask           */

#define SYS_GPG_MFPL_PG3MFP_Pos          (12)                                              /*!< SYS_T::GPG_MFPL: PG3MFP Position       */
#define SYS_GPG_MFPL_PG3MFP_Msk          (0xful << SYS_GPG_MFPL_PG3MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG3MFP Mask           */

#define SYS_GPG_MFPL_PG4MFP_Pos          (16)                                              /*!< SYS_T::GPG_MFPL: PG4MFP Position       */
#define SYS_GPG_MFPL_PG4MFP_Msk          (0xful << SYS_GPG_MFPL_PG4MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG4MFP Mask           */

#define SYS_GPG_MFPL_PG5MFP_Pos          (20)                                              /*!< SYS_T::GPG_MFPL: PG5MFP Position       */
#define SYS_GPG_MFPL_PG5MFP_Msk          (0xful << SYS_GPG_MFPL_PG5MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG5MFP Mask           */

#define SYS_GPG_MFPL_PG6MFP_Pos          (24)                                              /*!< SYS_T::GPG_MFPL: PG6MFP Position       */
#define SYS_GPG_MFPL_PG6MFP_Msk          (0xful << SYS_GPG_MFPL_PG6MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG6MFP Mask           */

#define SYS_GPG_MFPL_PG7MFP_Pos          (28)                                              /*!< SYS_T::GPG_MFPL: PG7MFP Position       */
#define SYS_GPG_MFPL_PG7MFP_Msk          (0xful << SYS_GPG_MFPL_PG7MFP_Pos)                /*!< SYS_T::GPG_MFPL: PG7MFP Mask           */

#define SYS_GPG_MFPH_PG8MFP_Pos          (0)                                               /*!< SYS_T::GPG_MFPH: PG8MFP Position       */
#define SYS_GPG_MFPH_PG8MFP_Msk          (0xful << SYS_GPG_MFPH_PG8MFP_Pos)                /*!< SYS_T::GPG_MFPH: PG8MFP Mask           */

#define SYS_GPG_MFPH_PG9MFP_Pos          (4)                                               /*!< SYS_T::GPG_MFPH: PG9MFP Position       */
#define SYS_GPG_MFPH_PG9MFP_Msk          (0xful << SYS_GPG_MFPH_PG9MFP_Pos)                /*!< SYS_T::GPG_MFPH: PG9MFP Mask           */

#define SYS_GPG_MFPH_PG10MFP_Pos         (8)                                               /*!< SYS_T::GPG_MFPH: PG10MFP Position      */
#define SYS_GPG_MFPH_PG10MFP_Msk         (0xful << SYS_GPG_MFPH_PG10MFP_Pos)               /*!< SYS_T::GPG_MFPH: PG10MFP Mask          */

#define SYS_GPG_MFPH_PG11MFP_Pos         (12)                                              /*!< SYS_T::GPG_MFPH: PG11MFP Position      */
#define SYS_GPG_MFPH_PG11MFP_Msk         (0xful << SYS_GPG_MFPH_PG11MFP_Pos)               /*!< SYS_T::GPG_MFPH: PG11MFP Mask          */

#define SYS_GPG_MFPH_PG12MFP_Pos         (16)                                              /*!< SYS_T::GPG_MFPH: PG12MFP Position      */
#define SYS_GPG_MFPH_PG12MFP_Msk         (0xful << SYS_GPG_MFPH_PG12MFP_Pos)               /*!< SYS_T::GPG_MFPH: PG12MFP Mask          */

#define SYS_GPG_MFPH_PG13MFP_Pos         (20)                                              /*!< SYS_T::GPG_MFPH: PG13MFP Position      */
#define SYS_GPG_MFPH_PG13MFP_Msk         (0xful << SYS_GPG_MFPH_PG13MFP_Pos)               /*!< SYS_T::GPG_MFPH: PG13MFP Mask          */

#define SYS_GPG_MFPH_PG14MFP_Pos         (24)                                              /*!< SYS_T::GPG_MFPH: PG14MFP Position      */
#define SYS_GPG_MFPH_PG14MFP_Msk         (0xful << SYS_GPG_MFPH_PG14MFP_Pos)               /*!< SYS_T::GPG_MFPH: PG14MFP Mask          */

#define SYS_GPG_MFPH_PG15MFP_Pos         (28)                                              /*!< SYS_T::GPG_MFPH: PG15MFP Position      */
#define SYS_GPG_MFPH_PG15MFP_Msk         (0xful << SYS_GPG_MFPH_PG15MFP_Pos)               /*!< SYS_T::GPG_MFPH: PG15MFP Mask          */

#define SYS_GPH_MFPL_PH0MFP_Pos          (0)                                               /*!< SYS_T::GPH_MFPL: PH0MFP Position       */
#define SYS_GPH_MFPL_PH0MFP_Msk          (0xful << SYS_GPH_MFPL_PH0MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH0MFP Mask           */

#define SYS_GPH_MFPL_PH1MFP_Pos          (4)                                               /*!< SYS_T::GPH_MFPL: PH1MFP Position       */
#define SYS_GPH_MFPL_PH1MFP_Msk          (0xful << SYS_GPH_MFPL_PH1MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH1MFP Mask           */

#define SYS_GPH_MFPL_PH2MFP_Pos          (8)                                               /*!< SYS_T::GPH_MFPL: PH2MFP Position       */
#define SYS_GPH_MFPL_PH2MFP_Msk          (0xful << SYS_GPH_MFPL_PH2MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH2MFP Mask           */

#define SYS_GPH_MFPL_PH3MFP_Pos          (12)                                              /*!< SYS_T::GPH_MFPL: PH3MFP Position       */
#define SYS_GPH_MFPL_PH3MFP_Msk          (0xful << SYS_GPH_MFPL_PH3MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH3MFP Mask           */

#define SYS_GPH_MFPL_PH4MFP_Pos          (16)                                              /*!< SYS_T::GPH_MFPL: PH4MFP Position       */
#define SYS_GPH_MFPL_PH4MFP_Msk          (0xful << SYS_GPH_MFPL_PH4MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH4MFP Mask           */

#define SYS_GPH_MFPL_PH5MFP_Pos          (20)                                              /*!< SYS_T::GPH_MFPL: PH5MFP Position       */
#define SYS_GPH_MFPL_PH5MFP_Msk          (0xful << SYS_GPH_MFPL_PH5MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH5MFP Mask           */

#define SYS_GPH_MFPL_PH6MFP_Pos          (24)                                              /*!< SYS_T::GPH_MFPL: PH6MFP Position       */
#define SYS_GPH_MFPL_PH6MFP_Msk          (0xful << SYS_GPH_MFPL_PH6MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH6MFP Mask           */

#define SYS_GPH_MFPL_PH7MFP_Pos          (28)                                              /*!< SYS_T::GPH_MFPL: PH7MFP Position       */
#define SYS_GPH_MFPL_PH7MFP_Msk          (0xful << SYS_GPH_MFPL_PH7MFP_Pos)                /*!< SYS_T::GPH_MFPL: PH7MFP Mask           */

#define SYS_GPH_MFPH_PH8MFP_Pos          (0)                                               /*!< SYS_T::GPH_MFPH: PH8MFP Position       */
#define SYS_GPH_MFPH_PH8MFP_Msk          (0xful << SYS_GPH_MFPH_PH8MFP_Pos)                /*!< SYS_T::GPH_MFPH: PH8MFP Mask           */

#define SYS_GPH_MFPH_PH9MFP_Pos          (4)                                               /*!< SYS_T::GPH_MFPH: PH9MFP Position       */
#define SYS_GPH_MFPH_PH9MFP_Msk          (0xful << SYS_GPH_MFPH_PH9MFP_Pos)                /*!< SYS_T::GPH_MFPH: PH9MFP Mask           */

#define SYS_GPH_MFPH_PH10MFP_Pos         (8)                                               /*!< SYS_T::GPH_MFPH: PH10MFP Position      */
#define SYS_GPH_MFPH_PH10MFP_Msk         (0xful << SYS_GPH_MFPH_PH10MFP_Pos)               /*!< SYS_T::GPH_MFPH: PH10MFP Mask          */

#define SYS_GPH_MFPH_PH11MFP_Pos         (12)                                              /*!< SYS_T::GPH_MFPH: PH11MFP Position      */
#define SYS_GPH_MFPH_PH11MFP_Msk         (0xful << SYS_GPH_MFPH_PH11MFP_Pos)               /*!< SYS_T::GPH_MFPH: PH11MFP Mask          */

#define SYS_GPH_MFPH_PH12MFP_Pos         (16)                                              /*!< SYS_T::GPH_MFPH: PH12MFP Position      */
#define SYS_GPH_MFPH_PH12MFP_Msk         (0xful << SYS_GPH_MFPH_PH12MFP_Pos)               /*!< SYS_T::GPH_MFPH: PH12MFP Mask          */

#define SYS_GPH_MFPH_PH13MFP_Pos         (20)                                              /*!< SYS_T::GPH_MFPH: PH13MFP Position      */
#define SYS_GPH_MFPH_PH13MFP_Msk         (0xful << SYS_GPH_MFPH_PH13MFP_Pos)               /*!< SYS_T::GPH_MFPH: PH13MFP Mask          */

#define SYS_GPH_MFPH_PH14MFP_Pos         (24)                                              /*!< SYS_T::GPH_MFPH: PH14MFP Position      */
#define SYS_GPH_MFPH_PH14MFP_Msk         (0xful << SYS_GPH_MFPH_PH14MFP_Pos)               /*!< SYS_T::GPH_MFPH: PH14MFP Mask          */

#define SYS_GPH_MFPH_PH15MFP_Pos         (28)                                              /*!< SYS_T::GPH_MFPH: PH15MFP Position      */
#define SYS_GPH_MFPH_PH15MFP_Msk         (0xful << SYS_GPH_MFPH_PH15MFP_Pos)               /*!< SYS_T::GPH_MFPH: PH15MFP Mask          */

#define SYS_GPA_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPA_MFOS: MFOS0 Position        */
#define SYS_GPA_MFOS_MFOS0_Msk           (0x1ul << SYS_GPA_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS0 Mask            */

#define SYS_GPA_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPA_MFOS: MFOS1 Position        */
#define SYS_GPA_MFOS_MFOS1_Msk           (0x1ul << SYS_GPA_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS1 Mask            */

#define SYS_GPA_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPA_MFOS: MFOS2 Position        */
#define SYS_GPA_MFOS_MFOS2_Msk           (0x1ul << SYS_GPA_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS2 Mask            */

#define SYS_GPA_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPA_MFOS: MFOS3 Position        */
#define SYS_GPA_MFOS_MFOS3_Msk           (0x1ul << SYS_GPA_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS3 Mask            */

#define SYS_GPA_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPA_MFOS: MFOS4 Position        */
#define SYS_GPA_MFOS_MFOS4_Msk           (0x1ul << SYS_GPA_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS4 Mask            */

#define SYS_GPA_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPA_MFOS: MFOS5 Position        */
#define SYS_GPA_MFOS_MFOS5_Msk           (0x1ul << SYS_GPA_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS5 Mask            */

#define SYS_GPA_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPA_MFOS: MFOS6 Position        */
#define SYS_GPA_MFOS_MFOS6_Msk           (0x1ul << SYS_GPA_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS6 Mask            */

#define SYS_GPA_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPA_MFOS: MFOS7 Position        */
#define SYS_GPA_MFOS_MFOS7_Msk           (0x1ul << SYS_GPA_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS7 Mask            */

#define SYS_GPA_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPA_MFOS: MFOS8 Position        */
#define SYS_GPA_MFOS_MFOS8_Msk           (0x1ul << SYS_GPA_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS8 Mask            */

#define SYS_GPA_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPA_MFOS: MFOS9 Position        */
#define SYS_GPA_MFOS_MFOS9_Msk           (0x1ul << SYS_GPA_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS9 Mask            */

#define SYS_GPA_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPA_MFOS: MFOS10 Position       */
#define SYS_GPA_MFOS_MFOS10_Msk          (0x1ul << SYS_GPA_MFOS_MFOS10_Pos)                /*!< SYS_T::GPA_MFOS: MFOS10 Mask           */

#define SYS_GPA_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPA_MFOS: MFOS11 Position       */
#define SYS_GPA_MFOS_MFOS11_Msk          (0x1ul << SYS_GPA_MFOS_MFOS11_Pos)                /*!< SYS_T::GPA_MFOS: MFOS11 Mask           */

#define SYS_GPA_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPA_MFOS: MFOS12 Position       */
#define SYS_GPA_MFOS_MFOS12_Msk          (0x1ul << SYS_GPA_MFOS_MFOS12_Pos)                /*!< SYS_T::GPA_MFOS: MFOS12 Mask           */

#define SYS_GPA_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPA_MFOS: MFOS13 Position       */
#define SYS_GPA_MFOS_MFOS13_Msk          (0x1ul << SYS_GPA_MFOS_MFOS13_Pos)                /*!< SYS_T::GPA_MFOS: MFOS13 Mask           */

#define SYS_GPA_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPA_MFOS: MFOS14 Position       */
#define SYS_GPA_MFOS_MFOS14_Msk          (0x1ul << SYS_GPA_MFOS_MFOS14_Pos)                /*!< SYS_T::GPA_MFOS: MFOS14 Mask           */

#define SYS_GPA_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPA_MFOS: MFOS15 Position       */
#define SYS_GPA_MFOS_MFOS15_Msk          (0x1ul << SYS_GPA_MFOS_MFOS15_Pos)                /*!< SYS_T::GPA_MFOS: MFOS15 Mask           */

#define SYS_GPB_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPB_MFOS: MFOS0 Position        */
#define SYS_GPB_MFOS_MFOS0_Msk           (0x1ul << SYS_GPB_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS0 Mask            */

#define SYS_GPB_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPB_MFOS: MFOS1 Position        */
#define SYS_GPB_MFOS_MFOS1_Msk           (0x1ul << SYS_GPB_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS1 Mask            */

#define SYS_GPB_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPB_MFOS: MFOS2 Position        */
#define SYS_GPB_MFOS_MFOS2_Msk           (0x1ul << SYS_GPB_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS2 Mask            */

#define SYS_GPB_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPB_MFOS: MFOS3 Position        */
#define SYS_GPB_MFOS_MFOS3_Msk           (0x1ul << SYS_GPB_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS3 Mask            */

#define SYS_GPB_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPB_MFOS: MFOS4 Position        */
#define SYS_GPB_MFOS_MFOS4_Msk           (0x1ul << SYS_GPB_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS4 Mask            */

#define SYS_GPB_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPB_MFOS: MFOS5 Position        */
#define SYS_GPB_MFOS_MFOS5_Msk           (0x1ul << SYS_GPB_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS5 Mask            */

#define SYS_GPB_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPB_MFOS: MFOS6 Position        */
#define SYS_GPB_MFOS_MFOS6_Msk           (0x1ul << SYS_GPB_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS6 Mask            */

#define SYS_GPB_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPB_MFOS: MFOS7 Position        */
#define SYS_GPB_MFOS_MFOS7_Msk           (0x1ul << SYS_GPB_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS7 Mask            */

#define SYS_GPB_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPB_MFOS: MFOS8 Position        */
#define SYS_GPB_MFOS_MFOS8_Msk           (0x1ul << SYS_GPB_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS8 Mask            */

#define SYS_GPB_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPB_MFOS: MFOS9 Position        */
#define SYS_GPB_MFOS_MFOS9_Msk           (0x1ul << SYS_GPB_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS9 Mask            */

#define SYS_GPB_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPB_MFOS: MFOS10 Position       */
#define SYS_GPB_MFOS_MFOS10_Msk          (0x1ul << SYS_GPB_MFOS_MFOS10_Pos)                /*!< SYS_T::GPB_MFOS: MFOS10 Mask           */

#define SYS_GPB_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPB_MFOS: MFOS11 Position       */
#define SYS_GPB_MFOS_MFOS11_Msk          (0x1ul << SYS_GPB_MFOS_MFOS11_Pos)                /*!< SYS_T::GPB_MFOS: MFOS11 Mask           */

#define SYS_GPB_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPB_MFOS: MFOS12 Position       */
#define SYS_GPB_MFOS_MFOS12_Msk          (0x1ul << SYS_GPB_MFOS_MFOS12_Pos)                /*!< SYS_T::GPB_MFOS: MFOS12 Mask           */

#define SYS_GPB_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPB_MFOS: MFOS13 Position       */
#define SYS_GPB_MFOS_MFOS13_Msk          (0x1ul << SYS_GPB_MFOS_MFOS13_Pos)                /*!< SYS_T::GPB_MFOS: MFOS13 Mask           */

#define SYS_GPB_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPB_MFOS: MFOS14 Position       */
#define SYS_GPB_MFOS_MFOS14_Msk          (0x1ul << SYS_GPB_MFOS_MFOS14_Pos)                /*!< SYS_T::GPB_MFOS: MFOS14 Mask           */

#define SYS_GPB_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPB_MFOS: MFOS15 Position       */
#define SYS_GPB_MFOS_MFOS15_Msk          (0x1ul << SYS_GPB_MFOS_MFOS15_Pos)                /*!< SYS_T::GPB_MFOS: MFOS15 Mask           */

#define SYS_GPC_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPC_MFOS: MFOS0 Position        */
#define SYS_GPC_MFOS_MFOS0_Msk           (0x1ul << SYS_GPC_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS0 Mask            */

#define SYS_GPC_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPC_MFOS: MFOS1 Position        */
#define SYS_GPC_MFOS_MFOS1_Msk           (0x1ul << SYS_GPC_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS1 Mask            */

#define SYS_GPC_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPC_MFOS: MFOS2 Position        */
#define SYS_GPC_MFOS_MFOS2_Msk           (0x1ul << SYS_GPC_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS2 Mask            */

#define SYS_GPC_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPC_MFOS: MFOS3 Position        */
#define SYS_GPC_MFOS_MFOS3_Msk           (0x1ul << SYS_GPC_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS3 Mask            */

#define SYS_GPC_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPC_MFOS: MFOS4 Position        */
#define SYS_GPC_MFOS_MFOS4_Msk           (0x1ul << SYS_GPC_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS4 Mask            */

#define SYS_GPC_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPC_MFOS: MFOS5 Position        */
#define SYS_GPC_MFOS_MFOS5_Msk           (0x1ul << SYS_GPC_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS5 Mask            */

#define SYS_GPC_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPC_MFOS: MFOS6 Position        */
#define SYS_GPC_MFOS_MFOS6_Msk           (0x1ul << SYS_GPC_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS6 Mask            */

#define SYS_GPC_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPC_MFOS: MFOS7 Position        */
#define SYS_GPC_MFOS_MFOS7_Msk           (0x1ul << SYS_GPC_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS7 Mask            */

#define SYS_GPC_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPC_MFOS: MFOS8 Position        */
#define SYS_GPC_MFOS_MFOS8_Msk           (0x1ul << SYS_GPC_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS8 Mask            */

#define SYS_GPC_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPC_MFOS: MFOS9 Position        */
#define SYS_GPC_MFOS_MFOS9_Msk           (0x1ul << SYS_GPC_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS9 Mask            */

#define SYS_GPC_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPC_MFOS: MFOS10 Position       */
#define SYS_GPC_MFOS_MFOS10_Msk          (0x1ul << SYS_GPC_MFOS_MFOS10_Pos)                /*!< SYS_T::GPC_MFOS: MFOS10 Mask           */

#define SYS_GPC_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPC_MFOS: MFOS11 Position       */
#define SYS_GPC_MFOS_MFOS11_Msk          (0x1ul << SYS_GPC_MFOS_MFOS11_Pos)                /*!< SYS_T::GPC_MFOS: MFOS11 Mask           */

#define SYS_GPC_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPC_MFOS: MFOS12 Position       */
#define SYS_GPC_MFOS_MFOS12_Msk          (0x1ul << SYS_GPC_MFOS_MFOS12_Pos)                /*!< SYS_T::GPC_MFOS: MFOS12 Mask           */

#define SYS_GPC_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPC_MFOS: MFOS13 Position       */
#define SYS_GPC_MFOS_MFOS13_Msk          (0x1ul << SYS_GPC_MFOS_MFOS13_Pos)                /*!< SYS_T::GPC_MFOS: MFOS13 Mask           */

#define SYS_GPC_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPC_MFOS: MFOS14 Position       */
#define SYS_GPC_MFOS_MFOS14_Msk          (0x1ul << SYS_GPC_MFOS_MFOS14_Pos)                /*!< SYS_T::GPC_MFOS: MFOS14 Mask           */

#define SYS_GPC_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPC_MFOS: MFOS15 Position       */
#define SYS_GPC_MFOS_MFOS15_Msk          (0x1ul << SYS_GPC_MFOS_MFOS15_Pos)                /*!< SYS_T::GPC_MFOS: MFOS15 Mask           */

#define SYS_GPD_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPD_MFOS: MFOS0 Position        */
#define SYS_GPD_MFOS_MFOS0_Msk           (0x1ul << SYS_GPD_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS0 Mask            */

#define SYS_GPD_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPD_MFOS: MFOS1 Position        */
#define SYS_GPD_MFOS_MFOS1_Msk           (0x1ul << SYS_GPD_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS1 Mask            */

#define SYS_GPD_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPD_MFOS: MFOS2 Position        */
#define SYS_GPD_MFOS_MFOS2_Msk           (0x1ul << SYS_GPD_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS2 Mask            */

#define SYS_GPD_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPD_MFOS: MFOS3 Position        */
#define SYS_GPD_MFOS_MFOS3_Msk           (0x1ul << SYS_GPD_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS3 Mask            */

#define SYS_GPD_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPD_MFOS: MFOS4 Position        */
#define SYS_GPD_MFOS_MFOS4_Msk           (0x1ul << SYS_GPD_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS4 Mask            */

#define SYS_GPD_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPD_MFOS: MFOS5 Position        */
#define SYS_GPD_MFOS_MFOS5_Msk           (0x1ul << SYS_GPD_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS5 Mask            */

#define SYS_GPD_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPD_MFOS: MFOS6 Position        */
#define SYS_GPD_MFOS_MFOS6_Msk           (0x1ul << SYS_GPD_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS6 Mask            */

#define SYS_GPD_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPD_MFOS: MFOS7 Position        */
#define SYS_GPD_MFOS_MFOS7_Msk           (0x1ul << SYS_GPD_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS7 Mask            */

#define SYS_GPD_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPD_MFOS: MFOS8 Position        */
#define SYS_GPD_MFOS_MFOS8_Msk           (0x1ul << SYS_GPD_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS8 Mask            */

#define SYS_GPD_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPD_MFOS: MFOS9 Position        */
#define SYS_GPD_MFOS_MFOS9_Msk           (0x1ul << SYS_GPD_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS9 Mask            */

#define SYS_GPD_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPD_MFOS: MFOS10 Position       */
#define SYS_GPD_MFOS_MFOS10_Msk          (0x1ul << SYS_GPD_MFOS_MFOS10_Pos)                /*!< SYS_T::GPD_MFOS: MFOS10 Mask           */

#define SYS_GPD_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPD_MFOS: MFOS11 Position       */
#define SYS_GPD_MFOS_MFOS11_Msk          (0x1ul << SYS_GPD_MFOS_MFOS11_Pos)                /*!< SYS_T::GPD_MFOS: MFOS11 Mask           */

#define SYS_GPD_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPD_MFOS: MFOS12 Position       */
#define SYS_GPD_MFOS_MFOS12_Msk          (0x1ul << SYS_GPD_MFOS_MFOS12_Pos)                /*!< SYS_T::GPD_MFOS: MFOS12 Mask           */

#define SYS_GPD_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPD_MFOS: MFOS13 Position       */
#define SYS_GPD_MFOS_MFOS13_Msk          (0x1ul << SYS_GPD_MFOS_MFOS13_Pos)                /*!< SYS_T::GPD_MFOS: MFOS13 Mask           */

#define SYS_GPD_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPD_MFOS: MFOS14 Position       */
#define SYS_GPD_MFOS_MFOS14_Msk          (0x1ul << SYS_GPD_MFOS_MFOS14_Pos)                /*!< SYS_T::GPD_MFOS: MFOS14 Mask           */

#define SYS_GPD_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPD_MFOS: MFOS15 Position       */
#define SYS_GPD_MFOS_MFOS15_Msk          (0x1ul << SYS_GPD_MFOS_MFOS15_Pos)                /*!< SYS_T::GPD_MFOS: MFOS15 Mask           */

#define SYS_GPE_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPE_MFOS: MFOS0 Position        */
#define SYS_GPE_MFOS_MFOS0_Msk           (0x1ul << SYS_GPE_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS0 Mask            */

#define SYS_GPE_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPE_MFOS: MFOS1 Position        */
#define SYS_GPE_MFOS_MFOS1_Msk           (0x1ul << SYS_GPE_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS1 Mask            */

#define SYS_GPE_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPE_MFOS: MFOS2 Position        */
#define SYS_GPE_MFOS_MFOS2_Msk           (0x1ul << SYS_GPE_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS2 Mask            */

#define SYS_GPE_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPE_MFOS: MFOS3 Position        */
#define SYS_GPE_MFOS_MFOS3_Msk           (0x1ul << SYS_GPE_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS3 Mask            */

#define SYS_GPE_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPE_MFOS: MFOS4 Position        */
#define SYS_GPE_MFOS_MFOS4_Msk           (0x1ul << SYS_GPE_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS4 Mask            */

#define SYS_GPE_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPE_MFOS: MFOS5 Position        */
#define SYS_GPE_MFOS_MFOS5_Msk           (0x1ul << SYS_GPE_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS5 Mask            */

#define SYS_GPE_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPE_MFOS: MFOS6 Position        */
#define SYS_GPE_MFOS_MFOS6_Msk           (0x1ul << SYS_GPE_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS6 Mask            */

#define SYS_GPE_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPE_MFOS: MFOS7 Position        */
#define SYS_GPE_MFOS_MFOS7_Msk           (0x1ul << SYS_GPE_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS7 Mask            */

#define SYS_GPE_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPE_MFOS: MFOS8 Position        */
#define SYS_GPE_MFOS_MFOS8_Msk           (0x1ul << SYS_GPE_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS8 Mask            */

#define SYS_GPE_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPE_MFOS: MFOS9 Position        */
#define SYS_GPE_MFOS_MFOS9_Msk           (0x1ul << SYS_GPE_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS9 Mask            */

#define SYS_GPE_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPE_MFOS: MFOS10 Position       */
#define SYS_GPE_MFOS_MFOS10_Msk          (0x1ul << SYS_GPE_MFOS_MFOS10_Pos)                /*!< SYS_T::GPE_MFOS: MFOS10 Mask           */

#define SYS_GPE_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPE_MFOS: MFOS11 Position       */
#define SYS_GPE_MFOS_MFOS11_Msk          (0x1ul << SYS_GPE_MFOS_MFOS11_Pos)                /*!< SYS_T::GPE_MFOS: MFOS11 Mask           */

#define SYS_GPE_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPE_MFOS: MFOS12 Position       */
#define SYS_GPE_MFOS_MFOS12_Msk          (0x1ul << SYS_GPE_MFOS_MFOS12_Pos)                /*!< SYS_T::GPE_MFOS: MFOS12 Mask           */

#define SYS_GPE_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPE_MFOS: MFOS13 Position       */
#define SYS_GPE_MFOS_MFOS13_Msk          (0x1ul << SYS_GPE_MFOS_MFOS13_Pos)                /*!< SYS_T::GPE_MFOS: MFOS13 Mask           */

#define SYS_GPE_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPE_MFOS: MFOS14 Position       */
#define SYS_GPE_MFOS_MFOS14_Msk          (0x1ul << SYS_GPE_MFOS_MFOS14_Pos)                /*!< SYS_T::GPE_MFOS: MFOS14 Mask           */

#define SYS_GPE_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPE_MFOS: MFOS15 Position       */
#define SYS_GPE_MFOS_MFOS15_Msk          (0x1ul << SYS_GPE_MFOS_MFOS15_Pos)                /*!< SYS_T::GPE_MFOS: MFOS15 Mask           */

#define SYS_GPF_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPF_MFOS: MFOS0 Position        */
#define SYS_GPF_MFOS_MFOS0_Msk           (0x1ul << SYS_GPF_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS0 Mask            */

#define SYS_GPF_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPF_MFOS: MFOS1 Position        */
#define SYS_GPF_MFOS_MFOS1_Msk           (0x1ul << SYS_GPF_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS1 Mask            */

#define SYS_GPF_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPF_MFOS: MFOS2 Position        */
#define SYS_GPF_MFOS_MFOS2_Msk           (0x1ul << SYS_GPF_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS2 Mask            */

#define SYS_GPF_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPF_MFOS: MFOS3 Position        */
#define SYS_GPF_MFOS_MFOS3_Msk           (0x1ul << SYS_GPF_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS3 Mask            */

#define SYS_GPF_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPF_MFOS: MFOS4 Position        */
#define SYS_GPF_MFOS_MFOS4_Msk           (0x1ul << SYS_GPF_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS4 Mask            */

#define SYS_GPF_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPF_MFOS: MFOS5 Position        */
#define SYS_GPF_MFOS_MFOS5_Msk           (0x1ul << SYS_GPF_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS5 Mask            */

#define SYS_GPF_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPF_MFOS: MFOS6 Position        */
#define SYS_GPF_MFOS_MFOS6_Msk           (0x1ul << SYS_GPF_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS6 Mask            */

#define SYS_GPF_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPF_MFOS: MFOS7 Position        */
#define SYS_GPF_MFOS_MFOS7_Msk           (0x1ul << SYS_GPF_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS7 Mask            */

#define SYS_GPF_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPF_MFOS: MFOS8 Position        */
#define SYS_GPF_MFOS_MFOS8_Msk           (0x1ul << SYS_GPF_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS8 Mask            */

#define SYS_GPF_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPF_MFOS: MFOS9 Position        */
#define SYS_GPF_MFOS_MFOS9_Msk           (0x1ul << SYS_GPF_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS9 Mask            */

#define SYS_GPF_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPF_MFOS: MFOS10 Position       */
#define SYS_GPF_MFOS_MFOS10_Msk          (0x1ul << SYS_GPF_MFOS_MFOS10_Pos)                /*!< SYS_T::GPF_MFOS: MFOS10 Mask           */

#define SYS_GPF_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPF_MFOS: MFOS11 Position       */
#define SYS_GPF_MFOS_MFOS11_Msk          (0x1ul << SYS_GPF_MFOS_MFOS11_Pos)                /*!< SYS_T::GPF_MFOS: MFOS11 Mask           */

#define SYS_GPF_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPF_MFOS: MFOS12 Position       */
#define SYS_GPF_MFOS_MFOS12_Msk          (0x1ul << SYS_GPF_MFOS_MFOS12_Pos)                /*!< SYS_T::GPF_MFOS: MFOS12 Mask           */

#define SYS_GPF_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPF_MFOS: MFOS13 Position       */
#define SYS_GPF_MFOS_MFOS13_Msk          (0x1ul << SYS_GPF_MFOS_MFOS13_Pos)                /*!< SYS_T::GPF_MFOS: MFOS13 Mask           */

#define SYS_GPF_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPF_MFOS: MFOS14 Position       */
#define SYS_GPF_MFOS_MFOS14_Msk          (0x1ul << SYS_GPF_MFOS_MFOS14_Pos)                /*!< SYS_T::GPF_MFOS: MFOS14 Mask           */

#define SYS_GPF_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPF_MFOS: MFOS15 Position       */
#define SYS_GPF_MFOS_MFOS15_Msk          (0x1ul << SYS_GPF_MFOS_MFOS15_Pos)                /*!< SYS_T::GPF_MFOS: MFOS15 Mask           */

#define SYS_GPG_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPG_MFOS: MFOS0 Position        */
#define SYS_GPG_MFOS_MFOS0_Msk           (0x1ul << SYS_GPG_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS0 Mask            */

#define SYS_GPG_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPG_MFOS: MFOS1 Position        */
#define SYS_GPG_MFOS_MFOS1_Msk           (0x1ul << SYS_GPG_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS1 Mask            */

#define SYS_GPG_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPG_MFOS: MFOS2 Position        */
#define SYS_GPG_MFOS_MFOS2_Msk           (0x1ul << SYS_GPG_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS2 Mask            */

#define SYS_GPG_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPG_MFOS: MFOS3 Position        */
#define SYS_GPG_MFOS_MFOS3_Msk           (0x1ul << SYS_GPG_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS3 Mask            */

#define SYS_GPG_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPG_MFOS: MFOS4 Position        */
#define SYS_GPG_MFOS_MFOS4_Msk           (0x1ul << SYS_GPG_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS4 Mask            */

#define SYS_GPG_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPG_MFOS: MFOS5 Position        */
#define SYS_GPG_MFOS_MFOS5_Msk           (0x1ul << SYS_GPG_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS5 Mask            */

#define SYS_GPG_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPG_MFOS: MFOS6 Position        */
#define SYS_GPG_MFOS_MFOS6_Msk           (0x1ul << SYS_GPG_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS6 Mask            */

#define SYS_GPG_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPG_MFOS: MFOS7 Position        */
#define SYS_GPG_MFOS_MFOS7_Msk           (0x1ul << SYS_GPG_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS7 Mask            */

#define SYS_GPG_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPG_MFOS: MFOS8 Position        */
#define SYS_GPG_MFOS_MFOS8_Msk           (0x1ul << SYS_GPG_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS8 Mask            */

#define SYS_GPG_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPG_MFOS: MFOS9 Position        */
#define SYS_GPG_MFOS_MFOS9_Msk           (0x1ul << SYS_GPG_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPG_MFOS: MFOS9 Mask            */

#define SYS_GPG_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPG_MFOS: MFOS10 Position       */
#define SYS_GPG_MFOS_MFOS10_Msk          (0x1ul << SYS_GPG_MFOS_MFOS10_Pos)                /*!< SYS_T::GPG_MFOS: MFOS10 Mask           */

#define SYS_GPG_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPG_MFOS: MFOS11 Position       */
#define SYS_GPG_MFOS_MFOS11_Msk          (0x1ul << SYS_GPG_MFOS_MFOS11_Pos)                /*!< SYS_T::GPG_MFOS: MFOS11 Mask           */

#define SYS_GPG_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPG_MFOS: MFOS12 Position       */
#define SYS_GPG_MFOS_MFOS12_Msk          (0x1ul << SYS_GPG_MFOS_MFOS12_Pos)                /*!< SYS_T::GPG_MFOS: MFOS12 Mask           */

#define SYS_GPG_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPG_MFOS: MFOS13 Position       */
#define SYS_GPG_MFOS_MFOS13_Msk          (0x1ul << SYS_GPG_MFOS_MFOS13_Pos)                /*!< SYS_T::GPG_MFOS: MFOS13 Mask           */

#define SYS_GPG_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPG_MFOS: MFOS14 Position       */
#define SYS_GPG_MFOS_MFOS14_Msk          (0x1ul << SYS_GPG_MFOS_MFOS14_Pos)                /*!< SYS_T::GPG_MFOS: MFOS14 Mask           */

#define SYS_GPG_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPG_MFOS: MFOS15 Position       */
#define SYS_GPG_MFOS_MFOS15_Msk          (0x1ul << SYS_GPG_MFOS_MFOS15_Pos)                /*!< SYS_T::GPG_MFOS: MFOS15 Mask           */

#define SYS_GPH_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPH_MFOS: MFOS0 Position        */
#define SYS_GPH_MFOS_MFOS0_Msk           (0x1ul << SYS_GPH_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS0 Mask            */

#define SYS_GPH_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPH_MFOS: MFOS1 Position        */
#define SYS_GPH_MFOS_MFOS1_Msk           (0x1ul << SYS_GPH_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS1 Mask            */

#define SYS_GPH_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPH_MFOS: MFOS2 Position        */
#define SYS_GPH_MFOS_MFOS2_Msk           (0x1ul << SYS_GPH_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS2 Mask            */

#define SYS_GPH_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPH_MFOS: MFOS3 Position        */
#define SYS_GPH_MFOS_MFOS3_Msk           (0x1ul << SYS_GPH_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS3 Mask            */

#define SYS_GPH_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPH_MFOS: MFOS4 Position        */
#define SYS_GPH_MFOS_MFOS4_Msk           (0x1ul << SYS_GPH_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS4 Mask            */

#define SYS_GPH_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPH_MFOS: MFOS5 Position        */
#define SYS_GPH_MFOS_MFOS5_Msk           (0x1ul << SYS_GPH_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS5 Mask            */

#define SYS_GPH_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPH_MFOS: MFOS6 Position        */
#define SYS_GPH_MFOS_MFOS6_Msk           (0x1ul << SYS_GPH_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS6 Mask            */

#define SYS_GPH_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPH_MFOS: MFOS7 Position        */
#define SYS_GPH_MFOS_MFOS7_Msk           (0x1ul << SYS_GPH_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS7 Mask            */

#define SYS_GPH_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPH_MFOS: MFOS8 Position        */
#define SYS_GPH_MFOS_MFOS8_Msk           (0x1ul << SYS_GPH_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS8 Mask            */

#define SYS_GPH_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPH_MFOS: MFOS9 Position        */
#define SYS_GPH_MFOS_MFOS9_Msk           (0x1ul << SYS_GPH_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPH_MFOS: MFOS9 Mask            */

#define SYS_GPH_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPH_MFOS: MFOS10 Position       */
#define SYS_GPH_MFOS_MFOS10_Msk          (0x1ul << SYS_GPH_MFOS_MFOS10_Pos)                /*!< SYS_T::GPH_MFOS: MFOS10 Mask           */

#define SYS_GPH_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPH_MFOS: MFOS11 Position       */
#define SYS_GPH_MFOS_MFOS11_Msk          (0x1ul << SYS_GPH_MFOS_MFOS11_Pos)                /*!< SYS_T::GPH_MFOS: MFOS11 Mask           */

#define SYS_GPH_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPH_MFOS: MFOS12 Position       */
#define SYS_GPH_MFOS_MFOS12_Msk          (0x1ul << SYS_GPH_MFOS_MFOS12_Pos)                /*!< SYS_T::GPH_MFOS: MFOS12 Mask           */

#define SYS_GPH_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPH_MFOS: MFOS13 Position       */
#define SYS_GPH_MFOS_MFOS13_Msk          (0x1ul << SYS_GPH_MFOS_MFOS13_Pos)                /*!< SYS_T::GPH_MFOS: MFOS13 Mask           */

#define SYS_GPH_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPH_MFOS: MFOS14 Position       */
#define SYS_GPH_MFOS_MFOS14_Msk          (0x1ul << SYS_GPH_MFOS_MFOS14_Pos)                /*!< SYS_T::GPH_MFOS: MFOS14 Mask           */

#define SYS_GPH_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPH_MFOS: MFOS15 Position       */
#define SYS_GPH_MFOS_MFOS15_Msk          (0x1ul << SYS_GPH_MFOS_MFOS15_Pos)                /*!< SYS_T::GPH_MFOS: MFOS15 Mask           */

#define SYS_SRAM_INTCTL_PERRIEN_Pos      (0)                                               /*!< SYS_T::SRAM_INTCTL: PERRIEN Position   */
#define SYS_SRAM_INTCTL_PERRIEN_Msk      (0x1ul << SYS_SRAM_INTCTL_PERRIEN_Pos)            /*!< SYS_T::SRAM_INTCTL: PERRIEN Mask       */

#define SYS_SRAM_STATUS_PERRIF_Pos       (0)                                               /*!< SYS_T::SRAM_STATUS: PERRIF Position    */
#define SYS_SRAM_STATUS_PERRIF_Msk       (0x1ul << SYS_SRAM_STATUS_PERRIF_Pos)             /*!< SYS_T::SRAM_STATUS: PERRIF Mask        */

#define SYS_SRAM_ERRADDR_ERRADDR_Pos     (0)                                               /*!< SYS_T::SRAM_ERRADDR: ERRADDR Position  */
#define SYS_SRAM_ERRADDR_ERRADDR_Msk     (0xfffffffful << SYS_SRAM_ERRADDR_ERRADDR_Pos)    /*!< SYS_T::SRAM_ERRADDR: ERRADDR Mask      */

#define SYS_SRAM_BISTCTL_SRBIST0_Pos     (0)                                               /*!< SYS_T::SRAM_BISTCTL: SRBIST0 Position  */
#define SYS_SRAM_BISTCTL_SRBIST0_Msk     (0x1ul << SYS_SRAM_BISTCTL_SRBIST0_Pos)           /*!< SYS_T::SRAM_BISTCTL: SRBIST0 Mask      */

#define SYS_SRAM_BISTCTL_SRBIST1_Pos     (1)                                               /*!< SYS_T::SRAM_BISTCTL: SRBIST1 Position  */
#define SYS_SRAM_BISTCTL_SRBIST1_Msk     (0x1ul << SYS_SRAM_BISTCTL_SRBIST1_Pos)           /*!< SYS_T::SRAM_BISTCTL: SRBIST1 Mask      */

#define SYS_SRAM_BISTCTL_CRBIST_Pos      (2)                                               /*!< SYS_T::SRAM_BISTCTL: CRBIST Position   */
#define SYS_SRAM_BISTCTL_CRBIST_Msk      (0x1ul << SYS_SRAM_BISTCTL_CRBIST_Pos)            /*!< SYS_T::SRAM_BISTCTL: CRBIST Mask       */

#define SYS_SRAM_BISTCTL_CANBIST_Pos     (3)                                               /*!< SYS_T::SRAM_BISTCTL: CANBIST Position  */
#define SYS_SRAM_BISTCTL_CANBIST_Msk     (0x1ul << SYS_SRAM_BISTCTL_CANBIST_Pos)           /*!< SYS_T::SRAM_BISTCTL: CANBIST Mask      */

#define SYS_SRAM_BISTCTL_USBBIST_Pos     (4)                                               /*!< SYS_T::SRAM_BISTCTL: USBBIST Position  */
#define SYS_SRAM_BISTCTL_USBBIST_Msk     (0x1ul << SYS_SRAM_BISTCTL_USBBIST_Pos)           /*!< SYS_T::SRAM_BISTCTL: USBBIST Mask      */

#define SYS_SRAM_BISTCTL_SPIMBIST_Pos    (5)                                               /*!< SYS_T::SRAM_BISTCTL: SPIMBIST Position */
#define SYS_SRAM_BISTCTL_SPIMBIST_Msk    (0x1ul << SYS_SRAM_BISTCTL_SPIMBIST_Pos)          /*!< SYS_T::SRAM_BISTCTL: SPIMBIST Mask     */

#define SYS_SRAM_BISTCTL_EMCBIST_Pos     (6)                                               /*!< SYS_T::SRAM_BISTCTL: EMCBIST Position  */
#define SYS_SRAM_BISTCTL_EMCBIST_Msk     (0x1ul << SYS_SRAM_BISTCTL_EMCBIST_Pos)           /*!< SYS_T::SRAM_BISTCTL: EMCBIST Mask      */

#define SYS_SRAM_BISTCTL_PDMABIST_Pos    (7)                                               /*!< SYS_T::SRAM_BISTCTL: PDMABIST Position */
#define SYS_SRAM_BISTCTL_PDMABIST_Msk    (0x1ul << SYS_SRAM_BISTCTL_PDMABIST_Pos)          /*!< SYS_T::SRAM_BISTCTL: PDMABIST Mask     */

#define SYS_SRAM_BISTCTL_HSUSBDBIST_Pos  (8)                                               /*!< SYS_T::SRAM_BISTCTL: HSUSBDBIST Position*/
#define SYS_SRAM_BISTCTL_HSUSBDBIST_Msk  (0x1ul << SYS_SRAM_BISTCTL_HSUSBDBIST_Pos)        /*!< SYS_T::SRAM_BISTCTL: HSUSBDBIST Mask   */

#define SYS_SRAM_BISTCTL_HSUSBHBIST_Pos  (9)                                               /*!< SYS_T::SRAM_BISTCTL: HSUSBHBIST Position*/
#define SYS_SRAM_BISTCTL_HSUSBHBIST_Msk  (0x1ul << SYS_SRAM_BISTCTL_HSUSBHBIST_Pos)        /*!< SYS_T::SRAM_BISTCTL: HSUSBHBIST Mask   */

#define SYS_SRAM_BISTCTL_SRB0S0_Pos      (16)                                              /*!< SYS_T::SRAM_BISTCTL: SRB0S0 Position   */
#define SYS_SRAM_BISTCTL_SRB0S0_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB0S0_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB0S0 Mask       */

#define SYS_SRAM_BISTCTL_SRB0S1_Pos      (17)                                              /*!< SYS_T::SRAM_BISTCTL: SRB0S1 Position   */
#define SYS_SRAM_BISTCTL_SRB0S1_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB0S1_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB0S1 Mask       */

#define SYS_SRAM_BISTCTL_SRB1S0_Pos      (18)                                              /*!< SYS_T::SRAM_BISTCTL: SRB1S0 Position   */
#define SYS_SRAM_BISTCTL_SRB1S0_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB1S0_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB1S0 Mask       */

#define SYS_SRAM_BISTCTL_SRB1S1_Pos      (19)                                              /*!< SYS_T::SRAM_BISTCTL: SRB1S1 Position   */
#define SYS_SRAM_BISTCTL_SRB1S1_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB1S1_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB1S1 Mask       */

#define SYS_SRAM_BISTCTL_SRB1S2_Pos      (20)                                              /*!< SYS_T::SRAM_BISTCTL: SRB1S2 Position   */
#define SYS_SRAM_BISTCTL_SRB1S2_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB1S2_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB1S2 Mask       */

#define SYS_SRAM_BISTCTL_SRB1S3_Pos      (21)                                              /*!< SYS_T::SRAM_BISTCTL: SRB1S3 Position   */
#define SYS_SRAM_BISTCTL_SRB1S3_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB1S3_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB1S3 Mask       */

#define SYS_SRAM_BISTCTL_SRB1S4_Pos      (22)                                              /*!< SYS_T::SRAM_BISTCTL: SRB1S4 Position   */
#define SYS_SRAM_BISTCTL_SRB1S4_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB1S4_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB1S4 Mask       */

#define SYS_SRAM_BISTCTL_SRB1S5_Pos      (23)                                              /*!< SYS_T::SRAM_BISTCTL: SRB1S5 Position   */
#define SYS_SRAM_BISTCTL_SRB1S5_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRB1S5_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRB1S5 Mask       */

#define SYS_SRAM_BISTSTS_SRBISTEF0_Pos   (0)                                               /*!< SYS_T::SRAM_BISTSTS: SRBISTEF0 Position*/
#define SYS_SRAM_BISTSTS_SRBISTEF0_Msk   (0x1ul << SYS_SRAM_BISTSTS_SRBISTEF0_Pos)         /*!< SYS_T::SRAM_BISTSTS: SRBISTEF0 Mask    */

#define SYS_SRAM_BISTSTS_SRBISTEF1_Pos   (1)                                               /*!< SYS_T::SRAM_BISTSTS: SRBISTEF1 Position*/
#define SYS_SRAM_BISTSTS_SRBISTEF1_Msk   (0x1ul << SYS_SRAM_BISTSTS_SRBISTEF1_Pos)         /*!< SYS_T::SRAM_BISTSTS: SRBISTEF1 Mask    */

#define SYS_SRAM_BISTSTS_CRBISTEF_Pos    (2)                                               /*!< SYS_T::SRAM_BISTSTS: CRBISTEF Position */
#define SYS_SRAM_BISTSTS_CRBISTEF_Msk    (0x1ul << SYS_SRAM_BISTSTS_CRBISTEF_Pos)          /*!< SYS_T::SRAM_BISTSTS: CRBISTEF Mask     */

#define SYS_SRAM_BISTSTS_CANBEF_Pos      (3)                                               /*!< SYS_T::SRAM_BISTSTS: CANBEF Position   */
#define SYS_SRAM_BISTSTS_CANBEF_Msk      (0x1ul << SYS_SRAM_BISTSTS_CANBEF_Pos)            /*!< SYS_T::SRAM_BISTSTS: CANBEF Mask       */

#define SYS_SRAM_BISTSTS_USBBEF_Pos      (4)                                               /*!< SYS_T::SRAM_BISTSTS: USBBEF Position   */
#define SYS_SRAM_BISTSTS_USBBEF_Msk      (0x1ul << SYS_SRAM_BISTSTS_USBBEF_Pos)            /*!< SYS_T::SRAM_BISTSTS: USBBEF Mask       */

#define SYS_SRAM_BISTSTS_SRBEND0_Pos     (16)                                              /*!< SYS_T::SRAM_BISTSTS: SRBEND0 Position  */
#define SYS_SRAM_BISTSTS_SRBEND0_Msk     (0x1ul << SYS_SRAM_BISTSTS_SRBEND0_Pos)           /*!< SYS_T::SRAM_BISTSTS: SRBEND0 Mask      */

#define SYS_SRAM_BISTSTS_SRBEND1_Pos     (17)                                              /*!< SYS_T::SRAM_BISTSTS: SRBEND1 Position  */
#define SYS_SRAM_BISTSTS_SRBEND1_Msk     (0x1ul << SYS_SRAM_BISTSTS_SRBEND1_Pos)           /*!< SYS_T::SRAM_BISTSTS: SRBEND1 Mask      */

#define SYS_SRAM_BISTSTS_CRBEND_Pos      (18)                                              /*!< SYS_T::SRAM_BISTSTS: CRBEND Position   */
#define SYS_SRAM_BISTSTS_CRBEND_Msk      (0x1ul << SYS_SRAM_BISTSTS_CRBEND_Pos)            /*!< SYS_T::SRAM_BISTSTS: CRBEND Mask       */

#define SYS_SRAM_BISTSTS_CANBEND_Pos     (19)                                              /*!< SYS_T::SRAM_BISTSTS: CANBEND Position  */
#define SYS_SRAM_BISTSTS_CANBEND_Msk     (0x1ul << SYS_SRAM_BISTSTS_CANBEND_Pos)           /*!< SYS_T::SRAM_BISTSTS: CANBEND Mask      */

#define SYS_SRAM_BISTSTS_USBBEND_Pos     (20)                                              /*!< SYS_T::SRAM_BISTSTS: USBBEND Position  */
#define SYS_SRAM_BISTSTS_USBBEND_Msk     (0x1ul << SYS_SRAM_BISTSTS_USBBEND_Pos)           /*!< SYS_T::SRAM_BISTSTS: USBBEND Mask      */

#define SYS_HIRCTCTL_FREQSEL_Pos         (0)                                               /*!< SYS_T::HIRCTCTL: FREQSEL Position      */
#define SYS_HIRCTCTL_FREQSEL_Msk         (0x3ul << SYS_HIRCTCTL_FREQSEL_Pos)               /*!< SYS_T::HIRCTCTL: FREQSEL Mask          */

#define SYS_HIRCTCTL_LOOPSEL_Pos         (4)                                               /*!< SYS_T::HIRCTCTL: LOOPSEL Position      */
#define SYS_HIRCTCTL_LOOPSEL_Msk         (0x3ul << SYS_HIRCTCTL_LOOPSEL_Pos)               /*!< SYS_T::HIRCTCTL: LOOPSEL Mask          */

#define SYS_HIRCTCTL_RETRYCNT_Pos        (6)                                               /*!< SYS_T::HIRCTCTL: RETRYCNT Position     */
#define SYS_HIRCTCTL_RETRYCNT_Msk        (0x3ul << SYS_HIRCTCTL_RETRYCNT_Pos)              /*!< SYS_T::HIRCTCTL: RETRYCNT Mask         */

#define SYS_HIRCTCTL_CESTOPEN_Pos        (8)                                               /*!< SYS_T::HIRCTCTL: CESTOPEN Position     */
#define SYS_HIRCTCTL_CESTOPEN_Msk        (0x1ul << SYS_HIRCTCTL_CESTOPEN_Pos)              /*!< SYS_T::HIRCTCTL: CESTOPEN Mask         */

#define SYS_HIRCTCTL_BOUNDEN_Pos         (9)                                               /*!< SYS_T::HIRCTCTL: BOUNDEN Position      */
#define SYS_HIRCTCTL_BOUNDEN_Msk         (0x1ul << SYS_HIRCTCTL_BOUNDEN_Pos)               /*!< SYS_T::HIRCTCTL: BOUNDEN Mask          */  

#define SYS_HIRCTCTL_REFCKSEL_Pos        (10)                                              /*!< SYS_T::HIRCTCTL: REFCKSEL Position     */
#define SYS_HIRCTCTL_REFCKSEL_Msk        (0x1ul << SYS_HIRCTCTL_REFCKSEL_Pos)              /*!< SYS_T::HIRCTCTL: REFCKSEL Mask         */

#define SYS_HIRCTCTL_BOUNDARY_Pos        (16)                                              /*!< SYS_T::HIRCTCTL: BOUNDARY Position     */
#define SYS_HIRCTCTL_BOUNDARY_Msk        (0x1ful << SYS_HIRCTCTL_BOUNDARY_Pos)             /*!< SYS_T::HIRCTCTL: BOUNDARY Mask         */

#define SYS_HIRCTIEN_TFAILIEN_Pos        (1)                                               /*!< SYS_T::HIRCTIEN: TFAILIEN Position     */
#define SYS_HIRCTIEN_TFAILIEN_Msk        (0x1ul << SYS_HIRCTIEN_TFAILIEN_Pos)              /*!< SYS_T::HIRCTIEN: TFAILIEN Mask         */

#define SYS_HIRCTIEN_CLKEIEN_Pos         (2)                                               /*!< SYS_T::HIRCTIEN: CLKEIEN Position      */
#define SYS_HIRCTIEN_CLKEIEN_Msk         (0x1ul << SYS_HIRCTIEN_CLKEIEN_Pos)               /*!< SYS_T::HIRCTIEN: CLKEIEN Mask          */

#define SYS_HIRCTISTS_FREQLOCK_Pos       (0)                                               /*!< SYS_T::HIRCTISTS: FREQLOCK Position    */
#define SYS_HIRCTISTS_FREQLOCK_Msk       (0x1ul << SYS_HIRCTISTS_FREQLOCK_Pos)             /*!< SYS_T::HIRCTISTS: FREQLOCK Mask        */

#define SYS_HIRCTISTS_TFAILIF_Pos        (1)                                               /*!< SYS_T::HIRCTISTS: TFAILIF Position     */
#define SYS_HIRCTISTS_TFAILIF_Msk        (0x1ul << SYS_HIRCTISTS_TFAILIF_Pos)              /*!< SYS_T::HIRCTISTS: TFAILIF Mask         */

#define SYS_HIRCTISTS_CLKERRIF_Pos       (2)                                               /*!< SYS_T::HIRCTISTS: CLKERRIF Position    */
#define SYS_HIRCTISTS_CLKERRIF_Msk       (0x1ul << SYS_HIRCTISTS_CLKERRIF_Pos)             /*!< SYS_T::HIRCTISTS: CLKERRIF Mask        */

#define SYS_HIRCTISTS_OVBDIF_Pos         (3)                                               /*!< SYS_T::HIRCTISTS: OVBDIF Position      */
#define SYS_HIRCTISTS_OVBDIF_Msk         (0x1ul << SYS_HIRCTISTS_OVBDIF_Pos)               /*!< SYS_T::HIRCTISTS: OVBDIF Mask          */

#define SYS_IRCTCTL_FREQSEL_Pos          (0)                                               /*!< SYS_T::IRCTCTL: FREQSEL Position       */
#define SYS_IRCTCTL_FREQSEL_Msk          (0x3ul << SYS_IRCTCTL_FREQSEL_Pos)                /*!< SYS_T::IRCTCTL: FREQSEL Mask           */

#define SYS_IRCTCTL_LOOPSEL_Pos          (4)                                               /*!< SYS_T::IRCTCTL: LOOPSEL Position       */
#define SYS_IRCTCTL_LOOPSEL_Msk          (0x3ul << SYS_IRCTCTL_LOOPSEL_Pos)                /*!< SYS_T::IRCTCTL: LOOPSEL Mask           */

#define SYS_IRCTCTL_RETRYCNT_Pos         (6)                                               /*!< SYS_T::IRCTCTL: RETRYCNT Position      */
#define SYS_IRCTCTL_RETRYCNT_Msk         (0x3ul << SYS_IRCTCTL_RETRYCNT_Pos)               /*!< SYS_T::IRCTCTL: RETRYCNT Mask          */

#define SYS_IRCTCTL_CESTOPEN_Pos         (8)                                               /*!< SYS_T::IRCTCTL: CESTOPEN Position      */
#define SYS_IRCTCTL_CESTOPEN_Msk         (0x1ul << SYS_IRCTCTL_CESTOPEN_Pos)               /*!< SYS_T::IRCTCTL: CESTOPEN Mask          */

#define SYS_IRCTCTL_REFCKSEL_Pos         (10)                                              /*!< SYS_T::IRCTCTL: REFCKSEL Position      */
#define SYS_IRCTCTL_REFCKSEL_Msk         (0x1ul << SYS_IRCTCTL_REFCKSEL_Pos)               /*!< SYS_T::IRCTCTL: REFCKSEL Mask          */

#define SYS_IRCTIEN_TFAILIEN_Pos         (1)                                               /*!< SYS_T::IRCTIEN: TFAILIEN Position      */
#define SYS_IRCTIEN_TFAILIEN_Msk         (0x1ul << SYS_IRCTIEN_TFAILIEN_Pos)               /*!< SYS_T::IRCTIEN: TFAILIEN Mask          */

#define SYS_IRCTIEN_CLKEIEN_Pos          (2)                                               /*!< SYS_T::IRCTIEN: CLKEIEN Position       */
#define SYS_IRCTIEN_CLKEIEN_Msk          (0x1ul << SYS_IRCTIEN_CLKEIEN_Pos)                /*!< SYS_T::IRCTIEN: CLKEIEN Mask           */

#define SYS_IRCTISTS_FREQLOCK_Pos        (0)                                               /*!< SYS_T::IRCTISTS: FREQLOCK Position     */
#define SYS_IRCTISTS_FREQLOCK_Msk        (0x1ul << SYS_IRCTISTS_FREQLOCK_Pos)              /*!< SYS_T::IRCTISTS: FREQLOCK Mask         */

#define SYS_IRCTISTS_TFAILIF_Pos         (1)                                               /*!< SYS_T::IRCTISTS: TFAILIF Position      */
#define SYS_IRCTISTS_TFAILIF_Msk         (0x1ul << SYS_IRCTISTS_TFAILIF_Pos)               /*!< SYS_T::IRCTISTS: TFAILIF Mask          */

#define SYS_IRCTISTS_CLKERRIF_Pos        (2)                                               /*!< SYS_T::IRCTISTS: CLKERRIF Position     */
#define SYS_IRCTISTS_CLKERRIF_Msk        (0x1ul << SYS_IRCTISTS_CLKERRIF_Pos)              /*!< SYS_T::IRCTISTS: CLKERRIF Mask         */

#define SYS_REGLCTL_REGLCTL_Pos          (0)                                               /*!< SYS_T::REGLCTL: REGLCTL Position       */
#define SYS_REGLCTL_REGLCTL_Msk          (0x1ul << SYS_REGLCTL_REGLCTL_Pos)                /*!< SYS_T::REGLCTL: REGLCTL Mask           */

#define SYS_PORDISAN_POROFFAN_Pos        (0)                                               /*!< SYS_T::PORDISAN: POROFFAN Position     */
#define SYS_PORDISAN_POROFFAN_Msk        (0xfffful << SYS_PORDISAN_POROFFAN_Pos)           /*!< SYS_T::PORDISAN: POROFFAN Mask         */

#define SYS_CSERVER_VERSION_Pos          (0)                                               /*!< SYS_T::CSERVER: VERSION Position       */
#define SYS_CSERVER_VERSION_Msk          (0xfful << SYS_CSERVER_VERSION_Pos)               /*!< SYS_T::CSERVER: VERSION Mask           */

#define SYS_PLCTL_PLSEL_Pos              (0)                                               /*!< SYS_T::PLCTL: PLSEL Position           */
#define SYS_PLCTL_PLSEL_Msk              (0x3ul << SYS_PLCTL_PLSEL_Pos)                    /*!< SYS_T::PLCTL: PLSEL Mask               */

#define SYS_PLCTL_LVSSTEP_Pos            (16)                                              /*!< SYS_T::PLCTL: LVSSTEP Position         */
#define SYS_PLCTL_LVSSTEP_Msk            (0x3ful << SYS_PLCTL_LVSSTEP_Pos)                 /*!< SYS_T::PLCTL: LVSSTEP Mask             */

#define SYS_PLCTL_LVSPRD_Pos             (24)                                              /*!< SYS_T::PLCTL: LVSPRD Position          */
#define SYS_PLCTL_LVSPRD_Msk             (0xfful << SYS_PLCTL_LVSPRD_Pos)                  /*!< SYS_T::PLCTL: LVSPRD Mask              */

#define SYS_PLSTS_PLCBUSY_Pos            (0)                                               /*!< SYS_T::PLSTS: PLCBUSY Position         */
#define SYS_PLSTS_PLCBUSY_Msk            (0x1ul << SYS_PLSTS_PLCBUSY_Pos)                  /*!< SYS_T::PLSTS: PLCBUSY Mask             */

#define SYS_PLSTS_PLSTATUS_Pos           (8)                                               /*!< SYS_T::PLSTS: PLSTATUS Position        */
#define SYS_PLSTS_PLSTATUS_Msk           (0x3ul << SYS_PLSTS_PLSTATUS_Pos)                 /*!< SYS_T::PLSTS: PLSTATUS Mask            */

#define SYS_AHBMCTL_INTACTEN_Pos         (0)                                               /*!< SYS_T::AHBMCTL: INTACTEN Position      */
#define SYS_AHBMCTL_INTACTEN_Msk         (0x1ul << SYS_AHBMCTL_INTACTEN_Pos)               /*!< SYS_T::AHBMCTL: INTACTEN Mask          */

/**@}*/ /* SYS_CONST */
/**@}*/ /* end of SYS register group */

/**
    @addtogroup NMI NMI Controller (NMI)
    Memory Mapped Structure for NMI Controller
@{ */

typedef struct
{


    /**
     * @var NMI_T::NMIEN
     * Offset: 0x00  NMI Source Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BODOUT    |BOD NMI Source Enable (Write Protect)
     * |        |          |0 = BOD NMI source Disabled.
     * |        |          |1 = BOD NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |IRC_INT   |IRC TRIM NMI Source Enable (Write Protect)
     * |        |          |0 = IRC TRIM NMI source Disabled.
     * |        |          |1 = IRC TRIM NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |PWRWU_INT |Power-down Mode Wake-up NMI Source Enable (Write Protect)
     * |        |          |0 = Power-down mode wake-up NMI source Disabled.
     * |        |          |1 = Power-down mode wake-up NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |SRAM_PERR |SRAM Parity Check NMI Source Enable (Write Protect)
     * |        |          |0 = SRAM parity check error NMI source Disabled.
     * |        |          |1 = SRAM parity check error NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[4]     |CLKFAIL   |Clock Fail Detected and IRC Auto Trim Interrupt NMI Source Enable (Write Protect)
     * |        |          |0 = Clock fail detected and IRC Auto Trim interrupt NMI source Disabled.
     * |        |          |1 = Clock fail detected and IRC Auto Trim interrupt NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |RTC_INT   |RTC NMI Source Enable (Write Protect)
     * |        |          |0 = RTC NMI source Disabled.
     * |        |          |1 = RTC NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7]     |TAMPER_INT|TAMPER_INT NMI Source Enable (Write Protect)
     * |        |          |0 = Backup register tamper detected NMI source Disabled.
     * |        |          |1 = Backup register tamper detected NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |EINT0     |External Interrupt From PA.6 or PB.5 Pin NMI Source Enable (Write Protect)
     * |        |          |0 = External interrupt from PA.6 or PB.5 pin NMI source Disabled.
     * |        |          |1 = External interrupt from PA.6 or PB.5 pin NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[9]     |EINT1     |External Interrupt From PA.7, PB.4 or PD.15 Pin NMI Source Enable (Write Protect)
     * |        |          |0 = External interrupt from PA.7, PB.4 or PD.15 pin NMI source Disabled.
     * |        |          |1 = External interrupt from PA.7, PB.4 or PD.15 pin NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[10]    |EINT2     |External Interrupt From PB.3 or PC.6 Pin NMI Source Enable (Write Protect)
     * |        |          |0 = External interrupt from PB.3 or PC.6 pin NMI source Disabled.
     * |        |          |1 = External interrupt from PB.3 or PC.6 pin NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[11]    |EINT3     |External Interrupt From PB.2 or PC.7 Pin NMI Source Enable (Write Protect)
     * |        |          |0 = External interrupt from PB.2 or PC.7 pin NMI source Disabled.
     * |        |          |1 = External interrupt from PB.2 or PC.7 pin NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[12]    |EINT4     |External Interrupt From PA.8, PB.6 or PF.15 Pin NMI Source Enable (Write Protect)
     * |        |          |0 = External interrupt from PA.8, PB.6 or PF.15 pin NMI source Disabled.
     * |        |          |1 = External interrupt from PA.8, PB.6 or PF.15 pin NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[13]    |EINT5     |External Interrupt From PB.7 or PF.14 Pin NMI Source Enable (Write Protect)
     * |        |          |0 = External interrupt from PB.7 or PF.14 pin NMI source Disabled.
     * |        |          |1 = External interrupt from PB.7 or PF.14 pin NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[14]    |UART0_INT |UART0 NMI Source Enable (Write Protect)
     * |        |          |0 = UART0 NMI source Disabled.
     * |        |          |1 = UART0 NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[15]    |UART1_INT |UART1 NMI Source Enable (Write Protect)
     * |        |          |0 = UART1 NMI source Disabled.
     * |        |          |1 = UART1 NMI source Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var NMI_T::NMISTS
     * Offset: 0x04  NMI Source Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BODOUT    |BOD Interrupt Flag (Read Only)
     * |        |          |0 = BOD interrupt is deasserted.
     * |        |          |1 = BOD interrupt is asserted.
     * |[1]     |IRC_INT   |IRC TRIM Interrupt Flag (Read Only)
     * |        |          |0 = HIRC TRIM interrupt is deasserted.
     * |        |          |1 = HIRC TRIM interrupt is asserted.
     * |[2]     |PWRWU_INT |Power-down Mode Wake-up Interrupt Flag (Read Only)
     * |        |          |0 = Power-down mode wake-up interrupt is deasserted.
     * |        |          |1 = Power-down mode wake-up interrupt is asserted.
     * |[3]     |SRAM_PERR |SRAM ParityCheck Error Interrupt Flag (Read Only)
     * |        |          |0 = SRAM parity check error interrupt is deasserted.
     * |        |          |1 = SRAM parity check error interrupt is asserted.
     * |[4]     |CLKFAIL   |Clock Fail Detected or IRC Auto Trim Interrupt Flag (Read Only)
     * |        |          |0 = Clock fail detected or IRC Auto Trim interrupt is deasserted.
     * |        |          |1 = Clock fail detected or IRC Auto Trim interrupt is asserted.
     * |[6]     |RTC_INT   |RTC Interrupt Flag (Read Only)
     * |        |          |0 = RTC interrupt is deasserted.
     * |        |          |1 = RTC interrupt is asserted.
     * |[7]     |TAMPER_INT|TAMPER_INT Interrupt Flag (Read Only)
     * |        |          |0 = Backup register tamper detected interrupt is deasserted.
     * |        |          |1 = Backup register tamper detected interrupt is asserted.
     * |[8]     |EINT0     |External Interrupt From PA.6 or PB.5 Pin Interrupt Flag (Read Only)
     * |        |          |0 = External Interrupt from PA.6 or PB.5 interrupt is deasserted.
     * |        |          |1 = External Interrupt from PA.6 or PB.5 interrupt is asserted.
     * |[9]     |EINT1     |External Interrupt From PA.7, PB.4 or PD.15 Pin Interrupt Flag (Read Only)
     * |        |          |0 = External Interrupt from PA.7, PB.4 or PD.15 interrupt is deasserted.
     * |        |          |1 = External Interrupt from PA.7, PB.4 or PD.15 interrupt is asserted.
     * |[10]    |EINT2     |External Interrupt From PB.3 or PC.6 Pin Interrupt Flag (Read Only)
     * |        |          |0 = External Interrupt from PB.3 or PC.6 interrupt is deasserted.
     * |        |          |1 = External Interrupt from PB.3 or PC.6 interrupt is asserted.
     * |[11]    |EINT3     |External Interrupt From PB.2 or PC.7 Pin Interrupt Flag (Read Only)
     * |        |          |0 = External Interrupt from PB.2 or PC.7 interrupt is deasserted.
     * |        |          |1 = External Interrupt from PB.2 or PC.7 interrupt is asserted.
     * |[12]    |EINT4     |External Interrupt From PA.8, PB.6 or PF.15 Pin Interrupt Flag (Read Only)
     * |        |          |0 = External Interrupt from PA.8, PB.6 or PF.15 interrupt is deasserted.
     * |        |          |1 = External Interrupt from PA.8, PB.6 or PF.15 interrupt is asserted.
     * |[13]    |EINT5     |External Interrupt From PB.7 or PF.14 Pin Interrupt Flag (Read Only)
     * |        |          |0 = External Interrupt from PB.7 or PF.14 interrupt is deasserted.
     * |        |          |1 = External Interrupt from PB.7 or PF.14 interrupt is asserted.
     * |[14]    |UART0_INT |UART0 Interrupt Flag (Read Only)
     * |        |          |0 = UART1 interrupt is deasserted.
     * |        |          |1 = UART1 interrupt is asserted.
     * |[15]    |UART1_INT |UART1 Interrupt Flag (Read Only)
     * |        |          |0 = UART1 interrupt is deasserted.
     * |        |          |1 = UART1 interrupt is asserted.
     */
    __IO uint32_t NMIEN;                 /*!< [0x0000] NMI Source Interrupt Enable Register                             */
    __I  uint32_t NMISTS;                /*!< [0x0004] NMI Source Interrupt Status Register                             */

} NMI_T;

/**
    @addtogroup NMI_CONST NMI Bit Field Definition
    Constant Definitions for NMI Controller
@{ */

#define NMI_NMIEN_BODOUT_Pos             (0)                                               /*!< NMI_T::NMIEN: BODOUT Position          */
#define NMI_NMIEN_BODOUT_Msk             (0x1ul << NMI_NMIEN_BODOUT_Pos)                   /*!< NMI_T::NMIEN: BODOUT Mask              */

#define NMI_NMIEN_IRC_INT_Pos            (1)                                               /*!< NMI_T::NMIEN: IRC_INT Position         */
#define NMI_NMIEN_IRC_INT_Msk            (0x1ul << NMI_NMIEN_IRC_INT_Pos)                  /*!< NMI_T::NMIEN: IRC_INT Mask             */

#define NMI_NMIEN_PWRWU_INT_Pos          (2)                                               /*!< NMI_T::NMIEN: PWRWU_INT Position       */
#define NMI_NMIEN_PWRWU_INT_Msk          (0x1ul << NMI_NMIEN_PWRWU_INT_Pos)                /*!< NMI_T::NMIEN: PWRWU_INT Mask           */

#define NMI_NMIEN_SRAM_PERR_Pos          (3)                                               /*!< NMI_T::NMIEN: SRAM_PERR Position       */
#define NMI_NMIEN_SRAM_PERR_Msk          (0x1ul << NMI_NMIEN_SRAM_PERR_Pos)                /*!< NMI_T::NMIEN: SRAM_PERR Mask           */

#define NMI_NMIEN_CLKFAIL_Pos            (4)                                               /*!< NMI_T::NMIEN: CLKFAIL Position         */
#define NMI_NMIEN_CLKFAIL_Msk            (0x1ul << NMI_NMIEN_CLKFAIL_Pos)                  /*!< NMI_T::NMIEN: CLKFAIL Mask             */

#define NMI_NMIEN_RTC_INT_Pos            (6)                                               /*!< NMI_T::NMIEN: RTC_INT Position         */
#define NMI_NMIEN_RTC_INT_Msk            (0x1ul << NMI_NMIEN_RTC_INT_Pos)                  /*!< NMI_T::NMIEN: RTC_INT Mask             */

#define NMI_NMIEN_TAMPER_INT_Pos         (7)                                               /*!< NMI_T::NMIEN: TAMPER_INT Position      */
#define NMI_NMIEN_TAMPER_INT_Msk         (0x1ul << NMI_NMIEN_TAMPER_INT_Pos)               /*!< NMI_T::NMIEN: TAMPER_INT Mask          */

#define NMI_NMIEN_EINT0_Pos              (8)                                               /*!< NMI_T::NMIEN: EINT0 Position           */
#define NMI_NMIEN_EINT0_Msk              (0x1ul << NMI_NMIEN_EINT0_Pos)                    /*!< NMI_T::NMIEN: EINT0 Mask               */

#define NMI_NMIEN_EINT1_Pos              (9)                                               /*!< NMI_T::NMIEN: EINT1 Position           */
#define NMI_NMIEN_EINT1_Msk              (0x1ul << NMI_NMIEN_EINT1_Pos)                    /*!< NMI_T::NMIEN: EINT1 Mask               */

#define NMI_NMIEN_EINT2_Pos              (10)                                              /*!< NMI_T::NMIEN: EINT2 Position           */
#define NMI_NMIEN_EINT2_Msk              (0x1ul << NMI_NMIEN_EINT2_Pos)                    /*!< NMI_T::NMIEN: EINT2 Mask               */

#define NMI_NMIEN_EINT3_Pos              (11)                                              /*!< NMI_T::NMIEN: EINT3 Position           */
#define NMI_NMIEN_EINT3_Msk              (0x1ul << NMI_NMIEN_EINT3_Pos)                    /*!< NMI_T::NMIEN: EINT3 Mask               */

#define NMI_NMIEN_EINT4_Pos              (12)                                              /*!< NMI_T::NMIEN: EINT4 Position           */
#define NMI_NMIEN_EINT4_Msk              (0x1ul << NMI_NMIEN_EINT4_Pos)                    /*!< NMI_T::NMIEN: EINT4 Mask               */

#define NMI_NMIEN_EINT5_Pos              (13)                                              /*!< NMI_T::NMIEN: EINT5 Position           */
#define NMI_NMIEN_EINT5_Msk              (0x1ul << NMI_NMIEN_EINT5_Pos)                    /*!< NMI_T::NMIEN: EINT5 Mask               */

#define NMI_NMIEN_UART0_INT_Pos          (14)                                              /*!< NMI_T::NMIEN: UART0_INT Position       */
#define NMI_NMIEN_UART0_INT_Msk          (0x1ul << NMI_NMIEN_UART0_INT_Pos)                /*!< NMI_T::NMIEN: UART0_INT Mask           */

#define NMI_NMIEN_UART1_INT_Pos          (15)                                              /*!< NMI_T::NMIEN: UART1_INT Position       */
#define NMI_NMIEN_UART1_INT_Msk          (0x1ul << NMI_NMIEN_UART1_INT_Pos)                /*!< NMI_T::NMIEN: UART1_INT Mask           */

#define NMI_NMISTS_BODOUT_Pos            (0)                                               /*!< NMI_T::NMISTS: BODOUT Position         */
#define NMI_NMISTS_BODOUT_Msk            (0x1ul << NMI_NMISTS_BODOUT_Pos)                  /*!< NMI_T::NMISTS: BODOUT Mask             */

#define NMI_NMISTS_IRC_INT_Pos           (1)                                               /*!< NMI_T::NMISTS: IRC_INT Position        */
#define NMI_NMISTS_IRC_INT_Msk           (0x1ul << NMI_NMISTS_IRC_INT_Pos)                 /*!< NMI_T::NMISTS: IRC_INT Mask            */

#define NMI_NMISTS_PWRWU_INT_Pos         (2)                                               /*!< NMI_T::NMISTS: PWRWU_INT Position      */
#define NMI_NMISTS_PWRWU_INT_Msk         (0x1ul << NMI_NMISTS_PWRWU_INT_Pos)               /*!< NMI_T::NMISTS: PWRWU_INT Mask          */

#define NMI_NMISTS_SRAM_PERR_Pos         (3)                                               /*!< NMI_T::NMISTS: SRAM_PERR Position      */
#define NMI_NMISTS_SRAM_PERR_Msk         (0x1ul << NMI_NMISTS_SRAM_PERR_Pos)               /*!< NMI_T::NMISTS: SRAM_PERR Mask          */

#define NMI_NMISTS_CLKFAIL_Pos           (4)                                               /*!< NMI_T::NMISTS: CLKFAIL Position        */
#define NMI_NMISTS_CLKFAIL_Msk           (0x1ul << NMI_NMISTS_CLKFAIL_Pos)                 /*!< NMI_T::NMISTS: CLKFAIL Mask            */

#define NMI_NMISTS_RTC_INT_Pos           (6)                                               /*!< NMI_T::NMISTS: RTC_INT Position        */
#define NMI_NMISTS_RTC_INT_Msk           (0x1ul << NMI_NMISTS_RTC_INT_Pos)                 /*!< NMI_T::NMISTS: RTC_INT Mask            */

#define NMI_NMISTS_TAMPER_INT_Pos        (7)                                               /*!< NMI_T::NMISTS: TAMPER_INT Position     */
#define NMI_NMISTS_TAMPER_INT_Msk        (0x1ul << NMI_NMISTS_TAMPER_INT_Pos)              /*!< NMI_T::NMISTS: TAMPER_INT Mask         */

#define NMI_NMISTS_EINT0_Pos             (8)                                               /*!< NMI_T::NMISTS: EINT0 Position          */
#define NMI_NMISTS_EINT0_Msk             (0x1ul << NMI_NMISTS_EINT0_Pos)                   /*!< NMI_T::NMISTS: EINT0 Mask              */

#define NMI_NMISTS_EINT1_Pos             (9)                                               /*!< NMI_T::NMISTS: EINT1 Position          */
#define NMI_NMISTS_EINT1_Msk             (0x1ul << NMI_NMISTS_EINT1_Pos)                   /*!< NMI_T::NMISTS: EINT1 Mask              */

#define NMI_NMISTS_EINT2_Pos             (10)                                              /*!< NMI_T::NMISTS: EINT2 Position          */
#define NMI_NMISTS_EINT2_Msk             (0x1ul << NMI_NMISTS_EINT2_Pos)                   /*!< NMI_T::NMISTS: EINT2 Mask              */

#define NMI_NMISTS_EINT3_Pos             (11)                                              /*!< NMI_T::NMISTS: EINT3 Position          */
#define NMI_NMISTS_EINT3_Msk             (0x1ul << NMI_NMISTS_EINT3_Pos)                   /*!< NMI_T::NMISTS: EINT3 Mask              */

#define NMI_NMISTS_EINT4_Pos             (12)                                              /*!< NMI_T::NMISTS: EINT4 Position          */
#define NMI_NMISTS_EINT4_Msk             (0x1ul << NMI_NMISTS_EINT4_Pos)                   /*!< NMI_T::NMISTS: EINT4 Mask              */

#define NMI_NMISTS_EINT5_Pos             (13)                                              /*!< NMI_T::NMISTS: EINT5 Position          */
#define NMI_NMISTS_EINT5_Msk             (0x1ul << NMI_NMISTS_EINT5_Pos)                   /*!< NMI_T::NMISTS: EINT5 Mask              */

#define NMI_NMISTS_UART0_INT_Pos         (14)                                              /*!< NMI_T::NMISTS: UART0_INT Position      */
#define NMI_NMISTS_UART0_INT_Msk         (0x1ul << NMI_NMISTS_UART0_INT_Pos)               /*!< NMI_T::NMISTS: UART0_INT Mask          */

#define NMI_NMISTS_UART1_INT_Pos         (15)                                              /*!< NMI_T::NMISTS: UART1_INT Position      */
#define NMI_NMISTS_UART1_INT_Msk         (0x1ul << NMI_NMISTS_UART1_INT_Pos)               /*!< NMI_T::NMISTS: UART1_INT Mask          */

/**@}*/ /* NMI_CONST */
/**@}*/ /* end of NMI register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __SYS_REG_H__ */
