#ifndef UTIL_H
#define UTIL_H

#include <dialogs/dialogs.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdlib.h>

void utilShowDialog(
    const char* paramTitle,
    const char* paramMessage,
    const char* paramConfirmButtonText);

#endif
