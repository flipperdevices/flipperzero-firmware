/**************************************************************************//**
 * @file     sdh_reg.h
 * @version  V1.00
 * @brief    SDH register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SDH_REG_H__
#define __SDH_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup SDH SD Card Host Interface(SDH)
    Memory Mapped Structure for SDH Controller
@{ */

typedef struct
{

    /**
     * @var SDH_T::FB
     * Offset: 0x00~0x7C  Shared Buffer (FIFO)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |BUFFER    |Shared Buffer
     * |        |          |Buffer for DMA transfer
     * @var SDH_T::DMACTL
     * Offset: 0x400  DMA Control and Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |DMAEN     |DMA Engine Enable Bit
     * |        |          |0 = DMA Disabled.
     * |        |          |1 = DMA Enabled.
     * |        |          |If this bit is cleared, DMA will ignore all requests from SD host and force bus master into IDLE state.
     * |        |          |Note: If target abort is occurred, DMAEN will be cleared.
     * |[1]     |DMARST    |Software Engine Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset internal state machine and pointers
     * |        |          |The contents of control register will not be cleared
     * |        |          |This bit will auto be cleared after few clock cycles.
     * |        |          |Note: The software reset DMA related registers.
     * |[3]     |SGEN      |Scatter-gather Function Enable Bit
     * |        |          |0 = Scatter-gather function Disabled (DMA will treat the starting address in DMASAR as starting pointer of a single block memory).
     * |        |          |1 = Scatter-gather function Enabled (DMA will treat the starting address in DMASAR as a starting address of Physical Address Descriptor (PAD) table
     * |        |          |The format of these Pads' will be described later).
     * |[9]     |DMABUSY   |DMA Transfer Is in Progress
     * |        |          |This bit indicates if SD Host is granted and doing DMA transfer or not.
     * |        |          |0 = DMA transfer is not in progress.
     * |        |          |1 = DMA transfer is in progress.
     * @var SDH_T::DMASA
     * Offset: 0x408  DMA Transfer Starting Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ORDER     |Determined to the PAD Table Fetching Is in Order or Out of Order
     * |        |          |0 = PAD table is fetched in order.
     * |        |          |1 = PAD table is fetched out of order.
     * |        |          |Note: the bit0 is valid in scatter-gather mode when SGEN = 1.
     * |[31:1]  |DMASA     |DMA Transfer Starting Address
     * |        |          |This field pads 0 as least significant bit indicates a 32-bit starting address of system memory (SRAM) for DMA to retrieve or fill in data.
     * |        |          |If DMA is not in normal mode, this field will be interpreted as a starting address of Physical Address Descriptor (PAD) table.
     * |        |          |Note: Starting address of the SRAM must be word aligned, for example, 0x0000_0000, 0x0000_0004.
     * @var SDH_T::DMABCNT
     * Offset: 0x40C  DMA Transfer Byte Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[25:0]  |BCNT      |DMA Transfer Byte Count (Read Only)
     * |        |          |This field indicates the remained byte count of DMA transfer
     * |        |          |The value of this field is valid only when DMA is busy; otherwise, it is 0.
     * @var SDH_T::DMAINTEN
     * Offset: 0x410  DMA Interrupt Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ABORTIEN  |DMA Read/Write Target Abort Interrupt Enable Bit
     * |        |          |0 = Target abort interrupt generation Disabled during DMA transfer.
     * |        |          |1 = Target abort interrupt generation Enabled during DMA transfer.
     * |[1]     |WEOTIEN   |Wrong EOT Encountered Interrupt Enable Bit
     * |        |          |0 = Interrupt generation Disabled when wrong EOT is encountered.
     * |        |          |1 = Interrupt generation Enabled when wrong EOT is encountered.
     * @var SDH_T::DMAINTSTS
     * Offset: 0x414  DMA Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ABORTIF   |DMA Read/Write Target Abort Interrupt Flag
     * |        |          |0 = No bus ERROR response received.
     * |        |          |1 = Bus ERROR response received.
     * |        |          |Note1: This bit is read only, but can be cleared by writing '1' to it.
     * |        |          |Note2: When DMA's bus master received ERROR response, it means that target abort is happened
     * |        |          |DMA will stop transfer and respond this event and then go to IDLE state
     * |        |          |When target abort occurred or WEOTIF is set, software must reset DMA and SD host, and then transfer those data again.
     * |[1]     |WEOTIF    |Wrong EOT Encountered Interrupt Flag
     * |        |          |When DMA Scatter-Gather function is enabled, and EOT of the descriptor is encountered before DMA transfer finished (that means the total sector count of all PAD is less than the sector count of SD host), this bit will be set.
     * |        |          |0 = No EOT encountered before DMA transfer finished.
     * |        |          |1 = EOT encountered before DMA transfer finished.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * @var SDH_T::GCTL
     * Offset: 0x800  Global Control and Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GCTLRST   |Software Engine Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset SD host
     * |        |          |The contents of control register will not be cleared
     * |        |          |This bit will auto cleared after reset complete.
     * |[1]     |SDEN      |Secure Digital Functionality Enable Bit
     * |        |          |0 = SD functionality disabled.
     * |        |          |1 = SD functionality enabled.
     * @var SDH_T::GINTEN
     * Offset: 0x804  Global Interrupt Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |DTAIEN    |DMA READ/WRITE Target Abort Interrupt Enable Bit
     * |        |          |0 = DMA READ/WRITE target abort interrupt generation disabled.
     * |        |          |1 = DMA READ/WRITE target abort interrupt generation enabled.
     * @var SDH_T::GINTSTS
     * Offset: 0x808  Global Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |DTAIF     |DMA READ/WRITE Target Abort Interrupt Flag (Read Only)
     * |        |          |This bit indicates DMA received an ERROR response from internal AHB bus during DMA read/write operation
     * |        |          |When Target Abort is occurred, please reset all engine.
     * |        |          |0 = No bus ERROR response received.
     * |        |          |1 = Bus ERROR response received.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * @var SDH_T::CTL
     * Offset: 0x820  SD Control and Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |COEN      |Command Output Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will output a command to SD card.
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[1]     |RIEN      |Response Input Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will wait to receive a response from SD card.
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[2]     |DIEN      |Data Input Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will wait to receive block data and the CRC16 value from SD card.
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[3]     |DOEN      |Data Output Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will transfer block data and the CRC16 value to SD card.
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[4]     |R2EN      |Response R2 Input Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will wait to receive a response R2 from SD card and store the response data into DMC's flash buffer (exclude CRC7).
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[5]     |CLK74OEN  |Initial 74 Clock Cycles Output Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will output 74 clock cycles to SD card.
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[6]     |CLK8OEN   |Generating 8 Clock Cycles Output Enable Bit
     * |        |          |0 = No effect. (Please use DMARST (SDH_CTL [0]) to clear this bit.)
     * |        |          |1 = Enabled, SD host will output 8 clock cycles.
     * |        |          |Note: When operation is finished, this bit will be cleared automatically, so don't write 0 to this bit (the controller will be abnormal).
     * |[7]     |CLKKEEP   |SD Clock Enable Control
     * |        |          |0 = SD host decided when to output clock and when to disable clock output automatically.
     * |        |          |1 = SD clock always keeps free running.
     * |[13:8]  |CMDCODE   |SD Command Code
     * |        |          |This register contains the SD command code (0x00 - 0x3F).
     * |[14]    |CTLRST    |Software Engine Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Reset the internal state machine and counters
     * |        |          |The contents of control register will not be cleared (but RIEN, DIEN, DOEN and R2_EN will be cleared)
     * |        |          |This bit will be auto cleared after few clock cycles.
     * |[15]    |DBW       |SD Data Bus Width (for 1-bit / 4-bit Selection)
     * |        |          |0 = Data bus width is 1-bit.
     * |        |          |1 = Data bus width is 4-bit.
     * |[23:16] |BLKCNT    |Block Counts to Be Transferred or Received
     * |        |          |This field contains the block counts for data-in and data-out transfer
     * |        |          |For READ_MULTIPLE_BLOCK and WRITE_MULTIPLE_BLOCK command, software can use this function to accelerate data transfer and improve performance
     * |        |          |Don't fill 0x0 to this field.
     * |        |          |Note: For READ_MULTIPLE_BLOCK and WRITE_MULTIPLE_BLOCK command, the actual total length is BLKCNT * (BLKLEN +1).
     * |[27:24] |SDNWR     |NWR Parameter for Block Write Operation
     * |        |          |This value indicates the NWR parameter for data block write operation in SD clock counts
     * |        |          |The actual clock cycle will be SDNWR+1.
     * @var SDH_T::CMDARG
     * Offset: 0x824  SD Command Argument Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ARGUMENT  |SD Command Argument
     * |        |          |This register contains a 32-bit value specifies the argument of SD command from host controller to SD card
     * |        |          |Before trigger COEN (SDH_CTL [0]), software should fill argument in this field.
     * @var SDH_T::INTEN
     * Offset: 0x828  SD Interrupt Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BLKDIEN   |Block Transfer Done Interrupt Enable Bit
     * |        |          |0 = BLKDIF (SDH_INTEN[0]) trigger interrupt Disable.
     * |        |          |1 = BLKDIF (SDH_INTEN[0]) trigger interrupt Enabled.
     * |[1]     |CRCIEN    |CRC7, CRC16 and CRC Status Error Interrupt Enable Bit
     * |        |          |0 = CRCIF (SDH_INTEN[1]) trigger interrupt Disable.
     * |        |          |1 = CRCIF (SDH_INTEN[1]) trigger interrupt Enabled.
     * |[8]     |CDIEN     |SD Card Detection Interrupt Enable Bit
     * |        |          |Enable/Disable interrupts generation of SD controller when card is inserted or removed.
     * |        |          |0 = CDIF (SDH_INTEN[8]) trigger interrupt Disable.
     * |        |          |1 = CDIF (SDH_INTEN[8]) trigger interrupt Enabled.
     * |[12]    |RTOIEN    |Response Time-out Interrupt Enable Bit
     * |        |          |Enable/Disable interrupts generation of SD controller when receiving response or R2 time-out
     * |        |          |Time-out value is specified at TOUT register.
     * |        |          |0 = RTOIF (SDH_INTEN[12]) trigger interrupt Disabled.
     * |        |          |1 = RTOIF (SDH_INTEN[12]) trigger interrupt Enabled.
     * |[13]    |DITOIEN   |Data Input Time-out Interrupt Enable Bit
     * |        |          |Enable/Disable interrupts generation of SD controller when data input time-out
     * |        |          |Time-out value is specified at TOUT register.
     * |        |          |0 = DITOIF (SDH_INTEN[13]) trigger interrupt Disabled.
     * |        |          |1 = DITOIF (SDH_INTEN[13]) trigger interrupt Enabled.
     * |[14]    |WKIEN     |Wake-up Signal Generating Enable Bit
     * |        |          |Enable/Disable wake-up signal generating of SD host when current using SD card issues an interrupt (wake-up) via DAT [1] to host.
     * |        |          |0 = SD Card interrupt to wake-up chip Disabled.
     * |        |          |1 = SD Card interrupt to wake-up chip Enabled.
     * |[30]    |CDSRC     |SD Card Detect Source Selection
     * |        |          |0 = From SD card's DAT3 pin.
     * |        |          |Host need clock to got data on pin DAT3
     * |        |          |Please make sure CLKKEEP (SDH_CTL[7]) is 1 in order to generate free running clock for DAT3 pin.
     * |        |          |1 = From GPIO pin.
     * @var SDH_T::INTSTS
     * Offset: 0x82C  SD Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BLKDIF    |Block Transfer Done Interrupt Flag (Read Only)
     * |        |          |This bit indicates that SD host has finished all data-in or data-out block transfer
     * |        |          |If there is a CRC16 error or incorrect CRC status during multiple block data transfer, the transfer will be broken and this bit will also be set.
     * |        |          |0 = Not finished yet.
     * |        |          |1 = Done.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * |[1]     |CRCIF     |CRC7, CRC16 and CRC Status Error Interrupt Flag (Read Only)
     * |        |          |This bit indicates that SD host has occurred CRC error during response in, data-in or data-out (CRC status error) transfer
     * |        |          |When CRC error is occurred, software should reset SD engine
     * |        |          |Some response (ex
     * |        |          |R3) doesn't have CRC7 information with it; SD host will still calculate CRC7, get CRC error and set this flag
     * |        |          |In this condition, software should ignore CRC error and clears this bit manually.
     * |        |          |0 = No CRC error is occurred.
     * |        |          |1 = CRC error is occurred.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * |[2]     |CRC7      |CRC7 Check Status (Read Only)
     * |        |          |SD host will check CRC7 correctness during each response in
     * |        |          |If that response does not contain CRC7 information (ex
     * |        |          |R3), then software should turn off CRCIEN (SDH_INTEN[1]) and ignore this bit.
     * |        |          |0 = Fault.
     * |        |          |1 = OK.
     * |[3]     |CRC16     |CRC16 Check Status of Data-in Transfer (Read Only)
     * |        |          |SD host will check CRC16 correctness after data-in transfer.
     * |        |          |0 = Fault.
     * |        |          |1 = OK.
     * |[6:4]   |CRCSTS    |CRC Status Value of Data-out Transfer (Read Only)
     * |        |          |SD host will record CRC status of data-out transfer
     * |        |          |Software could use this value to identify what type of error is during data-out transfer.
     * |        |          |010 = Positive CRC status.
     * |        |          |101 = Negative CRC status.
     * |        |          |111 = SD card programming error occurs.
     * |[7]     |DAT0STS   |DAT0 Pin Status of Current Selected SD Port (Read Only)
     * |        |          |This bit is the DAT0 pin status of current selected SD port.
     * |[8]     |CDIF      |SD Card Detection Interrupt Flag (Read Only)
     * |        |          |This bit indicates that SD card is inserted or removed
     * |        |          |Only when CDIEN (SDH_INTEN[8]) is set to 1, this bit is active.
     * |        |          |0 = No card is inserted or removed.
     * |        |          |1 = There is a card inserted in or removed from SD.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * |[12]    |RTOIF     |Response Time-out Interrupt Flag (Read Only)
     * |        |          |This bit indicates that SD host counts to time-out value when receiving response or R2 (waiting start bit).
     * |        |          |0 = Not time-out.
     * |        |          |1 = Response time-out.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * |[13]    |DITOIF    |Data Input Time-out Interrupt Flag (Read Only)
     * |        |          |This bit indicates that SD host counts to time-out value when receiving data (waiting start bit).
     * |        |          |0 = Not time-out.
     * |        |          |1 = Data input time-out.
     * |        |          |Note: This bit is read only, but can be cleared by writing '1' to it.
     * |[16]    |CDSTS     |Card Detect Status of SD (Read Only)
     * |        |          |This bit indicates the card detect pin status of SD, and is used for card detection
     * |        |          |When there is a card inserted in or removed from SD, software should check this bit to confirm if there is really a card insertion or removal.
     * |        |          |If CDSRC (SDH_INTEN[30]) = 0, to select DAT3 for card detection:.
     * |        |          |0 = Card removed.
     * |        |          |1 = Card inserted.
     * |        |          |If CDSRC (SDH_INTEN[30]) = 1, to select GPIO for card detection:.
     * |        |          |0 = Card inserted.
     * |        |          |1 = Card removed.
     * |[18]    |DAT1STS   |DAT1 Pin Status of SD Port (Read Only)
     * |        |          |This bit indicates the DAT1 pin status of SD port.
     * @var SDH_T::RESP0
     * Offset: 0x830  SD Receiving Response Token Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |RESPTK0   |SD Receiving Response Token 0
     * |        |          |SD host controller will receive a response token for getting a reply from SD card when RIEN (SDH_CTL[1]) is set
     * |        |          |This field contains response bit 47-16 of the response token.
     * @var SDH_T::RESP1
     * Offset: 0x834  SD Receiving Response Token Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |RESPTK1   |SD Receiving Response Token 1
     * |        |          |SD host controller will receive a response token for getting a reply from SD card when RIEN (SDH_CTL[1]) is set
     * |        |          |This register contains the bit 15-8 of the response token.
     * @var SDH_T::BLEN
     * Offset: 0x838  SD Block Length Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[10:0]  |BLKLEN    |SD BLOCK LENGTH in Byte Unit
     * |        |          |An 11-bit value specifies the SD transfer byte count of a block
     * |        |          |The actual byte count is equal to BLKLEN+1.
     * |        |          |Note: The default SD block length is 512 bytes
     * @var SDH_T::TOUT
     * Offset: 0x83C  SD Response/Data-in Time-out Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |TOUT      |SD Response/Data-in Time-out Value
     * |        |          |A 24-bit value specifies the time-out counts of response and data input
     * |        |          |SD host controller will wait start bit of response or data-in until this value reached
     * |        |          |The time period depends on SD engine clock frequency
     * |        |          |Do not write a small number into this field, or you may never get response or data due to time-out.
     * |        |          |Note: Filling 0x0 into this field will disable hardware time-out function.
     */

    __IO uint32_t FB[32];                /*!< Shared Buffer (FIFO)                                                      */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[224];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DMACTL;                /*!< [0x0400] DMA Control and Status Register                                  */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DMASA;                 /*!< [0x0408] DMA Transfer Starting Address Register                           */
    __I  uint32_t DMABCNT;               /*!< [0x040c] DMA Transfer Byte Count Register                                 */
    __IO uint32_t DMAINTEN;              /*!< [0x0410] DMA Interrupt Enable Control Register                            */
    __IO uint32_t DMAINTSTS;             /*!< [0x0414] DMA Interrupt Status Register                                    */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[250];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t GCTL;                  /*!< [0x0800] Global Control and Status Register                               */
    __IO uint32_t GINTEN;                /*!< [0x0804] Global Interrupt Control Register                                */
    __I  uint32_t GINTSTS;               /*!< [0x0808] Global Interrupt Status Register                                 */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE3[5];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CTL;                   /*!< [0x0820] SD Control and Status Register                                   */
    __IO uint32_t CMDARG;                /*!< [0x0824] SD Command Argument Register                                     */
    __IO uint32_t INTEN;                 /*!< [0x0828] SD Interrupt Control Register                                    */
    __IO uint32_t INTSTS;                /*!< [0x082c] SD Interrupt Status Register                                     */
    __I  uint32_t RESP0;                 /*!< [0x0830] SD Receiving Response Token Register 0                           */
    __I  uint32_t RESP1;                 /*!< [0x0834] SD Receiving Response Token Register 1                           */
    __IO uint32_t BLEN;                  /*!< [0x0838] SD Block Length Register                                         */
    __IO uint32_t TOUT;                  /*!< [0x083c] SD Response/Data-in Time-out Register                            */

} SDH_T;


