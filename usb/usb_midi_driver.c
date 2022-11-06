#include <furi.h>
#include <furi_hal_console.h>
#include <usb.h>
#include <usb_std.h>

#include "usb_midi_driver.h"
#include "cm3_usb_audio.h"
#include "cm3_usb_midi.h"

// Appendix B. "Example: Simple MIDI Adapter" from "Universal Serial Bus Device Class Definition for MIDI Devices", Revision 1.0

#define USB_VID 0x6666
#define USB_PID 0x5119

#define USB_EP0_SIZE 8

#define USB_MIDI_EP_SIZE 64
#define USB_MIDI_EP_IN 0x81
#define USB_MIDI_EP_OUT 0x01

#define EP_CFG_DECONFIGURE 0
#define EP_CFG_CONFIGURE 1

enum {
    USB_STR_ZERO,
    USB_STR_MANUFACTURER,
    USB_STR_PRODUCT,
    USB_STR_SERIAL_NUMBER,
};

/*
    B.1 Device Descriptor
*/
static const struct usb_device_descriptor device_descriptor = {
    .bLength = sizeof(struct usb_device_descriptor),
    .bDescriptorType = USB_DTYPE_DEVICE,
    .bcdUSB = VERSION_BCD(2, 0, 0), // was 0x0110, 1.10 - current revision of USBspecification.
    .bDeviceClass = USB_CLASS_PER_INTERFACE,
    .bDeviceSubClass = USB_SUBCLASS_NONE,
    .bDeviceProtocol = USB_PROTO_NONE,
    .bMaxPacketSize0 = USB_EP0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = VERSION_BCD(1, 0, 0),
    .iManufacturer = USB_STR_MANUFACTURER,
    .iProduct = USB_STR_PRODUCT,
    .iSerialNumber = USB_STR_SERIAL_NUMBER,
    .bNumConfigurations = 1,
};

struct usb_audio_header_descriptor {
    struct usb_audio_header_descriptor_head head;
    struct usb_audio_header_descriptor_body body;
} __attribute__((packed));

struct usb_midi_jacks_descriptor {
    struct usb_midi_header_descriptor header;
    struct usb_midi_in_jack_descriptor in_embedded;
    struct usb_midi_in_jack_descriptor in_external;
    struct usb_midi_out_jack_descriptor out_embedded;
    struct usb_midi_out_jack_descriptor out_external;
} __attribute__((packed));

struct MidiConfigDescriptor {
    /* 
        B.2 Configuration Descriptor 
    */
    struct usb_config_descriptor config;

    /* 
        B.3 AudioControl Interface Descriptors

        The AudioControl interface describes the device structure (audio function topology) 
        and is used to manipulate the Audio Controls. This device has no audio function incorporated. 
        However, the AudioControl interface is mandatory and therefore both the standard AC interface 
        descriptor and the classspecific AC interface descriptor must be present. 
        The class-specific AC interface descriptor only contains the header descriptor.
    */
    // B.3.1 Standard AC Interface Descriptor
    struct usb_interface_descriptor audio_control_iface;
    // B.3.2 Class-specific AC Interface Descriptor
    struct usb_audio_header_descriptor audio_control_header;

    /*
        B.4 MIDIStreaming Interface Descriptors
    */
    // B.4.1 Standard MS Interface Descriptor
    struct usb_interface_descriptor midi_streaming_iface;
    // B.4.2 Class-specific MS Interface Descriptor
    // B.4.3 MIDI IN Jack Descriptor
    // B.4.4 MIDI OUT Jack Descriptor
    struct usb_midi_jacks_descriptor midi_jacks;

    /*
        B.5 Bulk OUT Endpoint Descriptors
    */
    // B.5.1 Standard Bulk OUT Endpoint Descriptor
    struct usb_endpoint_descriptor bulk_out;
    // B.5.2 Class-specific MS Bulk OUT Endpoint Descriptor
    struct usb_midi_endpoint_descriptor midi_bulk_out;

    /*
        B.6 Bulk IN Endpoint Descriptors
    */
    // B.6.1 Standard Bulk IN Endpoint Descriptor
    struct usb_endpoint_descriptor bulk_in;
    // B.6.2 Class-specific MS Bulk IN Endpoint Descriptor
    struct usb_midi_endpoint_descriptor midi_bulk_in;
} __attribute__((packed));

