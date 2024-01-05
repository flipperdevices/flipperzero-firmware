#include "furi_hal_version.h"
#include "furi_hal_usb_i.h"
#include "furi_hal_usb_hid_u2f.h"
#include <furi.h>
#include "usb.h"
#include <usb_hid.h>
#include "usbd_core.h"
#include "usb_std.h"
#include "filelogger.h"

#include "usbif.h"
#include <storage/storage.h>
#include "constants.h"
#include <hid_usage_desktop.h>

static const struct usb_string_descriptor dev_manuf_desc = USB_STRING_DESC("Flipper Devices Inc.");
static const struct usb_string_descriptor dev_prod_desc = USB_STRING_DESC("U2F Token");
#define HID_PAGE_FIDO 0xF1D0
#define HID_FIDO_U2F 0x01
#define HID_FIDO_INPUT 0x20
#define HID_FIDO_OUTPUT 0x21

#define HID_EP_IN 0x81
#define HID_EP_OUT 0x01

static const uint8_t endpoint_type = USB_EPTYPE_INTERRUPT;

struct HidIadDescriptor {
    struct usb_interface_descriptor hid;
    struct usb_hid_descriptor hid_desc;
    struct usb_endpoint_descriptor hid_ep_in;
    struct usb_endpoint_descriptor hid_ep_out;

    struct usb_interface_descriptor hid2;
    struct usb_hid_descriptor hid_desc2;
    struct usb_endpoint_descriptor hid_ep_in2;
    struct usb_endpoint_descriptor hid_ep_out2;

    struct usb_interface_descriptor hid3;
    struct usb_hid_descriptor hid_desc3;
    struct usb_endpoint_descriptor hid_ep_in3;
    struct usb_endpoint_descriptor hid_ep_out3;

    struct usb_interface_descriptor hid4;
    struct usb_hid_descriptor hid_desc4;
    struct usb_endpoint_descriptor hid_ep_in4;
    struct usb_endpoint_descriptor hid_ep_out4;

    struct usb_interface_descriptor hid5;
    struct usb_hid_descriptor hid_desc5;
    struct usb_endpoint_descriptor hid_ep_in5;
    struct usb_endpoint_descriptor hid_ep_out5;

    struct usb_interface_descriptor hid6;
    struct usb_hid_descriptor hid_desc6;
    struct usb_endpoint_descriptor hid_ep_in6;
    struct usb_endpoint_descriptor hid_ep_out6;

    // struct usb_interface_descriptor hid7;
    // struct usb_hid_descriptor hid_desc7;
    // struct usb_endpoint_descriptor hid_ep_in7;
    // struct usb_endpoint_descriptor hid_ep_out7;

    // struct usb_interface_descriptor hid8;
    // struct usb_hid_descriptor hid_desc8;
    // struct usb_endpoint_descriptor hid_ep_in8;
    // struct usb_endpoint_descriptor hid_ep_out8;
} __attribute__((packed));

struct HidConfigDescriptor {
    struct usb_config_descriptor config;
    struct HidIadDescriptor iad_0;
} __attribute__((packed));

static const struct usb_device_descriptor hid_u2f_device_desc = {
    .bLength = sizeof(struct usb_device_descriptor),
    .bDescriptorType = USB_DTYPE_DEVICE,
    .bcdUSB = VERSION_BCD(2, 0, 0),
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = USB_EP0_SIZE,
    .idVendor = 0x485, // 0x0483,
    .idProduct = 0xFFFF, //0x5741,
    .bcdDevice = VERSION_BCD(1, 0, 0),
    .iManufacturer = UsbDevManuf,
    .iProduct = UsbDevProduct,
    .iSerialNumber = 0,
    .bNumConfigurations = 1,
};

