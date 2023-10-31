/**************************************************************************//**
 * @file     i2s_reg.h
 * @version  V1.00
 * @brief    I2S register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __I2S_REG_H__
#define __I2S_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup I2S I2S Interface Controller(I2S)
    Memory Mapped Structure for I2S Controller
@{ */

typedef struct
{


    /**
     * @var I2S_T::CTL0
     * Offset: 0x00  I2S Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |I2SEN     |I2S Controller Enable Control
     * |        |          |0 = I2S controller Disabled.
     * |        |          |1 = I2S controller Enabled.
     * |[1]     |TXEN      |Transmit Enable Control
     * |        |          |0 = Data transmission Disabled.
     * |        |          |1 = Data transmission Enabled.
     * |[2]     |RXEN      |Receive Enable Control
     * |        |          |0 = Data receiving Disabled.
     * |        |          |1 = Data receiving Enabled.
     * |[3]     |MUTE      |Transmit Mute Enable Control
     * |        |          |0 = Transmit data is shifted from buffer.
     * |        |          |1 = Send zero on transmit channel.
     * |[5:4]   |DATWIDTH  |Data Width
     * |        |          |This bit field is used to define the bit-width of data word in each audio channel
     * |        |          |00 = The bit-width of data word is 8-bit.
     * |        |          |01 = The bit-width of data word is 16-bit.
     * |        |          |10 = The bit-width of data word is 24-bit.
     * |        |          |11 = The bit-width of data word is 32-bit.
     * |[6]     |MONO      |Monaural Data Control
     * |        |          |0 = Data is stereo format.
     * |        |          |1 = Data is monaural format.
     * |        |          |Note: when chip records data, RXLCH (I2S_CTL0[23]) indicates which channel data will be saved if monaural format is selected.
     * |[7]     |ORDER     |Stereo Data Order in FIFO
     * |        |          |In 8-bit/16-bit data width, this bit is used to select whether the even or odd channel data is stored in higher byte
     * |        |          |In 24-bit data width, this is used to select the left/right alignment method of audio data which is stored in data memory consisted of 32-bit FIFO entries.
     * |        |          |0 = Even channel data at high byte in 8-bit/16-bit data width.
     * |        |          |LSB of 24-bit audio data in each channel is aligned to right side in 32-bit FIFO entries.
     * |        |          |1 = Even channel data at low byte.
     * |        |          | MSB of 24-bit audio data in each channel is aligned to left side in 32-bit FIFO entries.
     * |[8]     |SLAVE     |Slave Mode Enable Control
     * |        |          |0 = Master mode.
     * |        |          |1 = Slave mode.
     * |        |          |Note: I2S can operate as master or slave
     * |        |          |For Master mode, I2S_BCLK and I2S_LRCLK pins are output mode and send out bit clock to Audio CODEC chip
     * |        |          |In Slave mode, I2S_BCLK and I2S_LRCLK pins are input mode and I2S_BCLK and I2S_LRCLK signals are received from outer Audio CODEC chip.
     * |[15]    |MCLKEN    |Master Clock Enable Control
     * |        |          |If MCLKEN is set to 1, I2S controller will generate master clock on I2S_MCLK pin for external audio devices.
     * |        |          |0 = Master clock Disabled.
     * |        |          |1 = Master clock Enabled.
     * |[18]    |TXFBCLR   |Transmit FIFO Buffer Clear
     * |        |          |0 = No Effect.
     * |        |          |1 = Clear TX FIFO.
     * |        |          |Note1: Write 1 to clear transmit FIFO, internal pointer is reset to FIFO start point, and TXCNT (I2S_STATUS1[12:8]) returns 0 and transmit FIFO becomes empty but data in transmit FIFO is not changed.
     * |        |          |Note2: This bit is clear by hardware automatically, read it return zero.
     * |[19]    |RXFBCLR   |Receive FIFO Buffer Clear
     * |        |          |0 = No Effect.
     * |        |          |1 = Clear RX FIFO.
     * |        |          |Note1: Write 1 to clear receive FIFO, internal pointer is reset to FIFO start point, and RXCNT (I2S_STATUS1[20:16]) returns 0 and receive FIFO becomes empty.
     * |        |          |Note2: This bit is cleared by hardware automatically, read it return zero.
     * |[20]    |TXPDMAEN  |Transmit PDMA Enable Control
     * |        |          |0 = Transmit PDMA function Disabled.
     * |        |          |1 = Transmit PDMA function Enabled.
     * |[21]    |RXPDMAEN  |Receive PDMA Enable Control
     * |        |          |0 = Receiver PDMA function Disabled.
     * |        |          |1 = Receiver PDMA function Enabled.
     * |[23]    |RXLCH     |Receive Left Channel Enable Control
     * |        |          |When monaural format is selected (MONO = 1), I2S will receive channel1 data if RXLCH is set to 0, and receive channel0 data if RXLCH is set to 1.
     * |        |          |0 = Receives channel1 data in MONO mode.
     * |        |          |1 = Receives channel0 data in MONO mode.
     * |[26:24] |FORMAT    |Data Format Selection
     * |        |          |000 = I2S standard data format.
     * |        |          |001 = I2S with MSB justified.
     * |        |          |010 = I2S with LSB justified.
     * |        |          |011 = Reserved.
     * |        |          |100 = PCM standard data format.
     * |        |          |101 = PCM with MSB justified.
     * |        |          |110 = PCM with LSB justified.
     * |        |          |111 = Reserved.
     * |[27]    |PCMSYNC   |PCM Synchronization Pulse Length Selection
     * |        |          |This bit field is used to select the high pulse length of frame synchronization signal in PCM protocol
     * |        |          |0 = One BCLK period.
     * |        |          |1 = One channel period.
     * |        |          |Note: This bit is only available in master mode
     * |[29:28] |CHWIDTH   |Channel Width
     * |        |          |This bit fields are used to define the length of audio channel
     * |        |          |If CHWIDTH < DATWIDTH, the hardware will set the real channel length as the bit-width of audio data which is defined by DATWIDTH.
     * |        |          |00 = The bit-width of each audio channel is 8-bit.
     * |        |          |01 = The bit-width of each audio channel is 16-bit.
     * |        |          |10 = The bit-width of each audio channel is 24-bit.
     * |        |          |11 = The bit-width of each audio channel is 32-bit.
     * |[31:30] |TDMCHNUM  |TDM Channel Number
     * |        |          |This bit fields are used to define the TDM channel number in one audio frame while PCM mode (FORMAT[2] = 1).
     * |        |          |00 = 2 channels in audio frame.
     * |        |          |01 = 4 channels in audio frame.
     * |        |          |10 = 6 channels in audio frame.
     * |        |          |11 = 8 channels in audio frame.
     * @var I2S_T::CLKDIV
     * Offset: 0x04  I2S Clock Divider Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[5:0]   |MCLKDIV   |Master Clock Divider
     * |        |          |If chip external crystal frequency is (2xMCLKDIV)*256fs then software can program these bits to generate 256fs clock frequency to audio codec chip
     * |        |          |If MCLKDIV is set to 0, MCLK is the same as external clock input.
     * |        |          |For example, sampling rate is 24 kHz and chip external crystal clock is 12.288 MHz, set MCLKDIV = 1.
     * |        |          |F_MCLK = F_I2SCLK/(2x(MCLKDIV)) (When MCLKDIV is >= 1 ).
     * |        |          |F_MCLK = F_I2SCLK (When MCLKDIV is set to 0 ).
     * |        |          |Note: F_MCLK is the frequency of MCLK, and F_I2SCLK is the frequency of the I2S_CLK
     * |[16:8]  |BCLKDIV   |Bit Clock Divider
     * |        |          |The I2S controller will generate bit clock in Master mode
     * |        |          |Software can program these bit fields to generate sampling rate clock frequency.
     * |        |          |F_BCLK= F_I2SCLK / (2*(BCLKDIV + 1)).
     * |        |          |Note: F_BCLK is the frequency of BCLK and F_I2SCLK is the frequency of I2S_CLK
     * @var I2S_T::IEN
     * Offset: 0x08  I2S Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RXUDFIEN  |Receive FIFO Underflow Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: If software reads receive FIFO when it is empty then RXUDIF (I2S_STATUS0[8]) flag is set to 1.
     * |[1]     |RXOVFIEN  |Receive FIFO Overflow Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: Interrupt occurs if this bit is set to 1 and RXOVIF (I2S_STATUS0[9]) flag is set to 1
     * |[2]     |RXTHIEN   |Receive FIFO Threshold Level Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: When data word in receive FIFO is equal or higher than RXTH (I2S_CTL1[19:16]) and the RXTHIF (I2S_STATUS0[10]) bit is set to 1
     * |        |          |If RXTHIEN bit is enabled, interrupt occur.
     * |[8]     |TXUDFIEN  |Transmit FIFO Underflow Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: Interrupt occur if this bit is set to 1 and TXUDIF (I2S_STATUS0[16]) flag is set to 1.
     * |[9]     |TXOVFIEN  |Transmit FIFO Overflow Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: Interrupt occurs if this bit is set to 1 and TXOVIF (I2S_STATUS0[17]) flag is set to 1
     * |[10]    |TXTHIEN   |Transmit FIFO Threshold Level Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: Interrupt occurs if this bit is set to 1 and data words in transmit FIFO is less than TXTH (I2S_CTL1[11:8]).
     * |[16]    |CH0ZCIEN  |Channel0 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel0 zero-cross
     * |        |          |Note2: Channel0 also means left audio channel while I2S (FORMAT[2]=0) or 2-channel PCM mode.
     * |[17]    |CH1ZCIEN  |Channel1 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel1 zero-cross
     * |        |          |Note2: Channel1 also means right audio channel while I2S (FORMAT[2]=0) or 2-channel PCM mode.
     * |[18]    |CH2ZCIEN  |Channel2 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel2 zero-cross
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[19]    |CH3ZCIEN  |Channel3 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel3 zero-cross
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[20]    |CH4ZCIEN  |Channel4 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel4 zero-cross
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[21]    |CH5ZCIEN  |Channel5 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel5 zero-cross
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[22]    |CH6ZCIEN  |Channel6 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel6 zero-cross
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[23]    |CH7ZCIEN  |Channel7 Zero-cross Interrupt Enable Control
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note1: Interrupt occurs if this bit is set to 1 and channel7 zero-cross
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * @var I2S_T::STATUS0
     * Offset: 0x0C  I2S Status Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |I2SINT    |I2S Interrupt Flag (Read Only)
     * |        |          |0 = No I2S interrupt.
     * |        |          |1 = I2S interrupt.
     * |        |          |Note: It is wire-OR of I2STXINT and I2SRXINT bits.
     * |[1]     |I2SRXINT  |I2S Receive Interrupt (Read Only)
     * |        |          |0 = No receive interrupt.
     * |        |          |1 = Receive interrupt.
     * |[2]     |I2STXINT  |I2S Transmit Interrupt (Read Only)
     * |        |          |0 = No transmit interrupt.
     * |        |          |1 = Transmit interrupt.
     * |[5:3]   |DATACH    |Transmission Data Channel (Read Only)
     * |        |          |This bit fields are used to indicate which audio channel is current transmit data belong.
     * |        |          |000 = channel0 (means left channel while 2-channel I2S/PCM mode).
     * |        |          |001 = channel1 (means right channel while 2-channel I2S/PCM mode).
     * |        |          |010 = channel2 (available while 4-channel TDM PCM mode).
     * |        |          |011 = channel3 (available while 4-channel TDM PCM mode).
     * |        |          |100 = channel4 (available while 6-channel TDM PCM mode).
     * |        |          |101 = channel5 (available while 6-channel TDM PCM mode).
     * |        |          |110 = channel6 (available while 8-channel TDM PCM mode).
     * |        |          |111 = channel7 (available while 8-channel TDM PCM mode).
     * |[8]     |RXUDIF    |Receive FIFO Underflow Interrupt Flag
     * |        |          |0 = No underflow occur.
     * |        |          |1 = Underflow occur.
     * |        |          |Note1: When receive FIFO is empty, and software reads the receive FIFO again
     * |        |          |This bit will be set to 1, and it indicates underflow situation occurs.
     * |        |          |Note2: Write 1 to clear this bit to zero
     * |[9]     |RXOVIF    |Receive FIFO Overflow Interrupt Flag
     * |        |          |0 = No overflow occur.
     * |        |          |1 = Overflow occur.
     * |        |          |Note1: When receive FIFO is full and receive hardware attempt to write data into receive FIFO then this bit is set to 1, data in 1st buffer is overwrote.
     * |        |          |Note2: Write 1 to clear this bit to 0.
     * |[10]    |RXTHIF    |Receive FIFO Threshold Interrupt Flag (Read Only)
     * |        |          |0 = Data word(s) in FIFO is not higher than threshold level.
     * |        |          |1 = Data word(s) in FIFO is higher than threshold level.
     * |        |          |Note: When data word(s) in receive FIFO is higher than threshold value set in RXTH (I2S_CTL1[19:16]) the RXTHIF bit becomes to 1
     * |        |          |It keeps at 1 till RXCNT (I2S_STATUS1[20:16]) is not higher than RXTH (I2S_CTL1[19:16]) after software read RXFIFO register.
     * |[11]    |RXFULL    |Receive FIFO Full (Read Only)
     * |        |          |0 = Not full.
     * |        |          |1 = Full.
     * |        |          |Note: This bit reflects data words number in receive FIFO is 16.
     * |[12]    |RXEMPTY   |Receive FIFO Empty (Read Only)
     * |        |          |0 = Not empty.
     * |        |          |1 = Empty.
     * |        |          |Note: This bit reflects data words number in receive FIFO is zero
     * |[16]    |TXUDIF    |Transmit FIFO Underflow Interrupt Flag
     * |        |          |0 = No underflow.
     * |        |          |1 = Underflow.
     * |        |          |Note1: This bit will be set to 1 when shift logic hardware read data from transmitting FIFO and the filling data level in transmitting FIFO is not enough for one audio frame.
     * |        |          |Note2: Write 1 to clear this bit to 0.
     * |[17]    |TXOVIF    |Transmit FIFO Overflow Interrupt Flag
     * |        |          |0 = No overflow.
     * |        |          |1 = Overflow.
     * |        |          |Note1: Write data to transmit FIFO when it is full and this bit set to 1
     * |        |          |Note2: Write 1 to clear this bit to 0.
     * |[18]    |TXTHIF    |Transmit FIFO Threshold Interrupt Flag (Read Only)
     * |        |          |0 = Data word(s) in FIFO is higher than threshold level.
     * |        |          |1 = Data word(s) in FIFO is equal or lower than threshold level.
     * |        |          |Note: When data word(s) in transmit FIFO is equal or lower than threshold value set in TXTH (I2S_CTL1[11:8]) the TXTHIF bit becomes to 1
     * |        |          |It keeps at 1 till TXCNT (I2S_STATUS1[12:8]) is higher than TXTH (I2S_CTL1[11:8]) after software write TXFIFO register.
     * |[19]    |TXFULL    |Transmit FIFO Full (Read Only)
     * |        |          |This bit reflect data word number in transmit FIFO is 16
     * |        |          |0 = Not full.
     * |        |          |1 = Full.
     * |[20]    |TXEMPTY   |Transmit FIFO Empty (Read Only)
     * |        |          |This bit reflect data word number in transmit FIFO is zero
     * |        |          |0 = Not empty.
     * |        |          |1 = Empty.
     * |[21]    |TXBUSY    |Transmit Busy (Read Only)
     * |        |          |0 = Transmit shift buffer is empty.
     * |        |          |1 = Transmit shift buffer is busy.
     * |        |          |Note: This bit is cleared to 0 when all data in transmit FIFO and shift buffer is shifted out
     * |        |          |And set to 1 when 1st data is load to shift buffer
     * @var I2S_T::TXFIFO
     * Offset: 0x10  I2S Transmit FIFO Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TXFIFO    |Transmit FIFO Bits
     * |        |          |I2S contains 16 words (16x32 bit) data buffer for data transmit
     * |        |          |Write data to this register to prepare data for transmit
     * |        |          |The remaining word number is indicated by TXCNT (I2S_STATUS1[12:8]).
     * @var I2S_T::RXFIFO
     * Offset: 0x14  I2S Receive FIFO Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |RXFIFO    |Receive FIFO Bits
     * |        |          |I2S contains 16 words (16x32 bit) data buffer for data receive
     * |        |          |Read this register to get data in FIFO
     * |        |          |The remaining data word number is indicated by RXCNT (I2S_STATUS1[20:16]).
     * @var I2S_T::CTL1
     * Offset: 0x20  I2S Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CH0ZCEN   |Channel0 Zero-cross Detection Enable Control
     * |        |          |0 = channel0 zero-cross detect Disabled.
     * |        |          |1 = channel0 zero-cross detect Enabled.
     * |        |          |Note1: Channel0 also means left audio channel while I2S (FORMAT[2]=0) or 2-channel PCM mode.
     * |        |          |Note2: If this bit is set to 1, when channel0 data sign bit change or next shift data bits are all zero then CH0ZCIF(I2S_STATUS1[0]) flag is set to 1.
     * |        |          |Note3: If CH0ZCIF Flag is set to 1, the channel0 will be mute.
     * |[1]     |CH1ZCEN   |Channel1 Zero-cross Detect Enable Control
     * |        |          |0 = channel1 zero-cross detect Disabled.
     * |        |          |1 = channel1 zero-cross detect Enabled.
     * |        |          |Note1: Channel1 also means right audio channel while I2S (FORMAT[2]=0) or 2-channel PCM mode.
     * |        |          |Note2: If this bit is set to 1, when channel1 data sign bit change or next shift data bits are all zero then CH1ZCIF(I2S_STATUS1[1]) flag is set to 1.
     * |        |          |Note3: If CH1ZCIF Flag is set to 1, the channel1 will be mute.
     * |[2]     |CH2ZCEN   |Channel2 Zero-cross Detect Enable Control
     * |        |          |0 = channel2 zero-cross detect Disabled.
     * |        |          |1 = channel2 zero-cross detect Enabled.
     * |        |          |Note1: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |        |          |Note2: If this bit is set to 1, when channel2 data sign bit change or next shift data bits are all zero then CH2ZCIF(I2S_STATUS1[2]) flag is set to 1.
     * |        |          |Note3: If CH2ZCIF Flag is set to 1, the channel2 will be mute.
     * |[3]     |CH3ZCEN   |Channel3 Zero-cross Detect Enable Control
     * |        |          |0 = channel3 zero-cross detect Disabled.
     * |        |          |1 = channel3 zero-cross detect Enabled.
     * |        |          |Note1: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |        |          |Note2: If this bit is set to 1, when channel3 data sign bit change or next shift data bits are all zero then CH3ZCIF(I2S_STATUS1[3]) flag is set to 1.
     * |        |          |Note3: If CH3ZCIF Flag is set to 1, the channel3 will be mute.
     * |[4]     |CH4ZCEN   |Channel4 Zero-cross Detect Enable Control
     * |        |          |0 = channel4 zero-cross detect Disabled.
     * |        |          |1 = channel4 zero-cross detect Enabled.
     * |        |          |Note1: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |        |          |Note2: If this bit is set to 1, when channel4 data sign bit change or next shift data bits are all zero then CH4ZCIF(I2S_STATUS1[4]) flag is set to 1.
     * |        |          |Note3: If CH4ZCIF Flag is set to 1, the channel4 will be mute.
     * |[5]     |CH5ZCEN   |Channel5 Zero-cross Detect Enable Control
     * |        |          |0 = channel5 zero-cross detect Disabled.
     * |        |          |1 = channel5 zero-cross detect Enabled.
     * |        |          |Note1: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |        |          |Note2: If this bit is set to 1, when channel5 data sign bit change or next shift data bits are all zero then CH5ZCIF(I2S_STATUS1[5]) flag is set to 1.
     * |        |          |Note3: If CH5ZCIF Flag is set to 1, the channel5 will be mute.
     * |[6]     |CH6ZCEN   |Channel6 Zero-cross Detect Enable Control
     * |        |          |0 = channel6 zero-cross detect Disabled.
     * |        |          |1 = channel6 zero-cross detect Enabled.
     * |        |          |Note1: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |        |          |Note2: If this bit is set to 1, when channel6 data sign bit change or next shift data bits are all zero then CH6ZCIF(I2S_STATUS1[6]) flag is set to 1.
     * |        |          |Note3: If CH6ZCIF Flag is set to 1, the channel6 will be mute.
     * |[7]     |CH7ZCEN   |Channel7 Zero-cross Detect Enable Control
     * |        |          |0 = channel7 zero-cross detect Disabled.
     * |        |          |1 = channel7 zero-cross detect Enabled.
     * |        |          |Note1: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |        |          |Note2: If this bit is set to 1, when channel7 data sign bit change or next shift data bits are all zero then CH7ZCIF (I2S_STATUS1[7]) flag is set to 1.
     * |        |          |Note3: If CH7ZCIF Flag is set to 1, the channel7 will be mute.
     * |[11:8]  |TXTH      |Transmit FIFO Threshold Level
     * |        |          |0000 = 0 data word in transmit FIFO.
     * |        |          |0001 = 1 data word in transmit FIFO.
     * |        |          |0010 = 2 data words in transmit FIFO.
     * |        |          |...
     * |        |          |1110 = 14 data words in transmit FIFO.
     * |        |          |1111 = 15 data words in transmit FIFO.
     * |        |          |Note: If remain data word number in transmit FIFO is the same or less than threshold level then TXTHIF (I2S_STATUS0[18]) flag is set.
     * |[19:16] |RXTH      |Receive FIFO Threshold Level
     * |        |          |0000 = 1 data word in receive FIFO.
     * |        |          |0001 = 2 data words in receive FIFO.
     * |        |          |0010 = 3 data words in receive FIFO.
     * |        |          |...
     * |        |          |1110 = 15 data words in receive FIFO.
     * |        |          |1111 = 16 data words in receive FIFO.
     * |        |          |Note: When received data word number in receive buffer is greater than threshold level then RXTHIF (I2S_STATUS0[10]) flag is set.
     * |[24]    |PBWIDTH   |Peripheral Bus Data Width Selection
     * |        |          |This bit is used to choice the available data width of APB bus
     * |        |          |It must be set to 1 while PDMA function is enable and it is set to 16-bit transmission mode
     * |        |          |0 = 32 bits data width.
     * |        |          |1 = 16 bits data width.
     * |        |          |Note1: If PBWIDTH=1, the low 16 bits of 32-bit data bus are available.
     * |        |          |Note2: If PBWIDTH=1, the transmitting FIFO level will be increased after two FIFO write operations.
     * |        |          |Note3: If PBWIDTH=1, the receiving FIFO level will be decreased after two FIFO read operations.
     * |[25]    |PB16ORD   |FIFO Read/Write Order in 16-bit Width of Peripheral Bus
     * |        |          |When PBWIDTH = 1, the data FIFO will be increased or decreased by two peripheral bus access
     * |        |          |This bit is used to select the order of FIFO access operations to meet the 32-bit transmitting/receiving FIFO entries.
     * |        |          |0 = Low 16-bit read/write access first.
     * |        |          |1 = High 16-bit read/write access first.
     * |        |          |Note: This bit is available while PBWIDTH = 1.
     * @var I2S_T::STATUS1
     * Offset: 0x24  I2S Status Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CH0ZCIF   |Channel0 Zero-cross Interrupt Flag
     * |        |          |It indicates channel0 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel0.
     * |        |          |1 = Channel0 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: Channel0 also means left audio channel while I2S (FORMAT[2]=0) or 2-channel PCM mode.
     * |[1]     |CH1ZCIF   |Channel1 Zero-cross Interrupt Flag
     * |        |          |It indicates channel1 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel1.
     * |        |          |1 = Channel1 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: Channel1 also means right audio channel while I2S (FORMAT[2]=0) or 2-channel PCM mode.
     * |[2]     |CH2ZCIF   |Channel2 Zero-cross Interrupt Flag
     * |        |          |It indicates channel2 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel2.
     * |        |          |1 = Channel2 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[3]     |CH3ZCIF   |Channel3 Zero-cross Interrupt Flag
     * |        |          |It indicates channel3 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel3.
     * |        |          |1 = Channel3 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[4]     |CH4ZCIF   |Channel4 Zero-cross Interrupt Flag
     * |        |          |It indicates channel4 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel4.
     * |        |          |1 = Channel4 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[5]     |CH5ZCIF   |Channel5 Zero-cross Interrupt Flag
     * |        |          |It indicates channel5 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel5.
     * |        |          |1 = Channel5 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[6]     |CH6ZCIF   |Channel6 Zero-cross Interrupt Flag
     * |        |          |It indicates channel6 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel6.
     * |        |          |1 = Channel6 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[7]     |CH7ZCIF   |Channel7 Zero-cross Interrupt Flag
     * |        |          |It indicates channel7 next sample data sign bit is changed or all data bits are zero.
     * |        |          |0 = No zero-cross in channel7.
     * |        |          |1 = Channel7 zero-cross is detected.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: This bit is available while multi-channel PCM mode and TDMCHNUM (I2S_CTL0[31:30]) = 0x1, 0x2, 0x3.
     * |[12:8]  |TXCNT     |Transmit FIFO Level (Read Only)
     * |        |          |These bits indicate the number of available entries in transmit FIFO
     * |        |          |00000 = No data.
     * |        |          |00001 = 1 word in transmit FIFO.
     * |        |          |00010 = 2 words in transmit FIFO.
     * |        |          |...
     * |        |          |01110 = 14 words in transmit FIFO.
     * |        |          |01111 = 15 words in transmit FIFO.
     * |        |          |10000 = 16 words in transmit FIFO.
     * |        |          |Others are reserved.
     * |[20:16] |RXCNT     |Receive FIFO Level (Read Only)
     * |        |          |These bits indicate the number of available entries in receive FIFO
     * |        |          |00000 = No data.
     * |        |          |00001 = 1 word in receive FIFO.
     * |        |          |00010 = 2 words in receive FIFO.
     * |        |          |...
     * |        |          |01110 = 14 words in receive FIFO.
     * |        |          |01111 = 15 words in receive FIFO.
     * |        |          |10000 = 16 words in receive FIFO.
     * |        |          |Others are reserved.
     */
    __IO uint32_t CTL0;                  /*!< [0x0000] I2S Control Register 0                                           */
    __IO uint32_t CLKDIV;                /*!< [0x0004] I2S Clock Divider Register                                       */
    __IO uint32_t IEN;                   /*!< [0x0008] I2S Interrupt Enable Register                                    */
    __IO uint32_t STATUS0;               /*!< [0x000c] I2S Status Register 0                                            */
    __O  uint32_t TXFIFO;                /*!< [0x0010] I2S Transmit FIFO Register                                       */
    __I  uint32_t RXFIFO;                /*!< [0x0014] I2S Receive FIFO Register                                        */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CTL1;                  /*!< [0x0020] I2S Control Register 1                                           */
    __IO uint32_t STATUS1;               /*!< [0x0024] I2S Status Register 1                                            */

} I2S_T;

