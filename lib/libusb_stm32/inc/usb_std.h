/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _USB_STD_H_
#define _USB_STD_H_
#if defined(__cplusplus)
extern "C" {
#endif

#define __CAT(x,y) x ## y
#define CAT(x,y) __CAT(x,y)

/**\addtogroup USB_STD USB Standard
 * \brief This module contains generic USB device framework definitions
 * \details This module based on
 * + Chapter 9 of the [Universal Serial Bus Specification Revision 2.0]
 * (https://www.usb.org/sites/default/files/usb_20_20181221.zip)
 * + [LUFA - the Lightweight USB Framework for AVRs.](https://github.com/abcminiuser/lufa)
 * @{ */

/**\name Utility functional macros
 * @{ */
/** Macro to encode major/minor/version number into BCD code
 * \param maj Major version
 * \param min Minor version
 * \param rev Revision
 */
#define VERSION_BCD(maj, min, rev)  (((maj & 0xFF) << 8) | ((min & 0x0F) << 4) | (rev & 0x0F))

/** Macro to create \ref usb_string_descriptor from array */
#define USB_ARRAY_DESC(...)        {.bLength = 2 + sizeof((uint16_t[]){__VA_ARGS__}),\
                                    .bDescriptorType = USB_DTYPE_STRING,\
                                    .wString = {__VA_ARGS__}}
/** Macro to create \ref usb_string_descriptor from string */
#define USB_STRING_DESC(s)         {.bLength = sizeof(CAT(u,s)),\
                                    .bDescriptorType = USB_DTYPE_STRING,\
                                    .wString = {CAT(u,s)}}

/**\brief Macro to set мaximum power consumption field for the \ref usb_config_descriptor */
#define USB_CFG_POWER_MA(mA)        ((mA) >> 1)
/** @} */

/**\name USB device configuration definitions
 * @{ */
#define USB_CFG_ATTR_RESERVED       0x80
#define USB_CFG_ATTR_SELFPOWERED    0x40
/** @} */

/** \anchor USB_ENDPOINT_DEF
 *  \name USB endpoint attributes definitions
 * @{ */
#define USB_EPDIR_IN                0x00    /**<\brief Host-to-device endpoint direction.*/
#define USB_EPDIR_OUT               0x80    /**<\brief Device-to-host endpoint direction.*/
#define USB_EPTYPE_CONTROL          0x00    /**<\brief Control endpoint.*/
#define USB_EPTYPE_ISOCHRONUS       0x01    /**<\brief Isochronous endpoint.*/
#define USB_EPTYPE_BULK             0x02    /**<\brief Bbulk endpoint.*/
#define USB_EPTYPE_INTERRUPT        0x03    /**<\brief Interrupt endpoint.*/
#define USB_EPATTR_NO_SYNC          0x00    /**<\brief No synchronization.*/
#define USB_EPATTR_ASYNC            0x04    /**<\brief Asynchronous endpoint.*/
#define USB_EPATTR_ADAPTIVE         0x08    /**<\brief Adaptive endpoint.*/
#define USB_EPATTR_SYNC             0x0C    /**<\brief Synchronous endpoint.*/
#define USB_EPUSAGE_DATA            0x00    /**<\brief Data endpoint.*/
#define USB_EPUSAGE_FEEDBACK        0x10    /**<\brief Feedback endpoint.*/
#define USB_EPUSAGE_IMP_FEEDBACK    0x20    /**<\brief Implicit feedback Data endpoint.*/
/** @} */

/**\name Special string descriptor indexes
 * @{ */
#define NO_DESCRIPTOR               0x00    /**<\brief String descriptor doesn't exists in the device.*/
#define INTSERIALNO_DESCRIPTOR      0xFE    /**<\brief String descriptor is an internal serial number
                                             * provided by hardware driver.*/
/** @} */

/**\name USB class definitions
 * @{ */
