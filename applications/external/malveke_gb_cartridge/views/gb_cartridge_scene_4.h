#pragma once

#include <gui/view.h>
#include "../helpers/gb_cartridge_custom_event.h"

typedef struct GBCartridgeScene4 GBCartridgeScene4;

typedef void (*GBCartridgeScene4Callback)(GBCartridgeCustomEvent event, void* context);

void gb_cartridge_scene_4_set_callback(
    GBCartridgeScene4* instance,
    GBCartridgeScene4Callback callback,
    void* context);

GBCartridgeScene4* gb_cartridge_scene_4_alloc();

void gb_cartridge_scene_4_free(GBCartridgeScene4* gb_cartridge_static);

View* gb_cartridge_scene_4_get_view(GBCartridgeScene4* boilerpate_static);
