#include <furi_hal_version.h>
#include <furi_hal_usb_i.h>
#include <furi_hal_usb.h>
#include <furi.h>

#include "usb.h"
#include "furi_hal_usb_printer.h"

// USB Printer Class specific definitions
#define USB_PRINTER_CLASS 0x07
#define USB_PRINTER_SUBCLASS 0x01
#define USB_PRINTER_PROTOCOL 0x02  // Bidirectional interface

// Printer Class specific requests
#define USB_PRINTER_GET_DEVICE_ID 0x00
#define USB_PRINTER_GET_PORT_STATUS 0x01
#define USB_PRINTER_SOFT_RESET 0x02

// Printer Port Status Bits
#define PRINTER_STATUS_PAPER_EMPTY (1 << 5)
#define PRINTER_STATUS_SELECT (1 << 4)
#define PRINTER_STATUS_NOT_ERROR (1 << 3)

// Endpoints
#define PRINTER_DATA_OUT_EP 0x01
#define PRINTER_DATA_IN_EP 0x82

#define PRINTER_EP_SZ 64

// Device ID String (example for a simple thermal printer)
static const char printer_id_string[] = "MFG:Flipper;CMD:ESC/POS;CLS:PRINTER;MDL:Zero;";

struct PrinterConfigDescriptor {
    struct usb_config_descriptor config;
    struct usb_interface_descriptor printer;
    struct usb_endpoint_descriptor printer_ep_out;
    struct usb_endpoint_descriptor printer_ep_in;
} FURI_PACKED;

// Device descriptor
static struct usb_device_descriptor printer_device_desc = {
    .bLength = sizeof(struct usb_device_descriptor),
    .bDescriptorType = USB_DTYPE_DEVICE,
    .bcdUSB = VERSION_BCD(2, 0, 0),
    .bDeviceClass = USB_PRINTER_CLASS,
    .bDeviceSubClass = USB_PRINTER_SUBCLASS,
    .bDeviceProtocol = USB_PRINTER_PROTOCOL,
    .bMaxPacketSize0 = USB_EP0_SIZE,
    .idVendor = 0x0483,  // STMicroelectronics
    .idProduct = 0x5743,  // Unique PID for printer
    .bcdDevice = VERSION_BCD(1, 0, 0),
    .iManufacturer = 0,
    .iProduct = 0,
    .iSerialNumber = 0,
    .bNumConfigurations = 1,
};

// Configuration descriptor
static const struct PrinterConfigDescriptor printer_cfg_desc = {
    .config = {
        .bLength = sizeof(struct usb_config_descriptor),
        .bDescriptorType = USB_DTYPE_CONFIGURATION,
        .wTotalLength = sizeof(struct PrinterConfigDescriptor),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = NO_DESCRIPTOR,
        .bmAttributes = USB_CFG_ATTR_RESERVED | USB_CFG_ATTR_SELFPOWERED,
        .bMaxPower = USB_CFG_POWER_MA(500),
    },
    .printer = {
        .bLength = sizeof(struct usb_interface_descriptor),
        .bDescriptorType = USB_DTYPE_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_PRINTER_CLASS,
        .bInterfaceSubClass = USB_PRINTER_SUBCLASS,
        .bInterfaceProtocol = USB_PRINTER_PROTOCOL,
        .iInterface = NO_DESCRIPTOR,
    },
    .printer_ep_out = {
        .bLength = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType = USB_DTYPE_ENDPOINT,
        .bEndpointAddress = PRINTER_DATA_OUT_EP,
        .bmAttributes = USB_EPTYPE_BULK,
        .wMaxPacketSize = PRINTER_EP_SZ,
        .bInterval = 0,
    },
    .printer_ep_in = {
        .bLength = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType = USB_DTYPE_ENDPOINT,
        .bEndpointAddress = PRINTER_DATA_IN_EP,
        .bmAttributes = USB_EPTYPE_BULK,
        .wMaxPacketSize = PRINTER_EP_SZ,
        .bInterval = 0,
    },
};

static const struct usb_string_descriptor dev_manuf_desc = USB_STRING_DESC("Flipper Devices Inc.");
static const struct usb_string_descriptor dev_prod_desc = USB_STRING_DESC("Flipper Printer");

static usbd_device* usb_dev;
static uint8_t printer_port_status = PRINTER_STATUS_SELECT | PRINTER_STATUS_NOT_ERROR;
static bool printer_connected = false;
static FuriSemaphore* printer_semaphore = NULL;
static PrinterStatus printer_status = PrinterStatusIdle;
static PrinterError printer_error = PrinterErrorNone;
static FuriMessageQueue* printer_queue = NULL;
static uint8_t* printer_rx_buffer = NULL;
static PrinterCallbacks* printer_callbacks = NULL;
static void* printer_callback_context = NULL;
static void (*printer_rx_user_callback)(uint8_t* data, size_t len, void* context) = NULL;
static void* printer_rx_context = NULL;

