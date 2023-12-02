/**************************************************************************//**
 * @file     can_reg.h
 * @version  V1.00
 * @brief    CAN register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CAN_REG_H__
#define __CAN_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup CAN Controller Area Network Controller(CAN)
    Memory Mapped Structure for CAN Controller
@{ */


typedef struct
{

    /**
     * @var CAN_IF_T::CREQ
     * Offset: 0x20, 0x80  IFn Command Request Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[5:0]   |MessageNumber|Message Number
     * |        |          |0x01-0x20: Valid Message Number, the Message Object in the Message
     * |        |          |RAM is selected for data transfer.
     * |        |          |0x00: Not a valid Message Number, interpreted as 0x20.
     * |        |          |0x21-0x3F: Not a valid Message Number, interpreted as 0x01-0x1F.
     * |[15]    |Busy      |Busy Flag
     * |        |          |0 = Read/write action has finished.
     * |        |          |1 = Writing to the IFn Command Request Register is in progress
     * |        |          |This bit can only be read by the software.
     * @var CAN_IF_T::CMASK
     * Offset: 0x24, 0x84  IFn Command Mask Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |DAT_B     |Access Data Bytes [7:4]
     * |        |          |Write Operation:
     * |        |          |0 = Data Bytes [7:4] unchanged.
     * |        |          |1 = Transfer Data Bytes [7:4] to Message Object.
     * |        |          |Read Operation:
     * |        |          |0 = Data Bytes [7:4] unchanged.
     * |        |          |1 = Transfer Data Bytes [7:4] to IFn Message Buffer Register.
     * |[1]     |DAT_A     |Access Data Bytes [3:0]
     * |        |          |Write Operation:
     * |        |          |0 = Data Bytes [3:0] unchanged.
     * |        |          |1 = Transfer Data Bytes [3:0] to Message Object.
     * |        |          |Read Operation:
     * |        |          |0 = Data Bytes [3:0] unchanged.
     * |        |          |1 = Transfer Data Bytes [3:0] to IFn Message Buffer Register.
     * |[2]     |TxRqst_NewDat|Access Transmission Request Bit When Write Operation
     * |        |          |0 = TxRqst bit unchanged.
     * |        |          |1 = Set TxRqst bit.
     * |        |          |Note: If a transmission is requested by programming bit TxRqst/NewDat in the IFn Command Mask Register, bit TxRqst in the IFn Message Control Register will be ignored.
     * |        |          |Access New Data Bit when Read Operation.
     * |        |          |0 = NewDat bit remains unchanged.
     * |        |          |1 = Clear NewDat bit in the Message Object.
     * |        |          |Note: A read access to a Message Object can be combined with the reset of the control bits IntPnd and NewDat
     * |        |          |The values of these bits transferred to the IFn Message Control Register always reflect the status before resetting these bits.
     * |[3]     |ClrIntPnd |Clear Interrupt Pending Bit
     * |        |          |Write Operation:
     * |        |          |When writing to a Message Object, this bit is ignored.
     * |        |          |Read Operation:
     * |        |          |0 = IntPnd bit (CAN_IFn_MCON[13]) remains unchanged.
     * |        |          |1 = Clear IntPnd bit in the Message Object.
     * |[4]     |Control   |Control Access Control Bits
     * |        |          |Write Operation:
     * |        |          |0 = Control Bits unchanged.
     * |        |          |1 = Transfer Control Bits to Message Object.
     * |        |          |Read Operation:
     * |        |          |0 = Control Bits unchanged.
     * |        |          |1 = Transfer Control Bits to IFn Message Buffer Register.
     * |[5]     |Arb       |Access Arbitration Bits
     * |        |          |Write Operation:
     * |        |          |0 = Arbitration bits unchanged.
     * |        |          |1 = Transfer Identifier + Dir (CAN_IFn_ARB2[13]) + Xtd (CAN_IFn_ARB2[14]) + MsgVal (CAN_IFn_ARB2[15]) to Message Object.
     * |        |          |Read Operation:
     * |        |          |0 = Arbitration bits unchanged.
     * |        |          |1 = Transfer Identifier + Dir + Xtd + MsgVal to IFn Message Buffer Register.
     * |[6]     |Mask      |Access Mask Bits
     * |        |          |Write Operation:
     * |        |          |0 = Mask bits unchanged.
     * |        |          |1 = Transfer Identifier Mask + MDir + MXtd to Message Object.
     * |        |          |Read Operation:
     * |        |          |0 = Mask bits unchanged.
     * |        |          |1 = Transfer Identifier Mask + MDir + MXtd to IFn Message Buffer Register.
     * |[7]     |WR_RD     |Write / Read Mode
     * |        |          |0 = Read: Transfer data from the Message Object addressed by the Command Request Register into the selected Message Buffer Registers.
     * |        |          |1 = Write: Transfer data from the selected Message Buffer Registers to the Message Object addressed by the Command Request Register.
     * @var CAN_IF_T::MASK1
     * Offset: 0x28, 0x88  IFn Mask 1 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |Msk       |Identifier Mask 15-0
     * |        |          |0 = The corresponding bit in the identifier of the message object cannot inhibit the match in the acceptance filtering.
     * |        |          |1 = The corresponding identifier bit is used for acceptance filtering.
     * @var CAN_IF_T::MASK2
     * Offset: 0x2C, 0x8C  IFn Mask 2 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[12:0]  |Msk       |Identifier Mask 28-16
     * |        |          |0 = The corresponding bit in the identifier of the message object cannot inhibit the match in the acceptance filtering.
     * |        |          |1 = The corresponding identifier bit is used for acceptance filtering.
     * |[14]    |MDir      |Mask Message Direction
     * |        |          |0 = The message direction bit (Dir (CAN_IFn_ARB2[13])) has no effect on the acceptance filtering.
     * |        |          |1 = The message direction bit (Dir) is used for acceptance filtering.
     * |[15]    |MXtd      |Mask Extended Identifier
     * |        |          |0 = The extended identifier bit (IDE) has no effect on the acceptance filtering.
     * |        |          |1 = The extended identifier bit (IDE) is used for acceptance filtering.
     * |        |          |Note: When 11-bit (standard) Identifiers are used for a Message Object, the identifiers of received Data Frames are written into bits ID28 to ID18 (CAN_IFn_ARB2[12:2])
     * |        |          |For acceptance filtering, only these bits together with mask bits Msk28 to Msk18 (CAN_IFn_MASK2[12:2]) are considered.
     * @var CAN_IF_T::ARB1
     * Offset: 0x30, 0x90  IFn Arbitration 1 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ID        |Message Identifier 15-0
     * |        |          |ID28 - ID0, 29-bit Identifier (Extended Frame)
     * |        |          |ID28 - ID18, 11-bit Identifier (Standard Frame)
     * @var CAN_IF_T::ARB2
     * Offset: 0x34, 0x94  IFn Arbitration 2 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[12:0]  |ID        |Message Identifier 28-16
     * |        |          |ID28 - ID0, 29-bit Identifier (Extended Frame)
     * |        |          |ID28 - ID18, 11-bit Identifier (Standard Frame)
     * |[13]    |Dir       |Message Direction
     * |        |          |0 = Direction is receive.
     * |        |          |On TxRqst, a Remote Frame with the identifier of this Message Object is transmitted
     * |        |          |On reception of a Data Frame with matching identifier, that message is stored in this Message Object.
     * |        |          |1 = Direction is transmit.
     * |        |          |On TxRqst, the respective Message Object is transmitted as a Data Frame
     * |        |          |On reception of a Remote Frame with matching identifier, the TxRqst bit (CAN_IFn_CMASK[2]) of this Message Object is set (if RmtEn (CAN_IFn_MCON[9]) = one).
     * |[14]    |Xtd       |Extended Identifier
     * |        |          |0 = The 11-bit (standard) Identifier will be used for this Message Object.
     * |        |          |1 = The 29-bit (extended) Identifier will be used for this Message Object.
     * |[15]    |MsgVal    |Message Valid
     * |        |          |0 = The Message Object is ignored by the Message Handler.
     * |        |          |1 = The Message Object is configured and should be considered by the Message Handler.
     * |        |          |Note: The application software must reset the MsgVal bit of all unused Messages Objects during the initialization before it resets bit Init (CAN_CON[0])
     * |        |          |This bit must also be reset before the identifier Id28-0 (CAN_IFn_ARB1/2), the control bits Xtd (CAN_IFn_ARB2[14]), Dir (CAN_IFn_ARB2[13]), or the Data Length Code DLC3-0 (CAN_IFn_MCON[3:0]) are modified, or if the Messages Object is no longer required.
     * @var CAN_IF_T::MCON
     * Offset: 0x38, 0x98  IFn Message Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |DLC       |Data Length Code
     * |        |          |0-8: Data Frame has 0-8 data bytes.
     * |        |          |9-15: Data Frame has 8 data bytes
     * |        |          |Note: The Data Length Code of a Message Object must be defined the same as in all the corresponding objects with the same identifier at other nodes
     * |        |          |When the Message Handler stores a data frame, it will write the DLC to the value given by the received message.
     * |        |          |Data(0): 1st data byte of a CAN Data Frame
     * |        |          |Data(1): 2nd data byte of a CAN Data Frame
     * |        |          |Data(2): 3rd data byte of a CAN Data Frame
     * |        |          |Data(3): 4th data byte of a CAN Data Frame
     * |        |          |Data(4): 5th data byte of a CAN Data Frame
     * |        |          |Data(5): 6th data byte of a CAN Data Frame
     * |        |          |Data(6): 7th data byte of a CAN Data Frame
     * |        |          |Data(7): 8th data byte of a CAN Data Frame
     * |        |          |Note: The Data(0) byte is the first data byte shifted into the shift register of the CAN Core during a reception while the Data(7) byte is the last
     * |        |          |When the Message Handler stores a Data Frame, it will write all the eight data bytes into a Message Object
     * |        |          |If the Data Length Code is less than 8, the remaining bytes of the Message Object will be overwritten by unspecified values.
     * |[7]     |EoB       |End of Buffer
     * |        |          |0 = Message Object belongs to a FIFO Buffer and is not the last Message Object of that FIFO Buffer.
     * |        |          |1 = Single Message Object or last Message Object of a FIFO Buffer.
     * |        |          |Note: This bit is used to concatenate two or more Message Objects (up to 32) to build a FIFO Buffer
     * |        |          |For single Message Objects (not belonging to a FIFO Buffer), this bit must always be set to one
     * |[8]     |TxRqst    |Transmit Request
     * |        |          |0 = This Message Object is not waiting for transmission.
     * |        |          |1 = The transmission of this Message Object is requested and is not yet done.
     * |[9]     |RmtEn     |Remote Enable Bit
     * |        |          |0 = At the reception of a Remote Frame, TxRqst (CAN_IFn_MCON[8]) is left unchanged.
     * |        |          |1 = At the reception of a Remote Frame, TxRqst is set.
     * |[10]    |RxIE      |Receive Interrupt Enable Bit
     * |        |          |0 = IntPnd (CAN_IFn_MCON[13]) will be left unchanged after a successful reception of a frame.
     * |        |          |1 = IntPnd will be set after a successful reception of a frame.
     * |[11]    |TxIE      |Transmit Interrupt Enable Bit
     * |        |          |0 = IntPnd (CAN_IFn_MCON[13]) will be left unchanged after the successful transmission of a frame.
     * |        |          |1 = IntPnd will be set after a successful transmission of a frame.
     * |[12]    |UMask     |Use Acceptance Mask
     * |        |          |0 = Mask ignored.
     * |        |          |1 = Use Mask (Msk28-0, MXtd, and MDir) for acceptance filtering.
     * |        |          |Note: If the UMask bit is set to one, the Message Object's mask bits have to be programmed during initialization of the Message Object before MsgVal bit (CAN_IFn_ARB2[15]) is set to one.
     * |[13]    |IntPnd    |Interrupt Pending
     * |        |          |0 = This message object is not the source of an interrupt.
     * |        |          |1 = This message object is the source of an interrupt
     * |        |          |The Interrupt Identifier in the Interrupt Register will point to this message object if there is no other interrupt source with higher priority.
     * |[14]    |MsgLst    |Message Lost (only valid for Message Objects with direction = receive).
     * |        |          |0 = No message lost since last time this bit was reset by the CPU.
     * |        |          |1 = The Message Handler stored a new message into this object when NewDat was still set, the CPU has lost a message.
     * |[15]    |NewDat    |New Data
     * |        |          |0 = No new data has been written into the data portion of this Message Object by the Message Handler since last time this flag was cleared by the application software.
     * |        |          |1 = The Message Handler or the application software has written new data into the data portion of this Message Object.
     * @var CAN_IF_T::DAT_A1
     * Offset: 0x3C, 0x9C  IFn Data A1 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |Data_0_   |Data Byte 0
     * |        |          |1st data byte of a CAN Data Frame
     * |[15:8]  |Data_1_   |Data Byte 1
     * |        |          |2nd data byte of a CAN Data Frame
     * @var CAN_IF_T::DAT_A2
     * Offset: 0x40, 0xA0  IFn Data A2 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |Data_2_   |Data Byte 2
     * |        |          |3rd data byte of CAN Data Frame
     * |[15:8]  |Data_3_   |Data Byte 3
     * |        |          |4th data byte of CAN Data Frame
     * @var CAN_IF_T::DAT_B1
     * Offset: 0x44, 0xA4  IFn Data B1 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |Data_4_   |Data Byte 4
     * |        |          |5th data byte of CAN Data Frame
     * |[15:8]  |Data_5_   |Data Byte 5
     * |        |          |6th data byte of CAN Data Frame
     * @var CAN_IF_T::DAT_B2
     * Offset: 0x48, 0xA8  IFn Data B2 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |Data_6_   |Data Byte 6
     * |        |          |7th data byte of CAN Data Frame.
     * |[15:8]  |Data_7_   |Data Byte 7
     * |        |          |8th data byte of CAN Data Frame.
     */
    __IO uint32_t CREQ;         /*!< [0x0020] IFn Command Request Register                                     */
    __IO uint32_t CMASK;        /*!< [0x0024] IFn Command Mask Register                                        */
    __IO uint32_t MASK1;        /*!< [0x0028] IFn Mask 1 Register                                              */
    __IO uint32_t MASK2;        /*!< [0x002c] IFn Mask 2 Register                                              */
    __IO uint32_t ARB1;         /*!< [0x0030] IFn Arbitration 1 Register                                       */
    __IO uint32_t ARB2;         /*!< [0x0034] IFn Arbitration 2 Register                                       */
    __IO uint32_t MCON;         /*!< [0x0038] IFn Message Control Register                                     */
    __IO uint32_t DAT_A1;       /*!< [0x003c] IFn Data A1 Register                                             */
    __IO uint32_t DAT_A2;       /*!< [0x0040] IFn Data A2 Register                                             */
    __IO uint32_t DAT_B1;       /*!< [0x0044] IFn Data B1 Register                                             */
    __IO uint32_t DAT_B2;       /*!< [0x0048] IFn Data B2 Register                                             */
    /// @cond HIDDEN_SYMBOLS
    __I uint32_t RESERVE0[13];
    /// @endcond //HIDDEN_SYMBOLS
} CAN_IF_T;


