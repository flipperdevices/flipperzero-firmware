#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include <assets_icons.h>
#include <toolbox/stream/file_stream.h>
#include <xtreme/xtreme.h>

#include "helpers/minmea.h"
#include "wardriver_icons.h"

#define appname "ll-wardriver"

#define RX_BUF_SIZE 2048
#define MAX_ACCESS_POINTS 2048

#define MAX_SSID_LENGTH 32
#define MAX_BSSID_LENGTH 18

#define FILE_PATH EXT_PATH("apps_data/ll-wardriver")

typedef enum {
    EventTypeKey,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

typedef enum { SHOW_NMEA, NORMAL, NO_APS } ViewState;

typedef struct {
    char* recievedMac;
    char* sentMac;
} Packet;

typedef struct {
    char* ssid;
    char* bssid;
    int8_t rssi;
    uint8_t channel;
    FuriHalRtcDateTime datetime;
    uint16_t packetRxCount;
    uint16_t packetTxCount;
    float latitude;
    float longitude;
} AccessPoint;

typedef struct {
    float latitude;
    float longitude;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t satelites;
} GpsData;

typedef struct {
    FuriMessageQueue* queue;
    FuriMutex* mutex;
    FuriString* buffer;

    FuriThread* thread_esp;
    FuriStreamBuffer* rx_stream_esp;
    uint8_t rx_buf_esp[2048];
    FuriHalSerialHandle* serial_handle_esp;

    FuriThread* thread_gps;
    FuriStreamBuffer* rx_stream_gps;
    uint8_t rx_buf_gps[2048];
    FuriHalSerialHandle* serial_handle_gps;

    uint16_t access_points_count;
    AccessPoint access_points[MAX_ACCESS_POINTS];
    int16_t access_points_index;
    AccessPoint active_access_point;
    bool extra_info;
    bool pressedButton;

    ViewState view_state;
    GpsData gps_data;
} Context;