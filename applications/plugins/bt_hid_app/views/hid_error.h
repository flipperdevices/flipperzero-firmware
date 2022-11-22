#pragma once

#include <gui/modules/popup.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include "../views.h"

typedef struct HidError HidError;

HidError* hid_error_alloc();

void hid_error_free(HidError* hid_error);

View* hid_error_get_view(HidError* hid_error);