typedef struct
{


    /**
     * @var CAN_T::CON
     * Offset: 0x00  Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |Init      |Init Initialization
     * |        |          |0 = Normal Operation.
     * |        |          |1 = Initialization is started.
     * |[1]     |IE        |Module Interrupt Enable Bit
     * |        |          |0 = Function interrupt is Disabled.
     * |        |          |1 = Function interrupt is Enabled.
     * |[2]     |SIE       |Status Change Interrupt Enable Bit
     * |        |          |0 = Disabled - No Status Change Interrupt will be generated.
     * |        |          |1 = Enabled - An interrupt will be generated when a message transfer is successfully completed or a CAN bus error is detected.
     * |[3]     |EIE       |Error Interrupt Enable Bit
     * |        |          |0 = Disabled - No Error Status Interrupt will be generated.
     * |        |          |1 = Enabled - A change in the bits BOff (CAN_STATUS[7]) or EWarn (CAN_STATUS[6]) in the Status Register will generate an interrupt.
     * |[5]     |DAR       |Automatic Re-transmission Disable Bit
     * |        |          |0 = Automatic Retransmission of disturbed messages Enabled.
     * |        |          |1 = Automatic Retransmission Disabled.
     * |[6]     |CCE       |Configuration Change Enable Bit
     * |        |          |0 = No write access to the Bit Timing Register.
     * |        |          |1 = Write access to the Bit Timing Register (CAN_BTIME) allowed. (while Init bit (CAN_CON[0]) = 1).
     * |[7]     |Test      |Test Mode Enable Bit
     * |        |          |0 = Normal Operation.
     * |        |          |1 = Test Mode.
     * @var CAN_T::STATUS
     * Offset: 0x04  Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |LEC       |Last Error Code (Type of the Last Error to Occur on the CAN Bus)
     * |        |          |The LEC field holds a code, which indicates the type of the last error to occur on the CAN bus
     * |        |          |This field will be cleared to '0' when a message has been transferred (reception or transmission) without error
     * |        |          |The unused code '7' may be written by the CPU to check for updates
     * |        |          |The Error! Reference source not found
     * |        |          |describes the error code.
     * |[3]     |TxOK      |Transmitted a Message Successfully
     * |        |          |0 = Since this bit was reset by the CPU, no message has been successfully transmitted
     * |        |          |This bit is never reset by the CAN Core.
     * |        |          |1 = Since this bit was last reset by the CPU, a message has been successfully (error free and acknowledged by at least one other node) transmitted.
     * |[4]     |RxOK      |Received a Message Successfully
     * |        |          |0 = No message has been successfully received since this bit was last reset by the CPU
     * |        |          |This bit is never reset by the CAN Core.
     * |        |          |1 = A message has been successfully received since this bit was last reset by the CPU (independent of the result of acceptance filtering).
     * |[5]     |EPass     |Error Passive (Read Only)
     * |        |          |0 = The CAN Core is error active.
     * |        |          |1 = The CAN Core is in the error passive state as defined in the CAN Specification.
     * |[6]     |EWarn     |Error Warning Status (Read Only)
     * |        |          |0 = Both error counters are below the error warning limit of 96.
     * |        |          |1 = At least one of the error counters in the EML has reached the error warning limit of 96.
     * |[7]     |BOff      |Bus-off Status (Read Only)
     * |        |          |0 = The CAN module is not in bus-off state.
     * |        |          |1 = The CAN module is in bus-off state.
     * @var CAN_T::ERR
     * Offset: 0x08  Error Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |TEC       |Transmit Error Counter
     * |        |          |Actual state of the Transmit Error Counter. Values between 0 and 255.
     * |[14:8]  |REC       |Receive Error Counter
     * |        |          |Actual state of the Receive Error Counter. Values between 0 and 127.
     * |[15]    |RP        |Receive Error Passive
     * |        |          |0 = The Receive Error Counter is below the error passive level.
     * |        |          |1 = The Receive Error Counter has reached the error passive level as defined in the CAN Specification.
     * @var CAN_T::BTIME
     * Offset: 0x0C  Bit Timing Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[5:0]   |BRP       |Baud Rate Prescaler
     * |        |          |0x01-0x3F: The value by which the oscillator frequency is divided for generating the bit time quanta
     * |        |          |The bit time is built up from a multiple of this quanta
     * |        |          |Valid values for the Baud Rate Prescaler are [0...63]
     * |        |          |The actual interpretation by the hardware of this value is such that one more than the value programmed here is used.
     * |[7:6]   |SJW       |(Re)Synchronization Jump Width
     * |        |          |0x0-0x3: Valid programmed values are [0...3]
     * |        |          |The actual interpretation by the hardware of this value is such that one more than the value programmed here is used.
     * |[11:8]  |TSeg1     |Time Segment Before the Sample Point Minus Sync_Seg
     * |        |          |0x01-0x0F: valid values for TSeg1 are [1...15]
     * |        |          |The actual interpretation by the hardware of this value is such that one more than the value programmed is used.
     * |[14:12] |TSeg2     |Time Segment After Sample Point
     * |        |          |0x0-0x7: Valid values for TSeg2 are [0...7]
     * |        |          |The actual interpretation by the hardware of this value is such that one more than the value programmed here is used.
     * @var CAN_T::IIDR
     * Offset: 0x10  Interrupt Identifier Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |IntId     |Interrupt Identifier (Indicates the Source of the Interrupt)
     * |        |          |If several interrupts are pending, the CAN Interrupt Register will point to the pending interrupt with the highest priority, disregarding their chronological order
     * |        |          |An interrupt remains pending until the application software has cleared it
     * |        |          |If IntId is different from 0x0000 and IE (CAN_CON[1]) is set, the IRQ interrupt signal to the EIC is active
     * |        |          |The interrupt remains active until IntId is back to value 0x0000 (the cause of the interrupt is reset) or until IE is reset.
     * |        |          |The Status Interrupt has the highest priority
     * |        |          |Among the message interrupts, the Message Object' s interrupt priority decreases with increasing message number.
     * |        |          |A message interrupt is cleared by clearing the Message Object's IntPnd bit (CAN_IFn_MCON[13])
     * |        |          |The Status Interrupt is cleared by reading the Status Register.
     * @var CAN_T::TEST
     * Offset: 0x14  Test Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2]     |Basic     |Basic Mode
     * |        |          |0 = Basic Mode Disabled.
     * |        |          |1= IF1 Registers used as Tx Buffer, IF2 Registers used as Rx Buffer.
     * |[3]     |Silent    |Silent Mode
     * |        |          |0 = Normal operation.
     * |        |          |1 = The module is in Silent Mode.
     * |[4]     |LBack     |Loop Back Mode Enable Bit
     * |        |          |0 = Loop Back Mode is Disabled.
     * |        |          |1 = Loop Back Mode is Enabled.
     * |[6:5]   |Tx        |Tx[1:0]: Control of CAN_TX Pin
     * |        |          |00 = Reset value, CAN_TX pin is controlled by the CAN Core.
     * |        |          |01 = Sample Point can be monitored at CAN_TX pin.
     * |        |          |10 = CAN_TX pin drives a dominant ('0') value.
     * |        |          |11 = CAN_TX pin drives a recessive ('1') value.
     * |[7]     |Rx        |Monitors the Actual Value of CAN_RX Pin (Read Only) *(1)
     * |        |          |0 = The CAN bus is dominant (CAN_RX = '0').
     * |        |          |1 = The CAN bus is recessive (CAN_RX = '1').
     * @var CAN_T::BRPE
     * Offset: 0x18  Baud Rate Prescaler Extension Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |BRPE      |BRPE: Baud Rate Prescaler Extension
     * |        |          |0x00-0x0F: By programming BRPE, the Baud Rate Prescaler can be extended to values up to 1023
     * |        |          |The actual interpretation by the hardware is that one more than the value programmed by BRPE (MSBs) and BTIME (LSBs) is used.
     * @var CAN_T::TXREQ1
     * Offset: 0x100  Transmission Request Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TxRqst16_1|Transmission Request Bits 16-1 (of All Message Objects)
     * |        |          |0 = This Message Object is not waiting for transmission.
     * |        |          |1 = The transmission of this Message Object is requested and is not yet done.
     * |        |          |These bits are read only.
     * @var CAN_T::TXREQ2
     * Offset: 0x104  Transmission Request Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TxRqst32_17|Transmission Request Bits 32-17 (of All Message Objects)
     * |        |          |0 = This Message Object is not waiting for transmission.
     * |        |          |1 = The transmission of this Message Object is requested and is not yet done.
     * |        |          |These bits are read only.
     * @var CAN_T::NDAT1
     * Offset: 0x120  New Data Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |NewData16_1|New Data Bits 16-1 (of All Message Objects)
     * |        |          |0 = No new data has been written into the data portion of this Message Object by the Message Handler since the last time this flag was cleared by the application software.
     * |        |          |1 = The Message Handler or the application software has written new data into the data portion of this Message Object.
     * @var CAN_T::NDAT2
     * Offset: 0x124  New Data Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |NewData32_17|New Data Bits 32-17 (of All Message Objects)
     * |        |          |0 = No new data has been written into the data portion of this Message Object by the Message Handler since the last time this flag was cleared by the application software.
     * |        |          |1 = The Message Handler or the application software has written new data into the data portion of this Message Object.
     * @var CAN_T::IPND1
     * Offset: 0x140  Interrupt Pending Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |IntPnd16_1|Interrupt Pending Bits 16-1 (of All Message Objects)
     * |        |          |0 = This message object is not the source of an interrupt.
     * |        |          |1 = This message object is the source of an interrupt.
     * @var CAN_T::IPND2
     * Offset: 0x144  Interrupt Pending Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |IntPnd32_17|Interrupt Pending Bits 32-17 (of All Message Objects)
     * |        |          |0 = This message object is not the source of an interrupt.
     * |        |          |1 = This message object is the source of an interrupt.
     * @var CAN_T::MVLD1
     * Offset: 0x160  Message Valid Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |MsgVal16_1|Message Valid Bits 16-1 (of All Message Objects) (Read Only)
     * |        |          |0 = This Message Object is ignored by the Message Handler.
     * |        |          |1 = This Message Object is configured and should be considered by the Message Handler.
     * |        |          |Ex
     * |        |          |CAN_MVLD1[0] means Message object No.1 is valid or not
     * |        |          |If CAN_MVLD1[0] is set, message object No.1 is configured.
     * @var CAN_T::MVLD2
     * Offset: 0x164  Message Valid Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |MsgVal32_17|Message Valid Bits 32-17 (of All Message Objects) (Read Only)
     * |        |          |0 = This Message Object is ignored by the Message Handler.
     * |        |          |1 = This Message Object is configured and should be considered by the Message Handler.
     * |        |          |Ex.CAN_MVLD2[15] means Message object No.32 is valid or not
     * |        |          |If CAN_MVLD2[15] is set, message object No.32 is configured.
     * @var CAN_T::WU_EN
     * Offset: 0x168  Wake-up Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WAKUP_EN  |Wake-up Enable Bit
     * |        |          |0 = The wake-up function Disabled.
     * |        |          |1 = The wake-up function Enabled.
     * |        |          |Note: User can wake-up system when there is a falling edge in the CAN_Rx pin.
     * @var CAN_T::WU_STATUS
     * Offset: 0x16C  Wake-up Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WAKUP_STS |Wake-up Status
     * |        |          |0 = No wake-up event occurred.
     * |        |          |1 = Wake-up event occurred.
     * |        |          |Note: This bit can be cleared by writing '0'.
     */
    __IO uint32_t CON;                   /*!< [0x0000] Control Register                                                 */
    __IO uint32_t STATUS;                /*!< [0x0004] Status Register                                                  */
    __I  uint32_t ERR;                   /*!< [0x0008] Error Counter Register                                           */
    __IO uint32_t BTIME;                 /*!< [0x000c] Bit Timing Register                                              */
    __I  uint32_t IIDR;                  /*!< [0x0010] Interrupt Identifier Register                                    */
    __IO uint32_t TEST;                  /*!< [0x0014] Test Register                                                    */
    __IO uint32_t BRPE;                  /*!< [0x0018] Baud Rate Prescaler Extension Register                           */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO CAN_IF_T IF[2];
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[8];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t TXREQ1;                /*!< [0x0100] Transmission Request Register 1                                  */
    __I  uint32_t TXREQ2;                /*!< [0x0104] Transmission Request Register 2                                  */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE3[6];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t NDAT1;                 /*!< [0x0120] New Data Register 1                                              */
    __I  uint32_t NDAT2;                 /*!< [0x0124] New Data Register 2                                              */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE4[6];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t IPND1;                 /*!< [0x0140] Interrupt Pending Register 1                                     */
    __I  uint32_t IPND2;                 /*!< [0x0144] Interrupt Pending Register 2                                     */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE5[6];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t MVLD1;                 /*!< [0x0160] Message Valid Register 1                                         */
    __I  uint32_t MVLD2;                 /*!< [0x0164] Message Valid Register 2                                         */
    __IO uint32_t WU_EN;                 /*!< [0x0168] Wake-up Enable Control Register                                  */
    __IO uint32_t WU_STATUS;             /*!< [0x016c] Wake-up Status Register                                          */

} CAN_T;

