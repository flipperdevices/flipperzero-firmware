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

#include <stdint.h>
#include <stdbool.h>
#include "usb.h"

#define _MIN(a, b) ((a) < (b)) ? (a) : (b)

static void usbd_process_ep0 (usbd_device *dev, uint8_t event, uint8_t ep);

/** \brief Resets USB device state
 * \param dev pointer to usb device
 * \return none
 */
static void usbd_process_reset(usbd_device *dev) {
    dev->status.device_state = usbd_state_default;
    dev->status.control_state = usbd_ctl_idle;
    dev->status.device_cfg = 0;
    dev->driver->ep_config(0, USB_EPTYPE_CONTROL, dev->status.ep0size);
    dev->endpoint[0] = usbd_process_ep0;
    dev->driver->setaddr(0);
}

/** \brief Callback that sets USB device address
 * \param dev pointer to usb device
 * \param req pointer to usb control request data
 * \return none
 */
static void usbd_set_address (usbd_device *dev, usbd_ctlreq *req) {
    dev->driver->setaddr(req->wValue);
    dev->status.device_state = (req->wValue) ? usbd_state_addressed : usbd_state_default;
}

/** \brief Control transfer completion callback processing
 * \param dev pointer to the usb device
 * \return none
 */
static void usbd_process_callback (usbd_device *dev) {
    if (dev->complete_callback) {
        dev->complete_callback(dev, dev->status.data_buf);
        dev->complete_callback = 0;
    }
}

/** \brief SET_CONFIG request processing
 * \param dev usbd_device
 * \param config config number from request
 * \return usbd_ack if success
 */
static usbd_respond usbd_configure(usbd_device *dev, uint8_t config) {
    if (dev->config_callback) {
        if (dev->config_callback(dev, config) == usbd_ack) {
            dev->status.device_cfg = config;
            dev->status.device_state = (config) ? usbd_state_configured : usbd_state_addressed;
            return usbd_ack;
        }
    }
    return usbd_fail;
}


/** \brief Standard control request processing for device
 * \param dev pointer to usb device
 * \param req pointer to control request
 * \return TRUE if request is handled
 */
static usbd_respond usbd_process_devrq (usbd_device *dev, usbd_ctlreq *req) {
    switch (req->bRequest) {
    case USB_STD_CLEAR_FEATURE:
        /* not yet supported */
        break;
    case USB_STD_GET_CONFIG:
        req->data[0] = dev->status.device_cfg;
        return usbd_ack;
    case USB_STD_GET_DESCRIPTOR:
        if (req->wValue == ((USB_DTYPE_STRING << 8) | INTSERIALNO_DESCRIPTOR )) {
            dev->status.data_count = dev->driver->get_serialno_desc(req->data);
            return usbd_ack;
        } else {
            if (dev->descriptor_callback) {
                return dev->descriptor_callback(req, &(dev->status.data_ptr), &(dev->status.data_count));
            }
        }
        break;
    case USB_STD_GET_STATUS:
        req->data[0] = 0;
        req->data[1] = 0;
        return usbd_ack;
    case USB_STD_SET_ADDRESS:
        if (usbd_getinfo(dev) & USBD_HW_ADDRFST) {
            usbd_set_address(dev, req);
        } else {
            dev->complete_callback = usbd_set_address;
        }
        return usbd_ack;
    case USB_STD_SET_CONFIG:
        return usbd_configure(dev, req->wValue);
    case USB_STD_SET_DESCRIPTOR:
        /* should be externally handled */
        break;
    case USB_STD_SET_FEATURE:
        /* not yet supported */
        break;
    default:
        break;
    }
    return usbd_fail;
}

/** \brief Standard control request processing for interface
 * \param dev pointer to usb device
 * \param req pointer to control request
 * \return TRUE if request is handled
 */
static usbd_respond usbd_process_intrq(usbd_device *dev, usbd_ctlreq *req) {
    (void)dev;
    switch (req->bRequest) {
    case USB_STD_GET_STATUS:
        req->data[0] = 0;
        req->data[1] = 0;
        return usbd_ack;
    default:
        break;
    }
    return usbd_fail;
}

