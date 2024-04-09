// Utility methods for IR transmission
#include <furi.h>
#include "../quac.h"

#include <flipper_format/flipper_format.h>

#include "action_ir_utils.h"

InfraredSignal* infrared_utils_signal_alloc() {
    InfraredSignal* signal = malloc(sizeof(InfraredSignal));
    signal->is_raw = false;
    signal->payload.message.protocol = InfraredProtocolUnknown;
    return signal;
}

void infrared_utils_signal_free(InfraredSignal* signal) {
    if(signal->is_raw) {
        free(signal->payload.raw.timings);
        signal->payload.raw.timings = NULL;
    }
    free(signal);
}

bool infrared_utils_read_signal_at_index(
    FlipperFormat* fff_data_file,
    uint32_t index,
    InfraredSignal* signal,
    FuriString* name) {
    //
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32;
    bool success = false;

    do {
        if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
            FURI_LOG_E(TAG, "IR: Missing or incorrect header");
            break;
        }

        if(!furi_string_cmp_str(temp_str, INFRARED_FILE_TYPE) &&
           temp_data32 == INFRARED_FILE_VERSION) {
        } else {
            // ACTION_SET_ERROR("IR: File type or version mismatch");
            FURI_LOG_E(TAG, "IR: File type or version mismatch");
            break;
        }

        // Read the file until we find the signal we want
        uint32_t i = 0;
        bool found = false;
        while(flipper_format_read_string(fff_data_file, "name", name)) {
            if(i == index) {
                found = true;
                break;
            }
            ++i;
        }
        if(!found) {
            // ACTION_SET_ERROR("IR: Could not find command %lu!", index);
            FURI_LOG_E(TAG, "Requested IR command %lu not found", index);
            furi_string_reset(name);
            break;
        }

        FURI_LOG_I(TAG, "Reading signal %s", furi_string_get_cstr(temp_str));
        if(!flipper_format_read_string(fff_data_file, "type", temp_str)) {
            // ACTION_SET_ERROR("IR: Type missing");
            break;
        }
        if(furi_string_equal(temp_str, "parsed")) {
            signal->is_raw = false;

            if(!flipper_format_read_string(fff_data_file, "protocol", temp_str)) {
                // ACTION_SET_ERROR("IR: Invalid or missing protocol");
                break;
            }
            signal->payload.message.protocol =
                infrared_get_protocol_by_name(furi_string_get_cstr(temp_str));
            if(!infrared_is_protocol_valid(signal->payload.message.protocol)) {
                // ACTION_SET_ERROR("IR: Invalid or unknown protocol");
                break;
            }

            if(!flipper_format_read_hex(
                   fff_data_file, "address", (uint8_t*)&signal->payload.message.address, 4)) {
                // ACTION_SET_ERROR("IR: Failed to read address");
                break;
            }
            if(!flipper_format_read_hex(
                   fff_data_file, "command", (uint8_t*)&signal->payload.message.command, 4)) {
                // ACTION_SET_ERROR("IR: Failed to read command");
                break;
            }
            success = true;
        } else if(furi_string_equal(temp_str, "raw")) {
            signal->is_raw = true;

            if(!flipper_format_read_uint32(
                   fff_data_file, "frequency", &signal->payload.raw.frequency, 1)) {
                // ACTION_SET_ERROR("IR: Failed to read frequency");
                break;
            }
            if(!flipper_format_read_float(
                   fff_data_file, "duty_cycle", &signal->payload.raw.duty_cycle, 1)) {
                // ACTION_SET_ERROR("IR: Failed to read duty cycle");
                break;
            }
            if(!flipper_format_get_value_count(fff_data_file, "data", &temp_data32)) {
                // ACTION_SET_ERROR("IR: Failed to get size of data");
                break;
            }
            if(temp_data32 > MAX_TIMINGS_AMOUNT) {
                // ACTION_SET_ERROR("IR: Data size exceeds limit");
                break;
            }
            signal->payload.raw.timings_size = temp_data32;

            signal->payload.raw.timings =
                malloc(sizeof(uint32_t) * signal->payload.raw.timings_size);
            if(!flipper_format_read_uint32(
                   fff_data_file, "data", signal->payload.raw.timings, temp_data32)) {
                // ACTION_SET_ERROR("IR: Failed to read data");
                free(signal->payload.raw.timings);
                break;
            }
            success = true;
        }
    } while(false);

    return success;
}

bool infrared_utils_write_signal(
    FlipperFormat* fff_data_file,
    InfraredSignal* signal,
    FuriString* name) {
    //
    bool success = false;

    do {
        if(!flipper_format_write_header_cstr(
               fff_data_file, INFRARED_FILE_TYPE, INFRARED_FILE_VERSION)) {
            FURI_LOG_E(TAG, "Error writing header");
            break;
        }
        if(!flipper_format_write_comment_cstr(fff_data_file, "")) {
            FURI_LOG_E(TAG, "Error writing blank comment");
            break;
        }
        if(!flipper_format_write_string(fff_data_file, "name", name)) {
            FURI_LOG_E(TAG, "Error writing name");
            break;
        }
        if(!flipper_format_write_string_cstr(
               fff_data_file, "type", signal->is_raw ? "raw" : "parsed")) {
            FURI_LOG_E(TAG, "Error writing type");
            break;
        }
        if(signal->is_raw) {
            // raw
            if(!flipper_format_write_uint32(
                   fff_data_file, "frequency", &signal->payload.raw.frequency, 1)) {
                FURI_LOG_E(TAG, "Error writing frequency");
                break;
            }
            if(!flipper_format_write_float(
                   fff_data_file, "duty_cycle", &signal->payload.raw.duty_cycle, 1)) {
                FURI_LOG_E(TAG, "Error writing duty_cycle");
                break;
            }
            if(!flipper_format_write_uint32(
                   fff_data_file,
                   "data",
                   signal->payload.raw.timings,
                   signal->payload.raw.timings_size)) {
                FURI_LOG_E(TAG, "Error writing data");
                break;
            }
            success = true;
        } else {
            // parsed
            if(!flipper_format_write_string_cstr(
                   fff_data_file,
                   "protocol",
                   infrared_get_protocol_name(signal->payload.message.protocol))) {
                FURI_LOG_E(TAG, "Error writing protocol");
                break;
            }
            if(!flipper_format_write_hex(
                   fff_data_file, "address", (uint8_t*)&signal->payload.message.address, 4)) {
                FURI_LOG_E(TAG, "Error writing address");
                break;
            }
            if(!flipper_format_write_hex(
                   fff_data_file, "command", (uint8_t*)&signal->payload.message.command, 4)) {
                FURI_LOG_E(TAG, "Error writing command");
                break;
            }
            success = true;
        }
    } while(false);

    return success;
}
