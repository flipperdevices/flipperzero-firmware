#pragma once

#include <gui/view.h>
#include "../helpers/gb_cartridge_custom_event.h"

typedef struct GBCartridgeScene5 GBCartridgeScene5;

typedef void (*GBCartridgeScene5Callback)(GBCartridgeCustomEvent event, void* context);

void gb_cartridge_scene_5_set_callback(
    GBCartridgeScene5* instance,
    GBCartridgeScene5Callback callback,
    void* context);

GBCartridgeScene5* gb_cartridge_scene_5_alloc();

void gb_cartridge_scene_5_free(GBCartridgeScene5* gb_cartridge_static);

View* gb_cartridge_scene_5_get_view(GBCartridgeScene5* boilerpate_static);
