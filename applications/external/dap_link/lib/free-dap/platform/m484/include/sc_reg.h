/**************************************************************************//**
 * @file     sc_reg.h
 * @version  V1.00
 * @brief    SC register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SC_REG_H__
#define __SC_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup SC Smart Card Host Interface Controller(SC)
    Memory Mapped Structure for SC Controller
@{ */

typedef struct
{


    /**
     * @var SC_T::DAT
     * Offset: 0x00  SC Receive/Transmit Holding Buffer Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |DAT       |Receive/Transmit Holding Buffer
     * |        |          |Write Operation:
     * |        |          |By writing data to DAT, the SC will send out an 8-bit data.
     * |        |          |Note: If SCEN (SCn_CTL[0]) is not enabled, DAT cannot be programmed.
     * |        |          |Read Operation:
     * |        |          |By reading DAT, the SC will return an 8-bit received data.
     * @var SC_T::CTL
     * Offset: 0x04  SC Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SCEN      |SC Controller Enable Bit
     * |        |          |Set this bit to 1 to enable SC operation. If this bit is cleared,
     * |        |          |0 = SC will force all transition to IDLE state.
     * |        |          |1 = SC controller is enabled and all function can work correctly.
     * |        |          |Note1: SCEN must be set to 1 before filling in other SC registers, or smart card will not work properly.
     * |[1]     |RXOFF     |RX Transition Disable Control Bit
     * |        |          |This bit is used for disable Rx transition function.
     * |        |          |0 = The receiver Enabled.
     * |        |          |1 = The receiver Disabled.
     * |        |          |Note1: If AUTOCEN (SCn_CTL[3]) is enabled, this field is ignored.
     * |[2]     |TXOFF     |TX Transition Disable Control Bit
     * |        |          |This bit is used for disable Tx transition function.
     * |        |          |0 = The transceiver Enabled.
     * |        |          |1 = The transceiver Disabled.
     * |[3]     |AUTOCEN   |Auto Convention Enable Bit
     * |        |          |This bit is used for enable auto convention function.
     * |        |          |0 = Auto-convention Disabled.
     * |        |          |1 = Auto-convention Enabled.
     * |        |          |If user enables auto convention function, the setting step must be done before Answer to Reset (ATR)
     * |        |          |state and the first data must be 0x3B or 0x3F.
     * |        |          |After hardware received first data and stored it at buffer, hardware will decided the convention and
     * |        |          |change the CONSEL (SCn_CTL[5:4]) bits automatically when received first data is 0x3B or 0x3F.
     * |        |          |If received first byte is 0x3B, TS is direct convention, CONSEL (SCn_CTL[5:4]) will be set to 00
     * |        |          |automatically, otherwise the TS is inverse convention, and CONSEL (SCn_CTL[5:4]) will be set to 11.
     * |        |          |If the first data is not 0x3B or 0x3F, hardware will set ACERRIF (SCn_INTSTS[10]) and generate an
     * |        |          |interrupt to CPU when ACERRIEN (SCn_INTEN[10]) is enabled.
     * |[5:4]   |CONSEL    |Convention Selection
     * |        |          |00 = Direct convention.
     * |        |          |01 = Reserved.
     * |        |          |10 = Reserved.
     * |        |          |11 = Inverse convention.
     * |        |          |Note: If AUTOCEN (SCn_CTL[3]) is enabled, this field is ignored.
     * |[7:6]   |RXTRGLV   |Rx Buffer Trigger Level
     * |        |          |When the number of bytes in the receiving buffer equals the RXTRGLV, the RDAIF will be set
     * |        |          |If RDAIEN (SCn_INTEN[0]) is enabled, an interrupt will be generated to CPU.
     * |        |          |00 = Rx Buffer Trigger Level with 01 bytes.
     * |        |          |01 = Rx Buffer Trigger Level with 02 bytes.
     * |        |          |10 = Rx Buffer Trigger Level with 03 bytes.
     * |        |          |11 = Reserved.
     * |[12:8]  |BGT       |Block Guard Time (BGT)
     * |        |          |Block guard time means the minimum interval between the leading edges of two consecutive characters
     * |        |          |between different transfer directions
     * |        |          |This field indicates the counter for the bit length of block guard time
     * |        |          |According to ISO 7816-3, in T = 0 mode, user must fill 15 (real block guard time = 16.5) to this
     * |        |          |field; in T = 1 mode, user must fill 21 (real block guard time = 22.5) to it.
     * |        |          |Note: The real block guard time is BGT + 1.
     * |[14:13] |TMRSEL    |Timer Channel Selection
     * |        |          |00 = All internal timer function Disabled.
     * |        |          |11 = Internal 24 bit timer and two 8 bit timers Enabled
     * |        |          |User can configure them by setting SCn_TMRCTL0[23:0], SCn_TMRCTL1[7:0] and SCn_TMRCTL2[7:0].
     * |        |          |Other configurations are reserved
     * |[15]    |NSB       |Stop Bit Length
     * |        |          |This field indicates the length of stop bit.
     * |        |          |0 = The stop bit length is 2 ETU.
     * |        |          |1= The stop bit length is 1 ETU.
     * |        |          |Note1: The default stop bit length is 2. SC and UART adopts NSB to program the stop bit length.
     * |        |          |Note2: In UART mode, RX can receive the data sequence in 1 stop bit or 2 stop bits with NSB is set to 0.
     * |[18:16] |RXRTY     |RX Error Retry Count Number
     * |        |          |This field indicates the maximum number of receiver retries that are allowed when parity error has occurred.
     * |        |          |Note1: The real retry number is RXRTY + 1, so 8 is the maximum retry number.
     * |        |          |Note2: This field cannot be changed when RXRTYEN enabled
     * |        |          |The change flow is to disable RXRTYEN first and then fill in new retry value.
     * |[19]    |RXRTYEN   |RX Error Retry Enable Bit
     * |        |          |This bit enables receiver retry function when parity error has occurred.
     * |        |          |0 = RX error retry function Disabled.
     * |        |          |1 = RX error retry function Enabled.
     * |        |          |Note: User must fill in the RXRTY value before enabling this bit.
     * |[22:20] |TXRTY     |TX Error Retry Count Number
     * |        |          |This field indicates the maximum number of transmitter retries that are allowed when parity
     * |        |          |error has occurred.
     * |        |          |Note1: The real retry number is TXRTY + 1, so 8 is the maximum retry number.
     * |        |          |Note2: This field cannot be changed when TXRTYEN enabled
     * |        |          |The change flow is to disable TXRTYEN first and then fill in new retry value.
     * |[23]    |TXRTYEN   |TX Error Retry Enable Bit
     * |        |          |This bit enables transmitter retry function when parity error has occurred.
     * |        |          |0 = TX error retry function Disabled.
     * |        |          |1 = TX error retry function Enabled.
     * |[25:24] |CDDBSEL   |Card Detect De-bounce Selection
     * |        |          |This field indicates the card detect de-bounce selection.
     * |        |          |00 = De-bounce sample card insert once per 384 (128 * 3) SC module clocks and de-bounce
     * |        |          |sample card removal once per 128 SC module clocks.
     * |        |          |Other configurations are reserved.
     * |[26]    |CDLV      |Card Detect Level Selection
     * |        |          |0 = When hardware detects the card detect pin (SCn_CD) from high to low, it indicates a card is detected.
     * |        |          |1 = When hardware detects the card detect pin (SCn_CD) from low to high, it indicates a card is detected.
     * |        |          |Note: User must select card detect level before Smart Card controller enabled.
     * |[30]    |SYNC      |SYNC Flag Indicator (Read Only)
     * |        |          |Due to synchronization, user should check this bit before writing a new value to RXRTY and TXRTY fields.
     * |        |          |0 = Synchronizing is completion, user can write new data to RXRTY and TXRTY.
     * |        |          |1 = Last value is synchronizing.
     * @var SC_T::ALTCTL
     * Offset: 0x08  SC Alternate Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TXRST     |TX Software Reset
     * |        |          |When TXRST is set, all the bytes in the transmit buffer and TX internal state machine will be cleared.
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the TX internal state machine and pointers.
     * |        |          |Note: This bit will be auto cleared after reset is complete.
     * |[1]     |RXRST     |Rx Software Reset
     * |        |          |When RXRST is set, all the bytes in the receive buffer and Rx internal state machine will be cleared.
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the Rx internal state machine and pointers.
     * |        |          |Note: This bit will be auto cleared after reset is complete.
     * |[2]     |DACTEN    |Deactivation Sequence Generator Enable Bit
     * |        |          |This bit enables SC controller to initiate the card by deactivation sequence.
     * |        |          |0 = No effect.
     * |        |          |1 = Deactivation sequence generator Enabled.
     * |        |          |Note1: When the deactivation sequence completed, this bit will be cleared automatically and
     * |        |          |the INITIF (SCn_INTSTS[8]) will be set to 1.
     * |        |          |Note2: This field will be cleared by TXRST (SCn_ALTCTL[0]) and RXRST (SCn_ALTCTL[1])
     * |        |          |Thus, do not fill in this bit DACTEN, TXRST and RXRST at the same time.
     * |        |          |Note3: If SCEN (SCn_CTL[0]) is not enabled, this filed cannot be programmed.
     * |[3]     |ACTEN     |Activation Sequence Generator Enable Bit
     * |        |          |This bit enables SC controller to initiate the card by activation sequence.
     * |        |          |0 = No effect.
     * |        |          |1 = Activation sequence generator Enabled.
     * |        |          |Note1: When the activation sequence completed, this bit will be cleared automatically and the
     * |        |          |INITIF (SCn_INTSTS[8]) will be set to 1.
     * |        |          |Note2: This field will be cleared by TXRST (SCn_ALTCTL[0]) and RXRST (SCn_ALTCTL[1])
     * |        |          |Thus, do not fill in this bit ACTEN, TXRST and RXRST at the same time.
     * |        |          |Note3: If SCEN (SCn_CTL[0]) is not enabled, this filed cannot be programmed.
     * |        |          |Note4: During the activation sequence, RX is disabled automatically and can not receive data
     * |        |          |After the activation sequence completion, RXOFF (SCn_CTL[1]) keeps the state before hardware activation.
     * |[4]     |WARSTEN   |Warm Reset Sequence Generator Enable Bit
     * |        |          |This bit enables SC controller to initiate the card by warm reset sequence.
     * |        |          |0 = No effect.
     * |        |          |1 = Warm reset sequence generator Enabled.
     * |        |          |Note1: When the warm reset sequence completed, this bit will be cleared automatically and the
     * |        |          |INITIF (SCn_INTSTS[8]) will be set to 1.
     * |        |          |Note2: This field will be cleared by TXRST (SCn_ALTCTL[0]) and RXRST (SCn_ALTCTL[1])
     * |        |          |Thus, do not fill in this bit WARSTEN, TXRST and RXRST at the same time.
     * |        |          |Note3: If SCEN (SCn_CTL[0]) is not enabled, this filed cannot be programmed.
     * |        |          |Note4: During the warm reset sequence, RX is disabled automatically and can not receive data
     * |        |          |After the warm reset sequence completion, RXOFF (SCn_CTL[1]) keeps the state before perform
     * |        |          |warm reset sequence.
     * |[5]     |CNTEN0    |Internal Timer0 Start Enable Bit
     * |        |          |This bit enables Timer 0 to start counting
     * |        |          |User can fill 0 to stop it and set 1 to reload and count
     * |        |          |The counter unit is ETU base.
     * |        |          |0 = Stops counting.
     * |        |          |1 = Start counting.
     * |        |          |Note1: This field is used for internal 24 bit timer when TMRSEL (SCn_CTL[14:13]) is 11 only.
     * |        |          |Note2: If the operation mode is not in auto-reload mode (SCn_TMRCTL0[26] = 0), this bit will
     * |        |          |be auto-cleared by hardware.
     * |        |          |Note3: If SCEN (SCn_CTL[0]) is not enabled, this filed cannot be programmed.
     * |[6]     |CNTEN1    |Internal Timer1 Start Enable Bit
     * |        |          |This bit enables Timer 1 to start counting
     * |        |          |User can fill 0 to stop it and set 1 to reload and count
     * |        |          |The counter unit is ETU base.
     * |        |          |0 = Stops counting.
     * |        |          |1 = Start counting.
     * |        |          |Note1: This field is used for internal 8 bit timer when TMRSEL(SCn_CTL[14:13]) is 11 only
     * |        |          |Do not fill CNTEN1 when TMRSEL (SCn_CTL[14:13]) is not equal to 11.
     * |        |          |Note2: If the operation mode is not in auto-reload mode (SCn_TMRCTL1[26] = 0), this bit will
     * |        |          |be auto-cleared by hardware.
     * |        |          |Note3: If SCEN (SCn_CTL[0]) is not enabled, this filed cannot be programmed.
     * |[7]     |CNTEN2    |Internal Timer2 Start Enable Bit
     * |        |          |This bit enables Timer 2 to start counting
     * |        |          |User can fill 0 to stop it and set 1 to reload and count
     * |        |          |The counter unit is ETU base.
     * |        |          |0 = Stops counting.
     * |        |          |1 = Start counting.
     * |        |          |Note1: This field is used for internal 8 bit timer when TMRSEL (SCn_CTL[14:13]) is 11 only
     * |        |          |Do not fill in CNTEN2 when TMRSEL (SCn_CTL[14:13]) is not equal to 11.
     * |        |          |Note2: If the operation mode is not in auto-reload mode (SCn_TMRCTL2[26] = 0), this bit will
     * |        |          |be auto-cleared by hardware.
     * |        |          |Note3: If SCEN (SCn_CTL[0]) is not enabled, this filed cannot be programmed.
     * |[9:8]   |INITSEL   |Initial Timing Selection
     * |        |          |This fields indicates the initial timing of hardware activation, warm-reset or deactivation.
     * |        |          |The unit of initial timing is SC module clock.
     * |        |          |Activation: refer to SC Activation Sequence in Figure 7.17-54.
     * |        |          |Warm-reset: refer to Warm-Reset Sequence in Figure 7.17-5.
     * |        |          |Deactivation: refer to Deactivation Sequence in Figure 7.17-56.
     * |        |          |Note: When set activation and warm reset in Timer0 operation mode 0011, it may have deviation
     * |        |          |at most 128 SC module clock cycles.
     * |[11]    |ADACEN    |Auto Deactivation When Card Removal
     * |        |          |This bit is used for enable hardware auto deactivation when smart card is removed.
     * |        |          |0 = Auto deactivation Disabled.
     * |        |          |1 = Auto deactivation Enabled.
     * |        |          |Note: When the card is removed, hardware will stop any process and then do deactivation sequence
     * |        |          |if this bit is set
     * |        |          |If auto deactivation process completes, hardware will set INITIF (SCn_INTSTS[8]) also.
     * |[12]    |RXBGTEN   |Receiver Block Guard Time Function Enable Bit
     * |        |          |This bit enables the receiver block guard time function.
     * |        |          |0 = Receiver block guard time function Disabled.
     * |        |          |1 = Receiver block guard time function Enabled.
     * |[13]    |ACTSTS0   |Internal Timer0 Active Status (Read Only)
     * |        |          |This bit indicates the timer counter status of timer0.
     * |        |          |0 = Timer0 is not active.
     * |        |          |1 = Timer0 is active.
     * |        |          |Note: Timer0 is active does not always mean timer0 is counting the CNT (SCn_TMRCTL0[23:0]).
     * |[14]    |ACTSTS1   |Internal Timer1 Active Status (Read Only)
     * |        |          |This bit indicates the timer counter status of timer1.
     * |        |          |0 = Timer1 is not active.
     * |        |          |1 = Timer1 is active.
     * |        |          |Note: Timer1 is active does not always mean timer1 is counting the CNT (SCn_TMRCTL1[7:0]).
     * |[15]    |ACTSTS2   |Internal Timer2 Active Status (Read Only)
     * |        |          |This bit indicates the timer counter status of timer2.
     * |        |          |0 = Timer2 is not active.
     * |        |          |1 = Timer2 is active.
     * |        |          |Note: Timer2 is active does not always mean timer2 is counting the CNT (SCn_TMRCTL2[7:0]).
     * |[31]    |SYNC      |SYNC Flag Indicator (Read Only)
     * |        |          |Due to synchronization, user should check this bit when writing a new value to SCn_ALTCTL register.
     * |        |          |0 = Synchronizing is completion, user can write new data to SCn_ALTCTL register.
     * |        |          |1 = Last value is synchronizing.
     * @var SC_T::EGT
     * Offset: 0x0C  SC Extra Guard Time Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |EGT       |Extra Guard Time
     * |        |          |This field indicates the extra guard time value.
     * |        |          |Note: The extra guard time unit is ETU base.
     * @var SC_T::RXTOUT
     * Offset: 0x10  SC Receive Buffer Time-out Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |RFTM      |SC Receiver FIFO Time-out Counter
     * |        |          |The time-out down counter resets and starts counting whenever the RX buffer received a new data
     * |        |          |Once the counter decrease to 1 and no new data is received or CPU does not read data by
     * |        |          |reading SCn_DAT, a receiver time-out flag RBTOIF (SCn_INTSTS[9]) will be set, and hardware will
     * |        |          |generate an interrupt to CPU when RBTOIEN (SCn_INTEN[9]) is enabled.
     * |        |          |Note1: The counter unit is ETU based and the interval of time-out is RFTM + 0.5.
     * |        |          |Note2: Filling in all 0 to this field indicates to disable this function.
     * @var SC_T::ETUCTL
     * Offset: 0x14  SC Element Time Unit Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |ETURDIV   |ETU Rate Divider
     * |        |          |The field is used for ETU clock rate divider.
     * |        |          |The real ETU is ETURDIV + 1.
     * |        |          |Note: User can configure this field, but this field must be greater than 0x04.
     * @var SC_T::INTEN
     * Offset: 0x18  SC Interrupt Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RDAIEN    |Receive Data Reach Interrupt Enable Bit
     * |        |          |This field is used to enable received data reaching trigger level RXTRGLV (SCn_CTL[7:6]) interrupt.
     * |        |          |0 = Receive data reach trigger level interrupt Disabled.
     * |        |          |1 = Receive data reach trigger level interrupt Enabled.
     * |[1]     |TBEIEN    |Transmit Buffer Empty Interrupt Enable Bit
     * |        |          |This field is used to enable transmit buffer empty interrupt.
     * |        |          |0 = Transmit buffer empty interrupt Disabled.
     * |        |          |1 = Transmit buffer empty interrupt Enabled.
     * |[2]     |TERRIEN   |Transfer Error Interrupt Enable Bit
     * |        |          |This field is used to enable transfer error interrupt
     * |        |          |The transfer error states is at SCn_STATUS register which includes receiver break error
     * |        |          |BEF (SCn_STATUS[6]), frame error FEF (SCn_STATUS[5]), parity error PEF (SCn_STATUS[4]), receive
     * |        |          |buffer overflow error RXOV (SCn_STATUS[0]), transmit buffer overflow error TXOV (SCn_STATUS[8]),
     * |        |          |receiver retry over limit error RXOVERR (SCn_STATUS[22]) and transmitter retry over limit error
     * |        |          |TXOVERR (SCn_STATUS[30]).
     * |        |          |0 = Transfer error interrupt Disabled.
     * |        |          |1 = Transfer error interrupt Enabled.
     * |[3]     |TMR0IEN   |Timer0 Interrupt Enable Bit
     * |        |          |This field is used to enable Timer0 interrupt function.
     * |        |          |0 = Timer0 interrupt Disabled.
     * |        |          |1 = Timer0 interrupt Enabled.
     * |[4]     |TMR1IEN   |Timer1 Interrupt Enable Bit
     * |        |          |This field is used to enable the Timer1 interrupt function.
     * |        |          |0 = Timer1 interrupt Disabled.
     * |        |          |1 = Timer1 interrupt Enabled.
     * |[5]     |TMR2IEN   |Timer2 Interrupt Enable Bit
     * |        |          |This field is used to enable Timer2 interrupt function.
     * |        |          |0 = Timer2 interrupt Disabled.
     * |        |          |1 = Timer2 interrupt Enabled.
     * |[6]     |BGTIEN    |Block Guard Time Interrupt Enable Bit
     * |        |          |This field is used to enable block guard time interrupt in receive direction.
     * |        |          |0 = Block guard time interrupt Disabled.
     * |        |          |1 = Block guard time interrupt Enabled.
     * |        |          |Note: This bit is valid only for receive direction block guard time.
     * |[7]     |CDIEN     |Card Detect Interrupt Enable Bit
     * |        |          |This field is used to enable card detect interrupt
     * |        |          |The card detect status is CDPINSTS (SCn_STATUS[13]).
     * |        |          |0 = Card detect interrupt Disabled.
     * |        |          |1 = Card detect interrupt Enabled.
     * |[8]     |INITIEN   |Initial End Interrupt Enable Bit
     * |        |          |This field is used to enable activation (ACTEN (SCn_ALTCTL[3] = 1)), deactivation
     * |        |          |(DACTEN (SCn_ALTCTL[2] = 1)) and warm reset (WARSTEN (SCn_ALTCTL [4])) sequence complete interrupt.
     * |        |          |0 = Initial end interrupt Disabled.
     * |        |          |1 = Initial end interrupt Enabled.
     * |[9]     |RXTOIEN   |Receiver Buffer Time-out Interrupt Enable Bit
     * |        |          |This field is used to enable receiver buffer time-out interrupt.
     * |        |          |0 = Receiver buffer time-out interrupt Disabled.
     * |        |          |1 = Receiver buffer time-out interrupt Enabled.
     * |[10]    |ACERRIEN  |Auto Convention Error Interrupt Enable Bit
     * |        |          |This field is used to enable auto-convention error interrupt.
     * |        |          |0 = Auto-convention error interrupt Disabled.
     * |        |          |1 = Auto-convention error interrupt Enabled.
     * @var SC_T::INTSTS
     * Offset: 0x1C  SC Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RDAIF     |Receive Data Reach Interrupt Status Flag (Read Only)
     * |        |          |This field is used for received data reaching trigger level RXTRGLV (SCn_CTL[7:6]) interrupt status flag.
     * |        |          |0 = Number of receive buffer is less than RXTRGLV setting.
     * |        |          |1 = Number of receive buffer data equals the RXTRGLV setting.
     * |        |          |Note: This bit is read only
     * |        |          |If user reads data from SCn_DAT and receiver buffer data byte number is less than RXTRGLV,
     * |        |          |this bit will be cleared automatically.
     * |[1]     |TBEIF     |Transmit Buffer Empty Interrupt Status Flag (Read Only)
     * |        |          |This field is used for transmit buffer empty interrupt status flag.
     * |        |          |0 = Transmit buffer is not empty.
     * |        |          |1 = Transmit buffer is empty.
     * |        |          |Note: This bit is read only
     * |        |          |If user wants to clear this bit, user must write data to DAT (SCn_DAT[7:0]) and then this bit
     * |        |          |will be cleared automatically.
     * |[2]     |TERRIF    |Transfer Error Interrupt Status Flag
     * |        |          |This field is used for transfer error interrupt status flag
     * |        |          |The transfer error states is at SCn_STATUS register which includes receiver break error
     * |        |          |BEF (SCn_STATUS[6]), frame error FEF (SCn_STATUS[5], parity error PEF (SCn_STATUS[4] and receive
     * |        |          |buffer overflow error RXOV (SCn_STATUS[0]), transmit buffer overflow error TXOV (SCn_STATUS[8]),
     * |        |          |receiver retry over limit error RXOVERR (SCn_STATUS[22] or transmitter retry over limit error
     * |        |          |TXOVERR (SCn_STATUS[30]).
     * |        |          |0 = Transfer error interrupt did not occur.
     * |        |          |1 = Transfer error interrupt occurred.
     * |        |          |Note1: This field is the status flag of BEF, FEF, PEF, RXOV, TXOV, RXOVERR or TXOVERR.
     * |        |          |Note2: This bit can be cleared by writing 1 to it.
     * |[3]     |TMR0IF    |Timer0 Interrupt Status Flag
     * |        |          |This field is used for Timer0 interrupt status flag.
     * |        |          |0 = Timer0 interrupt did not occur.
     * |        |          |1 = Timer0 interrupt occurred.
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[4]     |TMR1IF    |Timer1 Interrupt Status Flag
     * |        |          |This field is used for Timer1 interrupt status flag.
     * |        |          |0 = Timer1 interrupt did not occur.
     * |        |          |1 = Timer1 interrupt occurred.
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[5]     |TMR2IF    |Timer2 Interrupt Status Flag
     * |        |          |This field is used for Timer2 interrupt status flag.
     * |        |          |0 = Timer2 interrupt did not occur.
     * |        |          |1 = Timer2 interrupt occurred.
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[6]     |BGTIF     |Block Guard Time Interrupt Status Flag
     * |        |          |This field is used for indicate block guard time interrupt status flag in receive direction.
     * |        |          |0 = Block guard time interrupt did not occur.
     * |        |          |1 = Block guard time interrupt occurred.
     * |        |          |Note1: This bit is valid only when RXBGTEN (SCn_ALTCTL[12]) is enabled.
     * |        |          |Note2: This bit can be cleared by writing 1 to it.
     * |[7]     |CDIF      |Card Detect Interrupt Status Flag (Read Only)
     * |        |          |This field is used for card detect interrupt status flag
     * |        |          |The card detect status is CINSERT (SCn_STATUS[12]) and CREMOVE (SCn_STATUS[11]).
     * |        |          |0 = Card detect event did not occur.
     * |        |          |1 = Card detect event occurred.
     * |        |          |Note: This bit is read only, user must to clear CINSERT or CREMOVE status to clear it.
     * |[8]     |INITIF    |Initial End Interrupt Status Flag
     * |        |          |This field is used for activation (ACTEN (SCn_ALTCTL[3])), deactivation (DACTEN (SCn_ALTCTL[2]))
     * |        |          |and warm reset (WARSTEN (SCn_ALTCTL[4])) sequence interrupt status flag.
     * |        |          |0 = Initial sequence is not complete.
     * |        |          |1 = Initial sequence is completed.
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[9]     |RXTOIF    |Receive Buffer Time-out Interrupt Status Flag (Read Only)
     * |        |          |This field is used for indicate receive buffer time-out interrupt status flag.
     * |        |          |0 = Receive buffer time-out interrupt did not occur.
     * |        |          |1 = Receive buffer time-out interrupt occurred.
     * |        |          |Note: This bit is read only, user must read all receive buffer remaining data by reading SCn_DAT
     * |        |          |register to clear it.
     * |[10]    |ACERRIF   |Auto Convention Error Interrupt Status Flag
     * |        |          |This field indicates auto convention sequence error.
     * |        |          |0 = Received TS at ATR state is 0x3B or 0x3F.
     * |        |          |1 = Received TS at ATR state is neither 0x3B nor 0x3F.
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * @var SC_T::STATUS
     * Offset: 0x20  SC Transfer Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RXOV      |Receive Overflow Error Status Flag
     * |        |          |This bit is set when Rx buffer overflow.
     * |        |          |0 = Rx buffer is not overflow.
     * |        |          |1 = Rx buffer is overflow when the number of received bytes is greater than Rx buffer size (4 bytes).
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[1]     |RXEMPTY   |Receive Buffer Empty Status Flag (Read Only)
     * |        |          |This bit indicates Rx buffer empty or not.
     * |        |          |0 = Rx buffer is not empty.
     * |        |          |1 = Rx buffer is empty, it means the last byte of Rx buffer has read from DAT (SCn_DAT[7:0]) by CPU.
     * |[2]     |RXFULL    |Receive Buffer Full Status Flag (Read Only)
     * |        |          |This bit indicates Rx buffer full or not.
     * |        |          |0 = Rx buffer count is less than 4.
     * |        |          |1 = Rx buffer count equals to 4.
     * |[4]     |PEF       |Receiver Parity Error Status Flag
     * |        |          |This bit is set to logic 1 whenever the received character does not have a valid parity bit.
     * |        |          |0 = Receiver parity error flag did not occur.
     * |        |          |1 = Receiver parity error flag occurred.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: If CPU sets receiver retries function by setting RXRTYEN (SCn_CTL[19]), hardware will not
     * |        |          |set this flag.
     * |[5]     |FEF       |Receiver Frame Error Status Flag
     * |        |          |This bit is set to logic 1 whenever the received character does not have a valid stop bit (that is,
     * |        |          |the stop bit following the last data bit or parity bit is detected as logic 0).
     * |        |          |0 = Receiver frame error flag did not occur.
     * |        |          |1 = Receiver frame error flag occurred.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: If CPU sets receiver retries function by setting RXRTYEN (SCn_CTL[19]), hardware will not
     * |        |          |set this flag.
     * |[6]     |BEF       |Receiver Break Error Status Flag
     * |        |          |This bit is set to logic 1 whenever the received data input (Rx) held in the spacing state
     * |        |          |(logic 0) is longer than a full word transmission time (that is, the total time of start bit +
     * |        |          |data bits + parity bit + stop bit).
     * |        |          |0 = Receiver break error flag did not occur.
     * |        |          |1 = Receiver break error flag occurred.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: If CPU sets receiver retries function by setting RXRTYEN (SCn_CTL[19]), hardware will not set
     * |        |          |this flag.
     * |[8]     |TXOV      |Transmit Overflow Error Interrupt Status Flag
     * |        |          |This bit is set when Tx buffer overflow.
     * |        |          |0 = Tx buffer is not overflow.
     * |        |          |1 = Tx buffer is overflow when Tx buffer is full and an additional write operation to DAT (SCn_DAT[7:0]).
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[9]     |TXEMPTY   |Transmit Buffer Empty Status Flag (Read Only)
     * |        |          |This bit indicates TX buffer empty or not.
     * |        |          |0 = Tx buffer is not empty.
     * |        |          |1 = Tx buffer is empty, it means the last byte of Tx buffer has been transferred to Transmitter
     * |        |          |Shift Register.
     * |        |          |Note: This bit will be cleared when writing data into DAT (SCn_DAT[7:0]).
     * |[10]    |TXFULL    |Transmit Buffer Full Status Flag (Read Only)
     * |        |          |This bit indicates Tx buffer full or not.
     * |        |          |0 = Tx buffer count is less than 4.
     * |        |          |1 = Tx buffer count equals to 4.
     * |[11]    |CREMOVE   |Card Removal Status of SCn_CD Pin
     * |        |          |This bit is set whenever card has been removal.
     * |        |          |0 = No effect.
     * |        |          |1 = Card removed.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: Card detect function will start after SCEN (SCn_CTL[0]) set.
     * |[12]    |CINSERT   |Card Insert Status of SCn_CD Pin
     * |        |          |This bit is set whenever card has been inserted.
     * |        |          |0 = No effect.
     * |        |          |1 = Card insert.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: The card detect function will start after SCEN (SCn_CTL[0]) set.
     * |[13]    |CDPINSTS  |Card Detect Pin Status (Read Only)
     * |        |          |This bit is the pin status of SCn_CD.
     * |        |          |0 = The SCn_CD pin state at low.
     * |        |          |1 = The SCn_CD pin state at high.
     * |[18:16] |RXPOINT   |Receive Buffer Pointer Status (Read Only)
     * |        |          |This field indicates the Rx buffer pointer status
     * |        |          |When SC controller receives one byte from external device, RXPOINT increases one
     * |        |          |When one byte of Rx buffer is read by CPU, RXPOINT decreases one.
     * |[21]    |RXRERR    |Receiver Retry Error
     * |        |          |This bit is used for receiver error retry and set by hardware.
     * |        |          |0 = No Rx retry transfer.
     * |        |          |1 = Rx has any error and retries transfer.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2 This bit is a flag and cannot generate any interrupt to CPU.
     * |        |          |Note3: If CPU enables receiver retries function by setting RXRTYEN (SCn_CTL[19]),
     * |        |          |hardware will not set this flag.
     * |[22]    |RXOVERR   |Receiver over Retry Error
     * |        |          |This bit is used for receiver retry counts over than retry number limitation.
     * |        |          |0 = Receiver retries counts is not over than RXRTY (SCn_CTL[18:16]) + 1.
     * |        |          |1 = Receiver retries counts over than RXRTY (SCn_CTL[18:16]) + 1.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: If CPU enables receiver retries function by setting RXRTYEN (SCn_CTL[19]), hardware
     * |        |          |will not set this flag.
     * |[23]    |RXACT     |Receiver in Active Status Flag (Read Only)
     * |        |          |This bit indicates Rx transfer status.
     * |        |          |0 = This bit is cleared automatically when Rx transfer is finished.
     * |        |          |1 = This bit is set by hardware when Rx transfer is in active.
     * |        |          |Note: This bit is read only.
     * |[26:24] |TXPOINT   |Transmit Buffer Pointer Status (Read Only)
     * |        |          |This field indicates the Tx buffer pointer status
     * |        |          |When CPU writes data into SCn_DAT, TXPOINT increases one
     * |        |          |When one byte of Tx buffer is transferred to transmitter shift register, TXPOINT decreases one.
     * |[29]    |TXRERR    |Transmitter Retry Error
     * |        |          |This bit is used for indicate transmitter error retry and set by hardware.
     * |        |          |0 = No Tx retry transfer.
     * |        |          |1 = Tx has any error and retries transfer.
     * |        |          |Note1: This bit can be cleared by writing 1 to it.
     * |        |          |Note2: This bit is a flag and cannot generate any interrupt to CPU.
     * |[30]    |TXOVERR   |Transmitter over Retry Error
     * |        |          |This bit is used for transmitter retry counts over than retry number limitation.
     * |        |          |0 = Transmitter retries counts is not over than TXRTY (SCn_CTL[22:20]) + 1.
     * |        |          |1 = Transmitter retries counts over than TXRTY (SCn_CTL[22:20]) + 1.
     * |        |          |Note: This bit can be cleared by writing 1 to it.
     * |[31]    |TXACT     |Transmit in Active Status Flag (Read Only)
     * |        |          |This bit indicates Tx transmit status.
     * |        |          |0 = This bit is cleared automatically when Tx transfer is finished or the last byte transmission
     * |        |          |has completed.
     * |        |          |1 = Transmit is active and this bit is set by hardware when Tx transfer is in active and the STOP
     * |        |          |bit of the last byte has not been transmitted.
     * |        |          |Note: This bit is read only.
     * @var SC_T::PINCTL
     * Offset: 0x24  SC Pin Control State Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PWREN     |SCn_PWR Pin Signal
     * |        |          |User can set PWRINV (SCn_PINCTL[11]) and PWREN (SCn_PINCTL[0]) to decide SCn_PWR pin is in high or low level.
     * |        |          |Write this field to drive SCn_PWR pin
     * |        |          |Refer PWRINV (SCn_PINCTL[11]) description for programming SCn_PWR pin voltage level.
     * |        |          |Read this field to get SCn_PWR signal status.
     * |        |          |0 = SCn_PWR signal status is low.
     * |        |          |1 = SCn_PWR signal status is high.
     * |        |          |Note: When operating at activation, warm reset or deactivation mode, this bit will be changed automatically.
     * |        |          |Thus, do not fill in this field when operating in these modes.
     * |[1]     |RSTEN     |SCn_RST Pin Signal
     * |        |          |User can set RSTEN (SCn_PINCTL[1]) to decide SCn_RST pin is in high or low level.
     * |        |          |Write this field to drive SCn_RST pin.
     * |        |          |0 = Drive SCn_RST pin to low.
     * |        |          |1 = Drive SCn_RST pin to high.
     * |        |          |Read this field to get SCn_RST signal status.
     * |        |          |0 = SCn_RST signal status is low.
     * |        |          |1 = SCn_RST signal status is high.
     * |        |          |Note: When operating at activation, warm reset or deactivation mode, this bit will be changed automatically.
     * |        |          |Thus, do not fill in this field when operating in these modes.
     * |[6]     |CLKKEEP   |SC Clock Enable Bit
     * |        |          |0 = SC clock generation Disabled.
     * |        |          |1 = SC clock always keeps free running.
     * |        |          |Note: When operating in activation, warm reset or deactivation mode, this bit will be changed automatically.
     * |        |          |Thus, do not fill in this field when operating in these modes.
     * |[9]     |SCDATA    |SCn_DATA Pin Signal
     * |        |          |This bit is the signal status of SCn_DATA but user can drive SCn_DATA pin to high or low by setting this bit.
     * |        |          |0 = Drive SCn_DATA pin to low.
     * |        |          |1 = Drive SCn_DATA pin to high.
     * |        |          |Read this field to get SCn_DATA signal status.
     * |        |          |0 = SCn_DATA signal status is low.
     * |        |          |1 = SCn_DATA signal status is high.
     * |        |          |Note: When SC is at activation, warm reset or deactivation mode, this bit will be changed automatically.
     * |        |          |Thus, do not fill in this field when SC is in these modes.
     * |[11]    |PWRINV    |SCn_PWR Pin Inverse
     * |        |          |This bit is used for inverse the SCn_PWR pin.
     * |        |          |There are four kinds of combination for SCn_PWR pin setting by PWRINV (SCn_PINCTL[11]) and PWREN (SCn_PINCTL[0]).
     * |        |          |PWRINV is 0 and PWREN is 0, SCn_PWR pin is 0.
     * |        |          |PWRINV is 0 and PWREN is 1, SCn_PWR pin is 1.
     * |        |          |PWRINV is 1 and PWREN is 0, SCn_PWR pin is 1.
     * |        |          |PWRINV is 1 and PWREN is 1, SCn_PWR pin is 0.
     * |        |          |Note: User must select PWRINV (SCn_PINCTL[11]) before smart card is enabled by SCEN (SCn_CTL[0]).
     * |[16]    |DATASTS   |SCn_DATA Pin Status (Read Only)
     * |        |          |This bit is the pin status of SCn_DATA.
     * |        |          |0 = The SCn_DATA pin status is low.
     * |        |          |1 = The SCn_DATA pin status is high.
     * |[17]    |PWRSTS    |SCn_PWR Pin Status (Read Only)
     * |        |          |This bit is the pin status of SCn_PWR.
     * |        |          |0 = SCn_PWR pin to low.
     * |        |          |1 = SCn_PWR pin to high.
     * |[18]    |RSTSTS    |SCn_RST Pin Status (Read Only)
     * |        |          |This bit is the pin status of SCn_RST.
     * |        |          |0 = SCn_RST pin is low.
     * |        |          |1 = SCn_RST pin is high.
     * |[30]    |SYNC      |SYNC Flag Indicator (Read Only)
     * |        |          |Due to synchronization, user should check this bit when writing a new value to SCn_PINCTL register.
     * |        |          |0 = Synchronizing is completion, user can write new data to SCn_PINCTL register.
     * |        |          |1 = Last value is synchronizing.
     * @var SC_T::TMRCTL0
     * Offset: 0x28  SC Internal Timer0 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |CNT       |Timer0 Counter Value
     * |        |          |This field indicates the internal Timer0 counter values.
     * |        |          |Note: Unit of Timer0 counter is ETU base.
     * |[27:24] |OPMODE    |Timer0 Operation Mode Selection
     * |        |          |This field indicates the internal 24-bit Timer0 operation selection.
     * |        |          |Refer to Table 7.17-3 for programming Timer0.
     * |[31]    |SYNC      |SYNC Flag Indicator (Read Only)
     * |        |          |Due to synchronization, user should check this bit when writing a new value to the SCn_TMRCTL0 register.
     * |        |          |0 = Synchronizing is completion, user can write new data to SCn_TMRCTL0 register.
     * |        |          |1 = Last value is synchronizing.
     * @var SC_T::TMRCTL1
     * Offset: 0x2C  SC Internal Timer1 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |CNT       |Timer 1 Counter Value
     * |        |          |This field indicates the internal Timer1 counter values.
     * |        |          |Note: Unit of Timer1 counter is ETU base.
     * |[27:24] |OPMODE    |Timer 1 Operation Mode Selection
     * |        |          |This field indicates the internal 8-bit Timer1 operation selection.
     * |        |          |Refer to Table 7.17-3 for programming Timer1.
     * |[31]    |SYNC      |SYNC Flag Indicator (Read Only)
     * |        |          |Due to synchronization, software should check this bit when writing a new value to SCn_TMRCTL1 register.
     * |        |          |0 = Synchronizing is completion, user can write new data to SCn_TMRCTL1 register.
     * |        |          |1 = Last value is synchronizing.
     * @var SC_T::TMRCTL2
     * Offset: 0x30  SC Internal Timer2 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |CNT       |Timer 2 Counter Value
     * |        |          |This field indicates the internal Timer2 counter values.
     * |        |          |Note: Unit of Timer2 counter is ETU base.
     * |[27:24] |OPMODE    |Timer 2 Operation Mode Selection
     * |        |          |This field indicates the internal 8-bit Timer2 operation selection
     * |        |          |Refer to Table 7.17-3 for programming Timer2.
     * |[31]    |SYNC      |SYNC Flag Indicator (Read Only)
     * |        |          |Due to synchronization, user should check this bit when writing a new value to SCn_TMRCTL2 register.
     * |        |          |0 = Synchronizing is completion, user can write new data to SCn_TMRCTL2 register.
     * |        |          |1 = Last value is synchronizing.
     * @var SC_T::UARTCTL
     * Offset: 0x34  SC UART Mode Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |UARTEN    |UART Mode Enable Bit
     * |        |          |Sets this bit to enable UART mode function.
     * |        |          |0 = Smart Card mode.
     * |        |          |1 = UART mode.
     * |        |          |Note1: When operating in UART mode, user must set CONSEL (SCn_CTL[5:4]) = 00 and AUTOCEN (SCn_CTL[3]) = 0.
     * |        |          |Note2: When operating in Smart Card mode, user must set UARTEN (SCn_UARTCTL[0]) = 0.
     * |        |          |Note3: When UART mode is enabled, hardware will generate a reset to reset FIFO and internal state machine.
     * |[5:4]   |WLS       |Word Length Selection
     * |        |          |This field is used for select UART data length.
     * |        |          |00 = Word length is 8 bits.
     * |        |          |01 = Word length is 7 bits.
     * |        |          |10 = Word length is 6 bits.
     * |        |          |11 = Word length is 5 bits.
     * |        |          |Note: In smart card mode, this WLS must be '00'.
     * |[6]     |PBOFF     |Parity Bit Disable Control
     * |        |          |Sets this bit is used for disable parity check function.
     * |        |          |0 = Parity bit is generated or checked between the last data word bit and stop bit of the serial data.
     * |        |          |1 = Parity bit is not generated (transmitting data) or checked (receiving data) during transfer.
     * |        |          |Note: In smart card mode, this field must be '0' (default setting is with parity bit).
     * |[7]     |OPE       |Odd Parity Enable Bit
     * |        |          |This is used for odd/even parity selection.
     * |        |          |0 = Even number of logic 1's are transmitted or check the data word and parity bits in receiving mode.
     * |        |          |1 = Odd number of logic 1's are transmitted or check the data word and parity bits in receiving mode.
     * |        |          |Note: This bit has effect only when PBOFF bit is '0'.
     * @var SC_T::ACTCTL
     * Offset: 0x4C  SC Activation Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4:0]   |T1EXT     |T1 Extend Time of Hardware Activation
     * |        |          |This field provide the configurable cycles to extend the activation time T1 period.
     * |        |          |The cycle scaling factor is 2048.
     * |        |          |Extend cycles = (filled value * 2048) cycles.
     * |        |          |Refer to SC activation sequence in Figure 7.17-4.
     * |        |          |For example,
     * |        |          |SCLK = 4MHz, each cycle = 0.25us,.
     * |        |          |Filled 20 to this field
     * |        |          |Extend time = 20 * 2048 * 0.25us = 10.24 ms.
     * |        |          |Note: Setting 0 to this field conforms to the protocol ISO/IEC 7816-3
     */
    __IO uint32_t DAT;                   /*!< [0x0000] SC Receive/Transmit Holding Buffer Register                      */
    __IO uint32_t CTL;                   /*!< [0x0004] SC Control Register                                              */
    __IO uint32_t ALTCTL;                /*!< [0x0008] SC Alternate Control Register                                    */
    __IO uint32_t EGT;                   /*!< [0x000c] SC Extra Guard Time Register                                     */
    __IO uint32_t RXTOUT;                /*!< [0x0010] SC Receive Buffer Time-out Counter Register                      */
    __IO uint32_t ETUCTL;                /*!< [0x0014] SC Element Time Unit Control Register                            */
    __IO uint32_t INTEN;                 /*!< [0x0018] SC Interrupt Enable Control Register                             */
    __IO uint32_t INTSTS;                /*!< [0x001c] SC Interrupt Status Register                                     */
    __IO uint32_t STATUS;                /*!< [0x0020] SC Transfer Status Register                                      */
    __IO uint32_t PINCTL;                /*!< [0x0024] SC Pin Control State Register                                    */
    __IO uint32_t TMRCTL0;               /*!< [0x0028] SC Internal Timer0 Control Register                              */
    __IO uint32_t TMRCTL1;               /*!< [0x002c] SC Internal Timer1 Control Register                              */
    __IO uint32_t TMRCTL2;               /*!< [0x0030] SC Internal Timer2 Control Register                              */
    __IO uint32_t UARTCTL;               /*!< [0x0034] SC UART Mode Control Register                                    */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE0[5];
    /** @endcond */
    __IO uint32_t ACTCTL;                /*!< [0x004c] SC Activation Control Register                                   */

} SC_T;