#define USB_CLASS_PER_INTERFACE     0x00    /**<\brief Class defined on interface level.*/
#define USB_SUBCLASS_NONE           0x00    /**<\brief No subclass defined.*/
#define USB_PROTO_NONE              0x00    /**<\brief No protocol defined.*/
#define USB_CLASS_AUDIO             0x01    /**<\brief Audio device class.*/
#define USB_CLASS_PHYSICAL          0x05    /**<\brief Physical device class.*/
#define USB_CLASS_STILL_IMAGE       0x06    /**<\brief Still Imaging device class.*/
#define USB_CLASS_PRINTER           0x07    /**<\brief Printer device class.*/
#define USB_CLASS_MASS_STORAGE      0x08    /**<\brief Mass Storage device class.*/
#define USB_CLASS_HUB               0x09    /**<\brief HUB device class.*/
#define USB_CLASS_CSCID             0x0B    /**<\brief Smart Card device class.*/
#define USB_CLASS_CONTENT_SEC       0x0D    /**<\brief Content Security device class.*/
#define USB_CLASS_VIDEO             0x0E    /**<\brief Video device class.*/
#define USB_CLASS_HEALTHCARE        0x0F    /**<\brief Personal Healthcare device class.*/
#define USB_CLASS_AV                0x10    /**<\brief Audio/Video device class.*/
#define USB_CLASS_BILLBOARD         0x11    /**<\brief Billboard device class.*/
#define USB_CLASS_CBRIDGE           0x12    /**<\brief USB Type-C Bridge device class.*/
#define USB_CLASS_DIAGNOSTIC        0xDC    /**<\brief Diagnostic device class.*/
#define USB_CLASS_WIRELESS          0xE0    /**<\brief Wireless controller class.*/
#define USB_CLASS_MISC              0xEF    /**<\brief Miscellanious device class.*/
#define USB_CLASS_APP_SPEC          0xFE    /**<\brief Application Specific class.*/
#define USB_CLASS_VENDOR            0xFF    /**<\brief Vendor specific class.*/
#define USB_SUBCLASS_VENDOR         0xFF    /**<\brief Vendor specific subclass.*/
#define USB_PROTO_VENDOR            0xFF    /**<\brief Vendor specific protocol.*/
#define USB_CLASS_IAD               0xEF    /**<\brief Class defined on interface association level.*/
#define USB_SUBCLASS_IAD            0x02    /**<\brief Subclass defined on interface association level.*/
#define USB_PROTO_IAD               0x01    /**<\brief Protocol defined on interface association level.*/
/** @} */

/**\name USB Standard descriptor types
 * @{ */
#define USB_DTYPE_DEVICE            0x01    /**<\brief Device descriptor.*/
#define USB_DTYPE_CONFIGURATION     0x02    /**<\brief Configuration descriptor.*/
#define USB_DTYPE_STRING            0x03    /**<\brief String descriptor.*/
#define USB_DTYPE_INTERFACE         0x04    /**<\brief Interface descriptor.*/
#define USB_DTYPE_ENDPOINT          0x05    /**<\brief Endpoint  descriptor.*/
#define USB_DTYPE_QUALIFIER         0x06    /**<\brief Qualifier descriptor.*/
#define USB_DTYPE_OTHER             0x07    /**<\brief Descriptor is of other type. */
#define USB_DTYPE_INTERFACEPOWER    0x08    /**<\brief Interface power descriptor. */
#define USB_DTYPE_OTG               0x09    /**<\brief OTG descriptor.*/
#define USB_DTYPE_DEBUG             0x0A    /**<\brief Debug descriptor.*/
#define USB_DTYPE_INTERFASEASSOC    0x0B    /**<\brief Interface association descriptor.*/
#define USB_DTYPE_CS_INTERFACE      0x24    /**<\brief Class specific interface descriptor.*/
#define USB_DTYPE_CS_ENDPOINT       0x25    /**<\brief Class specific endpoint descriptor.*/
/** @} */

/**\name USB Standard requests
 * @{ */
#define USB_STD_GET_STATUS          0x00    /**<\brief Returns status for the specified recipient.*/
#define USB_STD_CLEAR_FEATURE       0x01    /**<\brief Used to clear or disable a specific feature.*/
#define USB_STD_SET_FEATURE         0x03    /**<\brief Used to set or enable a specific feature.*/
#define USB_STD_SET_ADDRESS         0x05    /**<\brief Sets the device address for all future device
                                             * accesses.*/