/** \brief Standard control request processing for endpoint
 * \param dev pointer to usb device
 * \param req pointer to control request
 * \return TRUE if request is handled
 */
static usbd_respond usbd_process_eptrq(usbd_device *dev, usbd_ctlreq *req) {
    switch (req->bRequest) {
    case USB_STD_SET_FEATURE:
        dev->driver->ep_setstall(req->wIndex, 1);
        return usbd_ack;
    case USB_STD_CLEAR_FEATURE:
        dev->driver->ep_setstall(req->wIndex, 0);
        return usbd_ack;
    case USB_STD_GET_STATUS:
        req->data[0] = dev->driver->ep_isstalled(req->wIndex) ? 1 : 0;
        req->data[1] = 0;
        return usbd_ack;
    default:
        break;
    }
    return usbd_fail;
}

/** \brief Processing control request
 * \param dev pointer to usb device
 * \param req pointer to usb control request
 * \return TRUE if request is handled
 */
static usbd_respond usbd_process_request(usbd_device *dev, usbd_ctlreq *req) {
    /* processing control request by callback */
    if (dev->control_callback) {
        usbd_respond r = dev->control_callback(dev, req, &(dev->complete_callback));
        if (r != usbd_fail) return r;
    }
    /* continuing standard USB requests */
    switch (req->bmRequestType & (USB_REQ_TYPE | USB_REQ_RECIPIENT)) {
    case USB_REQ_STANDARD | USB_REQ_DEVICE:
        return usbd_process_devrq(dev, req);
    case USB_REQ_STANDARD | USB_REQ_INTERFACE:
        return usbd_process_intrq(dev, req);
    case USB_REQ_STANDARD | USB_REQ_ENDPOINT:
        return usbd_process_eptrq(dev, req);
    default:
        break;
    }
    return usbd_fail;
}


/** \brief Control endpoint stall (STALL PID)
 * \param dev pointer to usb device
 * \param ep endpoint number
 */
static void usbd_stall_pid(usbd_device *dev, uint8_t ep) {
    dev->driver->ep_setstall(ep & 0x7F, 1);
    dev->driver->ep_setstall(ep | 0x80, 1);
    dev->status.control_state = usbd_ctl_idle;
}


/** \brief Control endpoint TX event processing
 * \param dev pointer to usb device
 * \param ep endpoint number
 */
static void usbd_process_eptx(usbd_device *dev, uint8_t ep) {
    int32_t _t;
    switch (dev->status.control_state) {
    case usbd_ctl_ztxdata:
    case usbd_ctl_txdata:
        _t = _MIN(dev->status.data_count, dev->status.ep0size);
        dev->driver->ep_write(ep, dev->status.data_ptr, _t);
        dev->status.data_ptr = (uint8_t*)dev->status.data_ptr + _t;
        dev->status.data_count -= _t;
        /* if all data is not sent */
        if (0 != dev->status.data_count) break;
        /* if last packet has a EP0 size and host awaiting for the more data ZLP should be sent*/
        /* if ZLP required, control state will be unchanged, therefore next TX event sends ZLP */
        if ( usbd_ctl_txdata == dev->status.control_state || _t != dev->status.ep0size ) {
            dev->status.control_state = usbd_ctl_lastdata; /* no ZLP required */
        }
        break;
    case usbd_ctl_lastdata:
        dev->status.control_state = usbd_ctl_statusout;
        break;
    case usbd_ctl_statusin:
        dev->status.control_state = usbd_ctl_idle;
        usbd_process_callback(dev);
        break;
    default:
        /* unexpected TX completion */
        /* just skipping it */
        break;
    }
}

/** \brief Control endpoint RX event processing
 * \param dev pointer to usb device
 * \param ep endpoint number
 */
