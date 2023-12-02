/**************************************************************************//**
 * @file     uspi_reg.h
 * @version  V1.00
 * @brief    USPI register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __USPI_REG_H__
#define __USPI_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup USPI SPI Mode of USCI Controller(USPI)
    Memory Mapped Structure for USPI Controller
@{ */

typedef struct
{


    /**
     * @var USPI_T::CTL
     * Offset: 0x00  USCI Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |FUNMODE   |Function Mode
     * |        |          |This bit field selects the protocol for this USCI controller
     * |        |          |Selecting a protocol that is not available or a reserved combination disables the USCI
     * |        |          |When switching between two protocols, the USCI has to be disabled before selecting a new protocol
     * |        |          |Simultaneously, the USCI will be reset when user write 000 to FUNMODE.
     * |        |          |000 = The USCI is disabled. All protocol related state machines are set to idle state.
     * |        |          |001 = The SPI protocol is selected.
     * |        |          |010 = The UART protocol is selected.
     * |        |          |100 = The I2C protocol is selected.
     * |        |          |Note: Other bit combinations are reserved.
     * @var USPI_T::INTEN
     * Offset: 0x04  USCI Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TXSTIEN   |Transmit Start Interrupt Enable Bit
     * |        |          |This bit enables the interrupt generation in case of a transmit start event.
     * |        |          |0 = The transmit start interrupt is disabled.
     * |        |          |1 = The transmit start interrupt is enabled.
     * |[2]     |TXENDIEN  |Transmit End Interrupt Enable Bit
     * |        |          |This bit enables the interrupt generation in case of a transmit finish event.
     * |        |          |0 = The transmit finish interrupt is disabled.
     * |        |          |1 = The transmit finish interrupt is enabled.
     * |[3]     |RXSTIEN   |Receive Start Interrupt Enable Bit
     * |        |          |This bit enables the interrupt generation in case of a receive start event.
     * |        |          |0 = The receive start interrupt is disabled.
     * |        |          |1 = The receive start interrupt is enabled.
     * |[4]     |RXENDIEN  |Receive End Interrupt Enable Bit
     * |        |          |This bit enables the interrupt generation in case of a receive finish event.
     * |        |          |0 = The receive end interrupt is disabled.
     * |        |          |1 = The receive end interrupt is enabled.
     * @var USPI_T::BRGEN
     * Offset: 0x08  USCI Baud Rate Generator Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RCLKSEL   |Reference Clock Source Selection
     * |        |          |This bit selects the source of reference clock (fREF_CLK).
     * |        |          |0 = Peripheral device clock fPCLK.
     * |        |          |1 = Reserved.
     * |[1]     |PTCLKSEL  |Protocol Clock Source Selection
     * |        |          |This bit selects the source of protocol clock (fPROT_CLK).
     * |        |          |0 = Reference clock fREF_CLK.
     * |        |          |1 = fREF_CLK2 (its frequency is half of fREF_CLK).
     * |[3:2]   |SPCLKSEL  |Sample Clock Source Selection
     * |        |          |This bit field used for the clock source selection of sample clock (fSAMP_CLK) for the protocol processor.
     * |        |          |00 = fDIV_CLK.
     * |        |          |01 = fPROT_CLK.
     * |        |          |10 = fSCLK.
     * |        |          |11 = fREF_CLK.
     * |[4]     |TMCNTEN   |Time Measurement Counter Enable Bit
     * |        |          |This bit enables the 10-bit timing measurement counter.
     * |        |          |0 = Time measurement counter is Disabled.
     * |        |          |1 = Time measurement counter is Enabled.
     * |[5]     |TMCNTSRC  |Time Measurement Counter Clock Source Selection
     * |        |          |0 = Time measurement counter with fPROT_CLK.
     * |        |          |1 = Time measurement counter with fDIV_CLK.
     * |[25:16] |CLKDIV    |Clock Divider
     * |        |          |This bit field defines the ratio between the protocol clock frequency fPROT_CLK and the clock divider frequency fDIV_CLK (fDIV_CLK = fPROT_CLK / (CLKDIV+1) ).
     * |        |          |Note: In UART function, it can be updated by hardware in the 4th falling edge of the input data 0x55 when the auto baud rate function (ABREN(USPI_PROTCTL[6])) is enabled
     * |        |          |The revised value is the average bit time between bit 5 and bit 6
     * |        |          |The user can use revised CLKDIV and new BRDETITV (USPI_PROTCTL[24:16]) to calculate the precise baud rate.
     * @var USPI_T::DATIN0
     * Offset: 0x10  USCI Input Data Signal Configuration Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SYNCSEL   |Input Signal Synchronization Selection
     * |        |          |This bit selects if the un-synchronized input signal (with optionally inverted) or the   synchronized (and optionally filtered) signal can be used as input for the   data shift unit.
     * |        |          |0 = The un-synchronized signal can be taken as input for the data shift unit.
     * |        |          |1 = The synchronized signal can be taken as input for the data shift unit.
     * |        |          |Note: In SPI protocol, we suggest this bit   should be set as 0.
     * |[2]     |ININV     |Input Signal Inverse Selection
     * |        |          |This bit defines the inverter enable of the input asynchronous signal.
     * |        |          |0 = The un-synchronized input signal will not be inverted.
     * |        |          |1 = The un-synchronized input signal will be inverted.
     * |        |          |Note: In SPI protocol, we suggest this bit   should be set as 0.
     * @var USPI_T::CTLIN0
     * Offset: 0x20  USCI Input Control Signal Configuration Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SYNCSEL   |Input Synchronization Signal Selection
     * |        |          |This bit selects if the un-synchronized input signal (with optionally inverted) or the   synchronized (and optionally filtered) signal can be used as input for the   data shift unit.
     * |        |          |0 = The un-synchronized signal can be taken as input for the data shift unit.
     * |        |          |1 = The synchronized signal can be taken as input for the data shift unit.
     * |        |          |Note: In SPI protocol, we suggest this bit   should be set as 0.
     * |[2]     |ININV     |Input Signal Inverse Selection
     * |        |          |This bit defines the inverter enable of the input asynchronous signal.
     * |        |          |0 = The un-synchronized input signal will not be inverted.
     * |        |          |1 = The un-synchronized input signal will be inverted.
     * @var USPI_T::CLKIN
     * Offset: 0x28  USCI Input Clock Signal Configuration Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SYNCSEL   |Input Synchronization Signal Selection
     * |        |          |This bit selects if the un-synchronized input signal or the synchronized (and   optionally filtered) signal can be used as input for the data shift unit.
     * |        |          |0 = The un-synchronized signal can be taken as input for the data shift unit.
     * |        |          |1 = The synchronized signal can be taken as input for the data shift unit.
     * |        |          |Note: In SPI protocol, we suggest this bit   should be set as 0.
     * @var USPI_T::LINECTL
     * Offset: 0x2C  USCI Line Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LSB       |LSB First Transmission Selection
     * |        |          |0 = The MSB, which bit of transmit/receive data buffer depends on the setting of DWIDTH, is transmitted/received first.
     * |        |          |1 = The LSB, the bit 0 of data buffer, will be transmitted/received first.
     * |[5]     |DATOINV   |Data Output Inverse Selection
     * |        |          |This bit defines the relation between the internal shift data value and the output data signal of USCIx_DAT0/1 pin.
     * |        |          |0 = Data output level is not inverted.
     * |        |          |1 = Data output level is inverted.
     * |[7]     |CTLOINV   |Control Signal Output Inverse Selection
     * |        |          |This bit defines the relation between the internal control signal and the output control signal.
     * |        |          |0 = No effect.
     * |        |          |1 = The control signal will be inverted before its output.
     * |        |          |Note: The control signal has different definitions in different protocol
     * |        |          |In SPI protocol, the control signal means slave select signal
     * |[11:8]  |DWIDTH    |Word Length of Transmission
     * |        |          |This bit field defines the data word length (amount of bits) for reception and transmission
     * |        |          |The data word is always right-aligned in the data buffer
     * |        |          |USCI support word length from 4 to 16 bits.
     * |        |          |0x0: The data word contains 16 bits located at bit positions [15:0].
     * |        |          |0x1: Reserved.
     * |        |          |0x2: Reserved.
     * |        |          |0x3: Reserved.
     * |        |          |0x4: The data word contains 4 bits located at bit positions [3:0].
     * |        |          |0x5: The data word contains 5 bits located at bit positions [4:0].
     * |        |          |...
     * |        |          |0xF: The data word contains 15 bits located at bit positions [14:0].
     * @var USPI_T::TXDAT
     * Offset: 0x30  USCI Transmit Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TXDAT     |Transmit Data
     * |        |          |Software can use this bit field to write 16-bit transmit data for transmission
     * |        |          |In order to avoid overwriting the transmit data, user have to check TXEMPTY (USPI_BUFSTS[8]) status before writing transmit data into this bit field.
     * |[16]    |PORTDIR   |Port Direction Control
     * |        |          |This bit field is only available while USCI operates in SPI protocol (FUNMODE = 0x1) with half-duplex transfer
     * |        |          |It is used to define the direction of the data port pin
     * |        |          |When software writes USPI_TXDAT register, the transmit data and its port direction are settled simultaneously.
     * |        |          |0 = The data pin is configured as output mode.
     * |        |          |1 = The data pin is configured as input mode.
     * @var USPI_T::RXDAT
     * Offset: 0x34  USCI Receive Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RXDAT     |Received Data
     * |        |          |This bit field monitors the received data which stored in receive data buffer.
     * @var USPI_T::BUFCTL
     * Offset: 0x38  USCI Transmit/Receive Buffer Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[6]     |TXUDRIEN  |Slave Transmit Under Run Interrupt Enable Bit
     * |        |          |0 = Transmit under-run interrupt Disabled.
     * |        |          |1 = Transmit under-run interrupt Enabled.
     * |[7]     |TXCLR     |Clear Transmit Buffer
     * |        |          |0 = No effect.
     * |        |          |1 = The transmit buffer is cleared
     * |        |          |Should only be used while the buffer is not taking part in data traffic.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * |[14]    |RXOVIEN   |Receive Buffer Overrun Interrupt Enable Bit
     * |        |          |0 = Receive overrun interrupt Disabled.
     * |        |          |1 = Receive overrun interrupt Enabled.
     * |[15]    |RXCLR     |Clear Receive Buffer
     * |        |          |0 = No effect.
     * |        |          |1 = The receive buffer is cleared
     * |        |          |Should only be used while the buffer is not taking part in data traffic.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * |[16]    |TXRST     |Transmit Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the transmit-related counters, state machine, and the content of transmit shift register and data buffer.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * |[17]    |RXRST     |Receive Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the receive-related counters, state machine, and the content of receive shift register and data buffer.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * @var USPI_T::BUFSTS
     * Offset: 0x3C  USCI Transmit/Receive Buffer Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RXEMPTY   |Receive Buffer Empty Indicator
     * |        |          |0 = Receive buffer is not empty.
     * |        |          |1 = Receive buffer is empty.
     * |[1]     |RXFULL    |Receive Buffer Full Indicator
     * |        |          |0 = Receive buffer is not full.
     * |        |          |1 = Receive buffer is full.
     * |[3]     |RXOVIF    |Receive Buffer Over-run Interrupt Status
     * |        |          |This bit indicates that a receive buffer overrun event has been detected
     * |        |          |If RXOVIEN (USPI_BUFCTL[14]) is enabled, the corresponding interrupt request is activated
     * |        |          |It is cleared by software writes 1 to this bit.
     * |        |          |0 = A receive buffer overrun event has not been detected.
     * |        |          |1 = A receive buffer overrun event has been detected.
     * |[8]     |TXEMPTY   |Transmit Buffer Empty Indicator
     * |        |          |0 = Transmit buffer is not empty.
     * |        |          |1 = Transmit buffer is empty and available for the next transmission datum.
     * |[9]     |TXFULL    |Transmit Buffer Full Indicator
     * |        |          |0 = Transmit buffer is not full.
     * |        |          |1 = Transmit buffer is full.
     * |[11]    |TXUDRIF   |Transmit Buffer Under-run Interrupt Status
     * |        |          |This bit indicates that a transmit buffer under-run event has been detected
     * |        |          |If enabled by TXUDRIEN (USPI_BUFCTL[6]), the corresponding interrupt request is activated
     * |        |          |It is cleared by software writes 1 to this bit
     * |        |          |0 = A transmit buffer under-run event has not been detected.
     * |        |          |1 = A transmit buffer under-run event has been detected.
     * @var USPI_T::PDMACTL
     * Offset: 0x40  USCI PDMA Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PDMARST   |PDMA Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the USCI's PDMA control logic. This bit will be cleared to 0 automatically.
     * |[1]     |TXPDMAEN  |PDMA Transmit Channel Available
     * |        |          |0 = Transmit PDMA function Disabled.
     * |        |          |1 = Transmit PDMA function Enabled.
     * |[2]     |RXPDMAEN  |PDMA Receive Channel Available
     * |        |          |0 = Receive PDMA function Disabled.
     * |        |          |1 = Receive PDMA function Enabled.
     * |[3]     |PDMAEN    |PDMA Mode Enable Bit
     * |        |          |0 = PDMA function Disabled.
     * |        |          |1 = PDMA function Enabled.
     * |        |          |Notice: The I2C is not supporting PDMA function.
     * @var USPI_T::WKCTL
     * Offset: 0x54  USCI Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |Wake-up Enable Bit
     * |        |          |0 = Wake-up function Disabled.
     * |        |          |1 = Wake-up function Enabled.
     * |[1]     |WKADDREN  |Wake-up Address Match Enable Bit
     * |        |          |0 = The chip is woken up according data toggle.
     * |        |          |1 = The chip is woken up according address match.
     * |[2]     |PDBOPT    |Power Down Blocking Option
     * |        |          |0 = If user attempts to enter Power-down mode by executing WFI while the protocol is in transferring, MCU will stop the transfer and enter Power-down mode immediately.
     * |        |          |1 = If user attempts to enter Power-down mode by executing WFI while the protocol is in transferring, the on-going transfer will not be stopped and MCU will enter idle mode immediately.
     * @var USPI_T::WKSTS
     * Offset: 0x58  USCI Wake-up Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKF       |Wake-up Flag
     * |        |          |When chip is woken up from Power-down mode, this bit is set to 1
     * |        |          |Software can write 1 to clear this bit.
     * @var USPI_T::PROTCTL
     * Offset: 0x5C  USCI Protocol Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SLAVE     |Slave Mode Selection
     * |        |          |0 = Master mode.
     * |        |          |1 = Slave mode.
     * |[1]     |SLV3WIRE  |Slave 3-wire Mode Selection (Slave Only)
     * |        |          |The SPI protocol can work with 3-wire interface (without slave select signal) in Slave mode.
     * |        |          |0 = 4-wire bi-direction interface.
     * |        |          |1 = 3-wire bi-direction interface.
     * |[2]     |SS        |Slave Select Control (Master Only)
     * |        |          |If AUTOSS bit is cleared, setting this bit to 1 will set the slave select signal to active state, and setting this bit to 0 will set the slave select signal back to inactive state.
     * |        |          |If the AUTOSS function is enabled (AUTOSS = 1), the setting value of this bit will not affect the current state of slave select signal.
     * |        |          |Note: In SPI protocol, the internal slave select signal is active high.
     * |[3]     |AUTOSS    |Automatic Slave Select Function Enable (Master Only)
     * |        |          |0 = Slave select signal will be controlled by the setting value of SS (USPI_PROTCTL[2]) bit.
     * |        |          |1 = Slave select signal will be generated automatically
     * |        |          |The slave select signal will be asserted by the SPI controller when transmit/receive is started, and will be de-asserted after each transmit/receive is finished.
     * |[7:6]   |SCLKMODE  |Serial Bus Clock Mode
     * |        |          |This bit field defines the SCLK idle status, data transmit, and data receive edge.
     * |        |          |MODE0 = The idle state of SPI clock is low level
     * |        |          |Data is transmitted with falling edge and received with rising edge.
     * |        |          |MODE1 = The idle state of SPI clock is low level
     * |        |          |Data is transmitted with rising edge and received with falling edge.
     * |        |          |MODE2 = The idle state of SPI clock is high level
     * |        |          |Data is transmitted with rising edge and received with falling edge.
     * |        |          |MODE3 = The idle state of SPI clock is high level
     * |        |          |Data is transmitted with falling edge and received with rising edge.
     * |[11:8]  |SUSPITV   |Suspend Interval (Master Only)
     * |        |          |This bit field provides the configurable suspend interval between two successive transmit/receive transaction in a transfer
     * |        |          |The definition of the suspend interval is the interval between the last clock edge of the preceding transaction word and the first clock edge of the following transaction word
     * |        |          |The default value is 0x3
     * |        |          |The period of the suspend interval is obtained according to the following equation.
     * |        |          |(SUSPITV[3:0] + 0.5) * period of SPI_CLK clock cycle
     * |        |          |Example:
     * |        |          |SUSPITV = 0x0 ... 0.5 SPI_CLK clock cycle.
     * |        |          |SUSPITV = 0x1 ... 1.5 SPI_CLK clock cycle.
     * |        |          |.....
     * |        |          |SUSPITV = 0xE ... 14.5 SPI_CLK clock cycle.
     * |        |          |SUSPITV = 0xF ... 15.5 SPI_CLK clock cycle.
     * |[14:12] |TSMSEL    |Transmit Data Mode Selection
     * |        |          |This bit field describes how receive and transmit data is shifted in and out.
     * |        |          |TSMSEL = 000b: Full-duplex SPI.
     * |        |          |TSMSEL = 100b: Half-duplex SPI.
     * |        |          |Other values are reserved.
     * |        |          |Note: Changing the value of this bit field will produce the TXRST and RXRST to clear the TX/RX data buffer automatically.
     * |[25:16] |SLVTOCNT  |Slave Mode Time-out Period (Slave Only)
     * |        |          |In Slave mode, this bit field is used for Slave time-out period
     * |        |          |This bit field indicates how many clock periods (selected by TMCNTSRC, USPI_BRGEN[5]) between the two edges of input SCLK will assert the Slave time-out event
     * |        |          |Writing 0x0 into this bit field will disable the Slave time-out function.
     * |        |          |Example: Assume SLVTOCNT is 0x0A and TMCNTSRC (USPI_BRGEN[5]) is 1, it means the time-out event will occur if the state of SPI bus clock pin is not changed more than (10+1) periods of fDIV_CLK.
     * |[28]    |TXUDRPOL  |Transmit Under-run Data Polarity (for Slave)
     * |        |          |This bit defines the transmitting data level when no data is available for transferring.
     * |        |          |0 = The output data level is 0 if TX under run event occurs.
     * |        |          |1 = The output data level is 1 if TX under run event occurs.
     * |[31]    |PROTEN    |SPI Protocol Enable Bit
     * |        |          |0 = SPI Protocol Disabled.
     * |        |          |1 = SPI Protocol Enabled.
     * @var USPI_T::PROTIEN
     * Offset: 0x60  USCI Protocol Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SSINAIEN  |Slave Select Inactive Interrupt Enable Control
     * |        |          |This bit enables/disables the generation of a slave select interrupt if the slave select changes to inactive.
     * |        |          |0 = Slave select inactive interrupt generation Disabled.
     * |        |          |1 = Slave select inactive interrupt generation Enabled.
     * |[1]     |SSACTIEN  |Slave Select Active Interrupt Enable Control
     * |        |          |This bit enables/disables the generation of a slave select interrupt if the slave select changes to active.
     * |        |          |0 = Slave select active interrupt generation Disabled.
     * |        |          |1 = Slave select active interrupt generation Enabled.
     * |[2]     |SLVTOIEN  |Slave Time-out Interrupt Enable Control
     * |        |          |In SPI protocol, this bit enables the interrupt generation in case of a Slave time-out event.
     * |        |          |0 = The Slave time-out interrupt Disabled.
     * |        |          |1 = The Slave time-out interrupt Enabled.
     * |[3]     |SLVBEIEN  |Slave Mode Bit Count Error Interrupt Enable Control
     * |        |          |If data transfer is terminated by slave time-out or slave select inactive event in Slave mode, so that the transmit/receive data bit count does not match the setting of DWIDTH (USPI_LINECTL[11:8])
     * |        |          |Bit count error event occurs.
     * |        |          |0 = The Slave mode bit count error interrupt Disabled.
     * |        |          |1 = The Slave mode bit count error interrupt Enabled.
     * @var USPI_T::PROTSTS
     * Offset: 0x64  USCI Protocol Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TXSTIF    |Transmit Start Interrupt Flag
     * |        |          |0 = Transmit start event does not occur.
     * |        |          |1 = Transmit start event occurs.
     * |        |          |Note: It is cleared by software writes 1 to this bit
     * |[2]     |TXENDIF   |Transmit End Interrupt Flag
     * |        |          |0 = Transmit end event does not occur.
     * |        |          |1 = Transmit end event occurs.
     * |        |          |Note: It is cleared by software writes 1 to this bit
     * |[3]     |RXSTIF    |Receive Start Interrupt Flag
     * |        |          |0 = Receive start event does not occur.
     * |        |          |1 = Receive start event occurs.
     * |        |          |Note: It is cleared by software writes 1 to this bit
     * |[4]     |RXENDIF   |Receive End Interrupt Flag
     * |        |          |0 = Receive end event does not occur.
     * |        |          |1 = Receive end event occurs.
     * |        |          |Note: It is cleared by software writes 1 to this bit
     * |[5]     |SLVTOIF   |Slave Time-out Interrupt Flag (for Slave Only)
     * |        |          |0 = Slave time-out event does not occur.
     * |        |          |1 = Slave time-out event occurs.
     * |        |          |Note: It is cleared by software writes 1 to this bit
     * |[6]     |SLVBEIF   |Slave Bit Count Error Interrupt Flag (for Slave Only)
     * |        |          |0 = Slave bit count error event does not occur.
     * |        |          |1 = Slave bit count error event occurs.
     * |        |          |Note: It is cleared by software writes 1 to this bit.
     * |[8]     |SSINAIF   |Slave Select Inactive Interrupt Flag (for Slave Only)
     * |        |          |This bit indicates that the internal slave select signal has changed to inactive
     * |        |          |It is cleared by software writes 1 to this bit
     * |        |          |0 = The slave select signal has not changed to inactive.
     * |        |          |1 = The slave select signal has changed to inactive.
     * |        |          |Note: The internal slave select signal is active high.
     * |[9]     |SSACTIF   |Slave Select Active Interrupt Flag (for Slave Only)
     * |        |          |This bit indicates that the internal slave select signal has changed to active
     * |        |          |It is cleared by software writes one to this bit
     * |        |          |0 = The slave select signal has not changed to active.
     * |        |          |1 = The slave select signal has changed to active.
     * |        |          |Note: The internal slave select signal is active high.
     * |[16]    |SSLINE    |Slave Select Line Bus Status (Read Only)
     * |        |          |This bit is only available in Slave mode
     * |        |          |It used to monitor the current status of the input slave select signal on the bus.
     * |        |          |0 = The slave select line status is 0.
     * |        |          |1 = The slave select line status is 1.
     * |[17]    |BUSY      |Busy Status (Read Only)
     * |        |          |0 = SPI is in idle state.
     * |        |          |1 = SPI is in busy state.
     * |        |          |The following listing are the bus busy conditions:
     * |        |          |a. USPI_PROTCTL[31] = 1 and the TXEMPTY = 0.
     * |        |          |b. For SPI Master mode, the TXEMPTY = 1 but the current transaction is not finished yet.
     * |        |          |c. For SPI Slave mode, the USPI_PROTCTL[31] = 1 and there is serial clock input into the SPI core logic when slave select is active.
     * |        |          |d. For SPI Slave mode, the USPI_PROTCTL[31] = 1 and the transmit buffer or transmit shift register is not empty even if the slave select is inactive.
     * |[18]    |SLVUDR    |Slave Mode Transmit Under-run Status (Read Only)
     * |        |          |In Slave mode, if there is no available transmit data in buffer while transmit data shift out caused by input serial bus clock, this status flag will be set to 1
     * |        |          |This bit indicates whether the current shift-out data of word transmission is switched to TXUDRPOL (USPI_PROTCTL[28]) or not.
     * |        |          |0 = Slave transmit under-run event does not occur.
     * |        |          |1 = Slave transmit under-run event occurs.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] USCI Control Register                                            */
    __IO uint32_t INTEN;                 /*!< [0x0004] USCI Interrupt Enable Register                                   */
    __IO uint32_t BRGEN;                 /*!< [0x0008] USCI Baud Rate Generator Register                                */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DATIN0;                /*!< [0x0010] USCI Input Data Signal Configuration Register 0                  */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[3];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CTLIN0;                /*!< [0x0020] USCI Input Control Signal Configuration Register 0               */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CLKIN;                 /*!< [0x0028] USCI Input Clock Signal Configuration Register                   */
    __IO uint32_t LINECTL;               /*!< [0x002c] USCI Line Control Register                                       */
    __O  uint32_t TXDAT;                 /*!< [0x0030] USCI Transmit Data Register                                      */
    __I  uint32_t RXDAT;                 /*!< [0x0034] USCI Receive Data Register                                       */
    __IO uint32_t BUFCTL;                /*!< [0x0038] USCI Transmit/Receive Buffer Control Register                    */
    __IO uint32_t BUFSTS;                /*!< [0x003c] USCI Transmit/Receive Buffer Status Register                     */
    __IO uint32_t PDMACTL;               /*!< [0x0040] USCI PDMA Control Register                                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE3[4];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t WKCTL;                 /*!< [0x0054] USCI Wake-up Control Register                                    */
    __IO uint32_t WKSTS;                 /*!< [0x0058] USCI Wake-up Status Register                                     */
    __IO uint32_t PROTCTL;               /*!< [0x005c] USCI Protocol Control Register                                   */
    __IO uint32_t PROTIEN;               /*!< [0x0060] USCI Protocol Interrupt Enable Register                          */
    __IO uint32_t PROTSTS;               /*!< [0x0064] USCI Protocol Status Register                                    */

} USPI_T;

