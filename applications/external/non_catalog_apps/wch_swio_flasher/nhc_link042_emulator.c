/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <furi.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>
//#include <furi_hal_usb_i.h>
#include <hid_usage_desktop.h>
#include <furi_hal_usb_cdc.h>
#include <hid_usage_button.h>
#include "usb.h"
#include "usb_hid.h"

#include "utils.h"
#include "errors.h"
#include "nhc_link042_emulator.h"
#include "config.h"

#define EP_RX 0x01
#define EP_RX_SIZE 96

#define EP_TX 0x81
#define EP_TX_SIZE 64

#define TAG "NCHL_EMU"

// enum and definition from furi_hal_usb_i.h which is missing in 0.92.2 uFBT build stack
#define USB_EP0_SIZE 8
enum UsbDevDescStr {
    UsbDevLang = 0,
    UsbDevManuf = 1,
    UsbDevProduct = 2,
    UsbDevSerial = 3,
};

static FuriStreamBuffer* rx_stream;
//static FuriStreamBuffer* tx_stream;
static FuriThread* worker_thread;
static usbd_device* current_dev;
static FuriSemaphore* tx_semaphore;

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

#define WORKER_ALL_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

// Device descriptor
static struct usb_device_descriptor builk_device_desc = {
    .bLength = sizeof(struct usb_device_descriptor),
    .bDescriptorType = USB_DTYPE_DEVICE,
    .bcdUSB = VERSION_BCD(2, 0, 0),
    .bDeviceClass = USB_CLASS_PER_INTERFACE,
    .bDeviceSubClass = USB_SUBCLASS_NONE,
    .bDeviceProtocol = USB_PROTO_NONE,
    .bMaxPacketSize0 = USB_EP0_SIZE,
    .idVendor = 0x1986,
    .idProduct = 0x0034,
    .bcdDevice = VERSION_BCD(1, 0, 0),
    .iManufacturer = UsbDevManuf,
    .iProduct = UsbDevProduct,
    .iSerialNumber = NO_DESCRIPTOR,
    .bNumConfigurations = 1,
};

struct BulkIadDescriptor {
    struct usb_interface_descriptor data;
    struct usb_endpoint_descriptor data_eprx;
    struct usb_endpoint_descriptor data_eptx;
};

struct BulkConfigDescriptor {
    struct usb_config_descriptor config;
    struct BulkIadDescriptor iad_0;
} __attribute__((packed));

static const struct BulkConfigDescriptor bulk_cfg_desc = {
    .config =
        {
            .bLength = sizeof(struct usb_config_descriptor),
            .bDescriptorType = USB_DTYPE_CONFIGURATION,
            .wTotalLength = sizeof(struct BulkConfigDescriptor),
            .bNumInterfaces = 1,

            .bConfigurationValue = 1,
            .iConfiguration = NO_DESCRIPTOR,
            .bmAttributes = USB_CFG_ATTR_RESERVED | USB_CFG_ATTR_SELFPOWERED,
            .bMaxPower = USB_CFG_POWER_MA(100),
        },
    .iad_0 =
        {
            .data =
                {
                    .bLength = sizeof(struct usb_interface_descriptor),
                    .bDescriptorType = USB_DTYPE_INTERFACE,
                    .bInterfaceNumber = 0,
                    .bAlternateSetting = 0,
                    .bNumEndpoints = 2,
                    .bInterfaceClass = USB_CLASS_PER_INTERFACE,
                    .bInterfaceSubClass = USB_SUBCLASS_NONE,
                    .bInterfaceProtocol = USB_PROTO_NONE,
                    .iInterface = NO_DESCRIPTOR,
                },
            .data_eprx =
                {
                    .bLength = sizeof(struct usb_endpoint_descriptor),
                    .bDescriptorType = USB_DTYPE_ENDPOINT,
                    .bEndpointAddress = EP_RX,
                    .bmAttributes = USB_EPTYPE_BULK,
                    .wMaxPacketSize = EP_RX_SIZE,
                    .bInterval = 0x01,
                },
            .data_eptx =
                {
                    .bLength = sizeof(struct usb_endpoint_descriptor),
                    .bDescriptorType = USB_DTYPE_ENDPOINT,
                    .bEndpointAddress = EP_TX,
                    .bmAttributes = USB_EPTYPE_BULK,
                    .wMaxPacketSize = EP_TX_SIZE,
                    .bInterval = 0x01,
                },
        },
};

static const struct usb_string_descriptor dev_manuf_desc = USB_STRING_DESC("Flipper Devices Inc.");
static const struct usb_string_descriptor dev_prod_desc = USB_STRING_DESC("WCH SWIO Flasher");
//static const struct usb_string_descriptor dev_serial_desc = USB_STRING_DESC("s2-ch32xx-pgm-v0");

// Control requests handler
usbd_respond ep_control(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback) {
    UNUSED(callback);
    UNUSED(dev);

    switch(req->bRequest) {
    default:
        return usbd_fail;
    }

    return usbd_fail;
}