static void usbd_process_eprx(usbd_device *dev, uint8_t ep) {
    uint16_t _t;
    usbd_ctlreq *const req = dev->status.data_buf;
    switch (dev->status.control_state) {
    case usbd_ctl_idle:
        /* read SETUP packet, send STALL_PID if incorrect packet length */
        if (0x08 !=  dev->driver->ep_read(ep, req, dev->status.data_maxsize)) {
            usbd_stall_pid(dev, ep);
            return;
        }
        dev->status.data_ptr = req->data;
        dev->status.data_count = req->wLength;
        /* processing request with no payload data*/
        if ((req->bmRequestType & USB_REQ_DEVTOHOST) || (0 == req->wLength)) break;
        /* checking available memory for DATA OUT stage */
        if (req->wLength > dev->status.data_maxsize) {
            usbd_stall_pid(dev, ep);
            return;
        }
        /* continue DATA OUT stage */
        dev->status.control_state = usbd_ctl_rxdata;
        return;
    case usbd_ctl_rxdata:
        /*receive DATA OUT packet(s) */
        _t = dev->driver->ep_read(ep, dev->status.data_ptr, dev->status.data_count);
        if (dev->status.data_count < _t) {
        /* if received packet is large than expected */
        /* Must be error. Let's drop this request */
            usbd_stall_pid(dev, ep);
            return;
        } else if (dev->status.data_count != _t) {
        /* if all data payload was not received yet */
            dev->status.data_count -= _t;
            dev->status.data_ptr = (uint8_t*)dev->status.data_ptr + _t;
            return;
        }
        break;
    case usbd_ctl_statusout:
        /* fake reading STATUS OUT */
        dev->driver->ep_read(ep, 0, 0);
        dev->status.control_state = usbd_ctl_idle;
        usbd_process_callback(dev);
        return;
    default:
        /* unexpected RX packet */
        usbd_stall_pid(dev, ep);
        return;
    }
    /* usb request received. let's handle it */
    dev->status.data_ptr = req->data;
    dev->status.data_count = /*req->wLength;*/dev->status.data_maxsize;
    switch (usbd_process_request(dev, req)) {
    case usbd_ack:
        if (req->bmRequestType & USB_REQ_DEVTOHOST) {
            /* return data from function */
            if (dev->status.data_count >= req->wLength) {
                dev->status.data_count = req->wLength;
                dev->status.control_state = usbd_ctl_txdata;
            } else {
                /* DATA IN packet smaller than requested */
                /* ZLP maybe wanted */
                dev->status.control_state = usbd_ctl_ztxdata;
            }
            usbd_process_eptx(dev, ep | 0x80);
        } else {
            /* confirming by ZLP in STATUS_IN stage */
            dev->driver->ep_write(ep | 0x80, 0, 0);
            dev->status.control_state = usbd_ctl_statusin;
        }
        break;
    case usbd_nak:
        dev->status.control_state = usbd_ctl_statusin;
        break;
    default:
        usbd_stall_pid(dev, ep);
        break;
    }
}

/** \brief Control endpoint 0 event processing callback
 * \param dev usb device
 * \param event endpoint event
 */
static void usbd_process_ep0 (usbd_device *dev, uint8_t event, uint8_t ep) {
    switch (event) {
    case usbd_evt_epsetup:
        /* force switch to setup state */
        dev->status.control_state = usbd_ctl_idle;
        dev->complete_callback = 0;
    case usbd_evt_eprx:
        usbd_process_eprx(dev, ep);
        break;
    case usbd_evt_eptx:
        usbd_process_eptx(dev, ep);
        break;
    default:
        break;
    }
}


/** \brief General event processing callback
 * \param dev usb device
 * \param evt usb event
 * \param ep active endpoint
 */
static void usbd_process_evt(usbd_device *dev, uint8_t evt, uint8_t ep) {
    switch (evt) {
    case usbd_evt_reset:
        usbd_process_reset(dev);
        break;
    case usbd_evt_eprx:
    case usbd_evt_eptx:
    case usbd_evt_epsetup:
        if (dev->endpoint[ep & 0x07]) dev->endpoint[ep & 0x07](dev, evt, ep);
        break;
    default:
        break;
    }
    if (dev->events[evt]) dev->events[evt](dev, evt, ep);
}

 __attribute__((externally_visible)) void usbd_poll(usbd_device *dev) {
    dev->driver->poll(dev, usbd_process_evt);
}
