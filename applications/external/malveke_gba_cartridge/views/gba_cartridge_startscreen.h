#pragma once

#include <gui/view.h>
#include "../helpers/gba_cartridge_custom_event.h"

// #include <assets_icons.h>
#include <malveke_gba_cartridge_icons.h>

typedef struct GBACartridgeStartscreen GBACartridgeStartscreen;

typedef void (*GBACartridgeStartscreenCallback)(GBACartridgeCustomEvent event, void* context);

void gba_cartridge_startscreen_set_callback(
    GBACartridgeStartscreen* gba_cartridge_startscreen,
    GBACartridgeStartscreenCallback callback,
    void* context);

View* gba_cartridge_startscreen_get_view(GBACartridgeStartscreen* gba_cartridge_static);

GBACartridgeStartscreen* gba_cartridge_startscreen_alloc();

void gba_cartridge_startscreen_free(GBACartridgeStartscreen* gba_cartridge_static);