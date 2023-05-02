#include "tag_app.h"

char* TAG = "tag";

static TagAppState* state;

TagAppState* tag_app_state_get() {
    return state;
}

void tag_app_init() {
    state = malloc(sizeof(state));
    state->app_mode = AppUninitialised;
}

void tag_app_start_playing() {
    // TODO
}

void tag_app_stop_playing() {
    // TODO - stop playing
}

void test_tx() {
    FURI_LOG_I(TAG, "Creating message for address: 0x4, command: 0x11");
    InfraredMessage* message = tag_ir_create_firing_message(0x11);
    FURI_LOG_I(TAG, "Sending message (5 repeats)");
    tag_ir_send(message);
    FURI_LOG_I(TAG, "Freeing message");
    tag_ir_free_message(message);
}

void print_stuff(void* context, InfraredWorkerSignal* received_signal) {
    UNUSED(context);
    const InfraredMessage* msg = infrared_worker_get_decoded_signal(received_signal);
    FURI_LOG_I(TAG, "protocol: %s", infrared_get_protocol_name(msg->protocol));
    FURI_LOG_I(TAG, "address:  %d", (int)msg->address);
    FURI_LOG_I(TAG, "command:  %d", (int)msg->command);
}

void test_rx() {
    FURI_LOG_I(TAG, "Testing receive (10s)...");
    tag_ir_rx_start(print_stuff, NULL);
    furi_delay_ms(10000); // 10s delay
}

int32_t tag_game_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Initialising app");
    tag_app_init();
    FURI_LOG_I(TAG, "Initialising IR");
    tag_ir_init(InfraredProtocolNEC, 5, 0x4);

    test_tx();
    test_rx();

    FURI_LOG_I(TAG, "Tearing down IR");
    tag_ir_destroy();
    return 0;
}
