// Methods for IR transmission

// infrared
#include <infrared.h>
#include <infrared/encoder_decoder/infrared.h>
#include <infrared/worker/infrared_transmit.h>
#include <infrared/worker/infrared_worker.h>

#include <flipper_format/flipper_format.h>

#include "action_i.h"
#include "quac.h"

#define INFRARED_FILE_TYPE "IR signals file"
#define INFRARED_FILE_VERSION 1

typedef struct {
    size_t timings_size; /**< Number of elements in the timings array. */
    uint32_t* timings; /**< Pointer to an array of timings describing the signal. */
    uint32_t frequency; /**< Carrier frequency of the signal. */
    float duty_cycle; /**< Duty cycle of the signal. */
} InfraredRawSignal;

typedef struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message; // protocol, address, command, repeat
        InfraredRawSignal raw;
    } payload;
} InfraredSignal;

InfraredSignal* infrared_signal_alloc() {
    InfraredSignal* signal = malloc(sizeof(InfraredSignal));
    signal->is_raw = false;
    signal->payload.message.protocol = InfraredProtocolUnknown;
    return signal;
}

void infrared_signal_free(InfraredSignal* signal) {
    if(signal->is_raw) {
        free(signal->payload.raw.timings);
        signal->payload.raw.timings = NULL;
    }
    free(signal);
}

void action_ir_tx(void* context, const FuriString* action_path, FuriString* error) {
    UNUSED(error);
    App* app = context;
    const char* file_name = furi_string_get_cstr(action_path);
    InfraredSignal* signal = infrared_signal_alloc();

    FlipperFormat* fff_data_file = flipper_format_file_alloc(app->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32;

    // https://developer.flipper.net/flipperzero/doxygen/infrared_file_format.html
    // TODO: Right now we only read the first signal found in the file. Add support
    // for reading any signal by 'name'
    do {
        if(!flipper_format_file_open_existing(fff_data_file, file_name)) {
            ACTION_SET_ERROR("IR: Error opening %s", file_name);
            break;
        }
        if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
            ACTION_SET_ERROR("IR: Missing or incorrect header");
            break;
        }
        if(!furi_string_cmp_str(temp_str, INFRARED_FILE_TYPE) &&
           temp_data32 == INFRARED_FILE_VERSION) {
        } else {
            ACTION_SET_ERROR("IR: File type or version mismatch");
            break;
        }
        if(!flipper_format_read_string(fff_data_file, "name", temp_str)) {
            ACTION_SET_ERROR("IR: Invalid or missing name");
            break;
        }
        // FURI_LOG_I(TAG, "Reading signal %s", furi_string_get_cstr(temp_str));
        if(!flipper_format_read_string(fff_data_file, "type", temp_str)) {
            ACTION_SET_ERROR("IR: Type missing");
            break;
        }
        if(!furi_string_cmp_str(temp_str, "parsed")) {
            // FURI_LOG_I(TAG, "IR File is PARSED");
            signal->is_raw = false;

            if(!flipper_format_read_string(fff_data_file, "protocol", temp_str)) {
                ACTION_SET_ERROR("IR: Invalid or missing protocol");
                break;
            }
            signal->payload.message.protocol =
                infrared_get_protocol_by_name(furi_string_get_cstr(temp_str));
            if(!infrared_is_protocol_valid(signal->payload.message.protocol)) {
                ACTION_SET_ERROR("IR: Invalid or unknown protocol");
                break;
            }

            // Why do these methods exist, when the spec says the address and command
            // lengths MUST be 4 bytes?

            // uint8_t address_len;
            // address_len = infrared_get_protocol_address_length(signal->payload.message.protocol);

            // uint8_t command_len;
            // command_len = infrared_get_protocol_command_length(signal->payload.message.protocol);

            if(!flipper_format_read_hex(
                   fff_data_file, "address", (uint8_t*)&signal->payload.message.address, 4)) {
                ACTION_SET_ERROR("IR: Failed to read address");
                break;
            }
            if(!flipper_format_read_hex(
                   fff_data_file, "command", (uint8_t*)&signal->payload.message.command, 4)) {
                ACTION_SET_ERROR("IR: Failed to read command");
                break;
            }

            // FURI_LOG_I(
            //     TAG,
            //     "IR: Sending parsed => %s %lu %lu",
            //     infrared_get_protocol_name(signal->payload.message.protocol),
            //     signal->payload.message.address,
            //     signal->payload.message.command);

            infrared_send(&signal->payload.message, 1);

        } else if(!furi_string_cmp_str(temp_str, "raw")) {
            // FURI_LOG_I(TAG, "IR File is RAW");
            signal->is_raw = true;

            if(!flipper_format_read_uint32(
                   fff_data_file, "frequency", &signal->payload.raw.frequency, 1)) {
                ACTION_SET_ERROR("IR: Failed to read frequency");
                break;
            }
            if(!flipper_format_read_float(
                   fff_data_file, "duty_cycle", &signal->payload.raw.duty_cycle, 1)) {
                ACTION_SET_ERROR("IR: Failed to read duty cycle");
                break;
            }
            if(!flipper_format_get_value_count(fff_data_file, "data", &temp_data32)) {
                ACTION_SET_ERROR("IR: Failed to get size of data");
                break;
            }
            if(temp_data32 > MAX_TIMINGS_AMOUNT) {
                ACTION_SET_ERROR("IR: Data size exceeds limit");
                break;
            }
            signal->payload.raw.timings_size = temp_data32;

            signal->payload.raw.timings =
                malloc(sizeof(uint32_t) * signal->payload.raw.timings_size);
            if(!flipper_format_read_uint32(
                   fff_data_file, "data", signal->payload.raw.timings, temp_data32)) {
                ACTION_SET_ERROR("IR: Failed to read data");
                break;
            }

            // FURI_LOG_I(
            //     TAG,
            //     "IR: Sending raw => %d timings, %lu Hz, %f",
            //     signal->payload.raw.timings_size,
            //     signal->payload.raw.frequency,
            //     (double)signal->payload.raw.duty_cycle);

            infrared_send_raw_ext(
                signal->payload.raw.timings,
                signal->payload.raw.timings_size,
                true,
                signal->payload.raw.frequency,
                signal->payload.raw.duty_cycle);
        } else {
            ACTION_SET_ERROR("IR: Unknown type: %s", furi_string_get_cstr(temp_str));
            break;
        }

    } while(false);

    furi_string_free(temp_str);
    flipper_format_free(fff_data_file);
    infrared_signal_free(signal);
}