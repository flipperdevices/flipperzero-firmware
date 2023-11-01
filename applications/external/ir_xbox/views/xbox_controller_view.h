#pragma once

#include <gui/view.h>
#include <furi.h>
#include <gui/elements.h>
#include <xc_icons.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <infrared_transmit.h>

typedef struct XboxControllerView XboxControllerView;

XboxControllerView* xbox_controller_view_alloc();

void xbox_controller_view_free(XboxControllerView* xbox_controller_view);

View* xbox_controller_view_get_view(XboxControllerView* xbox_controller_view);

void xbox_controller_view_set_connected_status(
    XboxControllerView* xbox_controller_view,
    bool connected);
