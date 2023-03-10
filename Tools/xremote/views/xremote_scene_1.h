#pragma once

#include <gui/view.h>
#include "../helpers/xremote_custom_event.h"

typedef struct XRemoteScene1 XRemoteScene1;

typedef void (*XRemoteScene1Callback)(XRemoteCustomEvent event, void* context);

void xremote_scene_1_set_callback(
    XRemoteScene1* xremote_scene_1,
    XRemoteScene1Callback callback,
    void* context);

View* xremote_scene_1_get_view(XRemoteScene1* xremote_static);

XRemoteScene1* xremote_scene_1_alloc();

void xremote_scene_1_free(XRemoteScene1* xremote_static);