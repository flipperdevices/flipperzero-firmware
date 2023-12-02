/**************************************************************************//**
 * @file     spi_reg.h
 * @version  V1.00
 * @brief    SPI register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SPI_REG_H__
#define __SPI_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup SPI Serial Peripheral Interface Controller(SPI)
    Memory Mapped Structure for SPI Controller
@{ */

typedef struct
{


    /**
     * @var SPI_T::CTL
     * Offset: 0x00  SPI Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SPIEN     |SPI Transfer Control Enable Bit
     * |        |          |In Master mode, the transfer will start when there is data in the FIFO buffer after this bit is set to 1
     * |        |          |In Slave mode, this device is ready to receive data when this bit is set to 1.
     * |        |          |0 = Transfer control Disabled.
     * |        |          |1 = Transfer control Enabled.
     * |        |          |Note: Before changing the configurations of SPIx_CTL, SPIx_CLKDIV, SPIx_SSCTL and SPIx_FIFOCTL registers, user shall clear the SPIEN (SPIx_CTL[0]) and confirm the SPIENSTS (SPIx_STATUS[15]) is 0.
     * |[1]     |RXNEG     |Receive on Negative Edge
     * |        |          |0 = Received data input signal is latched on the rising edge of SPI bus clock.
     * |        |          |1 = Received data input signal is latched on the falling edge of SPI bus clock.
     * |[2]     |TXNEG     |Transmit on Negative Edge
     * |        |          |0 = Transmitted data output signal is changed on the rising edge of SPI bus clock.
     * |        |          |1 = Transmitted data output signal is changed on the falling edge of SPI bus clock.
     * |[3]     |CLKPOL    |Clock Polarity
     * |        |          |0 = SPI bus clock is idle low.
     * |        |          |1 = SPI bus clock is idle high.
     * |[7:4]   |SUSPITV   |Suspend Interval (Master Only)
     * |        |          |The four bits provide configurable suspend interval between two successive transmit/receive transaction in a transfer
     * |        |          |The definition of the suspend interval is the interval between the last clock edge of the preceding transaction word and the first clock edge of the following transaction word
     * |        |          |The default value is 0x3
     * |        |          |The period of the suspend interval is obtained according to the following equation.
     * |        |          |(SUSPITV[3:0] + 0.5) * period of SPICLK clock cycle
     * |        |          |Example:
     * |        |          |SUSPITV = 0x0 .... 0.5 SPICLK clock cycle.
     * |        |          |SUSPITV = 0x1 .... 1.5 SPICLK clock cycle.
     * |        |          |.....
     * |        |          |SUSPITV = 0xE .... 14.5 SPICLK clock cycle.
     * |        |          |SUSPITV = 0xF .... 15.5 SPICLK clock cycle.
     * |[12:8]  |DWIDTH    |Data Width
     * |        |          |This field specifies how many bits can be transmitted / received in one transaction
     * |        |          |The minimum bit length is 8 bits and can up to 32 bits.
     * |        |          |DWIDTH = 0x08 .... 8 bits.
     * |        |          |DWIDTH = 0x09 .... 9 bits.
     * |        |          |.....
     * |        |          |DWIDTH = 0x1F .... 31 bits.
     * |        |          |DWIDTH = 0x00 .... 32 bits.
     * |        |          |Note: For SPI1~SPI4, this bit field will decide the depth of TX/RX FIFO configuration in SPI mode
     * |        |          |Therefore, changing this bit field will clear TX/RX FIFO by hardware automatically in SPI1~SPI4.
     * |[13]    |LSB       |Send LSB First
     * |        |          |0 = The MSB, which bit of transmit/receive register depends on the setting of DWIDTH, is transmitted/received first.
     * |        |          |1 = The LSB, bit 0 of the SPI TX register, is sent first to the SPI data output pin, and the first bit received from the SPI data input pin will be put in the LSB position of the RX register (bit 0 of SPI_RX).
     * |[14]    |HALFDPX   |SPI Half-duplex Transfer Enable Bit
     * |        |          |This bit is used to select full-duplex or half-duplex for SPI transfer
     * |        |          |The bit field DATDIR (SPIx_CTL[20]) can be used to set the data direction in half-duplex transfer.
     * |        |          |0 = SPI operates in full-duplex transfer.
     * |        |          |1 = SPI operates in half-duplex transfer.
     * |[15]    |RXONLY    |Receive-only Mode Enable Bit (Master Only)
     * |        |          |This bit field is only available in Master mode
     * |        |          |In receive-only mode, SPI Master will generate SPI bus clock continuously for receiving data bit from SPI slave device and assert the BUSY status.
     * |        |          |0 = Receive-only mode Disabled.
     * |        |          |1 = Receive-only mode Enabled.
     * |[17]    |UNITIEN   |Unit Transfer Interrupt Enable Bit
     * |        |          |0 = SPI unit transfer interrupt Disabled.
     * |        |          |1 = SPI unit transfer interrupt Enabled.
     * |[18]    |SLAVE     |Slave Mode Control
     * |        |          |0 = Master mode.
     * |        |          |1 = Slave mode.
     * |[19]    |REORDER   |Byte Reorder Function Enable Bit
     * |        |          |0 = Byte Reorder function Disabled.
     * |        |          |1 = Byte Reorder function Enabled
     * |        |          |A byte suspend interval will be inserted among each byte
     * |        |          |The period of the byte suspend interval depends on the setting of SUSPITV.
     * |        |          |Note: Byte Reorder function is only available if DWIDTH is defined as 16, 24, and 32 bits.
     * |[20]    |DATDIR    |Data Port Direction Control
     * |        |          |This bit is used to select the data input/output direction in half-duplex transfer and Dual/Quad transfer
     * |        |          |0 = SPI data is input direction.
     * |        |          |1 = SPI data is output direction.
     * @var SPI_T::CLKDIV
     * Offset: 0x04  SPI Clock Divider Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |DIVIDER   |Clock Divider
     * |        |          |The value in this field is the frequency divider for generating the peripheral clock, fspi_eclk, and the SPI bus clock of SPI Master
     * |        |          |The frequency is obtained according to the following equation.
     * |        |          |where
     * |        |          |is the peripheral clock source, which is defined in the clock control register, CLK_CLKSEL2.
     * |        |          |Note: Not supported in I2S mode.
     * @var SPI_T::SSCTL
     * Offset: 0x08  SPI Slave Select Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SS        |Slave Selection Control (Master Only)
     * |        |          |If AUTOSS bit is cleared to 0,
     * |        |          |0 = set the SPIx_SS line to inactive state.
     * |        |          |1 = set the SPIx_SS line to active state.
     * |        |          |If the AUTOSS bit is set to 1,
     * |        |          |0 = Keep the SPIx_SS line at inactive state.
     * |        |          |1 = SPIx_SS line will be automatically driven to active state for the duration of data transfer, and will be driven to inactive state for the rest of the time
     * |        |          |The active state of SPIx_SS is specified in SSACTPOL (SPIx_SSCTL[2]).
     * |[2]     |SSACTPOL  |Slave Selection Active Polarity
     * |        |          |This bit defines the active polarity of slave selection signal (SPIx_SS).
     * |        |          |0 = The slave selection signal SPIx_SS is active low.
     * |        |          |1 = The slave selection signal SPIx_SS is active high.
     * |[3]     |AUTOSS    |Automatic Slave Selection Function Enable Bit (Master Only)
     * |        |          |0 = Automatic slave selection function Disabled
     * |        |          |Slave selection signal will be asserted/de-asserted according to SS (SPIx_SSCTL[0]).
     * |        |          |1 = Automatic slave selection function Enabled.
     * |[8]     |SLVBEIEN  |Slave Mode Bit Count Error Interrupt Enable Bit
     * |        |          |0 = Slave mode bit count error interrupt Disabled.
     * |        |          |1 = Slave mode bit count error interrupt Enabled.
     * |[9]     |SLVURIEN  |Slave Mode TX Under Run Interrupt Enable Bit
     * |        |          |0 = Slave mode TX under run interrupt Disabled.
     * |        |          |1 = Slave mode TX under run interrupt Enabled.
     * |[12]    |SSACTIEN  |Slave Select Active Interrupt Enable Bit
     * |        |          |0 = Slave select active interrupt Disabled.
     * |        |          |1 = Slave select active interrupt Enabled.
     * |[13]    |SSINAIEN  |Slave Select Inactive Interrupt Enable Bit
     * |        |          |0 = Slave select inactive interrupt Disabled.
     * |        |          |1 = Slave select inactive interrupt Enabled.
     * @var SPI_T::PDMACTL
     * Offset: 0x0C  SPI PDMA Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TXPDMAEN  |Transmit PDMA Enable Bit
     * |        |          |0 = Transmit PDMA function Disabled.
     * |        |          |1 = Transmit PDMA function Enabled.
     * |        |          |Note: In SPI Master mode with full duplex transfer, if both TX and RX PDMA functions are enabled, RX PDMA function cannot be enabled prior to TX PDMA function
     * |        |          |User can enable TX PDMA function firstly or enable both functions simultaneously.
     * |[1]     |RXPDMAEN  |Receive PDMA Enable Bit
     * |        |          |0 = Receive PDMA function Disabled.
     * |        |          |1 = Receive PDMA function Enabled.
     * |[2]     |PDMARST   |PDMA Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the PDMA control logic of the SPI controller. This bit will be automatically cleared to 0.
     * @var SPI_T::FIFOCTL
     * Offset: 0x10  SPI FIFO Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RXRST     |Receive Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset receive FIFO pointer and receive circuit
     * |        |          |The RXFULL bit will be cleared to 0 and the RXEMPTY bit will be set to 1
     * |        |          |This bit will be cleared to 0 by hardware about 3 system clock cycles + 2 peripheral clock cycles after it is set to 1
     * |        |          |User can read TXRXRST (SPIx_STATUS[23]) to check if reset is accomplished or not.
     * |[1]     |TXRST     |Transmit Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset transmit FIFO pointer and transmit circuit
     * |        |          |The TXFULL bit will be cleared to 0 and the TXEMPTY bit will be set to 1
     * |        |          |This bit will be cleared to 0 by hardware about 3 system clock cycles + 2 peripheral clock cycles after it is set to 1
     * |        |          |User can read TXRXRST (SPIx_STATUS[23]) to check if reset is accomplished or not.
     * |        |          |Note: If TX underflow event occurs in SPI Slave mode, this bit can be used to make SPI return to idle state.
     * |[2]     |RXTHIEN   |Receive FIFO Threshold Interrupt Enable Bit
     * |        |          |0 = RX FIFO threshold interrupt Disabled.
     * |        |          |1 = RX FIFO threshold interrupt Enabled.
     * |[3]     |TXTHIEN   |Transmit FIFO Threshold Interrupt Enable Bit
     * |        |          |0 = TX FIFO threshold interrupt Disabled.
     * |        |          |1 = TX FIFO threshold interrupt Enabled.
     * |[4]     |RXTOIEN   |Slave Receive Time-out Interrupt Enable Bit
     * |        |          |0 = Receive time-out interrupt Disabled.
     * |        |          |1 = Receive time-out interrupt Enabled.
     * |[5]     |RXOVIEN   |Receive FIFO Overrun Interrupt Enable Bit
     * |        |          |0 = Receive FIFO overrun interrupt Disabled.
     * |        |          |1 = Receive FIFO overrun interrupt Enabled.
     * |[6]     |TXUFPOL   |TX Underflow Data Polarity
     * |        |          |0 = The SPI data out is keep 0 if there is TX underflow event in Slave mode.
     * |        |          |1 = The SPI data out is keep 1 if there is TX underflow event in Slave mode.
     * |        |          |Note:
     * |        |          |1. The TX underflow event occurs if there is no any data in TX FIFO when the slave selection signal is active.
     * |        |          |2. This bit should be set as 0 in I2S mode.
     * |        |          |3. When TX underflow event occurs, SPIx_MISO pin state will be determined by this setting even though TX FIFO is not empty afterward
     * |        |          |Data stored in TX FIFO will be sent through SPIx_MISO pin in the next transfer frame.
     * |[7]     |TXUFIEN   |TX Underflow Interrupt Enable Bit
     * |        |          |When TX underflow event occurs in Slave mode, TXUFIF (SPIx_STATUS[19]) will be set to 1
     * |        |          |This bit is used to enable the TX underflow interrupt.
     * |        |          |0 = Slave TX underflow interrupt Disabled.
     * |        |          |1 = Slave TX underflow interrupt Enabled.
     * |[8]     |RXFBCLR   |Receive FIFO Buffer Clear
     * |        |          |0 = No effect.
     * |        |          |1 = Clear receive FIFO pointer
     * |        |          |The RXFULL bit will be cleared to 0 and the RXEMPTY bit will be set to 1
     * |        |          |This bit will be cleared to 0 by hardware about 1 system clock after it is set to 1.
     * |        |          |Note: The RX shift register will not be cleared.
     * |[9]     |TXFBCLR   |Transmit FIFO Buffer Clear
     * |        |          |0 = No effect.
     * |        |          |1 = Clear transmit FIFO pointer
     * |        |          |The TXFULL bit will be cleared to 0 and the TXEMPTY bit will be set to 1
     * |        |          |This bit will be cleared to 0 by hardware about 1 system clock after it is set to 1.
     * |        |          |Note: The TX shift register will not be cleared.
     * |[26:24] |RXTH      |Receive FIFO Threshold
     * |        |          |If the valid data count of the receive FIFO buffer is larger than the RXTH setting, the RXTHIF bit will be set to 1, else the RXTHIF bit will be cleared to 0
     * |        |          |For SPI1~SPI4, the MSB of this bit field is only meaningful while SPI mode 8~16 bits of data length.
     * |[30:28] |TXTH      |Transmit FIFO Threshold
     * |        |          |If the valid data count of the transmit FIFO buffer is less than or equal to the TXTH setting, the TXTHIF bit will be set to 1, else the TXTHIF bit will be cleared to 0
     * |        |          |For SPI1~SPI4, the MSB of this bit field is only meaningful while SPI mode 8~16 bits of data length
     * @var SPI_T::STATUS
     * Offset: 0x14  SPI Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSY      |Busy Status (Read Only)
     * |        |          |0 = SPI controller is in idle state.
     * |        |          |1 = SPI controller is in busy state.
     * |        |          |The following listing are the bus busy conditions:
     * |        |          |a. SPIx_CTL[0] = 1 and TXEMPTY = 0.
     * |        |          |b
     * |        |          |For SPI Master mode, SPIx_CTL[0] = 1 and TXEMPTY = 1 but the current transaction is not finished yet.
     * |        |          |c. For SPI Master mode, SPIx_CTL[0] = 1 and RXONLY = 1.
     * |        |          |d
     * |        |          |For SPI Slave mode, the SPIx_CTL[0] = 1 and there is serial clock input into the SPI core logic when slave select is active.
     * |        |          |For SPI Slave mode, the SPIx_CTL[0] = 1 and the transmit buffer or transmit shift register is not empty even if the slave select is inactive.
     * |[1]     |UNITIF    |Unit Transfer Interrupt Flag
     * |        |          |0 = No transaction has been finished since this bit was cleared to 0.
     * |        |          |1 = SPI controller has finished one unit transfer.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[2]     |SSACTIF   |Slave Select Active Interrupt Flag
     * |        |          |0 = Slave select active interrupt was cleared or not occurred.
     * |        |          |1 = Slave select active interrupt event occurred.
     * |        |          |Note: Only available in Slave mode. This bit will be cleared by writing 1 to it.
     * |[3]     |SSINAIF   |Slave Select Inactive Interrupt Flag
     * |        |          |0 = Slave select inactive interrupt was cleared or not occurred.
     * |        |          |1 = Slave select inactive interrupt event occurred.
     * |        |          |Note: Only available in Slave mode. This bit will be cleared by writing 1 to it.
     * |[4]     |SSLINE    |Slave Select Line Bus Status (Read Only)
     * |        |          |0 = The slave select line status is 0.
     * |        |          |1 = The slave select line status is 1.
     * |        |          |Note: This bit is only available in Slave mode
     * |        |          |If SSACTPOL (SPIx_SSCTL[2]) is set 0, and the SSLINE is 1, the SPI slave select is in inactive status.
     * |[6]     |SLVBEIF   |Slave Mode Bit Count Error Interrupt Flag
     * |        |          |In Slave mode, when the slave select line goes to inactive state, if bit counter is mismatch with DWIDTH, this interrupt flag will be set to 1.
     * |        |          |0 = No Slave mode bit count error event.
     * |        |          |1 = Slave mode bit count error event occurs.
     * |        |          |Note: If the slave select active but there is no any bus clock input, the SLVBEIF also active when the slave select goes to inactive state
     * |        |          |This bit will be cleared by writing 1 to it.
     * |[7]     |SLVURIF   |Slave Mode TX Under Run Interrupt Flag
     * |        |          |In Slave mode, if TX underflow event occurs and the slave select line goes to inactive state, this interrupt flag will be set to 1.
     * |        |          |0 = No Slave TX under run event.
     * |        |          |1 = Slave TX under run event occurs.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[8]     |RXEMPTY   |Receive FIFO Buffer Empty Indicator (Read Only)
     * |        |          |0 = Receive FIFO buffer is not empty.
     * |        |          |1 = Receive FIFO buffer is empty.
     * |[9]     |RXFULL    |Receive FIFO Buffer Full Indicator (Read Only)
     * |        |          |0 = Receive FIFO buffer is not full.
     * |        |          |1 = Receive FIFO buffer is full.
     * |[10]    |RXTHIF    |Receive FIFO Threshold Interrupt Flag (Read Only)
     * |        |          |0 = The valid data count within the receive FIFO buffer is smaller than or equal to the setting value of RXTH.
     * |        |          |1 = The valid data count within the receive FIFO buffer is larger than the setting value of RXTH.
     * |[11]    |RXOVIF    |Receive FIFO Overrun Interrupt Flag
     * |        |          |When the receive FIFO buffer is full, the follow-up data will be dropped and this bit will be set to 1.
     * |        |          |0 = No FIFO is overrun.
     * |        |          |1 = Receive FIFO is overrun.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[12]    |RXTOIF    |Receive Time-out Interrupt Flag
     * |        |          |0 = No receive FIFO time-out event.
     * |        |          |1 = Receive FIFO buffer is not empty and no read operation on receive FIFO buffer over 64 SPI peripheral clock periods in Master mode or over 576 SPI peripheral clock periods in Slave mode
     * |        |          |When the received FIFO buffer is read by software, the time-out status will be cleared automatically.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[15]    |SPIENSTS  |SPI Enable Status (Read Only)
     * |        |          |0 = The SPI controller is disabled.
     * |        |          |1 = The SPI controller is enabled.
     * |        |          |Note: The SPI peripheral clock is asynchronous with the system clock
     * |        |          |In order to make sure the SPI control logic is disabled, this bit indicates the real status of SPI controller.
     * |[16]    |TXEMPTY   |Transmit FIFO Buffer Empty Indicator (Read Only)
     * |        |          |0 = Transmit FIFO buffer is not empty.
     * |        |          |1 = Transmit FIFO buffer is empty.
     * |[17]    |TXFULL    |Transmit FIFO Buffer Full Indicator (Read Only)
     * |        |          |0 = Transmit FIFO buffer is not full.
     * |        |          |1 = Transmit FIFO buffer is full.
     * |[18]    |TXTHIF    |Transmit FIFO Threshold Interrupt Flag (Read Only)
     * |        |          |0 = The valid data count within the transmit FIFO buffer is larger than the setting value of TXTH.
     * |        |          |1 = The valid data count within the transmit FIFO buffer is less than or equal to the setting value of TXTH.
     * |[19]    |TXUFIF    |TX Underflow Interrupt Flag
     * |        |          |When the TX underflow event occurs, this bit will be set to 1, the state of data output pin depends on the setting of TXUFPOL.
     * |        |          |0 = No effect.
     * |        |          |1 = No data in Transmit FIFO and TX shift register when the slave selection signal is active.
     * |        |          |Note 1: This bit will be cleared by writing 1 to it.
     * |        |          |Note 2: If reset slave's transmission circuit when slave selection signal is active, this flag will be set to 1 after 2 peripheral clock cycles + 3 system clock cycles since the reset operation is done.
     * |[23]    |TXRXRST   |TX or RX Reset Status (Read Only)
     * |        |          |0 = The reset function of TXRST or RXRST is done.
     * |        |          |1 = Doing the reset function of TXRST or RXRST.
     * |        |          |Note: Both the reset operations of TXRST and RXRST need 3 system clock cycles + 2 peripheral clock cycles
     * |        |          |User can check the status of this bit to monitor the reset function is doing or done.
     * |[27:24] |RXCNT     |Receive FIFO Data Count (Read Only)
     * |        |          |This bit field indicates the valid data count of receive FIFO buffer.
     * |[31:28] |TXCNT     |Transmit FIFO Data Count (Read Only)
     * |        |          |This bit field indicates the valid data count of transmit FIFO buffer.
     * @var SPI_T::TX
     * Offset: 0x20  SPI Data Transmit Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TX        |Data Transmit Register
     * |        |          |The data transmit registers pass through the transmitted data into the 4-level transmit FIFO buffers
     * |        |          |The number of valid bits depends on the setting of DWIDTH (SPIx_CTL[12:8]) in SPI mode or WDWIDTH (SPIx_I2SCTL[5:4]) in I2S mode.
     * |        |          |In SPI mode, if DWIDTH is set to 0x08, the bits TX[7:0] will be transmitted
     * |        |          |If DWIDTH is set to 0x00 , the SPI controller will perform a 32-bit transfer.
     * |        |          |In I2S mode, if WDWIDTH (SPIx_I2SCTL[5:4]) is set to 0x2, the data width of audio channel is 24-bit and corresponding to TX[23:0]
     * |        |          |If WDWIDTH is set as 0x0, 0x1, or 0x3, all bits of this field are valid and referred to the data arrangement in I2S mode FIFO operation section
     * |        |          |Note: In Master mode, SPI controller will start to transfer the SPI bus clock after 1 APB clock and 6 peripheral clock cycles after user writes to this register.
     * @var SPI_T::RX
     * Offset: 0x30  SPI Data Receive Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |RX        |Data Receive Register
     * |        |          |There are 4-level FIFO buffers in this controller
     * |        |          |The data receive register holds the data received from SPI data input pin
     * |        |          |If the RXEMPTY (SPIx_STATUS[8] or SPIx_I2SSTS[8]) is not set to 1, the receive FIFO buffers can be accessed through software by reading this register
     * |        |          |This is a read only register.
     * @var SPI_T::I2SCTL
     * Offset: 0x60  I2S Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |I2SEN     |I2S Controller Enable Bit
     * |        |          |0 = Disabled I2S mode.
     * |        |          |1 = Enabled I2S mode.
     * |        |          |Note:
     * |        |          |1. If enable this bit, I2Sx_BCLK will start to output in Master mode.
     * |        |          |2
     * |        |          |Before changing the configurations of SPIx_I2SCTL, SPIx_I2SCLK, and SPIx_FIFOCTL registers, user shall clear the I2SEN (SPIx_I2SCTL[0]) and confirm the I2SENSTS (SPIx_I2SSTS[15]) is 0.
     * |[1]     |TXEN      |Transmit Enable Bit
     * |        |          |0 = Data transmit Disabled.
     * |        |          |1 = Data transmit Enabled.
     * |[2]     |RXEN      |Receive Enable Bit
     * |        |          |0 = Data receive Disabled.
     * |        |          |1 = Data receive Enabled.
     * |[3]     |MUTE      |Transmit Mute Enable Bit
     * |        |          |0 = Transmit data is shifted from buffer.
     * |        |          |1 = Transmit channel zero.
     * |[5:4]   |WDWIDTH   |Word Width
     * |        |          |00 = data size is 8-bit.
     * |        |          |01 = data size is 16-bit.
     * |        |          |10 = data size is 24-bit.
     * |        |          |11 = data size is 32-bit.
     * |[6]     |MONO      |Monaural Data
     * |        |          |0 = Data is stereo format.
     * |        |          |1 = Data is monaural format.
     * |[7]     |ORDER     |Stereo Data Order in FIFO
     * |        |          |0 = Left channel data at high byte.
     * |        |          |1 = Left channel data at low byte.
     * |[8]     |SLAVE     |Slave Mode
     * |        |          |I2S can operate as master or slave
     * |        |          |For Master mode, I2Sx_BCLK and I2Sx_LRCLK pins are output mode and send bit clock from NuMicro M480 series to audio CODEC chip
     * |        |          |In Slave mode, I2Sx_BCLK and I2Sx_LRCLK pins are input mode and I2Sx_BCLK and I2Sx_LRCLK signals are received from outer audio CODEC chip.
     * |        |          |0 = Master mode.
     * |        |          |1 = Slave mode.
     * |[15]    |MCLKEN    |Master Clock Enable Bit
     * |        |          |If MCLKEN is set to 1, I2S controller will generate master clock on SPIx_I2SMCLK pin for external audio devices.
     * |        |          |0 = Master clock Disabled.
     * |        |          |1 = Master clock Enabled.
     * |[16]    |RZCEN     |Right Channel Zero Cross Detection Enable Bit
     * |        |          |If this bit is set to 1, when right channel data sign bit change or next shift data bits are all 0 then RZCIF flag in SPIx_I2SSTS register is set to 1
     * |        |          |This function is only available in transmit operation.
     * |        |          |0 = Right channel zero cross detection Disabled.
     * |        |          |1 = Right channel zero cross detection Enabled.
     * |[17]    |LZCEN     |Left Channel Zero Cross Detection Enable Bit
     * |        |          |If this bit is set to 1, when left channel data sign bit changes or next shift data bits are all 0 then LZCIF flag in SPIx_I2SSTS register is set to 1
     * |        |          |This function is only available in transmit operation.
     * |        |          |0 = Left channel zero cross detection Disabled.
     * |        |          |1 = Left channel zero cross detection Enabled.
     * |[23]    |RXLCH     |Receive Left Channel Enable Bit
     * |        |          |When monaural format is selected (MONO = 1), I2S controller will receive right channel data if RXLCH is set to 0, and receive left channel data if RXLCH is set to 1.
     * |        |          |0 = Receive right channel data in Mono mode.
     * |        |          |1 = Receive left channel data in Mono mode.
     * |[24]    |RZCIEN    |Right Channel Zero Cross Interrupt Enable Bit
     * |        |          |Interrupt occurs if this bit is set to 1 and right channel zero cross event occurs.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[25]    |LZCIEN    |Left Channel Zero Cross Interrupt Enable Bit
     * |        |          |Interrupt occurs if this bit is set to 1 and left channel zero cross event occurs.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[29:28] |FORMAT    |Data Format Selection
     * |        |          |00 = I2S data format.
     * |        |          |01 = MSB justified data format.
     * |        |          |10 = PCM mode A.
     * |        |          |11 = PCM mode B.
     * @var SPI_T::I2SCLK
     * Offset: 0x64  I2S Clock Divider Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[6:0]   |MCLKDIV   |Master Clock Divider
     * |        |          |If MCLKEN is set to 1, I2S controller will generate master clock for external audio devices
     * |        |          |The frequency of master clock, fMCLK, is determined by the following expressions:
     * |        |          |If MCLKDIV >= 1,.
     * |        |          |If MCLKDIV = 0,.
     * |        |          |where
     * |        |          |is the frequency of I2S peripheral clock source, which is defined in the clock control register CLK_CLKSEL2
     * |        |          |In general, the master clock rate is 256 times sampling clock rate.
     * |[17:8]  |BCLKDIV   |Bit Clock Divider
     * |        |          |The I2S controller will generate bit clock in Master mode
     * |        |          |The clock frequency of bit clock , fBCLK, is determined by the following expression:
     * |        |          |where
     * |        |          |is the frequency of I2S peripheral clock source, which is defined in the clock control register CLK_CLKSEL2.
     * |        |          |In I2S Slave mode, this field is used to define the frequency of peripheral clock and it's determined by .
     * |        |          |The peripheral clock frequency in I2S Slave mode must be equal to or faster than 6 times of input bit clock.
     * @var SPI_T::I2SSTS
     * Offset: 0x68  I2S Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4]     |RIGHT     |Right Channel (Read Only)
     * |        |          |This bit indicates the current transmit data is belong to which channel.
     * |        |          |0 = Left channel.
     * |        |          |1 = Right channel.
     * |[8]     |RXEMPTY   |Receive FIFO Buffer Empty Indicator (Read Only)
     * |        |          |0 = Receive FIFO buffer is not empty.
     * |        |          |1 = Receive FIFO buffer is empty.
     * |[9]     |RXFULL    |Receive FIFO Buffer Full Indicator (Read Only)
     * |        |          |0 = Receive FIFO buffer is not full.
     * |        |          |1 = Receive FIFO buffer is full.
     * |[10]    |RXTHIF    |Receive FIFO Threshold Interrupt Flag (Read Only)
     * |        |          |0 = The valid data count within the receive FIFO buffer is smaller than or equal to the setting value of RXTH.
     * |        |          |1 = The valid data count within the receive FIFO buffer is larger than the setting value of RXTH.
     * |        |          |Note: If RXTHIEN = 1 and RXTHIF = 1, the SPI/I2S controller will generate a SPI interrupt request.
     * |[11]    |RXOVIF    |Receive FIFO Overrun Interrupt Flag
     * |        |          |When the receive FIFO buffer is full, the follow-up data will be dropped and this bit will be set to 1.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[12]    |RXTOIF    |Receive Time-out Interrupt Flag
     * |        |          |0 = No receive FIFO time-out event.
     * |        |          |1 = Receive FIFO buffer is not empty and no read operation on receive FIFO buffer over 64 SPI peripheral clock period in Master mode or over 576 SPI peripheral clock period in Slave mode
     * |        |          |When the received FIFO buffer is read by software, the time-out status will be cleared automatically.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[15]    |I2SENSTS  |I2S Enable Status (Read Only)
     * |        |          |0 = The SPI/I2S control logic is disabled.
     * |        |          |1 = The SPI/I2S control logic is enabled.
     * |        |          |Note: The SPI peripheral clock is asynchronous with the system clock
     * |        |          |In order to make sure the SPI/I2S control logic is disabled, this bit indicates the real status of SPI/I2S control logic for user.
     * |[16]    |TXEMPTY   |Transmit FIFO Buffer Empty Indicator (Read Only)
     * |        |          |0 = Transmit FIFO buffer is not empty.
     * |        |          |1 = Transmit FIFO buffer is empty.
     * |[17]    |TXFULL    |Transmit FIFO Buffer Full Indicator (Read Only)
     * |        |          |0 = Transmit FIFO buffer is not full.
     * |        |          |1 = Transmit FIFO buffer is full.
     * |[18]    |TXTHIF    |Transmit FIFO Threshold Interrupt Flag (Read Only)
     * |        |          |0 = The valid data count within the transmit FIFO buffer is larger than the setting value of TXTH.
     * |        |          |1 = The valid data count within the transmit FIFO buffer is less than or equal to the setting value of TXTH.
     * |        |          |Note: If TXTHIEN = 1 and TXTHIF = 1, the SPI/I2S controller will generate a SPI interrupt request.
     * |[19]    |TXUFIF    |Transmit FIFO Underflow Interrupt Flag
     * |        |          |When the transmit FIFO buffer is empty and there is no datum written into the FIFO buffer, if there is more bus clock input, this bit will be set to 1.
     * |        |          |Note: This bit will be cleared by writing 1 to it.
     * |[20]    |RZCIF     |Right Channel Zero Cross Interrupt Flag
     * |        |          |0 = No zero cross event occurred on right channel.
     * |        |          |1 = Zero cross event occurred on right channel.
     * |[21]    |LZCIF     |Left Channel Zero Cross Interrupt Flag
     * |        |          |0 = No zero cross event occurred on left channel.
     * |        |          |1 = Zero cross event occurred on left channel.
     * |[23]    |TXRXRST   |TX or RX Reset Status (Read Only)
     * |        |          |0 = The reset function of TXRST or RXRST is done.
     * |        |          |1 = Doing the reset function of TXRST or RXRST.
     * |        |          |Note: Both the reset operations of TXRST and RXRST need 3 system clock cycles + 2 peripheral clock cycles
     * |        |          |User can check the status of this bit to monitor the reset function is doing or done.
     * |[26:24] |RXCNT     |Receive FIFO Data Count (Read Only)
     * |        |          |This bit field indicates the valid data count of receive FIFO buffer.
     * |[30:28] |TXCNT     |Transmit FIFO Data Count (Read Only)
     * |        |          |This bit field indicates the valid data count of transmit FIFO buffer.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] SPI Control Register                                             */
    __IO uint32_t CLKDIV;                /*!< [0x0004] SPI Clock Divider Register                                       */
    __IO uint32_t SSCTL;                 /*!< [0x0008] SPI Slave Select Control Register                                */
    __IO uint32_t PDMACTL;               /*!< [0x000c] SPI PDMA Control Register                                        */
    __IO uint32_t FIFOCTL;               /*!< [0x0010] SPI FIFO Control Register                                        */
    __IO uint32_t STATUS;                /*!< [0x0014] SPI Status Register                                              */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[2];
    /// @endcond //HIDDEN_SYMBOLS
    __O  uint32_t TX;                    /*!< [0x0020] SPI Data Transmit Register                                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[3];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t RX;                    /*!< [0x0030] SPI Data Receive Register                                        */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[11];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t I2SCTL;                /*!< [0x0060] I2S Control Register                                             */
    __IO uint32_t I2SCLK;                /*!< [0x0064] I2S Clock Divider Control Register                               */
    __IO uint32_t I2SSTS;                /*!< [0x0068] I2S Status Register                                              */

} SPI_T;