#define USB_STD_GET_DESCRIPTOR      0x06    /**<\brief Returns the specified descriptor if the
                                             * descriptor exists.*/
#define USB_STD_SET_DESCRIPTOR      0x07    /**<\brief This request is optional and may be used to
                                             * update existing descriptors or new descriptors may be
                                             * added.*/
#define USB_STD_GET_CONFIG          0x08    /**<\brief Returns the current device configuration value.*/
#define USB_STD_SET_CONFIG          0x09    /**<\brief Sets the device configuration.*/
#define USB_STD_GET_INTERFACE       0x0A    /**<\brief Returns the selected alternate setting for
                                             * the specified interface.*/
#define USB_STD_SET_INTERFACE       0x0B    /**<\brief Allows the host to select an alternate setting
                                             * for the specified interface.*/
#define USB_STD_SYNCH_FRAME         0x0C    /**<\brief Used to set and then report an endpoint's
                                             * synchronization frame.*/
/** @} */

/**\name USB Feature selector
 * @{ */
#define USB_FEAT_ENDPOINT_HALT      0x00    /**<\brief Halt endpoint.*/
#define USB_FEAT_REMOTE_WKUP        0x01
#define USB_FEAT_TEST_MODE          0x02
#define USB_FEAT_DEBUG_MODE         0x06
/** @} */

/**\name USB Test mode Selectors
 * @{ */
#define USB_TEST_J                  0x01    /**<\brief Test J.*/
#define USB_TEST_K                  0x02    /**<\brief Test K.*/
#define USB_TEST_SE0_NAK            0x03    /**<\brief Test SE0 NAK.*/
#define USB_TEST_PACKET             0x04    /**<\brief Test Pcaket.*/
#define USB_TEST_FORCE_ENABLE       0x05    /**<\brief Test Force Enable.*/
/** @} */

/** \addtogroup USB_STD_LANGID USB standard LANGID codes
 * @{ */
