/**************************************************************************//**
 * @file     hsusbd_reg.h
 * @version  V1.00
 * @brief    HSUSBD register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __HSUSBD_REG_H__
#define __HSUSBD_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup HSUSBD USB 2.0 Device Controller(HSUSBD)
    Memory Mapped Structure for HSUSBD Controller
@{ */

typedef struct
{

    /**
     * @var HSUSBD_EP_T::EPDAT
     * Offset: 0x00  Endpoint n Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |EPDAT     |Endpoint A~L Data Register
     * |        |          |Endpoint A~L data buffer for the buffer transaction (read or write).
     * |        |          |Note: Only word access is supported.
     * @var HSUSBD_EP_T::EPDAT_BYTE
     * Offset: 0x00  Endpoint n Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |EPDAT     |Endpoint A~L Data Register
     * |        |          |Endpoint A~L data buffer for the buffer transaction (read or write).
     * |        |          |Note: Only byte access is supported.
     * @var HSUSBD_EP_T::EPINTSTS
     * Offset: 0x04  Endpoint n Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUFFULLIF |Buffer Full
     * |        |          |For an IN endpoint, the currently selected buffer is full, or no buffer is available to the local side for writing (no space to write)
     * |        |          |For an OUT endpoint, there is a buffer available on the local side, and there are FIFO full of bytes available to be read (entire packet is available for reading).
     * |        |          |0 = The endpoint packet buffer is not full.
     * |        |          |1 = The endpoint packet buffer is full.
     * |        |          |Note: This bit is read-only.
     * |[1]     |BUFEMPTYIF|Buffer Empty
     * |        |          |For an IN endpoint, a buffer is available to the local side for writing up to FIFO full of bytes.
     * |        |          |0 = The endpoint buffer is not empty.
     * |        |          |1 = The endpoint buffer is empty.
     * |        |          |For an OUT endpoint:
     * |        |          |0 = The currently selected buffer has not a count of 0.
     * |        |          |1 = The currently selected buffer has a count of 0, or no buffer is available on the local side (nothing to read).
     * |        |          |Note: This bit is read-only.
     * |[2]     |SHORTTXIF |Short Packet Transferred Interrupt
     * |        |          |0 = The length of the last packet was not less than the Maximum Packet Size (EPMPS).
     * |        |          |1 = The length of the last packet was less than the Maximum Packet Size (EPMPS).
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[3]     |TXPKIF    |Data Packet Transmitted Interrupt
     * |        |          |0 = Not a data packet is transmitted from the endpoint to the host.
     * |        |          |1 = A data packet is transmitted from the endpoint to the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[4]     |RXPKIF    |Data Packet Received Interrupt
     * |        |          |0 = No data packet is received from the host by the endpoint.
     * |        |          |1 = A data packet is received from the host by the endpoint.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |OUTTKIF   |Data OUT Token Interrupt
     * |        |          |0 = A Data OUT token has not been received from the host.
     * |        |          |1 = A Data OUT token has been received from the host
     * |        |          |This bit also set by PING token (in high-speed only).
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[6]     |INTKIF    |Data IN Token Interrupt
     * |        |          |0 = Not Data IN token has been received from the host.
     * |        |          |1 = A Data IN token has been received from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[7]     |PINGIF    |PING Token Interrupt
     * |        |          |0 = A Data PING token has not been received from the host.
     * |        |          |1 = A Data PING token has been received from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[8]     |NAKIF     |USB NAK Sent
     * |        |          |0 = The last USB IN packet could be provided, and was acknowledged with an ACK.
     * |        |          |1 = The last USB IN packet could not be provided, and was acknowledged with a NAK.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[9]     |STALLIF   |USB STALL Sent
     * |        |          |0 = The last USB packet could be accepted or provided because the endpoint was stalled, and was acknowledged with a STALL.
     * |        |          |1 = The last USB packet could not be accepted or provided because the endpoint was stalled, and was acknowledged with a STALL.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[10]    |NYETIF    |NYET Sent
     * |        |          |0 = The space available in the RAM is sufficient to accommodate the next on coming data packet.
     * |        |          |1 = The space available in the RAM is not sufficient to accommodate the next on coming data packet.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[11]    |ERRIF     |ERR Sent
     * |        |          |0 = No any error in the transaction.
     * |        |          |1 = There occurs any error in the transaction.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[12]    |SHORTRXIF |Bulk Out Short Packet Received
     * |        |          |0 = No bulk out short packet is received.
     * |        |          |1 = Received bulk out short packet (including zero length packet).
     * |        |          |Note: Write 1 to clear this bit to 0.
     * @var HSUSBD_EP_T::EPINTEN
     * Offset: 0x08  Endpoint n Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUFFULLIEN|Buffer Full Interrupt
     * |        |          |When set, this bit enables a local interrupt to be set when a buffer full condition is detected on the bus.
     * |        |          |0 = Buffer full interrupt Disabled.
     * |        |          |1 = Buffer full interrupt Enabled.
     * |[1]     |BUFEMPTYIEN|Buffer Empty Interrupt
     * |        |          |When set, this bit enables a local interrupt to be set when a buffer empty condition is detected on the bus.
     * |        |          |0 = Buffer empty interrupt Disabled.
     * |        |          |1 = Buffer empty interrupt Enabled.
     * |[2]     |SHORTTXIEN|Short Packet Transferred Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a short data packet has been transferred to/from the host.
     * |        |          |0 = Short data packet interrupt Disabled.
     * |        |          |1 = Short data packet interrupt Enabled.
     * |[3]     |TXPKIEN   |Data Packet Transmitted Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a data packet has been received from the host.
     * |        |          |0 = Data packet has been received from the host interrupt Disabled.
     * |        |          |1 = Data packet has been received from the host interrupt Enabled.
     * |[4]     |RXPKIEN   |Data Packet Received Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a data packet has been transmitted to the host.
     * |        |          |0 = Data packet has been transmitted to the host interrupt Disabled.
     * |        |          |1 = Data packet has been transmitted to the host interrupt Enabled.
     * |[5]     |OUTTKIEN  |Data OUT Token Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a Data OUT token has been received from the host.
     * |        |          |0 = Data OUT token interrupt Disabled.
     * |        |          |1 = Data OUT token interrupt Enabled.
     * |[6]     |INTKIEN   |Data IN Token Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a Data IN token has been received from the host.
     * |        |          |0 = Data IN token interrupt Disabled.
     * |        |          |1 = Data IN token interrupt Enabled.
     * |[7]     |PINGIEN   |PING Token Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a PING token has been received from the host.
     * |        |          |0 = PING token interrupt Disabled.
     * |        |          |1 = PING token interrupt Enabled.
     * |[8]     |NAKIEN    |USB NAK Sent Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a NAK token is sent to the host.
     * |        |          |0 = NAK token interrupt Disabled.
     * |        |          |1 = NAK token interrupt Enabled.
     * |[9]     |STALLIEN  |USB STALL Sent Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set when a stall token is sent to the host.
     * |        |          |0 = STALL token interrupt Disabled.
     * |        |          |1 = STALL token interrupt Enabled.
     * |[10]    |NYETIEN   |NYET Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set whenever NYET condition occurs on the bus for this endpoint.
     * |        |          |0 = NYET condition interrupt Disabled.
     * |        |          |1 = NYET condition interrupt Enabled.
     * |[11]    |ERRIEN    |ERR Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set whenever ERR condition occurs on the bus for this endpoint.
     * |        |          |0 = Error event interrupt Disabled.
     * |        |          |1 = Error event interrupt Enabled.
     * |[12]    |SHORTRXIEN|Bulk Out Short Packet Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be set whenever bulk out short packet occurs on the bus for this endpoint.
     * |        |          |0 = Bulk out interrupt Disabled.
     * |        |          |1 = Bulk out interrupt Enabled.
     * @var HSUSBD_EP_T::EPDATCNT
     * Offset: 0x0C  Endpoint n Data Available Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |DATCNT    |Data Count
     * |        |          |For an IN endpoint (EPDIR(USBD_EPxCFG[3] is high.), this register returns the number of valid bytes in the IN endpoint packet buffer.
     * |        |          |For an OUT endpoint (EPDIR(USBD_EPxCFG[3] is low.), this register returns the number of received valid bytes in the Host OUT transfer.
     * |[30:16] |DMALOOP   |DMA Loop
     * |        |          |This register is the remaining DMA loop to complete. Each loop means 32-byte transfer.
     * @var HSUSBD_EP_T::EPRSPCTL
     * Offset: 0x10  Endpoint n Response Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FLUSH     |Buffer Flush
     * |        |          |Writing 1 to this bit causes the packet buffer to be flushed and the corresponding EP_AVAIL register to be cleared
     * |        |          |This bit is self-clearing
     * |        |          |This bit should always be written after an configuration event.
     * |        |          |0 = The packet buffer is not flushed.
     * |        |          |1 = The packet buffer is flushed by user.
     * |[2:1]   |MODE      |Mode Control
     * |        |          |The two bits decide the operation mode of the in-endpoint.
     * |        |          |00: Auto-Validate Mode
     * |        |          |01: Manual-Validate Mode
     * |        |          |10: Fly Mode
     * |        |          |11: Reserved
     * |        |          |These bits are not valid for an out-endpoint
     * |        |          |The auto validate mode will be activated when the reserved mode is selected
     * |[3]     |TOGGLE    |Endpoint Toggle
     * |        |          |This bit is used to clear the endpoint data toggle bit
     * |        |          |Reading this bit returns the current state of the endpoint data toggle bit.
     * |        |          |The local CPU may use this bit to initialize the end-point's toggle in case of reception of a Set Interface request or a Clear Feature (ep_halt) request from the host
     * |        |          |Only when toggle bit is "1", this bit can be written into the inversed write data bit[3].
     * |        |          |0 = Not clear the endpoint data toggle bit.
     * |        |          |1 = Clear the endpoint data toggle bit.
     * |[4]     |HALT      |Endpoint Halt
     * |        |          |This bit is used to send a STALL handshake as response to the token from the host
     * |        |          |When an Endpoint Set Feature (ep_halt) is detected by the local CPU, it must write a '1' to this bit.
     * |        |          |0 = Not send a STALL handshake as response to the token from the host.
     * |        |          |1 = Send a STALL handshake as response to the token from the host.
     * |[5]     |ZEROLEN   |Zero Length
     * |        |          |This bit is used to send a zero-length packet response to an IN-token
     * |        |          |When this bit is set, a zero packet is sent to the host on reception of an IN-token
     * |        |          |This bit gets cleared once the zero length data packet is sent.
     * |        |          |0 = A zero packet is not sent to the host on reception of an IN-token.
     * |        |          |1 = A zero packet is sent to the host on reception of an IN-token.
     * |[6]     |SHORTTXEN |Short Packet Transfer Enable
     * |        |          |This bit is applicable only in case of Auto-Validate Method
     * |        |          |This bit is set to validate any remaining data in the buffer which is not equal to the MPS of the endpoint, and happens to be the last transfer
     * |        |          |This bit gets cleared once the data packet is sent.
     * |        |          |0 = Not validate any remaining data in the buffer which is not equal to the MPS of the endpoint.
     * |        |          |1 = Validate any remaining data in the buffer which is not equal to the MPS of the endpoint.
     * |[7]     |DISBUF    |Buffer Disable Bit
     * |        |          |This bit is used to receive unknown size OUT short packet
     * |        |          |The received packet size is reference USBD_EPxDATCNT register.
     * |        |          |0 = Buffer Not Disabled when Bulk-OUT short packet is received.
     * |        |          |1 = Buffer Disabled when Bulk-OUT short packet is received.
     * @var HSUSBD_EP_T::EPMPS
     * Offset: 0x14  Endpoint n Maximum Packet Size Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:0]  |EPMPS     |Endpoint Maximum Packet Size
     * |        |          |This field determines the Maximum Packet Size of the Endpoint.
     * @var HSUSBD_EP_T::EPTXCNT
     * Offset: 0x18  Endpoint n Transfer Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:0]  |TXCNT     |Endpoint Transfer Count
     * |        |          |For IN endpoints, this field determines the total number of bytes to be sent to the host in case of manual validation method.
     * |        |          |For OUT endpoints, this field has no effect.
     * @var HSUSBD_EP_T::EPCFG
     * Offset: 0x1C  Endpoint n Configuration Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |EPEN      |Endpoint Valid
     * |        |          |When set, this bit enables this endpoint
     * |        |          |This bit has no effect on Endpoint 0, which is always enabled.
     * |        |          |0 = The endpoint Disabled.
     * |        |          |1 = The endpoint Enabled.
     * |[2:1]   |EPTYPE    |Endpoint Type
     * |        |          |This field selects the type of this endpoint. Endpoint 0 is forced to a Control type.
     * |        |          |00 = Reserved.
     * |        |          |01 = Bulk.
     * |        |          |10 = Interrupt.
     * |        |          |11 = Isochronous.
     * |[3]     |EPDIR     |Endpoint Direction
     * |        |          |0 = out-endpoint (Host OUT to Device).
     * |        |          |1 = in-endpoint (Host IN to Device).
     * |        |          |Note: A maximum of one OUT and IN endpoint is allowed for each endpoint number.
     * |[7:4]   |EPNUM     |Endpoint Number
     * |        |          |This field selects the number of the endpoint. Valid numbers 1 to 15.
     * |        |          |Note: Do not support two endpoints have same endpoint number.
     * @var HSUSBD_EP_T::EPBUFST
     * Offset: 0x20  Endpoint n RAM Start Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |SADDR     |Endpoint Start Address
     * |        |          |This is the start-address of the RAM space allocated for the endpoint A~L.
     * @var HSUSBD_EP_T::EPBUFEND
     * Offset: 0x24  Endpoint n RAM End Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |EADDR     |Endpoint End Address
     * |        |          |This is the end-address of the RAM space allocated for the endpoint A~L.
     */

    union
    {
        __IO uint32_t EPDAT;
        __IO uint8_t  EPDAT_BYTE;

    };                                  /*!< [0x0000] Endpoint n Data Register                                         */

    __IO uint32_t EPINTSTS;             /*!< [0x0004] Endpoint n Interrupt Status Register                             */
    __IO uint32_t EPINTEN;              /*!< [0x0008] Endpoint n Interrupt Enable Register                             */
    __I  uint32_t EPDATCNT;             /*!< [0x000c] Endpoint n Data Available Count Register                         */
    __IO uint32_t EPRSPCTL;             /*!< [0x0010] Endpoint n Response Control Register                             */
    __IO uint32_t EPMPS;                /*!< [0x0014] Endpoint n Maximum Packet Size Register                          */
    __IO uint32_t EPTXCNT;              /*!< [0x0018] Endpoint n Transfer Count Register                               */
    __IO uint32_t EPCFG;                /*!< [0x001c] Endpoint n Configuration Register                                */
    __IO uint32_t EPBUFST;              /*!< [0x0020] Endpoint n RAM Start Address Register                            */
    __IO uint32_t EPBUFEND;             /*!< [0x0024] Endpoint n RAM End Address Register                              */

} HSUSBD_EP_T;

