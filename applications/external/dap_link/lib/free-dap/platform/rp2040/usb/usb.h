// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2016-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _USB_H_
#define _USB_H_

/*- Includes ----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "usb_descriptors.h"

/*- Prototypes --------------------------------------------------------------*/
void usb_hw_init(void);
void usb_attach(void);
void usb_detach(void);
void usb_configure_endpoint(usb_endpoint_descriptor_t *ep_desc);
bool usb_endpoint_configured(int ep, int dir);
int usb_endpoint_get_status(int ep, int dir);
void usb_endpoint_set_feature(int ep, int dir);
void usb_endpoint_clear_feature(int ep, int dir);
void usb_set_address(int address);
void usb_send(int ep, uint8_t *data, int size);
void usb_recv(int ep, uint8_t *data, int size);
void usb_control_send_zlp(void);
void usb_control_stall(void);
void usb_control_send(uint8_t *data, int size);
void usb_control_recv(void (*callback)(uint8_t *data, int size));
void usb_task(void);

void usb_configuration_callback(int config);

#endif // _USB_H_