/**
    @addtogroup SDH_CONST SDH Bit Field Definition
    Constant Definitions for SDH Controller
@{ */

#define SDH_DMACTL_DMAEN_Pos             (0)                                               /*!< SDH_T::DMACTL: DMAEN Position          */
#define SDH_DMACTL_DMAEN_Msk             (0x1ul << SDH_DMACTL_DMAEN_Pos)                   /*!< SDH_T::DMACTL: DMAEN Mask              */

#define SDH_DMACTL_DMARST_Pos            (1)                                               /*!< SDH_T::DMACTL: DMARST Position         */
#define SDH_DMACTL_DMARST_Msk            (0x1ul << SDH_DMACTL_DMARST_Pos)                  /*!< SDH_T::DMACTL: DMARST Mask             */

#define SDH_DMACTL_SGEN_Pos              (3)                                               /*!< SDH_T::DMACTL: SGEN Position           */
#define SDH_DMACTL_SGEN_Msk              (0x1ul << SDH_DMACTL_SGEN_Pos)                    /*!< SDH_T::DMACTL: SGEN Mask               */

#define SDH_DMACTL_DMABUSY_Pos           (9)                                               /*!< SDH_T::DMACTL: DMABUSY Position        */
#define SDH_DMACTL_DMABUSY_Msk           (0x1ul << SDH_DMACTL_DMABUSY_Pos)                 /*!< SDH_T::DMACTL: DMABUSY Mask            */

