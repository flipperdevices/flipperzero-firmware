#include "ir_decoder.h"

static void render_callback(Canvas* canvas, void* ctx) {
    FURI_LOG_T(TAG, "Render callback");
    IRDecoderState* state = (IRDecoderState*)ctx;
    FuriString* temp_str = furi_string_alloc();

    furi_mutex_acquire(state->mutex, FuriWaitForever);

    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "IR Decoder");

    canvas_set_font(canvas, FontSecondary);
    if(state->decoded_signal) {
        FURI_LOG_T(TAG, "Writing signal data to display...");
        uint8_t addr_digits = 4;
        // uint8_t addr_digits = ROUND_UP_TO(infrared_get_protocol_address_length(state->decoded_signal->protocol), 4);

        char* proto = "Unknown";
        if(infrared_is_protocol_valid(state->decoded_signal->protocol)) {
            proto = (char*)infrared_get_protocol_name(state->decoded_signal->protocol);
        }
        furi_string_printf(temp_str, "%s", proto);
        canvas_draw_str(canvas, 2, 20, "Protocol:");
        canvas_draw_str(canvas, 50, 20, furi_string_get_cstr(temp_str));

        furi_string_printf(temp_str, "0x%0*lX", addr_digits, state->decoded_signal->address);
        canvas_draw_str(canvas, 2, 30, "Address:");
        canvas_draw_str(canvas, 50, 30, furi_string_get_cstr(temp_str));
        uint8_t command_digits = 4;
        // uint8_t command_digits = ROUND_UP_TO(infrared_get_protocol_command_length(state->decoded_signal->protocol), 4);
        furi_string_printf(temp_str, "0x%0*lX", command_digits, state->decoded_signal->command);
        canvas_draw_str(canvas, 2, 40, "Command:");
        canvas_draw_str(canvas, 50, 40, furi_string_get_cstr(temp_str));

        // Check if address is 2 or 4 bytes
        if(state->decoded_signal->address >> 8 == 0) {
            // 2 bytes
            furi_string_printf(
                temp_str,
                "0x%02X%02X%02X%02X",
                bit_lib_reverse_8_fast(state->decoded_signal->address),
                bit_lib_reverse_8_fast(~state->decoded_signal->address),
                bit_lib_reverse_8_fast(state->decoded_signal->command),
                bit_lib_reverse_8_fast(~state->decoded_signal->command));
        } else {
            // 4 bytes
            uint32_t mask = ((1 << (16 - 8 + 1)) - 1) << 8;
            furi_string_printf(
                temp_str,
                "0x%02X%02X%02X%02X",
                bit_lib_reverse_8_fast(state->decoded_signal->address & 0xFF),
                bit_lib_reverse_8_fast((state->decoded_signal->address & mask) >> 8),
                bit_lib_reverse_8_fast(state->decoded_signal->command & 0xFF),
                bit_lib_reverse_8_fast((state->decoded_signal->command & mask) >> 8));
        }
        canvas_draw_str(canvas, 2, 50, "LIRC HEX:");
        canvas_draw_str(canvas, 50, 50, furi_string_get_cstr(temp_str));
    }

    furi_string_free(temp_str);
    furi_mutex_release(state->mutex);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FURI_LOG_D(TAG, "Input received: %s", input_get_key_name(input_event->key));
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void ir_received_callback(void* ctx, InfraredWorkerSignal* signal) {
    FURI_LOG_D(TAG, "Signal callback");
    furi_check(signal);
    IRDecoderState* state = (IRDecoderState*)ctx;
    furi_mutex_acquire(state->mutex, FuriWaitForever);

    if(infrared_worker_signal_is_decoded(signal)) {
        InfraredMessage* decodedSignal =
            (InfraredMessage*)infrared_worker_get_decoded_signal(signal);
        if(state->decoded_signal) {
            free((InfraredMessage*)state->decoded_signal);
        }
        state->decoded_signal = (InfraredMessage*)decodedSignal;
    }

    furi_mutex_release(state->mutex);
    view_port_update(state->view_port);
}

int32_t ir_decoder_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    furi_check(event_queue);

    if(furi_hal_infrared_is_busy()) {
        FURI_LOG_E(TAG, "Infrared is busy.");
        return -1;
    }

    IRDecoderState state = {.mutex = NULL, .decoded_signal = NULL};
    state.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!state.mutex) {
        FURI_LOG_E(TAG, "Cannot create mutex.");
        return -1;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state);
    view_port_input_callback_set(view_port, input_callback, event_queue);
    state.view_port = view_port;

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InfraredWorker* worker = infrared_worker_alloc();
    infrared_worker_rx_enable_signal_decoding(worker, true);
    infrared_worker_rx_enable_blink_on_receiving(worker, true);
    infrared_worker_rx_set_received_signal_callback(worker, ir_received_callback, &state);
    infrared_worker_rx_start(worker);

    InputEvent event;
    bool processing = true;
    while(processing) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeRelease) {
                furi_mutex_acquire(state.mutex, FuriWaitForever);

                if(event.key == InputKeyBack) {
                    processing = false;
                }

                furi_mutex_release(state.mutex);
            }
        }
    }

    // Clean up
    infrared_worker_rx_stop(worker);
    infrared_worker_free(worker);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");
    furi_message_queue_free(event_queue);
    furi_mutex_free(state.mutex);

    return 0;
}
