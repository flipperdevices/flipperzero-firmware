// Methods for IR transmission

#include "quac.h"
#include "action_i.h"
#include "action_ir_utils.h"

void action_ir_tx(void* context, const FuriString* action_path, FuriString* error) {
    UNUSED(error);
    App* app = context;
    const char* file_name = furi_string_get_cstr(action_path);
    InfraredSignal* signal = infrared_utils_signal_alloc();

    FlipperFormat* fff_data_file = flipper_format_file_alloc(app->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    // uint32_t temp_data32;

    // https://developer.flipper.net/flipperzero/doxygen/infrared_file_format.html
    // TODO: Right now we only read the first signal found in the file. Add support
    // for reading any signal by 'name'?
    do {
        if(!flipper_format_file_open_existing(fff_data_file, file_name)) {
            ACTION_SET_ERROR("IR: Error opening %s", file_name);
            break;
        }
        uint32_t index = 0;
        if(!infrared_utils_read_signal_at_index(fff_data_file, index, signal, temp_str)) {
            ACTION_SET_ERROR("IR: Failed to read from file");
            break;
        }

        if(signal->is_raw) {
            // raw
            FURI_LOG_I(
                TAG,
                "IR: Sending (%s) type=raw => %d timings, %lu Hz, %f",
                file_name,
                signal->payload.raw.timings_size,
                signal->payload.raw.frequency,
                (double)signal->payload.raw.duty_cycle);

            infrared_send_raw_ext(
                signal->payload.raw.timings,
                signal->payload.raw.timings_size,
                true,
                signal->payload.raw.frequency,
                signal->payload.raw.duty_cycle);

            FURI_LOG_I(TAG, "IR: Send complete");
        } else {
            //parsed
            FURI_LOG_I(
                TAG,
                "IR: Sending (%s) type=parsed => %s %lu %lu",
                file_name,
                infrared_get_protocol_name(signal->payload.message.protocol),
                signal->payload.message.address,
                signal->payload.message.command);

            infrared_send(&signal->payload.message, 1);

            FURI_LOG_I(TAG, "IR: Send complete");
        }

    } while(false);

    furi_string_free(temp_str);
    flipper_format_free(fff_data_file);
    infrared_utils_signal_free(signal);
}