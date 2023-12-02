/**************************************************************************//**
 * @file     otg_reg.h
 * @version  V1.00
 * @brief    OTG register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __OTG_REG_H__
#define __OTG_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup OTG USB On-The-Go Controller(OTG)
    Memory Mapped Structure for OTG Controller
@{ */

typedef struct
{


    /**
     * @var OTG_T::CTL
     * Offset: 0x00  OTG Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |VBUSDROP  |Drop VBUS Control
     * |        |          |If user application running on this OTG A-device wants to conserve power, set this bit to drop VBUS
     * |        |          |BUSREQ (OTG_CTL[1]) will be also cleared no matter A-device or B-device.
     * |        |          |0 = Not drop the VBUS.
     * |        |          |1 = Drop the VBUS.
     * |[1]     |BUSREQ    |OTG Bus Request
     * |        |          |If OTG A-device wants to do data transfers via USB bus, setting this bit will drive VBUS high to detect USB device connection
     * |        |          |If user won't use the bus any more, clearing this bit will drop VBUS to save power
     * |        |          |This bit will be cleared when A-device goes to A_wait_vfall state
     * |        |          |This bit will be also cleared if VBUSDROP (OTG_CTL[0]) bit is set or IDSTS (OTG_STATUS[1]) changed.
     * |        |          |If user of an OTG-B Device wants to request VBUS, setting this bit will run SRP protocol
     * |        |          |This bit will be cleared if SRP failure (OTG A-device does not provide VBUS after B-device issues ARP in specified interval, defined in OTG specification)
     * |        |          |This bit will be also cleared if VBUSDROP (OTG_CTL[0]) bit is set IDSTS (OTG_STATUS[1]) changed.
     * |        |          |0 = Not launch VBUS in OTG A-device or not request SRP in OTG B-device.
     * |        |          |1 = Launch VBUS in OTG A-device or request SRP in OTG B-device.
     * |[2]     |HNPREQEN  |OTG HNP Request Enable Bit
     * |        |          |When USB frame as A-device, set this bit when A-device allows to process HNP protocol -- A-device changes role from Host to Peripheral
     * |        |          |This bit will be cleared when OTG state changes from a_suspend to a_peripheral or goes back to a_idle state
     * |        |          |When USB frame as B-device, set this bit after the OTG A-device successfully sends a SetFeature (b_hnp_enable) command to the OTG B-device to start role change -- B-device changes role from Peripheral to Host
     * |        |          |This bit will be cleared when OTG state changes from b_peripheral to b_wait_acon or goes back to b_idle state.
     * |        |          |0 = HNP request Disabled.
     * |        |          |1 = HNP request Enabled (A-device can change role from Host to Peripheral or B-device can change role from Peripheral to Host).
     * |        |          |Note: Refer to OTG specification to get a_suspend, a_peripheral, a_idle and b_idle state.
     * |[4]     |OTGEN     |OTG Function Enable Bit
     * |        |          |User needs to set this bit to enable OTG function while USB frame configured as OTG device
     * |        |          |When USB frame not configured as OTG device, this bit is must be low.
     * |        |          |0= OTG function Disabled.
     * |        |          |1 = OTG function Enabled.
     * |[5]     |WKEN      |OTG ID Pin Wake-up Enable Bit
     * |        |          |0 = OTG ID pin status change wake-up function Disabled.
     * |        |          |1 = OTG ID pin status change wake-up function Enabled.
     * @var OTG_T::PHYCTL
     * Offset: 0x04  OTG PHY Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |OTGPHYEN  |OTG PHY Enable
     * |        |          |When USB frame is configured as OTG-device or ID-dependent, user needs to set this bit before using OTG function
     * |        |          |If device is not configured as OTG-device nor ID-dependent , this bit is "don't care".
     * |        |          |0 = OTG PHY Disabled.
     * |        |          |1 = OTG PHY Enabled.
     * |[1]     |IDDETEN   |ID Detection Enable Bit
     * |        |          |0 = Detect ID pin status Disabled.
     * |        |          |1 = Detect ID pin status Enabled.
     * |[4]     |VBENPOL   |Off-chip USB VBUS Power Switch Enable Polarity
     * |        |          |The OTG controller will enable off-chip USB VBUS power switch to provide VBUS power when need
     * |        |          |A USB_VBUS_EN pin is used to control the off-chip USB VBUS power switch.
     * |        |          |The polarity of enabling off-chip USB VBUS power switch (high active or low active) depends on the selected component
     * |        |          |Set this bit as following according to the polarity of off-chip USB VBUS power switch.
     * |        |          |0 = The off-chip USB VBUS power switch enable is active high.
     * |        |          |1 = The off-chip USB VBUS power switch enable is active low.
     * |[5]     |VBSTSPOL  |Off-chip USB VBUS Power Switch Status Polarity
     * |        |          |The polarity of off-chip USB VBUS power switch valid signal depends on the selected component
     * |        |          |A USB_VBUS_ST pin is used to monitor the valid signal of the off-chip USB VBUS power switch
     * |        |          |Set this bit as following according to the polarity of off-chip USB VBUS power switch.
     * |        |          |0 = The polarity of off-chip USB VBUS power switch valid status is high.
     * |        |          |1 = The polarity of off-chip USB VBUS power switch valid status is low.
     * @var OTG_T::INTEN
     * Offset: 0x08  OTG Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ROLECHGIEN|Role (Host or Peripheral) Changed Interrupt Enable Bit
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[1]     |VBEIEN    |VBUS Error Interrupt Enable Bit
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: VBUS error means going to a_vbus_err state. Please refer to A-device state diagram in OTG spec.
     * |[2]     |SRPFIEN   |SRP Fail Interrupt Enable Bit
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[3]     |HNPFIEN   |HNP Fail Interrupt Enable Bit
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[4]     |GOIDLEIEN |OTG Device Goes to IDLE State Interrupt Enable Bit
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |        |          |Note: Going to idle state means going to a_idle or b_idle state
     * |        |          |Please refer to A-device state diagram and B-device state diagram in OTG spec.
     * |[5]     |IDCHGIEN  |IDSTS Changed Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and IDSTS (OTG_STATUS[1]) status is changed from high to low or from low to high, a interrupt will be asserted.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[6]     |PDEVIEN   |Act As Peripheral Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and the device is changed as a peripheral, a interrupt will be asserted.
     * |        |          |0 = This device as a peripheral interrupt Disabled.
     * |        |          |1 = This device as a peripheral interrupt Enabled.
     * |[7]     |HOSTIEN   |Act As Host Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and the device is changed as a host, a interrupt will be asserted.
     * |        |          |0 = This device as a host interrupt Disabled.
     * |        |          |1 = This device as a host interrupt Enabled.
     * |[8]     |BVLDCHGIEN|B-device Session Valid Status Changed Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and BVLD (OTG_STATUS[3]) status is changed from high to low or from low to high, a interrupt will be asserted.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[9]     |AVLDCHGIEN|A-device Session Valid Status Changed Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and AVLD (OTG_STATUS[4]) status is changed from high to low or from low to high, a interrupt will be asserted.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[10]    |VBCHGIEN  |VBUSVLD Status Changed Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and VBUSVLD (OTG_STATUS[5]) status is changed from high to low or from low to high, a interrupt will be asserted.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[11]    |SECHGIEN  |SESSEND Status Changed Interrupt Enable Bit
     * |        |          |If this bit is set to 1 and SESSEND (OTG_STATUS[2]) status is changed from high to low or from low to high, a interrupt will be asserted.
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * |[13]    |SRPDETIEN |SRP Detected Interrupt Enable Bit
     * |        |          |0 = Interrupt Disabled.
     * |        |          |1 = Interrupt Enabled.
     * @var OTG_T::INTSTS
     * Offset: 0x0C  OTG Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ROLECHGIF |OTG Role Change Interrupt Status
     * |        |          |This flag is set when the role of an OTG device changed from a host to a peripheral, or changed from a peripheral to a host while USB_ID pin status does not change.
     * |        |          |0 = OTG device role not changed.
     * |        |          |1 = OTG device role changed.
     * |        |          |Note: Write 1 to clear this flag.
     * |[1]     |VBEIF     |VBUS Error Interrupt Status
     * |        |          |This bit will be set when voltage on VBUS cannot reach a minimum valid threshold 4.4V within a maximum time of 100ms after OTG A-device starting to drive VBUS high.
     * |        |          |0 = OTG A-device drives VBUS over threshold voltage before this interval expires.
     * |        |          |1 = OTG A-device cannot drive VBUS over threshold voltage before this interval expires.
     * |        |          |Note: Write 1 to clear this flag and recover from the VBUS error state.
     * |[2]     |SRPFIF    |SRP Fail Interrupt Status
     * |        |          |After initiating SRP, an OTG B-device will wait for the OTG A-device to drive VBUS high at least TB_SRP_FAIL minimum, defined in OTG specification
     * |        |          |This flag is set when the OTG B-device does not get VBUS high after this interval.
     * |        |          |0 = OTG B-device gets VBUS high before this interval.
     * |        |          |1 = OTG B-device does not get VBUS high before this interval.
     * |        |          |Note: Write 1 to clear this flag.
     * |[3]     |HNPFIF    |HNP Fail Interrupt Status
     * |        |          |When A-device has granted B-device to be host and USB bus is in SE0 (both USB_D+ and USB_D- low) state, this bit will be set when A-device does not connect after specified interval expires.
     * |        |          |0 = A-device connects to B-device before specified interval expires.
     * |        |          |1 = A-device does not connect to B-device before specified interval expires.
     * |        |          |Note: Write 1 to clear this flag.
     * |[4]     |GOIDLEIF  |OTG Device Goes to IDLE Interrupt Status
     * |        |          |Flag is set if the OTG device transfers from non-idle state to idle state
     * |        |          |The OTG device will be neither a host nor a peripheral.
     * |        |          |0 = OTG device does not go back to idle state (a_idle or b_idle).
     * |        |          |1 = OTG device goes back to idle state(a_idle or b_idle).
     * |        |          |Note 1: Going to idle state means going to a_idle or b_idle state. Please refer to OTG specification.
     * |        |          |Note 2: Write 1 to clear this flag.
     * |[5]     |IDCHGIF   |ID State Change Interrupt Status
     * |        |          |0 = IDSTS (OTG_STATUS[1]) not toggled.
     * |        |          |1 = IDSTS (OTG_STATUS[1]) from high to low or from low to high.
     * |        |          |Note: Write 1 to clear this flag.
     * |[6]     |PDEVIF    |Act As Peripheral Interrupt Status
     * |        |          |0= This device does not act as a peripheral.
     * |        |          |1 = This device acts as a peripheral.
     * |        |          |Note: Write 1 to clear this flag.
     * |[7]     |HOSTIF    |Act As Host Interrupt Status
     * |        |          |0= This device does not act as a host.
     * |        |          |1 = This device acts as a host.
     * |        |          |Note: Write 1 to clear this flag.
     * |[8]     |BVLDCHGIF |B-device Session Valid State Change Interrupt Status
     * |        |          |0 = BVLD (OTG_STATUS[3]) is not toggled.
     * |        |          |1 = BVLD (OTG_STATUS[3]) from high to low or low to high.
     * |        |          |Note: Write 1 to clear this status.
     * |[9]     |AVLDCHGIF |A-device Session Valid State Change Interrupt Status
     * |        |          |0 = AVLD (OTG_STATUS[4]) not toggled.
     * |        |          |1 = AVLD (OTG_STATUS[4]) from high to low or low to high.
     * |        |          |Note: Write 1 to clear this status.
     * |[10]    |VBCHGIF   |VBUSVLD State Change Interrupt Status
     * |        |          |0 = VBUSVLD (OTG_STATUS[5]) not toggled.
     * |        |          |1 = VBUSVLD (OTG_STATUS[5]) from high to low or from low to high.
     * |        |          |Note: Write 1 to clear this status.
     * |[11]    |SECHGIF   |SESSEND State Change Interrupt Status
     * |        |          |0 = SESSEND (OTG_STATUS[2]) not toggled.
     * |        |          |1 = SESSEND (OTG_STATUS[2]) from high to low or from low to high.
     * |        |          |Note: Write 1 to clear this flag.
     * |[13]    |SRPDETIF  |SRP Detected Interrupt Status
     * |        |          |0 = SRP not detected.
     * |        |          |1 = SRP detected.
     * |        |          |Note: Write 1 to clear this status.
     * @var OTG_T::STATUS
     * Offset: 0x10  OTG Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |OVERCUR   |over Current Condition
     * |        |          |The voltage on VBUS cannot reach a minimum VBUS valid threshold, 4.4V minimum, within a maximum time of 100ms after OTG A-device drives VBUS high.
     * |        |          |0 = OTG A-device drives VBUS successfully.
     * |        |          |1 = OTG A-device cannot drives VBUS high in this interval.
     * |[1]     |IDSTS     |USB_ID Pin State of Mini-b/Micro-plug
     * |        |          |0 = Mini-A/Micro-A plug is attached.
     * |        |          |1 = Mini-B/Micro-B plug is attached.
     * |[2]     |SESSEND   |Session End Status
     * |        |          |When VBUS voltage is lower than 0.4V, this bit will be set to 1
     * |        |          |Session end means no meaningful power on VBUS.
     * |        |          |0 = Session is not end.
     * |        |          |1 = Session is end.
     * |[3]     |BVLD      |B-device Session Valid Status
     * |        |          |0 = B-device session is not valid.
     * |        |          |1 = B-device session is valid.
     * |[4]     |AVLD      |A-device Session Valid Status
     * |        |          |0 = A-device session is not valid.
     * |        |          |1 = A-device session is valid.
     * |[5]     |VBUSVLD   |VBUS Valid Status
     * |        |          |When VBUS is larger than 4.7V, this bit will be set to 1.
     * |        |          |0 = VBUS is not valid.
     * |        |          |1 = VBUS is valid.
     * |[6]     |ASPERI    |As Peripheral Status
     * |        |          |When OTG as peripheral, this bit is set.
     * |        |          |0: OTG not as peripheral
     * |        |          |1: OTG as peripheral
     * |[7]     |ASHOST    |As Host Status
     * |        |          |When OTG as Host, this bit is set.
     * |        |          |0: OTG not as Host
     * |        |          |1: OTG as Host
     */
    __IO uint32_t CTL;                   /*!< [0x0000] OTG Control Register                                             */
    __IO uint32_t PHYCTL;                /*!< [0x0004] OTG PHY Control Register                                         */
    __IO uint32_t INTEN;                 /*!< [0x0008] OTG Interrupt Enable Register                                    */
    __IO uint32_t INTSTS;                /*!< [0x000c] OTG Interrupt Status Register                                    */
    __I  uint32_t STATUS;                /*!< [0x0010] OTG Status Register                                              */

} OTG_T;