/**
    @addtogroup CAN_CONST CAN Bit Field Definition
    Constant Definitions for CAN Controller
@{ */

#define CAN_CON_INIT_Pos                 (0)                                               /*!< CAN_T::CON: Init Position              */
#define CAN_CON_INIT_Msk                 (0x1ul << CAN_CON_INIT_Pos)                       /*!< CAN_T::CON: Init Mask                  */

#define CAN_CON_IE_Pos                   (1)                                               /*!< CAN_T::CON: IE Position                */
#define CAN_CON_IE_Msk                   (0x1ul << CAN_CON_IE_Pos)                         /*!< CAN_T::CON: IE Mask                    */

#define CAN_CON_SIE_Pos                  (2)                                               /*!< CAN_T::CON: SIE Position               */
#define CAN_CON_SIE_Msk                  (0x1ul << CAN_CON_SIE_Pos)                        /*!< CAN_T::CON: SIE Mask                   */

#define CAN_CON_EIE_Pos                  (3)                                               /*!< CAN_T::CON: EIE Position               */
#define CAN_CON_EIE_Msk                  (0x1ul << CAN_CON_EIE_Pos)                        /*!< CAN_T::CON: EIE Mask                   */

#define CAN_CON_DAR_Pos                  (5)                                               /*!< CAN_T::CON: DAR Position               */
#define CAN_CON_DAR_Msk                  (0x1ul << CAN_CON_DAR_Pos)                        /*!< CAN_T::CON: DAR Mask                   */

