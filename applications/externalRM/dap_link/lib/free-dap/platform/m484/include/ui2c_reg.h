/**************************************************************************//**
 * @file     ui2c_reg.h
 * @version  V1.00
 * @brief    UI2C register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __UI2C_REG_H__
#define __UI2C_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup UI2C I2C Mode of USCI Controller(UI2C)
    Memory Mapped Structure for UI2C Controller
@{ */

typedef struct
{


    /**
     * @var UI2C_T::CTL
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
     * @var UI2C_T::BRGEN
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
     * |[4]     |TMCNTEN   |Time Measurement Counter Enable Bit
     * |        |          |This bit enables the 10-bit timing measurement counter.
     * |        |          |0 = Time measurement counter is Disabled.
     * |        |          |1 = Time measurement counter is Enabled.
     * |[5]     |TMCNTSRC  |Time Measurement Counter Clock Source Selection
     * |        |          |0 = Time measurement counter with fPROT_CLK.
     * |        |          |1 = Time measurement counter with fDIV_CLK.
     * |[9:8]   |PDSCNT    |Pre-divider for Sample Counter
     * |        |          |This bit field defines the divide ratio of the clock division from sample clock fSAMP_CLK
     * |        |          |The divided frequency fPDS_CNT = fSAMP_CLK / (PDSCNT+1).
     * |[14:10] |DSCNT     |Denominator for Sample Counter
     * |        |          |This bit field defines the divide ratio of the sample clock fSAMP_CLK.
     * |        |          |The divided frequency fDS_CNT = fPDS_CNT / (DSCNT+1).
     * |        |          |Note: The maximum value of DSCNT is 0xF on UART mode and suggest to set over 4 to confirm the receiver data is sampled in right value
     * |[25:16] |CLKDIV    |Clock Divider
     * |        |          |This bit field defines the ratio between the protocol clock frequency fPROT_CLK and the clock divider frequency fDIV_CLK (fDIV_CLK = fPROT_CLK / (CLKDIV+1) ).
     * |        |          |Note: In UART function, it can be updated by hardware in the 4th falling edge of the input data 0x55 when the auto baud rate function (ABREN(USCI_PROTCTL[6])) is enabled
     * |        |          |The revised value is the average bit time between bit 5 and bit 6
     * |        |          |The user can use revised CLKDIV and new BRDETITV (USCI_PROTCTL[24:16]) to calculate the precise baud rate.
     * @var UI2C_T::LINECTL
     * Offset: 0x2C  USCI Line Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LSB       |LSB First Transmission Selection
     * |        |          |0 = The MSB, which bit of transmit/receive data buffer depends on the setting of DWIDTH, is transmitted/received first.
     * |        |          |1 = The LSB, the bit 0 of data buffer, will be transmitted/received first.
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
     * |        |          |Note: In UART protocol, the length can be configured as 6~13 bits
     * |        |          |And in I2C protocol, the length fixed as 8 bits.
     * @var UI2C_T::TXDAT
     * Offset: 0x30  USCI Transmit Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TXDAT     |Transmit Data
     * |        |          |Software can use this bit field to write 16-bit transmit data for transmission.
     * @var UI2C_T::RXDAT
     * Offset: 0x34  USCI Receive Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RXDAT     |Received Data
     * |        |          |This bit field monitors the received data which stored in receive data buffer.
     * |        |          |Note 1: In I2C protocol, RXDAT[12:8] indicate the different transmission conditions which defined in I2C.
     * |        |          |Note 2: In UART protocol, RXDAT[15:13] indicate the same frame status of BREAK, FRMERR and PARITYERR (USCI_PROTSTS[7:5]).
     * @var UI2C_T::DEVADDR0
     * Offset: 0x44  USCI Device Address Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |DEVADDR   |Device Address
     * |        |          |In I2C protocol, this bit field contains the programmed slave address
     * |        |          |If the first received address byte are 1111 0AAXB, the AA bits are compared to the bits DEVADDR[9:8] to check for address match, where the X is R/W bit
     * |        |          |Then the second address byte is also compared to DEVADDR[7:0].
     * |        |          |Note 1: The DEVADDR [9:7] must be set 3'b000 when I2C operating in 7-bit address mode.
     * |        |          |Note 2: When software set 10'h000, the address can not be used.
     * @var UI2C_T::DEVADDR1
     * Offset: 0x48  USCI Device Address Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |DEVADDR   |Device Address
     * |        |          |In I2C protocol, this bit field contains the programmed slave address
     * |        |          |If the first received address byte are 1111 0AAXB, the AA bits are compared to the bits DEVADDR[9:8] to check for address match, where the X is R/W bit
     * |        |          |Then the second address byte is also compared to DEVADDR[7:0].
     * |        |          |Note 1: The DEVADDR [9:7] must be set 3'000 when I2C operating in 7-bit address mode.
     * |        |          |Note 2: When software set 10'h000, the address can not be used.
     * @var UI2C_T::ADDRMSK0
     * Offset: 0x4C  USCI Device Address Mask Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |ADDRMSK   |USCI Device Address Mask
     * |        |          |0 = Mask Disabled (the received corresponding register bit should be exact the same as address register.).
     * |        |          |1 = Mask Enabled (the received corresponding address bit is don't care.).
     * |        |          |USCI support multiple address recognition with two address mask register
     * |        |          |When the bit in the address mask register is set to one, it means the received corresponding address bit is don't-care
     * |        |          |If the bit is set to zero, that means the received corresponding register bit should be exact the same as address register.
     * |        |          |Note: The wake-up function can not use address mask.
     * @var UI2C_T::ADDRMSK1
     * Offset: 0x50  USCI Device Address Mask Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |ADDRMSK   |USCI Device Address Mask
     * |        |          |0 = Mask Disabled (the received corresponding register bit should be exact the same as address register.).
     * |        |          |1 = Mask Enabled (the received corresponding address bit is don't care.).
     * |        |          |USCI support multiple address recognition with two address mask register
     * |        |          |When the bit in the address mask register is set to one, it means the received corresponding address bit is don't-care
     * |        |          |If the bit is set to zero, that means the received corresponding register bit should be exact the same as address register.
     * |        |          |Note: The wake-up function can not use address mask.
     * @var UI2C_T::WKCTL
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
     * @var UI2C_T::WKSTS
     * Offset: 0x58  USCI Wake-up Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKF       |Wake-up Flag
     * |        |          |When chip is woken up from Power-down mode, this bit is set to 1
     * |        |          |Software can write 1 to clear this bit.
     * @var UI2C_T::PROTCTL
     * Offset: 0x5C  USCI Protocol Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GCFUNC    |General Call Function
     * |        |          |0 = General Call Function Disabled.
     * |        |          |1 = General Call Function Enabled.
     * |[1]     |AA        |Assert Acknowledge Control
     * |        |          |When AA=1 prior to address or data received, an acknowledged (low level to SDA) will be returned during the acknowledge clock pulse on the SCL line when 1.) A slave is acknowledging the address sent from master, 2.) The receiver devices are acknowledging the data sent by transmitter
     * |        |          |When AA=0 prior to address or data received, a Not acknowledged (high level to SDA) will be returned during the acknowledge clock pulse on the SCL line.
     * |[2]     |STO       |I2C STOP Control
     * |        |          |In Master mode, setting STO to transmit a STOP condition to bus then I2C hardware will check the bus condition if a STOP condition is detected this bit will be cleared by hardware automatically
     * |        |          |In a slave mode, setting STO resets I2C hardware to the defined "not addressed" slave mode when bus error (USCI_PROTSTS.ERRIF = 1).
     * |[3]     |STA       |I2C START Control
     * |        |          |Setting STA to logic 1 to enter Master mode, the I2C hardware sends a START or repeat START condition to bus when the bus is free.
     * |[4]     |ADDR10EN  |Address 10-bit Function Enable Bit
     * |        |          |0 = Address match 10 bit function is disabled.
     * |        |          |1 = Address match 10 bit function is enabled.
     * |[5]     |PTRG      |I2C Protocol Trigger (Write Only)
     * |        |          |When a new state is present in the USCI_PROTSTS register, if the related interrupt enable bits are set, the I2C interrupt is requested
     * |        |          |It must write one by software to this bit after the related interrupt flags are set to 1 and the I2C protocol function will go ahead until the STOP is active or the PROTEN is disabled.
     * |        |          |0 = I2C's stretch disabled and the I2C protocol function will go ahead.
     * |        |          |1 = I2C's stretch active.
     * |[8]     |SCLOUTEN  |SCL Output Enable Bit
     * |        |          |This bit enables monitor pulling SCL to low
     * |        |          |This monitor will pull SCL to low until it has had time to respond to an I2C interrupt.
     * |        |          |0 = SCL output will be forced high due to open drain mechanism.
     * |        |          |1 = I2C module may act as a slave peripheral just like in normal operation, the I2C holds the clock line low until it has had time to clear I2C interrupt.
     * |[9]     |MONEN     |Monitor Mode Enable Bit
     * |        |          |This bit enables monitor mode
     * |        |          |In monitor mode the SDA output will be put in high impedance mode
     * |        |          |This prevents the I2C module from outputting data of any kind (including ACK) onto the I2C data bus.
     * |        |          |0 = The monitor mode is disabled.
     * |        |          |1 = The monitor mode is enabled.
     * |        |          |Note: Depending on the state of the SCLOUTEN bit, the SCL output may be also forced high, preventing the module from having control over the I2C clock line.
     * |[25:16] |TOCNT     |Time-out Clock Cycle
     * |        |          |This bit field indicates how many clock cycle selected by TMCNTSRC (USCI_BRGEN [5]) when each interrupt flags are clear
     * |        |          |The time-out is enable when TOCNT bigger than 0.
     * |        |          |Note: The TMCNTSRC (USCI_BRGEN [5]) must be set zero on I2C mode.
     * |[31]    |PROTEN    |I2C Protocol Enable Bit
     * |        |          |0 = I2C Protocol disable.
     * |        |          |1 = I2C Protocol enable.
     * @var UI2C_T::PROTIEN
     * Offset: 0x60  USCI Protocol Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TOIEN     |Time-out Interrupt Enable Control
     * |        |          |In I2C protocol, this bit enables the interrupt generation in case of a time-out event.
     * |        |          |0 = The time-out interrupt is disabled.
     * |        |          |1 = The time-out interrupt is enabled.
     * |[1]     |STARIEN   |Start Condition Received Interrupt Enable Control
     * |        |          |This bit enables the generation of a protocol interrupt if a start condition is detected.
     * |        |          |0 = The start condition interrupt is disabled.
     * |        |          |1 = The start condition interrupt is enabled.
     * |[2]     |STORIEN   |Stop Condition Received Interrupt Enable Control
     * |        |          |This bit enables the generation of a protocol interrupt if a stop condition is detected.
     * |        |          |0 = The stop condition interrupt is disabled.
     * |        |          |1 = The stop condition interrupt is enabled.
     * |[3]     |NACKIEN   |Non - Acknowledge Interrupt Enable Control
     * |        |          |This bit enables the generation of a protocol interrupt if a non - acknowledge is detected by a master.
     * |        |          |0 = The non - acknowledge interrupt is disabled.
     * |        |          |1 = The non - acknowledge interrupt is enabled.
     * |[4]     |ARBLOIEN  |Arbitration Lost Interrupt Enable Control
     * |        |          |This bit enables the generation of a protocol interrupt if an arbitration lost event is detected.
     * |        |          |0 = The arbitration lost interrupt is disabled.
     * |        |          |1 = The arbitration lost interrupt is enabled.
     * |[5]     |ERRIEN    |Error Interrupt Enable Control
     * |        |          |This bit enables the generation of a protocol interrupt if an I2C error condition is detected (indicated by ERR (USCI_PROTSTS [16])).
     * |        |          |0 = The error interrupt is disabled.
     * |        |          |1 = The error interrupt is enabled.
     * |[6]     |ACKIEN    |Acknowledge Interrupt Enable Control
     * |        |          |This bit enables the generation of a protocol interrupt if an acknowledge is detected by a master.
     * |        |          |0 = The acknowledge interrupt is disabled.
     * |        |          |1 = The acknowledge interrupt is enabled.
     * @var UI2C_T::PROTSTS
     * Offset: 0x64  USCI Protocol Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[5]     |TOIF      |Time-out Interrupt Flag
     * |        |          |0 = A time-out interrupt status has not occurred.
     * |        |          |1 = A time-out interrupt status has occurred.
     * |        |          |Note: It is cleared by software writing one into this bit
     * |[6]     |ONBUSY    |On Bus Busy
     * |        |          |Indicates that a communication is in progress on the bus
     * |        |          |It is set by hardware when a START condition is detected
     * |        |          |It is cleared by hardware when a STOP condition is detected
     * |        |          |0 = The bus is IDLE (both SCLK and SDA High).
     * |        |          |1 = The bus is busy.
     * |[8]     |STARIF    |Start Condition Received Interrupt Flag
     * |        |          |This bit indicates that a start condition or repeated start condition has been detected on master mode
     * |        |          |However, this bit also indicates that a repeated start condition has been detected on slave mode.
     * |        |          |A protocol interrupt can be generated if USCI_PROTCTL.STARIEN = 1.
     * |        |          |0 = A start condition has not yet been detected.
     * |        |          |1 = A start condition has been detected.
     * |        |          |It is cleared by software writing one into this bit
     * |[9]     |STORIF    |Stop Condition Received Interrupt Flag
     * |        |          |This bit indicates that a stop condition has been detected on the I2C bus lines
     * |        |          |A protocol interrupt can be generated if USCI_PROTCTL.STORIEN = 1.
     * |        |          |0 = A stop condition has not yet been detected.
     * |        |          |1 = A stop condition has been detected.
     * |        |          |It is cleared by software writing one into this bit
     * |        |          |Note: This bit is set when slave RX mode.
     * |[10]    |NACKIF    |Non - Acknowledge Received Interrupt Flag
     * |        |          |This bit indicates that a non - acknowledge has been received in master mode
     * |        |          |A protocol interrupt can be generated if USCI_PROTCTL.NACKIEN = 1.
     * |        |          |0 = A non - acknowledge has not been received.
     * |        |          |1 = A non - acknowledge has been received.
     * |        |          |It is cleared by software writing one into this bit
     * |[11]    |ARBLOIF   |Arbitration Lost Interrupt Flag
     * |        |          |This bit indicates that an arbitration has been lost
     * |        |          |A protocol interrupt can be generated if USCI_PROTCTL.ARBLOIEN = 1.
     * |        |          |0 = An arbitration has not been lost.
     * |        |          |1 = An arbitration has been lost.
     * |        |          |It is cleared by software writing one into this bit
     * |[12]    |ERRIF     |Error Interrupt Flag
     * |        |          |This bit indicates that a Bus Error occurs when a START or STOP condition is present at an illegal position in the formation frame
     * |        |          |Example of illegal position are during the serial transfer of an address byte, a data byte or an acknowledge bit
     * |        |          |A protocol interrupt can be generated if USCI_PROTCTL.ERRIEN = 1.
     * |        |          |0 = An I2C error has not been detected.
     * |        |          |1 = An I2C error has been detected.
     * |        |          |It is cleared by software writing one into this bit
     * |        |          |Note: This bit is set when slave mode, user must write one into STO register to the defined "not addressed" slave mode.
     * |[13]    |ACKIF     |Acknowledge Received Interrupt Flag
     * |        |          |This bit indicates that an acknowledge has been received in master mode
     * |        |          |A protocol interrupt can be generated if USCI_PROTCTL.ACKIEN = 1.
     * |        |          |0 = An acknowledge has not been received.
     * |        |          |1 = An acknowledge has been received.
     * |        |          |It is cleared by software writing one into this bit
     * |[14]    |SLASEL    |Slave Select Status
     * |        |          |This bit indicates that this device has been selected as slave.
     * |        |          |0 = The device is not selected as slave.
     * |        |          |1 = The device is selected as slave.
     * |        |          |Note: This bit has no interrupt signal, and it will be cleared automatically by hardware.
     * |[15]    |SLAREAD   |Slave Read Request Status
     * |        |          |This bit indicates that a slave read request has been detected.
     * |        |          |0 = A slave R/W bit is 1 has not been detected.
     * |        |          |1 = A slave R/W bit is 1 has been detected.
     * |        |          |Note: This bit has no interrupt signal, and it will be cleared automatically by hardware.
     * |[16]    |WKAKDONE  |Wakeup Address Frame Acknowledge Bit Done
     * |        |          |0 = The ACK bit cycle of address match frame isn't done.
     * |        |          |1 = The ACK bit cycle of address match frame is done in power-down.
     * |        |          |Note: This bit can't release when WKUPIF is set.
     * |[17]    |WRSTSWK   |Read/Write Status Bit in Address Wakeup Frame
     * |        |          |0 = Write command be record on the address match wakeup frame.
     * |        |          |1 = Read command be record on the address match wakeup frame.
     * |[18]    |BUSHANG   |Bus Hang-up
     * |        |          |This bit indicates bus hang-up status
     * |        |          |There is 4-bit counter count when SCL hold high and refer fSAMP_CLK
     * |        |          |The hang-up counter will count to overflow and set this bit when SDA is low
     * |        |          |The counter will be reset by falling edge of SCL signal.
     * |        |          |0 = The bus is normal status for transmission.
     * |        |          |1 = The bus is hang-up status for transmission.
     * |        |          |Note: This bit has no interrupt signal, and it will be cleared automatically by hardware when a START condition is present.
     * |[19]    |ERRARBLO  |Error Arbitration Lost
     * |        |          |This bit indicates bus arbitration lost due to bigger noise which is can't be filtered by input processor
     * |        |          |The I2C can send start condition when ERRARBLO is set
     * |        |          |Thus this bit doesn't be cared on slave mode.
     * |        |          |0 = The bus is normal status for transmission.
     * |        |          |1 = The bus is error arbitration lost status for transmission.
     * |        |          |Note: This bit has no interrupt signal, and it will be cleared automatically by hardware when a START condition is present.
     * @var UI2C_T::ADMAT
     * Offset: 0x88  I2C Slave Match Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADMAT0    |USCI Address 0 Match Status Register
     * |        |          |When address 0 is matched, hardware will inform which address used
     * |        |          |This bit will set to 1, and software can write 1 to clear this bit.
     * |[1]     |ADMAT1    |USCI Address 1 Match Status Register
     * |        |          |When address 1 is matched, hardware will inform which address used
     * |        |          |This bit will set to 1, and software can write 1 to clear this bit.
     * @var UI2C_T::TMCTL
     * Offset: 0x8C  I2C Timing Configure Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |STCTL     |Setup Time Configure Control Register
     * |        |          |This field is used to generate a delay timing between SDA edge and SCL rising edge in transmission mode.
     * |        |          |The delay setup time is numbers of peripheral clock = STCTL x fPCLK.
     * |[24:16] |HTCTL     |Hold Time Configure Control Register
     * |        |          |This field is used to generate the delay timing between SCL falling edge SDA edge in
     * |        |          |transmission mode.
     * |        |          |The delay hold time is numbers of peripheral clock = HTCTL x fPCLK.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] USCI Control Register                                            */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t BRGEN;                 /*!< [0x0008] USCI Baud Rate Generator Register                                */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[8];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t LINECTL;               /*!< [0x002c] USCI Line Control Register                                       */
    __O  uint32_t TXDAT;                 /*!< [0x0030] USCI Transmit Data Register                                      */
    __I  uint32_t RXDAT;                 /*!< [0x0034] USCI Receive Data Register                                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[3];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DEVADDR0;              /*!< [0x0044] USCI Device Address Register 0                                   */
    __IO uint32_t DEVADDR1;              /*!< [0x0048] USCI Device Address Register 1                                   */
    __IO uint32_t ADDRMSK0;              /*!< [0x004c] USCI Device Address Mask Register 0                              */
    __IO uint32_t ADDRMSK1;              /*!< [0x0050] USCI Device Address Mask Register 1                              */
    __IO uint32_t WKCTL;                 /*!< [0x0054] USCI Wake-up Control Register                                    */
    __IO uint32_t WKSTS;                 /*!< [0x0058] USCI Wake-up Status Register                                     */
    __IO uint32_t PROTCTL;               /*!< [0x005c] USCI Protocol Control Register                                   */
    __IO uint32_t PROTIEN;               /*!< [0x0060] USCI Protocol Interrupt Enable Register                          */
    __IO uint32_t PROTSTS;               /*!< [0x0064] USCI Protocol Status Register                                    */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE3[8];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t ADMAT;                 /*!< [0x0088] I2C Slave Match Address Register                                 */
    __IO uint32_t TMCTL;                 /*!< [0x008c] I2C Timing Configure Control Register                            */

} UI2C_T;

