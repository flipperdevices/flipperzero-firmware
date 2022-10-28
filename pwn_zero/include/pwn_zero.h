#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include "message_queue.h"
#include "constants.h"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* viewDispatcher;
    View* view;
    FuriThread* workerThread;
    FuriStreamBuffer* rxStream;
} PwnZeroApp;

