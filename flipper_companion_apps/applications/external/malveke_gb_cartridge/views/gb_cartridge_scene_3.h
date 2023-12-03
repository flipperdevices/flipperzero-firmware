#pragma once

#include <gui/view.h>
#include "../helpers/gb_cartridge_custom_event.h"

typedef struct GBCartridgeScene3 GBCartridgeScene3;

typedef void (*GBCartridgeScene3Callback)(GBCartridgeCustomEvent event, void* context);

void gb_cartridge_scene_3_set_callback(
    GBCartridgeScene3* instance,
    GBCartridgeScene3Callback callback,
    void* context);

GBCartridgeScene3* gb_cartridge_scene_3_alloc();

void gb_cartridge_scene_3_free(GBCartridgeScene3* gb_cartridge_static);

View* gb_cartridge_scene_3_get_view(GBCartridgeScene3* boilerpate_static);
