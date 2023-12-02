/**************************************************************************//**
 * @file     usbh_reg.h
 * @version  V1.00
 * @brief    USBH register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __USBH_REG_H__
#define __USBH_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup USBH USB Host Controller(USBH)
    Memory Mapped Structure for USBH Controller
@{ */

typedef struct
{

    /**
     * @var USBH_T::HcRevision
     * Offset: 0x00  Host Controller Revision Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |REV       |Revision Number
     * |        |          |Indicates the Open HCI Specification revision number implemented by the Hardware
     * |        |          |Host Controller supports 1.1 specification.
     * |        |          |(X.Y = XYh).
     * @var USBH_T::HcControl
     * Offset: 0x04  Host Controller Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |CBSR      |Control Bulk Service Ratio
     * |        |          |This specifies the service ratio between Control and Bulk EDs
     * |        |          |Before processing any of the non-periodic lists, HC must compare the ratio specified with its internal count on how many nonempty Control EDs have been processed, in determining whether to continue serving another Control ED or switching to Bulk EDs
     * |        |          |The internal count will be retained when crossing the frame boundary
     * |        |          |In case of reset, HCD is responsible for restoring this
     * |        |          |Value.
     * |        |          |00 = Number of Control EDs over Bulk EDs served is 1:1.
     * |        |          |01 = Number of Control EDs over Bulk EDs served is 2:1.
     * |        |          |10 = Number of Control EDs over Bulk EDs served is 3:1.
     * |        |          |11 = Number of Control EDs over Bulk EDs served is 4:1.
     * |[2]     |PLE       |Periodic List Enable Bit
     * |        |          |When set, this bit enables processing of the Periodic (interrupt and isochronous) list
     * |        |          |The Host Controller checks this bit prior to attempting any periodic transfers in a frame.
     * |        |          |0 = Processing of the Periodic (Interrupt and Isochronous) list after next SOF (Start-Of-Frame) Disabled.
     * |        |          |1 = Processing of the Periodic (Interrupt and Isochronous) list in the next frame Enabled.
     * |        |          |Note: To enable the processing of the Isochronous list, user has to set both PLE and IE (HcControl[3]) high.
     * |[3]     |IE        |Isochronous List Enable Bit
     * |        |          |Both ISOEn and PLE (HcControl[2]) high enables Host Controller to process the Isochronous list
     * |        |          |Either ISOEn or PLE (HcControl[2]) is low disables Host Controller to process the Isochronous list.
     * |        |          |0 = Processing of the Isochronous list after next SOF (Start-Of-Frame) Disabled.
     * |        |          |1 = Processing of the Isochronous list in the next frame Enabled, if the PLE (HcControl[2]) is high, too.
     * |[4]     |CLE       |Control List Enable Bit
     * |        |          |0 = Processing of the Control list after next SOF (Start-Of-Frame) Disabled.
     * |        |          |1 = Processing of the Control list in the next frame Enabled.
     * |[5]     |BLE       |Bulk List Enable Bit
     * |        |          |0 = Processing of the Bulk list after next SOF (Start-Of-Frame) Disabled.
     * |        |          |1 = Processing of the Bulk list in the next frame Enabled.
     * |[7:6]   |HCFS      |Host Controller Functional State
     * |        |          |This field sets the Host Controller state
     * |        |          |The Controller may force a state change from USBSUSPEND to USBRESUME after detecting resume signaling from a downstream port
     * |        |          |States are:
     * |        |          |00 = USBSUSPEND.
     * |        |          |01 = USBOPERATIONAL.
     * |        |          |10 = USBRESUME.
     * |        |          |11 = USBRESET.
     * @var USBH_T::HcCommandStatus
     * Offset: 0x08  Host Controller Command Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HCR       |Host Controller Reset
     * |        |          |This bit is set to initiate the software reset of Host Controller
     * |        |          |This bit is cleared by the Host Controller, upon completed of the reset operation.
     * |        |          |This bit, when set, didn't reset the Root Hub and no subsequent reset signaling be asserted to its downstream ports.
     * |        |          |0 = Host Controller is not in software reset state.
     * |        |          |1 = Host Controller is in software reset state.
     * |[1]     |CLF       |Control List Filled
     * |        |          |Set high to indicate there is an active TD on the Control List
     * |        |          |It may be set by either software or the Host Controller and cleared by the Host Controller each time it begins processing the head of the Control List.
     * |        |          |0 = No active TD found or Host Controller begins to process the head of the Control list.
     * |        |          |1 = An active TD added or found on the Control list.
     * |[2]     |BLF       |Bulk List Filled
     * |        |          |Set high to indicate there is an active TD on the Bulk list
     * |        |          |This bit may be set by either software or the Host Controller and cleared by the Host Controller each time it begins processing the head of the Bulk list.
     * |        |          |0 = No active TD found or Host Controller begins to process the head of the Bulk list.
     * |        |          |1 = An active TD added or found on the Bulk list.
     * |[17:16] |SOC       |Schedule Overrun Count
     * |        |          |These bits are incremented on each scheduling overrun error
     * |        |          |It is initialized to 00b and wraps around at 11b
     * |        |          |This will be incremented when a scheduling overrun is detected even if SO (HcInterruptStatus[0]) has already been set.
     * @var USBH_T::HcInterruptStatus
     * Offset: 0x0C  Host Controller Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SO        |Scheduling Overrun
     * |        |          |Set when the List Processor determines a Schedule Overrun has occurred.
     * |        |          |0 = Schedule Overrun didn't occur.
     * |        |          |1 = Schedule Overrun has occurred.
     * |[1]     |WDH       |Write Back Done Head
     * |        |          |Set after the Host Controller has written HcDoneHead to HccaDoneHead
     * |        |          |Further updates of the HccaDoneHead will not occur until this bit has been cleared.
     * |        |          |0 =.Host Controller didn't update HccaDoneHead.
     * |        |          |1 =.Host Controller has written HcDoneHead to HccaDoneHead.
     * |[2]     |SF        |Start of Frame
     * |        |          |Set when the Frame Management functional block signals a 'Start of Frame' event
     * |        |          |Host Control generates a SOF token at the same time.
     * |        |          |0 =.Not the start of a frame.
     * |        |          |1 =.Indicate the start of a frame and Host Controller generates a SOF token.
     * |[3]     |RD        |Resume Detected
     * |        |          |Set when Host Controller detects resume signaling on a downstream port.
     * |        |          |0 = No resume signaling detected on a downstream port.
     * |        |          |1 = Resume signaling detected on a downstream port.
     * |[5]     |FNO       |Frame Number Overflow
     * |        |          |This bit is set when bit 15 of Frame Number changes from 1 to 0 or from 0 to 1.
     * |        |          |0 = The bit 15 of Frame Number didn't change.
     * |        |          |1 = The bit 15 of Frame Number changes from 1 to 0 or from 0 to 1.
     * |[6]     |RHSC      |Root Hub Status Change
     * |        |          |This bit is set when the content of HcRhStatus or the content of HcRhPortStatus register has changed.
     * |        |          |0 = The content of HcRhStatus and the content of HcRhPortStatus register didn't change.
     * |        |          |1 = The content of HcRhStatus or the content of HcRhPortStatus register has changed.
     * @var USBH_T::HcInterruptEnable
     * Offset: 0x10  Host Controller Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SO        |Scheduling Overrun Enable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to SO (HcInterruptStatus[0]) Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to SO (HcInterruptStatus[0]) Disabled.
     * |        |          |1 = Interrupt generation due to SO (HcInterruptStatus[0]) Enabled.
     * |[1]     |WDH       |Write Back Done Head Enable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to WDH (HcInterruptStatus[1]) Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to WDH (HcInterruptStatus[1]) Disabled.
     * |        |          |1 = Interrupt generation due to WDH (HcInterruptStatus[1]) Enabled.
     * |[2]     |SF        |Start of Frame Enable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to SF (HcInterruptStatus[2]) Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to SF (HcInterruptStatus[2]) Disabled.
     * |        |          |1 = Interrupt generation due to SF (HcInterruptStatus[2]) Enabled.
     * |[3]     |RD        |Resume Detected Enable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to RD (HcInterruptStatus[3]) Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to RD (HcInterruptStatus[3]) Disabled.
     * |        |          |1 = Interrupt generation due to RD (HcInterruptStatus[3]) Enabled.
     * |[5]     |FNO       |Frame Number Overflow Enable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to FNO (HcInterruptStatus[5]) Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to FNO (HcInterruptStatus[5]) Disabled.
     * |        |          |1 = Interrupt generation due to FNO (HcInterruptStatus[5]) Enabled.
     * |[6]     |RHSC      |Root Hub Status Change Enable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]) Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to RHSC (HcInterruptStatus[6]) Disabled.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]) Enabled.
     * |[31]    |MIE       |Master Interrupt Enable Bit
     * |        |          |This bit is a global interrupt enable
     * |        |          |A write of '1' allows interrupts to be enabled via the specific enable bits listed above.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]), FNO (HcInterruptStatus[5]), RD (HcInterruptStatus[3]), SF (HcInterruptStatus[2]), WDH (HcInterruptStatus[1]) or SO (HcInterruptStatus[0]) Enabled if the corresponding bit in HcInterruptEnable is high.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to RHSC (HcInterruptStatus[6]), FNO (HcInterruptStatus[5]), RD (HcInterruptStatus[3]), SF (HcInterruptStatus[2]), WDH (HcInterruptStatus[1]) or SO (HcInterruptStatus[0]) Disabled even if the corresponding bit in HcInterruptEnable is high.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]), FNO (HcInterruptStatus[5]), RD (HcInterruptStatus[3]), SF (HcInterruptStatus[2]), WDH (HcInterruptStatus[1]) or SO (HcInterruptStatus[0]) Enabled if the corresponding bit in HcInterruptEnable is high.
     * @var USBH_T::HcInterruptDisable
     * Offset: 0x14  Host Controller Interrupt Disable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SO        |Scheduling Overrun Disable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to SO (HcInterruptStatus[0]) Disabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to SO (HcInterruptStatus[0]) Disabled.
     * |        |          |1 = Interrupt generation due to SO (HcInterruptStatus[0]) Enabled.
     * |[1]     |WDH       |Write Back Done Head Disable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to WDH (HcInterruptStatus[1]) Disabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to WDH (HcInterruptStatus[1]) Disabled.
     * |        |          |1 = Interrupt generation due to WDH (HcInterruptStatus[1]) Enabled.
     * |[2]     |SF        |Start of Frame Disable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to SF (HcInterruptStatus[2]) Disabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to SF (HcInterruptStatus[2]) Disabled.
     * |        |          |1 = Interrupt generation due to SF (HcInterruptStatus[2]) Enabled.
     * |[3]     |RD        |Resume Detected Disable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to RD (HcInterruptStatus[3]) Disabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to RD (HcInterruptStatus[3]) Disabled.
     * |        |          |1 = Interrupt generation due to RD (HcInterruptStatus[3]) Enabled.
     * |[5]     |FNO       |Frame Number Overflow Disable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to FNO (HcInterruptStatus[5]) Disabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to FNO (HcInterruptStatus[5]) Disabled.
     * |        |          |1 = Interrupt generation due to FNO (HcInterruptStatus[5]) Enabled.
     * |[6]     |RHSC      |Root Hub Status Change Disable Bit
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]) Disabled.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to RHSC (HcInterruptStatus[6]) Disabled.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]) Enabled.
     * |[31]    |MIE       |Master Interrupt Disable Bit
     * |        |          |Global interrupt disable. Writing '1' to disable all interrupts.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]), FNO (HcInterruptStatus[5]), RD (HcInterruptStatus[3]), SF (HcInterruptStatus[2]), WDH (HcInterruptStatus[1]) or SO (HcInterruptStatus[0]) Disabled if the corresponding bit in HcInterruptEnable is high.
     * |        |          |Read Operation:
     * |        |          |0 = Interrupt generation due to RHSC (HcInterruptStatus[6]), FNO (HcInterruptStatus[5]), RD (HcInterruptStatus[3]), SF (HcInterruptStatus[2]), WDH (HcInterruptStatus[1]) or SO (HcInterruptStatus[0]) Disabled even if the corresponding bit in HcInterruptEnable is high.
     * |        |          |1 = Interrupt generation due to RHSC (HcInterruptStatus[6]), FNO (HcInterruptStatus[5]), RD (HcInterruptStatus[3]), SF (HcInterruptStatus[2]), WDH (HcInterruptStatus[1]) or SO (HcInterruptStatus[0]) Enabled if the corresponding bit in HcInterruptEnable is high.
     * @var USBH_T::HcHCCA
     * Offset: 0x18  Host Controller Communication Area Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:8]  |HCCA      |Host Controller Communication Area
     * |        |          |Pointer to indicate base address of the Host Controller Communication Area (HCCA).
     * @var USBH_T::HcPeriodCurrentED
     * Offset: 0x1C  Host Controller Period Current ED Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:4]  |PCED      |Periodic Current ED
     * |        |          |Pointer to indicate physical address of the current Isochronous or Interrupt Endpoint Descriptor.
     * @var USBH_T::HcControlHeadED
     * Offset: 0x20  Host Controller Control Head ED Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:4]  |CHED      |Control Head ED
     * |        |          |Pointer to indicate physical address of the first Endpoint Descriptor of the Control list.
     * @var USBH_T::HcControlCurrentED
     * Offset: 0x24  Host Controller Control Current ED Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:4]  |CCED      |Control Current Head ED
     * |        |          |Pointer to indicate the physical address of the current Endpoint Descriptor of the Control list.
     * @var USBH_T::HcBulkHeadED
     * Offset: 0x28  Host Controller Bulk Head ED Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:4]  |BHED      |Bulk Head ED
     * |        |          |Pointer to indicate the physical address of the first Endpoint Descriptor of the Bulk list.
     * @var USBH_T::HcBulkCurrentED
     * Offset: 0x2C  Host Controller Bulk Current ED Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:4]  |BCED      |Bulk Current Head ED
     * |        |          |Pointer to indicate the physical address of the current endpoint of the Bulk list.
     * @var USBH_T::HcDoneHead
     * Offset: 0x30  Host Controller Done Head Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:4]  |DH        |Done Head
     * |        |          |Pointer to indicate the physical address of the last completed Transfer Descriptor that was added to the Done queue.
     * @var USBH_T::HcFmInterval
     * Offset: 0x34  Host Controller Frame Interval Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[13:0]  |FI        |Frame Interval
     * |        |          |This field specifies the length of a frame as (bit times - 1)
     * |        |          |For 12,000 bit times in a frame, a value of 11,999 is stored here.
     * |[30:16] |FSMPS     |FS Largest Data Packet
     * |        |          |This field specifies a value that is loaded into the Largest Data Packet Counter at the beginning of each frame.
     * |[31]    |FIT       |Frame Interval Toggle
     * |        |          |This bit is toggled by Host Controller Driver when it loads a new value into FI (HcFmInterval[13:0]).
     * |        |          |0 = Host Controller Driver didn't load new value into FI (HcFmInterval[13:0]).
     * |        |          |1 = Host Controller Driver loads a new value into FI (HcFmInterval[13:0]).
     * @var USBH_T::HcFmRemaining
     * Offset: 0x38  Host Controller Frame Remaining Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[13:0]  |FR        |Frame Remaining
     * |        |          |When the Host Controller is in the USBOPERATIONAL state, this 14-bit field decrements each 12 MHz clock period
     * |        |          |When the count reaches 0, (end of frame) the counter reloads with Frame Interval
     * |        |          |In addition, the counter loads when the Host Controller transitions into USBOPERATIONAL.
     * |[31]    |FRT       |Frame Remaining Toggle
     * |        |          |This bit is loaded from the FIT (HcFmInterval[31]) whenever FR (HcFmRemaining[13:0]) reaches 0.
     * @var USBH_T::HcFmNumber
     * Offset: 0x3C  Host Controller Frame Number Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |FN        |Frame Number
     * |        |          |This 16-bit incrementing counter field is incremented coincident with the re-load of FR (HcFmRemaining[13:0])
     * |        |          |The count rolls over from 'FFFFh' to '0h.'
     * @var USBH_T::HcPeriodicStart
     * Offset: 0x40  Host Controller Periodic Start Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[13:0]  |PS        |Periodic Start
     * |        |          |This field contains a value used by the List Processor to determine where in a frame the Periodic List processing must begin.
     * @var USBH_T::HcLSThreshold
     * Offset: 0x44  Host Controller Low-speed Threshold Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |LST       |Low-speed Threshold
     * |        |          |This field contains a value which is compared to the FR (HcFmRemaining[13:0]) field prior to initiating a Low-speed transaction
     * |        |          |The transaction is started only if FR (HcFmRemaining[13:0]) >= this field
     * |        |          |The value is calculated by Host Controller Driver with the consideration of transmission and setup overhead.
     * @var USBH_T::HcRhDescriptorA
     * Offset: 0x48  Host Controller Root Hub Descriptor A Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |NDP       |Number Downstream Ports
     * |        |          |USB host control supports two downstream ports and only one port is available in this series of chip.
     * |[8]     |PSM       |Power Switching Mode
     * |        |          |This bit is used to specify how the power switching of the Root Hub ports is controlled.
     * |        |          |0 = Global Switching.
     * |        |          |1 = Individual Switching.
     * |[11]    |OCPM      |over Current Protection Mode
     * |        |          |This bit describes how the over current status for the Root Hub ports reported
     * |        |          |This bit is only valid when NOCP (HcRhDescriptorA[12]) is cleared.
     * |        |          |0 = Global Over current.
     * |        |          |1 = Individual Over current.
     * |[12]    |NOCP      |No over Current Protection
     * |        |          |This bit describes how the over current status for the Root Hub ports reported.
     * |        |          |0 = Over current status is reported.
     * |        |          |1 = Over current status is not reported.
     * @var USBH_T::HcRhDescriptorB
     * Offset: 0x4C  Host Controller Root Hub Descriptor B Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:16] |PPCM      |Port Power Control Mask
     * |        |          |Global power switching
     * |        |          |This field is only valid if PowerSwitchingMode is set (individual port switching)
     * |        |          |When set, the port only responds to individual port power switching commands (Set/ClearPortPower)
     * |        |          |When cleared, the port only responds to global power switching commands (Set/ClearGlobalPower).
     * |        |          |0 = Port power controlled by global power switching.
     * |        |          |1 = Port power controlled by port power switching.
     * |        |          |Note: PPCM[15:2] and PPCM[0] are reserved.
     * @var USBH_T::HcRhStatus
     * Offset: 0x50  Host Controller Root Hub Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LPS       |Clear Global Power
     * |        |          |In global power mode (PSM (HcRhDescriptorA[8]) = 0), this bit is written to one to clear all ports' power.
     * |        |          |This bit always read as zero.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Clear global power.
     * |[1]     |OCI       |over Current Indicator
     * |        |          |This bit reflects the state of the over current status pin
     * |        |          |This field is only valid if NOCP (HcRhDesA[12]) and OCPM (HcRhDesA[11]) are cleared.
     * |        |          |0 = No over current condition.
     * |        |          |1 = Over current condition.
     * |[15]    |DRWE      |Device Remote Wakeup Enable Bit
     * |        |          |This bit controls if port's Connect Status Change as a remote wake-up event.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Connect Status Change as a remote wake-up event Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Connect Status Change as a remote wake-up event Disabled.
     * |        |          |1 = Connect Status Change as a remote wake-up event Enabled.
     * |[16]    |LPSC      |Set Global Power
     * |        |          |In global power mode (PSM (HcRhDescriptorA[8]) = 0), this bit is written to one to enable power to all ports.
     * |        |          |This bit always read as zero.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set global power.
     * |[17]    |OCIC      |over Current Indicator Change
     * |        |          |This bit is set by hardware when a change has occurred in OCI (HcRhStatus[1]).
     * |        |          |Write 1 to clear this bit to zero.
     * |        |          |0 = OCI (HcRhStatus[1]) didn't change.
     * |        |          |1 = OCI (HcRhStatus[1]) change.
     * |[31]    |CRWE      |Clear Remote Wake-up Enable Bit
     * |        |          |This bit is use to clear DRWE (HcRhStatus[15]).
     * |        |          |This bit always read as zero.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Clear DRWE (HcRhStatus[15]).
     * @var USBH_T::HcRhPortStatus[2]
     * Offset: 0x54  Host Controller Root Hub Port Status
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CCS       |CurrentConnectStatus (Read) or ClearPortEnable Bit (Write)
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Clear port enable.
     * |        |          |Read Operation:
     * |        |          |0 = No device connected.
     * |        |          |1 = Device connected.
     * |[1]     |PES       |Port Enable Status
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set port enable.
     * |        |          |Read Operation:
     * |        |          |0 = Port Disabled.
     * |        |          |1 = Port Enabled.
     * |[2]     |PSS       |Port Suspend Status
     * |        |          |This bit indicates the port is suspended
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set port suspend.
     * |        |          |Read Operation:
     * |        |          |0 = Port is not suspended.
     * |        |          |1 = Port is selectively suspended.
     * |[3]     |POCI      |Port over Current Indicator (Read) or Clear Port Suspend (Write)
     * |        |          |This bit reflects the state of the over current status pin dedicated to this port
     * |        |          |This field is only valid if NOCP (HcRhDescriptorA[12]) is cleared and OCPM (HcRhDescriptorA[11]) is set.
     * |        |          |This bit is also used to initiate the selective result sequence for the port.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Clear port suspend.
     * |        |          |Read Operation:
     * |        |          |0 = No over current condition.
     * |        |          |1 = Over current condition.
     * |[4]     |PRS       |Port Reset Status
     * |        |          |This bit reflects the reset state of the port.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set port reset.
     * |        |          |Read Operation
     * |        |          |0 = Port reset signal is not active.
     * |        |          |1 = Port reset signal is active.
     * |[8]     |PPS       |Port Power Status
     * |        |          |This bit reflects the power state of the port regardless of the power switching mode.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Port Power Enabled.
     * |        |          |Read Operation:
     * |        |          |0 = Port power is Disabled.
     * |        |          |1 = Port power is Enabled.
     * |[9]     |LSDA      |Low Speed Device Attached (Read) or Clear Port Power (Write)
     * |        |          |This bit defines the speed (and bud idle) of the attached device
     * |        |          |It is only valid when CCS (HcRhPortStatus1[0]) is set.
     * |        |          |This bit is also used to clear port power.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Clear PPS (HcRhPortStatus1[8]).
     * |        |          |Read Operation:
     * |        |          |0 = Full Speed device.
     * |        |          |1 = Low-speed device.
     * |[16]    |CSC       |Connect Status Change
     * |        |          |This bit indicates connect or disconnect event has been detected (CCS (HcRhPortStatus1[0]) changed).
     * |        |          |Write 1 to clear this bit to zero.
     * |        |          |0 = No connect/disconnect event (CCS (HcRhPortStatus1[0]) didn't change).
     * |        |          |1 = Hardware detection of connect/disconnect event (CCS (HcRhPortStatus1[0]) changed).
     * |[17]    |PESC      |Port Enable Status Change
     * |        |          |This bit indicates that the port has been disabled (PES (HcRhPortStatus1[1]) cleared) due to a hardware event.
     * |        |          |Write 1 to clear this bit to zero.
     * |        |          |0 = PES (HcRhPortStatus1[1]) didn't change.
     * |        |          |1 = PES (HcRhPortStatus1[1]) changed.
     * |[18]    |PSSC      |Port Suspend Status Change
     * |        |          |This bit indicates the completion of the selective resume sequence for the port.
     * |        |          |Write 1 to clear this bit to zero.
     * |        |          |0 = Port resume is not completed.
     * |        |          |1 = Port resume completed.
     * |[19]    |OCIC      |Port over Current Indicator Change
     * |        |          |This bit is set when POCI (HcRhPortStatus1[3]) changes.
     * |        |          |Write 1 to clear this bit to zero.
     * |        |          |0 = POCI (HcRhPortStatus1[3]) didn't change.
     * |        |          |1 = POCI (HcRhPortStatus1[3]) changes.
     * |[20]    |PRSC      |Port Reset Status Change
     * |        |          |This bit indicates that the port reset signal has completed.
     * |        |          |Write 1 to clear this bit to zero.
     * |        |          |0 = Port reset is not complete.
     * |        |          |1 = Port reset is complete.
     * @var USBH_T::HcPhyControl
     * Offset: 0x200  Host Controller PHY Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[27]    |STBYEN    |USB Transceiver Standby Enable Bit
     * |        |          |This bit controls if USB transceiver could enter the standby mode to reduce power consumption.
     * |        |          |0 = The USB transceiver would never enter the standby mode.
     * |        |          |1 = The USB transceiver will enter standby mode while port is in power off state (port power is inactive).
     * @var USBH_T::HcMiscControl
     * Offset: 0x204  Host Controller Miscellaneous Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |ABORT     |AHB Bus ERROR Response
     * |        |          |This bit indicates there is an ERROR response received in AHB bus.
     * |        |          |0 = No ERROR response received.
     * |        |          |1 = ERROR response received.
     * |[3]     |OCAL      |over Current Active Low
     * |        |          |This bit controls the polarity of over current flag from external power IC.
     * |        |          |0 = Over current flag is high active.
     * |        |          |1 = Over current flag is low active.
     * |[16]    |DPRT1     |Disable Port 1
     * |        |          |This bit controls if the connection between USB host controller and transceiver of port 1 is disabled
     * |        |          |If the connection is disabled, the USB host controller will not recognize any event of USB bus.
     * |        |          |Set this bit high, the transceiver of port 1 will also be forced into the standby mode no matter what USB host controller operation is.
     * |        |          |0 = The connection between USB host controller and transceiver of port 1 Enabled.
     * |        |          |1 = The connection between USB host controller and transceiver of port 1 Disabled and the transceiver of port 1 will also be forced into the standby mode.
     */
    __I  uint32_t HcRevision;            /*!< [0x0000] Host Controller Revision Register                                */
    __IO uint32_t HcControl;             /*!< [0x0004] Host Controller Control Register                                 */
    __IO uint32_t HcCommandStatus;       /*!< [0x0008] Host Controller Command Status Register                          */
    __IO uint32_t HcInterruptStatus;     /*!< [0x000c] Host Controller Interrupt Status Register                        */
    __IO uint32_t HcInterruptEnable;     /*!< [0x0010] Host Controller Interrupt Enable Register                        */
    __IO uint32_t HcInterruptDisable;    /*!< [0x0014] Host Controller Interrupt Disable Register                       */
    __IO uint32_t HcHCCA;                /*!< [0x0018] Host Controller Communication Area Register                      */
    __IO uint32_t HcPeriodCurrentED;     /*!< [0x001c] Host Controller Period Current ED Register                       */
    __IO uint32_t HcControlHeadED;       /*!< [0x0020] Host Controller Control Head ED Register                         */
    __IO uint32_t HcControlCurrentED;    /*!< [0x0024] Host Controller Control Current ED Register                      */
    __IO uint32_t HcBulkHeadED;          /*!< [0x0028] Host Controller Bulk Head ED Register                            */
    __IO uint32_t HcBulkCurrentED;       /*!< [0x002c] Host Controller Bulk Current ED Register                         */
    __IO uint32_t HcDoneHead;            /*!< [0x0030] Host Controller Done Head Register                               */
    __IO uint32_t HcFmInterval;          /*!< [0x0034] Host Controller Frame Interval Register                          */
    __I  uint32_t HcFmRemaining;         /*!< [0x0038] Host Controller Frame Remaining Register                         */
    __I  uint32_t HcFmNumber;            /*!< [0x003c] Host Controller Frame Number Register                            */
    __IO uint32_t HcPeriodicStart;       /*!< [0x0040] Host Controller Periodic Start Register                          */
    __IO uint32_t HcLSThreshold;         /*!< [0x0044] Host Controller Low-speed Threshold Register                     */
    __IO uint32_t HcRhDescriptorA;       /*!< [0x0048] Host Controller Root Hub Descriptor A Register                   */
    __IO uint32_t HcRhDescriptorB;       /*!< [0x004c] Host Controller Root Hub Descriptor B Register                   */
    __IO uint32_t HcRhStatus;            /*!< [0x0050] Host Controller Root Hub Status Register                         */
    __IO uint32_t HcRhPortStatus[2];     /*!< [0x0054] Host Controller Root Hub Port Status [1]                         */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[105];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t HcPhyControl;          /*!< [0x0200] Host Controller PHY Control Register                             */
    __IO uint32_t HcMiscControl;         /*!< [0x0204] Host Controller Miscellaneous Control Register                   */

} USBH_T;

