#pragma once
#include <stdint.h>

#define DTMF_DOLPHIN_BLUEBOX_TONE_COUNT 16

typedef struct {
    const char *name;
    const float frequency_1;
    const float frequency_2;
} DTMFDolphinBlueboxTones;

static const DTMFDolphinBlueboxTones dtmf_dolphin_bluebox_tone[DTMF_DOLPHIN_BLUEBOX_TONE_COUNT] = {
    {"2600 Hz", 2600.0, 0.0},
    {"1", 700.0, 900.0},
    {"2", 700.0, 1100.0},
    {"3", 900.0, 1100.0},
    {"4", 700.0, 1300.0},
    {"5", 900.0, 1300.0},
    {"6", 1100.0, 1300.0},
    {"7", 700.0, 1500.0},
    {"8", 900.0, 1500.0},
    {"9", 1100.0, 1500.0},
    {"0", 1300.0, 1500.0},
    {"Key Pulse (KP)", 1100.0, 1700.0},
    {"Start (ST)", 1500.0, 1700.0},
    {"CCITT 11", 700.0, 1700.0},
    {"CCITT 12", 900.0, 1700.0},
    {"CCITT KP2", 1300.0, 1700.0},
};
