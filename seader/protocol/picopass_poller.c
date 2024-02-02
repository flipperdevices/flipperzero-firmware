#include "picopass_poller_i.h"

#include <furi/furi.h>

#define TAG "Picopass"

typedef NfcCommand (*PicopassPollerStateHandler)(PicopassPoller* instance);

static void picopass_poller_reset(PicopassPoller* instance) {
    UNUSED(instance);
}

NfcCommand picopass_poller_request_mode_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->event.type = PicopassPollerEventTypeRequestMode;
    command = instance->callback(instance->event, instance->context);
    instance->mode = instance->event_data.req_mode.mode;
    instance->state = PicopassPollerStateDetect;

    return command;
}

NfcCommand picopass_poller_detect_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    PicopassError error = picopass_poller_actall(instance);

    if(error == PicopassErrorNone) {
        instance->state = PicopassPollerStateSelect;
        instance->event.type = PicopassPollerEventTypeCardDetected;
        command = instance->callback(instance->event, instance->context);
    } else {
        furi_delay_ms(100);
    }

    return command;
}

NfcCommand picopass_poller_select_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        PicopassError error = picopass_poller_identify(instance, &instance->col_res_serial_num);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }

        error =
            picopass_poller_select(instance, &instance->col_res_serial_num, &instance->serial_num);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }

        instance->state = PicopassPollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand picopass_poller_success_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->event.type = PicopassPollerEventTypeSuccess;
    command = instance->callback(instance->event, instance->context);
    furi_delay_ms(100);

    return command;
}

NfcCommand picopass_poller_fail_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandReset;

    instance->event.type = PicopassPollerEventTypeFail;
    command = instance->callback(instance->event, instance->context);
    picopass_poller_reset(instance);
    instance->state = PicopassPollerStateDetect;

    return command;
}

static const PicopassPollerStateHandler picopass_poller_state_handler[PicopassPollerStateNum] = {
    [PicopassPollerStateRequestMode] = picopass_poller_request_mode_handler,
    [PicopassPollerStateDetect] = picopass_poller_detect_handler,
    [PicopassPollerStateSelect] = picopass_poller_select_handler,
    [PicopassPollerStateSuccess] = picopass_poller_success_handler,
    [PicopassPollerStateFail] = picopass_poller_fail_handler,
};

static NfcCommand picopass_poller_callback(NfcEvent event, void* context) {
    furi_assert(context);

    PicopassPoller* instance = context;
    NfcCommand command = NfcCommandContinue;

    if(event.type == NfcEventTypePollerReady) {
        command = picopass_poller_state_handler[instance->state](instance);
    }

    if(instance->session_state == PicopassPollerSessionStateStopRequest) {
        command = NfcCommandStop;
    }

    return command;
}

void picopass_poller_start(
    PicopassPoller* instance,
    PicopassPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->session_state == PicopassPollerSessionStateIdle);

    instance->callback = callback;
    instance->context = context;

    instance->session_state = PicopassPollerSessionStateActive;
    nfc_start(instance->nfc, picopass_poller_callback, instance);
}

void picopass_poller_stop(PicopassPoller* instance) {
    furi_assert(instance);

    instance->session_state = PicopassPollerSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = PicopassPollerSessionStateIdle;
}

PicopassPoller* picopass_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    PicopassPoller* instance = malloc(sizeof(PicopassPoller));
    instance->nfc = nfc;
    nfc_config(instance->nfc, NfcModePoller, NfcTechIso15693);
    nfc_set_guard_time_us(instance->nfc, 10000);
    nfc_set_fdt_poll_fc(instance->nfc, 5000);
    nfc_set_fdt_poll_poll_us(instance->nfc, 1000);

    instance->event.data = &instance->event_data;

    instance->tx_buffer = bit_buffer_alloc(PICOPASS_POLLER_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(PICOPASS_POLLER_BUFFER_SIZE);
    instance->tmp_buffer = bit_buffer_alloc(PICOPASS_POLLER_BUFFER_SIZE);

    return instance;
}

void picopass_poller_free(PicopassPoller* instance) {
    furi_assert(instance);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    bit_buffer_free(instance->tmp_buffer);
    free(instance);
}

uint8_t* picopass_poller_get_csn(PicopassPoller* instance) {
    furi_assert(instance);

    return instance->serial_num.data;
}