/**
    @addtogroup USPI_CONST USPI Bit Field Definition
    Constant Definitions for USPI Controller
@{ */

#define USPI_CTL_FUNMODE_Pos             (0)                                               /*!< USPI_T::CTL: FUNMODE Position          */
#define USPI_CTL_FUNMODE_Msk             (0x7ul << USPI_CTL_FUNMODE_Pos)                   /*!< USPI_T::CTL: FUNMODE Mask              */

#define USPI_INTEN_TXSTIEN_Pos           (1)                                               /*!< USPI_T::INTEN: TXSTIEN Position        */
#define USPI_INTEN_TXSTIEN_Msk           (0x1ul << USPI_INTEN_TXSTIEN_Pos)                 /*!< USPI_T::INTEN: TXSTIEN Mask            */

#define USPI_INTEN_TXENDIEN_Pos          (2)                                               /*!< USPI_T::INTEN: TXENDIEN Position       */
#define USPI_INTEN_TXENDIEN_Msk          (0x1ul << USPI_INTEN_TXENDIEN_Pos)                /*!< USPI_T::INTEN: TXENDIEN Mask           */

#define USPI_INTEN_RXSTIEN_Pos           (3)                                               /*!< USPI_T::INTEN: RXSTIEN Position        */
#define USPI_INTEN_RXSTIEN_Msk           (0x1ul << USPI_INTEN_RXSTIEN_Pos)                 /*!< USPI_T::INTEN: RXSTIEN Mask            */

