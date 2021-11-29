#pragma once

#include "serial_service.h"

#define FURI_HAL_BT_SERIAL_PACKET_SIZE_MAX SERIAL_SVC_DATA_LEN_MAX

typedef SerialServiceEventCallback FuriHalBtSerialCallback;

void furi_hal_bt_serial_start();

void furi_hal_bt_serial_stop();

void furi_hal_bt_serial_set_event_callback(uint16_t buff_size, FuriHalBtSerialCallback callback, void* context);

void furi_hal_bt_serial_notify_buffer_is_empty();

/** Send data through BLE
 * @param data - data buffer
 * @param size - data buffer size
 */
bool furi_hal_bt_serial_tx(uint8_t* data, uint16_t size);