/**
    @addtogroup SC_CONST SC Bit Field Definition
    Constant Definitions for SC Controller
@{ */

#define SC_DAT_DAT_Pos                   (0)                                               /*!< SC_T::DAT: DAT Position                */
#define SC_DAT_DAT_Msk                   (0xfful << SC_DAT_DAT_Pos)                        /*!< SC_T::DAT: DAT Mask                    */

#define SC_CTL_SCEN_Pos                  (0)                                               /*!< SC_T::CTL: SCEN Position               */
#define SC_CTL_SCEN_Msk                  (0x1ul << SC_CTL_SCEN_Pos)                        /*!< SC_T::CTL: SCEN Mask                   */

#define SC_CTL_RXOFF_Pos                 (1)                                               /*!< SC_T::CTL: RXOFF Position              */
#define SC_CTL_RXOFF_Msk                 (0x1ul << SC_CTL_RXOFF_Pos)                       /*!< SC_T::CTL: RXOFF Mask                  */

#define SC_CTL_TXOFF_Pos                 (2)                                               /*!< SC_T::CTL: TXOFF Position              */
#define SC_CTL_TXOFF_Msk                 (0x1ul << SC_CTL_TXOFF_Pos)                       /*!< SC_T::CTL: TXOFF Mask                  */

#define SC_CTL_AUTOCEN_Pos               (3)                                               /*!< SC_T::CTL: AUTOCEN Position            */
#define SC_CTL_AUTOCEN_Msk               (0x1ul << SC_CTL_AUTOCEN_Pos)                     /*!< SC_T::CTL: AUTOCEN Mask                */

