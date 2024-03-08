#pragma once

#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#define FLIPPER_SCREEN_HEIGHT 64
#define FLIPPER_SCREEN_WIDTH 128

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)
#define UART_CH (FuriHalSerialIdUsart)

typedef void (*CameraSuiteViewCameraCallback)(CameraSuiteCustomEvent event, void* context);

typedef enum {
    // Reserved for StreamBuffer internal event
    WorkerEventReserved = (1 << 0),
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

typedef struct CameraSuiteViewCamera {
    CameraSuiteViewCameraCallback callback;
    FuriHalSerialHandle* serial_handle;
    FuriStreamBuffer* rx_stream;
    FuriThread* worker_thread;
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
    FuriString* log;
    size_t log_strlen;
} UartDumpModel;

CameraSuiteViewCamera* camera_suite_view_camera_alloc();
View* camera_suite_view_camera_get_view(CameraSuiteViewCamera* camera_suite_static);
void camera_suite_view_camera_free(CameraSuiteViewCamera* camera_suite_static);
void camera_suite_view_camera_set_callback(
    CameraSuiteViewCamera* camera_suite_view_camera,
    CameraSuiteViewCameraCallback callback,
    void* context);