/**
    @addtogroup UI2C_CONST UI2C Bit Field Definition
    Constant Definitions for UI2C Controller
@{ */

#define UI2C_CTL_FUNMODE_Pos             (0)                                               /*!< UI2C_T::CTL: FUNMODE Position          */
#define UI2C_CTL_FUNMODE_Msk             (0x7ul << UI2C_CTL_FUNMODE_Pos)                   /*!< UI2C_T::CTL: FUNMODE Mask              */

#define UI2C_BRGEN_RCLKSEL_Pos           (0)                                               /*!< UI2C_T::BRGEN: RCLKSEL Position        */
#define UI2C_BRGEN_RCLKSEL_Msk           (0x1ul << UI2C_BRGEN_RCLKSEL_Pos)                 /*!< UI2C_T::BRGEN: RCLKSEL Mask            */

#define UI2C_BRGEN_PTCLKSEL_Pos          (1)                                               /*!< UI2C_T::BRGEN: PTCLKSEL Position       */
#define UI2C_BRGEN_PTCLKSEL_Msk          (0x1ul << UI2C_BRGEN_PTCLKSEL_Pos)                /*!< UI2C_T::BRGEN: PTCLKSEL Mask           */

#define UI2C_BRGEN_SPCLKSEL_Pos          (2)                                               /*!< UI2C_T::BRGEN: SPCLKSEL Position       */
#define UI2C_BRGEN_SPCLKSEL_Msk          (0x3ul << UI2C_BRGEN_SPCLKSEL_Pos)                /*!< UI2C_T::BRGEN: SPCLKSEL Mask           */