#define USPI_INTEN_RXENDIEN_Pos          (4)                                               /*!< USPI_T::INTEN: RXENDIEN Position       */
#define USPI_INTEN_RXENDIEN_Msk          (0x1ul << USPI_INTEN_RXENDIEN_Pos)                /*!< USPI_T::INTEN: RXENDIEN Mask           */

#define USPI_BRGEN_RCLKSEL_Pos           (0)                                               /*!< USPI_T::BRGEN: RCLKSEL Position        */
#define USPI_BRGEN_RCLKSEL_Msk           (0x1ul << USPI_BRGEN_RCLKSEL_Pos)                 /*!< USPI_T::BRGEN: RCLKSEL Mask            */

#define USPI_BRGEN_PTCLKSEL_Pos          (1)                                               /*!< USPI_T::BRGEN: PTCLKSEL Position       */
#define USPI_BRGEN_PTCLKSEL_Msk          (0x1ul << USPI_BRGEN_PTCLKSEL_Pos)                /*!< USPI_T::BRGEN: PTCLKSEL Mask           */

#define USPI_BRGEN_SPCLKSEL_Pos          (2)                                               /*!< USPI_T::BRGEN: SPCLKSEL Position       */
#define USPI_BRGEN_SPCLKSEL_Msk          (0x3ul << USPI_BRGEN_SPCLKSEL_Pos)                /*!< USPI_T::BRGEN: SPCLKSEL Mask           */

