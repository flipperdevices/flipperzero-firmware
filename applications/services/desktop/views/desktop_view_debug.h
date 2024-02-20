#pragma once

#include <stdint.h>
#include <gui/view.h>
#include "desktop_events.h"

typedef struct DesktopDebugView DesktopDebugView;

typedef void (*DesktopDebugViewCallback)(DesktopEvent event, void* context);

struct DesktopDebugView {
    View* view;
    FuriTimer* timer;
    DesktopDebugViewCallback callback;
    void* context;
};
