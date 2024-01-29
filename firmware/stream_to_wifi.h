#ifndef STREAM_TO_WIFI_H
#define STREAM_TO_WIFI_H

// #include <AsyncTCP.h>
// #include <DNSServer.h>

#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <WiFi.h>
#include <esp_camera.h>

#include "camera.h"
#include "camera_model.h"

#define MAX_HTML_SIZE 20000

/** Start the WiFi camera stream. */
void stream_to_wifi();

/** Start the WiFi server. */
void start_server();

/** Start the WiFi camera stream. */
void start_wifi_stream();

/** Stop the WiFi camera stream. */
void stop_wifi_stream();

#endif