#define USPI_BRGEN_TMCNTEN_Pos           (4)                                               /*!< USPI_T::BRGEN: TMCNTEN Position        */
#define USPI_BRGEN_TMCNTEN_Msk           (0x1ul << USPI_BRGEN_TMCNTEN_Pos)                 /*!< USPI_T::BRGEN: TMCNTEN Mask            */

#define USPI_BRGEN_TMCNTSRC_Pos          (5)                                               /*!< USPI_T::BRGEN: TMCNTSRC Position       */
#define USPI_BRGEN_TMCNTSRC_Msk          (0x1ul << USPI_BRGEN_TMCNTSRC_Pos)                /*!< USPI_T::BRGEN: TMCNTSRC Mask           */

#define USPI_BRGEN_CLKDIV_Pos            (16)                                              /*!< USPI_T::BRGEN: CLKDIV Position         */
#define USPI_BRGEN_CLKDIV_Msk            (0x3fful << USPI_BRGEN_CLKDIV_Pos)                /*!< USPI_T::BRGEN: CLKDIV Mask             */

#define USPI_DATIN0_SYNCSEL_Pos          (0)                                               /*!< USPI_T::DATIN0: SYNCSEL Position       */
#define USPI_DATIN0_SYNCSEL_Msk          (0x1ul << USPI_DATIN0_SYNCSEL_Pos)                /*!< USPI_T::DATIN0: SYNCSEL Mask           */

