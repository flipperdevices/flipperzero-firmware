#pragma once

#include "speaker.h"
#include <furi.h>
#include <furi_hal.h>

struct Speaker {
    FuriThread* thread;
    bool is_running;
    float frequency;
    float volume;
};