/**
    @addtogroup SPI_CONST SPI Bit Field Definition
    Constant Definitions for SPI Controller
@{ */

#define SPI_CTL_SPIEN_Pos                (0)                                               /*!< SPI_T::CTL: SPIEN Position             */
#define SPI_CTL_SPIEN_Msk                (0x1ul << SPI_CTL_SPIEN_Pos)                      /*!< SPI_T::CTL: SPIEN Mask                 */

#define SPI_CTL_RXNEG_Pos                (1)                                               /*!< SPI_T::CTL: RXNEG Position             */
#define SPI_CTL_RXNEG_Msk                (0x1ul << SPI_CTL_RXNEG_Pos)                      /*!< SPI_T::CTL: RXNEG Mask                 */

#define SPI_CTL_TXNEG_Pos                (2)                                               /*!< SPI_T::CTL: TXNEG Position             */
#define SPI_CTL_TXNEG_Msk                (0x1ul << SPI_CTL_TXNEG_Pos)                      /*!< SPI_T::CTL: TXNEG Mask                 */

#define SPI_CTL_CLKPOL_Pos               (3)                                               /*!< SPI_T::CTL: CLKPOL Position            */
#define SPI_CTL_CLKPOL_Msk               (0x1ul << SPI_CTL_CLKPOL_Pos)                     /*!< SPI_T::CTL: CLKPOL Mask                */