/**
    @addtogroup USBH_CONST USBH Bit Field Definition
    Constant Definitions for USBH Controller
@{ */

#define USBH_HcRevision_REV_Pos          (0)                                               /*!< USBH_T::HcRevision: REV Position       */
#define USBH_HcRevision_REV_Msk          (0xfful << USBH_HcRevision_REV_Pos)               /*!< USBH_T::HcRevision: REV Mask           */

#define USBH_HcControl_CBSR_Pos          (0)                                               /*!< USBH_T::HcControl: CBSR Position       */
#define USBH_HcControl_CBSR_Msk          (0x3ul << USBH_HcControl_CBSR_Pos)                /*!< USBH_T::HcControl: CBSR Mask           */

#define USBH_HcControl_PLE_Pos           (2)                                               /*!< USBH_T::HcControl: PLE Position        */
#define USBH_HcControl_PLE_Msk           (0x1ul << USBH_HcControl_PLE_Pos)                 /*!< USBH_T::HcControl: PLE Mask            */

#define USBH_HcControl_IE_Pos            (3)                                               /*!< USBH_T::HcControl: IE Position         */
#define USBH_HcControl_IE_Msk            (0x1ul << USBH_HcControl_IE_Pos)                  /*!< USBH_T::HcControl: IE Mask             */