/**
    @addtogroup I2S_CONST I2S Bit Field Definition
    Constant Definitions for I2S Controller
@{ */

#define I2S_CTL0_I2SEN_Pos               (0)                                               /*!< I2S_T::CTL0: I2SEN Position            */
#define I2S_CTL0_I2SEN_Msk               (0x1ul << I2S_CTL0_I2SEN_Pos)                     /*!< I2S_T::CTL0: I2SEN Mask                */

#define I2S_CTL0_TXEN_Pos                (1)                                               /*!< I2S_T::CTL0: TXEN Position             */
#define I2S_CTL0_TXEN_Msk                (0x1ul << I2S_CTL0_TXEN_Pos)                      /*!< I2S_T::CTL0: TXEN Mask                 */

#define I2S_CTL0_RXEN_Pos                (2)                                               /*!< I2S_T::CTL0: RXEN Position             */
#define I2S_CTL0_RXEN_Msk                (0x1ul << I2S_CTL0_RXEN_Pos)                      /*!< I2S_T::CTL0: RXEN Mask                 */

#define I2S_CTL0_MUTE_Pos                (3)                                               /*!< I2S_T::CTL0: MUTE Position             */
#define I2S_CTL0_MUTE_Msk                (0x1ul << I2S_CTL0_MUTE_Pos)                      /*!< I2S_T::CTL0: MUTE Mask                 */

