#ifndef SCENE_SERIAL_HEADERS
#define SCENE_SERIAL_HEADERS

#include "flipper.h"
#include "usbuart/usb_uart_bridge.h"

typedef enum {
    SerialCustomEventTextUpdate,
} SerialCustomEvent;

void fcom_serial_scene_on_enter(void* context);
bool fcom_serial_scene_on_event(void* context, SceneManagerEvent event);
void fcom_serial_scene_on_exit(void* context);

#endif