#define SDH_DMASA_ORDER_Pos              (0)                                               /*!< SDH_T::DMASA: ORDER Position           */
#define SDH_DMASA_ORDER_Msk              (0x1ul << SDH_DMASA_ORDER_Pos)                    /*!< SDH_T::DMASA: ORDER Mask               */

#define SDH_DMASA_DMASA_Pos              (1)                                               /*!< SDH_T::DMASA: DMASA Position           */
#define SDH_DMASA_DMASA_Msk              (0x7ffffffful << SDH_DMASA_DMASA_Pos)             /*!< SDH_T::DMASA: DMASA Mask               */

#define SDH_DMABCNT_BCNT_Pos             (0)                                               /*!< SDH_T::DMABCNT: BCNT Position          */
#define SDH_DMABCNT_BCNT_Msk             (0x3fffffful << SDH_DMABCNT_BCNT_Pos)             /*!< SDH_T::DMABCNT: BCNT Mask              */

#define SDH_DMAINTEN_ABORTIEN_Pos        (0)                                               /*!< SDH_T::DMAINTEN: ABORTIEN Position     */
#define SDH_DMAINTEN_ABORTIEN_Msk        (0x1ul << SDH_DMAINTEN_ABORTIEN_Pos)              /*!< SDH_T::DMAINTEN: ABORTIEN Mask         */

