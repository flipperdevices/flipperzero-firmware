#pragma once

#include <stdint.h>
#include <gui/view.h>
#include <gui/modules/widget.h>
#include <furi.h>
#include <locale/locale.h>
#include <furi_hal.h>
#include "../ublox_device.h"
#include "../helpers/ublox_types.h"

typedef enum {
    DataDisplayHandheldMode,
    DataDisplayCarMode,
    DataDisplayGPSNotFound,
} DataDisplayState;

typedef struct DataDisplayView DataDisplayView;

typedef void (*DataDisplayViewCallback)(void* context, InputKey key);

DataDisplayView* data_display_alloc();

void data_display_free(DataDisplayView* data_display);

void data_display_reset(DataDisplayView* data_display);

View* data_display_get_view(DataDisplayView* data_display);

void data_display_set_callback(
    DataDisplayView* data_display,
    DataDisplayViewCallback callback,
    void* context);

void data_display_set_nav_messages(
    DataDisplayView* data_display,
    Ublox_NAV_PVT_Message pvt_message,
    Ublox_NAV_ODO_Message odo_message);

void data_display_set_state(DataDisplayView* data_display, DataDisplayState state);

void data_display_set_log_state(DataDisplayView* data_display, UbloxLogState log_state);

DataDisplayState data_display_get_state(DataDisplayView* data_display);