#define USBH_HcControl_CLE_Pos           (4)                                               /*!< USBH_T::HcControl: CLE Position        */
#define USBH_HcControl_CLE_Msk           (0x1ul << USBH_HcControl_CLE_Pos)                 /*!< USBH_T::HcControl: CLE Mask            */

#define USBH_HcControl_BLE_Pos           (5)                                               /*!< USBH_T::HcControl: BLE Position        */
#define USBH_HcControl_BLE_Msk           (0x1ul << USBH_HcControl_BLE_Pos)                 /*!< USBH_T::HcControl: BLE Mask            */

#define USBH_HcControl_HCFS_Pos          (6)                                               /*!< USBH_T::HcControl: HCFS Position       */
#define USBH_HcControl_HCFS_Msk          (0x3ul << USBH_HcControl_HCFS_Pos)                /*!< USBH_T::HcControl: HCFS Mask           */

#define USBH_HcCommandStatus_HCR_Pos     (0)                                               /*!< USBH_T::HcCommandStatus: HCR Position  */
#define USBH_HcCommandStatus_HCR_Msk     (0x1ul << USBH_HcCommandStatus_HCR_Pos)           /*!< USBH_T::HcCommandStatus: HCR Mask      */

#define USBH_HcCommandStatus_CLF_Pos     (1)                                               /*!< USBH_T::HcCommandStatus: CLF Position  */
#define USBH_HcCommandStatus_CLF_Msk     (0x1ul << USBH_HcCommandStatus_CLF_Pos)           /*!< USBH_T::HcCommandStatus: CLF Mask      */