/* HID report: FIDO U2F */
static const uint8_t hid_u2f_report_desc[] = {
    HID_RI_USAGE_PAGE(16, 0xff00),
    HID_USAGE(HID_FIDO_U2F),
    HID_COLLECTION(HID_APPLICATION_COLLECTION),
    HID_USAGE(HID_FIDO_INPUT),
    HID_LOGICAL_MINIMUM(0x00),
    HID_RI_LOGICAL_MAXIMUM(16, 0xFF),
    HID_REPORT_SIZE(8),
    HID_REPORT_COUNT(HID_U2F_PACKET_LEN),
    HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
    HID_USAGE(HID_FIDO_OUTPUT),
    HID_LOGICAL_MINIMUM(0x00),
    HID_RI_LOGICAL_MAXIMUM(16, 0xFF),
    HID_REPORT_SIZE(8),
    HID_REPORT_COUNT(HID_U2F_PACKET_LEN),
    HID_OUTPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
    HID_END_COLLECTION,
};

// static const uint8_t hid_u2f_report_desc[] = {
// 0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
// 0x09, 0x01,        // Usage (0x01)
// 0xA1, 0x01,        // Collection (Application)
// 0x19, 0x01,        //   Usage Minimum (0x01)
// 0x29, 0x40,        //   Usage Maximum (0x40)
// 0x15, 0x00,        //   Logical Minimum (0)
// 0x26, 0xFF, 0x00,  //   Logical Maximum (255)
// 0x75, 0x08,        //   Report Size (8)
// 0x95, 0x01,        //   Report Count (1)
// 0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
// 0x19, 0x01,        //   Usage Minimum (0x01)
// 0x29, 0x40,        //   Usage Maximum (0x40)
// 0x15, 0x00,        //   Logical Minimum (0)
// 0x26, 0xFF, 0x00,  //   Logical Maximum (255)
// 0x75, 0x08,        //   Report Size (8)
// 0x95, 0x01,        //   Report Count (1)
// 0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
// 0xC0,              // End Collection
// };

