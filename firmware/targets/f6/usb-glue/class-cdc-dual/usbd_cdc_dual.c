/**
  ******************************************************************************
  * @file    usbd_cdc.c
  * @author  MCD Application Team
  * @brief   This file provides the high layer firmware functions to manage the
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *
  *  @verbatim
  *
  *          ===================================================================
  *                                CDC Class Driver Description
  *          ===================================================================
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class
  *
  *           These aspects may be enriched or modified for a specific user application.
  *
  *            This driver doesn't implement the following aspects of the specification
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_dual.h"
#include "usbd_ctlreq.h"
#include <furi/check.h>

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_CDC
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_CDC_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_CDC_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_CDC_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CDC_Private_FunctionPrototypes
  * @{
  */

static uint8_t USBD_CDC_Init(USBD_HandleTypeDef* pdev, uint8_t cfgidx);
static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef* pdev, uint8_t cfgidx);
static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);
static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef* pdev, uint8_t epnum);
static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef* pdev, uint8_t epnum);
static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef* pdev);

static uint8_t* USBD_CDC_GetFSCfgDesc(uint16_t* length);
static uint8_t* USBD_CDC_GetHSCfgDesc(uint16_t* length);
static uint8_t* USBD_CDC_GetOtherSpeedCfgDesc(uint16_t* length);
static uint8_t* USBD_CDC_GetOtherSpeedCfgDesc(uint16_t* length);
uint8_t* USBD_CDC_GetDeviceQualifierDescriptor(uint16_t* length);

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CDC_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
    {
        USB_LEN_DEV_QUALIFIER_DESC,
        USB_DESC_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x40,
        0x01,
        0x00,
};

/**
  * @}
  */

/** @defgroup USBD_CDC_Private_Variables
  * @{
  */

/* CDC interface class callbacks structure */
USBD_ClassTypeDef USBD_CDC = {
    USBD_CDC_Init,
    USBD_CDC_DeInit,
    USBD_CDC_Setup,
    NULL, /* EP0_TxSent, */
    USBD_CDC_EP0_RxReady,
    USBD_CDC_DataIn,
    USBD_CDC_DataOut,
    NULL,
    NULL,
    NULL,
    USBD_CDC_GetHSCfgDesc,
    USBD_CDC_GetFSCfgDesc,
    USBD_CDC_GetOtherSpeedCfgDesc,
    USBD_CDC_GetDeviceQualifierDescriptor,
};

/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CDC_CfgHSDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END = {
    /* Configuration Descriptor */
    0x09, /* bLength: Configuration Descriptor size */
    USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
    USB_CDC_CONFIG_DESC_SIZ, /* wTotalLength:no of returned bytes */
    0x00,
    0x02 * 2, /* bNumInterfaces: 4 interface */
    0x01, /* bConfigurationValue: Configuration value */
    0x00, /* iConfiguration: Index of string descriptor describing the configuration */
#if(USBD_SELF_POWERED == 1U)
    0xC0, /* bmAttributes: Bus Powered according to user configuration */
#else
    0x80, /* bmAttributes: Bus Powered according to user configuration */
#endif
    USBD_MAX_POWER, /* MaxPower 100 mA */

    /*---------------------------------------------------------------------------*/

    /* IAD Configuration for first CDC if */
    0x08, /* bLength: Configuration Descriptor size */
    0x0B, /* bDescriptorType: IAD */
    0x00, /* bFirstInterface, starting of interface */
    0x02, /* bInterfaceCount, interfaces under this IAD class */
    0x02, /* bFunctionClass: CDC */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x02, /* iFunction */

    /*---------------------------------------------------------------------------*/

    /* Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */

    /* Interface descriptor type */
    0x00, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01, /* bDataInterface: 1 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00, /* bMasterInterface: Communication class interface */
    0x01, /* bSlaveInterface0: Data Class Interface */

    /* Endpoint 2 Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_CMD_EP, /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SIZE),
    CDC_HS_BINTERVAL, /* bInterval: */
    /*---------------------------------------------------------------------------*/

    /* Data class interface descriptor */
    0x09, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
    0x01, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_OUT_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_IN_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /*---------------------------------------------------------------------------*/

    /* IAD Configuration for second CDC if */
    0x08, /* bLength: Interface Descriptor size */
    0x0B, /* bDescriptorType: IAD */
    0x02, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass: CDC */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x02, /* iFunction */

    /*Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */

    /* Interface descriptor type */
    0x00 + 2, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01 + 2, /* bDataInterface: 1+2 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00 + 2, /* bMasterInterface: Communication class interface */
    0x01 + 2, /* bSlaveInterface0: Data Class Interface */

    /* Endpoint 2 Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_CMD_EP, /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SIZE),
    CDC_HS_BINTERVAL, /* bInterval: */

    /*---------------------------------------------------------------------------*/

    /* Data class interface descriptor */
    0x09, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
    0x01 + 2, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_OUT_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_IN_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
    0x00 /* bInterval: ignore for Bulk transfer */
};

