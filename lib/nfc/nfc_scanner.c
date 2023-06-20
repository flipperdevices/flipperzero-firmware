#include "nfc_scanner.h"
#include "nfc_poller_manager.h"
#include "nfc_poller_defs.h"

#include <furi/furi.h>

typedef enum {
    NfcScannerStateIdle,
    NfcScannerStateTryBasePollers,
    NfcScannerStateDetectChildrenProtocols,
    NfcScannerStateComplete,

    NfcScannerStateNum,
} NfcScannerState;

typedef enum {
    NfcScannerSessionStateIdle,
    NfcScannerSessionStateActive,
    NfcScannerSessionStateStopRequest,
} NfcScannerSessionState;

struct NfcScanner {
    Nfc* nfc;
    NfcPollerManager* poller_manager;
    NfcScannerState state;
    NfcScannerSessionState session_state;

    NfcScannerCallback callback;
    void* context;

    NfcEvent nfc_event;

    size_t base_protocols_num;
    size_t base_protocol_idx;
    NfcProtocolType base_protocols[NfcProtocolTypeMax];

    size_t detected_protocols_num;
    NfcProtocolType detected_protocols[NfcProtocolTypeMax];

    NfcProtocolType current_protocol;
    NfcPoller* current_poller;

    FuriThread* scan_worker;
};

static void nfc_scanner_reset(NfcScanner* instance) {
    instance->base_protocols_num = 0;
    instance->detected_protocols_num = 0;
}

typedef void (*NfcScannerStateHandler)(NfcScanner* instance);

void nfc_scanner_state_handler_idle(NfcScanner* instance) {
    for(size_t i = 0; i < NfcProtocolTypeMax; i++) {
        if(nfc_poller_nodes->parent_protocol == NfcProtocolTypeInvalid) {
            instance->base_protocols[instance->base_protocols_num] = i;
            instance->base_protocols_num++;
        }
    }
    instance->base_protocol_idx = 0;
}

static NfcCommand nfc_scanner_base_poller_detect_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcScanner* instance = context;

    NfcCommand command = NfcCommandContinue;
    instance->nfc_event = event;
    NfcPollerEvent scanner_event = {
        .protocol_type = NfcProtocolTypeInvalid,
        .poller = instance->nfc,
        .data = &instance->nfc_event,
    };
    const NfcPollerBase* current_poller_api = nfc_pollers_api[instance->current_protocol];

    if(event.type == NfcEventTypeConfigureRequest) {
        instance->current_poller = current_poller_api->alloc(instance->nfc);
    } else if(event.type == NfcEventTypePollerReady) {
        bool protocol_detected =
            current_poller_api->detect(scanner_event, instance->current_poller);
        if(protocol_detected) {
            instance->detected_protocols[instance->detected_protocols_num] =
                instance->current_protocol;
            instance->detected_protocols_num++;
        }
        command = NfcCommandStop;
    } else if(event.type == NfcEventTypeReset) {
        current_poller_api->free(instance->current_poller);
    }

    return command;
}

void nfc_scanner_state_handler_try_base_pollers(NfcScanner* instance) {
    instance->current_protocol = instance->base_protocols[instance->base_protocol_idx];

    nfc_start_poller(instance->nfc, nfc_scanner_base_poller_detect_callback, instance);
    nfc_stop(instance->nfc);

    if(instance->detected_protocols_num > 0) {
        instance->state = NfcScannerStateDetectChildrenProtocols;
    } else {
        instance->base_protocol_idx =
            (instance->base_protocol_idx + 1) % instance->base_protocols_num;
    }
}

void nfc_scanner_state_handler_detect_children_protocols(NfcScanner* instance) {
    instance->state = NfcScannerStateComplete;
}

void nfc_scanner_state_handler_complete(NfcScanner* instance) {
    NfcScannerEvent event = {
        .type = NfcScannerEventTypeDetected,
        .data =
            {
                .protocol_num = instance->detected_protocols_num,
                .protocols = instance->detected_protocols,
            },
    };

    instance->callback(event, instance->context);

    instance->state = NfcScannerStateIdle;
}

static NfcScannerStateHandler nfc_scanner_state_handlers[NfcScannerStateNum] = {
    [NfcScannerStateIdle] = nfc_scanner_state_handler_idle,
    [NfcScannerStateTryBasePollers] = nfc_scanner_state_handler_try_base_pollers,
    [NfcScannerStateDetectChildrenProtocols] = nfc_scanner_state_handler_detect_children_protocols,
    [NfcScannerStateComplete] = nfc_scanner_state_handler_complete,
};

static int32_t nfc_scanner_worker(void* context) {
    furi_assert(context);

    NfcScanner* instance = context;
    furi_assert(instance->poller_manager);

    while(instance->session_state == NfcScannerSessionStateActive) {
        nfc_scanner_state_handlers[instance->state](instance);
    }

    nfc_scanner_reset(instance);

    return 0;
}

NfcScanner* nfc_scanner_alloc(Nfc* nfc) {
    furi_assert(nfc);

    NfcScanner* instance = malloc(sizeof(NfcScanner));
    instance->nfc = nfc;
    instance->poller_manager = nfc_poller_manager_alloc(instance->nfc);

    instance->scan_worker = furi_thread_alloc();
    furi_thread_set_name(instance->scan_worker, "NfcScanWorker");
    furi_thread_set_context(instance->scan_worker, instance);
    furi_thread_set_stack_size(instance->scan_worker, 2 * 1024);
    furi_thread_set_callback(instance->scan_worker, nfc_scanner_worker);

    return instance;
}

void nfc_scanner_free(NfcScanner* instance) {
    furi_assert(instance);
    furi_assert(instance->poller_manager);

    nfc_poller_manager_free(instance->poller_manager);
    free(instance);
}

void nfc_scanner_start(NfcScanner* instance, NfcScannerCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(instance->session_state == NfcScannerSessionStateIdle);

    instance->callback = callback;
    instance->context = context;
    instance->session_state = NfcScannerSessionStateActive;
}

void nfc_scanner_stop(NfcScanner* instance) {
    furi_assert(instance);

    instance->session_state = NfcScannerSessionStateStopRequest;
    furi_thread_join(instance->scan_worker);
    instance->session_state = NfcScannerSessionStateIdle;

    instance->callback = NULL;
    instance->context = NULL;
}