#define USBH_HcCommandStatus_BLF_Pos     (2)                                               /*!< USBH_T::HcCommandStatus: BLF Position  */
#define USBH_HcCommandStatus_BLF_Msk     (0x1ul << USBH_HcCommandStatus_BLF_Pos)           /*!< USBH_T::HcCommandStatus: BLF Mask      */

#define USBH_HcCommandStatus_SOC_Pos     (16)                                              /*!< USBH_T::HcCommandStatus: SOC Position  */
#define USBH_HcCommandStatus_SOC_Msk     (0x3ul << USBH_HcCommandStatus_SOC_Pos)           /*!< USBH_T::HcCommandStatus: SOC Mask      */

#define USBH_HcInterruptStatus_SO_Pos    (0)                                               /*!< USBH_T::HcInterruptStatus: SO Position */
#define USBH_HcInterruptStatus_SO_Msk    (0x1ul << USBH_HcInterruptStatus_SO_Pos)          /*!< USBH_T::HcInterruptStatus: SO Mask     */

#define USBH_HcInterruptStatus_WDH_Pos   (1)                                               /*!< USBH_T::HcInterruptStatus: WDH Position*/
#define USBH_HcInterruptStatus_WDH_Msk   (0x1ul << USBH_HcInterruptStatus_WDH_Pos)         /*!< USBH_T::HcInterruptStatus: WDH Mask    */