#define UI2C_BRGEN_TMCNTEN_Pos           (4)                                               /*!< UI2C_T::BRGEN: TMCNTEN Position        */
#define UI2C_BRGEN_TMCNTEN_Msk           (0x1ul << UI2C_BRGEN_TMCNTEN_Pos)                 /*!< UI2C_T::BRGEN: TMCNTEN Mask            */

#define UI2C_BRGEN_TMCNTSRC_Pos          (5)                                               /*!< UI2C_T::BRGEN: TMCNTSRC Position       */
#define UI2C_BRGEN_TMCNTSRC_Msk          (0x1ul << UI2C_BRGEN_TMCNTSRC_Pos)                /*!< UI2C_T::BRGEN: TMCNTSRC Mask           */

#define UI2C_BRGEN_PDSCNT_Pos            (8)                                               /*!< UI2C_T::BRGEN: PDSCNT Position         */
#define UI2C_BRGEN_PDSCNT_Msk            (0x3ul << UI2C_BRGEN_PDSCNT_Pos)                  /*!< UI2C_T::BRGEN: PDSCNT Mask             */

#define UI2C_BRGEN_DSCNT_Pos             (10)                                              /*!< UI2C_T::BRGEN: DSCNT Position          */
#define UI2C_BRGEN_DSCNT_Msk             (0x1ful << UI2C_BRGEN_DSCNT_Pos)                  /*!< UI2C_T::BRGEN: DSCNT Mask              */