typedef struct
{

    /**
     * @var HSUSBD_T::GINTSTS
     * Offset: 0x00  Global Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |USBIF     |USB Interrupt
     * |        |          |This bit conveys the interrupt status for USB specific events endpoint
     * |        |          |When set, USB interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[1]     |CEPIF     |Control Endpoint Interrupt
     * |        |          |This bit conveys the interrupt status for control endpoint
     * |        |          |When set, Control-ep's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[2]     |EPAIF     |Endpoint a Interrupt
     * |        |          |When set, the corresponding Endpoint A's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[3]     |EPBIF     |Endpoint B Interrupt
     * |        |          |When set, the corresponding Endpoint B's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[4]     |EPCIF     |Endpoint C Interrupt
     * |        |          |When set, the corresponding Endpoint C's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[5]     |EPDIF     |Endpoint D Interrupt
     * |        |          |When set, the corresponding Endpoint D's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[6]     |EPEIF     |Endpoint E Interrupt
     * |        |          |When set, the corresponding Endpoint E's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[7]     |EPFIF     |Endpoint F Interrupt
     * |        |          |When set, the corresponding Endpoint F's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[8]     |EPGIF     |Endpoint G Interrupt
     * |        |          |When set, the corresponding Endpoint G's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[9]     |EPHIF     |Endpoint H Interrupt
     * |        |          |When set, the corresponding Endpoint H's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[10]    |EPIIF     |Endpoint I Interrupt
     * |        |          |When set, the corresponding Endpoint I's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[11]    |EPJIF     |Endpoint J Interrupt
     * |        |          |When set, the corresponding Endpoint J's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[12]    |EPKIF     |Endpoint K Interrupt
     * |        |          |When set, the corresponding Endpoint K's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * |[13]    |EPLIF     |Endpoint L Interrupt
     * |        |          |When set, the corresponding Endpoint L's interrupt status register should be read to determine the cause of the interrupt.
     * |        |          |0 = No interrupt event occurred.
     * |        |          |1 = The related interrupt event is occurred.
     * @var HSUSBD_T::GINTEN
     * Offset: 0x08  Global Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |USBIEN    |USB Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be generated when a USB event occurs on the bus.
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[1]     |CEPIEN    |Control Endpoint Interrupt Enable Bit
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the control endpoint.
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[2]     |EPAIEN    |Interrupt Enable Control for Endpoint a
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint A.
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[3]     |EPBIEN    |Interrupt Enable Control for Endpoint B
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint B
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[4]     |EPCIEN    |Interrupt Enable Control for Endpoint C
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint C
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[5]     |EPDIEN    |Interrupt Enable Control for Endpoint D
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint D
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[6]     |EPEIEN    |Interrupt Enable Control for Endpoint E
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint E
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[7]     |EPFIEN    |Interrupt Enable Control for Endpoint F
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint F
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[8]     |EPGIEN    |Interrupt Enable Control for Endpoint G
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint G
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[9]     |EPHIEN    |Interrupt Enable Control for Endpoint H
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint H
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[10]    |EPIIEN    |Interrupt Enable Control for Endpoint I
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint I
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[11]    |EPJIEN    |Interrupt Enable Control for Endpoint J
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint J
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[12]    |EPKIEN    |Interrupt Enable Control for Endpoint K
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint K
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * |[13]    |EPLIEN    |Interrupt Enable Control for Endpoint L
     * |        |          |When set, this bit enables a local interrupt to be generated when an interrupt is pending for the endpoint L
     * |        |          |0 = The related interrupt Disabled.
     * |        |          |1 = The related interrupt Enabled.
     * @var HSUSBD_T::BUSINTSTS
     * Offset: 0x10  USB Bus Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SOFIF     |SOF Receive Control
     * |        |          |This bit indicates when a start-of-frame packet has been received.
     * |        |          |0 = No start-of-frame packet has been received.
     * |        |          |1 = Start-of-frame packet has been received.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[1]     |RSTIF     |Reset Status
     * |        |          |When set, this bit indicates that either the USB root port reset is end.
     * |        |          |0 = No USB root port reset is end.
     * |        |          |1 = USB root port reset is end.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[2]     |RESUMEIF  |Resume
     * |        |          |When set, this bit indicates that a device resume has occurred.
     * |        |          |0 = No device resume has occurred.
     * |        |          |1 = Device resume has occurred.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[3]     |SUSPENDIF |Suspend Request
     * |        |          |This bit is set as default and it has to be cleared by writing '1' before the USB reset
     * |        |          |This bit is also set when a USB Suspend request is detected from the host.
     * |        |          |0 = No USB Suspend request is detected from the host.
     * |        |          |1= USB Suspend request is detected from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[4]     |HISPDIF   |High-speed Settle
     * |        |          |0 = No valid high-speed reset protocol is detected.
     * |        |          |1 = Valid high-speed reset protocol is over and the device has settled in high-speed.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |DMADONEIF |DMA Completion Interrupt
     * |        |          |0 = No DMA transfer over.
     * |        |          |1 = DMA transfer is over.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[6]     |PHYCLKVLDIF|Usable Clock Interrupt
     * |        |          |0 = Usable clock is not available.
     * |        |          |1 = Usable clock is available from the transceiver.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[8]     |VBUSDETIF |VBUS Detection Interrupt Status
     * |        |          |0 = No VBUS is plug-in.
     * |        |          |1 = VBUS is plug-in.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * @var HSUSBD_T::BUSINTEN
     * Offset: 0x14  USB Bus Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SOFIEN    |SOF Interrupt
     * |        |          |This bit enables the SOF interrupt.
     * |        |          |0 = SOF interrupt Disabled.
     * |        |          |1 = SOF interrupt Enabled.
     * |[1]     |RSTIEN    |Reset Status
     * |        |          |This bit enables the USB-Reset interrupt.
     * |        |          |0 = USB-Reset interrupt Disabled.
     * |        |          |1 = USB-Reset interrupt Enabled.
     * |[2]     |RESUMEIEN |Resume
     * |        |          |This bit enables the Resume interrupt.
     * |        |          |0 = Resume interrupt Disabled.
     * |        |          |1 = Resume interrupt Enabled.
     * |[3]     |SUSPENDIEN|Suspend Request
     * |        |          |This bit enables the Suspend interrupt.
     * |        |          |0 = Suspend interrupt Disabled.
     * |        |          |1 = Suspend interrupt Enabled.
     * |[4]     |HISPDIEN  |High-speed Settle
     * |        |          |This bit enables the high-speed settle interrupt.
     * |        |          |0 = High-speed settle interrupt Disabled.
     * |        |          |1 = High-speed settle interrupt Enabled.
     * |[5]     |DMADONEIEN|DMA Completion Interrupt
     * |        |          |This bit enables the DMA completion interrupt
     * |        |          |0 = DMA completion interrupt Disabled.
     * |        |          |1 = DMA completion interrupt Enabled.
     * |[6]     |PHYCLKVLDIEN|Usable Clock Interrupt
     * |        |          |This bit enables the usable clock interrupt.
     * |        |          |0 = Usable clock interrupt Disabled.
     * |        |          |1 = Usable clock interrupt Enabled.
     * |[8]     |VBUSDETIEN|VBUS Detection Interrupt Enable Bit
     * |        |          |This bit enables the VBUS floating detection interrupt.
     * |        |          |0 = VBUS floating detection interrupt Disabled.
     * |        |          |1 = VBUS floating detection interrupt Enabled.
     * @var HSUSBD_T::OPER
     * Offset: 0x18  USB Operational Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RESUMEEN  |Generate Resume
     * |        |          |0 = No Resume sequence to be initiated to the host.
     * |        |          |1 = A Resume sequence to be initiated to the host if device remote wakeup is enabled
     * |        |          |This bit is self-clearing.
     * |[1]     |HISPDEN   |USB High-speed
     * |        |          |0 = The USB device controller to suppress the chirp-sequence during reset protocol, thereby allowing the USB device controller to settle in full-speed, even though it is connected to a USB2.0 Host.
     * |        |          |1 = The USB device controller to initiate a chirp-sequence during reset protocol.
     * |[2]     |CURSPD    |USB Current Speed
     * |        |          |0 = The device has settled in Full Speed.
     * |        |          |1 = The USB device controller has settled in High-speed.
     * @var HSUSBD_T::FRAMECNT
     * Offset: 0x1C  USB Frame Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |MFRAMECNT |Micro-frame Counter
     * |        |          |This field contains the micro-frame number for the frame number in the frame counter field.
     * |[13:3]  |FRAMECNT  |Frame Counter
     * |        |          |This field contains the frame count from the most recent start-of-frame packet.
     * @var HSUSBD_T::FADDR
     * Offset: 0x20  USB Function Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[6:0]   |FADDR     |USB Function Address
     * |        |          |This field contains the current USB address of the device
     * |        |          |This field is cleared when a root port reset is detected
     * @var HSUSBD_T::TEST
     * Offset: 0x24  USB Test Mode Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |TESTMODE  |Test Mode Selection
     * |        |          |000 = Normal Operation.
     * |        |          |001 = Test_J.
     * |        |          |010 = Test_K.
     * |        |          |011 = Test_SE0_NAK.
     * |        |          |100 = Test_Packet.
     * |        |          |101 = Test_Force_Enable.
     * |        |          |110 = Reserved.
     * |        |          |111 = Reserved.
     * |        |          |Note: This field is cleared when root port reset is detected.
     * @var HSUSBD_T::CEPDAT
     * Offset: 0x28  Control-Endpoint Data Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DAT       |Control-endpoint Data Buffer
     * |        |          |Control endpoint data buffer for the buffer transaction (read or write).
     * |        |          |Note: Only word access is supported.
     * @var HSUSBD_T::CEPDAT_BYTE
     * Offset: 0x28  Control-Endpoint Data Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |DAT       |Control-endpoint Data Buffer
     * |        |          |Control endpoint data buffer for the buffer transaction (read or write).
     * |        |          |Note: Only byte access is supported.
     * @var HSUSBD_T::CEPCTL
     * Offset: 0x2C  Control-Endpoint Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |NAKCLR    |No Acknowledge Control
     * |        |          |This bit plays a crucial role in any control transfer.
     * |        |          |0 = The bit is being cleared by the local CPU by writing zero, the USB device controller will be responding with NAKs for the subsequent status phase
     * |        |          |This mechanism holds the host from moving to the next request, until the local CPU is also ready to process the next request.
     * |        |          |1 = This bit is set to one by the USB device controller, whenever a setup token is received
     * |        |          |The local CPU can take its own time to finish off any house-keeping work based on the request and then clear this bit.
     * |        |          |Note: Only when CPU writes data[1:0] is 2'b10 or 2'b00, this bit can be updated.
     * |[1]     |STALLEN   |Stall Enable Bit
     * |        |          |When this stall bit is set, the control endpoint sends a stall handshake in response to any in or out token thereafter
     * |        |          |This is typically used for response to invalid/unsupported requests
     * |        |          |When this bit is being set the NAK clear bit has to be cleared at the same time since the NAK clear bit has highest priority than STALL
     * |        |          |It is automatically cleared on receipt of a next setup-token
     * |        |          |So, the local CPU need not write again to clear this bit.
     * |        |          |0 = No sends a stall handshake in response to any in or out token thereafter.
     * |        |          |1 = The control endpoint sends a stall handshake in response to any in or out token thereafter.
     * |        |          |Note: Only when CPU writes data[1:0] is 2'b10 or 2'b00, this bit can be updated.
     * |[2]     |ZEROLEN   |Zero Packet Length
     * |        |          |This bit is valid for Auto Validation mode only.
     * |        |          |0 = No zero length packet to the host during Data stage to an IN token.
     * |        |          |1 = USB device controller can send a zero length packet to the host during Data stage to an IN token
     * |        |          |This bit gets cleared once the zero length data packet is sent
     * |        |          |So, the local CPU need not write again to clear this bit.
     * |[3]     |FLUSH     |CEP-flush Bit
     * |        |          |0 = No the packet buffer and its corresponding USBD_CEPDATCNT register to be cleared.
     * |        |          |1 = The packet buffer and its corresponding USBD_CEPDATCNT register to be cleared
     * |        |          |This bit is self-cleaning.
     * @var HSUSBD_T::CEPINTEN
     * Offset: 0x30  Control-Endpoint Interrupt Enable
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SETUPTKIEN|Setup Token Interrupt Enable Bit
     * |        |          |0 = The SETUP token interrupt in Control Endpoint Disabled.
     * |        |          |1 = The SETUP token interrupt in Control Endpoint Enabled.
     * |[1]     |SETUPPKIEN|Setup Packet Interrupt
     * |        |          |0 = The SETUP packet interrupt in Control Endpoint Disabled.
     * |        |          |1 = The SETUP packet interrupt in Control Endpoint Enabled.
     * |[2]     |OUTTKIEN  |Out Token Interrupt
     * |        |          |0 = The OUT token interrupt in Control Endpoint Disabled.
     * |        |          |1 = The OUT token interrupt in Control Endpoint Enabled.
     * |[3]     |INTKIEN   |In Token Interrupt
     * |        |          |0 = The IN token interrupt in Control Endpoint Disabled.
     * |        |          |1 = The IN token interrupt in Control Endpoint Enabled.
     * |[4]     |PINGIEN   |Ping Token Interrupt
     * |        |          |0 = The ping token interrupt in Control Endpoint Disabled.
     * |        |          |1 = The ping token interrupt Control Endpoint Enabled.
     * |[5]     |TXPKIEN   |Data Packet Transmitted Interrupt
     * |        |          |0 = The data packet transmitted interrupt in Control Endpoint Disabled.
     * |        |          |1 = The data packet transmitted interrupt in Control Endpoint Enabled.
     * |[6]     |RXPKIEN   |Data Packet Received Interrupt
     * |        |          |0 = The data received interrupt in Control Endpoint Disabled.
     * |        |          |1 = The data received interrupt in Control Endpoint Enabled.
     * |[7]     |NAKIEN    |NAK Sent Interrupt
     * |        |          |0 = The NAK sent interrupt in Control Endpoint Disabled.
     * |        |          |1 = The NAK sent interrupt in Control Endpoint Enabled.
     * |[8]     |STALLIEN  |STALL Sent Interrupt
     * |        |          |0 = The STALL sent interrupt in Control Endpoint Disabled.
     * |        |          |1 = The STALL sent interrupt in Control Endpoint Enabled.
     * |[9]     |ERRIEN    |USB Error Interrupt
     * |        |          |0 = The USB Error interrupt in Control Endpoint Disabled.
     * |        |          |1 = The USB Error interrupt in Control Endpoint Enabled.
     * |[10]    |STSDONEIEN|Status Completion Interrupt
     * |        |          |0 = The Status Completion interrupt in Control Endpoint Disabled.
     * |        |          |1 = The Status Completion interrupt in Control Endpoint Enabled.
     * |[11]    |BUFFULLIEN|Buffer Full Interrupt
     * |        |          |0 = The buffer full interrupt in Control Endpoint Disabled.
     * |        |          |1 = The buffer full interrupt in Control Endpoint Enabled.
     * |[12]    |BUFEMPTYIEN|Buffer Empty Interrupt
     * |        |          |0 = The buffer empty interrupt in Control Endpoint Disabled.
     * |        |          |1= The buffer empty interrupt in Control Endpoint Enabled.
     * @var HSUSBD_T::CEPINTSTS
     * Offset: 0x34  Control-Endpoint Interrupt Status
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SETUPTKIF |Setup Token Interrupt
     * |        |          |0 = Not a Setup token is received.
     * |        |          |1 = A Setup token is received. Writing 1 clears this status bit
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[1]     |SETUPPKIF |Setup Packet Interrupt
     * |        |          |This bit must be cleared (by writing 1) before the next setup packet can be received
     * |        |          |If the bit is not cleared, then the successive setup packets will be overwritten in the setup packet buffer.
     * |        |          |0 = Not a Setup packet has been received from the host.
     * |        |          |1 = A Setup packet has been received from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[2]     |OUTTKIF   |Out Token Interrupt
     * |        |          |0 = The control-endpoint does not received an OUT token from the host.
     * |        |          |1 = The control-endpoint receives an OUT token from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[3]     |INTKIF    |in Token Interrupt
     * |        |          |0 = The control-endpoint does not received an IN token from the host.
     * |        |          |1 = The control-endpoint receives an IN token from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[4]     |PINGIF    |Ping Token Interrupt
     * |        |          |0 = The control-endpoint does not received a ping token from the host.
     * |        |          |1 = The control-endpoint receives a ping token from the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |TXPKIF    |Data Packet Transmitted Interrupt
     * |        |          |0 = Not a data packet is successfully transmitted to the host in response to an IN-token and an ACK-token is received for the same.
     * |        |          |1 = A data packet is successfully transmitted to the host in response to an IN-token and an ACK-token is received for the same.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[6]     |RXPKIF    |Data Packet Received Interrupt
     * |        |          |0 = Not a data packet is successfully received from the host for an OUT-token and an ACK is sent to the host.
     * |        |          |1 = A data packet is successfully received from the host for an OUT-token and an ACK is sent to the host.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[7]     |NAKIF     |NAK Sent Interrupt
     * |        |          |0 = Not a NAK-token is sent in response to an IN/OUT token.
     * |        |          |1 = A NAK-token is sent in response to an IN/OUT token.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[8]     |STALLIF   |STALL Sent Interrupt
     * |        |          |0 = Not a stall-token is sent in response to an IN/OUT token.
     * |        |          |1 = A stall-token is sent in response to an IN/OUT token.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[9]     |ERRIF     |USB Error Interrupt
     * |        |          |0 = No error had occurred during the transaction.
     * |        |          |1 = An error had occurred during the transaction.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[10]    |STSDONEIF |Status Completion Interrupt
     * |        |          |0 = Not a USB transaction has completed successfully.
     * |        |          |1 = The status stage of a USB transaction has completed successfully.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[11]    |BUFFULLIF |Buffer Full Interrupt
     * |        |          |0 = The control-endpoint buffer is not full.
     * |        |          |1 = The control-endpoint buffer is full.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[12]    |BUFEMPTYIF|Buffer Empty Interrupt
     * |        |          |0 = The control-endpoint buffer is not empty.
     * |        |          |1 = The control-endpoint buffer is empty.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * @var HSUSBD_T::CEPTXCNT
     * Offset: 0x38  Control-Endpoint In-transfer Data Count
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |TXCNT     |In-transfer Data Count
     * |        |          |There is no mode selection for the control endpoint (but it operates like manual mode).The local-CPU has to fill the control-endpoint buffer with the data to be sent for an in-token and to write the count of bytes in this register
     * |        |          |When zero is written into this field, a zero length packet is sent to the host
     * |        |          |When the count written in the register is more than the MPS, the data sent will be of only MPS.
     * @var HSUSBD_T::CEPRXCNT
     * Offset: 0x3C  Control-Endpoint Out-transfer Data Count
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |RXCNT     |Out-transfer Data Count
     * |        |          |The USB device controller maintains the count of the data received in case of an out transfer, during the control transfer.
     * @var HSUSBD_T::CEPDATCNT
     * Offset: 0x40  Control-Endpoint data count
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |DATCNT    |Control-endpoint Data Count
     * |        |          |The USB device controller maintains the count of the data of control-endpoint.
     * @var HSUSBD_T::SETUP1_0
     * Offset: 0x44  Setup1 & Setup0 bytes
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SETUP0    |Setup Byte 0[7:0]
     * |        |          |This register provides byte 0 of the last setup packet received
     * |        |          |For a Standard Device Request, the following bmRequestType information is returned.
     * |        |          |Bit 7(Direction):
     * |        |          | 0: Host to device
     * |        |          | 1: Device to host
     * |        |          |Bit 6-5 (Type):
     * |        |          | 00: Standard
     * |        |          | 01: Class
     * |        |          | 10: Vendor
     * |        |          | 11: Reserved
     * |        |          |Bit 4-0 (Recipient)
     * |        |          | 00000: Device
     * |        |          | 00001: Interface
     * |        |          | 00010: Endpoint
     * |        |          | 00011: Other
     * |        |          | Others: Reserved
     * |[15:8]  |SETUP1    |Setup Byte 1[15:8]
     * |        |          |This register provides byte 1 of the last setup packet received
     * |        |          |For a Standard Device Request, the following bRequest Code information is returned.
     * |        |          |00000000 = Get Status.
     * |        |          |00000001 = Clear Feature.
     * |        |          |00000010 = Reserved.
     * |        |          |00000011 = Set Feature.
     * |        |          |00000100 = Reserved.
     * |        |          |00000101 = Set Address.
     * |        |          |00000110 = Get Descriptor.
     * |        |          |00000111 = Set Descriptor.
     * |        |          |00001000 = Get Configuration.
     * |        |          |00001001 = Set Configuration.
     * |        |          |00001010 = Get Interface.
     * |        |          |00001011 = Set Interface.
     * |        |          |00001100 = Sync Frame.
     * @var HSUSBD_T::SETUP3_2
     * Offset: 0x48  Setup3 & Setup2 Bytes
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SETUP2    |Setup Byte 2 [7:0]
     * |        |          |This register provides byte 2 of the last setup packet received
     * |        |          |For a Standard Device Request, the least significant byte of the wValue field is returned
     * |[15:8]  |SETUP3    |Setup Byte 3 [15:8]
     * |        |          |This register provides byte 3 of the last setup packet received
     * |        |          |For a Standard Device Request, the most significant byte of the wValue field is returned.
     * @var HSUSBD_T::SETUP5_4
     * Offset: 0x4C  Setup5 & Setup4 Bytes
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SETUP4    |Setup Byte 4[7:0]
     * |        |          |This register provides byte 4 of the last setup packet received
     * |        |          |For a Standard Device Request, the least significant byte of the wIndex is returned.
     * |[15:8]  |SETUP5    |Setup Byte 5[15:8]
     * |        |          |This register provides byte 5 of the last setup packet received
     * |        |          |For a Standard Device Request, the most significant byte of the wIndex field is returned.
     * @var HSUSBD_T::SETUP7_6
     * Offset: 0x50  Setup7 & Setup6 Bytes
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SETUP6    |Setup Byte 6[7:0]
     * |        |          |This register provides byte 6 of the last setup packet received
     * |        |          |For a Standard Device Request, the least significant byte of the wLength field is returned.
     * |[15:8]  |SETUP7    |Setup Byte 7[15:8]
     * |        |          |This register provides byte 7 of the last setup packet received
     * |        |          |For a Standard Device Request, the most significant byte of the wLength field is returned.
     * @var HSUSBD_T::CEPBUFST
     * Offset: 0x54  Control Endpoint RAM Start Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |SADDR     |Control-endpoint Start Address
     * |        |          |This is the start-address of the RAM space allocated for the control-endpoint.
     * @var HSUSBD_T::CEPBUFEND
     * Offset: 0x58  Control Endpoint RAM End Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |EADDR     |Control-endpoint End Address
     * |        |          |This is the end-address of the RAM space allocated for the control-endpoint.
     * @var HSUSBD_T::DMACTL
     * Offset: 0x5C  DMA Control Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |EPNUM     |DMA Endpoint Address Bits
     * |        |          |Used to define the Endpoint Address
     * |[4]     |DMARD     |DMA Operation
     * |        |          |0 : The operation is a DMA write (read from USB buffer)
     * |        |          |DMA will check endpoint data available count (USBD_EPxDATCNT) according to EPNM setting before to perform DMA write operation.
     * |        |          |1 : The operation is a DMA read (write to USB buffer).
     * |[5]     |DMAEN     |DMA Enable Bit
     * |        |          |0 : DMA function Disabled.
     * |        |          |1 : DMA function Enabled.
     * |[6]     |SGEN      |Scatter Gather Function Enable Bit
     * |        |          |0 : Scatter gather function Disabled.
     * |        |          |1 : Scatter gather function Enabled.
     * |[7]     |DMARST    |Reset DMA State Machine
     * |        |          |0 : No reset the DMA state machine.
     * |        |          |1 : Reset the DMA state machine.
     * |[8]     |SVINEP    |Serve IN Endpoint
     * |        |          |This bit is used to specify DMA serving endpoint-IN endpoint or OUT endpoint.
     * |        |          |0: DMA serves OUT endpoint
     * |        |          |1: DMA serves IN endpoint
     * @var HSUSBD_T::DMACNT
     * Offset: 0x60  DMA Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[19:0]  |DMACNT    |DMA Transfer Count
     * |        |          |The transfer count of the DMA operation to be performed is written to this register.
     * @var HSUSBD_T::DMAADDR
     * Offset: 0x700  AHB DMA Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DMAADDR   |DMAADDR
     * |        |          |The register specifies the address from which the DMA has to read / write
     * |        |          |The address must WORD (32-bit) aligned.
     * @var HSUSBD_T::PHYCTL
     * Offset: 0x704  USB PHY Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8]     |DPPUEN    |DP Pull-up
     * |        |          |0 = Pull-up resistor on D+ Disabled.
     * |        |          |1 = Pull-up resistor on D+ Enabled.
     * |[9]     |PHYEN     |PHY Suspend Enable Bit
     * |        |          |0 = The USB PHY is suspend.
     * |        |          |1 = The USB PHY is not suspend.
     * |[24]    |WKEN      |Wake-up Enable Bit
     * |        |          |0 = The wake-up function Disabled.
     * |        |          |1 = The wake-up function Enabled.
     * |[31]    |VBUSDET   |VBUS Status
     * |        |          |0 = The VBUS is not detected yet.
     * |        |          |1 = The VBUS is detected.
     */

    __I  uint32_t GINTSTS;               /*!< [0x0000] Global Interrupt Status Register                                 */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t GINTEN;                /*!< [0x0008] Global Interrupt Enable Register                                 */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t BUSINTSTS;             /*!< [0x0010] USB Bus Interrupt Status Register                                */
    __IO uint32_t BUSINTEN;              /*!< [0x0014] USB Bus Interrupt Enable Register                                */
    __IO uint32_t OPER;                  /*!< [0x0018] USB Operational Register                                         */
    __I  uint32_t FRAMECNT;              /*!< [0x001c] USB Frame Count Register                                         */
    __IO uint32_t FADDR;                 /*!< [0x0020] USB Function Address Register                                    */
    __IO uint32_t TEST;                  /*!< [0x0024] USB Test Mode Register                                           */

    union
    {
        __IO uint32_t CEPDAT;
        __IO uint8_t  CEPDAT_BYTE;

    };                                   /*!< [0x0028] Control-Endpoint Data Buffer                                     */

    __IO uint32_t CEPCTL;                /*!< [0x002c] Control-Endpoint Control Register                                */
    __IO uint32_t CEPINTEN;              /*!< [0x0030] Control-Endpoint Interrupt Enable                                */
    __IO uint32_t CEPINTSTS;             /*!< [0x0034] Control-Endpoint Interrupt Status                                */
    __IO uint32_t CEPTXCNT;              /*!< [0x0038] Control-Endpoint In-transfer Data Count                          */
    __I  uint32_t CEPRXCNT;              /*!< [0x003c] Control-Endpoint Out-transfer Data Count                         */
    __I  uint32_t CEPDATCNT;             /*!< [0x0040] Control-Endpoint data count                                      */
    __I  uint32_t SETUP1_0;              /*!< [0x0044] Setup1 & Setup0 bytes                                            */
    __I  uint32_t SETUP3_2;              /*!< [0x0048] Setup3 & Setup2 Bytes                                            */
    __I  uint32_t SETUP5_4;              /*!< [0x004c] Setup5 & Setup4 Bytes                                            */
    __I  uint32_t SETUP7_6;              /*!< [0x0050] Setup7 & Setup6 Bytes                                            */
    __IO uint32_t CEPBUFST;              /*!< [0x0054] Control Endpoint RAM Start Address Register                      */
    __IO uint32_t CEPBUFEND;             /*!< [0x0058] Control Endpoint RAM End Address Register                        */
    __IO uint32_t DMACTL;                /*!< [0x005c] DMA Control Status Register                                      */
    __IO uint32_t DMACNT;                /*!< [0x0060] DMA Count Register                                               */

    HSUSBD_EP_T EP[12];

    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[303];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DMAADDR;               /*!< [0x0700] AHB DMA Address Register                                         */
    __IO uint32_t PHYCTL;                /*!< [0x0704] USB PHY Control Register                                         */

} HSUSBD_T;

