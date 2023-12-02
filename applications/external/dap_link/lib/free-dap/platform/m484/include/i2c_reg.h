/**************************************************************************//**
 * @file     i2c_reg.h
 * @version  V1.00
 * @brief    I2C register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __I2C_REG_H__
#define __I2C_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup I2C Inter-IC Bus Controller(I2C)
    Memory Mapped Structure for I2C Controller
@{ */

typedef struct
{


    /**
     * @var I2C_T::CTL0
     * Offset: 0x00  I2C Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2]     |AA        |Assert Acknowledge Control
     * |        |          |When AA =1 prior to address or data is received, an acknowledged (low level to SDA) will be returned during the acknowledge clock pulse on the SCL line when 1.) A slave is acknowledging the address sent from master, 2.) The receiver devices are acknowledging the data sent by transmitter
     * |        |          |When AA=0 prior to address or data received, a Not acknowledged (high level to SDA) will be returned during the acknowledge clock pulse on the SCL line
     * |[3]     |SI        |I2C Interrupt Flag
     * |        |          |When a new I2C state is present in the I2C_STATUS register, the SI flag is set by hardware
     * |        |          |If bit INTEN (I2C_CTL [7]) is set, the I2C interrupt is requested
     * |        |          |SI must be cleared by software
     * |        |          |Clear SI by writing 1 to this bit.
     * |        |          |For ACKMEN is set in slave read mode, the SI flag is set in 8th clock period for user to confirm the acknowledge bit and 9th clock period for user to read the data in the data buffer.
     * |[4]     |STO       |I2C STOP Control
     * |        |          |In Master mode, setting STO to transmit a STOP condition to bus then I2C controller will check the bus condition if a STOP condition is detected
     * |        |          |This bit will be cleared by hardware automatically.
     * |[5]     |STA       |I2C START Control
     * |        |          |Setting STA to logic 1 to enter Master mode, the I2C hardware sends a START or repeat START condition to bus when the bus is free.
     * |[6]     |I2CEN     |I2C Controller Enable Bit
     * |        |          |Set to enable I2C serial function controller
     * |        |          |When I2CEN=1 the I2C serial function enable
     * |        |          |The multi-function pin function must set to SDA, and SCL of I2C function first.
     * |        |          |0 = I2C controller Disabled.
     * |        |          |1 = I2C controller Enabled.
     * |[7]     |INTEN     |Enable Interrupt
     * |        |          |0 = I2C interrupt Disabled.
     * |        |          |1 = I2C interrupt Enabled.
     * @var I2C_T::ADDR0
     * Offset: 0x04  I2C Slave Address Register0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GC        |General Call Function
     * |        |          |0 = General Call Function Disabled.
     * |        |          |1 = General Call Function Enabled.
     * |[10:1]  |ADDR      |I2C Address
     * |        |          |The content of this register is irrelevant when I2C is in Master mode
     * |        |          |In the slave mode, the seven most significant bits must be loaded with the chip's own address
     * |        |          |The I2C hardware will react if either of the address is matched.
     * |        |          |Note: When software set 10'h000, the address can not be used.
     * @var I2C_T::DAT
     * Offset: 0x08  I2C Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |DAT       |I2C Data
     * |        |          |Bit [7:0] is located with the 8-bit transferred/received data of I2C serial port.
     * @var I2C_T::STATUS0
     * Offset: 0x0C  I2C Status Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |STATUS    |I2C Status
     * |        |          |The three least significant bits are always 0
     * |        |          |The five most significant bits contain the status code
     * |        |          |There are 28 possible status codes
     * |        |          |When the content of I2C_STATUS is F8H, no serial interrupt is requested
     * |        |          |Others I2C_STATUS values correspond to defined I2C states
     * |        |          |When each of these states is entered, a status interrupt is requested (SI = 1)
     * |        |          |A valid status code is present in I2C_STATUS one cycle after SI is set by hardware and is still present one cycle after SI has been reset by software
     * |        |          |In addition, states 00H stands for a Bus Error
     * |        |          |A Bus Error occurs when a START or STOP condition is present at an illegal position in the formation frame
     * |        |          |Example of illegal position are during the serial transfer of an address byte, a data byte or an acknowledge bit.
     * @var I2C_T::CLKDIV
     * Offset: 0x10  I2C Clock Divided Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |DIVIDER   |I2C Clock Divided
     * |        |          |Indicates the I2C clock rate: Data Baud Rate of I2C = (system clock) / (4x (I2C_CLKDIV+1)).
     * |        |          |Note: The minimum value of I2C_CLKDIV is 4.
     * @var I2C_T::TOCTL
     * Offset: 0x14  I2C Time-out Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TOIF      |Time-out Flag
     * |        |          |This bit is set by hardware when I2C time-out happened and it can interrupt CPU if I2C interrupt enable bit (INTEN) is set to 1.
     * |        |          |Note: Software can write 1 to clear this bit.
     * |[1]     |TOCDIV4   |Time-out Counter Input Clock Divided by 4
     * |        |          |When Enabled, The time-out period is extend 4 times.
     * |        |          |0 = Time-out period is extend 4 times Disabled.
     * |        |          |1 = Time-out period is extend 4 times Enabled.
     * |[2]     |TOCEN     |Time-out Counter Enable Bit
     * |        |          |When Enabled, the 14-bit time-out counter will start counting when SI is clear
     * |        |          |Setting flag SI to '1' will reset counter and re-start up counting after SI is cleared.
     * |        |          |0 = Time-out counter Disabled.
     * |        |          |1 = Time-out counter Enabled.
     * @var I2C_T::ADDR1
     * Offset: 0x18  I2C Slave Address Register1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GC        |General Call Function
     * |        |          |0 = General Call Function Disabled.
     * |        |          |1 = General Call Function Enabled.
     * |[10:1]  |ADDR      |I2C Address
     * |        |          |The content of this register is irrelevant when I2C is in Master mode
     * |        |          |In the slave mode, the seven most significant bits must be loaded with the chip's own address
     * |        |          |The I2C hardware will react if either of the address is matched.
     * |        |          |Note: When software set 10'h000, the address can not be used.
     * @var I2C_T::ADDR2
     * Offset: 0x1C  I2C Slave Address Register2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GC        |General Call Function
     * |        |          |0 = General Call Function Disabled.
     * |        |          |1 = General Call Function Enabled.
     * |[10:1]  |ADDR      |I2C Address
     * |        |          |The content of this register is irrelevant when I2C is in Master mode
     * |        |          |In the slave mode, the seven most significant bits must be loaded with the chip's own address
     * |        |          |The I2C hardware will react if either of the address is matched.
     * |        |          |Note: When software set 10'h000, the address can not be used.
     * @var I2C_T::ADDR3
     * Offset: 0x20  I2C Slave Address Register3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GC        |General Call Function
     * |        |          |0 = General Call Function Disabled.
     * |        |          |1 = General Call Function Enabled.
     * |[10:1]  |ADDR      |I2C Address
     * |        |          |The content of this register is irrelevant when I2C is in Master mode
     * |        |          |In the slave mode, the seven most significant bits must be loaded with the chip's own address
     * |        |          |The I2C hardware will react if either of the address is matched.
     * |        |          |Note: When software set 10'h000, the address can not be used.
     * @var I2C_T::ADDRMSK0
     * Offset: 0x24  I2C Slave Address Mask Register0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:1]  |ADDRMSK   |I2C Address Mask
     * |        |          |0 = Mask Disabled (the received corresponding register bit should be exact the same as address register.).
     * |        |          |1 = Mask Enabled (the received corresponding address bit is don't care.).
     * |        |          |I2C bus controllers support multiple address recognition with four address mask register
     * |        |          |When the bit in the address mask register is set to one, it means the received corresponding address bit is don't-care
     * |        |          |If the bit is set to zero, that means the received corresponding register bit should be exact the same as address register.
     * |        |          |Note: The wake-up function can not use address mask.
     * @var I2C_T::ADDRMSK1
     * Offset: 0x28  I2C Slave Address Mask Register1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:1]  |ADDRMSK   |I2C Address Mask
     * |        |          |0 = Mask Disabled (the received corresponding register bit should be exact the same as address register.).
     * |        |          |1 = Mask Enabled (the received corresponding address bit is don't care.).
     * |        |          |I2C bus controllers support multiple address recognition with four address mask register
     * |        |          |When the bit in the address mask register is set to one, it means the received corresponding address bit is don't-care
     * |        |          |If the bit is set to zero, that means the received corresponding register bit should be exact the same as address register.
     * |        |          |Note: The wake-up function can not use address mask.
     * @var I2C_T::ADDRMSK2
     * Offset: 0x2C  I2C Slave Address Mask Register2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:1]  |ADDRMSK   |I2C Address Mask
     * |        |          |0 = Mask Disabled (the received corresponding register bit should be exact the same as address register.).
     * |        |          |1 = Mask Enabled (the received corresponding address bit is don't care.).
     * |        |          |I2C bus controllers support multiple address recognition with four address mask register
     * |        |          |When the bit in the address mask register is set to one, it means the received corresponding address bit is don't-care
     * |        |          |If the bit is set to zero, that means the received corresponding register bit should be exact the same as address register.
     * |        |          |Note: The wake-up function can not use address mask.
     * @var I2C_T::ADDRMSK3
     * Offset: 0x30  I2C Slave Address Mask Register3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:1]  |ADDRMSK   |I2C Address Mask
     * |        |          |0 = Mask Disabled (the received corresponding register bit should be exact the same as address register.).
     * |        |          |1 = Mask Enabled (the received corresponding address bit is don't care.).
     * |        |          |I2C bus controllers support multiple address recognition with four address mask register
     * |        |          |When the bit in the address mask register is set to one, it means the received corresponding address bit is don't-care
     * |        |          |If the bit is set to zero, that means the received corresponding register bit should be exact the same as address register.
     * |        |          |Note: The wake-up function can not use address mask.
     * @var I2C_T::WKCTL
     * Offset: 0x3C  I2C Wake-up Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKEN      |I2C Wake-up Enable Bit
     * |        |          |0 = I2C wake-up function Disabled.
     * |        |          |1 = I2C wake-up function Enabled.
     * |[7]     |NHDBUSEN  |I2C No Hold BUS Enable Bit
     * |        |          |0 = I2C hold bus after wake-up.
     * |        |          |1 = I2C don't hold bus after wake-up.
     * |        |          |Note: I2C controller could response when WKIF event is not clear, it may cause error data transmitted or received
     * |        |          |If data transmitted or received when WKIF event is not clear, user must reset I2C controller and execute the original operation again.
     * @var I2C_T::WKSTS
     * Offset: 0x40  I2C Wake-up Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WKIF      |I2C Wake-up Flag
     * |        |          |When chip is woken up from Power-down mode by I2C, this bit is set to 1
     * |        |          |Software can write 1 to clear this bit.
     * |[1]     |WKAKDONE  |Wakeup Address Frame Acknowledge Bit Done
     * |        |          |0 = The ACK bit cycle of address match frame isn't done.
     * |        |          |1 = The ACK bit cycle of address match frame is done in power-down.
     * |        |          |Note: This bit can't release WKIF. Software can write 1 to clear this bit.
     * |[2]     |WRSTSWK   |Read/Write Status Bit in Address Wakeup Frame
     * |        |          |0 = Write command be record on the address match wakeup frame.
     * |        |          |1 = Read command be record on the address match wakeup frame.
     * |        |          |Note: This bit will be cleared when software can write 1 to WKAKDONE bit.
     * @var I2C_T::CTL1
     * Offset: 0x44  I2C Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TXPDMAEN  |PDMA Transmit Channel Available
     * |        |          |0 = Transmit PDMA function disable.
     * |        |          |1 = Transmit PDMA function enable.
     * |[1]     |RXPDMAEN  |PDMA Receive Channel Available
     * |        |          |0 = Receive PDMA function disable.
     * |        |          |1 = Receive PDMA function enable.
     * |[2]     |PDMARST   |PDMA Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the I2C request to PDMA.
     * |[8]     |PDMASTR   |PDMA Stretch Bit
     * |        |          |0 = I2C send STOP automatically after PDMA transfer done. (only master TX)
     * |        |          |1 = I2C SCL bus is stretched by hardware after PDMA transfer done if the SI is not cleared
     * |        |          |(only master TX)
     * |[9]     |ADDR10EN  |Address 10-bit Function Enable
     * |        |          |0 = Address match 10-bit function is disabled.
     * |        |          |1 = Address match 10-bit function is enabled.
     * @var I2C_T::STATUS1
     * Offset: 0x48  I2C Status Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADMAT0    |I2C Address 0 Match Status Register
     * |        |          |When address 0 is matched, hardware will inform which address used
     * |        |          |This bit will set to 1, and software can write 1 to clear this bit.
     * |[1]     |ADMAT1    |I2C Address 1 Match Status Register
     * |        |          |When address 1 is matched, hardware will inform which address used
     * |        |          |This bit will set to 1, and software can write 1 to clear this bit.
     * |[2]     |ADMAT2    |I2C Address 2 Match Status Register
     * |        |          |When address 2 is matched, hardware will inform which address used
     * |        |          |This bit will set to 1, and software can write 1 to clear this bit.
     * |[3]     |ADMAT3    |I2C Address 3 Match Status Register
     * |        |          |When address 3 is matched, hardware will inform which address used
     * |        |          |This bit will set to 1, and software can write 1 to clear this bit.
     * |[8]     |ONBUSY    |On Bus Busy
     * |        |          |Indicates that a communication is in progress on the bus
     * |        |          |It is set by hardware when a START condition is detected
     * |        |          |It is cleared by hardware when a STOP condition is detected.
     * |        |          |0 = The bus is IDLE (both SCLK and SDA High).
     * |        |          |1 = The bus is busy.
     * |        |          |Note:This bit is read only.
     * @var I2C_T::TMCTL
     * Offset: 0x4C  I2C Timing Configure Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |STCTL     |Setup Time Configure Control Register
     * |        |          |This field is used to generate a delay timing between SDA falling edge and SCL rising edge in transmission mode.
     * |        |          |The delay setup time is numbers of peripheral clock = STCTL x PCLK.
     * |        |          |Note: Setup time setting should not make SCL output less than three PCLKs.
     * |[24:16] |HTCTL     |Hold Time Configure Control Register
     * |        |          |This field is used to generate the delay timing between SCL falling edge and SDA rising edge in transmission mode.
     * |        |          |The delay hold time is numbers of peripheral clock = HTCTL x PCLK.
     * @var I2C_T::BUSCTL
     * Offset: 0x50  I2C Bus Management Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ACKMEN    |Acknowledge Control by Manual
     * |        |          |In order to allow ACK control in slave reception including the command and data, slave byte control mode must be enabled by setting the ACKMEN bit.
     * |        |          |0 = Slave byte control Disabled.
     * |        |          |1 = Slave byte control Enabled
     * |        |          |The 9th bit can response the ACK or NACK according the received data by user
     * |        |          |When the byte is received, stretching the SCLK signal low between the 8th and 9th SCLK pulse.
     * |        |          |Note: If the BMDEN=1 and this bit is enabled, the information of I2C_STATUS will be fixed as 0xF0 in slave receive condition.
     * |[1]     |PECEN     |Packet Error Checking Calculation Enable Bit
     * |        |          |0 = Packet Error Checking Calculation Disabled.
     * |        |          |1 = Packet Error Checking Calculation Enabled.
     * |        |          |Note: When I2C enter power down mode, the bit should be enabled after wake-up if needed PEC calculation.
     * |[2]     |BMDEN     |Bus Management Device Default Address Enable Bit
     * |        |          |0 = Device default address Disable
     * |        |          |When the address 0'b1100001x coming and the both of BMDEN and ACKMEN are enabled, the device responses NACKed
     * |        |          |1 = Device default address Enabled
     * |        |          |When the address 0'b1100001x coming and the both of BMDEN and ACKMEN are enabled, the device responses ACKed.
     * |[3]     |BMHEN     |Bus Management Host Enable Bit
     * |        |          |0 = Host function Disabled.
     * |        |          |1 = Host function Enabled.
     * |[4]     |ALERTEN   |Bus Management Alert Enable Bit
     * |        |          |Device Mode (BMHEN=0).
     * |        |          |0 = Release the BM_ALERT pin high and Alert Response Header disabled: 0001100x followed by NACK if both of BMDEN and ACKMEN are enabled.
     * |        |          |1 = Drive BM_ALERT pin low and Alert Response Address Header enables: 0001100x followed by ACK if both of BMDEN and ACKMEN are enabled.
     * |        |          |Host Mode (BMHEN=1).
     * |        |          |0 = BM_ALERT pin not supported.
     * |        |          |1 = BM_ALERT pin supported.
     * |[5]     |SCTLOSTS  |Suspend/Control Data Output Status
     * |        |          |0 = The output of SUSCON pin is low.
     * |        |          |1 = The output of SUSCON pin is high.
     * |[6]     |SCTLOEN   |Suspend or Control Pin Output Enable Bit
     * |        |          |0 = The SUSCON pin in input.
     * |        |          |1 = The output enable is active on the SUSCON pin.
     * |[7]     |BUSEN     |BUS Enable Bit
     * |        |          |0 = The system management function is Disabled.
     * |        |          |1 = The system management function is Enable.
     * |        |          |Note: When the bit is enabled, the internal 14-bit counter is used to calculate the time out event of clock low condition.
     * |[8]     |PECTXEN   |Packet Error Checking Byte Transmission/Reception
     * |        |          |0 = No PEC transfer.
     * |        |          |1 = PEC transmission is requested.
     * |        |          |Note: This bit has no effect in slave mode when ACKMEN=0.
     * |[9]     |TIDLE     |Timer Check in Idle State
     * |        |          |The BUSTOUT is used to calculate the time-out of clock low in bus active and the idle period in bus Idle
     * |        |          |This bit is used to define which condition is enabled.
     * |        |          |0 = The BUSTOUT is used to calculate the clock low period in bus active.
     * |        |          |1 = The BUSTOUT is used to calculate the IDLE period in bus Idle.
     * |        |          |Note: The BUSY (I2C_BUSSTS[0]) indicate the current bus state.
     * |[10]    |PECCLR    |PEC Clear at Repeat Start
     * |        |          |The calculation of PEC starts when PECEN is set to 1 and it is clear when the STA or STO bit is detected
     * |        |          |This PECCLR bit is used to enable the condition of REPEAT START can clear the PEC calculation.
     * |        |          |0 = The PEC calculation is cleared by "Repeat Start" function is Disabled.
     * |        |          |1 = The PEC calculation is cleared by "Repeat Start"" function is Enabled.
     * |[11]    |ACKM9SI   |Acknowledge Manual Enable Extra SI Interrupt
     * |        |          |0 = There is no SI interrupt in the 9th clock cycle when the BUSEN=1 and ACKMEN=1.
     * |        |          |1 = There is SI interrupt in the 9th clock cycle when the BUSEN=1 and ACKMEN=1.
     * |[12]    |BCDIEN    |Packet Error Checking Byte Count Done Interrupt Enable Bit
     * |        |          |0 = Indicates the byte count done interrupt is Disabled.
     * |        |          |1 = Indicates the byte count done interrupt is Enabled.
     * |        |          |Note: This bit is used in PECEN=1.
     * |[13]    |PECDIEN   |Packet Error Checking Byte Transfer Done Interrupt Enable Bit
     * |        |          |0 = Indicates the PEC transfer done interrupt is Disabled.
     * |        |          |1 = Indicates the PEC transfer done interrupt is Enabled.
     * |        |          |Note: This bit is used in PECEN=1.
     * @var I2C_T::BUSTCTL
     * Offset: 0x54  I2C Bus Management Timer Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSTOEN   |Bus Time Out Enable Bit
     * |        |          |0 = Indicates the bus clock low time-out detection is Disabled.
     * |        |          |1 = Indicates the bus clock low time-out detection is Enabled (bus clock is low for more than TTime-out (in BIDLE=0) or high more than TTime-out(in BIDLE =1)
     * |[1]     |CLKTOEN   |Cumulative Clock Low Time Out Enable Bit
     * |        |          |0 = Indicates the cumulative clock low time-out detection is Disabled.
     * |        |          |1 = Indicates the cumulative clock low time-out detection is Enabled.
     * |        |          |For Master, it calculates the period from START to ACK
     * |        |          |For Slave, it calculates the period from START to STOP
     * |[2]     |BUSTOIEN  |Time-out Interrupt Enable Bit
     * |        |          |BUSY =1.
     * |        |          |0 = Indicates the SCLK low time-out interrupt is Disabled.
     * |        |          |1 = Indicates the SCLK low time-out interrupt is Enabled.
     * |        |          |BUSY =0.
     * |        |          |0 = Indicates the bus IDLE time-out interrupt is Disabled.
     * |        |          |1 = Indicates the bus IDLE time-out interrupt is Enabled.
     * |[3]     |CLKTOIEN  |Extended Clock Time Out Interrupt Enable Bit
     * |        |          |0 = Indicates the clock time out interrupt is Disabled.
     * |        |          |1 = Indicates the clock time out interrupt is Enabled.
     * |[4]     |TORSTEN   |Time Out Reset Enable Bit
     * |        |          |0 = Indicates the I2C state machine reset is Disable.
     * |        |          |1 = Indicates the I2C state machine reset is Enable. (The clock and data bus will be released to high)
     * @var I2C_T::BUSSTS
     * Offset: 0x58  I2C Bus Management Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSY      |Bus Busy
     * |        |          |Indicates that a communication is in progress on the bus
     * |        |          |It is set by hardware when a START condition is detected
     * |        |          |It is cleared by hardware when a STOP condition is detected
     * |        |          |0 = The bus is IDLE (both SCLK and SDA High).
     * |        |          |1 = The bus is busy.
     * |[1]     |BCDONE    |Byte Count Transmission/Receive Done
     * |        |          |0 = Indicates the byte count transmission/ receive is not finished when the PECEN is set.
     * |        |          |1 = Indicates the byte count transmission/ receive is finished when the PECEN is set.
     * |        |          |Note: Software can write 1 to clear this bit.
     * |[2]     |PECERR    |PEC Error in Reception
     * |        |          |0 = Indicates the PEC value equal the received PEC data packet.
     * |        |          |1 = Indicates the PEC value doesn't match the receive PEC data packet.
     * |        |          |Note: Software can write 1 to clear this bit.
     * |[3]     |ALERT     |SMBus Alert Status
     * |        |          |Device Mode (BMHEN =0).
     * |        |          |0 = Indicates SMBALERT pin state is low.
     * |        |          |1 = Indicates SMBALERT pin state is high.
     * |        |          |Host Mode (BMHEN =1).
     * |        |          |0 = No SMBALERT event.
     * |        |          |1 = Indicates there is SMBALERT event (falling edge) is detected in SMALERT pin when the BMHEN = 1 (SMBus host configuration) and the ALERTEN = 1.
     * |        |          |Note:
     * |        |          |1. The SMBALERT pin is an open-drain pin, the pull-high resistor is must in the system
     * |        |          |2. Software can write 1 to clear this bit.
     * |[4]     |SCTLDIN   |Bus Suspend or Control Signal Input Status
     * |        |          |0 = The input status of SUSCON pin is 0.
     * |        |          |1 = The input status of SUSCON pin is 1.
     * |[5]     |BUSTO     |Bus Time-out Status
     * |        |          |0 = Indicates that there is no any time-out or external clock time-out.
     * |        |          |1 = Indicates that a time-out or external clock time-out occurred.
     * |        |          |In bus busy, the bit indicates the total clock low time-out event occurred otherwise, it indicates the bus idle time-out event occurred.
     * |        |          |Note: Software can write 1 to clear this bit.
     * |[6]     |CLKTO     |Clock Low Cumulate Time-out Status
     * |        |          |0 = Indicates that the cumulative clock low is no any time-out.
     * |        |          |1 = Indicates that the cumulative clock low time-out occurred.
     * |        |          |Note: Software can write 1 to clear this bit.
     * |[7]     |PECDONE   |PEC Byte Transmission/Receive Done
     * |        |          |0 = Indicates the PEC transmission/ receive is not finished when the PECEN is set.
     * |        |          |1 = Indicates the PEC transmission/ receive is finished when the PECEN is set.
     * |        |          |Note: Software can write 1 to clear this bit.
     * @var I2C_T::PKTSIZE
     * Offset: 0x5C  I2C Packet Error Checking Byte Number Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |PLDSIZE   |Transfer Byte Number
     * |        |          |The transmission or receive byte number in one transaction when the PECEN is set
     * |        |          |The maximum transaction or receive byte is 256 Bytes.
     * |        |          |Notice: The byte number counting includes address, command code, and data frame.
     * @var I2C_T::PKTCRC
     * Offset: 0x60  I2C Packet Error Checking Byte Value Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |PECCRC    |Packet Error Checking Byte Value
     * |        |          |This byte indicates the packet error checking content after transmission or receive byte count by using the C(x) = X8 + X2 + X + 1
     * |        |          |It is read only.
     * @var I2C_T::BUSTOUT
     * Offset: 0x64  I2C Bus Management Timer Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |BUSTO     |Bus Management Time-out Value
     * |        |          |Indicate the bus time-out value in bus is IDLE or SCLK low.
     * |        |          |Note: If the user wants to revise the value of BUSTOUT, the TORSTEN (I2C_BUSTCTL[4]) bit shall be set to 1 and clear to 0 first in the BUSEN(I2C_BUSCTL[7]) is set.
     * @var I2C_T::CLKTOUT
     * Offset: 0x68  I2C Bus Management Clock Low Timer Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |CLKTO     |Bus Clock Low Timer
     * |        |          |The field is used to configure the cumulative clock extension time-out.
     * |        |          |Note: If the user wants to revise the value of CLKLTOUT, the TORSTEN bit shall be set to 1 and clear to 0 first in the BUSEN is set.
     */
    __IO uint32_t CTL0;                  /*!< [0x0000] I2C Control Register 0                                           */
    __IO uint32_t ADDR0;                 /*!< [0x0004] I2C Slave Address Register0                                      */
    __IO uint32_t DAT;                   /*!< [0x0008] I2C Data Register                                                */
    __I  uint32_t STATUS0;               /*!< [0x000c] I2C Status Register 0                                            */
    __IO uint32_t CLKDIV;                /*!< [0x0010] I2C Clock Divided Register                                       */
    __IO uint32_t TOCTL;                 /*!< [0x0014] I2C Time-out Control Register                                    */
    __IO uint32_t ADDR1;                 /*!< [0x0018] I2C Slave Address Register1                                      */
    __IO uint32_t ADDR2;                 /*!< [0x001c] I2C Slave Address Register2                                      */
    __IO uint32_t ADDR3;                 /*!< [0x0020] I2C Slave Address Register3                                      */
    __IO uint32_t ADDRMSK0;              /*!< [0x0024] I2C Slave Address Mask Register0                                 */
    __IO uint32_t ADDRMSK1;              /*!< [0x0028] I2C Slave Address Mask Register1                                 */
    __IO uint32_t ADDRMSK2;              /*!< [0x002c] I2C Slave Address Mask Register2                                 */
    __IO uint32_t ADDRMSK3;              /*!< [0x0030] I2C Slave Address Mask Register3                                 */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t WKCTL;                 /*!< [0x003c] I2C Wake-up Control Register                                     */
    __IO uint32_t WKSTS;                 /*!< [0x0040] I2C Wake-up Status Register                                      */
    __IO uint32_t CTL1;                  /*!< [0x0044] I2C Control Register 1                                           */
    __IO uint32_t STATUS1;               /*!< [0x0048] I2C Status Register 1                                            */
    __IO uint32_t TMCTL;                 /*!< [0x004c] I2C Timing Configure Control Register                            */
    __IO uint32_t BUSCTL;                /*!< [0x0050] I2C Bus Management Control Register                              */
    __IO uint32_t BUSTCTL;               /*!< [0x0054] I2C Bus Management Timer Control Register                        */
    __IO uint32_t BUSSTS;                /*!< [0x0058] I2C Bus Management Status Register                               */
    __IO uint32_t PKTSIZE;               /*!< [0x005c] I2C Packet Error Checking Byte Number Register                   */
    __I  uint32_t PKTCRC;                /*!< [0x0060] I2C Packet Error Checking Byte Value Register                    */
    __IO uint32_t BUSTOUT;               /*!< [0x0064] I2C Bus Management Timer Register                                */
    __IO uint32_t CLKTOUT;               /*!< [0x0068] I2C Bus Management Clock Low Timer Register                      */

} I2C_T;

