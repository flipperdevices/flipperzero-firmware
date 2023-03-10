#pragma once

#include <gui/view.h>
#include "../helpers/xremote_custom_event.h"

typedef struct XRemoteScene2 XRemoteScene2;

typedef void (*XRemoteScene2Callback)(XRemoteCustomEvent event, void* context);

void xremote_scene_2_set_callback(
    XRemoteScene2* instance,
    XRemoteScene2Callback callback,
    void * context);

XRemoteScene2* xremote_scene_2_alloc();

void xremote_scene_2_free(XRemoteScene2* xremote_static);

View* xremote_scene_2_get_view(XRemoteScene2* boilerpate_static);
