#pragma once

#include <furi.h>

#define LIN_DATA_LENGTH 8

typedef struct LinBus LinBus;

typedef enum { LinBusModeMaster, LinBusModeSlave } LinBusMode;
typedef enum { LinBusChecksumTypeClassic, LinBusChecksumTypeEnanced } LinBusChecksumType;
typedef enum { LinBusMasterRequest, LinBusMasterResponse, LinBusSlaveResponse } LinBusFrameType;

typedef struct {
    uint8_t id;
    uint8_t data[LIN_DATA_LENGTH];
    uint8_t length;
    uint8_t crc;
    LinBusChecksumType crc_type;
    LinBusFrameType frame_type;
    uint8_t response_length;
} LinBusFrame;

LinBus* lin_bus_init(LinBusMode mode, uint32_t baudrate);
void lin_bus_deinit(LinBus* instance);
bool lin_bus_tx_async(LinBus* instance, LinBusFrame* lin_frame);
size_t lin_bus_get_rx_frame_available(LinBus* instance);
LinBusFrame lin_bus_get_rx_frame_read(LinBus* instance);
bool lin_bus_slave_mode_add_or_update_response_id(LinBus* instance, LinBusFrame* frame);

void lin_bus_error_callback(void* context);
void lin_bus_break_callback(void* context);
void lin_uart_rx_callback(uint8_t data, void* context);
void lin_uart_tx_callback(void* context);
void lin_bus_timeout_callback(void* context);

void print_buf(void* context);