#define USB_LANGID_AFR              0x0436   /**<\brief Afrikaans */
#define USB_LANGID_SQI              0x041c   /**<\brief Albanian */
#define USB_LANGID_ARA_SA           0x0401   /**<\brief Arabic (Saudi Arabia) */
#define USB_LANGID_ARA_IQ           0x0801   /**<\brief Arabic (Iraq) */
#define USB_LANGID_ARA_EG           0x0c01   /**<\brief Arabic (Egypt) */
#define USB_LANGID_ARA_LY           0x1001   /**<\brief Arabic (Libya) */
#define USB_LANGID_ARA_DZ           0x1401   /**<\brief Arabic (Algeria) */
#define USB_LANGID_ARA_MA           0x1801   /**<\brief Arabic (Morocco) */
#define USB_LANGID_ARA_TN           0x1c01   /**<\brief Arabic (Tunisia) */
#define USB_LANGID_ARA_OM           0x2001   /**<\brief Arabic (Oman) */
#define USB_LANGID_ARA_YE           0x2401   /**<\brief Arabic (Yemen) */
#define USB_LANGID_ARA_SY           0x2801   /**<\brief Arabic (Syria) */
#define USB_LANGID_ARA_JO           0x2c01   /**<\brief Arabic (Jordan) */
#define USB_LANGID_ARA_LB           0x3001   /**<\brief Arabic (Lebanon) */
#define USB_LANGID_ARA_KW           0x3401   /**<\brief Arabic (Kuwait) */
#define USB_LANGID_ARA_AE           0x3801   /**<\brief Arabic (U.A.E.) */
#define USB_LANGID_ARA_BH           0x3c01   /**<\brief Arabic (Bahrain) */
#define USB_LANGID_ARA_QA           0x4001   /**<\brief Arabic (Qatar) */
#define USB_LANGID_HYE              0x042b   /**<\brief Armenian */
#define USB_LANGID_ASM              0x044d   /**<\brief Assamese */
#define USB_LANGID_AZE_LAT          0x042c   /**<\brief Azeri (Latin) */
#define USB_LANGID_AZE_CYR          0x082c   /**<\brief Azeri (Cyrillic) */
#define USB_LANGID_EUS              0x042d   /**<\brief Basque */
#define USB_LANGID_BEL              0x0423   /**<\brief Belarussian */
#define USB_LANGID_BEN              0x0445   /**<\brief Bengali */
#define USB_LANGID_BUL              0x0402   /**<\brief Bulgarian */
#define USB_LANGID_MYA              0x0455   /**<\brief Burmese */
#define USB_LANGID_CAT              0x0403   /**<\brief Catalan */
#define USB_LANGID_ZHO_TW           0x0404   /**<\brief Chinese (Taiwan) */
#define USB_LANGID_ZHO_CN           0x0804   /**<\brief Chinese (PRC) */
#define USB_LANGID_ZHO_HK           0x0c04   /**<\brief Chinese (Hong Kong SAR, PRC) */
#define USB_LANGID_ZHO_SG           0x1004   /**<\brief Chinese (Singapore) */
#define USB_LANGID_ZHO_MO           0x1404   /**<\brief Chinese (Macau SAR) */
#define USB_LANGID_HRV              0x041a   /**<\brief Croatian */
#define USB_LANGID_CZE              0x0405   /**<\brief Czech */
#define USB_LANGID_DAN              0x0406   /**<\brief Danish */
#define USB_LANGID_NLD_NL           0x0413   /**<\brief Dutch (Netherlands) */
#define USB_LANGID_NLD_BE           0x0813   /**<\brief Dutch (Belgium) */
#define USB_LANGID_ENG_US           0x0409   /**<\brief English (United States) */
#define USB_LANGID_ENG_UK           0x0809   /**<\brief English (United Kingdom) */
#define USB_LANGID_ENG_AU           0x0c09   /**<\brief English (Australian) */
#define USB_LANGID_ENG_CA           0x1009   /**<\brief English (Canadian) */
#define USB_LANGID_ENG_NZ           0x1409   /**<\brief English (New Zealand) */
#define USB_LANGID_ENG_IE           0x1809   /**<\brief English (Ireland) */
#define USB_LANGID_ENG_ZA           0x1c09   /**<\brief English (South Africa) */
#define USB_LANGID_ENG_JM           0x2009   /**<\brief English (Jamaica) */
#define USB_LANGID_ENG_CAR          0x2409   /**<\brief English (Caribbean) */
#define USB_LANGID_ENG_BZ           0x2809   /**<\brief English (Belize) */
#define USB_LANGID_ENG_TH           0x2c09   /**<\brief English (Trinidad) */
#define USB_LANGID_ENG_ZW           0x3009   /**<\brief English (Zimbabwe) */
#define USB_LANGID_ENG_PH           0x3409   /**<\brief English (Philippines) */
#define USB_LANGID_EST              0x0425   /**<\brief Estonian */
#define USB_LANGID_FAO              0x0438   /**<\brief Faeroese */
#define USB_LANGID_FAS              0x0429   /**<\brief Farsi */
#define USB_LANGID_FIN              0x040b   /**<\brief Finnish */
#define USB_LANGID_FRA              0x040c   /**<\brief French (Standard) */
#define USB_LANGID_FRA_BE           0x080c   /**<\brief French (Belgian) */
#define USB_LANGID_FRA_CA           0x0c0c   /**<\brief French (Canadian) */
#define USB_LANGID_FRA_SZ           0x100c   /**<\brief French (Switzerland) */
#define USB_LANGID_FRA_LU           0x140c   /**<\brief French (Luxembourg) */
#define USB_LANGID_FRA_MC           0x180c   /**<\brief French (Monaco) */
#define USB_LANGID_KAT              0x0437   /**<\brief Georgian */
#define USB_LANGID_DEU              0x0407   /**<\brief German (Standard) */
#define USB_LANGID_DEU_SZ           0x0807   /**<\brief German (Switzerland) */
#define USB_LANGID_DEU_AT           0x0c07   /**<\brief German (Austria) */
#define USB_LANGID_DEU_LU           0x1007   /**<\brief German (Luxembourg) */
#define USB_LANGID_DEU_LI           0x1407   /**<\brief German (Liechtenstein) */
#define USB_LANGID_ELL              0x0408   /**<\brief Greek */
#define USB_LANGID_GUJ              0x0447   /**<\brief Gujarati */
#define USB_LANGID_HEB              0x040d   /**<\brief Hebrew */
#define USB_LANGID_HIN              0x0439   /**<\brief Hindi */
#define USB_LANGID_HUN              0x040e   /**<\brief Hungarian */
#define USB_LANGID_ISL              0x040f   /**<\brief Icelandic */
#define USB_LANGID_IND              0x0421   /**<\brief Indonesian */
#define USB_LANGID_ITA              0x0410   /**<\brief Italian (Standard) */
#define USB_LANGID_ITA_SZ           0x0810   /**<\brief Italian (Switzerland) */
#define USB_LANGID_JPN              0x0411   /**<\brief Japanese */
#define USB_LANGID_KAN              0x044b   /**<\brief Kannada */
#define USB_LANGID_KAS              0x0860   /**<\brief Kashmiri (India) */
#define USB_LANGID_KAZ              0x043f   /**<\brief Kazakh */
#define USB_LANGID_KOK              0x0457   /**<\brief Konkani */
#define USB_LANGID_KOR              0x0412   /**<\brief Korean */
#define USB_LANGID_KOR_JOH          0x0812   /**<\brief Korean (Johab) */
#define USB_LANGID_LAV              0x0426   /**<\brief Latvian */
#define USB_LANGID_LIT              0x0427   /**<\brief Lithuanian */
#define USB_LANGID_LIT_CLS          0x0827   /**<\brief Lithuanian (Classic) */
#define USB_LANGID_MKD              0x042f   /**<\brief Macedonian */
#define USB_LANGID_MSA              0x043e   /**<\brief Malay (Malaysian) */
#define USB_LANGID_MSA_BN           0x083e   /**<\brief Malay (Brunei Darussalam) */
#define USB_LANGID_MAL              0x044c   /**<\brief Malayalam */
#define USB_LANGID_MNI              0x0458   /**<\brief Manipuri */
#define USB_LANGID_MAR              0x044e   /**<\brief Marathi */
#define USB_LANGID_NEP              0x0861   /**<\brief Nepali (India) */
#define USB_LANGID_NOB              0x0414   /**<\brief Norwegian (Bokmal) */
#define USB_LANGID_NNO              0x0814   /**<\brief Norwegian (Nynorsk) */
#define USB_LANGID_ORI              0x0448   /**<\brief Oriya */
#define USB_LANGID_POL              0x0415   /**<\brief Polish */
#define USB_LANGID_POR_BR           0x0416   /**<\brief Portuguese (Brazil) */
#define USB_LANGID_POR              0x0816   /**<\brief Portuguese (Standard) */
#define USB_LANGID_PAN              0x0446   /**<\brief Punjabi */
#define USB_LANGID_RON              0x0418   /**<\brief Romanian */
#define USB_LANGID_RUS              0x0419   /**<\brief Russian */
#define USB_LANGID_SAN              0x044f   /**<\brief Sanskrit */
#define USB_LANGID_SRB_CYR          0x0c1a   /**<\brief Serbian (Cyrillic) */
#define USB_LANGID_SRB_LAT          0x081a   /**<\brief Serbian (Latin) */
#define USB_LANGID_SND              0x0459   /**<\brief Sindhi */
#define USB_LANGID_SLK              0x041b   /**<\brief Slovak */
#define USB_LANGID_SLV              0x0424   /**<\brief Slovenian */
#define USB_LANGID_SPA              0x040a   /**<\brief Spanish (Traditional Sort) */
#define USB_LANGID_SPA_MX           0x080a   /**<\brief Spanish (Mexican) */
#define USB_LANGID_SPA_MDN          0x0c0a   /**<\brief Spanish (Modern Sort) */
#define USB_LANGID_SPA_GT           0x100a   /**<\brief Spanish (Guatemala) */
#define USB_LANGID_SPA_CR           0x140a   /**<\brief Spanish (Costa Rica) */
#define USB_LANGID_SPA_PA           0x180a   /**<\brief Spanish (Panama) */
#define USB_LANGID_SPA_DO           0x1c0a   /**<\brief Spanish (Dominican Republic) */
#define USB_LANGID_SPA_VE           0x200a   /**<\brief Spanish (Venezuela) */
#define USB_LANGID_SPA_CO           0x240a   /**<\brief Spanish (Colombia) */
#define USB_LANGID_SPA_PE           0x280a   /**<\brief Spanish (Peru) */
#define USB_LANGID_SPA_AR           0x2c0a   /**<\brief Spanish (Argentina) */
#define USB_LANGID_SPA_EC           0x300a   /**<\brief Spanish (Ecuador) */
#define USB_LANGID_SPA_CL           0x340a   /**<\brief Spanish (Chile) */
#define USB_LANGID_SPA_UY           0x380a   /**<\brief Spanish (Uruguay) */
#define USB_LANGID_SPA_PY           0x3c0a   /**<\brief Spanish (Paraguay) */
#define USB_LANGID_SPA_BO           0x400a   /**<\brief Spanish (Bolivia) */
#define USB_LANGID_SPA_SV           0x440a   /**<\brief Spanish (El Salvador) */
#define USB_LANGID_SPA_HN           0x480a   /**<\brief Spanish (Honduras) */
#define USB_LANGID_SPA_NI           0x4c0a   /**<\brief Spanish (Nicaragua) */
#define USB_LANGID_SPA_PR           0x500a   /**<\brief Spanish (Puerto Rico) */
#define USB_LANGID_NSO              0x0430   /**<\brief Sutu, Sotho. */
#define USB_LANGID_SWA              0x0441   /**<\brief Swahili (Kenya) */
#define USB_LANGID_SWE              0x041d   /**<\brief Swedish */
#define USB_LANGID_SWE_FI           0x081d   /**<\brief Swedish (Finland) */
#define USB_LANGID_TAM              0x0449   /**<\brief Tamil */
#define USB_LANGID_TAT              0x0444   /**<\brief Tatar (Tatarstan) */
#define USB_LANGID_TEL              0x044a   /**<\brief Telugu */
#define USB_LANGID_THA              0x041e   /**<\brief Thai */
#define USB_LANGID_TUR              0x041f   /**<\brief Turkish */
#define USB_LANGIG_UKR              0x0422   /**<\brief Ukrainian */
#define USB_LANGID_URD_PK           0x0420   /**<\brief Urdu (Pakistan) */
#define USB_LANGID_URD_IN           0x0820   /**<\brief Urdu (India) */
#define USB_LANGID_UZB_LAT          0x0443   /**<\brief Uzbek (Latin) */
#define USB_LANGID_UZB_CYR          0x0843   /**<\brief Uzbek (Cyrillic) */
#define USB_LANGID_VIE              0x042a   /**<\brief Vietnamese */
/** @} */