#define CAN_CON_CCE_Pos                  (6)                                               /*!< CAN_T::CON: CCE Position               */
#define CAN_CON_CCE_Msk                  (0x1ul << CAN_CON_CCE_Pos)                        /*!< CAN_T::CON: CCE Mask                   */

#define CAN_CON_TEST_Pos                 (7)                                               /*!< CAN_T::CON: Test Position              */
#define CAN_CON_TEST_Msk                 (0x1ul << CAN_CON_TEST_Pos)                       /*!< CAN_T::CON: Test Mask                  */

#define CAN_STATUS_LEC_Pos               (0)                                               /*!< CAN_T::STATUS: LEC Position            */
#define CAN_STATUS_LEC_Msk               (0x7ul << CAN_STATUS_LEC_Pos)                     /*!< CAN_T::STATUS: LEC Mask                */

#define CAN_STATUS_TXOK_Pos              (3)                                               /*!< CAN_T::STATUS: TxOK Position           */
#define CAN_STATUS_TXOK_Msk              (0x1ul << CAN_STATUS_TXOK_Pos)                    /*!< CAN_T::STATUS: TxOK Mask               */

#define CAN_STATUS_RXOK_Pos              (4)                                               /*!< CAN_T::STATUS: RxOK Position           */
#define CAN_STATUS_RXOK_Msk              (0x1ul << CAN_STATUS_RXOK_Pos)                    /*!< CAN_T::STATUS: RxOK Mask               */