/**
    @addtogroup HSUSBD_CONST HSUSBD Bit Field Definition
    Constant Definitions for HSUSBD Controller
@{ */

#define HSUSBD_GINTSTS_USBIF_Pos         (0)                                               /*!< HSUSBD_T::GINTSTS: USBIF Position      */
#define HSUSBD_GINTSTS_USBIF_Msk         (0x1ul << HSUSBD_GINTSTS_USBIF_Pos)               /*!< HSUSBD_T::GINTSTS: USBIF Mask          */

#define HSUSBD_GINTSTS_CEPIF_Pos         (1)                                               /*!< HSUSBD_T::GINTSTS: CEPIF Position      */
#define HSUSBD_GINTSTS_CEPIF_Msk         (0x1ul << HSUSBD_GINTSTS_CEPIF_Pos)               /*!< HSUSBD_T::GINTSTS: CEPIF Mask          */

#define HSUSBD_GINTSTS_EPAIF_Pos         (2)                                               /*!< HSUSBD_T::GINTSTS: EPAIF Position      */
#define HSUSBD_GINTSTS_EPAIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPAIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPAIF Mask          */

#define HSUSBD_GINTSTS_EPBIF_Pos         (3)                                               /*!< HSUSBD_T::GINTSTS: EPBIF Position      */
#define HSUSBD_GINTSTS_EPBIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPBIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPBIF Mask          */

