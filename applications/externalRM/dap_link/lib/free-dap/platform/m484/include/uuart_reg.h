/**************************************************************************//**
 * @file     uuart_reg.h
 * @version  V1.00
 * @brief    UUART register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __UUART_REG_H__
#define __UUART_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup UUART UART Mode of USCI Controller(UUART)
    Memory Mapped Structure for UUART Controller
@{ */

typedef struct
{


    /**
     * @var UUART_T::CTL
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
     * @var UUART_T::INTEN
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
     * |[3]     |RXSTIEN   |Receive Start Interrupt Enable BIt
     * |        |          |This bit enables the interrupt generation in case of a receive start event.
     * |        |          |0 = The receive start interrupt is disabled.
     * |        |          |1 = The receive start interrupt is enabled.
     * |[4]     |RXENDIEN  |Receive End Interrupt Enable Bit
     * |        |          |This bit enables the interrupt generation in case of a receive finish event.
     * |        |          |0 = The receive end interrupt is disabled.
     * |        |          |1 = The receive end interrupt is enabled.
     * @var UUART_T::BRGEN
     * Offset: 0x08  USCI Baud Rate Generator Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RCLKSEL   |Reference Clock Source Selection
     * |        |          |This bit selects the source signal of reference clock (fREF_CLK).
     * |        |          |0 = Peripheral device clock fPCLK.
     * |        |          |1 = Reserved.
     * |[1]     |PTCLKSEL  |Protocol Clock Source Selection
     * |        |          |This bit selects the source signal of protocol clock (fPROT_CLK).
     * |        |          |0 = Reference clock fREF_CLK.
     * |        |          |1 = fREF_CLK2 (its frequency is half of fREF_CLK).
     * |[3:2]   |SPCLKSEL  |Sample Clock Source Selection
     * |        |          |This bit field used for the clock source selection of a sample clock (fSAMP_CLK) for the protocol processor.
     * |        |          |00 = fSAMP_CLK = fDIV_CLK.
     * |        |          |01 = fSAMP_CLK = fPROT_CLK.
     * |        |          |10 = fSAMP_CLK = fSCLK.
     * |        |          |11 = fSAMP_CLK = fREF_CLK.
     * |[4]     |TMCNTEN   |Timing Measurement Counter Enable Bit
     * |        |          |This bit enables the 10-bit timing measurement counter.
     * |        |          |0 = Timing measurement counter is Disabled.
     * |        |          |1 = Timing measurement counter is Enabled.
     * |[5]     |TMCNTSRC  |Timing Measurement Counter Clock Source Selection
     * |        |          |0 = Timing measurement counter with fPROT_CLK.
     * |        |          |1 = Timing measurement counter with fDIV_CLK.
     * |[9:8]   |PDSCNT    |Pre-divider for Sample Counter
     * |        |          |This bit field defines the divide ratio of the clock division from sample clock fSAMP_CLK
     * |        |          |The divided frequency fPDS_CNT = fSAMP_CLK / (PDSCNT+1).
     * |[14:10] |DSCNT     |Denominator for Sample Counter
     * |        |          |This bit field defines the divide ratio of the sample clock fSAMP_CLK.
     * |        |          |The divided frequency fDS_CNT = fPDS_CNT / (DSCNT+1).
     * |        |          |Note: The maximum value of DSCNT is 0xF on UART mode and suggest to set over 4 to confirm the receiver data is sampled in right value
     * |[25:16] |CLKDIV    |Clock Divider
     * |        |          |This bit field defines the ratio between the protocol clock frequency fPROT_CLK and
     * |        |          |the clock divider frequency fDIV_CLK (fDIV_CLK = fPROT_CLK / (CLKDIV+1) ).
     * |        |          |Note: In UART function, it can be updated by hardware in the 4th falling edge of the input data 0x55
     * |        |          |when the auto baud rate function (ABREN(USCI_PROTCTL[6])) is enabled
     * |        |          |The revised value is the average bit time between bit 5 and bit 6
     * |        |          |The user can use revised CLKDIV and new BRDETITV (USCI_PROTCTL[24:16]) to calculate the precise baud rate.
     * @var UUART_T::DATIN0
     * Offset: 0x10  USCI Input Data Signal Configuration Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SYNCSEL   |Input Signal Synchronization Selection
     * |        |          |This bit selects if the un-synchronized input signal (with optionally inverted) or
     * |        |          |the synchronized (and optionally filtered) signal can be used as input for the   data shift unit.
     * |        |          |0 = The un-synchronized signal can be taken as input for the data shift unit.
     * |        |          |1 = The synchronized signal can be taken as input for the data shift unit.
     * |[2]     |ININV     |Input Signal Inverse Selection
     * |        |          |This bit defines the inverter enable of the input asynchronous signal.
     * |        |          |0 = The un-synchronized input signal will not be inverted.
     * |        |          |1 = The un-synchronized input signal will be inverted.
     * |[4:3]   |EDGEDET   |Input Signal   Edge Detection Mode
     * |        |          |This bit field selects which edge actives the trigger event of input data signal.
     * |        |          |00 = The trigger event activation is disabled.
     * |        |          |01 = A rising   edge activates the trigger event of input data signal.
     * |        |          |10 = A falling edge activates the trigger event of input data signal.
     * |        |          |11 = Both edges activate the trigger event of input data signal.
     * |        |          |Note: In UART function mode, it is suggested to   set this bit field as 10.
     * @var UUART_T::CTLIN0
     * Offset: 0x20  USCI Input Control Signal Configuration Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SYNCSEL   |Input Synchronization Signal Selection
     * |        |          |This bit selects if the un-synchronized input signal (with optionally inverted) or
     * |        |          |the synchronized (and optionally filtered) signal can be used as input for the   data shift unit.
     * |        |          |0 = The un-synchronized signal can be taken as input for the data shift unit.
     * |        |          |1 = The   synchronized signal can be taken as input for the data shift unit.
     * |[2]     |ININV     |Input Signal Inverse Selection
     * |        |          |This bit defines the inverter enable of the input asynchronous signal.
     * |        |          |0 = The un-synchronized input signal will not be inverted.
     * |        |          |1 = The un-synchronized input signal will be inverted.
     * @var UUART_T::CLKIN
     * Offset: 0x28  USCI Input Clock Signal Configuration Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SYNCSEL   |Input Synchronization Signal Selection
     * |        |          |This bit selects if the un-synchronized input signal or
     * |        |          |the synchronized (and optionally filtered) signal can be used as input for the data shift unit.
     * |        |          |0 = The un-synchronized signal can be taken as input for the data shift unit.
     * |        |          |1 = The synchronized signal can be taken as input for the data shift unit.
     * @var UUART_T::LINECTL
     * Offset: 0x2C  USCI Line Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LSB       |LSB First Transmission Selection
     * |        |          |0 = The MSB, which bit of transmit/receive data buffer depends on the setting of DWIDTH, is transmitted/received first.
     * |        |          |1 = The LSB, the bit 0 of data buffer, will be transmitted/received first.
     * |[5]     |DATOINV   |Data Output Inverse Selection
     * |        |          |This bit defines the relation between the internal shift data value and the output data signal of USCIx_DAT1 pin.
     * |        |          |0 = The value of USCIx_DAT1 is equal to the data shift register.
     * |        |          |1 = The value of USCIx_DAT1 is the inversion of data shift register.
     * |[7]     |CTLOINV   |Control Signal Output Inverse Selection
     * |        |          |This bit defines the relation between the internal control signal and the output control signal.
     * |        |          |0 = No effect.
     * |        |          |1 = The control signal will be inverted before its output.
     * |        |          |Note: In UART protocol, the control signal means nRTS signal.
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
     * |        |          |..
     * |        |          |0xF: The data word contains 15 bits located at bit positions [14:0].
     * |        |          |Note: In UART protocol, the length can be configured as 6~13 bits.
     * @var UUART_T::TXDAT
     * Offset: 0x30  USCI Transmit Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TXDAT     |Transmit Data
     * |        |          |Software can use this bit field to write 16-bit transmit data for transmission.
     * @var UUART_T::RXDAT
     * Offset: 0x34  USCI Receive Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RXDAT     |Received Data
     * |        |          |This bit field monitors the received data which stored in receive data buffer.
     * |        |          |Note: RXDAT[15:13] indicate the same frame status of BREAK, FRMERR and PARITYERR (USCI_PROTSTS[7:5]).
     * @var UUART_T::BUFCTL
     * Offset: 0x38  USCI Transmit/Receive Buffer Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7]     |TXCLR     |Clear Transmit Buffer
     * |        |          |0 = No effect.
     * |        |          |1 = The transmit buffer is cleared (filling level is cleared and output pointer is set to input pointer value)
     * |        |          |Should only be used while the buffer is not taking part in data traffic.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * |[14]    |RXOVIEN   |Receive Buffer Overrun Error Interrupt Enable Control
     * |        |          |0 = Receive overrun interrupt Disabled.
     * |        |          |1 = Receive overrun interrupt Enabled.
     * |[15]    |RXCLR     |Clear Receive Buffer
     * |        |          |0 = No effect.
     * |        |          |1 = The receive buffer is cleared (filling level is cleared and output pointer is set to input pointer value)
     * |        |          |Should only be used while the buffer is not taking part in data traffic.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * |[16]    |TXRST     |Transmit Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the transmit-related counters, state machine, and the content of transmit shift register and data buffer.
     * |        |          |Note: It is cleared automatically after one PCLK cycle.
     * |[17]    |RXRST     |Receive Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the receive-related counters, state machine, and the content of receive shift register and data buffer.
     * |        |          |Note 1: It is cleared automatically after one PCLK cycle.
     * |        |          |Note 2: It is suggest to check the RXBUSY (USCI_PROTSTS[10]) before this bit will be set to 1.
     * @var UUART_T::BUFSTS
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
     * |[3]     |RXOVIF    |Receive Buffer Over-run Error Interrupt Status
     * |        |          |This bit indicates that a receive buffer overrun error event has been detected
     * |        |          |If RXOVIEN (USCI_BUFCTL[14]) is enabled, the corresponding interrupt request is activated
     * |        |          |It is cleared by software writes 1 to this bit.
     * |        |          |0 = A receive buffer overrun error event has not been detected.
     * |        |          |1 = A receive buffer overrun error event has been detected.
     * |[8]     |TXEMPTY   |Transmit Buffer Empty Indicator
     * |        |          |0 = Transmit buffer is not empty.
     * |        |          |1 = Transmit buffer is empty.
     * |[9]     |TXFULL    |Transmit Buffer Full Indicator
     * |        |          |0 = Transmit buffer is not full.
     * |        |          |1 = Transmit buffer is full.
     * @var UUART_T::PDMACTL
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
     * @var UUART_T::WKCTL
     * Offset: 0x54  USCI Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |Wake-up Enable Bit
     * |        |          |0 = Wake-up function Disabled.
     * |        |          |1 = Wake-up function Enabled.
     * |[2]     |PDBOPT    |Power Down Blocking Option
     * |        |          |0 = If user attempts to enter Power-down mode by executing WFI while the protocol is in transferring, MCU will stop the transfer and enter Power-down mode immediately.
     * |        |          |1 = If user attempts to enter Power-down mode by executing WFI while the protocol is in transferring, the on-going transfer will not be stopped and MCU will enter idle mode immediately.
     * @var UUART_T::WKSTS
     * Offset: 0x58  USCI Wake-up Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKF       |Wake-up Flag
     * |        |          |When chip is woken up from Power-down mode, this bit is set to 1
     * |        |          |Software can write 1 to clear this bit.
     * @var UUART_T::PROTCTL
     * Offset: 0x5C  USCI Protocol Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |STOPB     |Stop Bits
     * |        |          |This bit defines the number of stop bits in an UART frame.
     * |        |          |0 = The number of stop bits is 1.
     * |        |          |1 = The number of stop bits is 2.
     * |[1]     |PARITYEN  |Parity Enable Bit
     * |        |          |This bit defines the parity bit is enabled in an UART frame.
     * |        |          |0 = The parity bit Disabled.
     * |        |          |1 = The parity bit Enabled.
     * |[2]     |EVENPARITY|Even Parity Enable Bit
     * |        |          |0 = Odd number of logic 1's is transmitted and checked in each word.
     * |        |          |1 = Even number of logic 1's is transmitted and checked in each word.
     * |        |          |Note: This bit has effect only when PARITYEN is set.
     * |[3]     |RTSAUTOEN |nRTS Auto-flow Control Enable Bit
     * |        |          |When nRTS auto-flow is enabled, if the receiver buffer is full (RXFULL (USCI_BUFSTS[1] = 1'b1)), the UART will de-assert nRTS signal.
     * |        |          |0 = nRTS auto-flow control Disabled.
     * |        |          |1 = nRTS auto-flow control Enabled.
     * |        |          |Note: This bit has effect only when the RTSAUDIREN is not set.
     * |[4]     |CTSAUTOEN |nCTS Auto-flow Control Enable Bit
     * |        |          |When nCTS auto-flow is enabled, the UART will send data to external device when nCTS input assert (UART will not send data to device if nCTS input is dis-asserted).
     * |        |          |0 = nCTS auto-flow control Disabled.
     * |        |          |1 = nCTS auto-flow control Enabled.
     * |[5]     |RTSAUDIREN|nRTS Auto Direction Enable Bit
     * |        |          |When nRTS auto direction is enabled, if the transmitted bytes in the TX buffer is empty, the UART asserted nRTS signal automatically.
     * |        |          |0 = nRTS auto direction control Disabled.
     * |        |          |1 = nRTS auto direction control Enabled.
     * |        |          |Note 1: This bit is used for nRTS auto direction control for RS485.
     * |        |          |Note 2: This bit has effect only when the RTSAUTOEN is not set.
     * |[6]     |ABREN     |Auto-baud Rate Detect Enable Bit
     * |        |          |0 = Auto-baud rate detect function Disabled.
     * |        |          |1 = Auto-baud rate detect function Enabled.
     * |        |          |Note: When the auto - baud rate detect operation finishes, hardware will clear this bit
     * |        |          |The associated interrupt ABRDETIF (USCI_PROTST[9]) will be generated (If ARBIEN (USCI_PROTIEN [1]) is enabled).
     * |[9]     |DATWKEN   |Data Wake-up Mode Enable Bit
     * |        |          |0 = Data wake-up mode Disabled.
     * |        |          |1 = Data wake-up mode Enabled.
     * |[10]    |CTSWKEN   |nCTS Wake-up Mode Enable Bit
     * |        |          |0 = nCTS wake-up mode Disabled.
     * |        |          |1 = nCTS wake-up mode Enabled.
     * |[14:11] |WAKECNT   |Wake-up Counter
     * |        |          |These bits field indicate how many clock cycle selected by fPDS_CNT do the slave can get the 1st bit (start bit) when the device is wake-up from Power-down mode.
     * |[24:16] |BRDETITV  |Baud Rate Detection Interval
     * |        |          |This bit fields indicate how many clock cycle selected by TMCNTSRC (USCI_BRGEN [5]) does the slave calculates the baud rate in one bits
     * |        |          |The order of the bus shall be 1 and 0 step by step (e.g. the input data pattern shall be 0x55)
     * |        |          |The user can read the value to know the current input baud rate of the bus whenever the ABRDETIF (USCI_PROTCTL[9]) is set.
     * |        |          |Note: This bit can be cleared to 0 by software writing '0' to the BRDETITV.
     * |[26]    |STICKEN   |Stick Parity Enable Bit
     * |        |          |0 = Stick parity Disabled.
     * |        |          |1 = Stick parity Enabled.
     * |        |          |Note: Refer to RS-485 Support section for detail information.
     * |[29]    |BCEN      |Transmit Break Control Enable Bit
     * |        |          |0 = Transmit Break Control Disabled.
     * |        |          |1 = Transmit Break Control Enabled.
     * |        |          |Note: When this bit is set to logic 1, the serial data output (TX) is forced to the Spacing State (logic 0)
     * |        |          |This bit acts only on TX line and has no effect on the transmitter logic.
     * |[31]    |PROTEN    |UART Protocol Enable Bit
     * |        |          |0 = UART Protocol Disabled.
     * |        |          |1 = UART Protocol Enabled.
     * @var UUART_T::PROTIEN
     * Offset: 0x60  USCI Protocol Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |ABRIEN    |Auto-baud Rate Interrupt Enable Bit
     * |        |          |0 = Auto-baud rate interrupt Disabled.
     * |        |          |1 = Auto-baud rate interrupt Enabled.
     * |[2]     |RLSIEN    |Receive Line Status Interrupt Enable Bit
     * |        |          |0 = Receive line status interrupt Disabled.
     * |        |          |1 = Receive line status interrupt Enabled.
     * |        |          |Note: USCI_PROTSTS[7:5] indicates the current interrupt event for receive line status interrupt.
     * @var UUART_T::PROTSTS
     * Offset: 0x64  USCI Protocol Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TXSTIF    |Transmit Start Interrupt Flag
     * |        |          |0 = A transmit start interrupt status has not occurred.
     * |        |          |1 = A transmit start interrupt status has occurred.
     * |        |          |Note 1: It is cleared by software writing one into this bit.
     * |        |          |Note 2: Used for user to load next transmit data when there is no data in transmit buffer.
     * |[2]     |TXENDIF   |Transmit End Interrupt Flag
     * |        |          |0 = A transmit end interrupt status has not occurred.
     * |        |          |1 = A transmit end interrupt status has occurred.
     * |        |          |Note: It is cleared by software writing one into this bit.
     * |[3]     |RXSTIF    |Receive Start Interrupt Flag
     * |        |          |0 = A receive start interrupt status has not occurred.
     * |        |          |1 = A receive start interrupt status has occurred.
     * |        |          |Note: It is cleared by software writing one into this bit.
     * |[4]     |RXENDIF   |Receive End Interrupt Flag
     * |        |          |0 = A receive finish interrupt status has not occurred.
     * |        |          |1 = A receive finish interrupt status has occurred.
     * |        |          |Note: It is cleared by software writing one into this bit.
     * |[5]     |PARITYERR |Parity Error Flag
     * |        |          |This bit is set to logic 1 whenever the received character does not have a valid 'parity bit'.
     * |        |          |0 = No parity error is generated.
     * |        |          |1 = Parity error is generated.
     * |        |          |Note: This bit can be cleared by write '1' among the BREAK, FRMERR and PARITYERR bits.
     * |[6]     |FRMERR    |Framing Error Flag
     * |        |          |This bit is set to logic 1 whenever the received character does not have a valid 'stop bit'
     * |        |          |(that is, the stop bit following the last data bit or parity bit is detected as logic 0).
     * |        |          |0 = No framing error is generated.
     * |        |          |1 = Framing error is generated.
     * |        |          |Note: This bit can be cleared by write '1' among the BREAK, FRMERR and PARITYERR bits.
     * |[7]     |BREAK     |Break Flag
     * |        |          |This bit is set to logic 1 whenever the received data input (RX) is held in the 'spacing state'
     * |        |          |(logic 0) for longer than a full word transmission time (that is, the total time of  start bit + data bits + parity + stop bits).
     * |        |          |0 = No Break is generated.
     * |        |          |1 = Break is generated in the receiver bus.
     * |        |          |Note: This bit can be cleared by write '1' among the BREAK, FRMERR and PARITYERR bits.
     * |[9]     |ABRDETIF  |Auto-baud Rate Interrupt Flag
     * |        |          |This bit is set when auto-baud rate detection is done among the falling edge of the input data
     * |        |          |If the ABRIEN (USCI_PROTCTL[6]) is set, the auto-baud rate interrupt will be generated
     * |        |          |This bit can be set 4 times when the input data pattern is 0x55 and it is cleared before the next falling edge of the input bus.
     * |        |          |0 = Auto-baud rate detect function is not done.
     * |        |          |1 = One Bit auto-baud rate detect function is done.
     * |        |          |Note: This bit can be cleared by writing '1' to it.
     * |[10]    |RXBUSY    |RX Bus Status Flag (Read Only)
     * |        |          |This bit indicates the busy status of the receiver.
     * |        |          |0 = The receiver is Idle.
     * |        |          |1 = The receiver is BUSY.
     * |[11]    |ABERRSTS  |Auto-baud Rate Error Status
     * |        |          |This bit is set when auto-baud rate detection counter overrun
     * |        |          |When the auto-baud rate counter overrun, the user shall revise the CLKDIV (USCI_BRGEN[25:16]) value and
     * |        |          |enable ABREN (USCI_PROTCTL[6]) to detect the correct baud rate again.
     * |        |          |0 = Auto-baud rate detect counter is not overrun.
     * |        |          |1 = Auto-baud rate detect counter is overrun.
     * |        |          |Note 1: This bit is set at the same time of ABRDETIF.
     * |        |          |Note 2: This bit can be cleared by writing '1' to ABRDETIF or ABERRSTS.
     * |[16]    |CTSSYNCLV |nCTS Synchronized Level Status (Read Only)
     * |        |          |This bit used to indicate the current status of the internal synchronized nCTS signal.
     * |        |          |0 = The internal synchronized nCTS is low.
     * |        |          |1 = The internal synchronized nCTS is high.
     * |[17]    |CTSLV     |nCTS Pin Status (Read Only)
     * |        |          |This bit used to monitor the current status of nCTS pin input.
     * |        |          |0 = nCTS pin input is low level voltage logic state.
     * |        |          |1 = nCTS pin input is high level voltage logic state.
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
    __IO uint32_t TXDAT;                 /*!< [0x0030] USCI Transmit Data Register                                      */
    __IO uint32_t RXDAT;                 /*!< [0x0034] USCI Receive Data Register                                       */
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

} UUART_T;

