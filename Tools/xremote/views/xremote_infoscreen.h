#pragma once

#include <gui/view.h>
#include "../helpers/xremote_custom_event.h"

typedef struct XRemoteInfoscreen XRemoteInfoscreen;

typedef void (*XRemoteInfoscreenCallback)(XRemoteCustomEvent event, void* context);

void xremote_infoscreen_set_callback(
    XRemoteInfoscreen* xremote_infoscreen,
    XRemoteInfoscreenCallback callback,
    void* context);

View* xremote_infoscreen_get_view(XRemoteInfoscreen* xremote_static);

XRemoteInfoscreen* xremote_infoscreen_alloc();

void xremote_infoscreen_free(XRemoteInfoscreen* xremote_static);