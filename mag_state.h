#pragma once

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <furi_hal_resources.h>
#include <furi_hal_rtc.h>

#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include "helpers/mag_types.h"

#define MAG_STATE_HEADER "Mag State"
#define MAG_STATE_VER 2
#define MAG_STATE_DIR STORAGE_APP_DATA_PATH_PREFIX
#define MAG_STATE_PATH MAG_STATE_DIR "/mag_state.txt"

typedef struct {
    MagTxState tx;
    MagTrackState track;
    MagReverseState reverse;
    uint32_t us_clock;
    uint32_t us_interpacket;
    MagPin pin_input;
    MagPin pin_output;
    MagPin pin_enable;
    bool allow_uart;
    bool is_debug;
    uint8_t n_repeats;
    bool repeat_mode;
} MagState;

const GpioPin* mag_state_enum_to_pin(MagPin pin);

bool mag_state_gpio_is_valid(MagState* state);

void mag_state_gpio_reset(MagState* state);

bool mag_state_load(MagState* out_state);

void mag_state_save(MagState* state);