/**
    @addtogroup I2C_CONST I2C Bit Field Definition
    Constant Definitions for I2C Controller
@{ */

#define I2C_CTL0_AA_Pos                  (2)                                               /*!< I2C_T::CTL: AA Position                */
#define I2C_CTL0_AA_Msk                  (0x1ul << I2C_CTL0_AA_Pos)                        /*!< I2C_T::CTL: AA Mask                    */

#define I2C_CTL0_SI_Pos                  (3)                                               /*!< I2C_T::CTL: SI Position                */
#define I2C_CTL0_SI_Msk                  (0x1ul << I2C_CTL0_SI_Pos)                        /*!< I2C_T::CTL: SI Mask                    */

#define I2C_CTL0_STO_Pos                 (4)                                               /*!< I2C_T::CTL: STO Position               */
#define I2C_CTL0_STO_Msk                 (0x1ul << I2C_CTL0_STO_Pos)                       /*!< I2C_T::CTL: STO Mask                   */

#define I2C_CTL0_STA_Pos                 (5)                                               /*!< I2C_T::CTL: STA Position               */
#define I2C_CTL0_STA_Msk                 (0x1ul << I2C_CTL0_STA_Pos)                       /*!< I2C_T::CTL: STA Mask                   */

#define I2C_CTL0_I2CEN_Pos               (6)                                               /*!< I2C_T::CTL: I2CEN Position             */
#define I2C_CTL0_I2CEN_Msk               (0x1ul << I2C_CTL0_I2CEN_Pos)                     /*!< I2C_T::CTL: I2CEN Mask                 */

