#pragma once
#include <furi.h>
#include <gui/view.h>
#include "../helpers/gba_cartridge_custom_event.h"

typedef struct GBACartridgeScene1 GBACartridgeScene1;

typedef void (*GBACartridgeScene1Callback)(GBACartridgeCustomEvent event, void* context);

void gba_cartridge_scene_1_set_callback(
    GBACartridgeScene1* gba_cartridge_scene_1,
    GBACartridgeScene1Callback callback,
    void* context);

View* gba_cartridge_scene_1_get_view(GBACartridgeScene1* gba_cartridge_static);

GBACartridgeScene1* gba_cartridge_scene_1_alloc();

void gba_cartridge_scene_1_free(GBACartridgeScene1* gba_cartridge_static);