/**
    @addtogroup UUART_CONST UUART Bit Field Definition
    Constant Definitions for UUART Controller
@{ */

#define UUART_CTL_FUNMODE_Pos            (0)                                               /*!< UUART_T::CTL: FUNMODE Position         */
#define UUART_CTL_FUNMODE_Msk            (0x7ul << UUART_CTL_FUNMODE_Pos)                  /*!< UUART_T::CTL: FUNMODE Mask             */

#define UUART_INTEN_TXSTIEN_Pos          (1)                                               /*!< UUART_T::INTEN: TXSTIEN Position       */
#define UUART_INTEN_TXSTIEN_Msk          (0x1ul << UUART_INTEN_TXSTIEN_Pos)                /*!< UUART_T::INTEN: TXSTIEN Mask           */

#define UUART_INTEN_TXENDIEN_Pos         (2)                                               /*!< UUART_T::INTEN: TXENDIEN Position      */
#define UUART_INTEN_TXENDIEN_Msk         (0x1ul << UUART_INTEN_TXENDIEN_Pos)               /*!< UUART_T::INTEN: TXENDIEN Mask          */

#define UUART_INTEN_RXSTIEN_Pos          (3)                                               /*!< UUART_T::INTEN: RXSTIEN Position       */
#define UUART_INTEN_RXSTIEN_Msk          (0x1ul << UUART_INTEN_RXSTIEN_Pos)                /*!< UUART_T::INTEN: RXSTIEN Mask           */