#define I2C_CTL0_INTEN_Pos               (7)                                               /*!< I2C_T::CTL: INTEN Position             */
#define I2C_CTL0_INTEN_Msk               (0x1ul << I2C_CTL0_INTEN_Pos)                     /*!< I2C_T::CTL: INTEN Mask                 */

#define I2C_ADDR0_GC_Pos                 (0)                                               /*!< I2C_T::ADDR0: GC Position              */
#define I2C_ADDR0_GC_Msk                 (0x1ul << I2C_ADDR0_GC_Pos)                       /*!< I2C_T::ADDR0: GC Mask                  */

#define I2C_ADDR0_ADDR_Pos               (1)                                               /*!< I2C_T::ADDR0: ADDR Position            */
#define I2C_ADDR0_ADDR_Msk               (0x3fful << I2C_ADDR0_ADDR_Pos)                   /*!< I2C_T::ADDR0: ADDR Mask                */

#define I2C_DAT_DAT_Pos                  (0)                                               /*!< I2C_T::DAT: DAT Position               */
#define I2C_DAT_DAT_Msk                  (0xfful << I2C_DAT_DAT_Pos)                       /*!< I2C_T::DAT: DAT Mask                   */

#define I2C_STATUS0_STATUS_Pos           (0)                                               /*!< I2C_T::STATUS: STATUS Position         */
#define I2C_STATUS0_STATUS_Msk           (0xfful << I2C_STATUS_STATUS0_Pos)                /*!< I2C_T::STATUS: STATUS Mask             */