static const struct MidiConfigDescriptor config_descriptor = {
    .config =
        {
            .bLength = sizeof(struct usb_config_descriptor),
            .bDescriptorType = USB_DTYPE_CONFIGURATION,
            .wTotalLength = sizeof(struct MidiConfigDescriptor),
            .bNumInterfaces = 2, /* control and data */
            .bConfigurationValue = 1,
            .iConfiguration = 0,
            .bmAttributes = USB_CFG_ATTR_RESERVED,
            .bMaxPower = USB_CFG_POWER_MA(100),
        },
    .audio_control_iface =
        {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DTYPE_INTERFACE,
            .bInterfaceNumber = 0,
            .bAlternateSetting = 0,
            .bNumEndpoints = 0,
            .bInterfaceClass = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_AUDIO_SUBCLASS_CONTROL,
            .bInterfaceProtocol = USB_PROTO_NONE,
            .iInterface = 0,
        },
    .audio_control_header =
        {
            .head =
                {
                    .bLength = sizeof(struct usb_audio_header_descriptor),
                    .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
                    .bDescriptorSubtype = USB_AUDIO_TYPE_HEADER,
                    .bcdADC = VERSION_BCD(1, 0, 0),
                    .wTotalLength = sizeof(struct usb_audio_header_descriptor),
                    .bInCollection = 1,
                },
            .body =
                {
                    .baInterfaceNr = 1,
                },
        },
    .midi_streaming_iface =
        {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DTYPE_INTERFACE,
            .bInterfaceNumber = 1,
            .bAlternateSetting = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = USB_CLASS_AUDIO,
            .bInterfaceSubClass = USB_AUDIO_SUBCLASS_MIDISTREAMING,
            .bInterfaceProtocol = USB_PROTO_NONE,
            .iInterface = 0,
        },
    .midi_jacks =
        {
            .header =
                {
                    .bLength = sizeof(struct usb_midi_header_descriptor),
                    .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
                    .bDescriptorSubtype = USB_MIDI_SUBTYPE_MS_HEADER,
                    .bcdMSC = VERSION_BCD(1, 0, 0),
                    .wTotalLength = sizeof(struct usb_midi_jacks_descriptor),
                },
            .in_embedded =
                {
                    .bLength = sizeof(struct usb_midi_in_jack_descriptor),
                    .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
                    .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_IN_JACK,
                    .bJackType = USB_MIDI_JACK_TYPE_EMBEDDED,
                    .bJackID = 0x01,
                    .iJack = 0x00,
                },
            .in_external =
                {
                    .bLength = sizeof(struct usb_midi_in_jack_descriptor),
                    .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
                    .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_IN_JACK,
                    .bJackType = USB_MIDI_JACK_TYPE_EXTERNAL,
                    .bJackID = 0x02,
                    .iJack = 0x00,
                },
            .out_embedded =
                {
                    .head =
                        {
                            .bLength = sizeof(struct usb_midi_out_jack_descriptor),
                            .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
                            .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_OUT_JACK,
                            .bJackType = USB_MIDI_JACK_TYPE_EMBEDDED,
                            .bJackID = 0x03,
                            .bNrInputPins = 1,
                        },
                    .source[0] =
                        {
                            .baSourceID = 0x02,
                            .baSourcePin = 0x01,
                        },
                    .tail =
                        {
                            .iJack = 0x00,
                        },
                },
            .out_external =
                {
                    .head =
                        {
                            .bLength = sizeof(struct usb_midi_out_jack_descriptor),
                            .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
                            .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_OUT_JACK,
                            .bJackType = USB_MIDI_JACK_TYPE_EXTERNAL,
                            .bJackID = 0x04,
                            .bNrInputPins = 1,
                        },
                    .source[0] =
                        {
                            .baSourceID = 0x01,
                            .baSourcePin = 0x01,
                        },
                    .tail =
                        {
                            .iJack = 0x00,
                        },
                },
        },
    .bulk_out =
        {
            .bLength = sizeof(struct usb_endpoint_descriptor),
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            .bEndpointAddress = USB_MIDI_EP_OUT,
            .bmAttributes = USB_EPTYPE_BULK,
            .wMaxPacketSize = USB_MIDI_EP_SIZE,
            .bInterval = 0,
        },
    .midi_bulk_out =
        {
            .head =
                {
                    .bLength = sizeof(struct usb_midi_endpoint_descriptor),
                    .bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT,
                    .bDescriptorSubType = USB_MIDI_SUBTYPE_MS_GENERAL,
                    .bNumEmbMIDIJack = 1,
                },
            .jack[0] =
                {
                    .baAssocJackID = 0x01,
                },
        },
    .bulk_in =
        {
            .bLength = sizeof(struct usb_endpoint_descriptor),
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            .bEndpointAddress = USB_MIDI_EP_IN,
            .bmAttributes = USB_EPTYPE_BULK,
            .wMaxPacketSize = USB_MIDI_EP_SIZE,
            .bInterval = 0,
        },
    .midi_bulk_in =
        {
            .head =
                {
                    .bLength = sizeof(struct usb_midi_endpoint_descriptor),
                    .bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT,
                    .bDescriptorSubType = USB_MIDI_SUBTYPE_MS_GENERAL,
                    .bNumEmbMIDIJack = 1,
                },
            .jack[0] =
                {
                    .baAssocJackID = 0x03,
                },
        },
};