#define SC_CTL_CONSEL_Pos                (4)                                               /*!< SC_T::CTL: CONSEL Position             */
#define SC_CTL_CONSEL_Msk                (0x3ul << SC_CTL_CONSEL_Pos)                      /*!< SC_T::CTL: CONSEL Mask                 */

#define SC_CTL_RXTRGLV_Pos               (6)                                               /*!< SC_T::CTL: RXTRGLV Position            */
#define SC_CTL_RXTRGLV_Msk               (0x3ul << SC_CTL_RXTRGLV_Pos)                     /*!< SC_T::CTL: RXTRGLV Mask                */

#define SC_CTL_BGT_Pos                   (8)                                               /*!< SC_T::CTL: BGT Position                */
#define SC_CTL_BGT_Msk                   (0x1ful << SC_CTL_BGT_Pos)                        /*!< SC_T::CTL: BGT Mask                    */

#define SC_CTL_TMRSEL_Pos                (13)                                              /*!< SC_T::CTL: TMRSEL Position             */
#define SC_CTL_TMRSEL_Msk                (0x3ul << SC_CTL_TMRSEL_Pos)                      /*!< SC_T::CTL: TMRSEL Mask                 */

#define SC_CTL_NSB_Pos                   (15)                                              /*!< SC_T::CTL: NSB Position                */
#define SC_CTL_NSB_Msk                   (0x1ul << SC_CTL_NSB_Pos)                         /*!< SC_T::CTL: NSB Mask                    */