#define I2C_CLKDIV_DIVIDER_Pos           (0)                                               /*!< I2C_T::CLKDIV: DIVIDER Position        */
#define I2C_CLKDIV_DIVIDER_Msk           (0x3fful << I2C_CLKDIV_DIVIDER_Pos)               /*!< I2C_T::CLKDIV: DIVIDER Mask            */

#define I2C_TOCTL_TOIF_Pos               (0)                                               /*!< I2C_T::TOCTL: TOIF Position            */
#define I2C_TOCTL_TOIF_Msk               (0x1ul << I2C_TOCTL_TOIF_Pos)                     /*!< I2C_T::TOCTL: TOIF Mask                */

#define I2C_TOCTL_TOCDIV4_Pos            (1)                                               /*!< I2C_T::TOCTL: TOCDIV4 Position         */
#define I2C_TOCTL_TOCDIV4_Msk            (0x1ul << I2C_TOCTL_TOCDIV4_Pos)                  /*!< I2C_T::TOCTL: TOCDIV4 Mask             */

#define I2C_TOCTL_TOCEN_Pos              (2)                                               /*!< I2C_T::TOCTL: TOCEN Position           */
#define I2C_TOCTL_TOCEN_Msk              (0x1ul << I2C_TOCTL_TOCEN_Pos)                    /*!< I2C_T::TOCTL: TOCEN Mask               */