static void txrx_ep_callback(usbd_device* dev, uint8_t event, uint8_t ep) {
    if(event == usbd_evt_eptx) {
        // TX event
        //if(furi_stream_buffer_bytes_available(tx_stream) > 0) {
        //    char buff[64];
        //    if(size_t len = furi_stream_buffer_receive(tx_stream, buff, sizeof(buff), 0)) {
        //        usbd_ep_write(dev, ep, buff, )
        //    }
        //}
        furi_semaphore_release(tx_semaphore);
    } else if(ep == EP_RX) {
        char buff[128];
        uint32_t readed = usbd_ep_read(dev, ep, buff, sizeof(buff));
        if(readed > 0) {
            furi_stream_buffer_send(rx_stream, buff, readed, 0);
            furi_thread_flags_set(furi_thread_get_id(worker_thread), WorkerEvtRxDone);
        }
    }
}

// Configure endpoints
usbd_respond ep_config(usbd_device* dev, uint8_t cfg) {
    switch(cfg) {
    case 0:
        // deconfiguring device
        usbd_ep_deconfig(dev, EP_RX);
        usbd_reg_endpoint(dev, EP_RX, 0);
        usbd_ep_deconfig(dev, EP_TX);
        usbd_reg_endpoint(dev, EP_TX, 0);
        return usbd_ack;
    case 1:
        // configuring device
        usbd_ep_config(dev, EP_RX, USB_EPTYPE_BULK, EP_RX_SIZE);
        usbd_reg_endpoint(dev, EP_RX, txrx_ep_callback);
        //usbd_ep_write(dev, EP_RX, 0, 0);

        usbd_ep_config(dev, EP_TX, USB_EPTYPE_BULK, EP_TX_SIZE);
        usbd_reg_endpoint(dev, EP_TX, txrx_ep_callback);
        //usbd_ep_write(dev, EP_TX, 0, 0);
        return usbd_ack;
    default:
        return usbd_fail;
    }
}

static void hid_init(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx) {
    UNUSED(intf);
    FuriHalUsbHidConfig* cfg = (FuriHalUsbHidConfig*)ctx;
    UNUSED(cfg);

    current_dev = dev;
    usbd_reg_config(dev, ep_config);
    usbd_reg_control(dev, ep_control);

    usbd_connect(dev, true);
}

static void hid_deinit(usbd_device* dev) {
    UNUSED(dev);
    current_dev = NULL;
    usbd_reg_config(dev, NULL);
    usbd_reg_control(dev, NULL);
}

static void hid_on_wakeup(usbd_device* dev) {
    UNUSED(dev);
}

static void hid_on_suspend(usbd_device* dev) {
    UNUSED(dev);
}

static FuriHalUsbInterface bulk_if = {
    .dev_descr = &builk_device_desc,
    .cfg_descr = (void*)&bulk_cfg_desc,
    .str_manuf_descr = (void*)&dev_manuf_desc,
    .str_prod_descr = (void*)&dev_prod_desc,
    //.str_serial_descr = (void*)&dev_serial_desc,
    .init = hid_init,
    .deinit = hid_deinit,
    .suspend = hid_on_suspend,
    .wakeup = hid_on_wakeup,
};

WchSwioFlasher_NhcLink042Emu*
    WchSwioFlasher_NhcLink042Emu_create(WchSwioFlasher_MinichlinkDebugger* debugger) {
    WchSwioFlasher_NhcLink042Emu* handle = malloc(sizeof(WchSwioFlasher_NhcLink042Emu));
    handle->debugger = debugger;
    handle->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    return handle;
}

