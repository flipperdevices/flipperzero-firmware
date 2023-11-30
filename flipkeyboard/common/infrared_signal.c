#include <furi.h>
#include <flipper_format/flipper_format_i.h>
#include <infrared.h>
#include <infrared_transmit.h>
#include <storage/storage.h>

#include "app_config.h"
#include "infrared_signal.h"

struct InfraredSignal {
    Resources* resources;
    FlipperFormat* fff_data_file;
    char* action;
    InfraredMessage message;
    uint32_t* timings;
    uint32_t timings_count;
    bool start_from_mark;
    uint32_t frequency;
    float duty_cycle;
};

/**
 * @brief Load an infrared signal (action) from a file.  
 * @details Load an infrared signal (action) from a file.  The first signal is loaded and ready for sending.
 * @param file_path The path to the file to load.
 * @param action The name of the action to load from the file.
 * @param resources The resources to use for sending the signal.
*/
InfraredSignal* infrared_signal_load_file(char* file_path, char* action, Resources* resources) {
    InfraredSignal* signal = (InfraredSignal*)malloc(sizeof(InfraredSignal));
    signal->resources = resources;
    signal->action = action;
    signal->message.protocol = InfraredProtocolUnknown;
    signal->timings_count = 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    signal->fff_data_file = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(signal->fff_data_file, file_path)) {
        if(!infrared_signal_load_next(signal)) {
            FURI_LOG_E(TAG, "Error '%s' not found in file '%s'.", action, file_path);
            infrared_signal_free(signal);
            signal = NULL;
        }
    } else {
        FURI_LOG_E(TAG, "Error opening file '%s'", file_path);
        infrared_signal_free(signal);
        signal = NULL;
    }

    return signal;
}

/**
 * @brief Load the next infrared signal (action) from the file.
 * @param signal The signal to load from.
 * @return True if the signal was loaded, false if there are no more signals.
*/
bool infrared_signal_load_next(InfraredSignal* signal) {
    if(!signal) {
        return false;
    }

    FuriString* temp_str = furi_string_alloc();
    bool parsed = false;
    signal->message.protocol = InfraredProtocolUnknown;
    signal->timings_count = 0;

    do {
        do {
            if(!flipper_format_read_string(signal->fff_data_file, "name", temp_str)) {
                FURI_LOG_D(TAG, "No more matching name fields");
                furi_string_free(temp_str);
                return false;
            }
        } while(strcmp(furi_string_get_cstr(temp_str), signal->action) != 0);

        if(!flipper_format_read_string(signal->fff_data_file, "type", temp_str)) {
            FURI_LOG_E(TAG, "Missing type field");
            break;
        }

        if(strcmp(furi_string_get_cstr(temp_str), "parsed") == 0) {
            // parsed hase fields in order of: protcol, address, command.
            if(!flipper_format_read_string(signal->fff_data_file, "protocol", temp_str)) {
                FURI_LOG_E(TAG, "Missing protocol field");
                break;
            }
            signal->message.protocol =
                infrared_get_protocol_by_name(furi_string_get_cstr(temp_str));
            if(signal->message.protocol == InfraredProtocolUnknown) {
                FURI_LOG_E(TAG, "Unknown protocol");
                break;
            }

            if(!flipper_format_read_hex(
                   signal->fff_data_file, "address", (uint8_t*)&signal->message.address, 4)) {
                FURI_LOG_E(TAG, "Missing address field");
                break;
            }

            if(!flipper_format_read_hex(
                   signal->fff_data_file, "command", (uint8_t*)&signal->message.command, 4)) {
                FURI_LOG_E(TAG, "Missing command field");
                break;
            }
        } else if(strcmp(furi_string_get_cstr(temp_str), "raw") == 0) {
            // raw has fields in order of: frequency, duty_cycle, data.
            if(!flipper_format_read_uint32(
                   signal->fff_data_file, "frequency", &signal->frequency, 1)) {
                FURI_LOG_E(TAG, "Missing frequency field");
                break;
            }

            if(!flipper_format_read_float(
                   signal->fff_data_file, "duty_cycle", &signal->duty_cycle, 1)) {
                FURI_LOG_E(TAG, "Missing duty_cycle field");
                break;
            }

            if(!flipper_format_get_value_count(
                   signal->fff_data_file, "data", &signal->timings_count)) {
                FURI_LOG_E(TAG, "Missing data field");
                break;
            }
            signal->timings = malloc(sizeof(uint32_t) * signal->timings_count);
            if(!flipper_format_read_uint32(
                   signal->fff_data_file, "data", signal->timings, signal->timings_count)) {
                free(signal->timings);
                signal->timings = NULL;
                FURI_LOG_E(TAG, "Missing data field contents");
                break;
            }

            // TODO: How is this supposed to be determined?
            signal->start_from_mark = true;
        } else {
            FURI_LOG_E(TAG, "Unknown type field: %s", furi_string_get_cstr(temp_str));
            break;
        }

        parsed = true;
    } while(false);

    if(!parsed) {
        signal->timings_count = 0;
        signal->message.protocol = InfraredProtocolUnknown;
    }

    return parsed;
}

/**
 * @brief Send the infrared signal (action) to the IR LED.
 * @param signal The signal to send.
 * @return True if the signal was sent, false if there was an error.
*/
bool infrared_signal_send(InfraredSignal* signal) {
    if(!signal) {
        FURI_LOG_E(TAG, "No signal to send");
        return false;
    }

    resources_acquire(signal->resources, ResourceIdInfrared, FuriWaitForever);
    if(signal->timings_count > 0) {
        FURI_LOG_D(TAG, "Sending raw signal with count %ld", signal->timings_count);
        infrared_send_raw_ext(
            signal->timings,
            signal->timings_count,
            signal->start_from_mark,
            signal->frequency,
            signal->duty_cycle);
        resources_release(signal->resources, ResourceIdInfrared);
        return true;
    } else if(signal->message.protocol != InfraredProtocolUnknown) {
        FURI_LOG_D(
            TAG,
            "Sending protocol signal with address %ld, command %ld",
            signal->message.address,
            signal->message.command);
        infrared_send(&signal->message, 5);
        resources_release(signal->resources, ResourceIdInfrared);
        return true;
    }

    resources_release(signal->resources, ResourceIdInfrared);
    FURI_LOG_E(TAG, "Unknown signal protocol");
    return false;
}

/**
 * @brief Free the memory used by the signal.
 * @param signal The signal to free.
*/
void infrared_signal_free(InfraredSignal* signal) {
    if(signal) {
        if(signal->fff_data_file) {
            flipper_format_file_close(signal->fff_data_file);
            flipper_format_free(signal->fff_data_file);
        }
        if(signal->timings) {
            free(signal->timings);
            signal->timings = NULL;
        }
        free(signal);
    }
    furi_record_close(RECORD_STORAGE);
}