#define I2C_ADDR1_GC_Pos                 (0)                                               /*!< I2C_T::ADDR1: GC Position              */
#define I2C_ADDR1_GC_Msk                 (0x1ul << I2C_ADDR1_GC_Pos)                       /*!< I2C_T::ADDR1: GC Mask                  */

#define I2C_ADDR1_ADDR_Pos               (1)                                               /*!< I2C_T::ADDR1: ADDR Position            */
#define I2C_ADDR1_ADDR_Msk               (0x3fful << I2C_ADDR1_ADDR_Pos)                   /*!< I2C_T::ADDR1: ADDR Mask                */

#define I2C_ADDR2_GC_Pos                 (0)                                               /*!< I2C_T::ADDR2: GC Position              */
#define I2C_ADDR2_GC_Msk                 (0x1ul << I2C_ADDR2_GC_Pos)                       /*!< I2C_T::ADDR2: GC Mask                  */

#define I2C_ADDR2_ADDR_Pos               (1)                                               /*!< I2C_T::ADDR2: ADDR Position            */
#define I2C_ADDR2_ADDR_Msk               (0x3fful << I2C_ADDR2_ADDR_Pos)                   /*!< I2C_T::ADDR2: ADDR Mask                */

#define I2C_ADDR3_GC_Pos                 (0)                                               /*!< I2C_T::ADDR3: GC Position              */
#define I2C_ADDR3_GC_Msk                 (0x1ul << I2C_ADDR3_GC_Pos)                       /*!< I2C_T::ADDR3: GC Mask                  */

