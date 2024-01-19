#pragma once

#include <furi_hal.h>
#include <notification/notification_messages.h>

#include <xtreme/xtreme.h>

#define UART_CH (xtreme_settings.uart_nmea_channel)

#define RX_BUF_SIZE 1024

static const int gps_baudrates[6] = {4800, 9600, 19200, 38400, 57600, 115200};
static int current_gps_baudrate = 1;

typedef struct {
    bool valid;
    float latitude;
    float longitude;
    float speed;
    float course;
    float altitude;
    char altitude_units;
    int fix_quality;
    int satellites_tracked;
    int time_hours;
    int time_minutes;
    int time_seconds;
} GpsStatus;

typedef enum { KNOTS, KPH, MPH, INVALID } SpeedUnit;

typedef enum {
    CHANGE_BAUDRATE,
    CHANGE_BACKLIGHT,
    CHANGE_DEEPSLEEP,
    CHANGE_SPEEDUNIT,
    NORMAL
} ViewState;

typedef struct {
    FuriMutex* mutex;
    FuriThread* thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE];
    FuriHalSerialHandle* serial_handle;

    NotificationApp* notifications;
    uint32_t baudrate;
    bool backlight_enabled;
    bool deep_sleep_enabled;
    SpeedUnit speed_units;
    ViewState view_state;

    GpsStatus status;
} GpsUart;

void gps_uart_init_thread(GpsUart* gps_uart);
void gps_uart_deinit_thread(GpsUart* gps_uart);

GpsUart* gps_uart_enable();

void gps_uart_disable(GpsUart* gps_uart);
