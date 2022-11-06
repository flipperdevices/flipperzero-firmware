#include "usb_midi_driver.h"
#include "cm3_usb_audio.h"
#include "cm3_usb_midi.h"

// Appendix B. "Example: Simple MIDI Adapter" from "Universal Serial Bus Device Class Definition for MIDI Devices", Revision 1.0

#define USB_VID 0x0483
#define USB_PID 0x5740

#define USB_EP0_SIZE 8

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
    .bcdUSB = VERSION_BCD(2, 0, 0),
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
    struct usb_audio_header_descriptor_head header_head;
    struct usb_audio_header_descriptor_body header_body;

    /*
        B.4 MIDIStreaming Interface Descriptors
    */
    // B.4.1 Standard MS Interface Descriptor
    struct usb_interface_descriptor midi_streaming_iface;
    // B.4.2 Class-specific MS Interface Descriptor
    struct usb_midi_header_descriptor header;
    // B.4.3 MIDI IN Jack Descriptor
    struct usb_midi_in_jack_descriptor in_embedded;
    struct usb_midi_in_jack_descriptor in_external;
    // B.4.4 MIDI OUT Jack Descriptor
    struct usb_midi_out_jack_descriptor out_embedded;
    struct usb_midi_out_jack_descriptor out_external;

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