#define UUART_INTEN_RXENDIEN_Pos         (4)                                               /*!< UUART_T::INTEN: RXENDIEN Position      */
#define UUART_INTEN_RXENDIEN_Msk         (0x1ul << UUART_INTEN_RXENDIEN_Pos)               /*!< UUART_T::INTEN: RXENDIEN Mask          */

#define UUART_BRGEN_RCLKSEL_Pos          (0)                                               /*!< UUART_T::BRGEN: RCLKSEL Position       */
#define UUART_BRGEN_RCLKSEL_Msk          (0x1ul << UUART_BRGEN_RCLKSEL_Pos)                /*!< UUART_T::BRGEN: RCLKSEL Mask           */

#define UUART_BRGEN_PTCLKSEL_Pos         (1)                                               /*!< UUART_T::BRGEN: PTCLKSEL Position      */
#define UUART_BRGEN_PTCLKSEL_Msk         (0x1ul << UUART_BRGEN_PTCLKSEL_Pos)               /*!< UUART_T::BRGEN: PTCLKSEL Mask          */

#define UUART_BRGEN_SPCLKSEL_Pos         (2)                                               /*!< UUART_T::BRGEN: SPCLKSEL Position      */
#define UUART_BRGEN_SPCLKSEL_Msk         (0x3ul << UUART_BRGEN_SPCLKSEL_Pos)               /*!< UUART_T::BRGEN: SPCLKSEL Mask          */

