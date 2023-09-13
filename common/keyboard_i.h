#pragma once

#include <furi.h>
#include <furi_hal.h>

#include "keyboard.h"

struct FlipboardKeyboard {
    FuriHalUsbInterface* usb_previous;
    bool attached;
};
