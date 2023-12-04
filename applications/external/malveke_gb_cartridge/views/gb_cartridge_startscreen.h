#pragma once

#include <gui/view.h>
#include "../helpers/gb_cartridge_custom_event.h"

// #include <assets_icons.h>
#include <malveke_gb_cartridge_icons.h>

typedef struct GBCartridgeStartscreen GBCartridgeStartscreen;

typedef void (*GBCartridgeStartscreenCallback)(GBCartridgeCustomEvent event, void* context);

void gb_cartridge_startscreen_set_callback(
    GBCartridgeStartscreen* gb_cartridge_startscreen,
    GBCartridgeStartscreenCallback callback,
    void* context);

View* gb_cartridge_startscreen_get_view(GBCartridgeStartscreen* gb_cartridge_static);

GBCartridgeStartscreen* gb_cartridge_startscreen_alloc();

void gb_cartridge_startscreen_free(GBCartridgeStartscreen* gb_cartridge_static);