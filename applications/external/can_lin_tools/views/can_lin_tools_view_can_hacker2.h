#pragma once

#include <gui/view.h>
#include "../helpers/can_lin_tools_types.h"
#include "../helpers/can_lin_tools_event.h"

typedef struct CanLinToolsCanHacker2View CanLinToolsCanHacker2View;

typedef void (*CanLinToolsCanHacker2ViewCallback)(CanLinToolsCustomEvent event, void* context);

void can_lin_tools_view_can_hacker2_set_callback(
    CanLinToolsCanHacker2View* instance,
    CanLinToolsCanHacker2ViewCallback callback,
    void* context);

CanLinToolsCanHacker2View* can_lin_tools_view_can_hacker2_alloc();

void can_lin_tools_view_can_hacker2_free(CanLinToolsCanHacker2View* instance);

View* can_lin_tools_view_can_hacker2_get_view(CanLinToolsCanHacker2View* instance);