#define USPI_DATIN0_ININV_Pos            (2)                                               /*!< USPI_T::DATIN0: ININV Position         */
#define USPI_DATIN0_ININV_Msk            (0x1ul << USPI_DATIN0_ININV_Pos)                  /*!< USPI_T::DATIN0: ININV Mask             */

#define USPI_CTLIN0_SYNCSEL_Pos          (0)                                               /*!< USPI_T::CTLIN0: SYNCSEL Position       */
#define USPI_CTLIN0_SYNCSEL_Msk          (0x1ul << USPI_CTLIN0_SYNCSEL_Pos)                /*!< USPI_T::CTLIN0: SYNCSEL Mask           */

#define USPI_CTLIN0_ININV_Pos            (2)                                               /*!< USPI_T::CTLIN0: ININV Position         */
#define USPI_CTLIN0_ININV_Msk            (0x1ul << USPI_CTLIN0_ININV_Pos)                  /*!< USPI_T::CTLIN0: ININV Mask             */

#define USPI_CLKIN_SYNCSEL_Pos           (0)                                               /*!< USPI_T::CLKIN: SYNCSEL Position        */
#define USPI_CLKIN_SYNCSEL_Msk           (0x1ul << USPI_CLKIN_SYNCSEL_Pos)                 /*!< USPI_T::CLKIN: SYNCSEL Mask            */