#define UI2C_BRGEN_CLKDIV_Pos            (16)                                              /*!< UI2C_T::BRGEN: CLKDIV Position         */
#define UI2C_BRGEN_CLKDIV_Msk            (0x3fful << UI2C_BRGEN_CLKDIV_Pos)                /*!< UI2C_T::BRGEN: CLKDIV Mask             */

#define UI2C_LINECTL_LSB_Pos             (0)                                               /*!< UI2C_T::LINECTL: LSB Position          */
#define UI2C_LINECTL_LSB_Msk             (0x1ul << UI2C_LINECTL_LSB_Pos)                   /*!< UI2C_T::LINECTL: LSB Mask              */

#define UI2C_LINECTL_DWIDTH_Pos          (8)                                               /*!< UI2C_T::LINECTL: DWIDTH Position       */
#define UI2C_LINECTL_DWIDTH_Msk          (0xful << UI2C_LINECTL_DWIDTH_Pos)                /*!< UI2C_T::LINECTL: DWIDTH Mask           */

#define UI2C_TXDAT_TXDAT_Pos             (0)                                               /*!< UI2C_T::TXDAT: TXDAT Position          */
#define UI2C_TXDAT_TXDAT_Msk             (0xfffful << UI2C_TXDAT_TXDAT_Pos)                /*!< UI2C_T::TXDAT: TXDAT Mask              */