/* Device configuration descriptor */
static const struct HidConfigDescriptor
    hid_u2f_cfg_desc =
        {
            .config =
                {
                    .bLength = sizeof(struct usb_config_descriptor),
                    .bDescriptorType = USB_DTYPE_CONFIGURATION,
                    .wTotalLength = sizeof(struct HidConfigDescriptor),
                    .bNumInterfaces = NUM_OF_INTERFACES,
                    .bConfigurationValue = 1,
                    .iConfiguration = NO_DESCRIPTOR,
                    .bmAttributes = USB_CFG_ATTR_RESERVED | USB_CFG_ATTR_SELFPOWERED,
                    .bMaxPower = USB_CFG_POWER_MA(100),
                },
            .iad_0 =
                {
                    .hid =
                        {
                            .bLength = sizeof(struct usb_interface_descriptor),
                            .bDescriptorType = USB_DTYPE_INTERFACE,
                            .bInterfaceNumber = 0,
                            .bAlternateSetting = 0,
                            .bNumEndpoints = 2,
                            .bInterfaceClass = USB_CLASS_HID,
                            .bInterfaceSubClass = USB_HID_SUBCLASS_NONBOOT,
                            .bInterfaceProtocol = USB_HID_PROTO_NONBOOT,
                            .iInterface = NO_DESCRIPTOR,
                        },
                    .hid_desc =
                        {
                            .bLength = sizeof(struct usb_hid_descriptor),
                            .bDescriptorType = USB_DTYPE_HID,
                            .bcdHID = VERSION_BCD(1, 0, 0),
                            .bCountryCode = USB_HID_COUNTRY_NONE,
                            .bNumDescriptors = 1,
                            .bDescriptorType0 = USB_DTYPE_HID_REPORT,
                            .wDescriptorLength0 = sizeof(hid_u2f_report_desc),
                        },
                    .hid_ep_in =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_IN,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid_ep_out =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_OUT,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid2 =
                        {
                            .bLength = sizeof(struct usb_interface_descriptor),
                            .bDescriptorType = USB_DTYPE_INTERFACE,
                            .bInterfaceNumber = 1,
                            .bAlternateSetting = 0,
                            .bNumEndpoints = 2,
                            .bInterfaceClass = USB_CLASS_HID,
                            .bInterfaceSubClass = USB_HID_SUBCLASS_NONBOOT,
                            .bInterfaceProtocol = USB_HID_PROTO_NONBOOT,
                            .iInterface = NO_DESCRIPTOR,
                        },
                    .hid_desc2 =
                        {
                            .bLength = sizeof(struct usb_hid_descriptor),
                            .bDescriptorType = USB_DTYPE_HID,
                            .bcdHID = VERSION_BCD(1, 0, 0),
                            .bCountryCode = USB_HID_COUNTRY_NONE,
                            .bNumDescriptors = 1,
                            .bDescriptorType0 = USB_DTYPE_HID_REPORT,
                            .wDescriptorLength0 = sizeof(hid_u2f_report_desc),
                        },
                    .hid_ep_in2 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_IN + 1,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid_ep_out2 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_OUT + 1,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid3 =
                        {
                            .bLength = sizeof(struct usb_interface_descriptor),
                            .bDescriptorType = USB_DTYPE_INTERFACE,
                            .bInterfaceNumber = 2,
                            .bAlternateSetting = 0,
                            .bNumEndpoints = 2,
                            .bInterfaceClass = USB_CLASS_HID,
                            .bInterfaceSubClass = USB_HID_SUBCLASS_NONBOOT,
                            .bInterfaceProtocol = USB_HID_PROTO_NONBOOT,
                            .iInterface = NO_DESCRIPTOR,
                        },
                    .hid_desc3 =
                        {
                            .bLength = sizeof(struct usb_hid_descriptor),
                            .bDescriptorType = USB_DTYPE_HID,
                            .bcdHID = VERSION_BCD(1, 0, 0),
                            .bCountryCode = USB_HID_COUNTRY_NONE,
                            .bNumDescriptors = 1,
                            .bDescriptorType0 = USB_DTYPE_HID_REPORT,
                            .wDescriptorLength0 = sizeof(hid_u2f_report_desc),
                        },
                    .hid_ep_in3 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_IN + 2,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid_ep_out3 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_OUT + 2,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid4 =
                        {
                            .bLength = sizeof(struct usb_interface_descriptor),
                            .bDescriptorType = USB_DTYPE_INTERFACE,
                            .bInterfaceNumber = 3,
                            .bAlternateSetting = 0,
                            .bNumEndpoints = 2,
                            .bInterfaceClass = USB_CLASS_HID,
                            .bInterfaceSubClass = USB_HID_SUBCLASS_NONBOOT,
                            .bInterfaceProtocol = USB_HID_PROTO_NONBOOT,
                            .iInterface = NO_DESCRIPTOR,
                        },
                    .hid_desc4 =
                        {
                            .bLength = sizeof(struct usb_hid_descriptor),
                            .bDescriptorType = USB_DTYPE_HID,
                            .bcdHID = VERSION_BCD(1, 0, 0),
                            .bCountryCode = USB_HID_COUNTRY_NONE,
                            .bNumDescriptors = 1,
                            .bDescriptorType0 = USB_DTYPE_HID_REPORT,
                            .wDescriptorLength0 = sizeof(hid_u2f_report_desc),
                        },
                    .hid_ep_in4 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_IN + 3,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid_ep_out4 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_OUT + 3,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid5 =
                        {
                            .bLength = sizeof(struct usb_interface_descriptor),
                            .bDescriptorType = USB_DTYPE_INTERFACE,
                            .bInterfaceNumber = 4,
                            .bAlternateSetting = 0,
                            .bNumEndpoints = 2,
                            .bInterfaceClass = USB_CLASS_HID,
                            .bInterfaceSubClass = USB_HID_SUBCLASS_NONBOOT,
                            .bInterfaceProtocol = USB_HID_PROTO_NONBOOT,
                            .iInterface = NO_DESCRIPTOR,
                        },
                    .hid_desc5 =
                        {
                            .bLength = sizeof(struct usb_hid_descriptor),
                            .bDescriptorType = USB_DTYPE_HID,
                            .bcdHID = VERSION_BCD(1, 0, 0),
                            .bCountryCode = USB_HID_COUNTRY_NONE,
                            .bNumDescriptors = 1,
                            .bDescriptorType0 = USB_DTYPE_HID_REPORT,
                            .wDescriptorLength0 = sizeof(hid_u2f_report_desc),
                        },
                    .hid_ep_in5 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_IN + 4,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid_ep_out5 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_OUT + 4,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid6 =
                        {
                            .bLength = sizeof(struct usb_interface_descriptor),
                            .bDescriptorType = USB_DTYPE_INTERFACE,
                            .bInterfaceNumber = 5,
                            .bAlternateSetting = 0,
                            .bNumEndpoints = 2,
                            .bInterfaceClass = USB_CLASS_HID,
                            .bInterfaceSubClass = USB_HID_SUBCLASS_NONBOOT,
                            .bInterfaceProtocol = USB_HID_PROTO_NONBOOT,
                            .iInterface = NO_DESCRIPTOR,
                        },
                    .hid_desc6 =
                        {
                            .bLength = sizeof(struct usb_hid_descriptor),
                            .bDescriptorType = USB_DTYPE_HID,
                            .bcdHID = VERSION_BCD(1, 0, 0),
                            .bCountryCode = USB_HID_COUNTRY_NONE,
                            .bNumDescriptors = 1,
                            .bDescriptorType0 = USB_DTYPE_HID_REPORT,
                            .wDescriptorLength0 = sizeof(hid_u2f_report_desc),
                        },
                    .hid_ep_in6 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_IN + 5,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                    .hid_ep_out6 =
                        {
                            .bLength = sizeof(struct usb_endpoint_descriptor),
                            .bDescriptorType = USB_DTYPE_ENDPOINT,
                            .bEndpointAddress = HID_EP_OUT + 5,
                            .bmAttributes = endpoint_type,
                            .wMaxPacketSize = HID_U2F_PACKET_LEN,
                            .bInterval = 1,
                        },
                },
};