#define HSUSBD_GINTSTS_EPCIF_Pos         (4)                                               /*!< HSUSBD_T::GINTSTS: EPCIF Position      */
#define HSUSBD_GINTSTS_EPCIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPCIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPCIF Mask          */

#define HSUSBD_GINTSTS_EPDIF_Pos         (5)                                               /*!< HSUSBD_T::GINTSTS: EPDIF Position      */
#define HSUSBD_GINTSTS_EPDIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPDIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPDIF Mask          */

#define HSUSBD_GINTSTS_EPEIF_Pos         (6)                                               /*!< HSUSBD_T::GINTSTS: EPEIF Position      */
#define HSUSBD_GINTSTS_EPEIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPEIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPEIF Mask          */

#define HSUSBD_GINTSTS_EPFIF_Pos         (7)                                               /*!< HSUSBD_T::GINTSTS: EPFIF Position      */
#define HSUSBD_GINTSTS_EPFIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPFIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPFIF Mask          */

#define HSUSBD_GINTSTS_EPGIF_Pos         (8)                                               /*!< HSUSBD_T::GINTSTS: EPGIF Position      */
#define HSUSBD_GINTSTS_EPGIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPGIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPGIF Mask          */

#define HSUSBD_GINTSTS_EPHIF_Pos         (9)                                               /*!< HSUSBD_T::GINTSTS: EPHIF Position      */
#define HSUSBD_GINTSTS_EPHIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPHIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPHIF Mask          */

#define HSUSBD_GINTSTS_EPIIF_Pos         (10)                                              /*!< HSUSBD_T::GINTSTS: EPIIF Position      */
#define HSUSBD_GINTSTS_EPIIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPIIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPIIF Mask          */

#define HSUSBD_GINTSTS_EPJIF_Pos         (11)                                              /*!< HSUSBD_T::GINTSTS: EPJIF Position      */
#define HSUSBD_GINTSTS_EPJIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPJIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPJIF Mask          */

#define HSUSBD_GINTSTS_EPKIF_Pos         (12)                                              /*!< HSUSBD_T::GINTSTS: EPKIF Position      */
#define HSUSBD_GINTSTS_EPKIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPKIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPKIF Mask          */

#define HSUSBD_GINTSTS_EPLIF_Pos         (13)                                              /*!< HSUSBD_T::GINTSTS: EPLIF Position      */
#define HSUSBD_GINTSTS_EPLIF_Msk         (0x1ul << HSUSBD_GINTSTS_EPLIF_Pos)               /*!< HSUSBD_T::GINTSTS: EPLIF Mask          */

#define HSUSBD_GINTEN_USBIEN_Pos         (0)                                               /*!< HSUSBD_T::GINTEN: USBIEN Position      */
#define HSUSBD_GINTEN_USBIEN_Msk         (0x1ul << HSUSBD_GINTEN_USBIEN_Pos)               /*!< HSUSBD_T::GINTEN: USBIEN Mask          */

#define HSUSBD_GINTEN_CEPIEN_Pos         (1)                                               /*!< HSUSBD_T::GINTEN: CEPIEN Position      */
#define HSUSBD_GINTEN_CEPIEN_Msk         (0x1ul << HSUSBD_GINTEN_CEPIEN_Pos)               /*!< HSUSBD_T::GINTEN: CEPIEN Mask          */

#define HSUSBD_GINTEN_EPAIEN_Pos         (2)                                               /*!< HSUSBD_T::GINTEN: EPAIEN Position      */
#define HSUSBD_GINTEN_EPAIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPAIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPAIEN Mask          */

#define HSUSBD_GINTEN_EPBIEN_Pos         (3)                                               /*!< HSUSBD_T::GINTEN: EPBIEN Position      */
#define HSUSBD_GINTEN_EPBIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPBIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPBIEN Mask          */

#define HSUSBD_GINTEN_EPCIEN_Pos         (4)                                               /*!< HSUSBD_T::GINTEN: EPCIEN Position      */
#define HSUSBD_GINTEN_EPCIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPCIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPCIEN Mask          */

#define HSUSBD_GINTEN_EPDIEN_Pos         (5)                                               /*!< HSUSBD_T::GINTEN: EPDIEN Position      */
#define HSUSBD_GINTEN_EPDIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPDIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPDIEN Mask          */

#define HSUSBD_GINTEN_EPEIEN_Pos         (6)                                               /*!< HSUSBD_T::GINTEN: EPEIEN Position      */
#define HSUSBD_GINTEN_EPEIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPEIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPEIEN Mask          */

#define HSUSBD_GINTEN_EPFIEN_Pos         (7)                                               /*!< HSUSBD_T::GINTEN: EPFIEN Position      */
#define HSUSBD_GINTEN_EPFIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPFIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPFIEN Mask          */

#define HSUSBD_GINTEN_EPGIEN_Pos         (8)                                               /*!< HSUSBD_T::GINTEN: EPGIEN Position      */
#define HSUSBD_GINTEN_EPGIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPGIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPGIEN Mask          */