/**\brief common USB descriptor header */
struct usb_header_descriptor {
    uint8_t bLength;                /**<\brief Size of the descriptor, in bytes. */
    uint8_t bDescriptorType;        /**<\brief Type of the descriptor. */
} __attribute__((packed));

/**\brief Represents a USB device descriptor
 * \details A device descriptor describes general information about a USB device. It includes
 * information that applies globally to the device and all of the device’s configurations. A USB
 * device has only one device descriptor. A high-speed capable device that has different device
 * information for full-speed and high-speed must also  have a \ref usb_qualifier_descriptor.*/
struct usb_device_descriptor {
    uint8_t  bLength;               /**<\brief Size of the descriptor, in bytes.*/
    uint8_t  bDescriptorType;       /**<\brief \ref USB_DTYPE_DEVICE Device descriptor.*/
    uint16_t bcdUSB;                /**<\brief BCD of the supported USB specification.*/
    uint8_t  bDeviceClass;          /**<\brief USB device class.*/
    uint8_t  bDeviceSubClass;       /**<\brief USB device subclass.*/
    uint8_t  bDeviceProtocol;       /**<\brief USB device protocol.*/
    uint8_t  bMaxPacketSize0;       /**<\brief Size of the control endpoint's bank in bytes.*/
    uint16_t idVendor;              /**<\brief Vendor ID for the USB product.*/
    uint16_t idProduct;             /**<\brief Unique product ID for the USB product.*/
    uint16_t bcdDevice;             /**<\brief Product release (version) number.*/
    uint8_t  iManufacturer;         /**<\brief String index for the manufacturer's name.*/
    uint8_t  iProduct;              /**<\brief String index for the product name/details.*/
    uint8_t  iSerialNumber;         /**<\brief String index for the product serial number.*/
    uint8_t  bNumConfigurations;    /**<\brief Total number of configurations supported by the device.*/
} __attribute__((packed));