/**
    @addtogroup OTG_CONST OTG Bit Field Definition
    Constant Definitions for OTG Controller
@{ */

#define OTG_CTL_VBUSDROP_Pos             (0)                                               /*!< OTG_T::CTL: VBUSDROP Position          */
#define OTG_CTL_VBUSDROP_Msk             (0x1ul << OTG_CTL_VBUSDROP_Pos)                   /*!< OTG_T::CTL: VBUSDROP Mask              */

#define OTG_CTL_BUSREQ_Pos               (1)                                               /*!< OTG_T::CTL: BUSREQ Position            */
#define OTG_CTL_BUSREQ_Msk               (0x1ul << OTG_CTL_BUSREQ_Pos)                     /*!< OTG_T::CTL: BUSREQ Mask                */

#define OTG_CTL_HNPREQEN_Pos             (2)                                               /*!< OTG_T::CTL: HNPREQEN Position          */
#define OTG_CTL_HNPREQEN_Msk             (0x1ul << OTG_CTL_HNPREQEN_Pos)                   /*!< OTG_T::CTL: HNPREQEN Mask              */

#define OTG_CTL_OTGEN_Pos                (4)                                               /*!< OTG_T::CTL: OTGEN Position             */
#define OTG_CTL_OTGEN_Msk                (0x1ul << OTG_CTL_OTGEN_Pos)                      /*!< OTG_T::CTL: OTGEN Mask                 */

