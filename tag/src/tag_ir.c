#include "tag_ir.h"

static InfraredWorker* worker;
static TagIrMode mode = InfraredUninitialised;
static InfraredProtocol protocol;
static int repeats;
static uint32_t address;

// lifecycle methods

TagIrMode tag_ir_mode_get() {
    return mode;
}

void tag_ir_init(InfraredProtocol proto, int rpts, uint32_t addr) {
    furi_assert(mode == InfraredUninitialised);
    worker = infrared_worker_alloc();
    infrared_worker_rx_enable_blink_on_receiving(worker, true);
    infrared_worker_rx_enable_signal_decoding(worker, true);
    protocol = proto;
    repeats = rpts;
    address = addr;
    mode = InfraredReady;
}

void tag_ir_rx_start(InfraredWorkerReceivedSignalCallback callback, void* context) {
    furi_assert(mode == InfraredReady);
    infrared_worker_rx_set_received_signal_callback(worker, callback, context);
    infrared_worker_rx_start(worker);
    mode = InfraredListening;
}

void tag_ir_rx_stop() {
    furi_assert(mode == InfraredListening);
    infrared_worker_rx_stop(worker);
    mode = InfraredReady;
}

void tag_ir_destroy() {
    if(mode == InfraredListening) tag_ir_rx_stop();
    furi_assert(mode == InfraredReady);
    infrared_worker_free(worker);
    mode = InfraredUninitialised;
}

// messaging methods

/**
 * Create an infrared message for the protocol, address, command provided.
 */
static InfraredMessage* tag_ir_create_message(
    InfraredProtocol protocol,
    uint32_t address,
    uint32_t command,
    bool repeat) {
    InfraredMessage* msg = malloc(sizeof(InfraredMessage));
    msg->protocol = protocol;
    msg->address = address;
    msg->command = command;
    msg->repeat = repeat;
    furi_assert(tag_ir_message_valid(msg));
    return msg;
}

InfraredMessage* tag_ir_create_firing_message(int identity) {
    uint32_t command = (uint32_t)identity;
    return tag_ir_create_message(protocol, address, command, true);
}

void tag_ir_free_message(InfraredMessage* msg) {
    furi_assert(msg != NULL);
    free(msg);
}

void tag_ir_send(InfraredMessage* msg) {
    furi_assert(tag_ir_message_valid(msg));
    infrared_send(msg, repeats);
}

// validation methods

bool tag_ir_message_valid(InfraredMessage* message) {
    if(!infrared_is_protocol_valid(message->protocol)) {
        FURI_LOG_E(TAG, "Unknown protocol");
        return false;
    }

    uint32_t address_length = infrared_get_protocol_address_length(message->protocol);
    uint32_t address_mask = (1UL << address_length) - 1;

    if(message->address != (message->address & address_mask)) {
        FURI_LOG_E(
            TAG,
            "Address is out of range (mask 0x%08lX): 0x%lX\r\n",
            address_mask,
            message->address);
        return false;
    }

    uint32_t command_length = infrared_get_protocol_command_length(message->protocol);
    uint32_t command_mask = (1UL << command_length) - 1;

    if(message->command != (message->command & command_mask)) {
        FURI_LOG_E(
            TAG,
            "Command is out of range (mask 0x%08lX): 0x%lX\r\n",
            command_mask,
            message->command);
        return false;
    }

    return true;
}