#define UI2C_RXDAT_RXDAT_Pos             (0)                                               /*!< UI2C_T::RXDAT: RXDAT Position          */
#define UI2C_RXDAT_RXDAT_Msk             (0xfffful << UI2C_RXDAT_RXDAT_Pos)                /*!< UI2C_T::RXDAT: RXDAT Mask              */

#define UI2C_DEVADDR0_DEVADDR_Pos        (0)                                               /*!< UI2C_T::DEVADDR0: DEVADDR Position     */
#define UI2C_DEVADDR0_DEVADDR_Msk        (0x3fful << UI2C_DEVADDR0_DEVADDR_Pos)            /*!< UI2C_T::DEVADDR0: DEVADDR Mask         */

#define UI2C_DEVADDR1_DEVADDR_Pos        (0)                                               /*!< UI2C_T::DEVADDR1: DEVADDR Position     */
#define UI2C_DEVADDR1_DEVADDR_Msk        (0x3fful << UI2C_DEVADDR1_DEVADDR_Pos)            /*!< UI2C_T::DEVADDR1: DEVADDR Mask         */

#define UI2C_ADDRMSK0_ADDRMSK_Pos        (0)                                               /*!< UI2C_T::ADDRMSK0: ADDRMSK Position     */
#define UI2C_ADDRMSK0_ADDRMSK_Msk        (0x3fful << UI2C_ADDRMSK0_ADDRMSK_Pos)            /*!< UI2C_T::ADDRMSK0: ADDRMSK Mask         */