/**\brief USB device qualifier descriptor
 * \details The device_qualifier descriptor describes information about a high-speed capable device
 * that would change if the device were operating at the other speed. For example, if the device is
 * currently operating at full-speed, the device qualifier returns information about how it would
 * operate at high-speed and vice-versa.*/
struct usb_qualifier_descriptor {
    uint8_t  bLength;               /**<\brief Size of the descriptor, in bytes.*/
    uint8_t  bDescriptorType;       /**<\brief Qualifier descriptor.*/
    uint16_t bcdUSB;                /**<\brief BCD of the supported USB specification.*/
    uint8_t  bDeviceClass;          /**<\brief USB device class.*/
    uint8_t  bDeviceSubClass;       /**<\brief USB device subclass.*/
    uint8_t  bDeviceProtocol;       /**<\brief USB device protocol.*/
    uint8_t  bMaxPacketSize0;       /**<\brief Size of the control endpoint's bank in bytes.*/
    uint8_t  bNumConfigurations;    /**<\brief Total number of configurations supported by the device.*/
    uint8_t  bReserved;             /**<\brief Reserved for future use, must be 0.*/
} __attribute__((packed));

/**\brief USB device configuration descriptor
 * \details The configuration descriptor describes information about a specific device configuration.
 * The descriptor contains a bConfigurationValue field with a value that, when used as a parameter
 * to the SetConfiguration() request, causes the device to assume the described configuration.*/
