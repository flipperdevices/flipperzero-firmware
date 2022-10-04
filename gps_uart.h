#pragma once

#include <furi_hal.h>
#include <stream_buffer.h>

#define GPS_BAUDRATE 9600
#define RX_BUF_SIZE 1024

typedef struct {
    float latitude;
    float longitude;
} GpsStatus;

typedef struct {
    FuriThread* thread;
    StreamBufferHandle_t rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE];

    GpsStatus status;
} GpsUart;

GpsUart* gps_uart_enable();

void gps_uart_disable(GpsUart* gps_uart);
