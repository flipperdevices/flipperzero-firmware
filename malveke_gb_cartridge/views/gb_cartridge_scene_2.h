#pragma once

#include <gui/view.h>
#include "../helpers/gb_cartridge_custom_event.h"

typedef struct GBCartridgeScene2 GBCartridgeScene2;

typedef void (*GBCartridgeScene2Callback)(GBCartridgeCustomEvent event, void* context);

void gb_cartridge_scene_2_set_callback(
    GBCartridgeScene2* instance,
    GBCartridgeScene2Callback callback,
    void* context);

GBCartridgeScene2* gb_cartridge_scene_2_alloc();

void gb_cartridge_scene_2_free(GBCartridgeScene2* gb_cartridge_static);

View* gb_cartridge_scene_2_get_view(GBCartridgeScene2* boilerpate_static);