#define USPI_LINECTL_LSB_Pos             (0)                                               /*!< USPI_T::LINECTL: LSB Position          */
#define USPI_LINECTL_LSB_Msk             (0x1ul << USPI_LINECTL_LSB_Pos)                   /*!< USPI_T::LINECTL: LSB Mask              */

#define USPI_LINECTL_DATOINV_Pos         (5)                                               /*!< USPI_T::LINECTL: DATOINV Position      */
#define USPI_LINECTL_DATOINV_Msk         (0x1ul << USPI_LINECTL_DATOINV_Pos)               /*!< USPI_T::LINECTL: DATOINV Mask          */

#define USPI_LINECTL_CTLOINV_Pos         (7)                                               /*!< USPI_T::LINECTL: CTLOINV Position      */
#define USPI_LINECTL_CTLOINV_Msk         (0x1ul << USPI_LINECTL_CTLOINV_Pos)               /*!< USPI_T::LINECTL: CTLOINV Mask          */

#define USPI_LINECTL_DWIDTH_Pos          (8)                                               /*!< USPI_T::LINECTL: DWIDTH Position       */
#define USPI_LINECTL_DWIDTH_Msk          (0xful << USPI_LINECTL_DWIDTH_Pos)                /*!< USPI_T::LINECTL: DWIDTH Mask           */

#define USPI_TXDAT_TXDAT_Pos             (0)                                               /*!< USPI_T::TXDAT: TXDAT Position          */
#define USPI_TXDAT_TXDAT_Msk             (0xfffful << USPI_TXDAT_TXDAT_Pos)                /*!< USPI_T::TXDAT: TXDAT Mask              */

#define USPI_TXDAT_PORTDIR_Pos           (16)                                              /*!< USPI_T::TXDAT: PORTDIR Position        */
#define USPI_TXDAT_PORTDIR_Msk           (0x1ul << USPI_TXDAT_PORTDIR_Pos)                 /*!< USPI_T::TXDAT: PORTDIR Mask            */

#define USPI_RXDAT_RXDAT_Pos             (0)                                               /*!< USPI_T::RXDAT: RXDAT Position          */
#define USPI_RXDAT_RXDAT_Msk             (0xfffful << USPI_RXDAT_RXDAT_Pos)                /*!< USPI_T::RXDAT: RXDAT Mask              */

#define USPI_BUFCTL_TXUDRIEN_Pos         (6)                                               /*!< USPI_T::BUFCTL: TXUDRIEN Position      */
#define USPI_BUFCTL_TXUDRIEN_Msk         (0x1ul << USPI_BUFCTL_TXUDRIEN_Pos)               /*!< USPI_T::BUFCTL: TXUDRIEN Mask          */

#define USPI_BUFCTL_TXCLR_Pos            (7)                                               /*!< USPI_T::BUFCTL: TXCLR Position         */
#define USPI_BUFCTL_TXCLR_Msk            (0x1ul << USPI_BUFCTL_TXCLR_Pos)                  /*!< USPI_T::BUFCTL: TXCLR Mask             */

#define USPI_BUFCTL_RXOVIEN_Pos          (14)                                              /*!< USPI_T::BUFCTL: RXOVIEN Position       */
#define USPI_BUFCTL_RXOVIEN_Msk          (0x1ul << USPI_BUFCTL_RXOVIEN_Pos)                /*!< USPI_T::BUFCTL: RXOVIEN Mask           */

#define USPI_BUFCTL_RXCLR_Pos            (15)                                              /*!< USPI_T::BUFCTL: RXCLR Position         */
#define USPI_BUFCTL_RXCLR_Msk            (0x1ul << USPI_BUFCTL_RXCLR_Pos)                  /*!< USPI_T::BUFCTL: RXCLR Mask             */

#define USPI_BUFCTL_TXRST_Pos            (16)                                              /*!< USPI_T::BUFCTL: TXRST Position         */
#define USPI_BUFCTL_TXRST_Msk            (0x1ul << USPI_BUFCTL_TXRST_Pos)                  /*!< USPI_T::BUFCTL: TXRST Mask             */

#define USPI_BUFCTL_RXRST_Pos            (17)                                              /*!< USPI_T::BUFCTL: RXRST Position         */
#define USPI_BUFCTL_RXRST_Msk            (0x1ul << USPI_BUFCTL_RXRST_Pos)                  /*!< USPI_T::BUFCTL: RXRST Mask             */

#define USPI_BUFSTS_RXEMPTY_Pos          (0)                                               /*!< USPI_T::BUFSTS: RXEMPTY Position       */
#define USPI_BUFSTS_RXEMPTY_Msk          (0x1ul << USPI_BUFSTS_RXEMPTY_Pos)                /*!< USPI_T::BUFSTS: RXEMPTY Mask           */

#define USPI_BUFSTS_RXFULL_Pos           (1)                                               /*!< USPI_T::BUFSTS: RXFULL Position        */
#define USPI_BUFSTS_RXFULL_Msk           (0x1ul << USPI_BUFSTS_RXFULL_Pos)                 /*!< USPI_T::BUFSTS: RXFULL Mask            */

#define USPI_BUFSTS_RXOVIF_Pos           (3)                                               /*!< USPI_T::BUFSTS: RXOVIF Position        */
#define USPI_BUFSTS_RXOVIF_Msk           (0x1ul << USPI_BUFSTS_RXOVIF_Pos)                 /*!< USPI_T::BUFSTS: RXOVIF Mask            */