#define PRINTER_RX_BUFFER_SIZE 1024
#define PRINTER_QUEUE_SIZE 8

typedef struct {
    uint8_t* data;
    size_t len;
} PrinterQueueItem;

static void printer_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx);
static void printer_deinit(usbd_device* dev);
static void printer_on_wakeup(usbd_device* dev);
static void printer_on_suspend(usbd_device* dev);
static usbd_respond printer_ep_config(usbd_device* dev, uint8_t cfg);
static usbd_respond printer_control(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback);
static void printer_rx_callback(usbd_device* dev, uint8_t event, uint8_t ep);
static void printer_tx_callback(usbd_device* dev, uint8_t event, uint8_t ep);

FuriHalUsbInterface usb_printer = {
    .init = printer_init,
    .deinit = printer_deinit,
    .wakeup = printer_on_wakeup,
    .suspend = printer_on_suspend,
    .dev_descr = (struct usb_device_descriptor*)&printer_device_desc,
    .str_manuf_descr = (void*)&dev_manuf_desc,
    .str_prod_descr = (void*)&dev_prod_desc,
    .str_serial_descr = NULL,
    .cfg_descr = (void*)&printer_cfg_desc,
};

static void printer_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx) {
    UNUSED(intf);
    UNUSED(ctx);
    usb_dev = dev;

    if(printer_semaphore == NULL) {
        printer_semaphore = furi_semaphore_alloc(1, 1);
    }

    if(printer_queue == NULL) {
        printer_queue = furi_message_queue_alloc(PRINTER_QUEUE_SIZE, sizeof(PrinterQueueItem));
    }

    if(printer_rx_buffer == NULL) {
        printer_rx_buffer = malloc(PRINTER_RX_BUFFER_SIZE);
    }

    printer_status = PrinterStatusIdle;
    printer_error = PrinterErrorNone;

    usbd_reg_config(dev, printer_ep_config);
    usbd_reg_control(dev, printer_control);
    usbd_connect(dev, true);
}

static void printer_deinit(usbd_device* dev) {
    UNUSED(dev);
    printer_connected = false;
    printer_status = PrinterStatusIdle;
    printer_error = PrinterErrorNone;

    if(printer_rx_buffer) {
        free(printer_rx_buffer);
        printer_rx_buffer = NULL;
    }

    if(printer_queue) {
        PrinterQueueItem item;
        while(furi_message_queue_get(printer_queue, &item, 0) == FuriStatusOk) {
            free(item.data);
        }
        furi_message_queue_free(printer_queue);
        printer_queue = NULL;
    }
}

static void printer_on_wakeup(usbd_device* dev) {
    UNUSED(dev);
    printer_connected = true;
    
    // Call status callback
    if(printer_callbacks && printer_callbacks->status_callback) {
        printer_callbacks->status_callback(true, printer_callback_context);
    }
}

static void printer_on_suspend(usbd_device* dev) {
    UNUSED(dev);
    printer_connected = false;
    
    // Call status callback
    if(printer_callbacks && printer_callbacks->status_callback) {
        printer_callbacks->status_callback(false, printer_callback_context);
    }
}

// Handle printer class-specific requests
static usbd_respond printer_control(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback) {
    UNUSED(callback);
    
    // Handle only printer class specific requests
    if((req->bmRequestType & (USB_REQ_TYPE | USB_REQ_RECIPIENT)) != (USB_REQ_CLASS | USB_REQ_INTERFACE)) {
        return usbd_fail;
    }

    switch(req->bRequest) {
    case USB_PRINTER_GET_DEVICE_ID:
        dev->status.data_ptr = (uint8_t*)printer_id_string;
        dev->status.data_count = strlen(printer_id_string);
        return usbd_ack;

    case USB_PRINTER_GET_PORT_STATUS:
        dev->status.data_ptr = &printer_port_status;
        dev->status.data_count = 1;
        return usbd_ack;

    case USB_PRINTER_SOFT_RESET:
        // Implement soft reset if needed
        return usbd_ack;

    default:
        return usbd_fail;
    }
}

