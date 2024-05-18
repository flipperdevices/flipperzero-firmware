#pragma once
#include <furi.h>
#include <gui/view.h>
#include "../helpers/gb_cartridge_custom_event.h"

typedef struct GBCartridgeScene1 GBCartridgeScene1;

typedef void (*GBCartridgeScene1Callback)(GBCartridgeCustomEvent event, void* context);

void gb_cartridge_scene_1_set_callback(
    GBCartridgeScene1* gb_cartridge_scene_1,
    GBCartridgeScene1Callback callback,
    void* context);

View* gb_cartridge_scene_1_get_view(GBCartridgeScene1* gb_cartridge_static);

GBCartridgeScene1* gb_cartridge_scene_1_alloc();

void gb_cartridge_scene_1_free(GBCartridgeScene1* gb_cartridge_static);