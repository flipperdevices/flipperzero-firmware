/* freertos_tcp_port.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include "bsp_api.h"
#include "FreeRTOS.h"
#include "FreeRTOS_sockets.h"
#include "FreeRTOS_IP.h"
#include <stdlib.h>

#define HOSTNAME "wolfssl_ra6m3g"

/*
 * Contains callback functions for FreeRTOS+TCP
 * The following implementations are NOT SUITABLE FOR PRODUCTION.
 * They currently serve to build the projects and are not secure.
 */

uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress,
                                            uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress,
                                            uint16_t usDestinationPort);

const char *pcApplicationHostnameHook(void) {
    return HOSTNAME;
}

uint32_t ulRand(void) {
    return (uint32_t) rand();
}

/* Disabled in RA Configuration. Re-Enable within FreeRTOS+TCP Stack Properties */
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent) {
    (void) eNetworkEvent;
}

uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress,
                                            uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress,
                                            uint16_t usDestinationPort)
{
    (void) ulSourceAddress;
    (void) usSourcePort;
    (void) ulDestinationAddress;
    (void) usDestinationPort;

    return ulRand();
}