#define SC_CTL_RXRTY_Pos                 (16)                                              /*!< SC_T::CTL: RXRTY Position              */
#define SC_CTL_RXRTY_Msk                 (0x7ul << SC_CTL_RXRTY_Pos)                       /*!< SC_T::CTL: RXRTY Mask                  */

#define SC_CTL_RXRTYEN_Pos               (19)                                              /*!< SC_T::CTL: RXRTYEN Position            */
#define SC_CTL_RXRTYEN_Msk               (0x1ul << SC_CTL_RXRTYEN_Pos)                     /*!< SC_T::CTL: RXRTYEN Mask                */

#define SC_CTL_TXRTY_Pos                 (20)                                              /*!< SC_T::CTL: TXRTY Position              */
#define SC_CTL_TXRTY_Msk                 (0x7ul << SC_CTL_TXRTY_Pos)                       /*!< SC_T::CTL: TXRTY Mask                  */

#define SC_CTL_TXRTYEN_Pos               (23)                                              /*!< SC_T::CTL: TXRTYEN Position            */
#define SC_CTL_TXRTYEN_Msk               (0x1ul << SC_CTL_TXRTYEN_Pos)                     /*!< SC_T::CTL: TXRTYEN Mask                */

#define SC_CTL_CDDBSEL_Pos               (24)                                              /*!< SC_T::CTL: CDDBSEL Position            */
#define SC_CTL_CDDBSEL_Msk               (0x3ul << SC_CTL_CDDBSEL_Pos)                     /*!< SC_T::CTL: CDDBSEL Mask                */