#define HSUSBD_GINTEN_EPHIEN_Pos         (9)                                               /*!< HSUSBD_T::GINTEN: EPHIEN Position      */
#define HSUSBD_GINTEN_EPHIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPHIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPHIEN Mask          */

#define HSUSBD_GINTEN_EPIIEN_Pos         (10)                                              /*!< HSUSBD_T::GINTEN: EPIIEN Position      */
#define HSUSBD_GINTEN_EPIIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPIIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPIIEN Mask          */

#define HSUSBD_GINTEN_EPJIEN_Pos         (11)                                              /*!< HSUSBD_T::GINTEN: EPJIEN Position      */
#define HSUSBD_GINTEN_EPJIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPJIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPJIEN Mask          */

#define HSUSBD_GINTEN_EPKIEN_Pos         (12)                                              /*!< HSUSBD_T::GINTEN: EPKIEN Position      */
#define HSUSBD_GINTEN_EPKIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPKIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPKIEN Mask          */

#define HSUSBD_GINTEN_EPLIEN_Pos         (13)                                              /*!< HSUSBD_T::GINTEN: EPLIEN Position      */
#define HSUSBD_GINTEN_EPLIEN_Msk         (0x1ul << HSUSBD_GINTEN_EPLIEN_Pos)               /*!< HSUSBD_T::GINTEN: EPLIEN Mask          */

#define HSUSBD_BUSINTSTS_SOFIF_Pos       (0)                                               /*!< HSUSBD_T::BUSINTSTS: SOFIF Position    */
#define HSUSBD_BUSINTSTS_SOFIF_Msk       (0x1ul << HSUSBD_BUSINTSTS_SOFIF_Pos)             /*!< HSUSBD_T::BUSINTSTS: SOFIF Mask        */

#define HSUSBD_BUSINTSTS_RSTIF_Pos       (1)                                               /*!< HSUSBD_T::BUSINTSTS: RSTIF Position    */
#define HSUSBD_BUSINTSTS_RSTIF_Msk       (0x1ul << HSUSBD_BUSINTSTS_RSTIF_Pos)             /*!< HSUSBD_T::BUSINTSTS: RSTIF Mask        */

#define HSUSBD_BUSINTSTS_RESUMEIF_Pos    (2)                                               /*!< HSUSBD_T::BUSINTSTS: RESUMEIF Position */
#define HSUSBD_BUSINTSTS_RESUMEIF_Msk    (0x1ul << HSUSBD_BUSINTSTS_RESUMEIF_Pos)          /*!< HSUSBD_T::BUSINTSTS: RESUMEIF Mask     */

#define HSUSBD_BUSINTSTS_SUSPENDIF_Pos   (3)                                               /*!< HSUSBD_T::BUSINTSTS: SUSPENDIF Position*/
#define HSUSBD_BUSINTSTS_SUSPENDIF_Msk   (0x1ul << HSUSBD_BUSINTSTS_SUSPENDIF_Pos)         /*!< HSUSBD_T::BUSINTSTS: SUSPENDIF Mask    */

#define HSUSBD_BUSINTSTS_HISPDIF_Pos     (4)                                               /*!< HSUSBD_T::BUSINTSTS: HISPDIF Position  */
#define HSUSBD_BUSINTSTS_HISPDIF_Msk     (0x1ul << HSUSBD_BUSINTSTS_HISPDIF_Pos)           /*!< HSUSBD_T::BUSINTSTS: HISPDIF Mask      */

#define HSUSBD_BUSINTSTS_DMADONEIF_Pos   (5)                                               /*!< HSUSBD_T::BUSINTSTS: DMADONEIF Position*/
#define HSUSBD_BUSINTSTS_DMADONEIF_Msk   (0x1ul << HSUSBD_BUSINTSTS_DMADONEIF_Pos)         /*!< HSUSBD_T::BUSINTSTS: DMADONEIF Mask    */

#define HSUSBD_BUSINTSTS_PHYCLKVLDIF_Pos (6)                                               /*!< HSUSBD_T::BUSINTSTS: PHYCLKVLDIF Position*/
#define HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk (0x1ul << HSUSBD_BUSINTSTS_PHYCLKVLDIF_Pos)       /*!< HSUSBD_T::BUSINTSTS: PHYCLKVLDIF Mask  */

#define HSUSBD_BUSINTSTS_VBUSDETIF_Pos   (8)                                               /*!< HSUSBD_T::BUSINTSTS: VBUSDETIF Position*/
#define HSUSBD_BUSINTSTS_VBUSDETIF_Msk   (0x1ul << HSUSBD_BUSINTSTS_VBUSDETIF_Pos)         /*!< HSUSBD_T::BUSINTSTS: VBUSDETIF Mask    */

#define HSUSBD_BUSINTEN_SOFIEN_Pos       (0)                                               /*!< HSUSBD_T::BUSINTEN: SOFIEN Position    */
#define HSUSBD_BUSINTEN_SOFIEN_Msk       (0x1ul << HSUSBD_BUSINTEN_SOFIEN_Pos)             /*!< HSUSBD_T::BUSINTEN: SOFIEN Mask        */

#define HSUSBD_BUSINTEN_RSTIEN_Pos       (1)                                               /*!< HSUSBD_T::BUSINTEN: RSTIEN Position    */
#define HSUSBD_BUSINTEN_RSTIEN_Msk       (0x1ul << HSUSBD_BUSINTEN_RSTIEN_Pos)             /*!< HSUSBD_T::BUSINTEN: RSTIEN Mask        */

#define HSUSBD_BUSINTEN_RESUMEIEN_Pos    (2)                                               /*!< HSUSBD_T::BUSINTEN: RESUMEIEN Position */
#define HSUSBD_BUSINTEN_RESUMEIEN_Msk    (0x1ul << HSUSBD_BUSINTEN_RESUMEIEN_Pos)          /*!< HSUSBD_T::BUSINTEN: RESUMEIEN Mask     */

#define HSUSBD_BUSINTEN_SUSPENDIEN_Pos   (3)                                               /*!< HSUSBD_T::BUSINTEN: SUSPENDIEN Position*/
#define HSUSBD_BUSINTEN_SUSPENDIEN_Msk   (0x1ul << HSUSBD_BUSINTEN_SUSPENDIEN_Pos)         /*!< HSUSBD_T::BUSINTEN: SUSPENDIEN Mask    */

#define HSUSBD_BUSINTEN_HISPDIEN_Pos     (4)                                               /*!< HSUSBD_T::BUSINTEN: HISPDIEN Position  */
#define HSUSBD_BUSINTEN_HISPDIEN_Msk     (0x1ul << HSUSBD_BUSINTEN_HISPDIEN_Pos)           /*!< HSUSBD_T::BUSINTEN: HISPDIEN Mask      */

#define HSUSBD_BUSINTEN_DMADONEIEN_Pos   (5)                                               /*!< HSUSBD_T::BUSINTEN: DMADONEIEN Position*/
#define HSUSBD_BUSINTEN_DMADONEIEN_Msk   (0x1ul << HSUSBD_BUSINTEN_DMADONEIEN_Pos)         /*!< HSUSBD_T::BUSINTEN: DMADONEIEN Mask    */

#define HSUSBD_BUSINTEN_PHYCLKVLDIEN_Pos (6)                                               /*!< HSUSBD_T::BUSINTEN: PHYCLKVLDIEN Position*/
#define HSUSBD_BUSINTEN_PHYCLKVLDIEN_Msk (0x1ul << HSUSBD_BUSINTEN_PHYCLKVLDIEN_Pos)       /*!< HSUSBD_T::BUSINTEN: PHYCLKVLDIEN Mask  */

#define HSUSBD_BUSINTEN_VBUSDETIEN_Pos   (8)                                               /*!< HSUSBD_T::BUSINTEN: VBUSDETIEN Position*/
#define HSUSBD_BUSINTEN_VBUSDETIEN_Msk   (0x1ul << HSUSBD_BUSINTEN_VBUSDETIEN_Pos)         /*!< HSUSBD_T::BUSINTEN: VBUSDETIEN Mask    */

#define HSUSBD_OPER_RESUMEEN_Pos         (0)                                               /*!< HSUSBD_T::OPER: RESUMEEN Position      */
#define HSUSBD_OPER_RESUMEEN_Msk         (0x1ul << HSUSBD_OPER_RESUMEEN_Pos)               /*!< HSUSBD_T::OPER: RESUMEEN Mask          */

#define HSUSBD_OPER_HISPDEN_Pos          (1)                                               /*!< HSUSBD_T::OPER: HISPDEN Position       */
#define HSUSBD_OPER_HISPDEN_Msk          (0x1ul << HSUSBD_OPER_HISPDEN_Pos)                /*!< HSUSBD_T::OPER: HISPDEN Mask           */

#define HSUSBD_OPER_CURSPD_Pos           (2)                                               /*!< HSUSBD_T::OPER: CURSPD Position        */
#define HSUSBD_OPER_CURSPD_Msk           (0x1ul << HSUSBD_OPER_CURSPD_Pos)                 /*!< HSUSBD_T::OPER: CURSPD Mask            */

#define HSUSBD_FRAMECNT_MFRAMECNT_Pos    (0)                                               /*!< HSUSBD_T::FRAMECNT: MFRAMECNT Position */
#define HSUSBD_FRAMECNT_MFRAMECNT_Msk    (0x7ul << HSUSBD_FRAMECNT_MFRAMECNT_Pos)          /*!< HSUSBD_T::FRAMECNT: MFRAMECNT Mask     */

#define HSUSBD_FRAMECNT_FRAMECNT_Pos     (3)                                               /*!< HSUSBD_T::FRAMECNT: FRAMECNT Position  */
#define HSUSBD_FRAMECNT_FRAMECNT_Msk     (0x7fful << HSUSBD_FRAMECNT_FRAMECNT_Pos)         /*!< HSUSBD_T::FRAMECNT: FRAMECNT Mask      */

#define HSUSBD_FADDR_FADDR_Pos           (0)                                               /*!< HSUSBD_T::FADDR: FADDR Position        */
#define HSUSBD_FADDR_FADDR_Msk           (0x7ful << HSUSBD_FADDR_FADDR_Pos)                /*!< HSUSBD_T::FADDR: FADDR Mask            */

#define HSUSBD_TEST_TESTMODE_Pos         (0)                                               /*!< HSUSBD_T::TEST: TESTMODE Position      */
#define HSUSBD_TEST_TESTMODE_Msk         (0x7ul << HSUSBD_TEST_TESTMODE_Pos)               /*!< HSUSBD_T::TEST: TESTMODE Mask          */

#define HSUSBD_CEPDAT_DAT_Pos            (0)                                               /*!< HSUSBD_T::CEPDAT: DAT Position         */
#define HSUSBD_CEPDAT_DAT_Msk            (0xfffffffful << HSUSBD_CEPDAT_DAT_Pos)           /*!< HSUSBD_T::CEPDAT: DAT Mask             */

#define HSUSBD_CEPCTL_NAKCLR_Pos         (0)                                               /*!< HSUSBD_T::CEPCTL: NAKCLR Position      */
#define HSUSBD_CEPCTL_NAKCLR_Msk         (0x1ul << HSUSBD_CEPCTL_NAKCLR_Pos)               /*!< HSUSBD_T::CEPCTL: NAKCLR Mask          */

#define HSUSBD_CEPCTL_STALLEN_Pos        (1)                                               /*!< HSUSBD_T::CEPCTL: STALLEN Position     */
#define HSUSBD_CEPCTL_STALLEN_Msk        (0x1ul << HSUSBD_CEPCTL_STALLEN_Pos)              /*!< HSUSBD_T::CEPCTL: STALLEN Mask         */

#define HSUSBD_CEPCTL_ZEROLEN_Pos        (2)                                               /*!< HSUSBD_T::CEPCTL: ZEROLEN Position     */
#define HSUSBD_CEPCTL_ZEROLEN_Msk        (0x1ul << HSUSBD_CEPCTL_ZEROLEN_Pos)              /*!< HSUSBD_T::CEPCTL: ZEROLEN Mask         */

#define HSUSBD_CEPCTL_FLUSH_Pos          (3)                                               /*!< HSUSBD_T::CEPCTL: FLUSH Position       */
#define HSUSBD_CEPCTL_FLUSH_Msk          (0x1ul << HSUSBD_CEPCTL_FLUSH_Pos)                /*!< HSUSBD_T::CEPCTL: FLUSH Mask           */