#define OTG_CTL_WKEN_Pos                 (5)                                               /*!< OTG_T::CTL: WKEN Position              */
#define OTG_CTL_WKEN_Msk                 (0x1ul << OTG_CTL_WKEN_Pos)                       /*!< OTG_T::CTL: WKEN Mask                  */

#define OTG_PHYCTL_OTGPHYEN_Pos          (0)                                               /*!< OTG_T::PHYCTL: OTGPHYEN Position       */
#define OTG_PHYCTL_OTGPHYEN_Msk          (0x1ul << OTG_PHYCTL_OTGPHYEN_Pos)                /*!< OTG_T::PHYCTL: OTGPHYEN Mask           */

#define OTG_PHYCTL_IDDETEN_Pos           (1)                                               /*!< OTG_T::PHYCTL: IDDETEN Position        */
#define OTG_PHYCTL_IDDETEN_Msk           (0x1ul << OTG_PHYCTL_IDDETEN_Pos)                 /*!< OTG_T::PHYCTL: IDDETEN Mask            */

#define OTG_PHYCTL_VBENPOL_Pos           (4)                                               /*!< OTG_T::PHYCTL: VBENPOL Position        */
#define OTG_PHYCTL_VBENPOL_Msk           (0x1ul << OTG_PHYCTL_VBENPOL_Pos)                 /*!< OTG_T::PHYCTL: VBENPOL Mask            */