#define SC_CTL_CDLV_Pos                  (26)                                              /*!< SC_T::CTL: CDLV Position               */
#define SC_CTL_CDLV_Msk                  (0x1ul << SC_CTL_CDLV_Pos)                        /*!< SC_T::CTL: CDLV Mask                   */

#define SC_CTL_SYNC_Pos                  (30)                                              /*!< SC_T::CTL: SYNC Position               */
#define SC_CTL_SYNC_Msk                  (0x1ul << SC_CTL_SYNC_Pos)                        /*!< SC_T::CTL: SYNC Mask                   */

#define SC_ALTCTL_TXRST_Pos              (0)                                               /*!< SC_T::ALTCTL: TXRST Position           */
#define SC_ALTCTL_TXRST_Msk              (0x1ul << SC_ALTCTL_TXRST_Pos)                    /*!< SC_T::ALTCTL: TXRST Mask               */

#define SC_ALTCTL_RXRST_Pos              (1)                                               /*!< SC_T::ALTCTL: RXRST Position           */
#define SC_ALTCTL_RXRST_Msk              (0x1ul << SC_ALTCTL_RXRST_Pos)                    /*!< SC_T::ALTCTL: RXRST Mask               */

#define SC_ALTCTL_DACTEN_Pos             (2)                                               /*!< SC_T::ALTCTL: DACTEN Position          */
#define SC_ALTCTL_DACTEN_Msk             (0x1ul << SC_ALTCTL_DACTEN_Pos)                   /*!< SC_T::ALTCTL: DACTEN Mask              */