#define SPI_CTL_SUSPITV_Pos              (4)                                               /*!< SPI_T::CTL: SUSPITV Position           */
#define SPI_CTL_SUSPITV_Msk              (0xful << SPI_CTL_SUSPITV_Pos)                    /*!< SPI_T::CTL: SUSPITV Mask               */

#define SPI_CTL_DWIDTH_Pos               (8)                                               /*!< SPI_T::CTL: DWIDTH Position            */
#define SPI_CTL_DWIDTH_Msk               (0x1ful << SPI_CTL_DWIDTH_Pos)                    /*!< SPI_T::CTL: DWIDTH Mask                */

#define SPI_CTL_LSB_Pos                  (13)                                              /*!< SPI_T::CTL: LSB Position               */
#define SPI_CTL_LSB_Msk                  (0x1ul << SPI_CTL_LSB_Pos)                        /*!< SPI_T::CTL: LSB Mask                   */

#define SPI_CTL_HALFDPX_Pos              (14)                                              /*!< SPI_T::CTL: HALFDPX Position           */
#define SPI_CTL_HALFDPX_Msk              (0x1ul << SPI_CTL_HALFDPX_Pos)                    /*!< SPI_T::CTL: HALFDPX Mask               */

#define SPI_CTL_RXONLY_Pos               (15)                                              /*!< SPI_T::CTL: RXONLY Position            */
#define SPI_CTL_RXONLY_Msk               (0x1ul << SPI_CTL_RXONLY_Pos)                     /*!< SPI_T::CTL: RXONLY Mask                */