struct usb_config_descriptor {
    uint8_t  bLength;               /**<\brief Size of the descriptor, in bytes.*/
    uint8_t  bDescriptorType;       /**<\brief Configuration descriptor.*/
    uint16_t wTotalLength;          /**<\brief Size of the configuration descriptor header, and all
                                     * sub descriptors inside the configuration. */
    uint8_t  bNumInterfaces;        /**<\brief Total number of interfaces in the configuration.*/
    uint8_t  bConfigurationValue;   /**<\brief Configuration index of the current configuration.*/
    uint8_t  iConfiguration;        /**<\brief Index of a string descriptor describing the configuration.*/
    uint8_t  bmAttributes;          /**<\brief Configuration attributes.
                                     * \details Comprised of a mask of \c USB_CONFIG_ATTR_ masks. On
                                     * all devices, this should include USB_CONFIG_ATTR_RESERVED at
                                     * a minimum. */
    uint8_t  bMaxPower;             /**<\brief Maximum power consumption of the device.
                                     * \ref USB_CFG_POWER_MA() macro.*/
} __attribute__((packed));

/**\brief USB interface descriptor
 * \details The interface descriptor describes a specific interface within a configuration. A
 *configuration provides one or more interfaces, each with zero or more endpoint descriptors
 * describing a unique set of endpoints within the configuration.*/
