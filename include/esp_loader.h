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

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes
 */
typedef enum
{
    ESP_LOADER_SUCCESS,                /*!< Success */
    ESP_LOADER_ERROR_FAIL,             /*!< Unspecified error */
    ESP_LOADER_ERROR_TIMEOUT,          /*!< Timeout elapsed */
    ESP_LOADER_ERROR_INVALID_MD5,      /*!< Computed and receied MD5 does not match */
    ESP_LOADER_ERROR_INVALID_RESPONSE  /*!< Internal error */
} esp_loader_error_t;

/**
 * @brief Connection arguments
 */
typedef struct 
{
  uint32_t sync_timeout;  /*!< Maximum time to wait for response from serial interface. */
  int32_t trials;         /*!< Number of trials to connect to target. If greater than 1,
                               100 millisecond delay is inserted after each try. */
} esp_loader_connect_args_t;

#define ESP_LOADER_CONNECT_DEFAULT() { \
  .sync_timeout = 100,  \
  .trials = 10  \
}

/**
  * @brief Connects to the target
  *
  * @param connect_args[in] Timing parameters to be used for connecting to target.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_connect(esp_loader_connect_args_t *connect_args);

/**
  * @brief Initiates flash operation
  *
  * @param offset[in]       Address from which flash operation will be performed.
  * @param image_size[in]   Size of the whole binary to be loaded into flash.
  * @param block_size[in]   Size of buffer used in subsequent calls to esp_loader_flash_write.
  *
  * @note  image_size is size of the whole image, whereas, block_size is chunk of data sent 
  *        to the target, each time esp_loader_flash_write function is called.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_flash_start(uint32_t offset, uint32_t image_size, uint32_t block_size);

/**
  * @brief Writes supplied data to target's flash memory.
  *
  * @param payload[in]      Data to be flashed into target's memory.
  * @param size[in]         Size of payload in bytes.
  *
  * @note  size must not be greater that block_size supplied to previously called
  *        esp_loader_flash_start function. If size is less than block_size, 
  *        remaining bytes of payload buffer will be padded with 0xff.
  *        Therefore, size of payload buffer has to be equal or greater than block_size.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_flash_write(void *payload, uint32_t size);

/**
  * @brief Ends flash operation.
  *
  * @param reboot[in]       reboot the target if true.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_flash_finish(bool reboot);

/**
  * @brief Writes register.
  *
  * @param address[in]      Address of register.
  * @param reg_value[in]    New register value.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_write_register(uint32_t address, uint32_t reg_value);

/**
  * @brief Reads register.
  *
  * @param address[in]      Address of register.
  * @param reg_value[out]   Register value.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_read_register(uint32_t address, uint32_t *reg_value);

/**
  * @brief Change baud rate.
  *
  * @note  Baud rate has to be also adjusted accordingly on host MCU, as
  *        target's baud rate is changed upon return from this function.
  *
  * @param baudrate[in]     new baud rate to be set.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_change_baudrate(uint32_t baudrate);

/**
  * @brief Verify target's flash integrity by checking MD5.
  *        MD5 checksum is computed from data pushed to target's memory by calling
  *        esp_loader_flash_write() function and compared against target's MD5.
  *        Target computes checksum based on offset and image_size passed to 
  *        esp_loader_flash_start() function.
  *
  * @note  This function is only available if MD5_ENABLED is set.
  *
  * @return
  *     - ESP_LOADER_SUCCESS Success
  *     - ESP_LOADER_ERROR_INVALID_MD5 MD5 does not match
  *     - ESP_LOADER_ERROR_TIMEOUT Timeout
  *     - ESP_LOADER_ERROR_INVALID_RESPONSE Internal error
  */
esp_loader_error_t esp_loader_flash_verify(void);

/**
  * @brief Toggles reset pin.
  */
void esp_loader_reset_target(void);



#ifdef __cplusplus
}
#endif