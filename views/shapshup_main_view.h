#pragma once

#include "../shapshup_files.h"
#include "../shapshup_custom_event.h"
#include <gui/view.h>
#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>

typedef void (*ShapShupMainViewCallback)(shapshupCustomEvent event, void* context);
typedef struct ShapShupMainView ShapShupMainView;

void shapshup_main_view_set_callback(
    ShapShupMainView* instance,
    ShapShupMainViewCallback callback,
    void* context);

ShapShupMainView* shapshup_main_view_alloc();
void shapshup_main_view_free(ShapShupMainView* instance);
View* shapshup_main_view_get_view(ShapShupMainView* instance);
ShapShupFileResults shapshup_main_view_load_file(ShapShupMainView* instance, const char* name);
bool shapshup_main_view_no_file(ShapShupMainView* instance);