#define OTG_PHYCTL_VBSTSPOL_Pos          (5)                                               /*!< OTG_T::PHYCTL: VBSTSPOL Position       */
#define OTG_PHYCTL_VBSTSPOL_Msk          (0x1ul << OTG_PHYCTL_VBSTSPOL_Pos)                /*!< OTG_T::PHYCTL: VBSTSPOL Mask           */

#define OTG_INTEN_ROLECHGIEN_Pos         (0)                                               /*!< OTG_T::INTEN: ROLECHGIEN Position      */
#define OTG_INTEN_ROLECHGIEN_Msk         (0x1ul << OTG_INTEN_ROLECHGIEN_Pos)               /*!< OTG_T::INTEN: ROLECHGIEN Mask          */

#define OTG_INTEN_VBEIEN_Pos             (1)                                               /*!< OTG_T::INTEN: VBEIEN Position          */
#define OTG_INTEN_VBEIEN_Msk             (0x1ul << OTG_INTEN_VBEIEN_Pos)                   /*!< OTG_T::INTEN: VBEIEN Mask              */

#define OTG_INTEN_SRPFIEN_Pos            (2)                                               /*!< OTG_T::INTEN: SRPFIEN Position         */
#define OTG_INTEN_SRPFIEN_Msk            (0x1ul << OTG_INTEN_SRPFIEN_Pos)                  /*!< OTG_T::INTEN: SRPFIEN Mask             */