#define SDH_DMAINTEN_WEOTIEN_Pos         (1)                                               /*!< SDH_T::DMAINTEN: WEOTIEN Position      */
#define SDH_DMAINTEN_WEOTIEN_Msk         (0x1ul << SDH_DMAINTEN_WEOTIEN_Pos)               /*!< SDH_T::DMAINTEN: WEOTIEN Mask          */

#define SDH_DMAINTSTS_ABORTIF_Pos        (0)                                               /*!< SDH_T::DMAINTSTS: ABORTIF Position     */
#define SDH_DMAINTSTS_ABORTIF_Msk        (0x1ul << SDH_DMAINTSTS_ABORTIF_Pos)              /*!< SDH_T::DMAINTSTS: ABORTIF Mask         */

#define SDH_DMAINTSTS_WEOTIF_Pos         (1)                                               /*!< SDH_T::DMAINTSTS: WEOTIF Position      */
#define SDH_DMAINTSTS_WEOTIF_Msk         (0x1ul << SDH_DMAINTSTS_WEOTIF_Pos)               /*!< SDH_T::DMAINTSTS: WEOTIF Mask          */

#define SDH_GCTL_GCTLRST_Pos             (0)                                               /*!< SDH_T::GCTL: GCTLRST Position          */
#define SDH_GCTL_GCTLRST_Msk             (0x1ul << SDH_GCTL_GCTLRST_Pos)                   /*!< SDH_T::GCTL: GCTLRST Mask              */

