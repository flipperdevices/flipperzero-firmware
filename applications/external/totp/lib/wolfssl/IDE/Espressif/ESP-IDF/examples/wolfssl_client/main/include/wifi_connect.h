/* user_settings.h
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
#ifndef _TLS_WIFI_H_
#define _TLS_WIFI_H_

#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_wifi.h"
#if ESP_IDF_VERSION_MAJOR >= 4
    #include "esp_event.h"
#else
    #include "esp_event_loop.h"
#endif

#define DEFAULT_PORT                     11111

#define TLS_SMP_CLIENT_TASK_NAME         "tls_client_example"
#define TLS_SMP_CLIENT_TASK_WORDS        10240
#define TLS_SMP_CLIENT_TASK_PRIORITY     8

#define TLS_SMP_WIFI_SSID                CONFIG_WIFI_SSID
#define TLS_SMP_WIFI_PASS                CONFIG_WIFI_PASSWORD
#define TLS_SMP_TARGET_HOST              CONFIG_TARGET_HOST

#endif