#define SC_ALTCTL_ACTEN_Pos              (3)                                               /*!< SC_T::ALTCTL: ACTEN Position           */
#define SC_ALTCTL_ACTEN_Msk              (0x1ul << SC_ALTCTL_ACTEN_Pos)                    /*!< SC_T::ALTCTL: ACTEN Mask               */

#define SC_ALTCTL_WARSTEN_Pos            (4)                                               /*!< SC_T::ALTCTL: WARSTEN Position         */
#define SC_ALTCTL_WARSTEN_Msk            (0x1ul << SC_ALTCTL_WARSTEN_Pos)                  /*!< SC_T::ALTCTL: WARSTEN Mask             */

#define SC_ALTCTL_CNTEN0_Pos             (5)                                               /*!< SC_T::ALTCTL: CNTEN0 Position          */
#define SC_ALTCTL_CNTEN0_Msk             (0x1ul << SC_ALTCTL_CNTEN0_Pos)                   /*!< SC_T::ALTCTL: CNTEN0 Mask              */

#define SC_ALTCTL_CNTEN1_Pos             (6)                                               /*!< SC_T::ALTCTL: CNTEN1 Position          */
#define SC_ALTCTL_CNTEN1_Msk             (0x1ul << SC_ALTCTL_CNTEN1_Pos)                   /*!< SC_T::ALTCTL: CNTEN1 Mask              */

#define SC_ALTCTL_CNTEN2_Pos             (7)                                               /*!< SC_T::ALTCTL: CNTEN2 Position          */
#define SC_ALTCTL_CNTEN2_Msk             (0x1ul << SC_ALTCTL_CNTEN2_Pos)                   /*!< SC_T::ALTCTL: CNTEN2 Mask              */

#define SC_ALTCTL_INITSEL_Pos            (8)                                               /*!< SC_T::ALTCTL: INITSEL Position         */
#define SC_ALTCTL_INITSEL_Msk            (0x3ul << SC_ALTCTL_INITSEL_Pos)                  /*!< SC_T::ALTCTL: INITSEL Mask             */

#define SC_ALTCTL_ADACEN_Pos             (11)                                              /*!< SC_T::ALTCTL: ADACEN Position          */
#define SC_ALTCTL_ADACEN_Msk             (0x1ul << SC_ALTCTL_ADACEN_Pos)                   /*!< SC_T::ALTCTL: ADACEN Mask              */

#define SC_ALTCTL_RXBGTEN_Pos            (12)                                              /*!< SC_T::ALTCTL: RXBGTEN Position         */
#define SC_ALTCTL_RXBGTEN_Msk            (0x1ul << SC_ALTCTL_RXBGTEN_Pos)                  /*!< SC_T::ALTCTL: RXBGTEN Mask             */

#define SC_ALTCTL_ACTSTS0_Pos            (13)                                              /*!< SC_T::ALTCTL: ACTSTS0 Position         */
#define SC_ALTCTL_ACTSTS0_Msk            (0x1ul << SC_ALTCTL_ACTSTS0_Pos)                  /*!< SC_T::ALTCTL: ACTSTS0 Mask             */

#define SC_ALTCTL_ACTSTS1_Pos            (14)                                              /*!< SC_T::ALTCTL: ACTSTS1 Position         */
#define SC_ALTCTL_ACTSTS1_Msk            (0x1ul << SC_ALTCTL_ACTSTS1_Pos)                  /*!< SC_T::ALTCTL: ACTSTS1 Mask             */

#define SC_ALTCTL_ACTSTS2_Pos            (15)                                              /*!< SC_T::ALTCTL: ACTSTS2 Position         */
#define SC_ALTCTL_ACTSTS2_Msk            (0x1ul << SC_ALTCTL_ACTSTS2_Pos)                  /*!< SC_T::ALTCTL: ACTSTS2 Mask             */

#define SC_ALTCTL_SYNC_Pos               (31)                                              /*!< SC_T::ALTCTL: SYNC Position            */
#define SC_ALTCTL_SYNC_Msk               (0x1ul << SC_ALTCTL_SYNC_Pos)                     /*!< SC_T::ALTCTL: SYNC Mask                */

#define SC_EGT_EGT_Pos                   (0)                                               /*!< SC_T::EGT: EGT Position                */
#define SC_EGT_EGT_Msk                   (0xfful << SC_EGT_EGT_Pos)                        /*!< SC_T::EGT: EGT Mask                    */

#define SC_RXTOUT_RFTM_Pos               (0)                                               /*!< SC_T::RXTOUT: RFTM Position            */
#define SC_RXTOUT_RFTM_Msk               (0x1fful << SC_RXTOUT_RFTM_Pos)                   /*!< SC_T::RXTOUT: RFTM Mask                */

#define SC_ETUCTL_ETURDIV_Pos            (0)                                               /*!< SC_T::ETUCTL: ETURDIV Position         */
#define SC_ETUCTL_ETURDIV_Msk            (0xffful << SC_ETUCTL_ETURDIV_Pos)                /*!< SC_T::ETUCTL: ETURDIV Mask             */

#define SC_INTEN_RDAIEN_Pos              (0)                                               /*!< SC_T::INTEN: RDAIEN Position           */
#define SC_INTEN_RDAIEN_Msk              (0x1ul << SC_INTEN_RDAIEN_Pos)                    /*!< SC_T::INTEN: RDAIEN Mask               */

#define SC_INTEN_TBEIEN_Pos              (1)                                               /*!< SC_T::INTEN: TBEIEN Position           */
#define SC_INTEN_TBEIEN_Msk              (0x1ul << SC_INTEN_TBEIEN_Pos)                    /*!< SC_T::INTEN: TBEIEN Mask               */

#define SC_INTEN_TERRIEN_Pos             (2)                                               /*!< SC_T::INTEN: TERRIEN Position          */
#define SC_INTEN_TERRIEN_Msk             (0x1ul << SC_INTEN_TERRIEN_Pos)                   /*!< SC_T::INTEN: TERRIEN Mask              */