#define UUART_BRGEN_TMCNTEN_Pos          (4)                                               /*!< UUART_T::BRGEN: TMCNTEN Position       */
#define UUART_BRGEN_TMCNTEN_Msk          (0x1ul << UUART_BRGEN_TMCNTEN_Pos)                /*!< UUART_T::BRGEN: TMCNTEN Mask           */

#define UUART_BRGEN_TMCNTSRC_Pos         (5)                                               /*!< UUART_T::BRGEN: TMCNTSRC Position      */
#define UUART_BRGEN_TMCNTSRC_Msk         (0x1ul << UUART_BRGEN_TMCNTSRC_Pos)               /*!< UUART_T::BRGEN: TMCNTSRC Mask          */

#define UUART_BRGEN_PDSCNT_Pos           (8)                                               /*!< UUART_T::BRGEN: PDSCNT Position        */
#define UUART_BRGEN_PDSCNT_Msk           (0x3ul << UUART_BRGEN_PDSCNT_Pos)                 /*!< UUART_T::BRGEN: PDSCNT Mask            */

#define UUART_BRGEN_DSCNT_Pos            (10)                                              /*!< UUART_T::BRGEN: DSCNT Position         */
#define UUART_BRGEN_DSCNT_Msk            (0x1ful << UUART_BRGEN_DSCNT_Pos)                 /*!< UUART_T::BRGEN: DSCNT Mask             */