#define SDH_GCTL_SDEN_Pos                (1)                                               /*!< SDH_T::GCTL: SDEN Position             */
#define SDH_GCTL_SDEN_Msk                (0x1ul << SDH_GCTL_SDEN_Pos)                      /*!< SDH_T::GCTL: SDEN Mask                 */

#define SDH_GINTEN_DTAIEN_Pos            (0)                                               /*!< SDH_T::GINTEN: DTAIEN Position         */
#define SDH_GINTEN_DTAIEN_Msk            (0x1ul << SDH_GINTEN_DTAIEN_Pos)                  /*!< SDH_T::GINTEN: DTAIEN Mask             */

#define SDH_GINTSTS_DTAIF_Pos            (0)                                               /*!< SDH_T::GINTSTS: DTAIF Position         */
#define SDH_GINTSTS_DTAIF_Msk            (0x1ul << SDH_GINTSTS_DTAIF_Pos)                  /*!< SDH_T::GINTSTS: DTAIF Mask             */

#define SDH_CTL_COEN_Pos                 (0)                                               /*!< SDH_T::CTL: COEN Position              */
#define SDH_CTL_COEN_Msk                 (0x1ul << SDH_CTL_COEN_Pos)                       /*!< SDH_T::CTL: COEN Mask                  */