#define I2C_ADDR3_ADDR_Pos               (1)                                               /*!< I2C_T::ADDR3: ADDR Position            */
#define I2C_ADDR3_ADDR_Msk               (0x3fful << I2C_ADDR3_ADDR_Pos)                   /*!< I2C_T::ADDR3: ADDR Mask                */

#define I2C_ADDRMSK0_ADDRMSK_Pos         (1)                                               /*!< I2C_T::ADDRMSK0: ADDRMSK Position      */
#define I2C_ADDRMSK0_ADDRMSK_Msk         (0x3fful << I2C_ADDRMSK0_ADDRMSK_Pos)             /*!< I2C_T::ADDRMSK0: ADDRMSK Mask          */

#define I2C_ADDRMSK1_ADDRMSK_Pos         (1)                                               /*!< I2C_T::ADDRMSK1: ADDRMSK Position      */
#define I2C_ADDRMSK1_ADDRMSK_Msk         (0x3fful << I2C_ADDRMSK1_ADDRMSK_Pos)             /*!< I2C_T::ADDRMSK1: ADDRMSK Mask          */

#define I2C_ADDRMSK2_ADDRMSK_Pos         (1)                                               /*!< I2C_T::ADDRMSK2: ADDRMSK Position      */
#define I2C_ADDRMSK2_ADDRMSK_Msk         (0x3fful << I2C_ADDRMSK2_ADDRMSK_Pos)             /*!< I2C_T::ADDRMSK2: ADDRMSK Mask          */

#define I2C_ADDRMSK3_ADDRMSK_Pos         (1)                                               /*!< I2C_T::ADDRMSK3: ADDRMSK Position      */
#define I2C_ADDRMSK3_ADDRMSK_Msk         (0x3fful << I2C_ADDRMSK3_ADDRMSK_Pos)             /*!< I2C_T::ADDRMSK3: ADDRMSK Mask          */

#define I2C_WKCTL_WKEN_Pos               (0)                                               /*!< I2C_T::WKCTL: WKEN Position            */
#define I2C_WKCTL_WKEN_Msk               (0x1ul << I2C_WKCTL_WKEN_Pos)                     /*!< I2C_T::WKCTL: WKEN Mask                */

#define I2C_WKCTL_NHDBUSEN_Pos           (7)                                               /*!< I2C_T::WKCTL: NHDBUSEN Position        */
#define I2C_WKCTL_NHDBUSEN_Msk           (0x1ul << I2C_WKCTL_NHDBUSEN_Pos)                 /*!< I2C_T::WKCTL: NHDBUSEN Mask            */

#define I2C_WKSTS_WKIF_Pos               (0)                                               /*!< I2C_T::WKSTS: WKIF Position            */
#define I2C_WKSTS_WKIF_Msk               (0x1ul << I2C_WKSTS_WKIF_Pos)                     /*!< I2C_T::WKSTS: WKIF Mask                */

#define I2C_WKSTS_WKAKDONE_Pos           (1)                                               /*!< I2C_T::WKSTS: WKAKDONE Position        */
#define I2C_WKSTS_WKAKDONE_Msk           (0x1ul << I2C_WKSTS_WKAKDONE_Pos)                 /*!< I2C_T::WKSTS: WKAKDONE Mask            */

#define I2C_WKSTS_WRSTSWK_Pos            (2)                                               /*!< I2C_T::WKSTS: WRSTSWK Position         */
#define I2C_WKSTS_WRSTSWK_Msk            (0x1ul << I2C_WKSTS_WRSTSWK_Pos)                  /*!< I2C_T::WKSTS: WRSTSWK Mask             */

#define I2C_CTL1_TXPDMAEN_Pos            (0)                                               /*!< I2C_T::CTL1: TXPDMAEN Position         */
#define I2C_CTL1_TXPDMAEN_Msk            (0x1ul << I2C_CTL1_TXPDMAEN_Pos)                  /*!< I2C_T::CTL1: TXPDMAEN Mask             */

#define I2C_CTL1_RXPDMAEN_Pos            (1)                                               /*!< I2C_T::CTL1: RXPDMAEN Position         */
#define I2C_CTL1_RXPDMAEN_Msk            (0x1ul << I2C_CTL1_RXPDMAEN_Pos)                  /*!< I2C_T::CTL1: RXPDMAEN Mask             */

#define I2C_CTL1_PDMARST_Pos             (2)                                               /*!< I2C_T::CTL1: PDMARST Position          */
#define I2C_CTL1_PDMARST_Msk             (0x1ul << I2C_CTL1_PDMARST_Pos)                   /*!< I2C_T::CTL1: PDMARST Mask              */

#define I2C_CTL1_PDMASTR_Pos             (8)                                               /*!< I2C_T::CTL1: PDMASTR Position          */
#define I2C_CTL1_PDMASTR_Msk             (0x1ul << I2C_CTL1_PDMASTR_Pos)                   /*!< I2C_T::CTL1: PDMASTR Mask              */

#define I2C_CTL1_ADDR10EN_Pos            (9)                                               /*!< I2C_T::CTL1: ADDR10EN Position         */
#define I2C_CTL1_ADDR10EN_Msk            (0x1ul << I2C_CTL1_ADDR10EN_Pos)                  /*!< I2C_T::CTL1: ADDR10EN Mask             */