/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CDC_CfgFSDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END = {
    /* Configuration Descriptor */
    0x09, /* bLength: Configuration Descriptor size */
    USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
    USB_CDC_CONFIG_DESC_SIZ, /* wTotalLength:no of returned bytes */
    0x00,
    0x02 * 2, /* bNumInterfaces: 4 interface */
    0x01, /* bConfigurationValue: Configuration value */
    0x00, /* iConfiguration: Index of string descriptor describing the configuration */
#if(USBD_SELF_POWERED == 1U)
    0xC0, /* bmAttributes: Bus Powered according to user configuration */
#else
    0x80, /* bmAttributes: Bus Powered according to user configuration */
#endif
    USBD_MAX_POWER, /* MaxPower 100 mA */

    /*---------------------------------------------------------------------------*/

    /* IAD Configuration for first CDC if */
    0x08, /* bLength: Configuration Descriptor size */
    0x0B, /* bDescriptorType: IAD */
    0x00, /* bFirstInterface, starting of interface */
    0x02, /* bInterfaceCount, interfaces under this IAD class */
    0x02, /* bFunctionClass: CDC */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x02, /* iFunction */

    /*---------------------------------------------------------------------------*/

    /* Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */

    /* Interface descriptor type */
    0x00, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01, /* bDataInterface: 1 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00, /* bMasterInterface: Communication class interface */
    0x01, /* bSlaveInterface0: Data Class Interface */

    /* Endpoint 2 Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_CMD_EP, /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SIZE),
    CDC_FS_BINTERVAL, /* bInterval: */
    /*---------------------------------------------------------------------------*/

    /* Data class interface descriptor */
    0x09, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
    0x01, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_OUT_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_IN_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /*---------------------------------------------------------------------------*/

    /* IAD Configuration for second CDC if */

    0x08, /* bLength: Interface Descriptor size */
    0x0B, /* bDescriptorType: IAD */
    0x02, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass: CDC */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x02, /* iFunction */

    /*Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */

    /* Interface descriptor type */
    0x00 + 2, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01 + 2, /* bDataInterface: 1+2 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00 + 2, /* bMasterInterface: Communication class interface */
    0x01 + 2, /* bSlaveInterface0: Data Class Interface */

    /* Endpoint 2 Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_CMD_EP, /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SIZE),
    CDC_FS_BINTERVAL, /* bInterval: */

    /*---------------------------------------------------------------------------*/

    /* Data class interface descriptor */
    0x09, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
    0x01 + 2, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_OUT_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_IN_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00 /* bInterval: ignore for Bulk transfer */
};