#define SDH_CTL_RIEN_Pos                 (1)                                               /*!< SDH_T::CTL: RIEN Position              */
#define SDH_CTL_RIEN_Msk                 (0x1ul << SDH_CTL_RIEN_Pos)                       /*!< SDH_T::CTL: RIEN Mask                  */

#define SDH_CTL_DIEN_Pos                 (2)                                               /*!< SDH_T::CTL: DIEN Position              */
#define SDH_CTL_DIEN_Msk                 (0x1ul << SDH_CTL_DIEN_Pos)                       /*!< SDH_T::CTL: DIEN Mask                  */

#define SDH_CTL_DOEN_Pos                 (3)                                               /*!< SDH_T::CTL: DOEN Position              */
#define SDH_CTL_DOEN_Msk                 (0x1ul << SDH_CTL_DOEN_Pos)                       /*!< SDH_T::CTL: DOEN Mask                  */

#define SDH_CTL_R2EN_Pos                 (4)                                               /*!< SDH_T::CTL: R2EN Position              */
#define SDH_CTL_R2EN_Msk                 (0x1ul << SDH_CTL_R2EN_Pos)                       /*!< SDH_T::CTL: R2EN Mask                  */

#define SDH_CTL_CLK74OEN_Pos             (5)                                               /*!< SDH_T::CTL: CLK74OEN Position          */
#define SDH_CTL_CLK74OEN_Msk             (0x1ul << SDH_CTL_CLK74OEN_Pos)                   /*!< SDH_T::CTL: CLK74OEN Mask              */

#define SDH_CTL_CLK8OEN_Pos              (6)                                               /*!< SDH_T::CTL: CLK8OEN Position           */
#define SDH_CTL_CLK8OEN_Msk              (0x1ul << SDH_CTL_CLK8OEN_Pos)                    /*!< SDH_T::CTL: CLK8OEN Mask               */

#define SDH_CTL_CLKKEEP_Pos              (7)                                               /*!< SDH_T::CTL: CLKKEEP Position          */
#define SDH_CTL_CLKKEEP_Msk              (0x1ul << SDH_CTL_CLKKEEP_Pos)                    /*!< SDH_T::CTL: CLKKEEP Mask              */

#define SDH_CTL_CMDCODE_Pos              (8)                                               /*!< SDH_T::CTL: CMDCODE Position           */
#define SDH_CTL_CMDCODE_Msk              (0x3ful << SDH_CTL_CMDCODE_Pos)                   /*!< SDH_T::CTL: CMDCODE Mask               */