#define I2S_CTL0_DATWIDTH_Pos            (4)                                               /*!< I2S_T::CTL0: DATWIDTH Position         */
#define I2S_CTL0_DATWIDTH_Msk            (0x3ul << I2S_CTL0_DATWIDTH_Pos)                  /*!< I2S_T::CTL0: DATWIDTH Mask             */

#define I2S_CTL0_MONO_Pos                (6)                                               /*!< I2S_T::CTL0: MONO Position             */
#define I2S_CTL0_MONO_Msk                (0x1ul << I2S_CTL0_MONO_Pos)                      /*!< I2S_T::CTL0: MONO Mask                 */

#define I2S_CTL0_ORDER_Pos               (7)                                               /*!< I2S_T::CTL0: ORDER Position            */
#define I2S_CTL0_ORDER_Msk               (0x1ul << I2S_CTL0_ORDER_Pos)                     /*!< I2S_T::CTL0: ORDER Mask                */

#define I2S_CTL0_SLAVE_Pos               (8)                                               /*!< I2S_T::CTL0: SLAVE Position            */
#define I2S_CTL0_SLAVE_Msk               (0x1ul << I2S_CTL0_SLAVE_Pos)                     /*!< I2S_T::CTL0: SLAVE Mask                */

#define I2S_CTL0_MCLKEN_Pos              (15)                                              /*!< I2S_T::CTL0: MCLKEN Position           */
#define I2S_CTL0_MCLKEN_Msk              (0x1ul << I2S_CTL0_MCLKEN_Pos)                    /*!< I2S_T::CTL0: MCLKEN Mask               */