#define UI2C_ADDRMSK1_ADDRMSK_Pos        (0)                                               /*!< UI2C_T::ADDRMSK1: ADDRMSK Position     */
#define UI2C_ADDRMSK1_ADDRMSK_Msk        (0x3fful << UI2C_ADDRMSK1_ADDRMSK_Pos)            /*!< UI2C_T::ADDRMSK1: ADDRMSK Mask         */

#define UI2C_WKCTL_WKEN_Pos              (0)                                               /*!< UI2C_T::WKCTL: WKEN Position           */
#define UI2C_WKCTL_WKEN_Msk              (0x1ul << UI2C_WKCTL_WKEN_Pos)                    /*!< UI2C_T::WKCTL: WKEN Mask               */

#define UI2C_WKCTL_WKADDREN_Pos          (1)                                               /*!< UI2C_T::WKCTL: WKADDREN Position       */
#define UI2C_WKCTL_WKADDREN_Msk          (0x1ul << UI2C_WKCTL_WKADDREN_Pos)                /*!< UI2C_T::WKCTL: WKADDREN Mask           */

#define UI2C_WKSTS_WKF_Pos               (0)                                               /*!< UI2C_T::WKSTS: WKF Position            */
#define UI2C_WKSTS_WKF_Msk               (0x1ul << UI2C_WKSTS_WKF_Pos)                     /*!< UI2C_T::WKSTS: WKF Mask                */

#define UI2C_PROTCTL_GCFUNC_Pos          (0)                                               /*!< UI2C_T::PROTCTL: GCFUNC Position       */
#define UI2C_PROTCTL_GCFUNC_Msk          (0x1ul << UI2C_PROTCTL_GCFUNC_Pos)                /*!< UI2C_T::PROTCTL: GCFUNC Mask           */

#define UI2C_PROTCTL_AA_Pos              (1)                                               /*!< UI2C_T::PROTCTL: AA Position           */
#define UI2C_PROTCTL_AA_Msk              (0x1ul << UI2C_PROTCTL_AA_Pos)                    /*!< UI2C_T::PROTCTL: AA Mask               */

#define UI2C_PROTCTL_STO_Pos             (2)                                               /*!< UI2C_T::PROTCTL: STO Position          */
#define UI2C_PROTCTL_STO_Msk             (0x1ul << UI2C_PROTCTL_STO_Pos)                   /*!< UI2C_T::PROTCTL: STO Mask              */

#define UI2C_PROTCTL_STA_Pos             (3)                                               /*!< UI2C_T::PROTCTL: STA Position          */
#define UI2C_PROTCTL_STA_Msk             (0x1ul << UI2C_PROTCTL_STA_Pos)                   /*!< UI2C_T::PROTCTL: STA Mask              */

#define UI2C_PROTCTL_ADDR10EN_Pos        (4)                                               /*!< UI2C_T::PROTCTL: ADDR10EN Position     */
#define UI2C_PROTCTL_ADDR10EN_Msk        (0x1ul << UI2C_PROTCTL_ADDR10EN_Pos)              /*!< UI2C_T::PROTCTL: ADDR10EN Mask         */

#define UI2C_PROTCTL_PTRG_Pos            (5)                                               /*!< UI2C_T::PROTCTL: PTRG Position         */
#define UI2C_PROTCTL_PTRG_Msk            (0x1ul << UI2C_PROTCTL_PTRG_Pos)                  /*!< UI2C_T::PROTCTL: PTRG Mask             */

#define UI2C_PROTCTL_SCLOUTEN_Pos        (8)                                               /*!< UI2C_T::PROTCTL: SCLOUTEN Position     */
#define UI2C_PROTCTL_SCLOUTEN_Msk        (0x1ul << UI2C_PROTCTL_SCLOUTEN_Pos)              /*!< UI2C_T::PROTCTL: SCLOUTEN Mask         */

