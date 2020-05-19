/* Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "loader_config_user.h"

// MD5 checksum can be used to check, if new image was flashed successfully.
// When enabled, esp_loader_flash_verify() function can be called to to verify
// flash integrity. In case verification is unnecessary, this option can be 
// disabled in order to reduce code size.
#if !defined MD5_ENABLED
#define MD5_ENABLED  1
#endif

#if !defined TARGET_ESP32_S2 && !defined TARGET_ESP8266
#define TARGET_ESP32 1
#endif

#if MD5_ENABLED && defined TARGET_ESP8266
#error "ESP8266 ROM loader does not support MD5 check"
#endif

#ifdef __cplusplus
}
#endif