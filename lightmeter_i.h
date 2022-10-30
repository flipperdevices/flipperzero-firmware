#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include "lightmeterI2C.h"
#include "views/main_view.h"

typedef struct {
    ViewPort* view_port;

    lightmeterMainView* main_view;
    // lightmeterConfigView* config_view;

    lightmeterI2CSender* sender;
} LightMeter;