#define HSUSBD_CEPINTEN_SETUPTKIEN_Pos   (0)                                               /*!< HSUSBD_T::CEPINTEN: SETUPTKIEN Position*/
#define HSUSBD_CEPINTEN_SETUPTKIEN_Msk   (0x1ul << HSUSBD_CEPINTEN_SETUPTKIEN_Pos)         /*!< HSUSBD_T::CEPINTEN: SETUPTKIEN Mask    */

#define HSUSBD_CEPINTEN_SETUPPKIEN_Pos   (1)                                               /*!< HSUSBD_T::CEPINTEN: SETUPPKIEN Position*/
#define HSUSBD_CEPINTEN_SETUPPKIEN_Msk   (0x1ul << HSUSBD_CEPINTEN_SETUPPKIEN_Pos)         /*!< HSUSBD_T::CEPINTEN: SETUPPKIEN Mask    */

#define HSUSBD_CEPINTEN_OUTTKIEN_Pos     (2)                                               /*!< HSUSBD_T::CEPINTEN: OUTTKIEN Position  */
#define HSUSBD_CEPINTEN_OUTTKIEN_Msk     (0x1ul << HSUSBD_CEPINTEN_OUTTKIEN_Pos)           /*!< HSUSBD_T::CEPINTEN: OUTTKIEN Mask      */

#define HSUSBD_CEPINTEN_INTKIEN_Pos      (3)                                               /*!< HSUSBD_T::CEPINTEN: INTKIEN Position   */
#define HSUSBD_CEPINTEN_INTKIEN_Msk      (0x1ul << HSUSBD_CEPINTEN_INTKIEN_Pos)            /*!< HSUSBD_T::CEPINTEN: INTKIEN Mask       */

#define HSUSBD_CEPINTEN_PINGIEN_Pos      (4)                                               /*!< HSUSBD_T::CEPINTEN: PINGIEN Position   */
#define HSUSBD_CEPINTEN_PINGIEN_Msk      (0x1ul << HSUSBD_CEPINTEN_PINGIEN_Pos)            /*!< HSUSBD_T::CEPINTEN: PINGIEN Mask       */

#define HSUSBD_CEPINTEN_TXPKIEN_Pos      (5)                                               /*!< HSUSBD_T::CEPINTEN: TXPKIEN Position   */
#define HSUSBD_CEPINTEN_TXPKIEN_Msk      (0x1ul << HSUSBD_CEPINTEN_TXPKIEN_Pos)            /*!< HSUSBD_T::CEPINTEN: TXPKIEN Mask       */

#define HSUSBD_CEPINTEN_RXPKIEN_Pos      (6)                                               /*!< HSUSBD_T::CEPINTEN: RXPKIEN Position   */
#define HSUSBD_CEPINTEN_RXPKIEN_Msk      (0x1ul << HSUSBD_CEPINTEN_RXPKIEN_Pos)            /*!< HSUSBD_T::CEPINTEN: RXPKIEN Mask       */

#define HSUSBD_CEPINTEN_NAKIEN_Pos       (7)                                               /*!< HSUSBD_T::CEPINTEN: NAKIEN Position    */
#define HSUSBD_CEPINTEN_NAKIEN_Msk       (0x1ul << HSUSBD_CEPINTEN_NAKIEN_Pos)             /*!< HSUSBD_T::CEPINTEN: NAKIEN Mask        */

#define HSUSBD_CEPINTEN_STALLIEN_Pos     (8)                                               /*!< HSUSBD_T::CEPINTEN: STALLIEN Position  */
#define HSUSBD_CEPINTEN_STALLIEN_Msk     (0x1ul << HSUSBD_CEPINTEN_STALLIEN_Pos)           /*!< HSUSBD_T::CEPINTEN: STALLIEN Mask      */

#define HSUSBD_CEPINTEN_ERRIEN_Pos       (9)                                               /*!< HSUSBD_T::CEPINTEN: ERRIEN Position    */
#define HSUSBD_CEPINTEN_ERRIEN_Msk       (0x1ul << HSUSBD_CEPINTEN_ERRIEN_Pos)             /*!< HSUSBD_T::CEPINTEN: ERRIEN Mask        */

#define HSUSBD_CEPINTEN_STSDONEIEN_Pos   (10)                                              /*!< HSUSBD_T::CEPINTEN: STSDONEIEN Position*/
#define HSUSBD_CEPINTEN_STSDONEIEN_Msk   (0x1ul << HSUSBD_CEPINTEN_STSDONEIEN_Pos)         /*!< HSUSBD_T::CEPINTEN: STSDONEIEN Mask    */

#define HSUSBD_CEPINTEN_BUFFULLIEN_Pos   (11)                                              /*!< HSUSBD_T::CEPINTEN: BUFFULLIEN Position*/
#define HSUSBD_CEPINTEN_BUFFULLIEN_Msk   (0x1ul << HSUSBD_CEPINTEN_BUFFULLIEN_Pos)         /*!< HSUSBD_T::CEPINTEN: BUFFULLIEN Mask    */

#define HSUSBD_CEPINTEN_BUFEMPTYIEN_Pos  (12)                                              /*!< HSUSBD_T::CEPINTEN: BUFEMPTYIEN Position*/
#define HSUSBD_CEPINTEN_BUFEMPTYIEN_Msk  (0x1ul << HSUSBD_CEPINTEN_BUFEMPTYIEN_Pos)        /*!< HSUSBD_T::CEPINTEN: BUFEMPTYIEN Mask   */

#define HSUSBD_CEPINTSTS_SETUPTKIF_Pos   (0)                                               /*!< HSUSBD_T::CEPINTSTS: SETUPTKIF Position*/
#define HSUSBD_CEPINTSTS_SETUPTKIF_Msk   (0x1ul << HSUSBD_CEPINTSTS_SETUPTKIF_Pos)         /*!< HSUSBD_T::CEPINTSTS: SETUPTKIF Mask    */

#define HSUSBD_CEPINTSTS_SETUPPKIF_Pos   (1)                                               /*!< HSUSBD_T::CEPINTSTS: SETUPPKIF Position*/
#define HSUSBD_CEPINTSTS_SETUPPKIF_Msk   (0x1ul << HSUSBD_CEPINTSTS_SETUPPKIF_Pos)         /*!< HSUSBD_T::CEPINTSTS: SETUPPKIF Mask    */

#define HSUSBD_CEPINTSTS_OUTTKIF_Pos     (2)                                               /*!< HSUSBD_T::CEPINTSTS: OUTTKIF Position  */
#define HSUSBD_CEPINTSTS_OUTTKIF_Msk     (0x1ul << HSUSBD_CEPINTSTS_OUTTKIF_Pos)           /*!< HSUSBD_T::CEPINTSTS: OUTTKIF Mask      */

#define HSUSBD_CEPINTSTS_INTKIF_Pos      (3)                                               /*!< HSUSBD_T::CEPINTSTS: INTKIF Position   */
#define HSUSBD_CEPINTSTS_INTKIF_Msk      (0x1ul << HSUSBD_CEPINTSTS_INTKIF_Pos)            /*!< HSUSBD_T::CEPINTSTS: INTKIF Mask       */

#define HSUSBD_CEPINTSTS_PINGIF_Pos      (4)                                               /*!< HSUSBD_T::CEPINTSTS: PINGIF Position   */
#define HSUSBD_CEPINTSTS_PINGIF_Msk      (0x1ul << HSUSBD_CEPINTSTS_PINGIF_Pos)            /*!< HSUSBD_T::CEPINTSTS: PINGIF Mask       */

#define HSUSBD_CEPINTSTS_TXPKIF_Pos      (5)                                               /*!< HSUSBD_T::CEPINTSTS: TXPKIF Position   */
#define HSUSBD_CEPINTSTS_TXPKIF_Msk      (0x1ul << HSUSBD_CEPINTSTS_TXPKIF_Pos)            /*!< HSUSBD_T::CEPINTSTS: TXPKIF Mask       */

#define HSUSBD_CEPINTSTS_RXPKIF_Pos      (6)                                               /*!< HSUSBD_T::CEPINTSTS: RXPKIF Position   */
#define HSUSBD_CEPINTSTS_RXPKIF_Msk      (0x1ul << HSUSBD_CEPINTSTS_RXPKIF_Pos)            /*!< HSUSBD_T::CEPINTSTS: RXPKIF Mask       */

#define HSUSBD_CEPINTSTS_NAKIF_Pos       (7)                                               /*!< HSUSBD_T::CEPINTSTS: NAKIF Position    */
#define HSUSBD_CEPINTSTS_NAKIF_Msk       (0x1ul << HSUSBD_CEPINTSTS_NAKIF_Pos)             /*!< HSUSBD_T::CEPINTSTS: NAKIF Mask        */

#define HSUSBD_CEPINTSTS_STALLIF_Pos     (8)                                               /*!< HSUSBD_T::CEPINTSTS: STALLIF Position  */
#define HSUSBD_CEPINTSTS_STALLIF_Msk     (0x1ul << HSUSBD_CEPINTSTS_STALLIF_Pos)           /*!< HSUSBD_T::CEPINTSTS: STALLIF Mask      */

#define HSUSBD_CEPINTSTS_ERRIF_Pos       (9)                                               /*!< HSUSBD_T::CEPINTSTS: ERRIF Position    */
#define HSUSBD_CEPINTSTS_ERRIF_Msk       (0x1ul << HSUSBD_CEPINTSTS_ERRIF_Pos)             /*!< HSUSBD_T::CEPINTSTS: ERRIF Mask        */

#define HSUSBD_CEPINTSTS_STSDONEIF_Pos   (10)                                              /*!< HSUSBD_T::CEPINTSTS: STSDONEIF Position*/
#define HSUSBD_CEPINTSTS_STSDONEIF_Msk   (0x1ul << HSUSBD_CEPINTSTS_STSDONEIF_Pos)         /*!< HSUSBD_T::CEPINTSTS: STSDONEIF Mask    */

#define HSUSBD_CEPINTSTS_BUFFULLIF_Pos   (11)                                              /*!< HSUSBD_T::CEPINTSTS: BUFFULLIF Position*/
#define HSUSBD_CEPINTSTS_BUFFULLIF_Msk   (0x1ul << HSUSBD_CEPINTSTS_BUFFULLIF_Pos)         /*!< HSUSBD_T::CEPINTSTS: BUFFULLIF Mask    */

#define HSUSBD_CEPINTSTS_BUFEMPTYIF_Pos  (12)                                              /*!< HSUSBD_T::CEPINTSTS: BUFEMPTYIF Position*/
#define HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk  (0x1ul << HSUSBD_CEPINTSTS_BUFEMPTYIF_Pos)        /*!< HSUSBD_T::CEPINTSTS: BUFEMPTYIF Mask   */

#define HSUSBD_CEPTXCNT_TXCNT_Pos        (0)                                               /*!< HSUSBD_T::CEPTXCNT: TXCNT Position     */
#define HSUSBD_CEPTXCNT_TXCNT_Msk        (0xfful << HSUSBD_CEPTXCNT_TXCNT_Pos)             /*!< HSUSBD_T::CEPTXCNT: TXCNT Mask         */

#define HSUSBD_CEPRXCNT_RXCNT_Pos        (0)                                               /*!< HSUSBD_T::CEPRXCNT: RXCNT Position     */
#define HSUSBD_CEPRXCNT_RXCNT_Msk        (0xfful << HSUSBD_CEPRXCNT_RXCNT_Pos)             /*!< HSUSBD_T::CEPRXCNT: RXCNT Mask         */

#define HSUSBD_CEPDATCNT_DATCNT_Pos      (0)                                               /*!< HSUSBD_T::CEPDATCNT: DATCNT Position   */
#define HSUSBD_CEPDATCNT_DATCNT_Msk      (0xfffful << HSUSBD_CEPDATCNT_DATCNT_Pos)         /*!< HSUSBD_T::CEPDATCNT: DATCNT Mask       */

#define HSUSBD_SETUP1_0_SETUP0_Pos       (0)                                               /*!< HSUSBD_T::SETUP1_0: SETUP0 Position    */
#define HSUSBD_SETUP1_0_SETUP0_Msk       (0xfful << HSUSBD_SETUP1_0_SETUP0_Pos)            /*!< HSUSBD_T::SETUP1_0: SETUP0 Mask        */

#define HSUSBD_SETUP1_0_SETUP1_Pos       (8)                                               /*!< HSUSBD_T::SETUP1_0: SETUP1 Position    */
#define HSUSBD_SETUP1_0_SETUP1_Msk       (0xfful << HSUSBD_SETUP1_0_SETUP1_Pos)            /*!< HSUSBD_T::SETUP1_0: SETUP1 Mask        */