#define SC_INTEN_TMR0IEN_Pos             (3)                                               /*!< SC_T::INTEN: TMR0IEN Position          */
#define SC_INTEN_TMR0IEN_Msk             (0x1ul << SC_INTEN_TMR0IEN_Pos)                   /*!< SC_T::INTEN: TMR0IEN Mask              */

#define SC_INTEN_TMR1IEN_Pos             (4)                                               /*!< SC_T::INTEN: TMR1IEN Position          */
#define SC_INTEN_TMR1IEN_Msk             (0x1ul << SC_INTEN_TMR1IEN_Pos)                   /*!< SC_T::INTEN: TMR1IEN Mask              */

#define SC_INTEN_TMR2IEN_Pos             (5)                                               /*!< SC_T::INTEN: TMR2IEN Position          */
#define SC_INTEN_TMR2IEN_Msk             (0x1ul << SC_INTEN_TMR2IEN_Pos)                   /*!< SC_T::INTEN: TMR2IEN Mask              */

#define SC_INTEN_BGTIEN_Pos              (6)                                               /*!< SC_T::INTEN: BGTIEN Position           */
#define SC_INTEN_BGTIEN_Msk              (0x1ul << SC_INTEN_BGTIEN_Pos)                    /*!< SC_T::INTEN: BGTIEN Mask               */

#define SC_INTEN_CDIEN_Pos               (7)                                               /*!< SC_T::INTEN: CDIEN Position            */
#define SC_INTEN_CDIEN_Msk               (0x1ul << SC_INTEN_CDIEN_Pos)                     /*!< SC_T::INTEN: CDIEN Mask                */

#define SC_INTEN_INITIEN_Pos             (8)                                               /*!< SC_T::INTEN: INITIEN Position          */
#define SC_INTEN_INITIEN_Msk             (0x1ul << SC_INTEN_INITIEN_Pos)                   /*!< SC_T::INTEN: INITIEN Mask              */

#define SC_INTEN_RXTOIEN_Pos             (9)                                               /*!< SC_T::INTEN: RXTOIEN Position          */
#define SC_INTEN_RXTOIEN_Msk             (0x1ul << SC_INTEN_RXTOIEN_Pos)                   /*!< SC_T::INTEN: RXTOIEN Mask              */

#define SC_INTEN_ACERRIEN_Pos            (10)                                              /*!< SC_T::INTEN: ACERRIEN Position         */
#define SC_INTEN_ACERRIEN_Msk            (0x1ul << SC_INTEN_ACERRIEN_Pos)                  /*!< SC_T::INTEN: ACERRIEN Mask             */

#define SC_INTSTS_RDAIF_Pos              (0)                                               /*!< SC_T::INTSTS: RDAIF Position           */
#define SC_INTSTS_RDAIF_Msk              (0x1ul << SC_INTSTS_RDAIF_Pos)                    /*!< SC_T::INTSTS: RDAIF Mask               */

#define SC_INTSTS_TBEIF_Pos              (1)                                               /*!< SC_T::INTSTS: TBEIF Position           */
#define SC_INTSTS_TBEIF_Msk              (0x1ul << SC_INTSTS_TBEIF_Pos)                    /*!< SC_T::INTSTS: TBEIF Mask               */

#define SC_INTSTS_TERRIF_Pos             (2)                                               /*!< SC_T::INTSTS: TERRIF Position          */
#define SC_INTSTS_TERRIF_Msk             (0x1ul << SC_INTSTS_TERRIF_Pos)                   /*!< SC_T::INTSTS: TERRIF Mask              */

#define SC_INTSTS_TMR0IF_Pos             (3)                                               /*!< SC_T::INTSTS: TMR0IF Position          */
#define SC_INTSTS_TMR0IF_Msk             (0x1ul << SC_INTSTS_TMR0IF_Pos)                   /*!< SC_T::INTSTS: TMR0IF Mask              */

#define SC_INTSTS_TMR1IF_Pos             (4)                                               /*!< SC_T::INTSTS: TMR1IF Position          */
#define SC_INTSTS_TMR1IF_Msk             (0x1ul << SC_INTSTS_TMR1IF_Pos)                   /*!< SC_T::INTSTS: TMR1IF Mask              */

#define SC_INTSTS_TMR2IF_Pos             (5)                                               /*!< SC_T::INTSTS: TMR2IF Position          */
#define SC_INTSTS_TMR2IF_Msk             (0x1ul << SC_INTSTS_TMR2IF_Pos)                   /*!< SC_T::INTSTS: TMR2IF Mask              */

#define SC_INTSTS_BGTIF_Pos              (6)                                               /*!< SC_T::INTSTS: BGTIF Position           */
#define SC_INTSTS_BGTIF_Msk              (0x1ul << SC_INTSTS_BGTIF_Pos)                    /*!< SC_T::INTSTS: BGTIF Mask               */

#define SC_INTSTS_CDIF_Pos               (7)                                               /*!< SC_T::INTSTS: CDIF Position            */
#define SC_INTSTS_CDIF_Msk               (0x1ul << SC_INTSTS_CDIF_Pos)                     /*!< SC_T::INTSTS: CDIF Mask                */

#define SC_INTSTS_INITIF_Pos             (8)                                               /*!< SC_T::INTSTS: INITIF Position          */
#define SC_INTSTS_INITIF_Msk             (0x1ul << SC_INTSTS_INITIF_Pos)                   /*!< SC_T::INTSTS: INITIF Mask              */

#define SC_INTSTS_RXTOIF_Pos             (9)                                               /*!< SC_T::INTSTS: RXTOIF Position          */
#define SC_INTSTS_RXTOIF_Msk             (0x1ul << SC_INTSTS_RXTOIF_Pos)                   /*!< SC_T::INTSTS: RXTOIF Mask              */

#define SC_INTSTS_ACERRIF_Pos            (10)                                              /*!< SC_T::INTSTS: ACERRIF Position         */
#define SC_INTSTS_ACERRIF_Msk            (0x1ul << SC_INTSTS_ACERRIF_Pos)                  /*!< SC_T::INTSTS: ACERRIF Mask             */

#define SC_STATUS_RXOV_Pos               (0)                                               /*!< SC_T::STATUS: RXOV Position            */
#define SC_STATUS_RXOV_Msk               (0x1ul << SC_STATUS_RXOV_Pos)                     /*!< SC_T::STATUS: RXOV Mask                */

#define SC_STATUS_RXEMPTY_Pos            (1)                                               /*!< SC_T::STATUS: RXEMPTY Position         */
#define SC_STATUS_RXEMPTY_Msk            (0x1ul << SC_STATUS_RXEMPTY_Pos)                  /*!< SC_T::STATUS: RXEMPTY Mask             */

#define SC_STATUS_RXFULL_Pos             (2)                                               /*!< SC_T::STATUS: RXFULL Position          */
#define SC_STATUS_RXFULL_Msk             (0x1ul << SC_STATUS_RXFULL_Pos)                   /*!< SC_T::STATUS: RXFULL Mask              */

#define SC_STATUS_PEF_Pos                (4)                                               /*!< SC_T::STATUS: PEF Position             */
#define SC_STATUS_PEF_Msk                (0x1ul << SC_STATUS_PEF_Pos)                      /*!< SC_T::STATUS: PEF Mask                 */

#define SC_STATUS_FEF_Pos                (5)                                               /*!< SC_T::STATUS: FEF Position             */
#define SC_STATUS_FEF_Msk                (0x1ul << SC_STATUS_FEF_Pos)                      /*!< SC_T::STATUS: FEF Mask                 */

#define SC_STATUS_BEF_Pos                (6)                                               /*!< SC_T::STATUS: BEF Position             */
#define SC_STATUS_BEF_Msk                (0x1ul << SC_STATUS_BEF_Pos)                      /*!< SC_T::STATUS: BEF Mask                 */

#define SC_STATUS_TXOV_Pos               (8)                                               /*!< SC_T::STATUS: TXOV Position            */
#define SC_STATUS_TXOV_Msk               (0x1ul << SC_STATUS_TXOV_Pos)                     /*!< SC_T::STATUS: TXOV Mask                */

#define SC_STATUS_TXEMPTY_Pos            (9)                                               /*!< SC_T::STATUS: TXEMPTY Position         */
#define SC_STATUS_TXEMPTY_Msk            (0x1ul << SC_STATUS_TXEMPTY_Pos)                  /*!< SC_T::STATUS: TXEMPTY Mask             */

#define SC_STATUS_TXFULL_Pos             (10)                                              /*!< SC_T::STATUS: TXFULL Position          */
#define SC_STATUS_TXFULL_Msk             (0x1ul << SC_STATUS_TXFULL_Pos)                   /*!< SC_T::STATUS: TXFULL Mask              */

#define SC_STATUS_CREMOVE_Pos            (11)                                              /*!< SC_T::STATUS: CREMOVE Position         */
#define SC_STATUS_CREMOVE_Msk            (0x1ul << SC_STATUS_CREMOVE_Pos)                  /*!< SC_T::STATUS: CREMOVE Mask             */

#define SC_STATUS_CINSERT_Pos            (12)                                              /*!< SC_T::STATUS: CINSERT Position         */
#define SC_STATUS_CINSERT_Msk            (0x1ul << SC_STATUS_CINSERT_Pos)                  /*!< SC_T::STATUS: CINSERT Mask             */

#define SC_STATUS_CDPINSTS_Pos           (13)                                              /*!< SC_T::STATUS: CDPINSTS Position        */
#define SC_STATUS_CDPINSTS_Msk           (0x1ul << SC_STATUS_CDPINSTS_Pos)                 /*!< SC_T::STATUS: CDPINSTS Mask            */

#define SC_STATUS_RXPOINT_Pos            (16)                                              /*!< SC_T::STATUS: RXPOINT Position         */
#define SC_STATUS_RXPOINT_Msk            (0x7ul << SC_STATUS_RXPOINT_Pos)                  /*!< SC_T::STATUS: RXPOINT Mask             */