#define CAN_STATUS_EPASS_Pos             (5)                                               /*!< CAN_T::STATUS: EPass Position          */
#define CAN_STATUS_EPASS_Msk             (0x1ul << CAN_STATUS_EPASS_Pos)                   /*!< CAN_T::STATUS: EPass Mask              */

#define CAN_STATUS_EWARN_Pos             (6)                                               /*!< CAN_T::STATUS: EWarn Position          */
#define CAN_STATUS_EWARN_Msk             (0x1ul << CAN_STATUS_EWARN_Pos)                   /*!< CAN_T::STATUS: EWarn Mask              */

#define CAN_STATUS_BOFF_Pos              (7)                                               /*!< CAN_T::STATUS: BOff Position           */
#define CAN_STATUS_BOFF_Msk              (0x1ul << CAN_STATUS_BOFF_Pos)                    /*!< CAN_T::STATUS: BOff Mask               */

#define CAN_ERR_TEC_Pos                  (0)                                               /*!< CAN_T::ERR: TEC Position               */
#define CAN_ERR_TEC_Msk                  (0xfful << CAN_ERR_TEC_Pos)                       /*!< CAN_T::ERR: TEC Mask                   */

#define CAN_ERR_REC_Pos                  (8)                                               /*!< CAN_T::ERR: REC Position               */
#define CAN_ERR_REC_Msk                  (0x7ful << CAN_ERR_REC_Pos)                       /*!< CAN_T::ERR: REC Mask                   */

