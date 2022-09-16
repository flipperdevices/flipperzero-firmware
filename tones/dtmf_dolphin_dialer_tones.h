#pragma once
#include <stdint.h>

#define DTMF_DOLPHIN_DIALER_TONE_COUNT 16

typedef struct DTMFDolphinDialerTonePos {
    const uint8_t row;
    const uint8_t col;
    const uint8_t span;
} DTMFDolphinDialerTonePos;

typedef struct {
    const char *name;
    const float frequency_1;
    const float frequency_2;
    const struct DTMFDolphinDialerTonePos pos;
} DTMFDolphinDialerTones;

/*  Via https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling
        1209 Hz 	1336 Hz 	1477 Hz 	1633 Hz
697 Hz 	1 	        2 	        3 	        A
770 Hz 	4 	        5 	        6 	        B
852 Hz 	7 	        8 	        9 	        C
941 Hz 	* 	        0 	        # 	        D */

static const DTMFDolphinDialerTones dtmf_dolphin_dialer_tone[DTMF_DOLPHIN_DIALER_TONE_COUNT] = {
    {"1", 697.0, 1209.0, {0, 0, 1}},
    {"2", 697.0, 1336.0, {0, 1, 1}},
    {"3", 697.0, 1477.0, {0, 2, 1}},
    {"A", 697.0, 1633.0, {0, 3, 1}},
    {"4", 770.0, 1209.0, {1, 0, 1}},
    {"5", 770.0, 1336.0, {1, 1, 1}},
    {"6", 770.0, 1477.0, {1, 2, 1}},
    {"B", 770.0, 1633.0, {1, 3, 1}},
    {"7", 852.0, 1209.0, {2, 0, 1}},
    {"8", 852.0, 1336.0, {2, 1, 1}},
    {"9", 852.0, 1477.0, {2, 2, 1}},
    {"C", 852.0, 1633.0, {2, 3, 1}},
    {"*", 941.0, 1209.0, {3, 0, 1}},
    {"0", 941.0, 1336.0, {3, 1, 1}},
    {"#", 941.0, 1477.0, {3, 2, 1}},
    {"D", 941.0, 1633.0, {3, 3, 1}},
};