#define UUART_BRGEN_CLKDIV_Pos           (16)                                              /*!< UUART_T::BRGEN: CLKDIV Position        */
#define UUART_BRGEN_CLKDIV_Msk           (0x3fful << UUART_BRGEN_CLKDIV_Pos)               /*!< UUART_T::BRGEN: CLKDIV Mask            */

#define UUART_DATIN0_SYNCSEL_Pos         (0)                                               /*!< UUART_T::DATIN0: SYNCSEL Position      */
#define UUART_DATIN0_SYNCSEL_Msk         (0x1ul << UUART_DATIN0_SYNCSEL_Pos)               /*!< UUART_T::DATIN0: SYNCSEL Mask          */

#define UUART_DATIN0_ININV_Pos           (2)                                               /*!< UUART_T::DATIN0: ININV Position        */
#define UUART_DATIN0_ININV_Msk           (0x1ul << UUART_DATIN0_ININV_Pos)                 /*!< UUART_T::DATIN0: ININV Mask            */

#define UUART_DATIN0_EDGEDET_Pos         (3)                                               /*!< UUART_T::DATIN0: EDGEDET Position      */
#define UUART_DATIN0_EDGEDET_Msk         (0x3ul << UUART_DATIN0_EDGEDET_Pos)               /*!< UUART_T::DATIN0: EDGEDET Mask          */

#define UUART_CTLIN0_SYNCSEL_Pos         (0)                                               /*!< UUART_T::CTLIN0: SYNCSEL Position      */
#define UUART_CTLIN0_SYNCSEL_Msk         (0x1ul << UUART_CTLIN0_SYNCSEL_Pos)               /*!< UUART_T::CTLIN0: SYNCSEL Mask          */

#define UUART_CTLIN0_ININV_Pos           (2)                                               /*!< UUART_T::CTLIN0: ININV Position        */
#define UUART_CTLIN0_ININV_Msk           (0x1ul << UUART_CTLIN0_ININV_Pos)                 /*!< UUART_T::CTLIN0: ININV Mask            */

#define UUART_CLKIN_SYNCSEL_Pos          (0)                                               /*!< UUART_T::CLKIN: SYNCSEL Position       */
#define UUART_CLKIN_SYNCSEL_Msk          (0x1ul << UUART_CLKIN_SYNCSEL_Pos)                /*!< UUART_T::CLKIN: SYNCSEL Mask           */

#define UUART_LINECTL_LSB_Pos            (0)                                               /*!< UUART_T::LINECTL: LSB Position         */
#define UUART_LINECTL_LSB_Msk            (0x1ul << UUART_LINECTL_LSB_Pos)                  /*!< UUART_T::LINECTL: LSB Mask             */

