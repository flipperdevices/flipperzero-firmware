#pragma once

#include "../xremote.h"
//#include <gui/view.h>
#include "../helpers/xremote_custom_event.h"

typedef struct XRemotePauseSet XRemotePauseSet;

typedef void (*XRemotePauseSetCallback)(XRemoteCustomEvent event, void* context);

void xremote_pause_set_set_callback(
    XRemotePauseSet* instance,
    XRemotePauseSetCallback callback,
    void* context);

XRemotePauseSet* xremote_pause_set_alloc();

void xremote_pause_set_enter(void* context);

View* xremote_pause_set_get_view(XRemotePauseSet* instance);