#define CAN_ERR_RP_Pos                   (15)                                              /*!< CAN_T::ERR: RP Position                */
#define CAN_ERR_RP_Msk                   (0x1ul << CAN_ERR_RP_Pos)                         /*!< CAN_T::ERR: RP Mask                    */

#define CAN_BTIME_BRP_Pos                (0)                                               /*!< CAN_T::BTIME: BRP Position             */
#define CAN_BTIME_BRP_Msk                (0x3ful << CAN_BTIME_BRP_Pos)                     /*!< CAN_T::BTIME: BRP Mask                 */

#define CAN_BTIME_SJW_Pos                (6)                                               /*!< CAN_T::BTIME: SJW Position             */
#define CAN_BTIME_SJW_Msk                (0x3ul << CAN_BTIME_SJW_Pos)                      /*!< CAN_T::BTIME: SJW Mask                 */

#define CAN_BTIME_TSEG1_Pos              (8)                                               /*!< CAN_T::BTIME: TSeg1 Position           */
#define CAN_BTIME_TSEG1_Msk              (0xful << CAN_BTIME_TSEG1_Pos)                    /*!< CAN_T::BTIME: TSeg1 Mask               */

#define CAN_BTIME_TSEG2_Pos              (12)                                              /*!< CAN_T::BTIME: TSeg2 Position           */
#define CAN_BTIME_TSEG2_Msk              (0x7ul << CAN_BTIME_TSEG2_Pos)                    /*!< CAN_T::BTIME: TSeg2 Mask               */

#define CAN_IIDR_IntId_Pos               (0)                                               /*!< CAN_T::IIDR: IntId Position            */
#define CAN_IIDR_IntId_Msk               (0xfffful << CAN_IIDR_IntId_Pos)                  /*!< CAN_T::IIDR: IntId Mask                */

#define CAN_TEST_BASIC_Pos               (2)                                               /*!< CAN_T::TEST: Basic Position            */
#define CAN_TEST_BASIC_Msk               (0x1ul << CAN_TEST_BASIC_Pos)                     /*!< CAN_T::TEST: Basic Mask                */

#define CAN_TEST_SILENT_Pos              (3)                                               /*!< CAN_T::TEST: Silent Position           */
#define CAN_TEST_SILENT_Msk              (0x1ul << CAN_TEST_SILENT_Pos)                    /*!< CAN_T::TEST: Silent Mask               */

#define CAN_TEST_LBACK_Pos               (4)                                               /*!< CAN_T::TEST: LBack Position            */
#define CAN_TEST_LBACK_Msk               (0x1ul << CAN_TEST_LBACK_Pos)                     /*!< CAN_T::TEST: LBack Mask                */

#define CAN_TEST_Tx_Pos                  (5)                                               /*!< CAN_T::TEST: Tx Position               */
#define CAN_TEST_Tx_Msk                  (0x3ul << CAN_TEST_Tx_Pos)                        /*!< CAN_T::TEST: Tx Mask                   */

#define CAN_TEST_Rx_Pos                  (7)                                               /*!< CAN_T::TEST: Rx Position               */
#define CAN_TEST_Rx_Msk                  (0x1ul << CAN_TEST_Rx_Pos)                        /*!< CAN_T::TEST: Rx Mask                   */

#define CAN_BRPE_BRPE_Pos                (0)                                               /*!< CAN_T::BRPE: BRPE Position             */
#define CAN_BRPE_BRPE_Msk                (0xful << CAN_BRPE_BRPE_Pos)                      /*!< CAN_T::BRPE: BRPE Mask                 */

#define CAN_IF_CREQ_MSGNUM_Pos   (0)                                               /*!< CAN_IF_T::CREQ: MessageNumber Position*/
#define CAN_IF_CREQ_MSGNUM_Msk   (0x3ful << CAN_IF_CREQ_MSGNUM_Pos)        /*!< CAN_IF_T::CREQ: MessageNumber Mask    */

#define CAN_IF_CREQ_BUSY_Pos            (15)                                              /*!< CAN_IF_T::CREQ: Busy Position         */
#define CAN_IF_CREQ_BUSY_Msk            (0x1ul << CAN_IF_CREQ_BUSY_Pos)                   /*!< CAN_IF_T::CREQ: Busy Mask             */

#define CAN_IF_CMASK_DATAB_Pos          (0)                                               /*!< CAN_IF_T::CMASK: DAT_B Position       */
#define CAN_IF_CMASK_DATAB_Msk          (0x1ul << CAN_IF_CMASK_DATAB_Pos)                /*!< CAN_IF_T::CMASK: DAT_B Mask           */

#define CAN_IF_CMASK_DATAA_Pos          (1)                                               /*!< CAN_IF_T::CMASK: DAT_A Position       */
#define CAN_IF_CMASK_DATAA_Msk          (0x1ul << CAN_IF_CMASK_DATAA_Pos)                /*!< CAN_IF_T::CMASK: DAT_A Mask           */

#define CAN_IF_CMASK_TXRQSTNEWDAT_Pos  (2)                                               /*!< CAN_IF_T::CMASK: TxRqst_NewDat Position*/
#define CAN_IF_CMASK_TXRQSTNEWDAT_Msk  (0x1ul << CAN_IF_CMASK_TXRQSTNEWDAT_Pos)        /*!< CAN_IF_T::CMASK: TxRqst_NewDat Mask   */

#define CAN_IF_CMASK_CLRINTPND_Pos      (3)                                               /*!< CAN_IF_T::CMASK: ClrIntPnd Position   */
#define CAN_IF_CMASK_CLRINTPND_Msk      (0x1ul << CAN_IF_CMASK_CLRINTPND_Pos)            /*!< CAN_IF_T::CMASK: ClrIntPnd Mask       */

#define CAN_IF_CMASK_CONTROL_Pos        (4)                                               /*!< CAN_IF_T::CMASK: Control Position     */
#define CAN_IF_CMASK_CONTROL_Msk        (0x1ul << CAN_IF_CMASK_CONTROL_Pos)              /*!< CAN_IF_T::CMASK: Control Mask         */

#define CAN_IF_CMASK_ARB_Pos            (5)                                               /*!< CAN_IF_T::CMASK: Arb Position         */
#define CAN_IF_CMASK_ARB_Msk            (0x1ul << CAN_IF_CMASK_ARB_Pos)                  /*!< CAN_IF_T::CMASK: Arb Mask             */

#define CAN_IF_CMASK_MASK_Pos           (6)                                               /*!< CAN_IF_T::CMASK: Mask Position        */
#define CAN_IF_CMASK_MASK_Msk           (0x1ul << CAN_IF_CMASK_MASK_Pos)                 /*!< CAN_IF_T::CMASK: Mask Mask            */