__ALIGN_BEGIN static uint8_t USBD_CDC_OtherSpeedCfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END = {
    /* Configuration Descriptor */
    0x09, /* bLength: Configuration Descriptor size */
    USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION, /* bDescriptorType: Configuration */
    USB_CDC_CONFIG_DESC_SIZ, /* wTotalLength:no of returned bytes */
    0x00,
    0x02 * 2, /* bNumInterfaces: 4 interface */
    0x01, /* bConfigurationValue: Configuration value */
    0x04, /* iConfiguration: Index of string descriptor describing the configuration */
#if(USBD_SELF_POWERED == 1U)
    0xC0, /* bmAttributes: Bus Powered according to user configuration */
#else
    0x80, /* bmAttributes: Bus Powered according to user configuration */
#endif
    USBD_MAX_POWER, /* MaxPower 100 mA */

    /*---------------------------------------------------------------------------*/

    /* IAD Configuration for first CDC if */
    0x08, /* bLength: Configuration Descriptor size */
    0x0B, /* bDescriptorType: IAD */
    0x00, /* bFirstInterface, starting of interface */
    0x02, /* bInterfaceCount, interfaces under this IAD class */
    0x02, /* bFunctionClass: CDC */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x02, /* iFunction */

    /*---------------------------------------------------------------------------*/

    /* Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */

    /* Interface descriptor type */
    0x00, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01, /* bDataInterface: 1 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00, /* bMasterInterface: Communication class interface */
    0x01, /* bSlaveInterface0: Data Class Interface */

    /* Endpoint 2 Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_CMD_EP, /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SIZE),
    CDC_FS_BINTERVAL, /* bInterval: */
    /*---------------------------------------------------------------------------*/

    /* Data class interface descriptor */
    0x09, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
    0x01, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_OUT_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_1_IN_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /*---------------------------------------------------------------------------*/

    /* IAD Configuration for second CDC if */

    0x08, /* bLength: Interface Descriptor size */
    0x0B, /* bDescriptorType: IAD */
    0x02, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass: CDC */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x02, /* iFunction */

    /*Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */

    /* Interface descriptor type */
    0x00 + 2, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01 + 2, /* bDataInterface: 1+2 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00 + 2, /* bMasterInterface: Communication class interface */
    0x01 + 2, /* bSlaveInterface0: Data Class Interface */

    /* Endpoint 2 Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_CMD_EP, /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SIZE),
    CDC_FS_BINTERVAL, /* bInterval: */

    /*---------------------------------------------------------------------------*/

    /* Data class interface descriptor */
    0x09, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
    0x01 + 2, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_OUT_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07, /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    CDC_2_IN_EP, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00 /* bInterval: ignore for Bulk transfer */
};

/**
  * @}
  */