#define I2S_CTL0_TXFBCLR_Pos             (18)                                              /*!< I2S_T::CTL0: TXFBCLR Position          */
#define I2S_CTL0_TXFBCLR_Msk             (0x1ul << I2S_CTL0_TXFBCLR_Pos)                   /*!< I2S_T::CTL0: TXFBCLR Mask              */

#define I2S_CTL0_RXFBCLR_Pos             (19)                                              /*!< I2S_T::CTL0: RXFBCLR Position          */
#define I2S_CTL0_RXFBCLR_Msk             (0x1ul << I2S_CTL0_RXFBCLR_Pos)                   /*!< I2S_T::CTL0: RXFBCLR Mask              */

#define I2S_CTL0_TXPDMAEN_Pos            (20)                                              /*!< I2S_T::CTL0: TXPDMAEN Position         */
#define I2S_CTL0_TXPDMAEN_Msk            (0x1ul << I2S_CTL0_TXPDMAEN_Pos)                  /*!< I2S_T::CTL0: TXPDMAEN Mask             */

#define I2S_CTL0_RXPDMAEN_Pos            (21)                                              /*!< I2S_T::CTL0: RXPDMAEN Position         */
#define I2S_CTL0_RXPDMAEN_Msk            (0x1ul << I2S_CTL0_RXPDMAEN_Pos)                  /*!< I2S_T::CTL0: RXPDMAEN Mask             */

