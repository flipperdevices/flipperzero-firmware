#include "nfc_magic_scanner.h"

#include "protocols/gen1a/gen1a_poller.h"
#include "protocols/gen4/gen4_poller.h"
#include <nfc/nfc_poller.h>

#include <furi/furi.h>

typedef enum {
    NfcMagicScannerSessionStateIdle,
    NfcMagicScannerSessionStateActive,
    NfcMagicScannerSessionStateStopRequest,
} NfcMagicScannerSessionState;

struct NfcMagicScanner {
    Nfc* nfc;
    NfcMagicScannerSessionState session_state;
    NfcMagicProtocol current_protocol;

    uint32_t gen4_password;
    bool magic_protocol_detected;

    NfcMagicScannerCallback callback;
    void* context;

    FuriThread* scan_worker;
};

static const NfcProtocol nfc_magic_scanner_not_magic_protocols[] = {
    NfcProtocolIso14443_3b,
    NfcProtocolIso15693_3,
    NfcProtocolFelica,
};

static void nfc_magic_scanner_reset(NfcMagicScanner* instance) {
    instance->session_state = NfcMagicScannerSessionStateIdle;
    instance->current_protocol = NfcMagicProtocolGen1;
}

NfcMagicScanner* nfc_magic_scanner_alloc(Nfc* nfc) {
    furi_assert(nfc);

    NfcMagicScanner* instance = malloc(sizeof(NfcMagicScanner));
    instance->nfc = nfc;

    return instance;
}

void nfc_magic_scanner_free(NfcMagicScanner* instance) {
    furi_assert(instance);

    free(instance);
}

void nfc_magic_scanner_set_gen4_password(NfcMagicScanner* instance, uint32_t password) {
    furi_assert(instance);

    instance->gen4_password = password;
}

static int32_t nfc_magic_scanner_worker(void* context) {
    furi_assert(context);

    NfcMagicScanner* instance = context;
    furi_assert(instance->session_state == NfcMagicScannerSessionStateActive);

    while(instance->session_state == NfcMagicScannerSessionStateActive) {
        if(instance->current_protocol == NfcMagicProtocolGen1) {
            instance->magic_protocol_detected = gen1a_poller_detect(instance->nfc);
        } else if(instance->current_protocol == NfcMagicProtocolGen4) {
            Gen4PollerError error = gen4_poller_detect(instance->nfc, instance->gen4_password);
            if(error == Gen4PollerErrorProtocol) {
                NfcMagicScannerEvent event = {
                    .type = NfcMagicScannerEventTypeDetectedNotMagic,
                };
                instance->callback(event, instance->context);
                break;
            } else {
                instance->magic_protocol_detected = (error == Gen4PollerErrorNone);
            }
        }

        if(instance->magic_protocol_detected) {
            NfcMagicScannerEvent event = {
                .type = NfcMagicScannerEventTypeDetected,
                .data.protocol = instance->current_protocol,
            };
            instance->callback(event, instance->context);
            break;
        }

        if(instance->current_protocol == NfcMagicProtocolNum - 1) {
            bool not_magic_protocol_detected = false;
            for(size_t i = 0; i < COUNT_OF(nfc_magic_scanner_not_magic_protocols); i++) {
                NfcProtocol protocol = nfc_magic_scanner_not_magic_protocols[i];
                NfcPoller* poller = nfc_poller_alloc(instance->nfc, protocol);
                not_magic_protocol_detected = nfc_poller_detect(poller);
                nfc_poller_free(poller);
                if(not_magic_protocol_detected) {
                    break;
                }
            }
            if(not_magic_protocol_detected) {
                NfcMagicScannerEvent event = {
                    .type = NfcMagicScannerEventTypeDetectedNotMagic,
                };
                instance->callback(event, instance->context);
                break;
            }
        }
        instance->current_protocol = (instance->current_protocol + 1) % NfcMagicProtocolNum;
    }

    nfc_magic_scanner_reset(instance);

    return 0;
}

void nfc_magic_scanner_start(
    NfcMagicScanner* instance,
    NfcMagicScannerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    instance->scan_worker = furi_thread_alloc();
    furi_thread_set_name(instance->scan_worker, "NfcMagicScanWorker");
    furi_thread_set_context(instance->scan_worker, instance);
    furi_thread_set_stack_size(instance->scan_worker, 4 * 1024);
    furi_thread_set_callback(instance->scan_worker, nfc_magic_scanner_worker);
    furi_thread_start(instance->scan_worker);

    instance->session_state = NfcMagicScannerSessionStateActive;
}

void nfc_magic_scanner_stop(NfcMagicScanner* instance) {
    furi_assert(instance);

    instance->session_state = NfcMagicScannerSessionStateStopRequest;
    furi_thread_join(instance->scan_worker);
    instance->session_state = NfcMagicScannerSessionStateIdle;

    furi_thread_free(instance->scan_worker);
    instance->scan_worker = NULL;
    instance->callback = NULL;
    instance->context = NULL;
}