#define USBH_HcInterruptStatus_SF_Pos    (2)                                               /*!< USBH_T::HcInterruptStatus: SF Position */
#define USBH_HcInterruptStatus_SF_Msk    (0x1ul << USBH_HcInterruptStatus_SF_Pos)          /*!< USBH_T::HcInterruptStatus: SF Mask     */

#define USBH_HcInterruptStatus_RD_Pos    (3)                                               /*!< USBH_T::HcInterruptStatus: RD Position */
#define USBH_HcInterruptStatus_RD_Msk    (0x1ul << USBH_HcInterruptStatus_RD_Pos)          /*!< USBH_T::HcInterruptStatus: RD Mask     */

#define USBH_HcInterruptStatus_FNO_Pos   (5)                                               /*!< USBH_T::HcInterruptStatus: FNO Position*/
#define USBH_HcInterruptStatus_FNO_Msk   (0x1ul << USBH_HcInterruptStatus_FNO_Pos)         /*!< USBH_T::HcInterruptStatus: FNO Mask    */

#define USBH_HcInterruptStatus_RHSC_Pos  (6)                                               /*!< USBH_T::HcInterruptStatus: RHSC Position*/
#define USBH_HcInterruptStatus_RHSC_Msk  (0x1ul << USBH_HcInterruptStatus_RHSC_Pos)        /*!< USBH_T::HcInterruptStatus: RHSC Mask   */