static void hid_u2f_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx);
static void hid_u2f_deinit(usbd_device* dev);
static void hid_u2f_on_wakeup(usbd_device* dev);
static void hid_u2f_on_suspend(usbd_device* dev);

//static bool hid_u2f_send_report(uint8_t report_id);
static usbd_respond hid_u2f_ep_config(usbd_device* dev, uint8_t cfg);
usbd_respond hid_u2f_controlf(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback);
static usbd_device* usb_dev;
static bool hid_u2f_connected = false;

static void* cb_ctx;

bool furi_hal_hid_u2f_is_connected() {
    return hid_u2f_connected;
}

static void callbackf(HidU2fEvent ev, void* context) {
    FURI_LOG_D(TAG_IF, "callback %d", ev);
    if(context) {
        return;
    }
}

FuriHalUsbInterface usb_hid_bulk = {
    .init = hid_u2f_init,
    .deinit = hid_u2f_deinit,
    .wakeup = hid_u2f_on_wakeup,
    .suspend = hid_u2f_on_suspend,

    .dev_descr = (struct usb_device_descriptor*)&hid_u2f_device_desc,

    .str_manuf_descr = (void*)&dev_manuf_desc,
    .str_prod_descr = (void*)&dev_prod_desc,
    .str_serial_descr = NULL,

    .cfg_descr = (void*)&hid_u2f_cfg_desc,
};

FuriHalUsbInterface* getUsbHidBulk() {
    return &usb_hid_bulk;
}

typedef struct ThreadContext {
    uint8_t id;
    FuriThread* thread;
    FuriSemaphore* hid;
} ThreadContext;