#define I2S_CTL0_RXLCH_Pos               (23)                                              /*!< I2S_T::CTL0: RXLCH Position            */
#define I2S_CTL0_RXLCH_Msk               (0x1ul << I2S_CTL0_RXLCH_Pos)                     /*!< I2S_T::CTL0: RXLCH Mask                */

#define I2S_CTL0_FORMAT_Pos              (24)                                              /*!< I2S_T::CTL0: FORMAT Position           */
#define I2S_CTL0_FORMAT_Msk              (0x7ul << I2S_CTL0_FORMAT_Pos)                    /*!< I2S_T::CTL0: FORMAT Mask               */

#define I2S_CTL0_PCMSYNC_Pos             (27)                                              /*!< I2S_T::CTL0: PCMSYNC Position          */
#define I2S_CTL0_PCMSYNC_Msk             (0x1ul << I2S_CTL0_PCMSYNC_Pos)                   /*!< I2S_T::CTL0: PCMSYNC Mask              */

#define I2S_CTL0_CHWIDTH_Pos             (28)                                              /*!< I2S_T::CTL0: CHWIDTH Position          */
#define I2S_CTL0_CHWIDTH_Msk             (0x3ul << I2S_CTL0_CHWIDTH_Pos)                   /*!< I2S_T::CTL0: CHWIDTH Mask              */

