#include "iso14443_4a_poller_i.h"

#include <furi.h>

#define TAG "Iso14443_4aPoller"

#define ISO14443_4A_BUF_SIZE_MAX (512U)

typedef Iso14443_4aPollerCommand (*Iso14443_4aPollerStateHandler)(Iso14443_4aPoller* instance);

static Iso14443_4aPollerCommand iso14443_4a_poller_handler_idle(Iso14443_4aPoller* instance);
static Iso14443_4aPollerCommand iso14443_4a_poller_handler_read_ats(Iso14443_4aPoller* instance);
static Iso14443_4aPollerCommand iso14443_4a_poller_handler_error(Iso14443_4aPoller* instance);
static Iso14443_4aPollerCommand iso14443_4a_poller_handler_ready(Iso14443_4aPoller* instance);

static const Iso14443_4aPollerStateHandler
    iso14443_4a_poller_state_handler[Iso14443_4aPollerStateNum] = {
        [Iso14443_4aPollerStateIdle] = iso14443_4a_poller_handler_idle,
        [Iso14443_4aPollerStateReadAts] = iso14443_4a_poller_handler_read_ats,
        [Iso14443_4aPollerStateError] = iso14443_4a_poller_handler_error,
        [Iso14443_4aPollerStateReady] = iso14443_4a_poller_handler_ready,
};

Iso14443_4aPoller* iso14443_4a_poller_alloc(NfcaPoller* iso14443_3a_poller) {
    Iso14443_4aPoller* instance = malloc(sizeof(Iso14443_4aPoller));
    instance->iso14443_3a_poller = iso14443_3a_poller;
    return instance;
}

void iso14443_4a_poller_free(Iso14443_4aPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->state == Iso14443_4aPollerStateIdle);

    free(instance);
}

Iso14443_4aPollerCommand iso14443_4a_poller_handler_idle(Iso14443_4aPoller* instance) {
    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);
    nfca_poller_get_data(instance->iso14443_3a_poller, instance->data->iso14443_3a_data);
    instance->state = Iso14443_4aPollerStateReadAts;
    return Iso14443_4aPollerCommandContinue;
}

Iso14443_4aPollerCommand iso14443_4a_poller_handler_read_ats(Iso14443_4aPoller* instance) {
    Iso14443_4aError error = iso14443_4a_poller_async_read_ats(instance);
    if(error == Iso14443_4aErrorNone) {
        FURI_LOG_D(TAG, "Read ATS success");
        instance->state = Iso14443_4aPollerStateReady;
    } else {
        FURI_LOG_D(TAG, "Failed to read ATS");
        instance->state = Iso14443_4aPollerStateError;
    }

    return Iso14443_4aPollerCommandContinue;
}

Iso14443_4aPollerCommand iso14443_4a_poller_handler_error(Iso14443_4aPoller* instance) {
    nfca_poller_halt(instance->iso14443_3a_poller);
    Iso14443_4aPollerEventData event_data = {.error = instance->error};
    Iso14443_4aPollerEvent event = {.type = Iso14443_4aPollerEventTypeError, .data = &event_data};
    Iso14443_4aPollerCommand command = instance->callback(event, instance->context);
    instance->state = Iso14443_4aPollerStateIdle;
    return command;
}

Iso14443_4aPollerCommand iso14443_4a_poller_handler_ready(Iso14443_4aPoller* instance) {
    Iso14443_4aPollerEvent event = {.type = Iso14443_4aPollerEventTypeReady};
    Iso14443_4aPollerCommand command = instance->callback(event, instance->context);
    return command;
}

static NfcaPollerCommand iso14443_4a_poller_process_command(Iso14443_4aPollerCommand command) {
    NfcaPollerCommand ret = NfcaPollerCommandContinue;

    if(command == Iso14443_4aPollerCommandContinue) {
        ret = NfcaPollerCommandContinue;
    } else if(command == Iso14443_4aPollerCommandReset) {
        ret = NfcaPollerCommandReset;
    } else if(command == Iso14443_4aPollerCommandStop) {
        ret = NfcaPollerCommandStop;
    } else {
        furi_crash("Unknown command");
    }

    return ret;
}

static NfcaPollerCommand iso14443_4a_poller_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    Iso14443_4aPoller* instance = context;
    Iso14443_4aPollerEventData event_data = {};
    Iso14443_4aPollerEvent iso14443_4a_event = {.data = &event_data};
    Iso14443_4aPollerCommand command = Iso14443_4aPollerCommandContinue;

    furi_assert(instance->session_state != Iso14443_4aPollerSessionStateIdle);
    if(instance->session_state == Iso14443_4aPollerSessionStateStopRequest) {
        command = Iso14443_4aPollerCommandStop;
    } else {
        if(event.type == NfcaPollerEventTypeReady) {
            command = iso14443_4a_poller_state_handler[instance->state](instance);
        } else if(event.type == NfcaPollerEventTypeError) {
            iso14443_4a_event.type = Iso14443_4aPollerEventTypeError;
            command = instance->callback(iso14443_4a_event, instance->context);
        }
    }

    return iso14443_4a_poller_process_command(command);
}

Iso14443_4aError iso14443_4a_poller_start(
    Iso14443_4aPoller* instance,
    NfcaPollerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);
    furi_assert(instance->state == Iso14443_4aPollerStateIdle);
    furi_assert(instance->session_state == Iso14443_4aPollerSessionStateIdle);
    furi_assert(callback);
    furi_assert(context);

    instance->data = iso14443_4a_alloc();
    instance->tx_buffer = bit_buffer_alloc(ISO14443_4A_BUF_SIZE_MAX);
    instance->rx_buffer = bit_buffer_alloc(ISO14443_4A_BUF_SIZE_MAX);

    instance->session_state = Iso14443_4aPollerSessionStateActive;
    instance->protocol_data.block_number = 0;

    nfca_poller_start(instance->iso14443_3a_poller, callback, context);

    return Iso14443_4aErrorNone;
}

Iso14443_4aError iso14443_4a_poller_read(
    Iso14443_4aPoller* instance,
    Iso14443_4aPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);
    furi_assert(instance->state == Iso14443_4aPollerStateIdle);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    return iso14443_4a_poller_start(instance, iso14443_4a_poller_callback, instance);
}

Iso14443_4aError iso14443_4a_poller_reset(Iso14443_4aPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->iso14443_3a_poller);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    instance->callback = NULL;
    instance->context = NULL;
    instance->state = Iso14443_4aPollerStateIdle;

    return Iso14443_4aErrorNone;
}

Iso14443_4aError iso14443_4a_poller_stop(Iso14443_4aPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);

    instance->session_state = Iso14443_4aPollerSessionStateStopRequest;
    nfca_poller_stop(instance->iso14443_3a_poller);
    instance->session_state = Iso14443_4aPollerSessionStateIdle;
    iso14443_4a_free(instance->data);

    return iso14443_4a_poller_reset(instance);
}

Iso14443_4aError iso14443_4a_poller_get_data(Iso14443_4aPoller* instance, Iso14443_4aData* data) {
    furi_assert(instance);

    iso14443_4a_copy(data, instance->data);
    return Iso14443_4aErrorNone;
}