// Configure endpoints
static usbd_respond printer_ep_config(usbd_device* dev, uint8_t cfg) {
    switch(cfg) {
    case 0:
        // Deconfigure device
        usbd_ep_deconfig(dev, PRINTER_DATA_OUT_EP);
        usbd_ep_deconfig(dev, PRINTER_DATA_IN_EP);
        usbd_reg_endpoint(dev, PRINTER_DATA_OUT_EP, 0);
        usbd_reg_endpoint(dev, PRINTER_DATA_IN_EP, 0);
        return usbd_ack;
    case 1:
        // Configure device
        usbd_ep_config(dev, PRINTER_DATA_OUT_EP, USB_EPTYPE_BULK, PRINTER_EP_SZ);
        usbd_ep_config(dev, PRINTER_DATA_IN_EP, USB_EPTYPE_BULK, PRINTER_EP_SZ);
        usbd_reg_endpoint(dev, PRINTER_DATA_OUT_EP, printer_rx_callback);
        usbd_reg_endpoint(dev, PRINTER_DATA_IN_EP, printer_tx_callback);
        return usbd_ack;
    default:
        return usbd_fail;
    }
}

// Callback for receiving data from host
static void printer_rx_callback(usbd_device* dev, uint8_t event, uint8_t ep) {
    UNUSED(event);
    
    int32_t len = usbd_ep_read(dev, ep, printer_rx_buffer, PRINTER_RX_BUFFER_SIZE);
    if(len > 0) {
        printer_status = PrinterStatusBusy;
        
        if(printer_callbacks && printer_callbacks->data_rx_callback) {
            printer_callbacks->data_rx_callback(printer_rx_buffer, len, printer_callback_context);
        } else if(printer_rx_user_callback) {
            printer_rx_user_callback(printer_rx_buffer, len, printer_rx_context);
        } else {
            // Store in queue if no callback is registered
            uint8_t* data_copy = malloc(len);
            if(data_copy) {
                memcpy(data_copy, printer_rx_buffer, len);
                PrinterQueueItem item = {
                    .data = data_copy,
                    .len = len,
                };
                if(furi_message_queue_put(printer_queue, &item, 0) != FuriStatusOk) {
                    free(data_copy);
                    printer_error = PrinterErrorGeneral;
                }
            } else {
                printer_error = PrinterErrorGeneral;
            }
        }
        
        printer_status = PrinterStatusIdle;
    }
}

// Callback for transmitting data to host
static void printer_tx_callback(usbd_device* dev, uint8_t event, uint8_t ep) {
    UNUSED(dev);
    UNUSED(event);
    UNUSED(ep);
    
    if(printer_semaphore) {
        furi_semaphore_release(printer_semaphore);
    }
}

// Public API functions
int32_t furi_hal_usb_printer_receive(uint8_t* buffer, uint16_t max_len) {
    if(!printer_connected || !printer_queue) {
        return 0;
    }

    PrinterQueueItem item;
    if(furi_message_queue_get(printer_queue, &item, 0) == FuriStatusOk) {
        uint16_t copy_len = (item.len > max_len) ? max_len : item.len;
        memcpy(buffer, item.data, copy_len);
        free(item.data);
        return copy_len;
    }
    
    return 0;
}

void furi_hal_usb_printer_set_status(bool paper_empty, bool selected, bool error) {
    printer_port_status = 0;
    if(paper_empty) printer_port_status |= PRINTER_STATUS_PAPER_EMPTY;
    if(selected) printer_port_status |= PRINTER_STATUS_SELECT;
    if(!error) printer_port_status |= PRINTER_STATUS_NOT_ERROR;
}

bool furi_hal_usb_printer_is_connected(void) {
    return printer_connected;
}

PrinterStatus furi_hal_usb_printer_get_status(void) {
    return printer_status;
}

PrinterError furi_hal_usb_printer_get_error(void) {
    return printer_error;
}

void furi_hal_usb_printer_set_callbacks(PrinterCallbacks* callbacks, void* context) {
    printer_callbacks = callbacks;
    printer_callback_context = context;
    
    // Process any queued data if callback is being set
    if(callbacks && callbacks->data_rx_callback && printer_queue) {
        PrinterQueueItem item;
        while(furi_message_queue_get(printer_queue, &item, 0) == FuriStatusOk) {
            callbacks->data_rx_callback(item.data, item.len, context);
            free(item.data);
        }
    }
    
    // Send current connection status
    if(callbacks && callbacks->status_callback) {
        callbacks->status_callback(printer_connected, context);
    }
}

void furi_hal_usb_printer_set_rx_callback(
    void (*callback)(uint8_t* data, size_t len, void* context),
    void* context) {
    printer_rx_user_callback = callback;
    printer_rx_context = context;
    
    // Process any queued data if callback is being set
    if(callback && printer_queue) {
        PrinterQueueItem item;
        while(furi_message_queue_get(printer_queue, &item, 0) == FuriStatusOk) {
            callback(item.data, item.len, context);
            free(item.data);
        }
    }
}