#define UUART_LINECTL_DATOINV_Pos        (5)                                               /*!< UUART_T::LINECTL: DATOINV Position     */
#define UUART_LINECTL_DATOINV_Msk        (0x1ul << UUART_LINECTL_DATOINV_Pos)              /*!< UUART_T::LINECTL: DATOINV Mask         */

#define UUART_LINECTL_CTLOINV_Pos        (7)                                               /*!< UUART_T::LINECTL: CTLOINV Position     */
#define UUART_LINECTL_CTLOINV_Msk        (0x1ul << UUART_LINECTL_CTLOINV_Pos)              /*!< UUART_T::LINECTL: CTLOINV Mask         */

#define UUART_LINECTL_DWIDTH_Pos         (8)                                               /*!< UUART_T::LINECTL: DWIDTH Position      */
#define UUART_LINECTL_DWIDTH_Msk         (0xful << UUART_LINECTL_DWIDTH_Pos)               /*!< UUART_T::LINECTL: DWIDTH Mask          */

#define UUART_TXDAT_TXDAT_Pos            (0)                                               /*!< UUART_T::TXDAT: TXDAT Position         */
#define UUART_TXDAT_TXDAT_Msk            (0xfffful << UUART_TXDAT_TXDAT_Pos)               /*!< UUART_T::TXDAT: TXDAT Mask             */

#define UUART_RXDAT_RXDAT_Pos            (0)                                               /*!< UUART_T::RXDAT: RXDAT Position         */
#define UUART_RXDAT_RXDAT_Msk            (0xfffful << UUART_RXDAT_RXDAT_Pos)               /*!< UUART_T::RXDAT: RXDAT Mask             */

#define UUART_BUFCTL_TXCLR_Pos           (7)                                               /*!< UUART_T::BUFCTL: TXCLR Position        */
#define UUART_BUFCTL_TXCLR_Msk           (0x1ul << UUART_BUFCTL_TXCLR_Pos)                 /*!< UUART_T::BUFCTL: TXCLR Mask            */

#define UUART_BUFCTL_RXOVIEN_Pos         (14)                                              /*!< UUART_T::BUFCTL: RXOVIEN Position      */
#define UUART_BUFCTL_RXOVIEN_Msk         (0x1ul << UUART_BUFCTL_RXOVIEN_Pos)               /*!< UUART_T::BUFCTL: RXOVIEN Mask          */

#define UUART_BUFCTL_RXCLR_Pos           (15)                                              /*!< UUART_T::BUFCTL: RXCLR Position        */
#define UUART_BUFCTL_RXCLR_Msk           (0x1ul << UUART_BUFCTL_RXCLR_Pos)                 /*!< UUART_T::BUFCTL: RXCLR Mask            */

#define UUART_BUFCTL_TXRST_Pos           (16)                                              /*!< UUART_T::BUFCTL: TXRST Position        */
#define UUART_BUFCTL_TXRST_Msk           (0x1ul << UUART_BUFCTL_TXRST_Pos)                 /*!< UUART_T::BUFCTL: TXRST Mask            */

#define UUART_BUFCTL_RXRST_Pos           (17)                                              /*!< UUART_T::BUFCTL: RXRST Position        */
#define UUART_BUFCTL_RXRST_Msk           (0x1ul << UUART_BUFCTL_RXRST_Pos)                 /*!< UUART_T::BUFCTL: RXRST Mask            */

#define UUART_BUFSTS_RXEMPTY_Pos         (0)                                               /*!< UUART_T::BUFSTS: RXEMPTY Position      */
#define UUART_BUFSTS_RXEMPTY_Msk         (0x1ul << UUART_BUFSTS_RXEMPTY_Pos)               /*!< UUART_T::BUFSTS: RXEMPTY Mask          */

#define UUART_BUFSTS_RXFULL_Pos          (1)                                               /*!< UUART_T::BUFSTS: RXFULL Position       */
#define UUART_BUFSTS_RXFULL_Msk          (0x1ul << UUART_BUFSTS_RXFULL_Pos)                /*!< UUART_T::BUFSTS: RXFULL Mask           */

#define UUART_BUFSTS_RXOVIF_Pos          (3)                                               /*!< UUART_T::BUFSTS: RXOVIF Position       */
#define UUART_BUFSTS_RXOVIF_Msk          (0x1ul << UUART_BUFSTS_RXOVIF_Pos)                /*!< UUART_T::BUFSTS: RXOVIF Mask           */

#define UUART_BUFSTS_TXEMPTY_Pos         (8)                                               /*!< UUART_T::BUFSTS: TXEMPTY Position      */
#define UUART_BUFSTS_TXEMPTY_Msk         (0x1ul << UUART_BUFSTS_TXEMPTY_Pos)               /*!< UUART_T::BUFSTS: TXEMPTY Mask          */

#define UUART_BUFSTS_TXFULL_Pos          (9)                                               /*!< UUART_T::BUFSTS: TXFULL Position       */
#define UUART_BUFSTS_TXFULL_Msk          (0x1ul << UUART_BUFSTS_TXFULL_Pos)                /*!< UUART_T::BUFSTS: TXFULL Mask           */

#define UUART_PDMACTL_PDMARST_Pos        (0)                                               /*!< UUART_T::PDMACTL: PDMARST Position     */
#define UUART_PDMACTL_PDMARST_Msk        (0x1ul << UUART_PDMACTL_PDMARST_Pos)              /*!< UUART_T::PDMACTL: PDMARST Mask         */

#define UUART_PDMACTL_TXPDMAEN_Pos       (1)                                               /*!< UUART_T::PDMACTL: TXPDMAEN Position    */
#define UUART_PDMACTL_TXPDMAEN_Msk       (0x1ul << UUART_PDMACTL_TXPDMAEN_Pos)             /*!< UUART_T::PDMACTL: TXPDMAEN Mask        */