static ThreadContext* threadContexts[NUM_OF_INTERFACES];
static FuriMessageQueue* dataQueue;

static uint32_t tick_space = 0;

static void logQueueStats() {
    uint32_t ticks = furi_get_tick();
    if(ticks - tick_space > 4000) {
        tick_space = ticks;
        size_t queueSize = furi_message_queue_get_count(dataQueue);
        size_t freeS = furi_message_queue_get_space(dataQueue);
        double heapFree = memmgr_get_free_heap() / 1000;
        double totalHeap = memmgr_get_total_heap() / 1000;
        FURI_LOG_D(
            TAG,
            "Stats:\r\n\tQueueCount: %d\r\n\tQueueSpace: %d\r\n\tHeapFree: %.3f\r\n\tTotalHeap: %.3f",
            queueSize,
            freeS,
            heapFree,
            totalHeap);
    }
}

int32_t send_data_thread(void* context) {
    ThreadContext* ctx = (ThreadContext*)context;
    while(true) {
        ThreadMessage msg;
        furi_check(furi_message_queue_get(dataQueue, &msg, FuriWaitForever) == FuriStatusOk);
        if(msg.dataPointer == NULL) {
            FURI_LOG_D(
                TAG_IF,
                "Shutting down thread %s",
                furi_thread_get_name(furi_thread_get_id(ctx->thread)));
            return 0;
        }
        furi_check(furi_semaphore_acquire(ctx->hid, FuriWaitForever) == FuriStatusOk);

        usbd_ep_write(usb_dev, HID_EP_IN + ctx->id, msg.dataPointer, 64);
        free(msg.dataPointer);
        furi_thread_yield();
    }
}

int32_t receive_data_thread(void* context) {
    ThreadContext* ctx = (ThreadContext*)context;
    uint8_t* data;
    int32_t len;
    bool received = false;
    while(true) {
        if(!received) {
            furi_check(furi_semaphore_acquire(ctx->hid, FuriWaitForever) == FuriStatusOk);
        } else {
            FuriStatus status = furi_semaphore_acquire(ctx->hid, 5000);
            if(status == FuriStatusErrorTimeout) {
                FURI_LOG_I(TAG_IF, "Timeout, terminating thread %d", ctx->id);
                ThreadMessage msg = {.dataPointer = NULL};
                furi_check(
                    furi_message_queue_put(dataQueue, &msg, FuriWaitForever) == FuriStatusOk);
                return 0;
            }
            furi_check(status == FuriStatusOk);
        }

        data = malloc(64);
        len = usbd_ep_read(usb_dev, HID_EP_OUT + ctx->id, data, 64);
        if(len <= 0) {
            FURI_LOG_D(TAG_IF, "Received 0 bytes (EP %d)", ctx->id);
            free(data);
            furi_thread_yield();
            continue;
        } else if(len < 64) {
            FURI_LOG_D(TAG_IF, "Received %ld bytes (EP %d)", len, ctx->id);
        }

        //FURI_LOG_D(TAG_IF, "Received %ld bytes (EP %d)", len, ctx->id);
        //FURI_LOG_D(TAG_IF, "0: %02x last: %02x", data[0], data[len-1]);
        received = true;
        ThreadMessage msg = {.dataPointer = data};
        furi_check(furi_message_queue_put(dataQueue, &msg, FuriWaitForever) == FuriStatusOk);
        logQueueStats();
        furi_thread_yield();
    }
}

static int sendThreads = 0;

void initializeSendingData(int sendingThreads) {
    furi_check(sendingThreads <= NUM_OF_INTERFACES);
    sendThreads = sendingThreads;
    for(int i = 0; i < sendThreads; i++) {
        ThreadContext* ctx = threadContexts[i];

        char name[15];
        snprintf(name, 15, "SendThread %d", ctx->id);
        FURI_LOG_D(TAG_IF, "Spawn thread %s", name);
        ctx->thread = furi_thread_alloc_ex(name, 512, send_data_thread, ctx);
        furi_thread_start(ctx->thread);
    }
}