#define I2C_STATUS1_ADMAT0_Pos           (0)                                               /*!< I2C_T::STATUS1: ADMAT0 Position        */
#define I2C_STATUS1_ADMAT0_Msk           (0x1ul << I2C_STATUS1_ADMAT0_Pos)                 /*!< I2C_T::STATUS1: ADMAT0 Mask            */

#define I2C_STATUS1_ADMAT1_Pos           (1)                                               /*!< I2C_T::STATUS1: ADMAT1 Position        */
#define I2C_STATUS1_ADMAT1_Msk           (0x1ul << I2C_STATUS1_ADMAT1_Pos)                 /*!< I2C_T::STATUS1: ADMAT1 Mask            */

#define I2C_STATUS1_ADMAT2_Pos           (2)                                               /*!< I2C_T::STATUS1: ADMAT2 Position        */
#define I2C_STATUS1_ADMAT2_Msk           (0x1ul << I2C_STATUS1_ADMAT2_Pos)                 /*!< I2C_T::STATUS1: ADMAT2 Mask            */

#define I2C_STATUS1_ADMAT3_Pos           (3)                                               /*!< I2C_T::STATUS1: ADMAT3 Position        */
#define I2C_STATUS1_ADMAT3_Msk           (0x1ul << I2C_STATUS1_ADMAT3_Pos)                 /*!< I2C_T::STATUS1: ADMAT3 Mask            */

#define I2C_STATUS1_ONBUSY_Pos           (8)                                               /*!< I2C_T::STATUS1: ONBUSY Position        */
#define I2C_STATUS1_ONBUSY_Msk           (0x1ul << I2C_STATUS1_ONBUSY_Pos)                 /*!< I2C_T::STATUS1: ONBUSY Mask            */

#define I2C_TMCTL_STCTL_Pos              (0)                                               /*!< I2C_T::TMCTL: STCTL Position           */
#define I2C_TMCTL_STCTL_Msk              (0x1fful << I2C_TMCTL_STCTL_Pos)                  /*!< I2C_T::TMCTL: STCTL Mask               */

#define I2C_TMCTL_HTCTL_Pos              (16)                                              /*!< I2C_T::TMCTL: HTCTL Position           */
#define I2C_TMCTL_HTCTL_Msk              (0x1fful << I2C_TMCTL_HTCTL_Pos)                  /*!< I2C_T::TMCTL: HTCTL Mask               */

#define I2C_BUSCTL_ACKMEN_Pos            (0)                                               /*!< I2C_T::BUSCTL: ACKMEN Position         */
#define I2C_BUSCTL_ACKMEN_Msk            (0x1ul << I2C_BUSCTL_ACKMEN_Pos)                  /*!< I2C_T::BUSCTL: ACKMEN Mask             */

#define I2C_BUSCTL_PECEN_Pos             (1)                                               /*!< I2C_T::BUSCTL: PECEN Position          */
#define I2C_BUSCTL_PECEN_Msk             (0x1ul << I2C_BUSCTL_PECEN_Pos)                   /*!< I2C_T::BUSCTL: PECEN Mask              */

#define I2C_BUSCTL_BMDEN_Pos             (2)                                               /*!< I2C_T::BUSCTL: BMDEN Position          */
#define I2C_BUSCTL_BMDEN_Msk             (0x1ul << I2C_BUSCTL_BMDEN_Pos)                   /*!< I2C_T::BUSCTL: BMDEN Mask              */

#define I2C_BUSCTL_BMHEN_Pos             (3)                                               /*!< I2C_T::BUSCTL: BMHEN Position          */
#define I2C_BUSCTL_BMHEN_Msk             (0x1ul << I2C_BUSCTL_BMHEN_Pos)                   /*!< I2C_T::BUSCTL: BMHEN Mask              */

#define I2C_BUSCTL_ALERTEN_Pos           (4)                                               /*!< I2C_T::BUSCTL: ALERTEN Position        */
#define I2C_BUSCTL_ALERTEN_Msk           (0x1ul << I2C_BUSCTL_ALERTEN_Pos)                 /*!< I2C_T::BUSCTL: ALERTEN Mask            */

#define I2C_BUSCTL_SCTLOSTS_Pos          (5)                                               /*!< I2C_T::BUSCTL: SCTLOSTS Position       */
#define I2C_BUSCTL_SCTLOSTS_Msk          (0x1ul << I2C_BUSCTL_SCTLOSTS_Pos)                /*!< I2C_T::BUSCTL: SCTLOSTS Mask           */

#define I2C_BUSCTL_SCTLOEN_Pos           (6)                                               /*!< I2C_T::BUSCTL: SCTLOEN Position        */
#define I2C_BUSCTL_SCTLOEN_Msk           (0x1ul << I2C_BUSCTL_SCTLOEN_Pos)                 /*!< I2C_T::BUSCTL: SCTLOEN Mask            */

#define I2C_BUSCTL_BUSEN_Pos             (7)                                               /*!< I2C_T::BUSCTL: BUSEN Position          */
#define I2C_BUSCTL_BUSEN_Msk             (0x1ul << I2C_BUSCTL_BUSEN_Pos)                   /*!< I2C_T::BUSCTL: BUSEN Mask              */

#define I2C_BUSCTL_PECTXEN_Pos           (8)                                               /*!< I2C_T::BUSCTL: PECTXEN Position        */
#define I2C_BUSCTL_PECTXEN_Msk           (0x1ul << I2C_BUSCTL_PECTXEN_Pos)                 /*!< I2C_T::BUSCTL: PECTXEN Mask            */

#define I2C_BUSCTL_TIDLE_Pos             (9)                                               /*!< I2C_T::BUSCTL: TIDLE Position          */
#define I2C_BUSCTL_TIDLE_Msk             (0x1ul << I2C_BUSCTL_TIDLE_Pos)                   /*!< I2C_T::BUSCTL: TIDLE Mask              */

#define I2C_BUSCTL_PECCLR_Pos            (10)                                              /*!< I2C_T::BUSCTL: PECCLR Position         */
#define I2C_BUSCTL_PECCLR_Msk            (0x1ul << I2C_BUSCTL_PECCLR_Pos)                  /*!< I2C_T::BUSCTL: PECCLR Mask             */

#define I2C_BUSCTL_ACKM9SI_Pos           (11)                                              /*!< I2C_T::BUSCTL: ACKM9SI Position        */
#define I2C_BUSCTL_ACKM9SI_Msk           (0x1ul << I2C_BUSCTL_ACKM9SI_Pos)                 /*!< I2C_T::BUSCTL: ACKM9SI Mask            */