#define SPI_CTL_UNITIEN_Pos              (17)                                              /*!< SPI_T::CTL: UNITIEN Position           */
#define SPI_CTL_UNITIEN_Msk              (0x1ul << SPI_CTL_UNITIEN_Pos)                    /*!< SPI_T::CTL: UNITIEN Mask               */

#define SPI_CTL_SLAVE_Pos                (18)                                              /*!< SPI_T::CTL: SLAVE Position             */
#define SPI_CTL_SLAVE_Msk                (0x1ul << SPI_CTL_SLAVE_Pos)                      /*!< SPI_T::CTL: SLAVE Mask                 */

#define SPI_CTL_REORDER_Pos              (19)                                              /*!< SPI_T::CTL: REORDER Position           */
#define SPI_CTL_REORDER_Msk              (0x1ul << SPI_CTL_REORDER_Pos)                    /*!< SPI_T::CTL: REORDER Mask               */

#define SPI_CTL_DATDIR_Pos               (20)                                              /*!< SPI_T::CTL: DATDIR Position            */
#define SPI_CTL_DATDIR_Msk               (0x1ul << SPI_CTL_DATDIR_Pos)                     /*!< SPI_T::CTL: DATDIR Mask                */

#define SPI_CLKDIV_DIVIDER_Pos           (0)                                               /*!< SPI_T::CLKDIV: DIVIDER Position        */
#define SPI_CLKDIV_DIVIDER_Msk           (0x1fful << SPI_CLKDIV_DIVIDER_Pos)               /*!< SPI_T::CLKDIV: DIVIDER Mask            */

