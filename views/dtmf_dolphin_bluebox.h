#pragma once

#include <gui/view.h>
#include "../dtmf_dolphin_tone.h"

typedef struct DTMFDolphinBluebox DTMFDolphinBluebox;
typedef void (*DTMFDolphinBlueboxOkCallback)(InputType type, void* context);

DTMFDolphinBluebox* dtmf_dolphin_bluebox_alloc();

void dtmf_dolphin_bluebox_free(DTMFDolphinBluebox* dtmf_dolphin_bluebox);

View* dtmf_dolphin_bluebox_get_view(DTMFDolphinBluebox* dtmf_dolphin_bluebox);

void dtmf_dolphin_bluebox_set_ok_callback(DTMFDolphinBluebox* dtmf_dolphin_bluebox, DTMFDolphinBlueboxOkCallback callback, void* context);