/** @defgroup USBD_CDC_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CDC_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CDC_Init(USBD_HandleTypeDef* pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);
    USBD_CDC_HandleTypeDef* hcdc;
    USBD_StatusTypeDef status;

    hcdc = USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));

    if(hcdc == NULL) {
        pdev->pClassData = NULL;
        return (uint8_t)USBD_EMEM;
    }

    pdev->pClassData = (void*)hcdc;

    if(pdev->dev_speed == USBD_SPEED_HIGH) {
        /* Open EP IN 1 */
        (void)USBD_LL_OpenEP(pdev, CDC_1_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_IN_PACKET_SIZE);

        pdev->ep_in[CDC_1_IN_EP & 0xFU].is_used = 1U;

        /* Open EP OUT 1 */
        (void)USBD_LL_OpenEP(pdev, CDC_1_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_OUT_PACKET_SIZE);

        pdev->ep_out[CDC_1_OUT_EP & 0xFU].is_used = 1U;

        /* Open EP IN 2 */
        (void)USBD_LL_OpenEP(pdev, CDC_2_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_IN_PACKET_SIZE);

        pdev->ep_in[CDC_2_IN_EP & 0xFU].is_used = 1U;

        /* Open EP OUT 2 */
        (void)USBD_LL_OpenEP(pdev, CDC_2_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_OUT_PACKET_SIZE);

        pdev->ep_out[CDC_2_OUT_EP & 0xFU].is_used = 1U;

        /* Set bInterval for CDC CMD Endpoints */
        pdev->ep_in[CDC_1_CMD_EP & 0xFU].bInterval = CDC_HS_BINTERVAL;
        pdev->ep_in[CDC_2_CMD_EP & 0xFU].bInterval = CDC_HS_BINTERVAL;
    } else {
        /* Open EP IN 1 */
        status = USBD_LL_OpenEP(pdev, CDC_1_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);
        pdev->ep_in[CDC_1_IN_EP & 0xFU].is_used = 1U;
        if(status != USBD_OK) furi_check(0);

        /* Open EP OUT 1 */
        status = USBD_LL_OpenEP(pdev, CDC_1_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_OUT_PACKET_SIZE);
        pdev->ep_out[CDC_1_OUT_EP & 0xFU].is_used = 1U;
        if(status != USBD_OK) furi_check(0);

        /* Open EP IN 2 */
        status = USBD_LL_OpenEP(pdev, CDC_2_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);
        pdev->ep_in[CDC_2_IN_EP & 0xFU].is_used = 1U;
        if(status != USBD_OK) furi_check(0);

        /* Open EP OUT 2*/
        status = USBD_LL_OpenEP(pdev, CDC_2_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_OUT_PACKET_SIZE);
        pdev->ep_out[CDC_2_OUT_EP & 0xFU].is_used = 1U;
        if(status != USBD_OK) furi_check(0);

        /* Set bInterval for CMD Endpoints */
        pdev->ep_in[CDC_1_CMD_EP & 0xFU].bInterval = CDC_FS_BINTERVAL;
        pdev->ep_in[CDC_2_CMD_EP & 0xFU].bInterval = CDC_FS_BINTERVAL;
    }

    /* Open Command IN EP 1 */
    status = USBD_LL_OpenEP(pdev, CDC_1_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
    pdev->ep_in[CDC_1_CMD_EP & 0xFU].is_used = 1U;
    if(status != USBD_OK) furi_check(0);

    /* Open Command IN EP 2 */
    status = USBD_LL_OpenEP(pdev, CDC_2_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
    pdev->ep_in[CDC_2_CMD_EP & 0xFU].is_used = 1U;
    if(status != USBD_OK) furi_check(0);

    /* Init  physical Interface components */
    ((USBD_CDC_ItfTypeDef*)pdev->pUserData)->Init();

    /* Init Xfer states */
    hcdc->TxState = 0U;
    hcdc->RxState = 0U;

    if(pdev->dev_speed == USBD_SPEED_HIGH) {
        /* Prepare Out endpoint to receive next packet 1 */
        (void)USBD_LL_PrepareReceive(
            pdev, CDC_1_OUT_EP, hcdc->RxBuffer, CDC_DATA_HS_OUT_PACKET_SIZE);

        /* Prepare Out endpoint to receive next packet 2 */
        (void)USBD_LL_PrepareReceive(
            pdev, CDC_2_OUT_EP, hcdc->RxBuffer, CDC_DATA_HS_OUT_PACKET_SIZE);
    } else {
        /* Prepare Out endpoint to receive next packet 1 */
        (void)USBD_LL_PrepareReceive(
            pdev, CDC_1_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
        /* Prepare Out endpoint to receive next packet 2 */
        (void)USBD_LL_PrepareReceive(
            pdev, CDC_2_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
    }

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_DeInit
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef* pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    /* Close EP IN 1 */
    (void)USBD_LL_CloseEP(pdev, CDC_1_IN_EP);
    pdev->ep_in[CDC_1_IN_EP & 0xFU].is_used = 0U;

    /* Close EP OUT 1 */
    (void)USBD_LL_CloseEP(pdev, CDC_1_OUT_EP);
    pdev->ep_out[CDC_1_OUT_EP & 0xFU].is_used = 0U;

    /* Close Command IN EP 1 */
    (void)USBD_LL_CloseEP(pdev, CDC_1_CMD_EP);
    pdev->ep_in[CDC_1_CMD_EP & 0xFU].is_used = 0U;
    pdev->ep_in[CDC_1_CMD_EP & 0xFU].bInterval = 0U;

    /* Close EP IN 2 */
    (void)USBD_LL_CloseEP(pdev, CDC_2_IN_EP);
    pdev->ep_in[CDC_2_IN_EP & 0xFU].is_used = 0U;

    /* Close EP OUT 2 */
    (void)USBD_LL_CloseEP(pdev, CDC_2_OUT_EP);
    pdev->ep_out[CDC_2_OUT_EP & 0xFU].is_used = 0U;

    /* Close Command IN EP 2 */
    (void)USBD_LL_CloseEP(pdev, CDC_2_CMD_EP);
    pdev->ep_in[CDC_2_CMD_EP & 0xFU].is_used = 0U;
    pdev->ep_in[CDC_2_CMD_EP & 0xFU].bInterval = 0U;

    /* DeInit  physical Interface components */
    if(pdev->pClassData != NULL) {
        ((USBD_CDC_ItfTypeDef*)pdev->pUserData)->DeInit();
        (void)USBD_free(pdev->pClassData);
        pdev->pClassData = NULL;
    }

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;
    uint16_t len;
    uint8_t ifalt = 0U;
    uint16_t status_info = 0U;
    USBD_StatusTypeDef ret = USBD_OK;
    
    if(hcdc == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    switch(req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
        if(req->wLength != 0U) {
            if((req->bmRequest & 0x80U) != 0U) {
                ((USBD_CDC_ItfTypeDef*)pdev->pUserData)
                    ->Control(req->bRequest, (uint8_t*)hcdc->data, req->wLength, req->wIndex);

                len = MIN(CDC_REQ_MAX_DATA_SIZE, req->wLength);
                (void)USBD_CtlSendData(pdev, (uint8_t*)hcdc->data, len);
            } else {
                hcdc->CmdOpCode = req->bRequest;
                hcdc->CmdLength = (uint8_t)req->wLength;

                (void)USBD_CtlPrepareRx(pdev, (uint8_t*)hcdc->data, req->wLength);
            }
        } else {
            ((USBD_CDC_ItfTypeDef*)pdev->pUserData)
                ->Control(req->bRequest, (uint8_t*)req, 0U, req->wIndex);
        }
        break;

    case USB_REQ_TYPE_STANDARD:
        switch(req->bRequest) {
        case USB_REQ_GET_STATUS:
            if(pdev->dev_state == USBD_STATE_CONFIGURED) {
                (void)USBD_CtlSendData(pdev, (uint8_t*)&status_info, 2U);
            } else {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
            }
            break;

        case USB_REQ_GET_INTERFACE:
            if(pdev->dev_state == USBD_STATE_CONFIGURED) {
                (void)USBD_CtlSendData(pdev, &ifalt, 1U);
            } else {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
            }
            break;

        case USB_REQ_SET_INTERFACE:
            if(pdev->dev_state != USBD_STATE_CONFIGURED) {
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
            }
            break;

        case USB_REQ_CLEAR_FEATURE:
            break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
        }
        break;

    default:
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
        break;
    }

    return (uint8_t)ret;
}

/**
  * @brief  USBD_CDC_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef* pdev, uint8_t epnum) {
    USBD_CDC_HandleTypeDef* hcdc;
    PCD_HandleTypeDef* hpcd = pdev->pData;

    if(pdev->pClassData == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

    if((pdev->ep_in[epnum].total_length > 0U) &&
       ((pdev->ep_in[epnum].total_length % hpcd->IN_ep[epnum].maxpacket) == 0U)) {
        /* Update the packet total length */
        pdev->ep_in[epnum].total_length = 0U;

        /* Send ZLP */
        (void)USBD_LL_Transmit(pdev, epnum, NULL, 0U);
    } else {
        hcdc->TxState = 0U;

        if(((USBD_CDC_ItfTypeDef*)pdev->pUserData)->TransmitCplt != NULL) {
            ((USBD_CDC_ItfTypeDef*)pdev->pUserData)
                ->TransmitCplt(
                    hcdc->TxBuffer, &hcdc->TxLength, epnum, (epnum == CDC_1_OUT_EP) ? 0 : 2);
        }
    }

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef* pdev, uint8_t epnum) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

    if(pdev->pClassData == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    /* Get the received data length */
    hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

    /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */

    ((USBD_CDC_ItfTypeDef*)pdev->pUserData)
        ->Receive(hcdc->RxBuffer, &hcdc->RxLength, (epnum == CDC_1_OUT_EP) ? 0 : 2);

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef* pdev) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

    if(hcdc == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    if((pdev->pUserData != NULL) && (hcdc->CmdOpCode != 0xFFU)) {
        ((USBD_CDC_ItfTypeDef*)pdev->pUserData)
            ->Control(
                hcdc->CmdOpCode,
                (uint8_t*)hcdc->data,
                (uint16_t)hcdc->CmdLength,
                pdev->request.wIndex);
        hcdc->CmdOpCode = 0xFFU;
    }

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t* USBD_CDC_GetFSCfgDesc(uint16_t* length) {
    *length = (uint16_t)sizeof(USBD_CDC_CfgFSDesc);

    return USBD_CDC_CfgFSDesc;
}

/**
  * @brief  USBD_CDC_GetHSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t* USBD_CDC_GetHSCfgDesc(uint16_t* length) {
    *length = (uint16_t)sizeof(USBD_CDC_CfgHSDesc);

    return USBD_CDC_CfgHSDesc;
}

/**
  * @brief  USBD_CDC_GetOtherSpeedCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t* USBD_CDC_GetOtherSpeedCfgDesc(uint16_t* length) {
    *length = (uint16_t)sizeof(USBD_CDC_OtherSpeedCfgDesc);

    return USBD_CDC_OtherSpeedCfgDesc;
}

/**
  * @brief  USBD_CDC_GetDeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t* USBD_CDC_GetDeviceQualifierDescriptor(uint16_t* length) {
    *length = (uint16_t)sizeof(USBD_CDC_DeviceQualifierDesc);

    return USBD_CDC_DeviceQualifierDesc;
}

/**
  * @brief  USBD_CDC_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef* pdev, USBD_CDC_ItfTypeDef* fops) {
    if(fops == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    pdev->pUserData = fops;

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t USBD_CDC_SetTxBuffer(USBD_HandleTypeDef* pdev, uint8_t* pbuff, uint32_t length) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

    if(hcdc == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    hcdc->TxBuffer = pbuff;
    hcdc->TxLength = length;

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t USBD_CDC_SetRxBuffer(USBD_HandleTypeDef* pdev, uint8_t* pbuff) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

    if(hcdc == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    hcdc->RxBuffer = pbuff;

    return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_TransmitPacket(USBD_HandleTypeDef* pdev, uint16_t index) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;
    USBD_StatusTypeDef ret = USBD_BUSY;

    if(pdev->pClassData == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    if(hcdc->TxState == 0U) {
        /* Tx Transfer in progress */
        hcdc->TxState = 1U;

        /* Update the packet total length */
        pdev->ep_in[((index < 2) ? CDC_1_IN_EP : CDC_2_IN_EP) & 0xFU].total_length =
            hcdc->TxLength;

        /* Transmit next packet */
        (void)USBD_LL_Transmit(
            pdev, (index < 2) ? CDC_1_IN_EP : CDC_2_IN_EP, hcdc->TxBuffer, hcdc->TxLength);

        ret = USBD_OK;
    }

    return (uint8_t)ret;
}

/**
  * @brief  USBD_CDC_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_ReceivePacket(USBD_HandleTypeDef* pdev, uint16_t index) {
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

    if(pdev->pClassData == NULL) {
        return (uint8_t)USBD_FAIL;
    }

    if(pdev->dev_speed == USBD_SPEED_HIGH) {
        /* Prepare Out endpoint to receive next packet */
        (void)USBD_LL_PrepareReceive(
            pdev,
            (index < 2) ? CDC_1_OUT_EP : CDC_2_OUT_EP,
            hcdc->RxBuffer,
            CDC_DATA_HS_OUT_PACKET_SIZE);

    } else {
        /* Prepare Out endpoint to receive next packet */
        (void)USBD_LL_PrepareReceive(
            pdev,
            (index < 2) ? CDC_1_OUT_EP : CDC_2_OUT_EP,
            hcdc->RxBuffer,
            CDC_DATA_FS_OUT_PACKET_SIZE);
    }

    return (uint8_t)USBD_OK;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