#define SDH_CTL_CTLRST_Pos               (14)                                              /*!< SDH_T::CTL: CTLRST Position            */
#define SDH_CTL_CTLRST_Msk               (0x1ul << SDH_CTL_CTLRST_Pos)                     /*!< SDH_T::CTL: CTLRST Mask                */

#define SDH_CTL_DBW_Pos                  (15)                                              /*!< SDH_T::CTL: DBW Position               */
#define SDH_CTL_DBW_Msk                  (0x1ul << SDH_CTL_DBW_Pos)                        /*!< SDH_T::CTL: DBW Mask                   */

#define SDH_CTL_BLKCNT_Pos               (16)                                              /*!< SDH_T::CTL: BLKCNT Position            */
#define SDH_CTL_BLKCNT_Msk               (0xfful << SDH_CTL_BLKCNT_Pos)                    /*!< SDH_T::CTL: BLKCNT Mask                */

#define SDH_CTL_SDNWR_Pos                (24)                                              /*!< SDH_T::CTL: SDNWR Position             */
#define SDH_CTL_SDNWR_Msk                (0xful << SDH_CTL_SDNWR_Pos)                      /*!< SDH_T::CTL: SDNWR Mask                 */

#define SDH_CMDARG_ARGUMENT_Pos          (0)                                               /*!< SDH_T::CMDARG: ARGUMENT Position       */
#define SDH_CMDARG_ARGUMENT_Msk          (0xfffffffful << SDH_CMDARG_ARGUMENT_Pos)         /*!< SDH_T::CMDARG: ARGUMENT Mask           */

#define SDH_INTEN_BLKDIEN_Pos            (0)                                               /*!< SDH_T::INTEN: BLKDIEN Position         */
#define SDH_INTEN_BLKDIEN_Msk            (0x1ul << SDH_INTEN_BLKDIEN_Pos)                  /*!< SDH_T::INTEN: BLKDIEN Mask             */

#define SDH_INTEN_CRCIEN_Pos             (1)                                               /*!< SDH_T::INTEN: CRCIEN Position          */
#define SDH_INTEN_CRCIEN_Msk             (0x1ul << SDH_INTEN_CRCIEN_Pos)                   /*!< SDH_T::INTEN: CRCIEN Mask              */

#define SDH_INTEN_CDIEN_Pos              (8)                                               /*!< SDH_T::INTEN: CDIEN Position          */
#define SDH_INTEN_CDIEN_Msk              (0x1ul << SDH_INTEN_CDIEN_Pos)                    /*!< SDH_T::INTEN: CDIEN Mask              */

#define SDH_INTEN_RTOIEN_Pos             (12)                                              /*!< SDH_T::INTEN: RTOIEN Position          */
#define SDH_INTEN_RTOIEN_Msk             (0x1ul << SDH_INTEN_RTOIEN_Pos)                   /*!< SDH_T::INTEN: RTOIEN Mask              */

#define SDH_INTEN_DITOIEN_Pos            (13)                                              /*!< SDH_T::INTEN: DITOIEN Position         */
#define SDH_INTEN_DITOIEN_Msk            (0x1ul << SDH_INTEN_DITOIEN_Pos)                  /*!< SDH_T::INTEN: DITOIEN Mask             */

#define SDH_INTEN_WKIEN_Pos              (14)                                              /*!< SDH_T::INTEN: WKIEN Position           */
#define SDH_INTEN_WKIEN_Msk              (0x1ul << SDH_INTEN_WKIEN_Pos)                    /*!< SDH_T::INTEN: WKIEN Mask               */

#define SDH_INTEN_CDSRC_Pos              (30)                                              /*!< SDH_T::INTEN: CDSRC Position          */
#define SDH_INTEN_CDSRC_Msk              (0x1ul << SDH_INTEN_CDSRC_Pos)                    /*!< SDH_T::INTEN: CDSRC Mask              */

#define SDH_INTSTS_BLKDIF_Pos            (0)                                               /*!< SDH_T::INTSTS: BLKDIF Position         */
#define SDH_INTSTS_BLKDIF_Msk            (0x1ul << SDH_INTSTS_BLKDIF_Pos)                  /*!< SDH_T::INTSTS: BLKDIF Mask             */

#define SDH_INTSTS_CRCIF_Pos             (1)                                               /*!< SDH_T::INTSTS: CRCIF Position          */
#define SDH_INTSTS_CRCIF_Msk             (0x1ul << SDH_INTSTS_CRCIF_Pos)                   /*!< SDH_T::INTSTS: CRCIF Mask              */

