#pragma once

#include <gui/view.h>
#include "../helpers/boilerplate_custom_event.h"

typedef struct BoilerplateStartscreen BoilerplateStartscreen;

typedef void (*BoilerplateStartscreenCallback)(BoilerplateCustomEvent event, void* context);

void boilerplate_startscreen_set_callback(
    BoilerplateStartscreen* boilerplate_startscreen,
    BoilerplateStartscreenCallback callback,
    void* context);

View* boilerplate_startscreen_get_view(BoilerplateStartscreen* boilerplate_static);

BoilerplateStartscreen* boilerplate_startscreen_alloc();

void boilerplate_startscreen_free(BoilerplateStartscreen* boilerplate_static);