#define CAN_IF_CMASK_WRRD_Pos          (7)                                               /*!< CAN_IF_T::CMASK: WR_RD Position       */
#define CAN_IF_CMASK_WRRD_Msk          (0x1ul << CAN_IF_CMASK_WRRD_Pos)                /*!< CAN_IF_T::CMASK: WR_RD Mask           */

#define CAN_IF_MASK1_Msk_Pos            (0)                                               /*!< CAN_IF_T::MASK1: Msk Position         */
#define CAN_IF_MASK1_Msk_Msk            (0xfffful << CAN_IF_MASK1_Msk_Pos)               /*!< CAN_IF_T::MASK1: Msk Mask             */

#define CAN_IF_MASK2_Msk_Pos            (0)                                               /*!< CAN_IF_T::MASK2: Msk Position         */
#define CAN_IF_MASK2_Msk_Msk            (0x1ffful << CAN_IF_MASK2_Msk_Pos)               /*!< CAN_IF_T::MASK2: Msk Mask             */

#define CAN_IF_MASK2_MDIR_Pos           (14)                                              /*!< CAN_IF_T::MASK2: MDir Position        */
#define CAN_IF_MASK2_MDIR_Msk           (0x1ul << CAN_IF_MASK2_MDIR_Pos)                 /*!< CAN_IF_T::MASK2: MDir Mask            */

#define CAN_IF_MASK2_MXTD_Pos           (15)                                              /*!< CAN_IF_T::MASK2: MXtd Position        */
#define CAN_IF_MASK2_MXTD_Msk           (0x1ul << CAN_IF_MASK2_MXTD_Pos)                 /*!< CAN_IF_T::MASK2: MXtd Mask            */

#define CAN_IF_ARB1_ID_Pos              (0)                                               /*!< CAN_IF_T::ARB1: ID Position           */
#define CAN_IF_ARB1_ID_Msk              (0xfffful << CAN_IF_ARB1_ID_Pos)                 /*!< CAN_IF_T::ARB1: ID Mask               */

#define CAN_IF_ARB2_ID_Pos              (0)                                               /*!< CAN_IF_T::ARB2: ID Position           */
#define CAN_IF_ARB2_ID_Msk              (0x1ffful << CAN_IF_ARB2_ID_Pos)                 /*!< CAN_IF_T::ARB2: ID Mask               */

#define CAN_IF_ARB2_DIR_Pos             (13)                                              /*!< CAN_IF_T::ARB2: Dir Position          */
#define CAN_IF_ARB2_DIR_Msk             (0x1ul << CAN_IF_ARB2_DIR_Pos)                   /*!< CAN_IF_T::ARB2: Dir Mask              */

#define CAN_IF_ARB2_XTD_Pos             (14)                                              /*!< CAN_IF_T::ARB2: Xtd Position          */
#define CAN_IF_ARB2_XTD_Msk             (0x1ul << CAN_IF_ARB2_XTD_Pos)                   /*!< CAN_IF_T::ARB2: Xtd Mask              */

#define CAN_IF_ARB2_MSGVAL_Pos          (15)                                              /*!< CAN_IF_T::ARB2: MsgVal Position       */
#define CAN_IF_ARB2_MSGVAL_Msk          (0x1ul << CAN_IF_ARB2_MSGVAL_Pos)                /*!< CAN_IF_T::ARB2: MsgVal Mask           */

#define CAN_IF_MCON_DLC_Pos             (0)                                               /*!< CAN_IF_T::MCON: DLC Position          */
#define CAN_IF_MCON_DLC_Msk             (0xful << CAN_IF_MCON_DLC_Pos)                   /*!< CAN_IF_T::MCON: DLC Mask              */

#define CAN_IF_MCON_EOB_Pos             (7)                                               /*!< CAN_IF_T::MCON: EoB Position          */
#define CAN_IF_MCON_EOB_Msk             (0x1ul << CAN_IF_MCON_EOB_Pos)                   /*!< CAN_IF_T::MCON: EoB Mask              */

#define CAN_IF_MCON_TxRqst_Pos          (8)                                               /*!< CAN_IF_T::MCON: TxRqst Position       */
#define CAN_IF_MCON_TxRqst_Msk          (0x1ul << CAN_IF_MCON_TxRqst_Pos)                /*!< CAN_IF_T::MCON: TxRqst Mask           */

#define CAN_IF_MCON_RmtEn_Pos           (9)                                               /*!< CAN_IF_T::MCON: RmtEn Position        */
#define CAN_IF_MCON_RmtEn_Msk           (0x1ul << CAN_IF_MCON_RmtEn_Pos)                 /*!< CAN_IF_T::MCON: RmtEn Mask            */

#define CAN_IF_MCON_RXIE_Pos            (10)                                              /*!< CAN_IF_T::MCON: RxIE Position         */
#define CAN_IF_MCON_RXIE_Msk            (0x1ul << CAN_IF_MCON_RXIE_Pos)                  /*!< CAN_IF_T::MCON: RxIE Mask             */

#define CAN_IF_MCON_TXIE_Pos            (11)                                              /*!< CAN_IF_T::MCON: TxIE Position         */
#define CAN_IF_MCON_TXIE_Msk            (0x1ul << CAN_IF_MCON_TXIE_Pos)                  /*!< CAN_IF_T::MCON: TxIE Mask             */

#define CAN_IF_MCON_UMASK_Pos           (12)                                              /*!< CAN_IF_T::MCON: UMask Position        */
#define CAN_IF_MCON_UMASK_Msk           (0x1ul << CAN_IF_MCON_UMASK_Pos)                 /*!< CAN_IF_T::MCON: UMask Mask            */

#define CAN_IF_MCON_IntPnd_Pos          (13)                                              /*!< CAN_IF_T::MCON: IntPnd Position       */
#define CAN_IF_MCON_IntPnd_Msk          (0x1ul << CAN_IF_MCON_IntPnd_Pos)                /*!< CAN_IF_T::MCON: IntPnd Mask           */

#define CAN_IF_MCON_MsgLst_Pos          (14)                                              /*!< CAN_IF_T::MCON: MsgLst Position       */
#define CAN_IF_MCON_MsgLst_Msk          (0x1ul << CAN_IF_MCON_MsgLst_Pos)                /*!< CAN_IF_T::MCON: MsgLst Mask           */

#define CAN_IF_MCON_NEWDAT_Pos          (15)                                              /*!< CAN_IF_T::MCON: NewDat Position       */
#define CAN_IF_MCON_NEWDAT_Msk          (0x1ul << CAN_IF_MCON_NEWDAT_Pos)                 /*!< CAN_IF_T::MCON: NewDat Mask           */

