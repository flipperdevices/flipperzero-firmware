#pragma once

#include <furi.h>
#include <furi_hal.h>

#include "keyboard.h"

// How long to wait after pressing the button before performing next action.
#define PRESS_DELAY_MS 20

// How long to wait after releasing the button before performing next action.
#define RELEASE_DELAY_MS 5

struct FlipboardKeyboard {
    FuriHalUsbInterface* usb_previous;
    bool attached;
};