static const struct usb_string_descriptor dev_manufacturer_string =
    USB_STRING_DESC("Flipper Devices Inc.");

static const struct usb_string_descriptor dev_product_string =
    USB_STRING_DESC("Flipper MIDI Device");

static const struct usb_string_descriptor dev_serial_number_string =
    USB_STRING_DESC("Serial Number");

static void midi_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx);
static void midi_deinit(usbd_device* dev);
static void midi_on_wakeup(usbd_device* dev);
static void midi_on_suspend(usbd_device* dev);
static usbd_respond midi_ep_config(usbd_device* dev, uint8_t cfg);
static usbd_respond midi_control(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback);

FuriHalUsbInterface midi_usb_interface = {
    .init = midi_init,
    .deinit = midi_deinit,
    .wakeup = midi_on_wakeup,
    .suspend = midi_on_suspend,
    .dev_descr = (struct usb_device_descriptor*)&device_descriptor,
    .cfg_descr = (void*)&config_descriptor,
};

static void print_bytes(uint8_t* data, uint16_t len) {
    for(uint16_t i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\r\n");
}

#define STRUCT_AS_ARRAY(name) (uint8_t*)&name, sizeof(name)

static void midi_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx) {
    UNUSED(intf);
    UNUSED(ctx);

    FURI_LOG_I("MIDI", "Init");
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.config));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.audio_control_iface));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.audio_control_header));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_streaming_iface));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_jacks.header));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_jacks.in_embedded));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_jacks.in_external));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_jacks.out_embedded));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_jacks.out_external));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.bulk_out));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_bulk_out));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.bulk_in));
    print_bytes(STRUCT_AS_ARRAY(config_descriptor.midi_bulk_in));

    midi_usb_interface.str_manuf_descr = (void*)&dev_manufacturer_string;
    midi_usb_interface.str_prod_descr = (void*)&dev_product_string;
    midi_usb_interface.str_serial_descr = (void*)&dev_serial_number_string;
    midi_usb_interface.dev_descr->idVendor = USB_VID;
    midi_usb_interface.dev_descr->idProduct = USB_PID;

    usbd_reg_config(dev, midi_ep_config);
    usbd_reg_control(dev, midi_control);

    usbd_connect(dev, true);
}

static void midi_deinit(usbd_device* dev) {
    usbd_reg_config(dev, NULL);
    usbd_reg_control(dev, NULL);
}

static void midi_on_wakeup(usbd_device* dev) {
    UNUSED(dev);
}

static void midi_on_suspend(usbd_device* dev) {
    UNUSED(dev);
}

static void midi_in(usbd_device* dev, uint8_t event, uint8_t ep) {
    UNUSED(dev);
    UNUSED(event);
    UNUSED(ep);
}

static void midi_out(usbd_device* dev, uint8_t event, uint8_t ep) {
    char buffer[64];
    int len = 0;

    UNUSED(dev);
    switch(event) {
    case usbd_evt_eptx:
        // tx complete
        break;
    case usbd_evt_eprx:
        len = usbd_ep_read(dev, ep, buffer, 64);
        if(len > 0) {
            // process data
        }
        break;
    default:
        break;
    }
}

static usbd_respond midi_ep_config(usbd_device* dev, uint8_t cfg) {
    switch(cfg) {
    case EP_CFG_DECONFIGURE:
        usbd_ep_deconfig(dev, USB_MIDI_EP_OUT);
        usbd_ep_deconfig(dev, USB_MIDI_EP_IN);
        usbd_reg_endpoint(dev, USB_MIDI_EP_OUT, NULL);
        usbd_reg_endpoint(dev, USB_MIDI_EP_IN, NULL);
        return usbd_ack;
    case EP_CFG_CONFIGURE:
        usbd_ep_config(dev, USB_MIDI_EP_OUT, USB_EPTYPE_BULK, USB_MIDI_EP_SIZE);
        usbd_ep_config(dev, USB_MIDI_EP_IN, USB_EPTYPE_BULK, USB_MIDI_EP_SIZE);
        usbd_reg_endpoint(dev, USB_MIDI_EP_OUT, midi_out);
        usbd_reg_endpoint(dev, USB_MIDI_EP_IN, midi_in);
        return usbd_ack;
    default:
        return usbd_fail;
    }
}

static usbd_respond midi_control(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback) {
    UNUSED(dev);
    UNUSED(req);
    UNUSED(callback);

    return usbd_fail;
}