#pragma once

#include <furi_hal.h>
#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#include "../helpers/camera_suite_haptic.h"
#include "../helpers/camera_suite_led.h"
#include "../helpers/camera_suite_speaker.h"
#include "../helpers/camera_suite_custom_event.h"

typedef void (*CameraSuiteViewWiFiCameraCallback)(CameraSuiteCustomEvent event, void* context);

typedef struct CameraSuiteViewWiFiCamera {
    View* view;
    CameraSuiteViewCameraCallback callback;
    void* context;
    FuriHalSerialHandle* wifi_serial_handle;
    FuriStreamBuffer* wifi_rx_stream;
    FuriThread* wifi_worker_thread;
} CameraSuiteViewWiFiCamera;

typedef struct {
    FuriString* log;
    size_t log_strlen;
} CameraSuiteViewWiFiCameraModel;

// Function Prototypes
CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc();
void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* wifi_view_instance);
View* camera_suite_view_wifi_camera_get_view(CameraSuiteViewWiFiCamera* wifi_view_instance);
void camera_suite_view_wifi_camera_set_callback(
    CameraSuiteViewWiFiCamera* wifi_view_instance,
    CameraSuiteViewWiFiCameraCallback callback,
    void* context);