#define OTG_INTEN_HNPFIEN_Pos            (3)                                               /*!< OTG_T::INTEN: HNPFIEN Position         */
#define OTG_INTEN_HNPFIEN_Msk            (0x1ul << OTG_INTEN_HNPFIEN_Pos)                  /*!< OTG_T::INTEN: HNPFIEN Mask             */

#define OTG_INTEN_GOIDLEIEN_Pos          (4)                                               /*!< OTG_T::INTEN: GOIDLEIEN Position       */
#define OTG_INTEN_GOIDLEIEN_Msk          (0x1ul << OTG_INTEN_GOIDLEIEN_Pos)                /*!< OTG_T::INTEN: GOIDLEIEN Mask           */

#define OTG_INTEN_IDCHGIEN_Pos           (5)                                               /*!< OTG_T::INTEN: IDCHGIEN Position        */
#define OTG_INTEN_IDCHGIEN_Msk           (0x1ul << OTG_INTEN_IDCHGIEN_Pos)                 /*!< OTG_T::INTEN: IDCHGIEN Mask            */

#define OTG_INTEN_PDEVIEN_Pos            (6)                                               /*!< OTG_T::INTEN: PDEVIEN Position         */
#define OTG_INTEN_PDEVIEN_Msk            (0x1ul << OTG_INTEN_PDEVIEN_Pos)                  /*!< OTG_T::INTEN: PDEVIEN Mask             */

#define OTG_INTEN_HOSTIEN_Pos            (7)                                               /*!< OTG_T::INTEN: HOSTIEN Position         */
#define OTG_INTEN_HOSTIEN_Msk            (0x1ul << OTG_INTEN_HOSTIEN_Pos)                  /*!< OTG_T::INTEN: HOSTIEN Mask             */

#define OTG_INTEN_BVLDCHGIEN_Pos         (8)                                               /*!< OTG_T::INTEN: BVLDCHGIEN Position      */
#define OTG_INTEN_BVLDCHGIEN_Msk         (0x1ul << OTG_INTEN_BVLDCHGIEN_Pos)               /*!< OTG_T::INTEN: BVLDCHGIEN Mask          */