#define SC_STATUS_RXRERR_Pos             (21)                                              /*!< SC_T::STATUS: RXRERR Position          */
#define SC_STATUS_RXRERR_Msk             (0x1ul << SC_STATUS_RXRERR_Pos)                   /*!< SC_T::STATUS: RXRERR Mask              */

#define SC_STATUS_RXOVERR_Pos            (22)                                              /*!< SC_T::STATUS: RXOVERR Position         */
#define SC_STATUS_RXOVERR_Msk            (0x1ul << SC_STATUS_RXOVERR_Pos)                  /*!< SC_T::STATUS: RXOVERR Mask             */

#define SC_STATUS_RXACT_Pos              (23)                                              /*!< SC_T::STATUS: RXACT Position           */
#define SC_STATUS_RXACT_Msk              (0x1ul << SC_STATUS_RXACT_Pos)                    /*!< SC_T::STATUS: RXACT Mask               */

#define SC_STATUS_TXPOINT_Pos            (24)                                              /*!< SC_T::STATUS: TXPOINT Position         */
#define SC_STATUS_TXPOINT_Msk            (0x7ul << SC_STATUS_TXPOINT_Pos)                  /*!< SC_T::STATUS: TXPOINT Mask             */

#define SC_STATUS_TXRERR_Pos             (29)                                              /*!< SC_T::STATUS: TXRERR Position          */
#define SC_STATUS_TXRERR_Msk             (0x1ul << SC_STATUS_TXRERR_Pos)                   /*!< SC_T::STATUS: TXRERR Mask              */

#define SC_STATUS_TXOVERR_Pos            (30)                                              /*!< SC_T::STATUS: TXOVERR Position         */
#define SC_STATUS_TXOVERR_Msk            (0x1ul << SC_STATUS_TXOVERR_Pos)                  /*!< SC_T::STATUS: TXOVERR Mask             */

#define SC_STATUS_TXACT_Pos              (31)                                              /*!< SC_T::STATUS: TXACT Position           */
#define SC_STATUS_TXACT_Msk              (0x1ul << SC_STATUS_TXACT_Pos)                    /*!< SC_T::STATUS: TXACT Mask               */

#define SC_PINCTL_PWREN_Pos              (0)                                               /*!< SC_T::PINCTL: PWREN Position           */
#define SC_PINCTL_PWREN_Msk              (0x1ul << SC_PINCTL_PWREN_Pos)                    /*!< SC_T::PINCTL: PWREN Mask               */

#define SC_PINCTL_RSTEN_Pos              (1)                                               /*!< SC_T::PINCTL: RSTEN Position           */
#define SC_PINCTL_RSTEN_Msk              (0x1ul << SC_PINCTL_RSTEN_Pos)                    /*!< SC_T::PINCTL: RSTEN Mask               */

#define SC_PINCTL_CLKKEEP_Pos            (6)                                               /*!< SC_T::PINCTL: CLKKEEP Position         */
#define SC_PINCTL_CLKKEEP_Msk            (0x1ul << SC_PINCTL_CLKKEEP_Pos)                  /*!< SC_T::PINCTL: CLKKEEP Mask             */

#define SC_PINCTL_SCDATA_Pos             (9)                                               /*!< SC_T::PINCTL: SCDATA Position          */
#define SC_PINCTL_SCDATA_Msk             (0x1ul << SC_PINCTL_SCDATA_Pos)                   /*!< SC_T::PINCTL: SCDATA Mask              */

#define SC_PINCTL_PWRINV_Pos             (11)                                              /*!< SC_T::PINCTL: PWRINV Position          */
#define SC_PINCTL_PWRINV_Msk             (0x1ul << SC_PINCTL_PWRINV_Pos)                   /*!< SC_T::PINCTL: PWRINV Mask              */

#define SC_PINCTL_DATASTS_Pos            (16)                                              /*!< SC_T::PINCTL: DATASTS Position         */
#define SC_PINCTL_DATASTS_Msk            (0x1ul << SC_PINCTL_DATASTS_Pos)                  /*!< SC_T::PINCTL: DATASTS Mask             */

#define SC_PINCTL_PWRSTS_Pos             (17)                                              /*!< SC_T::PINCTL: PWRSTS Position          */
#define SC_PINCTL_PWRSTS_Msk             (0x1ul << SC_PINCTL_PWRSTS_Pos)                   /*!< SC_T::PINCTL: PWRSTS Mask              */

#define SC_PINCTL_RSTSTS_Pos             (18)                                              /*!< SC_T::PINCTL: RSTSTS Position          */
#define SC_PINCTL_RSTSTS_Msk             (0x1ul << SC_PINCTL_RSTSTS_Pos)                   /*!< SC_T::PINCTL: RSTSTS Mask              */

#define SC_PINCTL_SYNC_Pos               (30)                                              /*!< SC_T::PINCTL: SYNC Position            */
#define SC_PINCTL_SYNC_Msk               (0x1ul << SC_PINCTL_SYNC_Pos)                     /*!< SC_T::PINCTL: SYNC Mask                */

#define SC_TMRCTL0_CNT_Pos               (0)                                               /*!< SC_T::TMRCTL0: CNT Position            */
#define SC_TMRCTL0_CNT_Msk               (0xfffffful << SC_TMRCTL0_CNT_Pos)                /*!< SC_T::TMRCTL0: CNT Mask                */

#define SC_TMRCTL0_OPMODE_Pos            (24)                                              /*!< SC_T::TMRCTL0: OPMODE Position         */
#define SC_TMRCTL0_OPMODE_Msk            (0xful << SC_TMRCTL0_OPMODE_Pos)                  /*!< SC_T::TMRCTL0: OPMODE Mask             */

#define SC_TMRCTL0_SYNC_Pos              (31)                                              /*!< SC_T::TMRCTL0: SYNC Position           */
#define SC_TMRCTL0_SYNC_Msk              (0x1ul << SC_TMRCTL0_SYNC_Pos)                    /*!< SC_T::TMRCTL0: SYNC Mask               */

#define SC_TMRCTL1_CNT_Pos               (0)                                               /*!< SC_T::TMRCTL1: CNT Position            */
#define SC_TMRCTL1_CNT_Msk               (0xfful << SC_TMRCTL1_CNT_Pos)                    /*!< SC_T::TMRCTL1: CNT Mask                */

#define SC_TMRCTL1_OPMODE_Pos            (24)                                              /*!< SC_T::TMRCTL1: OPMODE Position         */
#define SC_TMRCTL1_OPMODE_Msk            (0xful << SC_TMRCTL1_OPMODE_Pos)                  /*!< SC_T::TMRCTL1: OPMODE Mask             */

#define SC_TMRCTL1_SYNC_Pos              (31)                                              /*!< SC_T::TMRCTL1: SYNC Position           */
#define SC_TMRCTL1_SYNC_Msk              (0x1ul << SC_TMRCTL1_SYNC_Pos)                    /*!< SC_T::TMRCTL1: SYNC Mask               */

#define SC_TMRCTL2_CNT_Pos               (0)                                               /*!< SC_T::TMRCTL2: CNT Position            */
#define SC_TMRCTL2_CNT_Msk               (0xfful << SC_TMRCTL2_CNT_Pos)                    /*!< SC_T::TMRCTL2: CNT Mask                */

#define SC_TMRCTL2_OPMODE_Pos            (24)                                              /*!< SC_T::TMRCTL2: OPMODE Position         */
#define SC_TMRCTL2_OPMODE_Msk            (0xful << SC_TMRCTL2_OPMODE_Pos)                  /*!< SC_T::TMRCTL2: OPMODE Mask             */

#define SC_TMRCTL2_SYNC_Pos              (31)                                              /*!< SC_T::TMRCTL2: SYNC Position           */
#define SC_TMRCTL2_SYNC_Msk              (0x1ul << SC_TMRCTL2_SYNC_Pos)                    /*!< SC_T::TMRCTL2: SYNC Mask               */

#define SC_UARTCTL_UARTEN_Pos            (0)                                               /*!< SC_T::UARTCTL: UARTEN Position         */
#define SC_UARTCTL_UARTEN_Msk            (0x1ul << SC_UARTCTL_UARTEN_Pos)                  /*!< SC_T::UARTCTL: UARTEN Mask             */

#define SC_UARTCTL_WLS_Pos               (4)                                               /*!< SC_T::UARTCTL: WLS Position            */
#define SC_UARTCTL_WLS_Msk               (0x3ul << SC_UARTCTL_WLS_Pos)                     /*!< SC_T::UARTCTL: WLS Mask                */

#define SC_UARTCTL_PBOFF_Pos             (6)                                               /*!< SC_T::UARTCTL: PBOFF Position          */
#define SC_UARTCTL_PBOFF_Msk             (0x1ul << SC_UARTCTL_PBOFF_Pos)                   /*!< SC_T::UARTCTL: PBOFF Mask              */

#define SC_UARTCTL_OPE_Pos               (7)                                               /*!< SC_T::UARTCTL: OPE Position            */
#define SC_UARTCTL_OPE_Msk               (0x1ul << SC_UARTCTL_OPE_Pos)                     /*!< SC_T::UARTCTL: OPE Mask                */

#define SC_ACTCTL_T1EXT_Pos              (0)                                               /*!< SC_T::ACTCTL: T1EXT Position           */
#define SC_ACTCTL_T1EXT_Msk              (0x1ful << SC_ACTCTL_T1EXT_Pos)                   /*!< SC_T::ACTCTL: T1EXT Mask               */

/**@}*/ /* SC_CONST */
/**@}*/ /* end of SC register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __SC_REG_H__ */