#define SPI_SSCTL_SS_Pos                 (0)                                               /*!< SPI_T::SSCTL: SS Position              */
#define SPI_SSCTL_SS_Msk                 (0x1ul << SPI_SSCTL_SS_Pos)                       /*!< SPI_T::SSCTL: SS Mask                  */

#define SPI_SSCTL_SSACTPOL_Pos           (2)                                               /*!< SPI_T::SSCTL: SSACTPOL Position        */
#define SPI_SSCTL_SSACTPOL_Msk           (0x1ul << SPI_SSCTL_SSACTPOL_Pos)                 /*!< SPI_T::SSCTL: SSACTPOL Mask            */

#define SPI_SSCTL_AUTOSS_Pos             (3)                                               /*!< SPI_T::SSCTL: AUTOSS Position          */
#define SPI_SSCTL_AUTOSS_Msk             (0x1ul << SPI_SSCTL_AUTOSS_Pos)                   /*!< SPI_T::SSCTL: AUTOSS Mask              */

#define SPI_SSCTL_SLVBEIEN_Pos           (8)                                               /*!< SPI_T::SSCTL: SLVBEIEN Position        */
#define SPI_SSCTL_SLVBEIEN_Msk           (0x1ul << SPI_SSCTL_SLVBEIEN_Pos)                 /*!< SPI_T::SSCTL: SLVBEIEN Mask            */

#define SPI_SSCTL_SLVURIEN_Pos           (9)                                               /*!< SPI_T::SSCTL: SLVURIEN Position        */
#define SPI_SSCTL_SLVURIEN_Msk           (0x1ul << SPI_SSCTL_SLVURIEN_Pos)                 /*!< SPI_T::SSCTL: SLVURIEN Mask            */

#define SPI_SSCTL_SSACTIEN_Pos           (12)                                              /*!< SPI_T::SSCTL: SSACTIEN Position        */
#define SPI_SSCTL_SSACTIEN_Msk           (0x1ul << SPI_SSCTL_SSACTIEN_Pos)                 /*!< SPI_T::SSCTL: SSACTIEN Mask            */