#define HSUSBD_SETUP3_2_SETUP2_Pos       (0)                                               /*!< HSUSBD_T::SETUP3_2: SETUP2 Position    */
#define HSUSBD_SETUP3_2_SETUP2_Msk       (0xfful << HSUSBD_SETUP3_2_SETUP2_Pos)            /*!< HSUSBD_T::SETUP3_2: SETUP2 Mask        */

#define HSUSBD_SETUP3_2_SETUP3_Pos       (8)                                               /*!< HSUSBD_T::SETUP3_2: SETUP3 Position    */
#define HSUSBD_SETUP3_2_SETUP3_Msk       (0xfful << HSUSBD_SETUP3_2_SETUP3_Pos)            /*!< HSUSBD_T::SETUP3_2: SETUP3 Mask        */

#define HSUSBD_SETUP5_4_SETUP4_Pos       (0)                                               /*!< HSUSBD_T::SETUP5_4: SETUP4 Position    */
#define HSUSBD_SETUP5_4_SETUP4_Msk       (0xfful << HSUSBD_SETUP5_4_SETUP4_Pos)            /*!< HSUSBD_T::SETUP5_4: SETUP4 Mask        */

#define HSUSBD_SETUP5_4_SETUP5_Pos       (8)                                               /*!< HSUSBD_T::SETUP5_4: SETUP5 Position    */
#define HSUSBD_SETUP5_4_SETUP5_Msk       (0xfful << HSUSBD_SETUP5_4_SETUP5_Pos)            /*!< HSUSBD_T::SETUP5_4: SETUP5 Mask        */

#define HSUSBD_SETUP7_6_SETUP6_Pos       (0)                                               /*!< HSUSBD_T::SETUP7_6: SETUP6 Position    */
#define HSUSBD_SETUP7_6_SETUP6_Msk       (0xfful << HSUSBD_SETUP7_6_SETUP6_Pos)            /*!< HSUSBD_T::SETUP7_6: SETUP6 Mask        */

#define HSUSBD_SETUP7_6_SETUP7_Pos       (8)                                               /*!< HSUSBD_T::SETUP7_6: SETUP7 Position    */
#define HSUSBD_SETUP7_6_SETUP7_Msk       (0xfful << HSUSBD_SETUP7_6_SETUP7_Pos)            /*!< HSUSBD_T::SETUP7_6: SETUP7 Mask        */

#define HSUSBD_CEPBUFST_SADDR_Pos        (0)                                               /*!< HSUSBD_T::CEPBUFST: SADDR Position     */
#define HSUSBD_CEPBUFST_SADDR_Msk        (0xffful << HSUSBD_CEPBUFST_SADDR_Pos)            /*!< HSUSBD_T::CEPBUFST: SADDR Mask         */

#define HSUSBD_CEPBUFEND_EADDR_Pos       (0)                                               /*!< HSUSBD_T::CEPBUFEND: EADDR Position    */
#define HSUSBD_CEPBUFEND_EADDR_Msk       (0xffful << HSUSBD_CEPBUFEND_EADDR_Pos)           /*!< HSUSBD_T::CEPBUFEND: EADDR Mask        */

#define HSUSBD_DMACTL_EPNUM_Pos          (0)                                               /*!< HSUSBD_T::DMACTL: EPNUM Position       */
#define HSUSBD_DMACTL_EPNUM_Msk          (0xful << HSUSBD_DMACTL_EPNUM_Pos)                /*!< HSUSBD_T::DMACTL: EPNUM Mask           */

#define HSUSBD_DMACTL_DMARD_Pos          (4)                                               /*!< HSUSBD_T::DMACTL: DMARD Position       */
#define HSUSBD_DMACTL_DMARD_Msk          (0x1ul << HSUSBD_DMACTL_DMARD_Pos)                /*!< HSUSBD_T::DMACTL: DMARD Mask           */

#define HSUSBD_DMACTL_DMAEN_Pos          (5)                                               /*!< HSUSBD_T::DMACTL: DMAEN Position       */
#define HSUSBD_DMACTL_DMAEN_Msk          (0x1ul << HSUSBD_DMACTL_DMAEN_Pos)                /*!< HSUSBD_T::DMACTL: DMAEN Mask           */

#define HSUSBD_DMACTL_SGEN_Pos           (6)                                               /*!< HSUSBD_T::DMACTL: SGEN Position        */
#define HSUSBD_DMACTL_SGEN_Msk           (0x1ul << HSUSBD_DMACTL_SGEN_Pos)                 /*!< HSUSBD_T::DMACTL: SGEN Mask            */

#define HSUSBD_DMACTL_DMARST_Pos         (7)                                               /*!< HSUSBD_T::DMACTL: DMARST Position      */
#define HSUSBD_DMACTL_DMARST_Msk         (0x1ul << HSUSBD_DMACTL_DMARST_Pos)               /*!< HSUSBD_T::DMACTL: DMARST Mask          */

#define HSUSBD_DMACTL_SVINEP_Pos         (8)                                               /*!< HSUSBD_T::DMACTL: SVINEP Position      */
#define HSUSBD_DMACTL_SVINEP_Msk         (0x1ul << HSUSBD_DMACTL_SVINEP_Pos)               /*!< HSUSBD_T::DMACTL: SVINEP Mask          */

#define HSUSBD_DMACNT_DMACNT_Pos         (0)                                               /*!< HSUSBD_T::DMACNT: DMACNT Position      */
#define HSUSBD_DMACNT_DMACNT_Msk         (0xffffful << HSUSBD_DMACNT_DMACNT_Pos)           /*!< HSUSBD_T::DMACNT: DMACNT Mask          */

#define HSUSBD_EPDAT_EPDAT_Pos           (0)                                               /*!< HSUSBD_T::EPDAT: EPDAT Position        */
#define HSUSBD_EPDAT_EPDAT_Msk           (0xfffffffful << HSUSBD_EPDAT_EPDAT_Pos)          /*!< HSUSBD_T::EPDAT: EPDAT Mask            */

#define HSUSBD_EPINTSTS_BUFFULLIF_Pos    (0)                                               /*!< HSUSBD_T::EPINTSTS: BUFFULLIF Position */
#define HSUSBD_EPINTSTS_BUFFULLIF_Msk    (0x1ul << HSUSBD_EPINTSTS_BUFFULLIF_Pos)          /*!< HSUSBD_T::EPINTSTS: BUFFULLIF Mask     */

#define HSUSBD_EPINTSTS_BUFEMPTYIF_Pos   (1)                                               /*!< HSUSBD_T::EPINTSTS: BUFEMPTYIF Position*/
#define HSUSBD_EPINTSTS_BUFEMPTYIF_Msk   (0x1ul << HSUSBD_EPINTSTS_BUFEMPTYIF_Pos)         /*!< HSUSBD_T::EPINTSTS: BUFEMPTYIF Mask    */

#define HSUSBD_EPINTSTS_SHORTTXIF_Pos    (2)                                               /*!< HSUSBD_T::EPINTSTS: SHORTTXIF Position */
#define HSUSBD_EPINTSTS_SHORTTXIF_Msk    (0x1ul << HSUSBD_EPINTSTS_SHORTTXIF_Pos)          /*!< HSUSBD_T::EPINTSTS: SHORTTXIF Mask     */

#define HSUSBD_EPINTSTS_TXPKIF_Pos       (3)                                               /*!< HSUSBD_T::EPINTSTS: TXPKIF Position    */
#define HSUSBD_EPINTSTS_TXPKIF_Msk       (0x1ul << HSUSBD_EPINTSTS_TXPKIF_Pos)             /*!< HSUSBD_T::EPINTSTS: TXPKIF Mask        */

#define HSUSBD_EPINTSTS_RXPKIF_Pos       (4)                                               /*!< HSUSBD_T::EPINTSTS: RXPKIF Position    */
#define HSUSBD_EPINTSTS_RXPKIF_Msk       (0x1ul << HSUSBD_EPINTSTS_RXPKIF_Pos)             /*!< HSUSBD_T::EPINTSTS: RXPKIF Mask        */

#define HSUSBD_EPINTSTS_OUTTKIF_Pos      (5)                                               /*!< HSUSBD_T::EPINTSTS: OUTTKIF Position   */
#define HSUSBD_EPINTSTS_OUTTKIF_Msk      (0x1ul << HSUSBD_EPINTSTS_OUTTKIF_Pos)            /*!< HSUSBD_T::EPINTSTS: OUTTKIF Mask       */

#define HSUSBD_EPINTSTS_INTKIF_Pos       (6)                                               /*!< HSUSBD_T::EPINTSTS: INTKIF Position    */
#define HSUSBD_EPINTSTS_INTKIF_Msk       (0x1ul << HSUSBD_EPINTSTS_INTKIF_Pos)             /*!< HSUSBD_T::EPINTSTS: INTKIF Mask        */

#define HSUSBD_EPINTSTS_PINGIF_Pos       (7)                                               /*!< HSUSBD_T::EPINTSTS: PINGIF Position    */
#define HSUSBD_EPINTSTS_PINGIF_Msk       (0x1ul << HSUSBD_EPINTSTS_PINGIF_Pos)             /*!< HSUSBD_T::EPINTSTS: PINGIF Mask        */

#define HSUSBD_EPINTSTS_NAKIF_Pos        (8)                                               /*!< HSUSBD_T::EPINTSTS: NAKIF Position     */
#define HSUSBD_EPINTSTS_NAKIF_Msk        (0x1ul << HSUSBD_EPINTSTS_NAKIF_Pos)              /*!< HSUSBD_T::EPINTSTS: NAKIF Mask         */

#define HSUSBD_EPINTSTS_STALLIF_Pos      (9)                                               /*!< HSUSBD_T::EPINTSTS: STALLIF Position   */
#define HSUSBD_EPINTSTS_STALLIF_Msk      (0x1ul << HSUSBD_EPINTSTS_STALLIF_Pos)            /*!< HSUSBD_T::EPINTSTS: STALLIF Mask       */

#define HSUSBD_EPINTSTS_NYETIF_Pos       (10)                                              /*!< HSUSBD_T::EPINTSTS: NYETIF Position    */
#define HSUSBD_EPINTSTS_NYETIF_Msk       (0x1ul << HSUSBD_EPINTSTS_NYETIF_Pos)             /*!< HSUSBD_T::EPINTSTS: NYETIF Mask        */

#define HSUSBD_EPINTSTS_ERRIF_Pos        (11)                                              /*!< HSUSBD_T::EPINTSTS: ERRIF Position     */
#define HSUSBD_EPINTSTS_ERRIF_Msk        (0x1ul << HSUSBD_EPINTSTS_ERRIF_Pos)              /*!< HSUSBD_T::EPINTSTS: ERRIF Mask         */

#define HSUSBD_EPINTSTS_SHORTRXIF_Pos    (12)                                              /*!< HSUSBD_T::EPINTSTS: SHORTRXIF Position */
#define HSUSBD_EPINTSTS_SHORTRXIF_Msk    (0x1ul << HSUSBD_EPINTSTS_SHORTRXIF_Pos)          /*!< HSUSBD_T::EPINTSTS: SHORTRXIF Mask     */

#define HSUSBD_EPINTEN_BUFFULLIEN_Pos    (0)                                               /*!< HSUSBD_T::EPINTEN: BUFFULLIEN Position */
#define HSUSBD_EPINTEN_BUFFULLIEN_Msk    (0x1ul << HSUSBD_EPINTEN_BUFFULLIEN_Pos)          /*!< HSUSBD_T::EPINTEN: BUFFULLIEN Mask     */

#define HSUSBD_EPINTEN_BUFEMPTYIEN_Pos   (1)                                               /*!< HSUSBD_T::EPINTEN: BUFEMPTYIEN Position*/
#define HSUSBD_EPINTEN_BUFEMPTYIEN_Msk   (0x1ul << HSUSBD_EPINTEN_BUFEMPTYIEN_Pos)         /*!< HSUSBD_T::EPINTEN: BUFEMPTYIEN Mask    */

#define HSUSBD_EPINTEN_SHORTTXIEN_Pos    (2)                                               /*!< HSUSBD_T::EPINTEN: SHORTTXIEN Position */
#define HSUSBD_EPINTEN_SHORTTXIEN_Msk    (0x1ul << HSUSBD_EPINTEN_SHORTTXIEN_Pos)          /*!< HSUSBD_T::EPINTEN: SHORTTXIEN Mask     */

