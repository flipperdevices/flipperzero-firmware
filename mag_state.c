#include "mag_state.h"

#define TAG "MagState"

const GpioPin* mag_state_enum_to_pin(MagPin pin) {
    switch(pin) {
    case MagPinA7:
        return &gpio_ext_pa7;
    case MagPinA6:
        return &gpio_ext_pa6;
    case MagPinA4:
        return &gpio_ext_pa4;
    case MagPinB3:
        return &gpio_ext_pb3;
    case MagPinB2:
        return &gpio_ext_pb2;
    case MagPinC3:
        return &gpio_ext_pc3;
    case MagPinC1:
        return &gpio_ext_pc1;
    case MagPinC0:
        return &gpio_ext_pc0;
    default:
        return NULL;
    }
}

bool mag_state_gpio_is_valid(MagState* state) {
    return (state->pin_input != state->pin_output) && (state->pin_input != state->pin_enable) &&
           (state->pin_enable != state->pin_output);
}

void mag_state_gpio_reset(MagState* state) {
    state->pin_input = MAG_STATE_DEFAULT_PIN_INPUT;
    state->pin_output = MAG_STATE_DEFAULT_PIN_OUTPUT;
    state->pin_enable = MAG_STATE_DEFAULT_PIN_ENABLE;
}

bool mag_state_load(MagState* out_state) {
    MagState state;

    // Try to load from file
    bool loaded_from_file = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(storage_file_exists(storage, MAG_STATE_PATH)) {
        FlipperFormat* file = flipper_format_file_alloc(storage);
        do {
            uint32_t tmp;
            FuriString* str = furi_string_alloc();
            if(!flipper_format_file_open_existing(file, MAG_STATE_PATH)) break;
            if(!flipper_format_read_header(file, str, &tmp)) break;
            if(furi_string_cmp_str(str, MAG_STATE_HEADER)) break;
            if(tmp != MAG_STATE_VER) break;

            if(!flipper_format_read_uint32(file, "pin_input", &tmp, 1)) break;
            state.pin_input = tmp;
            if(!flipper_format_read_uint32(file, "pin_output", &tmp, 1)) break;
            state.pin_output = tmp;
            if(!flipper_format_read_uint32(file, "pin_enable", &tmp, 1)) break;
            state.pin_enable = tmp;

            loaded_from_file = true;
        } while(0);
        flipper_format_free(file);
    }
    furi_record_close(RECORD_STORAGE);

    // If could not be read from file
    // Or file GPIO config is invalid (pins overlap)
    // Set defaults
    if(!loaded_from_file || !mag_state_gpio_is_valid(&state)) {
        mag_state_gpio_reset(&state);
    }

    // set defaults we don't save
    state.tx = MAG_STATE_DEFAULT_TX;
    state.track = MAG_STATE_DEFAULT_TRACK;
    state.reverse = MAG_STATE_DEFAULT_REVERSE;
    state.us_clock = MAG_STATE_DEFAULT_US_CLOCK;
    state.us_interpacket = MAG_STATE_DEFAULT_US_INTERPACKET;
    state.is_debug = furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug);

    // Copy to caller state before popping stack
    memcpy(out_state, &state, sizeof(state));

    return loaded_from_file;
}

void mag_state_save(MagState* state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, MAG_STATE_DIR);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        uint32_t tmp;
        if(!flipper_format_file_open_always(file, MAG_STATE_PATH)) break;
        if(!flipper_format_write_header_cstr(file, MAG_STATE_HEADER, MAG_STATE_VER)) break;

        tmp = state->pin_input;
        if(!flipper_format_write_uint32(file, "pin_input", &tmp, 1)) break;
        tmp = state->pin_output;
        if(!flipper_format_write_uint32(file, "pin_output", &tmp, 1)) break;
        tmp = state->pin_enable;
        if(!flipper_format_write_uint32(file, "pin_enable", &tmp, 1)) break;

    } while(0);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}