#define USPI_BUFSTS_TXEMPTY_Pos          (8)                                               /*!< USPI_T::BUFSTS: TXEMPTY Position       */
#define USPI_BUFSTS_TXEMPTY_Msk          (0x1ul << USPI_BUFSTS_TXEMPTY_Pos)                /*!< USPI_T::BUFSTS: TXEMPTY Mask           */

#define USPI_BUFSTS_TXFULL_Pos           (9)                                               /*!< USPI_T::BUFSTS: TXFULL Position        */
#define USPI_BUFSTS_TXFULL_Msk           (0x1ul << USPI_BUFSTS_TXFULL_Pos)                 /*!< USPI_T::BUFSTS: TXFULL Mask            */

#define USPI_BUFSTS_TXUDRIF_Pos          (11)                                              /*!< USPI_T::BUFSTS: TXUDRIF Position       */
#define USPI_BUFSTS_TXUDRIF_Msk          (0x1ul << USPI_BUFSTS_TXUDRIF_Pos)                /*!< USPI_T::BUFSTS: TXUDRIF Mask           */

#define USPI_PDMACTL_PDMARST_Pos         (0)                                               /*!< USPI_T::PDMACTL: PDMARST Position      */
#define USPI_PDMACTL_PDMARST_Msk         (0x1ul << USPI_PDMACTL_PDMARST_Pos)               /*!< USPI_T::PDMACTL: PDMARST Mask          */

#define USPI_PDMACTL_TXPDMAEN_Pos        (1)                                               /*!< USPI_T::PDMACTL: TXPDMAEN Position     */
#define USPI_PDMACTL_TXPDMAEN_Msk        (0x1ul << USPI_PDMACTL_TXPDMAEN_Pos)              /*!< USPI_T::PDMACTL: TXPDMAEN Mask         */

#define USPI_PDMACTL_RXPDMAEN_Pos        (2)                                               /*!< USPI_T::PDMACTL: RXPDMAEN Position     */
#define USPI_PDMACTL_RXPDMAEN_Msk        (0x1ul << USPI_PDMACTL_RXPDMAEN_Pos)              /*!< USPI_T::PDMACTL: RXPDMAEN Mask         */

#define USPI_PDMACTL_PDMAEN_Pos          (3)                                               /*!< USPI_T::PDMACTL: PDMAEN Position       */
#define USPI_PDMACTL_PDMAEN_Msk          (0x1ul << USPI_PDMACTL_PDMAEN_Pos)                /*!< USPI_T::PDMACTL: PDMAEN Mask           */

#define USPI_WKCTL_WKEN_Pos              (0)                                               /*!< USPI_T::WKCTL: WKEN Position           */
#define USPI_WKCTL_WKEN_Msk              (0x1ul << USPI_WKCTL_WKEN_Pos)                    /*!< USPI_T::WKCTL: WKEN Mask               */

#define USPI_WKCTL_WKADDREN_Pos          (1)                                               /*!< USPI_T::WKCTL: WKADDREN Position       */
#define USPI_WKCTL_WKADDREN_Msk          (0x1ul << USPI_WKCTL_WKADDREN_Pos)                /*!< USPI_T::WKCTL: WKADDREN Mask           */

#define USPI_WKCTL_PDBOPT_Pos            (2)                                               /*!< USPI_T::WKCTL: PDBOPT Position         */
#define USPI_WKCTL_PDBOPT_Msk            (0x1ul << USPI_WKCTL_PDBOPT_Pos)                  /*!< USPI_T::WKCTL: PDBOPT Mask             */

#define USPI_WKSTS_WKF_Pos               (0)                                               /*!< USPI_T::WKSTS: WKF Position            */
#define USPI_WKSTS_WKF_Msk               (0x1ul << USPI_WKSTS_WKF_Pos)                     /*!< USPI_T::WKSTS: WKF Mask                */

#define USPI_PROTCTL_SLAVE_Pos           (0)                                               /*!< USPI_T::PROTCTL: SLAVE Position        */
#define USPI_PROTCTL_SLAVE_Msk           (0x1ul << USPI_PROTCTL_SLAVE_Pos)                 /*!< USPI_T::PROTCTL: SLAVE Mask            */

#define USPI_PROTCTL_SLV3WIRE_Pos        (1)                                               /*!< USPI_T::PROTCTL: SLV3WIRE Position     */
#define USPI_PROTCTL_SLV3WIRE_Msk        (0x1ul << USPI_PROTCTL_SLV3WIRE_Pos)              /*!< USPI_T::PROTCTL: SLV3WIRE Mask         */

#define USPI_PROTCTL_SS_Pos              (2)                                               /*!< USPI_T::PROTCTL: SS Position           */
#define USPI_PROTCTL_SS_Msk              (0x1ul << USPI_PROTCTL_SS_Pos)                    /*!< USPI_T::PROTCTL: SS Mask               */

#define USPI_PROTCTL_AUTOSS_Pos          (3)                                               /*!< USPI_T::PROTCTL: AUTOSS Position       */
#define USPI_PROTCTL_AUTOSS_Msk          (0x1ul << USPI_PROTCTL_AUTOSS_Pos)                /*!< USPI_T::PROTCTL: AUTOSS Mask           */

#define USPI_PROTCTL_SCLKMODE_Pos        (6)                                               /*!< USPI_T::PROTCTL: SCLKMODE Position     */
#define USPI_PROTCTL_SCLKMODE_Msk        (0x3ul << USPI_PROTCTL_SCLKMODE_Pos)              /*!< USPI_T::PROTCTL: SCLKMODE Mask         */

#define USPI_PROTCTL_SUSPITV_Pos         (8)                                               /*!< USPI_T::PROTCTL: SUSPITV Position      */
#define USPI_PROTCTL_SUSPITV_Msk         (0xful << USPI_PROTCTL_SUSPITV_Pos)               /*!< USPI_T::PROTCTL: SUSPITV Mask          */

#define USPI_PROTCTL_TSMSEL_Pos          (12)                                              /*!< USPI_T::PROTCTL: TSMSEL Position       */
#define USPI_PROTCTL_TSMSEL_Msk          (0x7ul << USPI_PROTCTL_TSMSEL_Pos)                /*!< USPI_T::PROTCTL: TSMSEL Mask           */

#define USPI_PROTCTL_SLVTOCNT_Pos        (16)                                              /*!< USPI_T::PROTCTL: SLVTOCNT Position     */
#define USPI_PROTCTL_SLVTOCNT_Msk        (0x3fful << USPI_PROTCTL_SLVTOCNT_Pos)            /*!< USPI_T::PROTCTL: SLVTOCNT Mask         */

