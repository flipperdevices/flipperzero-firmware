#pragma once

#include "esp_flasher_app_i.h"
#include "esp_flasher_uart.h"
#ifndef SERIAL_FLASHER_INTERFACE_UART
#define SERIAL_FLASHER_INTERFACE_UART /* TODO why is application.fam not passing this via cdefines */
#endif
#define SERIAL_FLASHER_RESET_HOLD_TIME_MS 100
#define SERIAL_FLASHER_BOOT_HOLD_TIME_MS 50
#include "esp_loader_io.h"

#define ESP_ADDR_BOOT_S3 0x0
#define ESP_ADDR_BOOT 0x1000
#define ESP_ADDR_PART 0x8000
#define ESP_ADDR_NVS 0x9000
#define ESP_ADDR_BOOT_APP0 0xE000
#define ESP_ADDR_APP_A 0x10000
#define ESP_ADDR_APP_B 0x150000

#define ESP_ADDR_OTADATA_OFFSET_APP_A 0x0
#define ESP_ADDR_OTADATA_OFFSET_APP_B 0x1000

void esp_flasher_worker_start_thread(EspFlasherApp* app);
void esp_flasher_worker_stop_thread(EspFlasherApp* app);
void esp_flasher_worker_handle_rx_data_cb(uint8_t* buf, size_t len, void* context);