#pragma once

#include <gui/view.h>
#include "../helpers/can_lin_tools_types.h"
#include "../helpers/can_lin_tools_event.h"

typedef struct CanLinToolsLinHacker CanLinToolsLinHacker;

typedef void (*CanLinToolsLinHackerViewCallback)(CanLinToolsCustomEvent event, void* context);

typedef enum {
    CanLinToolsLinHackerViewStatusNoUSBConnect,
    CanLinToolsLinHackerViewStatusUSBConnect,
} CanLinToolsLinHackerViewStatus;

CanLinToolsLinHacker* can_lin_tools_view_lin_hacker_alloc();

void can_lin_tools_view_lin_hacker_free(CanLinToolsLinHacker* instance);

View* can_lin_tools_view_lin_hacker_get_view(CanLinToolsLinHacker* instance);