#define SPI_SSCTL_SSINAIEN_Pos           (13)                                              /*!< SPI_T::SSCTL: SSINAIEN Position        */
#define SPI_SSCTL_SSINAIEN_Msk           (0x1ul << SPI_SSCTL_SSINAIEN_Pos)                 /*!< SPI_T::SSCTL: SSINAIEN Mask            */

#define SPI_SSCTL_SLVTOCNT_Pos           (16)                                              /*!< SPI_T::SSCTL: SLVTOCNT Position        */
#define SPI_SSCTL_SLVTOCNT_Msk           (0xfffful << SPI_SSCTL_SLVTOCNT_Pos)              /*!< SPI_T::SSCTL: SLVTOCNT Mask            */

#define SPI_PDMACTL_TXPDMAEN_Pos         (0)                                               /*!< SPI_T::PDMACTL: TXPDMAEN Position      */
#define SPI_PDMACTL_TXPDMAEN_Msk         (0x1ul << SPI_PDMACTL_TXPDMAEN_Pos)               /*!< SPI_T::PDMACTL: TXPDMAEN Mask          */

#define SPI_PDMACTL_RXPDMAEN_Pos         (1)                                               /*!< SPI_T::PDMACTL: RXPDMAEN Position      */
#define SPI_PDMACTL_RXPDMAEN_Msk         (0x1ul << SPI_PDMACTL_RXPDMAEN_Pos)               /*!< SPI_T::PDMACTL: RXPDMAEN Mask          */

#define SPI_PDMACTL_PDMARST_Pos          (2)                                               /*!< SPI_T::PDMACTL: PDMARST Position       */
#define SPI_PDMACTL_PDMARST_Msk          (0x1ul << SPI_PDMACTL_PDMARST_Pos)                /*!< SPI_T::PDMACTL: PDMARST Mask           */

#define SPI_FIFOCTL_RXRST_Pos            (0)                                               /*!< SPI_T::FIFOCTL: RXRST Position         */
#define SPI_FIFOCTL_RXRST_Msk            (0x1ul << SPI_FIFOCTL_RXRST_Pos)                  /*!< SPI_T::FIFOCTL: RXRST Mask             */

#define SPI_FIFOCTL_TXRST_Pos            (1)                                               /*!< SPI_T::FIFOCTL: TXRST Position         */
#define SPI_FIFOCTL_TXRST_Msk            (0x1ul << SPI_FIFOCTL_TXRST_Pos)                  /*!< SPI_T::FIFOCTL: TXRST Mask             */

#define SPI_FIFOCTL_RXTHIEN_Pos          (2)                                               /*!< SPI_T::FIFOCTL: RXTHIEN Position       */
#define SPI_FIFOCTL_RXTHIEN_Msk          (0x1ul << SPI_FIFOCTL_RXTHIEN_Pos)                /*!< SPI_T::FIFOCTL: RXTHIEN Mask           */

#define SPI_FIFOCTL_TXTHIEN_Pos          (3)                                               /*!< SPI_T::FIFOCTL: TXTHIEN Position       */
#define SPI_FIFOCTL_TXTHIEN_Msk          (0x1ul << SPI_FIFOCTL_TXTHIEN_Pos)                /*!< SPI_T::FIFOCTL: TXTHIEN Mask           */

#define SPI_FIFOCTL_RXTOIEN_Pos          (4)                                               /*!< SPI_T::FIFOCTL: RXTOIEN Position       */
#define SPI_FIFOCTL_RXTOIEN_Msk          (0x1ul << SPI_FIFOCTL_RXTOIEN_Pos)                /*!< SPI_T::FIFOCTL: RXTOIEN Mask           */

#define SPI_FIFOCTL_RXOVIEN_Pos          (5)                                               /*!< SPI_T::FIFOCTL: RXOVIEN Position       */
#define SPI_FIFOCTL_RXOVIEN_Msk          (0x1ul << SPI_FIFOCTL_RXOVIEN_Pos)                /*!< SPI_T::FIFOCTL: RXOVIEN Mask           */

#define SPI_FIFOCTL_TXUFPOL_Pos          (6)                                               /*!< SPI_T::FIFOCTL: TXUFPOL Position       */
#define SPI_FIFOCTL_TXUFPOL_Msk          (0x1ul << SPI_FIFOCTL_TXUFPOL_Pos)                /*!< SPI_T::FIFOCTL: TXUFPOL Mask           */

#define SPI_FIFOCTL_TXUFIEN_Pos          (7)                                               /*!< SPI_T::FIFOCTL: TXUFIEN Position       */
#define SPI_FIFOCTL_TXUFIEN_Msk          (0x1ul << SPI_FIFOCTL_TXUFIEN_Pos)                /*!< SPI_T::FIFOCTL: TXUFIEN Mask           */

#define SPI_FIFOCTL_RXFBCLR_Pos          (8)                                               /*!< SPI_T::FIFOCTL: RXFBCLR Position       */
#define SPI_FIFOCTL_RXFBCLR_Msk          (0x1ul << SPI_FIFOCTL_RXFBCLR_Pos)                /*!< SPI_T::FIFOCTL: RXFBCLR Mask           */

#define SPI_FIFOCTL_TXFBCLR_Pos          (9)                                               /*!< SPI_T::FIFOCTL: TXFBCLR Position       */
#define SPI_FIFOCTL_TXFBCLR_Msk          (0x1ul << SPI_FIFOCTL_TXFBCLR_Pos)                /*!< SPI_T::FIFOCTL: TXFBCLR Mask           */

#define SPI_FIFOCTL_RXTH_Pos             (24)                                              /*!< SPI_T::FIFOCTL: RXTH Position          */
#define SPI_FIFOCTL_RXTH_Msk             (0x7ul << SPI_FIFOCTL_RXTH_Pos)                   /*!< SPI_T::FIFOCTL: RXTH Mask              */

#define SPI_FIFOCTL_TXTH_Pos             (28)                                              /*!< SPI_T::FIFOCTL: TXTH Position          */
#define SPI_FIFOCTL_TXTH_Msk             (0x7ul << SPI_FIFOCTL_TXTH_Pos)                   /*!< SPI_T::FIFOCTL: TXTH Mask              */

#define SPI_STATUS_BUSY_Pos              (0)                                               /*!< SPI_T::STATUS: BUSY Position           */
#define SPI_STATUS_BUSY_Msk              (0x1ul << SPI_STATUS_BUSY_Pos)                    /*!< SPI_T::STATUS: BUSY Mask               */

#define SPI_STATUS_UNITIF_Pos            (1)                                               /*!< SPI_T::STATUS: UNITIF Position         */
#define SPI_STATUS_UNITIF_Msk            (0x1ul << SPI_STATUS_UNITIF_Pos)                  /*!< SPI_T::STATUS: UNITIF Mask             */

#define SPI_STATUS_SSACTIF_Pos           (2)                                               /*!< SPI_T::STATUS: SSACTIF Position        */
#define SPI_STATUS_SSACTIF_Msk           (0x1ul << SPI_STATUS_SSACTIF_Pos)                 /*!< SPI_T::STATUS: SSACTIF Mask            */

#define SPI_STATUS_SSINAIF_Pos           (3)                                               /*!< SPI_T::STATUS: SSINAIF Position        */
#define SPI_STATUS_SSINAIF_Msk           (0x1ul << SPI_STATUS_SSINAIF_Pos)                 /*!< SPI_T::STATUS: SSINAIF Mask            */

#define SPI_STATUS_SSLINE_Pos            (4)                                               /*!< SPI_T::STATUS: SSLINE Position         */
#define SPI_STATUS_SSLINE_Msk            (0x1ul << SPI_STATUS_SSLINE_Pos)                  /*!< SPI_T::STATUS: SSLINE Mask             */

#define SPI_STATUS_SLVBEIF_Pos           (6)                                               /*!< SPI_T::STATUS: SLVBEIF Position        */
#define SPI_STATUS_SLVBEIF_Msk           (0x1ul << SPI_STATUS_SLVBEIF_Pos)                 /*!< SPI_T::STATUS: SLVBEIF Mask            */

#define SPI_STATUS_SLVURIF_Pos           (7)                                               /*!< SPI_T::STATUS: SLVURIF Position        */
#define SPI_STATUS_SLVURIF_Msk           (0x1ul << SPI_STATUS_SLVURIF_Pos)                 /*!< SPI_T::STATUS: SLVURIF Mask            */

