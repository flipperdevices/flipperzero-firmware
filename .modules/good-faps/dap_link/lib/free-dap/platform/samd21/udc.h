/*
 * Copyright (c) 2016, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UDC_H_
#define _UDC_H_

/*- Includes ----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "usb.h"

/*- Definitions -------------------------------------------------------------*/
#define UDC_EPT_NUM    8

/*- Prototypes --------------------------------------------------------------*/
void udc_init(void);
void udc_attach(void);
void udc_detach(void);
void udc_reset_endpoint(int ep, int dir);
void udc_configure_endpoint(usb_endpoint_descriptor_t *ep_desc);
bool udc_endpoint_configured(int ep, int dir);
int udc_endpoint_get_status(int ep, int dir);
void udc_endpoint_set_feature(int ep, int dir);
void udc_endpoint_clear_feature(int ep, int dir);
void udc_set_address(int address);
void udc_send(int ep, uint8_t *data, int size);
void udc_recv(int ep, uint8_t *data, int size);
void udc_control_send_zlp(void);
void udc_control_stall(void);
void udc_control_send(uint8_t *data, int size);

void udc_send_callback(int ep);
void udc_recv_callback(int ep);

#endif // _UDC_H_

