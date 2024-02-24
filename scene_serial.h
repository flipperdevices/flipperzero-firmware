/*
Starts a USB Serial CDC device on the flipper's USB port
and then binds it to the dmcomm serial streams.

This turns the flipper basically into an A-Com
*/
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