#define SPI_STATUS_RXEMPTY_Pos           (8)                                               /*!< SPI_T::STATUS: RXEMPTY Position        */
#define SPI_STATUS_RXEMPTY_Msk           (0x1ul << SPI_STATUS_RXEMPTY_Pos)                 /*!< SPI_T::STATUS: RXEMPTY Mask            */

#define SPI_STATUS_RXFULL_Pos            (9)                                               /*!< SPI_T::STATUS: RXFULL Position         */
#define SPI_STATUS_RXFULL_Msk            (0x1ul << SPI_STATUS_RXFULL_Pos)                  /*!< SPI_T::STATUS: RXFULL Mask             */

#define SPI_STATUS_RXTHIF_Pos            (10)                                              /*!< SPI_T::STATUS: RXTHIF Position         */
#define SPI_STATUS_RXTHIF_Msk            (0x1ul << SPI_STATUS_RXTHIF_Pos)                  /*!< SPI_T::STATUS: RXTHIF Mask             */

#define SPI_STATUS_RXOVIF_Pos            (11)                                              /*!< SPI_T::STATUS: RXOVIF Position         */
#define SPI_STATUS_RXOVIF_Msk            (0x1ul << SPI_STATUS_RXOVIF_Pos)                  /*!< SPI_T::STATUS: RXOVIF Mask             */

#define SPI_STATUS_RXTOIF_Pos            (12)                                              /*!< SPI_T::STATUS: RXTOIF Position         */
#define SPI_STATUS_RXTOIF_Msk            (0x1ul << SPI_STATUS_RXTOIF_Pos)                  /*!< SPI_T::STATUS: RXTOIF Mask             */

#define SPI_STATUS_SPIENSTS_Pos          (15)                                              /*!< SPI_T::STATUS: SPIENSTS Position       */
#define SPI_STATUS_SPIENSTS_Msk          (0x1ul << SPI_STATUS_SPIENSTS_Pos)                /*!< SPI_T::STATUS: SPIENSTS Mask           */

#define SPI_STATUS_TXEMPTY_Pos           (16)                                              /*!< SPI_T::STATUS: TXEMPTY Position        */
#define SPI_STATUS_TXEMPTY_Msk           (0x1ul << SPI_STATUS_TXEMPTY_Pos)                 /*!< SPI_T::STATUS: TXEMPTY Mask            */

#define SPI_STATUS_TXFULL_Pos            (17)                                              /*!< SPI_T::STATUS: TXFULL Position         */
#define SPI_STATUS_TXFULL_Msk            (0x1ul << SPI_STATUS_TXFULL_Pos)                  /*!< SPI_T::STATUS: TXFULL Mask             */

#define SPI_STATUS_TXTHIF_Pos            (18)                                              /*!< SPI_T::STATUS: TXTHIF Position         */
#define SPI_STATUS_TXTHIF_Msk            (0x1ul << SPI_STATUS_TXTHIF_Pos)                  /*!< SPI_T::STATUS: TXTHIF Mask             */

#define SPI_STATUS_TXUFIF_Pos            (19)                                              /*!< SPI_T::STATUS: TXUFIF Position         */
#define SPI_STATUS_TXUFIF_Msk            (0x1ul << SPI_STATUS_TXUFIF_Pos)                  /*!< SPI_T::STATUS: TXUFIF Mask             */

#define SPI_STATUS_TXRXRST_Pos           (23)                                              /*!< SPI_T::STATUS: TXRXRST Position        */
#define SPI_STATUS_TXRXRST_Msk           (0x1ul << SPI_STATUS_TXRXRST_Pos)                 /*!< SPI_T::STATUS: TXRXRST Mask            */

#define SPI_STATUS_RXCNT_Pos             (24)                                              /*!< SPI_T::STATUS: RXCNT Position          */
#define SPI_STATUS_RXCNT_Msk             (0xful << SPI_STATUS_RXCNT_Pos)                   /*!< SPI_T::STATUS: RXCNT Mask              */

#define SPI_STATUS_TXCNT_Pos             (28)                                              /*!< SPI_T::STATUS: TXCNT Position          */
#define SPI_STATUS_TXCNT_Msk             (0xful << SPI_STATUS_TXCNT_Pos)                   /*!< SPI_T::STATUS: TXCNT Mask              */

#define SPI_TX_TX_Pos                    (0)                                               /*!< SPI_T::TX: TX Position                 */
#define SPI_TX_TX_Msk                    (0xfffffffful << SPI_TX_TX_Pos)                   /*!< SPI_T::TX: TX Mask                     */

#define SPI_RX_RX_Pos                    (0)                                               /*!< SPI_T::RX: RX Position                 */
#define SPI_RX_RX_Msk                    (0xfffffffful << SPI_RX_RX_Pos)                   /*!< SPI_T::RX: RX Mask                     */

#define SPI_I2SCTL_I2SEN_Pos             (0)                                               /*!< SPI_T::I2SCTL: I2SEN Position          */
#define SPI_I2SCTL_I2SEN_Msk             (0x1ul << SPI_I2SCTL_I2SEN_Pos)                   /*!< SPI_T::I2SCTL: I2SEN Mask              */

#define SPI_I2SCTL_TXEN_Pos              (1)                                               /*!< SPI_T::I2SCTL: TXEN Position           */
#define SPI_I2SCTL_TXEN_Msk              (0x1ul << SPI_I2SCTL_TXEN_Pos)                    /*!< SPI_T::I2SCTL: TXEN Mask               */

#define SPI_I2SCTL_RXEN_Pos              (2)                                               /*!< SPI_T::I2SCTL: RXEN Position           */
#define SPI_I2SCTL_RXEN_Msk              (0x1ul << SPI_I2SCTL_RXEN_Pos)                    /*!< SPI_T::I2SCTL: RXEN Mask               */

#define SPI_I2SCTL_MUTE_Pos              (3)                                               /*!< SPI_T::I2SCTL: MUTE Position           */
#define SPI_I2SCTL_MUTE_Msk              (0x1ul << SPI_I2SCTL_MUTE_Pos)                    /*!< SPI_T::I2SCTL: MUTE Mask               */

#define SPI_I2SCTL_WDWIDTH_Pos           (4)                                               /*!< SPI_T::I2SCTL: WDWIDTH Position        */
#define SPI_I2SCTL_WDWIDTH_Msk           (0x3ul << SPI_I2SCTL_WDWIDTH_Pos)                 /*!< SPI_T::I2SCTL: WDWIDTH Mask            */

#define SPI_I2SCTL_MONO_Pos              (6)                                               /*!< SPI_T::I2SCTL: MONO Position           */
#define SPI_I2SCTL_MONO_Msk              (0x1ul << SPI_I2SCTL_MONO_Pos)                    /*!< SPI_T::I2SCTL: MONO Mask               */

#define SPI_I2SCTL_ORDER_Pos             (7)                                               /*!< SPI_T::I2SCTL: ORDER Position          */
#define SPI_I2SCTL_ORDER_Msk             (0x1ul << SPI_I2SCTL_ORDER_Pos)                   /*!< SPI_T::I2SCTL: ORDER Mask              */

#define SPI_I2SCTL_SLAVE_Pos             (8)                                               /*!< SPI_T::I2SCTL: SLAVE Position          */
#define SPI_I2SCTL_SLAVE_Msk             (0x1ul << SPI_I2SCTL_SLAVE_Pos)                   /*!< SPI_T::I2SCTL: SLAVE Mask              */

#define SPI_I2SCTL_MCLKEN_Pos            (15)                                              /*!< SPI_T::I2SCTL: MCLKEN Position         */
#define SPI_I2SCTL_MCLKEN_Msk            (0x1ul << SPI_I2SCTL_MCLKEN_Pos)                  /*!< SPI_T::I2SCTL: MCLKEN Mask             */

#define SPI_I2SCTL_RZCEN_Pos             (16)                                              /*!< SPI_T::I2SCTL: RZCEN Position          */
#define SPI_I2SCTL_RZCEN_Msk             (0x1ul << SPI_I2SCTL_RZCEN_Pos)                   /*!< SPI_T::I2SCTL: RZCEN Mask              */