#define I2S_CTL0_TDMCHNUM_Pos            (30)                                              /*!< I2S_T::CTL0: TDMCHNUM Position         */
#define I2S_CTL0_TDMCHNUM_Msk            (0x3ul << I2S_CTL0_TDMCHNUM_Pos)                  /*!< I2S_T::CTL0: TDMCHNUM Mask             */

#define I2S_CLKDIV_MCLKDIV_Pos           (0)                                               /*!< I2S_T::CLKDIV: MCLKDIV Position        */
#define I2S_CLKDIV_MCLKDIV_Msk           (0x3ful << I2S_CLKDIV_MCLKDIV_Pos)                /*!< I2S_T::CLKDIV: MCLKDIV Mask            */

#define I2S_CLKDIV_BCLKDIV_Pos           (8)                                               /*!< I2S_T::CLKDIV: BCLKDIV Position        */
#define I2S_CLKDIV_BCLKDIV_Msk           (0x1fful << I2S_CLKDIV_BCLKDIV_Pos)               /*!< I2S_T::CLKDIV: BCLKDIV Mask            */

#define I2S_IEN_RXUDFIEN_Pos             (0)                                               /*!< I2S_T::IEN: RXUDFIEN Position          */
#define I2S_IEN_RXUDFIEN_Msk             (0x1ul << I2S_IEN_RXUDFIEN_Pos)                   /*!< I2S_T::IEN: RXUDFIEN Mask              */

#define I2S_IEN_RXOVFIEN_Pos             (1)                                               /*!< I2S_T::IEN: RXOVFIEN Position          */
#define I2S_IEN_RXOVFIEN_Msk             (0x1ul << I2S_IEN_RXOVFIEN_Pos)                   /*!< I2S_T::IEN: RXOVFIEN Mask              */

#define I2S_IEN_RXTHIEN_Pos              (2)                                               /*!< I2S_T::IEN: RXTHIEN Position           */
#define I2S_IEN_RXTHIEN_Msk              (0x1ul << I2S_IEN_RXTHIEN_Pos)                    /*!< I2S_T::IEN: RXTHIEN Mask               */

#define I2S_IEN_TXUDFIEN_Pos             (8)                                               /*!< I2S_T::IEN: TXUDFIEN Position          */
#define I2S_IEN_TXUDFIEN_Msk             (0x1ul << I2S_IEN_TXUDFIEN_Pos)                   /*!< I2S_T::IEN: TXUDFIEN Mask              */

#define I2S_IEN_TXOVFIEN_Pos             (9)                                               /*!< I2S_T::IEN: TXOVFIEN Position          */
#define I2S_IEN_TXOVFIEN_Msk             (0x1ul << I2S_IEN_TXOVFIEN_Pos)                   /*!< I2S_T::IEN: TXOVFIEN Mask              */

#define I2S_IEN_TXTHIEN_Pos              (10)                                              /*!< I2S_T::IEN: TXTHIEN Position           */
#define I2S_IEN_TXTHIEN_Msk              (0x1ul << I2S_IEN_TXTHIEN_Pos)                    /*!< I2S_T::IEN: TXTHIEN Mask               */

#define I2S_IEN_CH0ZCIEN_Pos             (16)                                              /*!< I2S_T::IEN: CH0ZCIEN Position          */
#define I2S_IEN_CH0ZCIEN_Msk             (0x1ul << I2S_IEN_CH0ZCIEN_Pos)                   /*!< I2S_T::IEN: CH0ZCIEN Mask              */

#define I2S_IEN_CH1ZCIEN_Pos             (17)                                              /*!< I2S_T::IEN: CH1ZCIEN Position          */
#define I2S_IEN_CH1ZCIEN_Msk             (0x1ul << I2S_IEN_CH1ZCIEN_Pos)                   /*!< I2S_T::IEN: CH1ZCIEN Mask              */

#define I2S_IEN_CH2ZCIEN_Pos             (18)                                              /*!< I2S_T::IEN: CH2ZCIEN Position          */
#define I2S_IEN_CH2ZCIEN_Msk             (0x1ul << I2S_IEN_CH2ZCIEN_Pos)                   /*!< I2S_T::IEN: CH2ZCIEN Mask              */

#define I2S_IEN_CH3ZCIEN_Pos             (19)                                              /*!< I2S_T::IEN: CH3ZCIEN Position          */
#define I2S_IEN_CH3ZCIEN_Msk             (0x1ul << I2S_IEN_CH3ZCIEN_Pos)                   /*!< I2S_T::IEN: CH3ZCIEN Mask              */

#define I2S_IEN_CH4ZCIEN_Pos             (20)                                              /*!< I2S_T::IEN: CH4ZCIEN Position          */
#define I2S_IEN_CH4ZCIEN_Msk             (0x1ul << I2S_IEN_CH4ZCIEN_Pos)                   /*!< I2S_T::IEN: CH4ZCIEN Mask              */

#define I2S_IEN_CH5ZCIEN_Pos             (21)                                              /*!< I2S_T::IEN: CH5ZCIEN Position          */
#define I2S_IEN_CH5ZCIEN_Msk             (0x1ul << I2S_IEN_CH5ZCIEN_Pos)                   /*!< I2S_T::IEN: CH5ZCIEN Mask              */

#define I2S_IEN_CH6ZCIEN_Pos             (22)                                              /*!< I2S_T::IEN: CH6ZCIEN Position          */
#define I2S_IEN_CH6ZCIEN_Msk             (0x1ul << I2S_IEN_CH6ZCIEN_Pos)                   /*!< I2S_T::IEN: CH6ZCIEN Mask              */

#define I2S_IEN_CH7ZCIEN_Pos             (23)                                              /*!< I2S_T::IEN: CH7ZCIEN Position          */
#define I2S_IEN_CH7ZCIEN_Msk             (0x1ul << I2S_IEN_CH7ZCIEN_Pos)                   /*!< I2S_T::IEN: CH7ZCIEN Mask              */

#define I2S_STATUS0_I2SINT_Pos           (0)                                               /*!< I2S_T::STATUS0: I2SINT Position        */
#define I2S_STATUS0_I2SINT_Msk           (0x1ul << I2S_STATUS0_I2SINT_Pos)                 /*!< I2S_T::STATUS0: I2SINT Mask            */