void stopSendingData() {
    for(int i = 0; i < sendThreads; i++) {
        ThreadMessage msg = {.dataPointer = NULL};
        furi_check(furi_message_queue_put(dataQueue, &msg, FuriWaitForever) == FuriStatusOk);
    }

    for(int i = 0; i < sendThreads; i++) {
        ThreadContext* ctx = threadContexts[i];
        furi_thread_join(ctx->thread);
        furi_thread_free(ctx->thread);
    }
}

FuriMessageQueue* initializeReceivingData() {
    for(int i = 0; i < NUM_OF_INTERFACES; i++) {
        ThreadContext* ctx = threadContexts[i];

        char name[16];
        snprintf(name, 16, "RecThread %d", ctx->id);
        FURI_LOG_D(TAG_IF, "Spawn thread %s", name);
        ctx->thread = furi_thread_alloc_ex(name, 1024, receive_data_thread, ctx);
        furi_thread_start(ctx->thread);
    }

    return dataQueue;
}

void stopReceivingData() {
    for(int i = 0; i < NUM_OF_INTERFACES; i++) {
        ThreadContext* ctx = threadContexts[i];
        furi_thread_join(ctx->thread);
        furi_thread_free(ctx->thread);
    }
}

void sendBulkData(uint8_t* data, uint8_t len) {
    UNUSED(len);
    ThreadMessage msg = {.dataPointer = data};
    logQueueStats();
    furi_check(furi_message_queue_put(dataQueue, &msg, FuriWaitForever) == FuriStatusOk);
}

void sendViaEP(uint8_t* data, int interfaceNumber) {
    ThreadContext* ctx = threadContexts[interfaceNumber];
    furi_check(furi_semaphore_acquire(ctx->hid, FuriWaitForever) == FuriStatusOk);
    usbd_ep_write(usb_dev, HID_EP_IN + ctx->id, data, 64);
}

void receiveFromEP(uint8_t* outBuf, int interfaceNumber) {
    ThreadContext* ctx = threadContexts[interfaceNumber];
    furi_check(furi_semaphore_acquire(ctx->hid, FuriWaitForever) == FuriStatusOk);
    usbd_ep_read(usb_dev, HID_EP_OUT + ctx->id, outBuf, 64);
}

static void hid_u2f_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx) {
    FURI_LOG_D(TAG_IF, "hid_u2f_init");
    UNUSED(intf);
    UNUSED(ctx);

    if(threadContexts[0] == NULL) {
        dataQueue = furi_message_queue_alloc(1000, sizeof(ThreadMessage));

        for(int i = 0; i < NUM_OF_INTERFACES; i++) {
            ThreadContext* ctx = malloc(sizeof(ThreadContext));
            threadContexts[i] = ctx;

            ctx->hid = furi_semaphore_alloc(1, 1);
            ctx->id = i;
        }
    }

    usb_dev = dev;

    usbd_reg_config(dev, hid_u2f_ep_config);
    usbd_reg_control(dev, hid_u2f_controlf);

    usbd_connect(dev, true);
}

static void hid_u2f_deinit(usbd_device* dev) {
    FURI_LOG_D(TAG_IF, "hid_u2f_deinit");
    usbd_reg_config(dev, NULL);
    usbd_reg_control(dev, NULL);

    for(int i = 0; i < NUM_OF_INTERFACES; i++) {
        ThreadContext* ctx = threadContexts[i];
        furi_semaphore_free(ctx->hid);

        free(threadContexts[i]);
    }
}

static void hid_u2f_on_wakeup(usbd_device* dev) {
    FURI_LOG_D(TAG_IF, "hid_u2f_on_wakeup");
    UNUSED(dev);
    hid_u2f_connected = true;
    callbackf(HidU2fConnected, cb_ctx);
}

