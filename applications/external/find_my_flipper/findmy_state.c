#include "findmy_state.h"

#include <string.h>
#include <stddef.h>
#include <furi_hal_bt.h>
#include <flipper_format/flipper_format.h>

bool findmy_state_load(FindMyState* out_state) {
    FindMyState state;

    // Try to load from file
    bool loaded_from_file = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(storage_file_exists(storage, FINDMY_STATE_PATH)) {
        FlipperFormat* file = flipper_format_file_alloc(storage);
        do {
            uint32_t tmp;
            FuriString* str = furi_string_alloc();
            if(!flipper_format_file_open_existing(file, FINDMY_STATE_PATH)) break;
            if(!flipper_format_read_header(file, str, &tmp)) break;
            if(furi_string_cmp_str(str, FINDMY_STATE_HEADER)) break;
            if(tmp != FINDMY_STATE_VER) break;

            if(!flipper_format_read_bool(file, "beacon_active", &state.beacon_active, 1)) break;

            if(!flipper_format_read_uint32(file, "broadcast_interval", &tmp, 1)) break;
            state.broadcast_interval = tmp;

            if(!flipper_format_read_uint32(file, "transmit_power", &tmp, 1)) break;
            state.transmit_power = tmp;

            if(!flipper_format_read_bool(file, "show_mac", &state.show_mac, 1)) {
                // Support migrating from old config
                state.show_mac = false;
                flipper_format_rewind(file);
            }

            if(!flipper_format_read_uint32(file, "tag_type", &tmp, 1)) {
                // Support migrating from old config
                tmp = FindMyTypeApple;
                flipper_format_rewind(file);
            }
            state.tag_type = tmp;

            if(!flipper_format_read_hex(file, "mac", state.mac, sizeof(state.mac))) break;

            if(!flipper_format_read_hex(
                   file, "data", state.data, findmy_state_data_size(state.tag_type)))
                break;

            loaded_from_file = true;
        } while(0);
        flipper_format_free(file);
    }
    furi_record_close(RECORD_STORAGE);

    // Otherwise set default values
    if(!loaded_from_file) {
        state.beacon_active = false;
        state.broadcast_interval = 5;
        state.transmit_power = 6;
        state.show_mac = false;
        state.tag_type = FindMyTypeApple;

        // Set default mac
        uint8_t default_mac[EXTRA_BEACON_MAC_ADDR_SIZE] = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        memcpy(state.mac, default_mac, sizeof(state.mac));

        // Set default empty AirTag data
        uint8_t* data = state.data;
        *data++ = 0x1E; // Length
        *data++ = 0xFF; // Manufacturer Specific Data
        *data++ = 0x4C; // Company ID (Apple, Inc.)
        *data++ = 0x00; // ...
        *data++ = 0x12; // Type (FindMy)
        *data++ = 0x19; // Length
        *data++ = 0x00; // Status
        // Placeholder Empty Public Key without the MAC address
        for(size_t i = 0; i < 22; ++i) {
            *data++ = 0x00;
        }
        *data++ = 0x00; // First 2 bits are the version, the rest is the battery level
        *data++ = 0x00; // Hint (0x00)
    }

    // Sync values to config
    findmy_state_sync_config(&state);

    // Set constants
    state.config.adv_channel_map = GapAdvChannelMapAll;
    state.config.address_type = GapAddressTypePublic;

    // Copy to caller state before popping stack
    memcpy(out_state, &state, sizeof(state));

    // Return if active, can be used to start after loading in an if statement
    return state.beacon_active;
}

void findmy_state_apply(FindMyState* state) {
    // Stop any running beacon
    if(furi_hal_bt_extra_beacon_is_active()) {
        furi_check(furi_hal_bt_extra_beacon_stop());
    }

    furi_check(furi_hal_bt_extra_beacon_set_config(&state->config));

    furi_check(
        furi_hal_bt_extra_beacon_set_data(state->data, findmy_state_data_size(state->tag_type)));

    if(state->beacon_active) {
        furi_check(furi_hal_bt_extra_beacon_start());
    }
}

void findmy_state_sync_config(FindMyState* state) {
    state->config.min_adv_interval_ms = state->broadcast_interval * 1000; // Converting s to ms
    state->config.max_adv_interval_ms = (state->broadcast_interval * 1000) + 150;
    state->config.adv_power_level = GapAdvPowerLevel_0dBm + state->transmit_power;
    memcpy(state->config.address, state->mac, sizeof(state->config.address));
}

void findmy_state_save(FindMyState* state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, FINDMY_STATE_DIR);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        uint32_t tmp;
        if(!flipper_format_file_open_always(file, FINDMY_STATE_PATH)) break;
        if(!flipper_format_write_header_cstr(file, FINDMY_STATE_HEADER, FINDMY_STATE_VER)) break;

        if(!flipper_format_write_bool(file, "beacon_active", &state->beacon_active, 1)) break;

        tmp = state->broadcast_interval;
        if(!flipper_format_write_uint32(file, "broadcast_interval", &tmp, 1)) break;

        tmp = state->transmit_power;
        if(!flipper_format_write_uint32(file, "transmit_power", &tmp, 1)) break;

        tmp = state->tag_type;
        if(!flipper_format_write_uint32(file, "tag_type", &tmp, 1)) break;

        if(!flipper_format_write_bool(file, "show_mac", &state->show_mac, 1)) break;

        if(!flipper_format_write_hex(file, "mac", state->mac, sizeof(state->mac))) break;

        if(!flipper_format_write_hex(
               file, "data", state->data, findmy_state_data_size(state->tag_type)))
            break;
    } while(0);

    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

uint8_t findmy_state_data_size(FindMyType type) {
    switch(type) {
    case FindMyTypeApple:
    case FindMyTypeSamsung:
        return 31;
    case FindMyTypeTile:
        return 21;
    default:
        return 0;
    }
}
