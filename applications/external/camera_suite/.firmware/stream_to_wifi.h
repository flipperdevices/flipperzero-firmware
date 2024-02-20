#ifndef STREAM_TO_WIFI_H
#define STREAM_TO_WIFI_H

// #include "ESPAsyncWebServer.h"
// #include <AsyncTCP.h>
// #include <DNSServer.h>

#include <FS.h>
#include <WiFi.h>
#include <esp_camera.h>

#include "camera.h"
#include "camera_model.h"
#include "dither_image.h"

/** Start the WiFi camera stream. */
void stream_to_wifi();

/** Start the WiFi camera stream. */
void start_wifi_stream();

/** Start the WiFi server. */
void start_server();

/** Stop the WiFi camera stream. */
void stop_wifi_stream();

#endif