#define I2S_STATUS0_I2SRXINT_Pos         (1)                                               /*!< I2S_T::STATUS0: I2SRXINT Position      */
#define I2S_STATUS0_I2SRXINT_Msk         (0x1ul << I2S_STATUS0_I2SRXINT_Pos)               /*!< I2S_T::STATUS0: I2SRXINT Mask          */

#define I2S_STATUS0_I2STXINT_Pos         (2)                                               /*!< I2S_T::STATUS0: I2STXINT Position      */
#define I2S_STATUS0_I2STXINT_Msk         (0x1ul << I2S_STATUS0_I2STXINT_Pos)               /*!< I2S_T::STATUS0: I2STXINT Mask          */

#define I2S_STATUS0_DATACH_Pos           (3)                                               /*!< I2S_T::STATUS0: DATACH Position        */
#define I2S_STATUS0_DATACH_Msk           (0x7ul << I2S_STATUS0_DATACH_Pos)                 /*!< I2S_T::STATUS0: DATACH Mask            */

#define I2S_STATUS0_RXUDIF_Pos           (8)                                               /*!< I2S_T::STATUS0: RXUDIF Position        */
#define I2S_STATUS0_RXUDIF_Msk           (0x1ul << I2S_STATUS0_RXUDIF_Pos)                 /*!< I2S_T::STATUS0: RXUDIF Mask            */

#define I2S_STATUS0_RXOVIF_Pos           (9)                                               /*!< I2S_T::STATUS0: RXOVIF Position        */
#define I2S_STATUS0_RXOVIF_Msk           (0x1ul << I2S_STATUS0_RXOVIF_Pos)                 /*!< I2S_T::STATUS0: RXOVIF Mask            */

#define I2S_STATUS0_RXTHIF_Pos           (10)                                              /*!< I2S_T::STATUS0: RXTHIF Position        */
#define I2S_STATUS0_RXTHIF_Msk           (0x1ul << I2S_STATUS0_RXTHIF_Pos)                 /*!< I2S_T::STATUS0: RXTHIF Mask            */

#define I2S_STATUS0_RXFULL_Pos           (11)                                              /*!< I2S_T::STATUS0: RXFULL Position        */
#define I2S_STATUS0_RXFULL_Msk           (0x1ul << I2S_STATUS0_RXFULL_Pos)                 /*!< I2S_T::STATUS0: RXFULL Mask            */

#define I2S_STATUS0_RXEMPTY_Pos          (12)                                              /*!< I2S_T::STATUS0: RXEMPTY Position       */
#define I2S_STATUS0_RXEMPTY_Msk          (0x1ul << I2S_STATUS0_RXEMPTY_Pos)                /*!< I2S_T::STATUS0: RXEMPTY Mask           */

#define I2S_STATUS0_TXUDIF_Pos           (16)                                              /*!< I2S_T::STATUS0: TXUDIF Position        */
#define I2S_STATUS0_TXUDIF_Msk           (0x1ul << I2S_STATUS0_TXUDIF_Pos)                 /*!< I2S_T::STATUS0: TXUDIF Mask            */

#define I2S_STATUS0_TXOVIF_Pos           (17)                                              /*!< I2S_T::STATUS0: TXOVIF Position        */
#define I2S_STATUS0_TXOVIF_Msk           (0x1ul << I2S_STATUS0_TXOVIF_Pos)                 /*!< I2S_T::STATUS0: TXOVIF Mask            */

#define I2S_STATUS0_TXTHIF_Pos           (18)                                              /*!< I2S_T::STATUS0: TXTHIF Position        */
#define I2S_STATUS0_TXTHIF_Msk           (0x1ul << I2S_STATUS0_TXTHIF_Pos)                 /*!< I2S_T::STATUS0: TXTHIF Mask            */

#define I2S_STATUS0_TXFULL_Pos           (19)                                              /*!< I2S_T::STATUS0: TXFULL Position        */
#define I2S_STATUS0_TXFULL_Msk           (0x1ul << I2S_STATUS0_TXFULL_Pos)                 /*!< I2S_T::STATUS0: TXFULL Mask            */

#define I2S_STATUS0_TXEMPTY_Pos          (20)                                              /*!< I2S_T::STATUS0: TXEMPTY Position       */
#define I2S_STATUS0_TXEMPTY_Msk          (0x1ul << I2S_STATUS0_TXEMPTY_Pos)                /*!< I2S_T::STATUS0: TXEMPTY Mask           */

#define I2S_STATUS0_TXBUSY_Pos           (21)                                              /*!< I2S_T::STATUS0: TXBUSY Position        */
#define I2S_STATUS0_TXBUSY_Msk           (0x1ul << I2S_STATUS0_TXBUSY_Pos)                 /*!< I2S_T::STATUS0: TXBUSY Mask            */

#define I2S_TXFIFO_TXFIFO_Pos            (0)                                               /*!< I2S_T::TXFIFO: TXFIFO Position         */
#define I2S_TXFIFO_TXFIFO_Msk            (0xfffffffful << I2S_TXFIFO_TXFIFO_Pos)           /*!< I2S_T::TXFIFO: TXFIFO Mask             */

#define I2S_RXFIFO_RXFIFO_Pos            (0)                                               /*!< I2S_T::RXFIFO: RXFIFO Position         */
#define I2S_RXFIFO_RXFIFO_Msk            (0xfffffffful << I2S_RXFIFO_RXFIFO_Pos)           /*!< I2S_T::RXFIFO: RXFIFO Mask             */

#define I2S_CTL1_CH0ZCEN_Pos             (0)                                               /*!< I2S_T::CTL1: CH0ZCEN Position          */
#define I2S_CTL1_CH0ZCEN_Msk             (0x1ul << I2S_CTL1_CH0ZCEN_Pos)                   /*!< I2S_T::CTL1: CH0ZCEN Mask              */

#define I2S_CTL1_CH1ZCEN_Pos             (1)                                               /*!< I2S_T::CTL1: CH1ZCEN Position          */
#define I2S_CTL1_CH1ZCEN_Msk             (0x1ul << I2S_CTL1_CH1ZCEN_Pos)                   /*!< I2S_T::CTL1: CH1ZCEN Mask              */

#define I2S_CTL1_CH2ZCEN_Pos             (2)                                               /*!< I2S_T::CTL1: CH2ZCEN Position          */
#define I2S_CTL1_CH2ZCEN_Msk             (0x1ul << I2S_CTL1_CH2ZCEN_Pos)                   /*!< I2S_T::CTL1: CH2ZCEN Mask              */

