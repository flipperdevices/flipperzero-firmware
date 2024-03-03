#pragma once

typedef void (*CameraSuiteViewWiFiCameraCallback)(CameraSuiteCustomEvent event, void* context);

typedef struct CameraSuiteViewWiFiCamera {
    CameraSuiteViewWiFiCameraCallback callback;
    View* view;
    void* context;
} CameraSuiteViewWiFiCamera;

typedef struct UartWiFiModel {
    FuriString* log;
    size_t log_strlen;
} UartWiFiModel;

void camera_suite_view_wifi_camera_set_callback(
    CameraSuiteViewWiFiCamera* wifi_view_instance,
    CameraSuiteViewWiFiCameraCallback callback,
    void* context);

CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc();

void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* wifi_view_instance);

View* camera_suite_view_wifi_camera_get_view(CameraSuiteViewWiFiCamera* wifi_view_instance);