#define SDH_INTSTS_CRC7_Pos              (2)                                               /*!< SDH_T::INTSTS: CRC7 Position           */
#define SDH_INTSTS_CRC7_Msk              (0x1ul << SDH_INTSTS_CRC7_Pos)                    /*!< SDH_T::INTSTS: CRC7 Mask               */

#define SDH_INTSTS_CRC16_Pos             (3)                                               /*!< SDH_T::INTSTS: CRC16 Position          */
#define SDH_INTSTS_CRC16_Msk             (0x1ul << SDH_INTSTS_CRC16_Pos)                   /*!< SDH_T::INTSTS: CRC16 Mask              */

#define SDH_INTSTS_CRCSTS_Pos            (4)                                               /*!< SDH_T::INTSTS: CRCSTS Position         */
#define SDH_INTSTS_CRCSTS_Msk            (0x7ul << SDH_INTSTS_CRCSTS_Pos)                  /*!< SDH_T::INTSTS: CRCSTS Mask             */

#define SDH_INTSTS_DAT0STS_Pos           (7)                                               /*!< SDH_T::INTSTS: DAT0STS Position        */
#define SDH_INTSTS_DAT0STS_Msk           (0x1ul << SDH_INTSTS_DAT0STS_Pos)                 /*!< SDH_T::INTSTS: DAT0STS Mask            */

#define SDH_INTSTS_CDIF_Pos              (8)                                               /*!< SDH_T::INTSTS: CDIF Position          */
#define SDH_INTSTS_CDIF_Msk              (0x1ul << SDH_INTSTS_CDIF_Pos)                    /*!< SDH_T::INTSTS: CDIF Mask              */

#define SDH_INTSTS_RTOIF_Pos             (12)                                              /*!< SDH_T::INTSTS: RTOIF Position          */
#define SDH_INTSTS_RTOIF_Msk             (0x1ul << SDH_INTSTS_RTOIF_Pos)                   /*!< SDH_T::INTSTS: RTOIF Mask              */

#define SDH_INTSTS_DITOIF_Pos            (13)                                              /*!< SDH_T::INTSTS: DITOIF Position         */
#define SDH_INTSTS_DITOIF_Msk            (0x1ul << SDH_INTSTS_DITOIF_Pos)                  /*!< SDH_T::INTSTS: DITOIF Mask             */

#define SDH_INTSTS_CDSTS_Pos             (16)                                              /*!< SDH_T::INTSTS: CDSTS Position         */
#define SDH_INTSTS_CDSTS_Msk             (0x1ul << SDH_INTSTS_CDSTS_Pos)                   /*!< SDH_T::INTSTS: CDSTS Mask             */

#define SDH_INTSTS_DAT1STS_Pos           (18)                                              /*!< SDH_T::INTSTS: DAT1STS Position        */
#define SDH_INTSTS_DAT1STS_Msk           (0x1ul << SDH_INTSTS_DAT1STS_Pos)                 /*!< SDH_T::INTSTS: DAT1STS Mask            */

#define SDH_RESP0_RESPTK0_Pos            (0)                                               /*!< SDH_T::RESP0: RESPTK0 Position         */
#define SDH_RESP0_RESPTK0_Msk            (0xfffffffful << SDH_RESP0_RESPTK0_Pos)           /*!< SDH_T::RESP0: RESPTK0 Mask             */

#define SDH_RESP1_RESPTK1_Pos            (0)                                               /*!< SDH_T::RESP1: RESPTK1 Position         */
#define SDH_RESP1_RESPTK1_Msk            (0xfful << SDH_RESP1_RESPTK1_Pos)                 /*!< SDH_T::RESP1: RESPTK1 Mask             */

#define SDH_BLEN_BLKLEN_Pos              (0)                                               /*!< SDH_T::BLEN: BLKLEN Position           */
#define SDH_BLEN_BLKLEN_Msk              (0x7fful << SDH_BLEN_BLKLEN_Pos)                  /*!< SDH_T::BLEN: BLKLEN Mask               */

#define SDH_TOUT_TOUT_Pos                (0)                                               /*!< SDH_T::TOUT: TOUT Position             */
#define SDH_TOUT_TOUT_Msk                (0xfffffful << SDH_TOUT_TOUT_Pos)                 /*!< SDH_T::TOUT: TOUT Mask                 */

/**@}*/ /* SDH_CONST */
/**@}*/ /* end of SDH register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __SDH_REG_H__ */