#define UI2C_PROTCTL_MONEN_Pos           (9)                                               /*!< UI2C_T::PROTCTL: MONEN Position        */
#define UI2C_PROTCTL_MONEN_Msk           (0x1ul << UI2C_PROTCTL_MONEN_Pos)                 /*!< UI2C_T::PROTCTL: MONEN Mask            */

#define UI2C_PROTCTL_TOCNT_Pos           (16)                                              /*!< UI2C_T::PROTCTL: TOCNT Position        */
#define UI2C_PROTCTL_TOCNT_Msk           (0x3fful << UI2C_PROTCTL_TOCNT_Pos)               /*!< UI2C_T::PROTCTL: TOCNT Mask            */

#define UI2C_PROTCTL_PROTEN_Pos          (31)                                              /*!< UI2C_T::PROTCTL: PROTEN Position       */
#define UI2C_PROTCTL_PROTEN_Msk          (0x1ul << UI2C_PROTCTL_PROTEN_Pos)                /*!< UI2C_T::PROTCTL: PROTEN Mask           */

#define UI2C_PROTIEN_TOIEN_Pos           (0)                                               /*!< UI2C_T::PROTIEN: TOIEN Position        */
#define UI2C_PROTIEN_TOIEN_Msk           (0x1ul << UI2C_PROTIEN_TOIEN_Pos)                 /*!< UI2C_T::PROTIEN: TOIEN Mask            */

#define UI2C_PROTIEN_STARIEN_Pos         (1)                                               /*!< UI2C_T::PROTIEN: STARIEN Position      */
#define UI2C_PROTIEN_STARIEN_Msk         (0x1ul << UI2C_PROTIEN_STARIEN_Pos)               /*!< UI2C_T::PROTIEN: STARIEN Mask          */

#define UI2C_PROTIEN_STORIEN_Pos         (2)                                               /*!< UI2C_T::PROTIEN: STORIEN Position      */
#define UI2C_PROTIEN_STORIEN_Msk         (0x1ul << UI2C_PROTIEN_STORIEN_Pos)               /*!< UI2C_T::PROTIEN: STORIEN Mask          */

#define UI2C_PROTIEN_NACKIEN_Pos         (3)                                               /*!< UI2C_T::PROTIEN: NACKIEN Position      */
#define UI2C_PROTIEN_NACKIEN_Msk         (0x1ul << UI2C_PROTIEN_NACKIEN_Pos)               /*!< UI2C_T::PROTIEN: NACKIEN Mask          */

#define UI2C_PROTIEN_ARBLOIEN_Pos        (4)                                               /*!< UI2C_T::PROTIEN: ARBLOIEN Position     */
#define UI2C_PROTIEN_ARBLOIEN_Msk        (0x1ul << UI2C_PROTIEN_ARBLOIEN_Pos)              /*!< UI2C_T::PROTIEN: ARBLOIEN Mask         */

#define UI2C_PROTIEN_ERRIEN_Pos          (5)                                               /*!< UI2C_T::PROTIEN: ERRIEN Position       */
#define UI2C_PROTIEN_ERRIEN_Msk          (0x1ul << UI2C_PROTIEN_ERRIEN_Pos)                /*!< UI2C_T::PROTIEN: ERRIEN Mask           */

#define UI2C_PROTIEN_ACKIEN_Pos          (6)                                               /*!< UI2C_T::PROTIEN: ACKIEN Position       */
#define UI2C_PROTIEN_ACKIEN_Msk          (0x1ul << UI2C_PROTIEN_ACKIEN_Pos)                /*!< UI2C_T::PROTIEN: ACKIEN Mask           */

#define UI2C_PROTSTS_TOIF_Pos            (5)                                               /*!< UI2C_T::PROTSTS: TOIF Position         */
#define UI2C_PROTSTS_TOIF_Msk            (0x1ul << UI2C_PROTSTS_TOIF_Pos)                  /*!< UI2C_T::PROTSTS: TOIF Mask             */

#define UI2C_PROTSTS_ONBUSY_Pos          (6)                                               /*!< UI2C_T::PROTSTS: ONBUSY Position       */
#define UI2C_PROTSTS_ONBUSY_Msk          (0x1ul << UI2C_PROTSTS_ONBUSY_Pos)                /*!< UI2C_T::PROTSTS: ONBUSY Mask           */

#define UI2C_PROTSTS_STARIF_Pos          (8)                                               /*!< UI2C_T::PROTSTS: STARIF Position       */
#define UI2C_PROTSTS_STARIF_Msk          (0x1ul << UI2C_PROTSTS_STARIF_Pos)                /*!< UI2C_T::PROTSTS: STARIF Mask           */

#define UI2C_PROTSTS_STORIF_Pos          (9)                                               /*!< UI2C_T::PROTSTS: STORIF Position       */
#define UI2C_PROTSTS_STORIF_Msk          (0x1ul << UI2C_PROTSTS_STORIF_Pos)                /*!< UI2C_T::PROTSTS: STORIF Mask           */