#define I2S_CTL1_CH3ZCEN_Pos             (3)                                               /*!< I2S_T::CTL1: CH3ZCEN Position          */
#define I2S_CTL1_CH3ZCEN_Msk             (0x1ul << I2S_CTL1_CH3ZCEN_Pos)                   /*!< I2S_T::CTL1: CH3ZCEN Mask              */

#define I2S_CTL1_CH4ZCEN_Pos             (4)                                               /*!< I2S_T::CTL1: CH4ZCEN Position          */
#define I2S_CTL1_CH4ZCEN_Msk             (0x1ul << I2S_CTL1_CH4ZCEN_Pos)                   /*!< I2S_T::CTL1: CH4ZCEN Mask              */

#define I2S_CTL1_CH5ZCEN_Pos             (5)                                               /*!< I2S_T::CTL1: CH5ZCEN Position          */
#define I2S_CTL1_CH5ZCEN_Msk             (0x1ul << I2S_CTL1_CH5ZCEN_Pos)                   /*!< I2S_T::CTL1: CH5ZCEN Mask              */

#define I2S_CTL1_CH6ZCEN_Pos             (6)                                               /*!< I2S_T::CTL1: CH6ZCEN Position          */
#define I2S_CTL1_CH6ZCEN_Msk             (0x1ul << I2S_CTL1_CH6ZCEN_Pos)                   /*!< I2S_T::CTL1: CH6ZCEN Mask              */

#define I2S_CTL1_CH7ZCEN_Pos             (7)                                               /*!< I2S_T::CTL1: CH7ZCEN Position          */
#define I2S_CTL1_CH7ZCEN_Msk             (0x1ul << I2S_CTL1_CH7ZCEN_Pos)                   /*!< I2S_T::CTL1: CH7ZCEN Mask              */

#define I2S_CTL1_TXTH_Pos                (8)                                               /*!< I2S_T::CTL1: TXTH Position             */
#define I2S_CTL1_TXTH_Msk                (0xful << I2S_CTL1_TXTH_Pos)                      /*!< I2S_T::CTL1: TXTH Mask                 */

#define I2S_CTL1_RXTH_Pos                (16)                                              /*!< I2S_T::CTL1: RXTH Position             */
#define I2S_CTL1_RXTH_Msk                (0xful << I2S_CTL1_RXTH_Pos)                      /*!< I2S_T::CTL1: RXTH Mask                 */

#define I2S_CTL1_PBWIDTH_Pos             (24)                                              /*!< I2S_T::CTL1: PBWIDTH Position          */
#define I2S_CTL1_PBWIDTH_Msk             (0x1ul << I2S_CTL1_PBWIDTH_Pos)                   /*!< I2S_T::CTL1: PBWIDTH Mask              */

#define I2S_CTL1_PB16ORD_Pos             (25)                                              /*!< I2S_T::CTL1: PB16ORD Position          */
#define I2S_CTL1_PB16ORD_Msk             (0x1ul << I2S_CTL1_PB16ORD_Pos)                   /*!< I2S_T::CTL1: PB16ORD Mask              */

#define I2S_STATUS1_CH0ZCIF_Pos          (0)                                               /*!< I2S_T::STATUS1: CH0ZCIF Position       */
#define I2S_STATUS1_CH0ZCIF_Msk          (0x1ul << I2S_STATUS1_CH0ZCIF_Pos)                /*!< I2S_T::STATUS1: CH0ZCIF Mask           */

#define I2S_STATUS1_CH1ZCIF_Pos          (1)                                               /*!< I2S_T::STATUS1: CH1ZCIF Position       */
#define I2S_STATUS1_CH1ZCIF_Msk          (0x1ul << I2S_STATUS1_CH1ZCIF_Pos)                /*!< I2S_T::STATUS1: CH1ZCIF Mask           */

#define I2S_STATUS1_CH2ZCIF_Pos          (2)                                               /*!< I2S_T::STATUS1: CH2ZCIF Position       */
#define I2S_STATUS1_CH2ZCIF_Msk          (0x1ul << I2S_STATUS1_CH2ZCIF_Pos)                /*!< I2S_T::STATUS1: CH2ZCIF Mask           */

#define I2S_STATUS1_CH3ZCIF_Pos          (3)                                               /*!< I2S_T::STATUS1: CH3ZCIF Position       */
#define I2S_STATUS1_CH3ZCIF_Msk          (0x1ul << I2S_STATUS1_CH3ZCIF_Pos)                /*!< I2S_T::STATUS1: CH3ZCIF Mask           */

#define I2S_STATUS1_CH4ZCIF_Pos          (4)                                               /*!< I2S_T::STATUS1: CH4ZCIF Position       */
#define I2S_STATUS1_CH4ZCIF_Msk          (0x1ul << I2S_STATUS1_CH4ZCIF_Pos)                /*!< I2S_T::STATUS1: CH4ZCIF Mask           */

#define I2S_STATUS1_CH5ZCIF_Pos          (5)                                               /*!< I2S_T::STATUS1: CH5ZCIF Position       */
#define I2S_STATUS1_CH5ZCIF_Msk          (0x1ul << I2S_STATUS1_CH5ZCIF_Pos)                /*!< I2S_T::STATUS1: CH5ZCIF Mask           */

#define I2S_STATUS1_CH6ZCIF_Pos          (6)                                               /*!< I2S_T::STATUS1: CH6ZCIF Position       */
#define I2S_STATUS1_CH6ZCIF_Msk          (0x1ul << I2S_STATUS1_CH6ZCIF_Pos)                /*!< I2S_T::STATUS1: CH6ZCIF Mask           */

#define I2S_STATUS1_CH7ZCIF_Pos          (7)                                               /*!< I2S_T::STATUS1: CH7ZCIF Position       */
#define I2S_STATUS1_CH7ZCIF_Msk          (0x1ul << I2S_STATUS1_CH7ZCIF_Pos)                /*!< I2S_T::STATUS1: CH7ZCIF Mask           */

#define I2S_STATUS1_TXCNT_Pos            (8)                                               /*!< I2S_T::STATUS1: TXCNT Position         */
#define I2S_STATUS1_TXCNT_Msk            (0x1ful << I2S_STATUS1_TXCNT_Pos)                 /*!< I2S_T::STATUS1: TXCNT Mask             */

#define I2S_STATUS1_RXCNT_Pos            (16)                                              /*!< I2S_T::STATUS1: RXCNT Position         */
#define I2S_STATUS1_RXCNT_Msk            (0x1ful << I2S_STATUS1_RXCNT_Pos)                 /*!< I2S_T::STATUS1: RXCNT Mask             */

/**@}*/ /* I2S_CONST */
/**@}*/ /* end of I2S register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __I2S_REG_H__ */