#define USBH_HcInterruptEnable_SO_Pos    (0)                                               /*!< USBH_T::HcInterruptEnable: SO Position */
#define USBH_HcInterruptEnable_SO_Msk    (0x1ul << USBH_HcInterruptEnable_SO_Pos)          /*!< USBH_T::HcInterruptEnable: SO Mask     */

#define USBH_HcInterruptEnable_WDH_Pos   (1)                                               /*!< USBH_T::HcInterruptEnable: WDH Position*/
#define USBH_HcInterruptEnable_WDH_Msk   (0x1ul << USBH_HcInterruptEnable_WDH_Pos)         /*!< USBH_T::HcInterruptEnable: WDH Mask    */

#define USBH_HcInterruptEnable_SF_Pos    (2)                                               /*!< USBH_T::HcInterruptEnable: SF Position */
#define USBH_HcInterruptEnable_SF_Msk    (0x1ul << USBH_HcInterruptEnable_SF_Pos)          /*!< USBH_T::HcInterruptEnable: SF Mask     */

#define USBH_HcInterruptEnable_RD_Pos    (3)                                               /*!< USBH_T::HcInterruptEnable: RD Position */
#define USBH_HcInterruptEnable_RD_Msk    (0x1ul << USBH_HcInterruptEnable_RD_Pos)          /*!< USBH_T::HcInterruptEnable: RD Mask     */

#define USBH_HcInterruptEnable_FNO_Pos   (5)                                               /*!< USBH_T::HcInterruptEnable: FNO Position*/
#define USBH_HcInterruptEnable_FNO_Msk   (0x1ul << USBH_HcInterruptEnable_FNO_Pos)         /*!< USBH_T::HcInterruptEnable: FNO Mask    */

#define USBH_HcInterruptEnable_RHSC_Pos  (6)                                               /*!< USBH_T::HcInterruptEnable: RHSC Position*/
#define USBH_HcInterruptEnable_RHSC_Msk  (0x1ul << USBH_HcInterruptEnable_RHSC_Pos)        /*!< USBH_T::HcInterruptEnable: RHSC Mask   */

#define USBH_HcInterruptEnable_MIE_Pos   (31)                                              /*!< USBH_T::HcInterruptEnable: MIE Position*/
#define USBH_HcInterruptEnable_MIE_Msk   (0x1ul << USBH_HcInterruptEnable_MIE_Pos)         /*!< USBH_T::HcInterruptEnable: MIE Mask    */

#define USBH_HcInterruptDisable_SO_Pos   (0)                                               /*!< USBH_T::HcInterruptDisable: SO Position*/
#define USBH_HcInterruptDisable_SO_Msk   (0x1ul << USBH_HcInterruptDisable_SO_Pos)         /*!< USBH_T::HcInterruptDisable: SO Mask    */

#define USBH_HcInterruptDisable_WDH_Pos  (1)                                               /*!< USBH_T::HcInterruptDisable: WDH Position*/
#define USBH_HcInterruptDisable_WDH_Msk  (0x1ul << USBH_HcInterruptDisable_WDH_Pos)        /*!< USBH_T::HcInterruptDisable: WDH Mask   */

#define USBH_HcInterruptDisable_SF_Pos   (2)                                               /*!< USBH_T::HcInterruptDisable: SF Position*/
#define USBH_HcInterruptDisable_SF_Msk   (0x1ul << USBH_HcInterruptDisable_SF_Pos)         /*!< USBH_T::HcInterruptDisable: SF Mask    */

#define USBH_HcInterruptDisable_RD_Pos   (3)                                               /*!< USBH_T::HcInterruptDisable: RD Position*/
#define USBH_HcInterruptDisable_RD_Msk   (0x1ul << USBH_HcInterruptDisable_RD_Pos)         /*!< USBH_T::HcInterruptDisable: RD Mask    */

#define USBH_HcInterruptDisable_FNO_Pos  (5)                                               /*!< USBH_T::HcInterruptDisable: FNO Position*/
#define USBH_HcInterruptDisable_FNO_Msk  (0x1ul << USBH_HcInterruptDisable_FNO_Pos)        /*!< USBH_T::HcInterruptDisable: FNO Mask   */

#define USBH_HcInterruptDisable_RHSC_Pos (6)                                               /*!< USBH_T::HcInterruptDisable: RHSC Position*/
#define USBH_HcInterruptDisable_RHSC_Msk (0x1ul << USBH_HcInterruptDisable_RHSC_Pos)       /*!< USBH_T::HcInterruptDisable: RHSC Mask  */

#define USBH_HcInterruptDisable_MIE_Pos  (31)                                              /*!< USBH_T::HcInterruptDisable: MIE Position*/
#define USBH_HcInterruptDisable_MIE_Msk  (0x1ul << USBH_HcInterruptDisable_MIE_Pos)        /*!< USBH_T::HcInterruptDisable: MIE Mask   */

#define USBH_HcHCCA_HCCA_Pos             (8)                                               /*!< USBH_T::HcHCCA: HCCA Position          */
#define USBH_HcHCCA_HCCA_Msk             (0xfffffful << USBH_HcHCCA_HCCA_Pos)              /*!< USBH_T::HcHCCA: HCCA Mask              */

#define USBH_HcPeriodCurrentED_PCED_Pos  (4)                                               /*!< USBH_T::HcPeriodCurrentED: PCED Position*/
#define USBH_HcPeriodCurrentED_PCED_Msk  (0xffffffful << USBH_HcPeriodCurrentED_PCED_Pos)  /*!< USBH_T::HcPeriodCurrentED: PCED Mask   */

#define USBH_HcControlHeadED_CHED_Pos    (4)                                               /*!< USBH_T::HcControlHeadED: CHED Position */
#define USBH_HcControlHeadED_CHED_Msk    (0xffffffful << USBH_HcControlHeadED_CHED_Pos)    /*!< USBH_T::HcControlHeadED: CHED Mask     */