#define I2C_BUSCTL_BCDIEN_Pos            (12)                                              /*!< I2C_T::BUSCTL: BCDIEN Position         */
#define I2C_BUSCTL_BCDIEN_Msk            (0x1ul << I2C_BUSCTL_BCDIEN_Pos)                  /*!< I2C_T::BUSCTL: BCDIEN Mask             */

#define I2C_BUSCTL_PECDIEN_Pos           (13)                                              /*!< I2C_T::BUSCTL: PECDIEN Position        */
#define I2C_BUSCTL_PECDIEN_Msk           (0x1ul << I2C_BUSCTL_PECDIEN_Pos)                 /*!< I2C_T::BUSCTL: PECDIEN Mask            */

#define I2C_BUSTCTL_BUSTOEN_Pos          (0)                                               /*!< I2C_T::BUSTCTL: BUSTOEN Position       */
#define I2C_BUSTCTL_BUSTOEN_Msk          (0x1ul << I2C_BUSTCTL_BUSTOEN_Pos)                /*!< I2C_T::BUSTCTL: BUSTOEN Mask           */

#define I2C_BUSTCTL_CLKTOEN_Pos          (1)                                               /*!< I2C_T::BUSTCTL: CLKTOEN Position       */
#define I2C_BUSTCTL_CLKTOEN_Msk          (0x1ul << I2C_BUSTCTL_CLKTOEN_Pos)                /*!< I2C_T::BUSTCTL: CLKTOEN Mask           */

#define I2C_BUSTCTL_BUSTOIEN_Pos         (2)                                               /*!< I2C_T::BUSTCTL: BUSTOIEN Position      */
#define I2C_BUSTCTL_BUSTOIEN_Msk         (0x1ul << I2C_BUSTCTL_BUSTOIEN_Pos)               /*!< I2C_T::BUSTCTL: BUSTOIEN Mask          */

#define I2C_BUSTCTL_CLKTOIEN_Pos         (3)                                               /*!< I2C_T::BUSTCTL: CLKTOIEN Position      */
#define I2C_BUSTCTL_CLKTOIEN_Msk         (0x1ul << I2C_BUSTCTL_CLKTOIEN_Pos)               /*!< I2C_T::BUSTCTL: CLKTOIEN Mask          */

#define I2C_BUSTCTL_TORSTEN_Pos          (4)                                               /*!< I2C_T::BUSTCTL: TORSTEN Position       */
#define I2C_BUSTCTL_TORSTEN_Msk          (0x1ul << I2C_BUSTCTL_TORSTEN_Pos)                /*!< I2C_T::BUSTCTL: TORSTEN Mask           */

#define I2C_BUSSTS_BUSY_Pos              (0)                                               /*!< I2C_T::BUSSTS: BUSY Position           */
#define I2C_BUSSTS_BUSY_Msk              (0x1ul << I2C_BUSSTS_BUSY_Pos)                    /*!< I2C_T::BUSSTS: BUSY Mask               */

#define I2C_BUSSTS_BCDONE_Pos            (1)                                               /*!< I2C_T::BUSSTS: BCDONE Position         */
#define I2C_BUSSTS_BCDONE_Msk            (0x1ul << I2C_BUSSTS_BCDONE_Pos)                  /*!< I2C_T::BUSSTS: BCDONE Mask             */

#define I2C_BUSSTS_PECERR_Pos            (2)                                               /*!< I2C_T::BUSSTS: PECERR Position         */
#define I2C_BUSSTS_PECERR_Msk            (0x1ul << I2C_BUSSTS_PECERR_Pos)                  /*!< I2C_T::BUSSTS: PECERR Mask             */

#define I2C_BUSSTS_ALERT_Pos             (3)                                               /*!< I2C_T::BUSSTS: ALERT Position          */
#define I2C_BUSSTS_ALERT_Msk             (0x1ul << I2C_BUSSTS_ALERT_Pos)                   /*!< I2C_T::BUSSTS: ALERT Mask              */

#define I2C_BUSSTS_SCTLDIN_Pos           (4)                                               /*!< I2C_T::BUSSTS: SCTLDIN Position        */
#define I2C_BUSSTS_SCTLDIN_Msk           (0x1ul << I2C_BUSSTS_SCTLDIN_Pos)                 /*!< I2C_T::BUSSTS: SCTLDIN Mask            */

#define I2C_BUSSTS_BUSTO_Pos             (5)                                               /*!< I2C_T::BUSSTS: BUSTO Position          */
#define I2C_BUSSTS_BUSTO_Msk             (0x1ul << I2C_BUSSTS_BUSTO_Pos)                   /*!< I2C_T::BUSSTS: BUSTO Mask              */

#define I2C_BUSSTS_CLKTO_Pos             (6)                                               /*!< I2C_T::BUSSTS: CLKTO Position          */
#define I2C_BUSSTS_CLKTO_Msk             (0x1ul << I2C_BUSSTS_CLKTO_Pos)                   /*!< I2C_T::BUSSTS: CLKTO Mask              */

#define I2C_BUSSTS_PECDONE_Pos           (7)                                               /*!< I2C_T::BUSSTS: PECDONE Position        */
#define I2C_BUSSTS_PECDONE_Msk           (0x1ul << I2C_BUSSTS_PECDONE_Pos)                 /*!< I2C_T::BUSSTS: PECDONE Mask            */

#define I2C_PKTSIZE_PLDSIZE_Pos          (0)                                               /*!< I2C_T::PKTSIZE: PLDSIZE Position       */
#define I2C_PKTSIZE_PLDSIZE_Msk          (0x1fful << I2C_PKTSIZE_PLDSIZE_Pos)              /*!< I2C_T::PKTSIZE: PLDSIZE Mask           */

#define I2C_PKTCRC_PECCRC_Pos            (0)                                               /*!< I2C_T::PKTCRC: PECCRC Position         */
#define I2C_PKTCRC_PECCRC_Msk            (0xfful << I2C_PKTCRC_PECCRC_Pos)                 /*!< I2C_T::PKTCRC: PECCRC Mask             */

#define I2C_BUSTOUT_BUSTO_Pos            (0)                                               /*!< I2C_T::BUSTOUT: BUSTO Position         */
#define I2C_BUSTOUT_BUSTO_Msk            (0xfful << I2C_BUSTOUT_BUSTO_Pos)                 /*!< I2C_T::BUSTOUT: BUSTO Mask             */

#define I2C_CLKTOUT_CLKTO_Pos            (0)                                               /*!< I2C_T::CLKTOUT: CLKTO Position         */
#define I2C_CLKTOUT_CLKTO_Msk            (0xfful << I2C_CLKTOUT_CLKTO_Pos)                 /*!< I2C_T::CLKTOUT: CLKTO Mask             */

/**@}*/ /* I2C_CONST */
/**@}*/ /* end of I2C register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __I2C_REG_H__ */