#define SPI_I2SCTL_LZCEN_Pos             (17)                                              /*!< SPI_T::I2SCTL: LZCEN Position          */
#define SPI_I2SCTL_LZCEN_Msk             (0x1ul << SPI_I2SCTL_LZCEN_Pos)                   /*!< SPI_T::I2SCTL: LZCEN Mask              */

#define SPI_I2SCTL_RXLCH_Pos             (23)                                              /*!< SPI_T::I2SCTL: RXLCH Position          */
#define SPI_I2SCTL_RXLCH_Msk             (0x1ul << SPI_I2SCTL_RXLCH_Pos)                   /*!< SPI_T::I2SCTL: RXLCH Mask              */

#define SPI_I2SCTL_RZCIEN_Pos            (24)                                              /*!< SPI_T::I2SCTL: RZCIEN Position         */
#define SPI_I2SCTL_RZCIEN_Msk            (0x1ul << SPI_I2SCTL_RZCIEN_Pos)                  /*!< SPI_T::I2SCTL: RZCIEN Mask             */

#define SPI_I2SCTL_LZCIEN_Pos            (25)                                              /*!< SPI_T::I2SCTL: LZCIEN Position         */
#define SPI_I2SCTL_LZCIEN_Msk            (0x1ul << SPI_I2SCTL_LZCIEN_Pos)                  /*!< SPI_T::I2SCTL: LZCIEN Mask             */

#define SPI_I2SCTL_FORMAT_Pos            (28)                                              /*!< SPI_T::I2SCTL: FORMAT Position         */
#define SPI_I2SCTL_FORMAT_Msk            (0x3ul << SPI_I2SCTL_FORMAT_Pos)                  /*!< SPI_T::I2SCTL: FORMAT Mask             */

#define SPI_I2SCLK_MCLKDIV_Pos           (0)                                               /*!< SPI_T::I2SCLK: MCLKDIV Position        */
#define SPI_I2SCLK_MCLKDIV_Msk           (0x7ful << SPI_I2SCLK_MCLKDIV_Pos)                /*!< SPI_T::I2SCLK: MCLKDIV Mask            */

#define SPI_I2SCLK_BCLKDIV_Pos           (8)                                               /*!< SPI_T::I2SCLK: BCLKDIV Position        */
#define SPI_I2SCLK_BCLKDIV_Msk           (0x3fful << SPI_I2SCLK_BCLKDIV_Pos)               /*!< SPI_T::I2SCLK: BCLKDIV Mask            */

#define SPI_I2SSTS_RIGHT_Pos             (4)                                               /*!< SPI_T::I2SSTS: RIGHT Position          */
#define SPI_I2SSTS_RIGHT_Msk             (0x1ul << SPI_I2SSTS_RIGHT_Pos)                   /*!< SPI_T::I2SSTS: RIGHT Mask              */

#define SPI_I2SSTS_RXEMPTY_Pos           (8)                                               /*!< SPI_T::I2SSTS: RXEMPTY Position        */
#define SPI_I2SSTS_RXEMPTY_Msk           (0x1ul << SPI_I2SSTS_RXEMPTY_Pos)                 /*!< SPI_T::I2SSTS: RXEMPTY Mask            */

#define SPI_I2SSTS_RXFULL_Pos            (9)                                               /*!< SPI_T::I2SSTS: RXFULL Position         */
#define SPI_I2SSTS_RXFULL_Msk            (0x1ul << SPI_I2SSTS_RXFULL_Pos)                  /*!< SPI_T::I2SSTS: RXFULL Mask             */

#define SPI_I2SSTS_RXTHIF_Pos            (10)                                              /*!< SPI_T::I2SSTS: RXTHIF Position         */
#define SPI_I2SSTS_RXTHIF_Msk            (0x1ul << SPI_I2SSTS_RXTHIF_Pos)                  /*!< SPI_T::I2SSTS: RXTHIF Mask             */

#define SPI_I2SSTS_RXOVIF_Pos            (11)                                              /*!< SPI_T::I2SSTS: RXOVIF Position         */
#define SPI_I2SSTS_RXOVIF_Msk            (0x1ul << SPI_I2SSTS_RXOVIF_Pos)                  /*!< SPI_T::I2SSTS: RXOVIF Mask             */

#define SPI_I2SSTS_RXTOIF_Pos            (12)                                              /*!< SPI_T::I2SSTS: RXTOIF Position         */
#define SPI_I2SSTS_RXTOIF_Msk            (0x1ul << SPI_I2SSTS_RXTOIF_Pos)                  /*!< SPI_T::I2SSTS: RXTOIF Mask             */

#define SPI_I2SSTS_I2SENSTS_Pos          (15)                                              /*!< SPI_T::I2SSTS: I2SENSTS Position       */
#define SPI_I2SSTS_I2SENSTS_Msk          (0x1ul << SPI_I2SSTS_I2SENSTS_Pos)                /*!< SPI_T::I2SSTS: I2SENSTS Mask           */

#define SPI_I2SSTS_TXEMPTY_Pos           (16)                                              /*!< SPI_T::I2SSTS: TXEMPTY Position        */
#define SPI_I2SSTS_TXEMPTY_Msk           (0x1ul << SPI_I2SSTS_TXEMPTY_Pos)                 /*!< SPI_T::I2SSTS: TXEMPTY Mask            */

#define SPI_I2SSTS_TXFULL_Pos            (17)                                              /*!< SPI_T::I2SSTS: TXFULL Position         */
#define SPI_I2SSTS_TXFULL_Msk            (0x1ul << SPI_I2SSTS_TXFULL_Pos)                  /*!< SPI_T::I2SSTS: TXFULL Mask             */

#define SPI_I2SSTS_TXTHIF_Pos            (18)                                              /*!< SPI_T::I2SSTS: TXTHIF Position         */
#define SPI_I2SSTS_TXTHIF_Msk            (0x1ul << SPI_I2SSTS_TXTHIF_Pos)                  /*!< SPI_T::I2SSTS: TXTHIF Mask             */

#define SPI_I2SSTS_TXUFIF_Pos            (19)                                              /*!< SPI_T::I2SSTS: TXUFIF Position         */
#define SPI_I2SSTS_TXUFIF_Msk            (0x1ul << SPI_I2SSTS_TXUFIF_Pos)                  /*!< SPI_T::I2SSTS: TXUFIF Mask             */

#define SPI_I2SSTS_RZCIF_Pos             (20)                                              /*!< SPI_T::I2SSTS: RZCIF Position          */
#define SPI_I2SSTS_RZCIF_Msk             (0x1ul << SPI_I2SSTS_RZCIF_Pos)                   /*!< SPI_T::I2SSTS: RZCIF Mask              */

#define SPI_I2SSTS_LZCIF_Pos             (21)                                              /*!< SPI_T::I2SSTS: LZCIF Position          */
#define SPI_I2SSTS_LZCIF_Msk             (0x1ul << SPI_I2SSTS_LZCIF_Pos)                   /*!< SPI_T::I2SSTS: LZCIF Mask              */

#define SPI_I2SSTS_TXRXRST_Pos           (23)                                              /*!< SPI_T::I2SSTS: TXRXRST Position        */
#define SPI_I2SSTS_TXRXRST_Msk           (0x1ul << SPI_I2SSTS_TXRXRST_Pos)                 /*!< SPI_T::I2SSTS: TXRXRST Mask            */

#define SPI_I2SSTS_RXCNT_Pos             (24)                                              /*!< SPI_T::I2SSTS: RXCNT Position          */
#define SPI_I2SSTS_RXCNT_Msk             (0x7ul << SPI_I2SSTS_RXCNT_Pos)                   /*!< SPI_T::I2SSTS: RXCNT Mask              */

#define SPI_I2SSTS_TXCNT_Pos             (28)                                              /*!< SPI_T::I2SSTS: TXCNT Position          */
#define SPI_I2SSTS_TXCNT_Msk             (0x7ul << SPI_I2SSTS_TXCNT_Pos)                   /*!< SPI_T::I2SSTS: TXCNT Mask              */

/**@}*/ /* SPI_CONST */
/**@}*/ /* end of SPI register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __SPI_REG_H__ */