#define UI2C_PROTSTS_NACKIF_Pos          (10)                                              /*!< UI2C_T::PROTSTS: NACKIF Position       */
#define UI2C_PROTSTS_NACKIF_Msk          (0x1ul << UI2C_PROTSTS_NACKIF_Pos)                /*!< UI2C_T::PROTSTS: NACKIF Mask           */

#define UI2C_PROTSTS_ARBLOIF_Pos         (11)                                              /*!< UI2C_T::PROTSTS: ARBLOIF Position      */
#define UI2C_PROTSTS_ARBLOIF_Msk         (0x1ul << UI2C_PROTSTS_ARBLOIF_Pos)               /*!< UI2C_T::PROTSTS: ARBLOIF Mask          */

#define UI2C_PROTSTS_ERRIF_Pos           (12)                                              /*!< UI2C_T::PROTSTS: ERRIF Position        */
#define UI2C_PROTSTS_ERRIF_Msk           (0x1ul << UI2C_PROTSTS_ERRIF_Pos)                 /*!< UI2C_T::PROTSTS: ERRIF Mask            */

#define UI2C_PROTSTS_ACKIF_Pos           (13)                                              /*!< UI2C_T::PROTSTS: ACKIF Position        */
#define UI2C_PROTSTS_ACKIF_Msk           (0x1ul << UI2C_PROTSTS_ACKIF_Pos)                 /*!< UI2C_T::PROTSTS: ACKIF Mask            */

#define UI2C_PROTSTS_SLASEL_Pos          (14)                                              /*!< UI2C_T::PROTSTS: SLASEL Position       */
#define UI2C_PROTSTS_SLASEL_Msk          (0x1ul << UI2C_PROTSTS_SLASEL_Pos)                /*!< UI2C_T::PROTSTS: SLASEL Mask           */

#define UI2C_PROTSTS_SLAREAD_Pos         (15)                                              /*!< UI2C_T::PROTSTS: SLAREAD Position      */
#define UI2C_PROTSTS_SLAREAD_Msk         (0x1ul << UI2C_PROTSTS_SLAREAD_Pos)               /*!< UI2C_T::PROTSTS: SLAREAD Mask          */

#define UI2C_PROTSTS_WKAKDONE_Pos        (16)                                              /*!< UI2C_T::PROTSTS: WKAKDONE Position     */
#define UI2C_PROTSTS_WKAKDONE_Msk        (0x1ul << UI2C_PROTSTS_WKAKDONE_Pos)              /*!< UI2C_T::PROTSTS: WKAKDONE Mask         */

#define UI2C_PROTSTS_WRSTSWK_Pos         (17)                                              /*!< UI2C_T::PROTSTS: WRSTSWK Position      */
#define UI2C_PROTSTS_WRSTSWK_Msk         (0x1ul << UI2C_PROTSTS_WRSTSWK_Pos)               /*!< UI2C_T::PROTSTS: WRSTSWK Mask          */

#define UI2C_PROTSTS_BUSHANG_Pos         (18)                                              /*!< UI2C_T::PROTSTS: BUSHANG Position      */
#define UI2C_PROTSTS_BUSHANG_Msk         (0x1ul << UI2C_PROTSTS_BUSHANG_Pos)               /*!< UI2C_T::PROTSTS: BUSHANG Mask          */

#define UI2C_PROTSTS_ERRARBLO_Pos        (19)                                              /*!< UI2C_T::PROTSTS: ERRARBLO Position     */
#define UI2C_PROTSTS_ERRARBLO_Msk        (0x1ul << UI2C_PROTSTS_ERRARBLO_Pos)              /*!< UI2C_T::PROTSTS: ERRARBLO Mask         */

#define UI2C_ADMAT_ADMAT0_Pos            (0)                                               /*!< UI2C_T::ADMAT: ADMAT0 Position         */
#define UI2C_ADMAT_ADMAT0_Msk            (0x1ul << UI2C_ADMAT_ADMAT0_Pos)                  /*!< UI2C_T::ADMAT: ADMAT0 Mask             */

#define UI2C_ADMAT_ADMAT1_Pos            (1)                                               /*!< UI2C_T::ADMAT: ADMAT1 Position         */
#define UI2C_ADMAT_ADMAT1_Msk            (0x1ul << UI2C_ADMAT_ADMAT1_Pos)                  /*!< UI2C_T::ADMAT: ADMAT1 Mask             */

#define UI2C_TMCTL_STCTL_Pos             (0)                                               /*!< UI2C_T::TMCTL: STCTL Position          */
#define UI2C_TMCTL_STCTL_Msk             (0x1fful << UI2C_TMCTL_STCTL_Pos)                 /*!< UI2C_T::TMCTL: STCTL Mask              */

#define UI2C_TMCTL_HTCTL_Pos             (16)                                              /*!< UI2C_T::TMCTL: HTCTL Position          */
#define UI2C_TMCTL_HTCTL_Msk             (0x1fful << UI2C_TMCTL_HTCTL_Pos)                 /*!< UI2C_T::TMCTL: HTCTL Mask              */

/**@}*/ /* UI2C_CONST */
/**@}*/ /* end of UI2C register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __UI2C_REG_H__ */