struct usb_interface_descriptor {
    uint8_t bLength;                /**<\brief Size of the descriptor, in bytes.*/
    uint8_t bDescriptorType;        /**<\brief Interface descriptor.*/
    uint8_t bInterfaceNumber;       /**<\brief Index of the interface in the current configuration.*/
    uint8_t bAlternateSetting;      /**<\brief Alternate setting for the interface number.*/
    uint8_t bNumEndpoints;          /**<\brief Total number of endpoints in the interface.*/
    uint8_t bInterfaceClass;        /**<\brief Interface class ID.*/
    uint8_t bInterfaceSubClass;     /**<\brief Interface subclass ID.*/
    uint8_t bInterfaceProtocol;     /**<\brief Interface protocol ID. */
    uint8_t iInterface;             /**<\brief Index of the string descriptor describing the interface. */
} __attribute__((packed));

/**\brief USB interface association descriptor
 * \details USB interface association descriptor (IAD) allows the device to group interfaces that
 * belong to a function.*/
struct usb_iad_descriptor {
    uint8_t bLength;                /**<\brief Size of the descriptor, in bytes.*/
    uint8_t bDescriptorType;        /**<\brief IAD descriptor */
    uint8_t bFirstInterface;        /**<\brief Index of the first associated interface. */
    uint8_t bInterfaceCount;        /**<\brief Total number of associated interfaces. */
    uint8_t bFunctionClass;         /**<\brief Function class ID. */
    uint8_t bFunctionSubClass;      /**<\brief Function subclass ID. */
    uint8_t bFunctionProtocol;      /**<\brief Function protocol ID. */
    uint8_t iFunction;              /**<\brief Index of the string descriptor describing the
                                     * interface association. */
} __attribute__((packed));

/**\brief USB endpoint descriptor
 * \details This descriptor contains the information required by the host to determine the bandwidth
 * requirements of each endpoint.*/
struct usb_endpoint_descriptor {
    uint8_t  bLength;               /**<\brief Size of the descriptor, in bytes. */
    uint8_t  bDescriptorType;       /**<\brief Endpoint descriptor.*/
    uint8_t  bEndpointAddress;      /**<\brief Logical address of the endpoint within the device for
                                     * the current configuration, including direction mask. */
    uint8_t  bmAttributes;          /**<\brief Endpoint attributes, \ref USB_ENDPOINT_DEF. */
    uint16_t wMaxPacketSize;        /**<\brief Size of the endpoint bank, in bytes. This indicates the
                                     * maximum packet size that the endpoint can receive at a time. */
    uint8_t  bInterval;             /**<\brief Polling interval in milliseconds for the endpoint if
                                     * it is an INTERRUPT or ISOCHRONOUS type.*/
} __attribute__((packed));

/**\brief USB string descriptor
 * \details String descriptors are referenced by their one-based index number. A string descriptor
 * contains one or more not NULL-terminated Unicode strings.
 * \note String descriptors are optional. if a device does not support string descriptors, all
 * references to string descriptors within device, configuration, and interface descriptors must be
 * reset to zero.*/
struct usb_string_descriptor {
    uint8_t  bLength;               /**<\brief Size of the descriptor, in bytes.*/
    uint8_t  bDescriptorType;       /**<\brief String descriptor type.*/
    uint16_t wString[];             /**<\brief String data, as unicode characters or array of
                                     * \ref USB_STD_LANGID codes. */
} __attribute__((packed, aligned(2)));

/**\brief USB debug descriptor
 * \details This descriptor is used to describe certain characteristics of the device that the host
 * debug port driver needs to know to communicate with the device. Specifically, the debug descriptor
 * lists the addresses of the endpoints that comprise the Debug Pipe. The endpoints are identified by
 * endpoint number.*/
struct usb_debug_descriptor {
    uint8_t  bLength;               /**<\brief Size of the descriptor, in bytes.*/
    uint8_t  bDescriptorType;       /**<\brief Debug descriptor type.*/
    uint8_t  bDebugInEndpoint;      /**<\brief Endpoint number of the Debug Data IN endpoint.*/
    uint8_t  bDebugOutEndpoint;     /**<\brief Endpoint number of the Debug Data OUTendpoint.*/
} __attribute__((packed));

/** @} */

#if defined (__cplusplus)
}
#endif
#endif //_USB_STD_H_