#define OTG_INTEN_AVLDCHGIEN_Pos         (9)                                               /*!< OTG_T::INTEN: AVLDCHGIEN Position      */
#define OTG_INTEN_AVLDCHGIEN_Msk         (0x1ul << OTG_INTEN_AVLDCHGIEN_Pos)               /*!< OTG_T::INTEN: AVLDCHGIEN Mask          */

#define OTG_INTEN_VBCHGIEN_Pos           (10)                                              /*!< OTG_T::INTEN: VBCHGIEN Position        */
#define OTG_INTEN_VBCHGIEN_Msk           (0x1ul << OTG_INTEN_VBCHGIEN_Pos)                 /*!< OTG_T::INTEN: VBCHGIEN Mask            */

#define OTG_INTEN_SECHGIEN_Pos           (11)                                              /*!< OTG_T::INTEN: SECHGIEN Position        */
#define OTG_INTEN_SECHGIEN_Msk           (0x1ul << OTG_INTEN_SECHGIEN_Pos)                 /*!< OTG_T::INTEN: SECHGIEN Mask            */

#define OTG_INTEN_SRPDETIEN_Pos          (13)                                              /*!< OTG_T::INTEN: SRPDETIEN Position       */
#define OTG_INTEN_SRPDETIEN_Msk          (0x1ul << OTG_INTEN_SRPDETIEN_Pos)                /*!< OTG_T::INTEN: SRPDETIEN Mask           */

#define OTG_INTSTS_ROLECHGIF_Pos         (0)                                               /*!< OTG_T::INTSTS: ROLECHGIF Position      */
#define OTG_INTSTS_ROLECHGIF_Msk         (0x1ul << OTG_INTSTS_ROLECHGIF_Pos)               /*!< OTG_T::INTSTS: ROLECHGIF Mask          */

#define OTG_INTSTS_VBEIF_Pos             (1)                                               /*!< OTG_T::INTSTS: VBEIF Position          */
#define OTG_INTSTS_VBEIF_Msk             (0x1ul << OTG_INTSTS_VBEIF_Pos)                   /*!< OTG_T::INTSTS: VBEIF Mask              */

#define OTG_INTSTS_SRPFIF_Pos            (2)                                               /*!< OTG_T::INTSTS: SRPFIF Position         */
#define OTG_INTSTS_SRPFIF_Msk            (0x1ul << OTG_INTSTS_SRPFIF_Pos)                  /*!< OTG_T::INTSTS: SRPFIF Mask             */

#define OTG_INTSTS_HNPFIF_Pos            (3)                                               /*!< OTG_T::INTSTS: HNPFIF Position         */
#define OTG_INTSTS_HNPFIF_Msk            (0x1ul << OTG_INTSTS_HNPFIF_Pos)                  /*!< OTG_T::INTSTS: HNPFIF Mask             */

#define OTG_INTSTS_GOIDLEIF_Pos          (4)                                               /*!< OTG_T::INTSTS: GOIDLEIF Position       */
#define OTG_INTSTS_GOIDLEIF_Msk          (0x1ul << OTG_INTSTS_GOIDLEIF_Pos)                /*!< OTG_T::INTSTS: GOIDLEIF Mask           */

#define OTG_INTSTS_IDCHGIF_Pos           (5)                                               /*!< OTG_T::INTSTS: IDCHGIF Position        */
#define OTG_INTSTS_IDCHGIF_Msk           (0x1ul << OTG_INTSTS_IDCHGIF_Pos)                 /*!< OTG_T::INTSTS: IDCHGIF Mask            */

#define OTG_INTSTS_PDEVIF_Pos            (6)                                               /*!< OTG_T::INTSTS: PDEVIF Position         */
#define OTG_INTSTS_PDEVIF_Msk            (0x1ul << OTG_INTSTS_PDEVIF_Pos)                  /*!< OTG_T::INTSTS: PDEVIF Mask             */

#define OTG_INTSTS_HOSTIF_Pos            (7)                                               /*!< OTG_T::INTSTS: HOSTIF Position         */
#define OTG_INTSTS_HOSTIF_Msk            (0x1ul << OTG_INTSTS_HOSTIF_Pos)                  /*!< OTG_T::INTSTS: HOSTIF Mask             */