#define CAN_IF_DAT_A1_DATA0_Pos       (0)                                               /*!< CAN_IF_T::DAT_A1: Data_0_ Position    */
#define CAN_IF_DAT_A1_DATA0_Msk       (0xfful << CAN_IF_DAT_A1_DATA0_Pos)            /*!< CAN_IF_T::DAT_A1: Data_0_ Mask        */

#define CAN_IF_DAT_A1_DATA1_Pos       (8)                                               /*!< CAN_IF_T::DAT_A1: Data_1_ Position    */
#define CAN_IF_DAT_A1_DATA1_Msk       (0xfful << CAN_IF_DAT_A1_DATA1_Pos)            /*!< CAN_IF_T::DAT_A1: Data_1_ Mask        */

#define CAN_IF_DAT_A2_DATA2_Pos       (0)                                               /*!< CAN_IF_T::DAT_A2: Data_2_ Position    */
#define CAN_IF_DAT_A2_DATA2_Msk       (0xfful << CAN_IF_DAT_A2_DATA2_Pos)            /*!< CAN_IF_T::DAT_A2: Data_2_ Mask        */

#define CAN_IF_DAT_A2_DATA3_Pos       (8)                                               /*!< CAN_IF_T::DAT_A2: Data_3_ Position    */
#define CAN_IF_DAT_A2_DATA3_Msk       (0xfful << CAN_IF_DAT_A2_DATA3_Pos)            /*!< CAN_IF_T::DAT_A2: Data_3_ Mask        */

#define CAN_IF_DAT_B1_DATA4_Pos       (0)                                               /*!< CAN_IF_T::DAT_B1: Data_4_ Position    */
#define CAN_IF_DAT_B1_DATA4_Msk       (0xfful << CAN_IF_DAT_B1_DATA4_Pos)            /*!< CAN_IF_T::DAT_B1: Data_4_ Mask        */

#define CAN_IF_DAT_B1_DATA5_Pos       (8)                                               /*!< CAN_IF_T::DAT_B1: Data_5_ Position    */
#define CAN_IF_DAT_B1_DATA5_Msk       (0xfful << CAN_IF_DAT_B1_DATA5_Pos)            /*!< CAN_IF_T::DAT_B1: Data_5_ Mask        */

#define CAN_IF_DAT_B2_DATA6_Pos       (0)                                               /*!< CAN_IF_T::DAT_B2: Data_6_ Position    */
#define CAN_IF_DAT_B2_DATA6_Msk       (0xfful << CAN_IF_DAT_B2_DATA6_Pos)            /*!< CAN_IF_T::DAT_B2: Data_6_ Mask        */

#define CAN_IF_DAT_B2_DATA7_Pos       (8)                                               /*!< CAN_IF_T::DAT_B2: Data_7_ Position    */
#define CAN_IF_DAT_B2_DATA7_Msk       (0xfful << CAN_IF_DAT_B2_DATA7_Pos)            /*!< CAN_IF_T::DAT_B2: Data_7_ Mask        */

#define CAN_TXREQ1_TXRQST16_1_Pos        (0)                                               /*!< CAN_T::TXREQ1: TxRqst16_1 Position     */
#define CAN_TXREQ1_TXRQST16_1_Msk        (0xfffful << CAN_TXREQ1_TXRQST16_1_Pos)           /*!< CAN_T::TXREQ1: TxRqst16_1 Mask         */

#define CAN_TXREQ2_TXRQST32_17_Pos       (0)                                               /*!< CAN_T::TXREQ2: TxRqst32_17 Position    */
#define CAN_TXREQ2_TXRQST32_17_Msk       (0xfffful << CAN_TXREQ2_TXRQST32_17_Pos)          /*!< CAN_T::TXREQ2: TxRqst32_17 Mask        */

#define CAN_NDAT1_NewData16_1_Pos        (0)                                               /*!< CAN_T::NDAT1: NewData16_1 Position     */
#define CAN_NDAT1_NewData16_1_Msk        (0xfffful << CAN_NDAT1_NewData16_1_Pos)           /*!< CAN_T::NDAT1: NewData16_1 Mask         */

#define CAN_NDAT2_NewData32_17_Pos       (0)                                               /*!< CAN_T::NDAT2: NewData32_17 Position    */
#define CAN_NDAT2_NewData32_17_Msk       (0xfffful << CAN_NDAT2_NewData32_17_Pos)          /*!< CAN_T::NDAT2: NewData32_17 Mask        */

#define CAN_IPND1_IntPnd16_1_Pos         (0)                                               /*!< CAN_T::IPND1: IntPnd16_1 Position      */
#define CAN_IPND1_IntPnd16_1_Msk         (0xfffful << CAN_IPND1_IntPnd16_1_Pos)            /*!< CAN_T::IPND1: IntPnd16_1 Mask          */

#define CAN_IPND2_IntPnd32_17_Pos        (0)                                               /*!< CAN_T::IPND2: IntPnd32_17 Position     */
#define CAN_IPND2_IntPnd32_17_Msk        (0xfffful << CAN_IPND2_IntPnd32_17_Pos)           /*!< CAN_T::IPND2: IntPnd32_17 Mask         */

#define CAN_MVLD1_MsgVal16_1_Pos         (0)                                               /*!< CAN_T::MVLD1: MsgVal16_1 Position      */
#define CAN_MVLD1_MsgVal16_1_Msk         (0xfffful << CAN_MVLD1_MsgVal16_1_Pos)            /*!< CAN_T::MVLD1: MsgVal16_1 Mask          */

#define CAN_MVLD2_MsgVal32_17_Pos        (0)                                               /*!< CAN_T::MVLD2: MsgVal32_17 Position     */
#define CAN_MVLD2_MsgVal32_17_Msk        (0xfffful << CAN_MVLD2_MsgVal32_17_Pos)           /*!< CAN_T::MVLD2: MsgVal32_17 Mask         */

#define CAN_WU_EN_WAKUP_EN_Pos           (0)                                               /*!< CAN_T::WU_EN: WAKUP_EN Position        */
#define CAN_WU_EN_WAKUP_EN_Msk           (0x1ul << CAN_WU_EN_WAKUP_EN_Pos)                 /*!< CAN_T::WU_EN: WAKUP_EN Mask            */

#define CAN_WU_STATUS_WAKUP_STS_Pos      (0)                                               /*!< CAN_T::WU_STATUS: WAKUP_STS Position   */
#define CAN_WU_STATUS_WAKUP_STS_Msk      (0x1ul << CAN_WU_STATUS_WAKUP_STS_Pos)            /*!< CAN_T::WU_STATUS: WAKUP_STS Mask       */

/**@}*/ /* CAN_CONST */
/**@}*/ /* end of CAN register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __CAN_REG_H__ */
