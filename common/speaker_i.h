#pragma once

#include "speaker.h"
#include <furi.h>
#include <furi_hal.h>

struct Speaker {
    // The thread that runs the speaker worker
    FuriThread* thread;

    // True is the thread is running
    bool is_running;

    // Frequency of the tone to play (in Hz)
    float frequency;

    // Volume of the tone to play (0.0 - 1.0)
    float volume;
};