#define USBH_HcControlCurrentED_CCED_Pos (4)                                               /*!< USBH_T::HcControlCurrentED: CCED Position*/
#define USBH_HcControlCurrentED_CCED_Msk (0xffffffful << USBH_HcControlCurrentED_CCED_Pos) /*!< USBH_T::HcControlCurrentED: CCED Mask  */

#define USBH_HcBulkHeadED_BHED_Pos       (4)                                               /*!< USBH_T::HcBulkHeadED: BHED Position    */
#define USBH_HcBulkHeadED_BHED_Msk       (0xffffffful << USBH_HcBulkHeadED_BHED_Pos)       /*!< USBH_T::HcBulkHeadED: BHED Mask        */

#define USBH_HcBulkCurrentED_BCED_Pos    (4)                                               /*!< USBH_T::HcBulkCurrentED: BCED Position */
#define USBH_HcBulkCurrentED_BCED_Msk    (0xffffffful << USBH_HcBulkCurrentED_BCED_Pos)    /*!< USBH_T::HcBulkCurrentED: BCED Mask     */

#define USBH_HcDoneHead_DH_Pos           (4)                                               /*!< USBH_T::HcDoneHead: DH Position        */
#define USBH_HcDoneHead_DH_Msk           (0xffffffful << USBH_HcDoneHead_DH_Pos)           /*!< USBH_T::HcDoneHead: DH Mask            */

#define USBH_HcFmInterval_FI_Pos         (0)                                               /*!< USBH_T::HcFmInterval: FI Position      */
#define USBH_HcFmInterval_FI_Msk         (0x3ffful << USBH_HcFmInterval_FI_Pos)            /*!< USBH_T::HcFmInterval: FI Mask          */

#define USBH_HcFmInterval_FSMPS_Pos      (16)                                              /*!< USBH_T::HcFmInterval: FSMPS Position   */
#define USBH_HcFmInterval_FSMPS_Msk      (0x7ffful << USBH_HcFmInterval_FSMPS_Pos)         /*!< USBH_T::HcFmInterval: FSMPS Mask       */

#define USBH_HcFmInterval_FIT_Pos        (31)                                              /*!< USBH_T::HcFmInterval: FIT Position     */
#define USBH_HcFmInterval_FIT_Msk        (0x1ul << USBH_HcFmInterval_FIT_Pos)              /*!< USBH_T::HcFmInterval: FIT Mask         */

#define USBH_HcFmRemaining_FR_Pos        (0)                                               /*!< USBH_T::HcFmRemaining: FR Position     */
#define USBH_HcFmRemaining_FR_Msk        (0x3ffful << USBH_HcFmRemaining_FR_Pos)           /*!< USBH_T::HcFmRemaining: FR Mask         */

#define USBH_HcFmRemaining_FRT_Pos       (31)                                              /*!< USBH_T::HcFmRemaining: FRT Position    */
#define USBH_HcFmRemaining_FRT_Msk       (0x1ul << USBH_HcFmRemaining_FRT_Pos)             /*!< USBH_T::HcFmRemaining: FRT Mask        */

#define USBH_HcFmNumber_FN_Pos           (0)                                               /*!< USBH_T::HcFmNumber: FN Position        */
#define USBH_HcFmNumber_FN_Msk           (0xfffful << USBH_HcFmNumber_FN_Pos)              /*!< USBH_T::HcFmNumber: FN Mask            */

#define USBH_HcPeriodicStart_PS_Pos      (0)                                               /*!< USBH_T::HcPeriodicStart: PS Position   */
#define USBH_HcPeriodicStart_PS_Msk      (0x3ffful << USBH_HcPeriodicStart_PS_Pos)         /*!< USBH_T::HcPeriodicStart: PS Mask       */

#define USBH_HcLSThreshold_LST_Pos       (0)                                               /*!< USBH_T::HcLSThreshold: LST Position    */
#define USBH_HcLSThreshold_LST_Msk       (0xffful << USBH_HcLSThreshold_LST_Pos)           /*!< USBH_T::HcLSThreshold: LST Mask        */

#define USBH_HcRhDescriptorA_NDP_Pos     (0)                                               /*!< USBH_T::HcRhDescriptorA: NDP Position  */
#define USBH_HcRhDescriptorA_NDP_Msk     (0xfful << USBH_HcRhDescriptorA_NDP_Pos)          /*!< USBH_T::HcRhDescriptorA: NDP Mask      */

#define USBH_HcRhDescriptorA_PSM_Pos     (8)                                               /*!< USBH_T::HcRhDescriptorA: PSM Position  */
#define USBH_HcRhDescriptorA_PSM_Msk     (0x1ul << USBH_HcRhDescriptorA_PSM_Pos)           /*!< USBH_T::HcRhDescriptorA: PSM Mask      */

#define USBH_HcRhDescriptorA_OCPM_Pos    (11)                                              /*!< USBH_T::HcRhDescriptorA: OCPM Position */
#define USBH_HcRhDescriptorA_OCPM_Msk    (0x1ul << USBH_HcRhDescriptorA_OCPM_Pos)          /*!< USBH_T::HcRhDescriptorA: OCPM Mask     */

#define USBH_HcRhDescriptorA_NOCP_Pos    (12)                                              /*!< USBH_T::HcRhDescriptorA: NOCP Position */
#define USBH_HcRhDescriptorA_NOCP_Msk    (0x1ul << USBH_HcRhDescriptorA_NOCP_Pos)          /*!< USBH_T::HcRhDescriptorA: NOCP Mask     */

#define USBH_HcRhDescriptorB_PPCM_Pos    (16)                                              /*!< USBH_T::HcRhDescriptorB: PPCM Position */
#define USBH_HcRhDescriptorB_PPCM_Msk    (0xfffful << USBH_HcRhDescriptorB_PPCM_Pos)       /*!< USBH_T::HcRhDescriptorB: PPCM Mask     */

#define USBH_HcRhStatus_LPS_Pos          (0)                                               /*!< USBH_T::HcRhStatus: LPS Position       */
#define USBH_HcRhStatus_LPS_Msk          (0x1ul << USBH_HcRhStatus_LPS_Pos)                /*!< USBH_T::HcRhStatus: LPS Mask           */

#define USBH_HcRhStatus_OCI_Pos          (1)                                               /*!< USBH_T::HcRhStatus: OCI Position       */
#define USBH_HcRhStatus_OCI_Msk          (0x1ul << USBH_HcRhStatus_OCI_Pos)                /*!< USBH_T::HcRhStatus: OCI Mask           */

#define USBH_HcRhStatus_DRWE_Pos         (15)                                              /*!< USBH_T::HcRhStatus: DRWE Position      */
#define USBH_HcRhStatus_DRWE_Msk         (0x1ul << USBH_HcRhStatus_DRWE_Pos)               /*!< USBH_T::HcRhStatus: DRWE Mask          */