static WchSwioFlasher_Error rxData(WchSwioFlasher_NhcLink042Emu* handle) {
    size_t len = 0;
    char buff[64];
    static uint8_t cnt = 0;

    while(1) {
        len = furi_stream_buffer_receive(rx_stream, buff, sizeof(buff), 0);
        if(len == 0) {
            break;
        }
        /*
        FURI_LOG_D(
            TAG,
            "RX: %02X %02X %02X %02X %02X %02X %02X %02X",
            buff[0],
            buff[1],
            buff[2],
            buff[3],
            buff[4],
            buff[5],
            buff[6],
            buff[7]);
        */
        switch(buff[0]) {
        case 0xa0:
// init
#ifdef NCHLNK_TXRX_DEBUG_MSG_ENABLE
            FURI_LOG_D(TAG, "init session");
#endif
            WchSwioFlasher_MinichlinkDebugger_initSession(handle->debugger);
            break;
        case 0xa1:
// exit
#ifdef NCHLNK_TXRX_DEBUG_MSG_ENABLE
            FURI_LOG_D(TAG, "exit session");
#endif
            WchSwioFlasher_MinichlinkDebugger_endSession(handle->debugger);
            break;
        case 0xa6: {
            // delay
            uint8_t delay = *((uint32_t*)&buff[1]);
#ifdef NCHLNK_TXRX_DEBUG_MSG_ENABLE
            FURI_LOG_D(TAG, "delay %d", delay);
#endif
            WchSwioFlasher_MinichlinkDebugger_delayUs(handle->debugger, delay);
            break;
        }
        case 0xa2: {
            // read reg
            uint8_t addr = buff[1];
            uint32_t reg_value;
            memset(buff, 0, sizeof(buff));
            buff[7] = ++cnt; // TX counter (for easier solving problems with minichlink)
            if(WchSwioFlasher_MinichlinkDebugger_readRegister(
                   handle->debugger, addr, &reg_value) == WchSwioFlasher_Ok) {
                buff[0] = 0xa2; // restore command name
                *((uint32_t*)&buff[1]) = reg_value;
#ifdef NCHLNK_TXRX_DEBUG_MSG_ENABLE
                FURI_LOG_D(TAG, "read reg 0x%02X = " FMT_4HEX, addr, _UI(reg_value));
#endif
            } else {
                FURI_LOG_E(TAG, "read reg 0x%02X failed", addr);
            }

            FURI_CRITICAL_ENTER();
            usbd_device* dev = current_dev;
            FURI_CRITICAL_EXIT();

            if(dev) {
                FuriStatus lock_status = furi_semaphore_acquire(
                    tx_semaphore, furi_ms_to_ticks(NCHLNK_TX_SEMAPHORE_TIMEOUT));
                if(lock_status != FuriStatusOk) {
                    FURI_LOG_E(TAG, "unable to acquire TX semaphore " FMT_4HEX, _UI(lock_status));
                    break;
                }
                /*
                FURI_LOG_D(
                    TAG,
                    "TX: %02X %02X %02X %02X %02X %02X %02X %02X",
                    buff[0],
                    buff[1],
                    buff[2],
                    buff[3],
                    buff[4],
                    buff[5],
                    buff[6],
                    buff[7]);
                */
                int32_t wr = usbd_ep_write(dev, EP_TX, buff, sizeof(buff));
                if(wr != sizeof(buff)) {
                    FURI_LOG_E(TAG, "unable to write data error " FMT_4HEX, _UI(wr));
                }

            } else {
                FURI_LOG_E(TAG, "unable to write response, USB not ready");
            }

            break;
        }
        case 0xa3: {
            // write reg
            uint8_t reg = buff[1];
            uint32_t data = *((uint32_t*)&buff[2]);
#ifdef NCHLNK_TXRX_DEBUG_MSG_ENABLE
            FURI_LOG_D(TAG, "write reg 0x%02X to " FMT_4HEX, reg, _UI(data));
#endif
            WchSwioFlasher_MinichlinkDebugger_writeRegister(handle->debugger, reg, data);
            break;
        }
        default:
            FURI_LOG_E(TAG, "unknown command 0x%02X", buff[0]);
            break;
        }
    }

    return WchSwioFlasher_Ok;
}

static int32_t emulator_worker(void* context) {
    furi_assert(context);
    WchSwioFlasher_NhcLink042Emu* handle = context;
    UNUSED(handle);

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check(!(events & FuriFlagError));

        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            rxData(handle);
        }
    }
    return 0;
}

WchSwioFlasher_Error WchSwioFlasher_NhcLink042Emu_attach(WchSwioFlasher_NhcLink042Emu* handle) {
    furi_check(furi_mutex_acquire(handle->mutex, FuriWaitForever) == FuriStatusOk);
    if(!handle->attached) {
        rx_stream = furi_stream_buffer_alloc(RVD_NCHLINKEMU_RX_BUFF_SIZE, 4);
        //tx_stream = furi_stream_buffer_alloc(RVD_NCHLINKEMU_TX_BUFF_SIZE, 4);

        furi_check(rx_stream != NULL);
        //furi_check(tx_stream != NULL);

        handle->usb_mode_prev = furi_hal_usb_get_config();
        furi_hal_usb_unlock();
        furi_check(furi_hal_usb_set_config(&bulk_if, NULL) == true);

        worker_thread = furi_thread_alloc_ex("NhcLink042EmuWorker", 1024, emulator_worker, handle);
        tx_semaphore = furi_semaphore_alloc(1, 1);

        handle->attached = 1;

        furi_thread_start(worker_thread);
    }
    furi_check(furi_mutex_release(handle->mutex) == FuriStatusOk);

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_NhcLink042Emu_detach(WchSwioFlasher_NhcLink042Emu* handle) {
    furi_check(furi_mutex_acquire(handle->mutex, FuriWaitForever) == FuriStatusOk);
    if(handle->attached) {
        furi_thread_flags_set(furi_thread_get_id(worker_thread), WorkerEvtStop);
        furi_thread_join(worker_thread);
        furi_thread_free(worker_thread);

        furi_stream_buffer_free(rx_stream);
        //furi_stream_buffer_free(tx_stream);
        furi_semaphore_free(tx_semaphore);
        furi_check(furi_hal_usb_set_config(handle->usb_mode_prev, NULL));
        handle->attached = 0;
    }
    furi_check(furi_mutex_release(handle->mutex) == FuriStatusOk);

    return WchSwioFlasher_Ok;
}

void WchSwioFlasher_NhcLink042Emu_destroy(WchSwioFlasher_NhcLink042Emu* handle) {
    furi_mutex_free(handle->mutex);

    free(handle);
}