#define UUART_PDMACTL_RXPDMAEN_Pos       (2)                                               /*!< UUART_T::PDMACTL: RXPDMAEN Position    */
#define UUART_PDMACTL_RXPDMAEN_Msk       (0x1ul << UUART_PDMACTL_RXPDMAEN_Pos)             /*!< UUART_T::PDMACTL: RXPDMAEN Mask        */

#define UUART_PDMACTL_PDMAEN_Pos         (3)                                               /*!< UUART_T::PDMACTL: PDMAEN Position      */
#define UUART_PDMACTL_PDMAEN_Msk         (0x1ul << UUART_PDMACTL_PDMAEN_Pos)               /*!< UUART_T::PDMACTL: PDMAEN Mask          */

#define UUART_WKCTL_WKEN_Pos             (0)                                               /*!< UUART_T::WKCTL: WKEN Position          */
#define UUART_WKCTL_WKEN_Msk             (0x1ul << UUART_WKCTL_WKEN_Pos)                   /*!< UUART_T::WKCTL: WKEN Mask              */

#define UUART_WKCTL_PDBOPT_Pos           (2)                                               /*!< UUART_T::WKCTL: PDBOPT Position        */
#define UUART_WKCTL_PDBOPT_Msk           (0x1ul << UUART_WKCTL_PDBOPT_Pos)                 /*!< UUART_T::WKCTL: PDBOPT Mask            */

#define UUART_WKSTS_WKF_Pos              (0)                                               /*!< UUART_T::WKSTS: WKF Position           */
#define UUART_WKSTS_WKF_Msk              (0x1ul << UUART_WKSTS_WKF_Pos)                    /*!< UUART_T::WKSTS: WKF Mask               */

#define UUART_PROTCTL_STOPB_Pos          (0)                                               /*!< UUART_T::PROTCTL: STOPB Position       */
#define UUART_PROTCTL_STOPB_Msk          (0x1ul << UUART_PROTCTL_STOPB_Pos)                /*!< UUART_T::PROTCTL: STOPB Mask           */

#define UUART_PROTCTL_PARITYEN_Pos       (1)                                               /*!< UUART_T::PROTCTL: PARITYEN Position    */
#define UUART_PROTCTL_PARITYEN_Msk       (0x1ul << UUART_PROTCTL_PARITYEN_Pos)             /*!< UUART_T::PROTCTL: PARITYEN Mask        */

#define UUART_PROTCTL_EVENPARITY_Pos     (2)                                               /*!< UUART_T::PROTCTL: EVENPARITY Position  */
#define UUART_PROTCTL_EVENPARITY_Msk     (0x1ul << UUART_PROTCTL_EVENPARITY_Pos)           /*!< UUART_T::PROTCTL: EVENPARITY Mask      */

#define UUART_PROTCTL_RTSAUTOEN_Pos      (3)                                               /*!< UUART_T::PROTCTL: RTSAUTOEN Position   */
#define UUART_PROTCTL_RTSAUTOEN_Msk      (0x1ul << UUART_PROTCTL_RTSAUTOEN_Pos)            /*!< UUART_T::PROTCTL: RTSAUTOEN Mask       */

#define UUART_PROTCTL_CTSAUTOEN_Pos      (4)                                               /*!< UUART_T::PROTCTL: CTSAUTOEN Position   */
#define UUART_PROTCTL_CTSAUTOEN_Msk      (0x1ul << UUART_PROTCTL_CTSAUTOEN_Pos)            /*!< UUART_T::PROTCTL: CTSAUTOEN Mask       */

#define UUART_PROTCTL_RTSAUDIREN_Pos     (5)                                               /*!< UUART_T::PROTCTL: RTSAUDIREN Position  */
#define UUART_PROTCTL_RTSAUDIREN_Msk     (0x1ul << UUART_PROTCTL_RTSAUDIREN_Pos)           /*!< UUART_T::PROTCTL: RTSAUDIREN Mask      */

#define UUART_PROTCTL_ABREN_Pos          (6)                                               /*!< UUART_T::PROTCTL: ABREN Position       */
#define UUART_PROTCTL_ABREN_Msk          (0x1ul << UUART_PROTCTL_ABREN_Pos)                /*!< UUART_T::PROTCTL: ABREN Mask           */

#define UUART_PROTCTL_DATWKEN_Pos        (9)                                               /*!< UUART_T::PROTCTL: DATWKEN Position     */
#define UUART_PROTCTL_DATWKEN_Msk        (0x1ul << UUART_PROTCTL_DATWKEN_Pos)              /*!< UUART_T::PROTCTL: DATWKEN Mask         */

#define UUART_PROTCTL_CTSWKEN_Pos        (10)                                              /*!< UUART_T::PROTCTL: CTSWKEN Position     */
#define UUART_PROTCTL_CTSWKEN_Msk        (0x1ul << UUART_PROTCTL_CTSWKEN_Pos)              /*!< UUART_T::PROTCTL: CTSWKEN Mask         */

#define UUART_PROTCTL_WAKECNT_Pos        (11)                                              /*!< UUART_T::PROTCTL: WAKECNT Position     */
#define UUART_PROTCTL_WAKECNT_Msk        (0xful << UUART_PROTCTL_WAKECNT_Pos)              /*!< UUART_T::PROTCTL: WAKECNT Mask         */

#define UUART_PROTCTL_BRDETITV_Pos       (16)                                              /*!< UUART_T::PROTCTL: BRDETITV Position    */
#define UUART_PROTCTL_BRDETITV_Msk       (0x1fful << UUART_PROTCTL_BRDETITV_Pos)           /*!< UUART_T::PROTCTL: BRDETITV Mask        */

#define UUART_PROTCTL_STICKEN_Pos        (26)                                              /*!< UUART_T::PROTCTL: STICKEN Position     */
#define UUART_PROTCTL_STICKEN_Msk        (0x1ul << UUART_PROTCTL_STICKEN_Pos)              /*!< UUART_T::PROTCTL: STICKEN Mask         */