#define OTG_INTSTS_BVLDCHGIF_Pos         (8)                                               /*!< OTG_T::INTSTS: BVLDCHGIF Position      */
#define OTG_INTSTS_BVLDCHGIF_Msk         (0x1ul << OTG_INTSTS_BVLDCHGIF_Pos)               /*!< OTG_T::INTSTS: BVLDCHGIF Mask          */

#define OTG_INTSTS_AVLDCHGIF_Pos         (9)                                               /*!< OTG_T::INTSTS: AVLDCHGIF Position      */
#define OTG_INTSTS_AVLDCHGIF_Msk         (0x1ul << OTG_INTSTS_AVLDCHGIF_Pos)               /*!< OTG_T::INTSTS: AVLDCHGIF Mask          */

#define OTG_INTSTS_VBCHGIF_Pos           (10)                                              /*!< OTG_T::INTSTS: VBCHGIF Position        */
#define OTG_INTSTS_VBCHGIF_Msk           (0x1ul << OTG_INTSTS_VBCHGIF_Pos)                 /*!< OTG_T::INTSTS: VBCHGIF Mask            */

#define OTG_INTSTS_SECHGIF_Pos           (11)                                              /*!< OTG_T::INTSTS: SECHGIF Position        */
#define OTG_INTSTS_SECHGIF_Msk           (0x1ul << OTG_INTSTS_SECHGIF_Pos)                 /*!< OTG_T::INTSTS: SECHGIF Mask            */

#define OTG_INTSTS_SRPDETIF_Pos          (13)                                              /*!< OTG_T::INTSTS: SRPDETIF Position       */
#define OTG_INTSTS_SRPDETIF_Msk          (0x1ul << OTG_INTSTS_SRPDETIF_Pos)                /*!< OTG_T::INTSTS: SRPDETIF Mask           */

#define OTG_STATUS_OVERCUR_Pos           (0)                                               /*!< OTG_T::STATUS: OVERCUR Position        */
#define OTG_STATUS_OVERCUR_Msk           (0x1ul << OTG_STATUS_OVERCUR_Pos)                 /*!< OTG_T::STATUS: OVERCUR Mask            */

#define OTG_STATUS_IDSTS_Pos             (1)                                               /*!< OTG_T::STATUS: IDSTS Position          */
#define OTG_STATUS_IDSTS_Msk             (0x1ul << OTG_STATUS_IDSTS_Pos)                   /*!< OTG_T::STATUS: IDSTS Mask              */

#define OTG_STATUS_SESSEND_Pos           (2)                                               /*!< OTG_T::STATUS: SESSEND Position        */
#define OTG_STATUS_SESSEND_Msk           (0x1ul << OTG_STATUS_SESSEND_Pos)                 /*!< OTG_T::STATUS: SESSEND Mask            */

#define OTG_STATUS_BVLD_Pos              (3)                                               /*!< OTG_T::STATUS: BVLD Position           */
#define OTG_STATUS_BVLD_Msk              (0x1ul << OTG_STATUS_BVLD_Pos)                    /*!< OTG_T::STATUS: BVLD Mask               */

#define OTG_STATUS_AVLD_Pos              (4)                                               /*!< OTG_T::STATUS: AVLD Position           */
#define OTG_STATUS_AVLD_Msk              (0x1ul << OTG_STATUS_AVLD_Pos)                    /*!< OTG_T::STATUS: AVLD Mask               */

#define OTG_STATUS_VBUSVLD_Pos           (5)                                               /*!< OTG_T::STATUS: VBUSVLD Position        */
#define OTG_STATUS_VBUSVLD_Msk           (0x1ul << OTG_STATUS_VBUSVLD_Pos)                 /*!< OTG_T::STATUS: VBUSVLD Mask            */

#define OTG_STATUS_ASPERI_Pos            (6)                                               /*!< OTG_T::STATUS: ASPERI Position         */
#define OTG_STATUS_ASPERI_Msk            (0x1ul << OTG_STATUS_ASPERI_Pos)                  /*!< OTG_T::STATUS: ASPERI Mask             */

#define OTG_STATUS_ASHOST_Pos            (7)                                               /*!< OTG_T::STATUS: ASHOST Position         */
#define OTG_STATUS_ASHOST_Msk            (0x1ul << OTG_STATUS_ASHOST_Pos)                  /*!< OTG_T::STATUS: ASHOST Mask             */

/**@}*/ /* OTG_CONST */
/**@}*/ /* end of OTG register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __OTG_REG_H__ */