#define USBH_HcRhStatus_LPSC_Pos         (16)                                              /*!< USBH_T::HcRhStatus: LPSC Position      */
#define USBH_HcRhStatus_LPSC_Msk         (0x1ul << USBH_HcRhStatus_LPSC_Pos)               /*!< USBH_T::HcRhStatus: LPSC Mask          */

#define USBH_HcRhStatus_OCIC_Pos         (17)                                              /*!< USBH_T::HcRhStatus: OCIC Position      */
#define USBH_HcRhStatus_OCIC_Msk         (0x1ul << USBH_HcRhStatus_OCIC_Pos)               /*!< USBH_T::HcRhStatus: OCIC Mask          */

#define USBH_HcRhStatus_CRWE_Pos         (31)                                              /*!< USBH_T::HcRhStatus: CRWE Position      */
#define USBH_HcRhStatus_CRWE_Msk         (0x1ul << USBH_HcRhStatus_CRWE_Pos)               /*!< USBH_T::HcRhStatus: CRWE Mask          */

#define USBH_HcRhPortStatus_CCS_Pos      (0)                                               /*!< USBH_T::HcRhPortStatus1: CCS Position  */
#define USBH_HcRhPortStatus_CCS_Msk      (0x1ul << USBH_HcRhPortStatus_CCS_Pos)            /*!< USBH_T::HcRhPortStatus1: CCS Mask      */

#define USBH_HcRhPortStatus_PES_Pos      (1)                                               /*!< USBH_T::HcRhPortStatus1: PES Position  */
#define USBH_HcRhPortStatus_PES_Msk      (0x1ul << USBH_HcRhPortStatus_PES_Pos)            /*!< USBH_T::HcRhPortStatus1: PES Mask      */

#define USBH_HcRhPortStatus_PSS_Pos      (2)                                               /*!< USBH_T::HcRhPortStatus1: PSS Position  */
#define USBH_HcRhPortStatus_PSS_Msk      (0x1ul << USBH_HcRhPortStatus_PSS_Pos)            /*!< USBH_T::HcRhPortStatus1: PSS Mask      */

#define USBH_HcRhPortStatus_POCI_Pos     (3)                                               /*!< USBH_T::HcRhPortStatus1: POCI Position */
#define USBH_HcRhPortStatus_POCI_Msk     (0x1ul << USBH_HcRhPortStatus_POCI_Pos)           /*!< USBH_T::HcRhPortStatus1: POCI Mask     */

#define USBH_HcRhPortStatus_PRS_Pos      (4)                                               /*!< USBH_T::HcRhPortStatus1: PRS Position  */
#define USBH_HcRhPortStatus_PRS_Msk      (0x1ul << USBH_HcRhPortStatus_PRS_Pos)            /*!< USBH_T::HcRhPortStatus1: PRS Mask      */

#define USBH_HcRhPortStatus_PPS_Pos      (8)                                               /*!< USBH_T::HcRhPortStatus1: PPS Position  */
#define USBH_HcRhPortStatus_PPS_Msk      (0x1ul << USBH_HcRhPortStatus_PPS_Pos)            /*!< USBH_T::HcRhPortStatus1: PPS Mask      */

#define USBH_HcRhPortStatus_LSDA_Pos     (9)                                               /*!< USBH_T::HcRhPortStatus1: LSDA Position */
#define USBH_HcRhPortStatus_LSDA_Msk     (0x1ul << USBH_HcRhPortStatus_LSDA_Pos)           /*!< USBH_T::HcRhPortStatus1: LSDA Mask     */

#define USBH_HcRhPortStatus_CSC_Pos      (16)                                              /*!< USBH_T::HcRhPortStatus1: CSC Position  */
#define USBH_HcRhPortStatus_CSC_Msk      (0x1ul << USBH_HcRhPortStatus_CSC_Pos)            /*!< USBH_T::HcRhPortStatus1: CSC Mask      */

#define USBH_HcRhPortStatus_PESC_Pos     (17)                                              /*!< USBH_T::HcRhPortStatus1: PESC Position */
#define USBH_HcRhPortStatus_PESC_Msk     (0x1ul << USBH_HcRhPortStatus_PESC_Pos)           /*!< USBH_T::HcRhPortStatus1: PESC Mask     */

#define USBH_HcRhPortStatus_PSSC_Pos     (18)                                              /*!< USBH_T::HcRhPortStatus1: PSSC Position */
#define USBH_HcRhPortStatus_PSSC_Msk     (0x1ul << USBH_HcRhPortStatus_PSSC_Pos)           /*!< USBH_T::HcRhPortStatus1: PSSC Mask     */

#define USBH_HcRhPortStatus_OCIC_Pos     (19)                                              /*!< USBH_T::HcRhPortStatus1: OCIC Position */
#define USBH_HcRhPortStatus_OCIC_Msk     (0x1ul << USBH_HcRhPortStatus_OCIC_Pos)           /*!< USBH_T::HcRhPortStatus1: OCIC Mask     */

#define USBH_HcRhPortStatus_PRSC_Pos     (20)                                              /*!< USBH_T::HcRhPortStatus1: PRSC Position */
#define USBH_HcRhPortStatus_PRSC_Msk     (0x1ul << USBH_HcRhPortStatus_PRSC_Pos)           /*!< USBH_T::HcRhPortStatus1: PRSC Mask     */

#define USBH_HcPhyControl_STBYEN_Pos     (27)                                              /*!< USBH_T::HcPhyControl: STBYEN Position  */
#define USBH_HcPhyControl_STBYEN_Msk     (0x1ul << USBH_HcPhyControl_STBYEN_Pos)           /*!< USBH_T::HcPhyControl: STBYEN Mask      */

#define USBH_HcMiscControl_ABORT_Pos     (1)                                               /*!< USBH_T::HcMiscControl: ABORT Position  */
#define USBH_HcMiscControl_ABORT_Msk     (0x1ul << USBH_HcMiscControl_ABORT_Pos)           /*!< USBH_T::HcMiscControl: ABORT Mask      */

#define USBH_HcMiscControl_OCAL_Pos      (3)                                               /*!< USBH_T::HcMiscControl: OCAL Position   */
#define USBH_HcMiscControl_OCAL_Msk      (0x1ul << USBH_HcMiscControl_OCAL_Pos)            /*!< USBH_T::HcMiscControl: OCAL Mask       */

#define USBH_HcMiscControl_DPRT1_Pos     (16)                                              /*!< USBH_T::HcMiscControl: DPRT1 Position  */
#define USBH_HcMiscControl_DPRT1_Msk     (0x1ul << USBH_HcMiscControl_DPRT1_Pos)           /*!< USBH_T::HcMiscControl: DPRT1 Mask      */

/**@}*/ /* USBH_CONST */
/**@}*/ /* end of USBH register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __USBH_REG_H__ */
