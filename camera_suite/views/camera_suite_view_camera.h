#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_serial_control.h>
#include <furi_hal_serial.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <gui/icon_i.h>
#include <gui/modules/dialog_ex.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <storage/filesystem_api_defines.h>
#include <storage/storage.h>

#include "../helpers/camera_suite_custom_event.h"

#include <xtreme/xtreme.h>

#define UART_CH (xtreme_settings.uart_esp_channel)

#define BITMAP_HEADER_LENGTH 62
#define FRAME_BIT_DEPTH 1
#define FRAME_BUFFER_LENGTH 1024
#define FRAME_HEIGHT 64
#define FRAME_WIDTH 128
#define HEADER_LENGTH 3 // 'Y', ':', and row identifier
#define LAST_ROW_INDEX 1008
#define RING_BUFFER_LENGTH 19
#define ROW_BUFFER_LENGTH 16

static const unsigned char bitmap_header[BITMAP_HEADER_LENGTH] = {
    0x42, 0x4D, 0x3E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00};

typedef enum {
    WorkerEventReserved = (1 << 0), // Reserved for StreamBuffer internal event
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)

// Forward declaration
typedef void (*CameraSuiteViewCameraCallback)(CameraSuiteCustomEvent event, void* context);

typedef struct CameraSuiteViewCamera {
    CameraSuiteViewCameraCallback callback;
    FuriStreamBuffer* rx_stream;
    FuriHalSerialHandle* serial_handle;
    FuriThread* worker_thread;
    NotificationApp* notification;
    View* view;
    void* context;
} CameraSuiteViewCamera;

typedef struct UartDumpModel {
    bool is_dithering_enabled;
    bool is_initialized;
    bool is_inverted;
    int rotation_angle;
    uint32_t orientation;
    uint8_t pixels[FRAME_BUFFER_LENGTH];
    uint8_t ringbuffer_index;
    uint8_t row_identifier;
    uint8_t row_ringbuffer[RING_BUFFER_LENGTH];
} UartDumpModel;

// Function Prototypes
CameraSuiteViewCamera* camera_suite_view_camera_alloc();
View* camera_suite_view_camera_get_view(CameraSuiteViewCamera* camera_suite_static);
void camera_suite_view_camera_free(CameraSuiteViewCamera* camera_suite_static);
void camera_suite_view_camera_set_callback(
    CameraSuiteViewCamera* camera_suite_view_camera,
    CameraSuiteViewCameraCallback callback,
    void* context);