#define UUART_PROTCTL_BCEN_Pos           (29)                                              /*!< UUART_T::PROTCTL: BCEN Position        */
#define UUART_PROTCTL_BCEN_Msk           (0x1ul << UUART_PROTCTL_BCEN_Pos)                 /*!< UUART_T::PROTCTL: BCEN Mask            */

#define UUART_PROTCTL_PROTEN_Pos         (31)                                              /*!< UUART_T::PROTCTL: PROTEN Position      */
#define UUART_PROTCTL_PROTEN_Msk         (0x1ul << UUART_PROTCTL_PROTEN_Pos)               /*!< UUART_T::PROTCTL: PROTEN Mask          */

#define UUART_PROTIEN_ABRIEN_Pos         (1)                                               /*!< UUART_T::PROTIEN: ABRIEN Position      */
#define UUART_PROTIEN_ABRIEN_Msk         (0x1ul << UUART_PROTIEN_ABRIEN_Pos)               /*!< UUART_T::PROTIEN: ABRIEN Mask          */

#define UUART_PROTIEN_RLSIEN_Pos         (2)                                               /*!< UUART_T::PROTIEN: RLSIEN Position      */
#define UUART_PROTIEN_RLSIEN_Msk         (0x1ul << UUART_PROTIEN_RLSIEN_Pos)               /*!< UUART_T::PROTIEN: RLSIEN Mask          */

#define UUART_PROTSTS_TXSTIF_Pos         (1)                                               /*!< UUART_T::PROTSTS: TXSTIF Position      */
#define UUART_PROTSTS_TXSTIF_Msk         (0x1ul << UUART_PROTSTS_TXSTIF_Pos)               /*!< UUART_T::PROTSTS: TXSTIF Mask          */

#define UUART_PROTSTS_TXENDIF_Pos        (2)                                               /*!< UUART_T::PROTSTS: TXENDIF Position     */
#define UUART_PROTSTS_TXENDIF_Msk        (0x1ul << UUART_PROTSTS_TXENDIF_Pos)              /*!< UUART_T::PROTSTS: TXENDIF Mask         */

#define UUART_PROTSTS_RXSTIF_Pos         (3)                                               /*!< UUART_T::PROTSTS: RXSTIF Position      */
#define UUART_PROTSTS_RXSTIF_Msk         (0x1ul << UUART_PROTSTS_RXSTIF_Pos)               /*!< UUART_T::PROTSTS: RXSTIF Mask          */

#define UUART_PROTSTS_RXENDIF_Pos        (4)                                               /*!< UUART_T::PROTSTS: RXENDIF Position     */
#define UUART_PROTSTS_RXENDIF_Msk        (0x1ul << UUART_PROTSTS_RXENDIF_Pos)              /*!< UUART_T::PROTSTS: RXENDIF Mask         */

#define UUART_PROTSTS_PARITYERR_Pos      (5)                                               /*!< UUART_T::PROTSTS: PARITYERR Position   */
#define UUART_PROTSTS_PARITYERR_Msk      (0x1ul << UUART_PROTSTS_PARITYERR_Pos)            /*!< UUART_T::PROTSTS: PARITYERR Mask       */

#define UUART_PROTSTS_FRMERR_Pos         (6)                                               /*!< UUART_T::PROTSTS: FRMERR Position      */
#define UUART_PROTSTS_FRMERR_Msk         (0x1ul << UUART_PROTSTS_FRMERR_Pos)               /*!< UUART_T::PROTSTS: FRMERR Mask          */

#define UUART_PROTSTS_BREAK_Pos          (7)                                               /*!< UUART_T::PROTSTS: BREAK Position       */
#define UUART_PROTSTS_BREAK_Msk          (0x1ul << UUART_PROTSTS_BREAK_Pos)                /*!< UUART_T::PROTSTS: BREAK Mask           */

#define UUART_PROTSTS_ABRDETIF_Pos       (9)                                               /*!< UUART_T::PROTSTS: ABRDETIF Position    */
#define UUART_PROTSTS_ABRDETIF_Msk       (0x1ul << UUART_PROTSTS_ABRDETIF_Pos)             /*!< UUART_T::PROTSTS: ABRDETIF Mask        */

#define UUART_PROTSTS_RXBUSY_Pos         (10)                                              /*!< UUART_T::PROTSTS: RXBUSY Position      */
#define UUART_PROTSTS_RXBUSY_Msk         (0x1ul << UUART_PROTSTS_RXBUSY_Pos)               /*!< UUART_T::PROTSTS: RXBUSY Mask          */

#define UUART_PROTSTS_ABERRSTS_Pos       (11)                                              /*!< UUART_T::PROTSTS: ABERRSTS Position    */
#define UUART_PROTSTS_ABERRSTS_Msk       (0x1ul << UUART_PROTSTS_ABERRSTS_Pos)             /*!< UUART_T::PROTSTS: ABERRSTS Mask        */

#define UUART_PROTSTS_CTSSYNCLV_Pos      (16)                                              /*!< UUART_T::PROTSTS: CTSSYNCLV Position   */
#define UUART_PROTSTS_CTSSYNCLV_Msk      (0x1ul << UUART_PROTSTS_CTSSYNCLV_Pos)            /*!< UUART_T::PROTSTS: CTSSYNCLV Mask       */

#define UUART_PROTSTS_CTSLV_Pos          (17)                                              /*!< UUART_T::PROTSTS: CTSLV Position       */
#define UUART_PROTSTS_CTSLV_Msk          (0x1ul << UUART_PROTSTS_CTSLV_Pos)                /*!< UUART_T::PROTSTS: CTSLV Mask           */

/**@}*/ /* UUART_CONST */
/**@}*/ /* end of UUART register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __UUART_REG_H__ */