#define HSUSBD_EPINTEN_TXPKIEN_Pos       (3)                                               /*!< HSUSBD_T::EPINTEN: TXPKIEN Position    */
#define HSUSBD_EPINTEN_TXPKIEN_Msk       (0x1ul << HSUSBD_EPINTEN_TXPKIEN_Pos)             /*!< HSUSBD_T::EPINTEN: TXPKIEN Mask        */

#define HSUSBD_EPINTEN_RXPKIEN_Pos       (4)                                               /*!< HSUSBD_T::EPINTEN: RXPKIEN Position    */
#define HSUSBD_EPINTEN_RXPKIEN_Msk       (0x1ul << HSUSBD_EPINTEN_RXPKIEN_Pos)             /*!< HSUSBD_T::EPINTEN: RXPKIEN Mask        */

#define HSUSBD_EPINTEN_OUTTKIEN_Pos      (5)                                               /*!< HSUSBD_T::EPINTEN: OUTTKIEN Position   */
#define HSUSBD_EPINTEN_OUTTKIEN_Msk      (0x1ul << HSUSBD_EPINTEN_OUTTKIEN_Pos)            /*!< HSUSBD_T::EPINTEN: OUTTKIEN Mask       */

#define HSUSBD_EPINTEN_INTKIEN_Pos       (6)                                               /*!< HSUSBD_T::EPINTEN: INTKIEN Position    */
#define HSUSBD_EPINTEN_INTKIEN_Msk       (0x1ul << HSUSBD_EPINTEN_INTKIEN_Pos)             /*!< HSUSBD_T::EPINTEN: INTKIEN Mask        */

#define HSUSBD_EPINTEN_PINGIEN_Pos       (7)                                               /*!< HSUSBD_T::EPINTEN: PINGIEN Position    */
#define HSUSBD_EPINTEN_PINGIEN_Msk       (0x1ul << HSUSBD_EPINTEN_PINGIEN_Pos)             /*!< HSUSBD_T::EPINTEN: PINGIEN Mask        */

#define HSUSBD_EPINTEN_NAKIEN_Pos        (8)                                               /*!< HSUSBD_T::EPINTEN: NAKIEN Position     */
#define HSUSBD_EPINTEN_NAKIEN_Msk        (0x1ul << HSUSBD_EPINTEN_NAKIEN_Pos)              /*!< HSUSBD_T::EPINTEN: NAKIEN Mask         */

#define HSUSBD_EPINTEN_STALLIEN_Pos      (9)                                               /*!< HSUSBD_T::EPINTEN: STALLIEN Position   */
#define HSUSBD_EPINTEN_STALLIEN_Msk      (0x1ul << HSUSBD_EPINTEN_STALLIEN_Pos)            /*!< HSUSBD_T::EPINTEN: STALLIEN Mask       */

#define HSUSBD_EPINTEN_NYETIEN_Pos       (10)                                              /*!< HSUSBD_T::EPINTEN: NYETIEN Position    */
#define HSUSBD_EPINTEN_NYETIEN_Msk       (0x1ul << HSUSBD_EPINTEN_NYETIEN_Pos)             /*!< HSUSBD_T::EPINTEN: NYETIEN Mask        */

#define HSUSBD_EPINTEN_ERRIEN_Pos        (11)                                              /*!< HSUSBD_T::EPINTEN: ERRIEN Position     */
#define HSUSBD_EPINTEN_ERRIEN_Msk        (0x1ul << HSUSBD_EPINTEN_ERRIEN_Pos)              /*!< HSUSBD_T::EPINTEN: ERRIEN Mask         */

#define HSUSBD_EPINTEN_SHORTRXIEN_Pos    (12)                                              /*!< HSUSBD_T::EPINTEN: SHORTRXIEN Position */
#define HSUSBD_EPINTEN_SHORTRXIEN_Msk    (0x1ul << HSUSBD_EPINTEN_SHORTRXIEN_Pos)          /*!< HSUSBD_T::EPINTEN: SHORTRXIEN Mask     */

#define HSUSBD_EPDATCNT_DATCNT_Pos       (0)                                               /*!< HSUSBD_T::EPDATCNT: DATCNT Position    */
#define HSUSBD_EPDATCNT_DATCNT_Msk       (0xfffful << HSUSBD_EPDATCNT_DATCNT_Pos)          /*!< HSUSBD_T::EPDATCNT: DATCNT Mask        */

#define HSUSBD_EPDATCNT_DMALOOP_Pos      (16)                                              /*!< HSUSBD_T::EPDATCNT: DMALOOP Position   */
#define HSUSBD_EPDATCNT_DMALOOP_Msk      (0x7ffful << HSUSBD_EPDATCNT_DMALOOP_Pos)         /*!< HSUSBD_T::EPDATCNT: DMALOOP Mask       */

#define HSUSBD_EPRSPCTL_FLUSH_Pos        (0)                                               /*!< HSUSBD_T::EPRSPCTL: FLUSH Position     */
#define HSUSBD_EPRSPCTL_FLUSH_Msk        (0x1ul << HSUSBD_EPRSPCTL_FLUSH_Pos)              /*!< HSUSBD_T::EPRSPCTL: FLUSH Mask         */

#define HSUSBD_EPRSPCTL_MODE_Pos         (1)                                               /*!< HSUSBD_T::EPRSPCTL: MODE Position      */
#define HSUSBD_EPRSPCTL_MODE_Msk         (0x3ul << HSUSBD_EPRSPCTL_MODE_Pos)               /*!< HSUSBD_T::EPRSPCTL: MODE Mask          */

#define HSUSBD_EPRSPCTL_TOGGLE_Pos       (3)                                               /*!< HSUSBD_T::EPRSPCTL: TOGGLE Position    */
#define HSUSBD_EPRSPCTL_TOGGLE_Msk       (0x1ul << HSUSBD_EPRSPCTL_TOGGLE_Pos)             /*!< HSUSBD_T::EPRSPCTL: TOGGLE Mask        */

#define HSUSBD_EPRSPCTL_HALT_Pos         (4)                                               /*!< HSUSBD_T::EPRSPCTL: HALT Position      */
#define HSUSBD_EPRSPCTL_HALT_Msk         (0x1ul << HSUSBD_EPRSPCTL_HALT_Pos)               /*!< HSUSBD_T::EPRSPCTL: HALT Mask          */

#define HSUSBD_EPRSPCTL_ZEROLEN_Pos      (5)                                               /*!< HSUSBD_T::EPRSPCTL: ZEROLEN Position   */
#define HSUSBD_EPRSPCTL_ZEROLEN_Msk      (0x1ul << HSUSBD_EPRSPCTL_ZEROLEN_Pos)            /*!< HSUSBD_T::EPRSPCTL: ZEROLEN Mask       */

#define HSUSBD_EPRSPCTL_SHORTTXEN_Pos    (6)                                               /*!< HSUSBD_T::EPRSPCTL: SHORTTXEN Position */
#define HSUSBD_EPRSPCTL_SHORTTXEN_Msk    (0x1ul << HSUSBD_EPRSPCTL_SHORTTXEN_Pos)          /*!< HSUSBD_T::EPRSPCTL: SHORTTXEN Mask     */

#define HSUSBD_EPRSPCTL_DISBUF_Pos       (7)                                               /*!< HSUSBD_T::EPRSPCTL: DISBUF Position    */
#define HSUSBD_EPRSPCTL_DISBUF_Msk       (0x1ul << HSUSBD_EPRSPCTL_DISBUF_Pos)             /*!< HSUSBD_T::EPRSPCTL: DISBUF Mask        */

#define HSUSBD_EPMPS_EPMPS_Pos           (0)                                               /*!< HSUSBD_T::EPMPS: EPMPS Position        */
#define HSUSBD_EPMPS_EPMPS_Msk           (0x7fful << HSUSBD_EPMPS_EPMPS_Pos)               /*!< HSUSBD_T::EPMPS: EPMPS Mask            */

#define HSUSBD_EPTXCNT_TXCNT_Pos         (0)                                               /*!< HSUSBD_T::EPTXCNT: TXCNT Position      */
#define HSUSBD_EPTXCNT_TXCNT_Msk         (0x7fful << HSUSBD_EPTXCNT_TXCNT_Pos)             /*!< HSUSBD_T::EPTXCNT: TXCNT Mask          */

#define HSUSBD_EPCFG_EPEN_Pos            (0)                                               /*!< HSUSBD_T::EPCFG: EPEN Position         */
#define HSUSBD_EPCFG_EPEN_Msk            (0x1ul << HSUSBD_EPCFG_EPEN_Pos)                  /*!< HSUSBD_T::EPCFG: EPEN Mask             */

#define HSUSBD_EPCFG_EPTYPE_Pos          (1)                                               /*!< HSUSBD_T::EPCFG: EPTYPE Position       */
#define HSUSBD_EPCFG_EPTYPE_Msk          (0x3ul << HSUSBD_EPCFG_EPTYPE_Pos)                /*!< HSUSBD_T::EPCFG: EPTYPE Mask           */

#define HSUSBD_EPCFG_EPDIR_Pos           (3)                                               /*!< HSUSBD_T::EPCFG: EPDIR Position        */
#define HSUSBD_EPCFG_EPDIR_Msk           (0x1ul << HSUSBD_EPCFG_EPDIR_Pos)                 /*!< HSUSBD_T::EPCFG: EPDIR Mask            */

#define HSUSBD_EPCFG_EPNUM_Pos           (4)                                               /*!< HSUSBD_T::EPCFG: EPNUM Position        */
#define HSUSBD_EPCFG_EPNUM_Msk           (0xful << HSUSBD_EPCFG_EPNUM_Pos)                 /*!< HSUSBD_T::EPCFG: EPNUM Mask            */

#define HSUSBD_EPBUFST_SADDR_Pos         (0)                                               /*!< HSUSBD_T::EPBUFST: SADDR Position      */
#define HSUSBD_EPBUFST_SADDR_Msk         (0xffful << HSUSBD_EPBUFST_SADDR_Pos)             /*!< HSUSBD_T::EPBUFST: SADDR Mask          */

#define HSUSBD_EPBUFEND_EADDR_Pos        (0)                                               /*!< HSUSBD_T::EPBUFEND: EADDR Position     */
#define HSUSBD_EPBUFEND_EADDR_Msk        (0xffful << HSUSBD_EPBUFEND_EADDR_Pos)            /*!< HSUSBD_T::EPBUFEND: EADDR Mask         */

#define HSUSBD_DMAADDR_DMAADDR_Pos       (0)                                               /*!< HSUSBD_T::DMAADDR: DMAADDR Position    */
#define HSUSBD_DMAADDR_DMAADDR_Msk       (0xfffffffful << HSUSBD_DMAADDR_DMAADDR_Pos)      /*!< HSUSBD_T::DMAADDR: DMAADDR Mask        */

#define HSUSBD_PHYCTL_DPPUEN_Pos         (8)                                               /*!< HSUSBD_T::PHYCTL: DPPUEN Position      */
#define HSUSBD_PHYCTL_DPPUEN_Msk         (0x1ul << HSUSBD_PHYCTL_DPPUEN_Pos)               /*!< HSUSBD_T::PHYCTL: DPPUEN Mask          */

#define HSUSBD_PHYCTL_PHYEN_Pos          (9)                                               /*!< HSUSBD_T::PHYCTL: PHYEN Position       */
#define HSUSBD_PHYCTL_PHYEN_Msk          (0x1ul << HSUSBD_PHYCTL_PHYEN_Pos)                /*!< HSUSBD_T::PHYCTL: PHYEN Mask           */

#define HSUSBD_PHYCTL_WKEN_Pos           (24)                                              /*!< HSUSBD_T::PHYCTL: WKEN Position        */
#define HSUSBD_PHYCTL_WKEN_Msk           (0x1ul << HSUSBD_PHYCTL_WKEN_Pos)                 /*!< HSUSBD_T::PHYCTL: WKEN Mask            */

#define HSUSBD_PHYCTL_VBUSDET_Pos        (31)                                              /*!< HSUSBD_T::PHYCTL: VBUSDET Position     */
#define HSUSBD_PHYCTL_VBUSDET_Msk        (0x1ul << HSUSBD_PHYCTL_VBUSDET_Pos)              /*!< HSUSBD_T::PHYCTL: VBUSDET Mask         */

/**@}*/ /* HSUSBD_CONST */
/**@}*/ /* end of HSUSBD register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __HSUSBD_REG_H__ */