#define USPI_PROTCTL_TXUDRPOL_Pos        (28)                                              /*!< USPI_T::PROTCTL: TXUDRPOL Position     */
#define USPI_PROTCTL_TXUDRPOL_Msk        (0x1ul << USPI_PROTCTL_TXUDRPOL_Pos)              /*!< USPI_T::PROTCTL: TXUDRPOL Mask         */

#define USPI_PROTCTL_PROTEN_Pos          (31)                                              /*!< USPI_T::PROTCTL: PROTEN Position       */
#define USPI_PROTCTL_PROTEN_Msk          (0x1ul << USPI_PROTCTL_PROTEN_Pos)                /*!< USPI_T::PROTCTL: PROTEN Mask           */

#define USPI_PROTIEN_SSINAIEN_Pos        (0)                                               /*!< USPI_T::PROTIEN: SSINAIEN Position     */
#define USPI_PROTIEN_SSINAIEN_Msk        (0x1ul << USPI_PROTIEN_SSINAIEN_Pos)              /*!< USPI_T::PROTIEN: SSINAIEN Mask         */

#define USPI_PROTIEN_SSACTIEN_Pos        (1)                                               /*!< USPI_T::PROTIEN: SSACTIEN Position     */
#define USPI_PROTIEN_SSACTIEN_Msk        (0x1ul << USPI_PROTIEN_SSACTIEN_Pos)              /*!< USPI_T::PROTIEN: SSACTIEN Mask         */

#define USPI_PROTIEN_SLVTOIEN_Pos        (2)                                               /*!< USPI_T::PROTIEN: SLVTOIEN Position     */
#define USPI_PROTIEN_SLVTOIEN_Msk        (0x1ul << USPI_PROTIEN_SLVTOIEN_Pos)              /*!< USPI_T::PROTIEN: SLVTOIEN Mask         */

#define USPI_PROTIEN_SLVBEIEN_Pos        (3)                                               /*!< USPI_T::PROTIEN: SLVBEIEN Position     */
#define USPI_PROTIEN_SLVBEIEN_Msk        (0x1ul << USPI_PROTIEN_SLVBEIEN_Pos)              /*!< USPI_T::PROTIEN: SLVBEIEN Mask         */

#define USPI_PROTSTS_TXSTIF_Pos          (1)                                               /*!< USPI_T::PROTSTS: TXSTIF Position       */
#define USPI_PROTSTS_TXSTIF_Msk          (0x1ul << USPI_PROTSTS_TXSTIF_Pos)                /*!< USPI_T::PROTSTS: TXSTIF Mask           */

#define USPI_PROTSTS_TXENDIF_Pos         (2)                                               /*!< USPI_T::PROTSTS: TXENDIF Position      */
#define USPI_PROTSTS_TXENDIF_Msk         (0x1ul << USPI_PROTSTS_TXENDIF_Pos)               /*!< USPI_T::PROTSTS: TXENDIF Mask          */

#define USPI_PROTSTS_RXSTIF_Pos          (3)                                               /*!< USPI_T::PROTSTS: RXSTIF Position       */
#define USPI_PROTSTS_RXSTIF_Msk          (0x1ul << USPI_PROTSTS_RXSTIF_Pos)                /*!< USPI_T::PROTSTS: RXSTIF Mask           */

#define USPI_PROTSTS_RXENDIF_Pos         (4)                                               /*!< USPI_T::PROTSTS: RXENDIF Position      */
#define USPI_PROTSTS_RXENDIF_Msk         (0x1ul << USPI_PROTSTS_RXENDIF_Pos)               /*!< USPI_T::PROTSTS: RXENDIF Mask          */

#define USPI_PROTSTS_SLVTOIF_Pos         (5)                                               /*!< USPI_T::PROTSTS: SLVTOIF Position      */
#define USPI_PROTSTS_SLVTOIF_Msk         (0x1ul << USPI_PROTSTS_SLVTOIF_Pos)               /*!< USPI_T::PROTSTS: SLVTOIF Mask          */

#define USPI_PROTSTS_SLVBEIF_Pos         (6)                                               /*!< USPI_T::PROTSTS: SLVBEIF Position      */
#define USPI_PROTSTS_SLVBEIF_Msk         (0x1ul << USPI_PROTSTS_SLVBEIF_Pos)               /*!< USPI_T::PROTSTS: SLVBEIF Mask          */

#define USPI_PROTSTS_SSINAIF_Pos         (8)                                               /*!< USPI_T::PROTSTS: SSINAIF Position      */
#define USPI_PROTSTS_SSINAIF_Msk         (0x1ul << USPI_PROTSTS_SSINAIF_Pos)               /*!< USPI_T::PROTSTS: SSINAIF Mask          */

#define USPI_PROTSTS_SSACTIF_Pos         (9)                                               /*!< USPI_T::PROTSTS: SSACTIF Position      */
#define USPI_PROTSTS_SSACTIF_Msk         (0x1ul << USPI_PROTSTS_SSACTIF_Pos)               /*!< USPI_T::PROTSTS: SSACTIF Mask          */

#define USPI_PROTSTS_SSLINE_Pos          (16)                                              /*!< USPI_T::PROTSTS: SSLINE Position       */
#define USPI_PROTSTS_SSLINE_Msk          (0x1ul << USPI_PROTSTS_SSLINE_Pos)                /*!< USPI_T::PROTSTS: SSLINE Mask           */

#define USPI_PROTSTS_BUSY_Pos            (17)                                              /*!< USPI_T::PROTSTS: BUSY Position         */
#define USPI_PROTSTS_BUSY_Msk            (0x1ul << USPI_PROTSTS_BUSY_Pos)                  /*!< USPI_T::PROTSTS: BUSY Mask             */

#define USPI_PROTSTS_SLVUDR_Pos          (18)                                              /*!< USPI_T::PROTSTS: SLVUDR Position       */
#define USPI_PROTSTS_SLVUDR_Msk          (0x1ul << USPI_PROTSTS_SLVUDR_Pos)                /*!< USPI_T::PROTSTS: SLVUDR Mask           */

/**@}*/ /* USPI_CONST */
/**@}*/ /* end of USPI register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __USPI_REG_H__ */