static void hid_u2f_on_suspend(usbd_device* dev) {
    FURI_LOG_D(TAG_IF, "hid_u2f_on_suspend");
    UNUSED(dev);
    if(hid_u2f_connected) {
        hid_u2f_connected = false;
        for(int i = 0; i < NUM_OF_INTERFACES; i++) {
            furi_semaphore_release(threadContexts[i]->hid);
        }
        callbackf(HidU2fDisconnected, cb_ctx);
    }
}

static void hid_u2f_txrx_ep_callback(usbd_device* dev, uint8_t event, uint8_t ep) {
    UNUSED(dev);
    UNUSED(event);
    UNUSED(ep);

    if(ep >= HID_EP_IN) {
        furi_semaphore_release(threadContexts[ep - HID_EP_IN]->hid);
    } else {
        furi_semaphore_release(threadContexts[ep - HID_EP_OUT]->hid);
    }
}

/* Configure endpoints */
static usbd_respond hid_u2f_ep_config(usbd_device* dev, uint8_t cfg) {
    switch(cfg) {
    case 0:
        /* deconfiguring device */
        for(int i = 0; i < NUM_OF_INTERFACES; i++) {
            usbd_ep_deconfig(dev, HID_EP_OUT + i);
            usbd_ep_deconfig(dev, HID_EP_IN + i);
            usbd_reg_endpoint(dev, HID_EP_OUT + i, 0);
            usbd_reg_endpoint(dev, HID_EP_IN + i, 0);
        }

        return usbd_ack;
    case 1:
        /* configuring device */

        for(int i = 0; i < NUM_OF_INTERFACES; i++) {
            usbd_ep_config(dev, HID_EP_IN + i, endpoint_type, HID_U2F_PACKET_LEN);
            usbd_ep_config(dev, HID_EP_OUT + i, endpoint_type, HID_U2F_PACKET_LEN);
            usbd_reg_endpoint(dev, HID_EP_IN + i, hid_u2f_txrx_ep_callback);
            usbd_reg_endpoint(dev, HID_EP_OUT + i, hid_u2f_txrx_ep_callback);
            usbd_ep_write(dev, HID_EP_IN + 1, 0, 0);
        }
        return usbd_ack;
    default:
        return usbd_fail;
    }
}

/* Control requests handler */
usbd_respond hid_u2f_controlf(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback) {
    UNUSED(callback);

    FURI_LOG_D(
        TAG_IF,
        "control: RT %02x, R %02x, V %04x, I %04x, L %04x",
        req->bmRequestType,
        req->bRequest,
        req->wValue,
        req->wIndex,
        req->wLength);

    /* HID control requests */
    if(((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) ==
           (USB_REQ_INTERFACE | USB_REQ_CLASS) &&
       req->wIndex == 0) {
        switch(req->bRequest) {
        case USB_HID_SETIDLE:

            return usbd_ack;
        case USB_STD_SET_INTERFACE:
            return usbd_ack;
        default:
            return usbd_fail;
        }
    }
    if(((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) ==
           (USB_REQ_INTERFACE | USB_REQ_STANDARD) &&
       req->bRequest == USB_STD_GET_DESCRIPTOR) {
        switch(req->wValue >> 8) {
        case USB_DTYPE_HID:
            //dev->status.data_ptr = (uint8_t*)&(hid_u2f_cfg_desc.iad_0.hid_desc2);
            //dev->status.data_count = sizeof(hid_u2f_cfg_desc.iad_0.hid_desc2);
            return usbd_fail;
        case USB_DTYPE_HID_REPORT:
            dev->status.data_ptr = (uint8_t*)hid_u2f_report_desc;
            dev->status.data_count = sizeof(hid_u2f_report_desc);
            return usbd_ack;
        case USB_STD_SET_INTERFACE:
        case USB_STD_GET_INTERFACE:
            return usbd_ack;
        default:
            return usbd_fail;
        }
    }
    return usbd_fail;
}
