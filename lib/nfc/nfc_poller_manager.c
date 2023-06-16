#include "nfc_poller_manager.h"

#include "nfc_poller_defs.h"

#include <furi.h>

#include "nfc.h"

typedef enum {
    NfcPollerManagerStateIdle,

    NfcPollerManagerStateNum,
} NfcPollerManagerState;

typedef enum {
    NfcPollerManagerSessionStateIdle,
    NfcPollerManagerSessionStateActive,
    NfcPollerManagerSessionStateStopRequest,
} NfcPollerManagerSessionState;

typedef struct NfcPollerListElement {
    NfcProtocolType protocol;
    NfcPoller* poller;
    const NfcPollerBase* poller_api;
    struct NfcPollerListElement* child;
} NfcPollerListElement;

typedef struct {
    NfcPollerListElement* head;
} NfcPollerList;

struct NfcPollerManager {
    Nfc* nfc;
    NfcPollerManagerState state;
    NfcPollerManagerSessionState session_state;

    size_t base_protocol_current;
    size_t base_protocol_total;
    NfcPollerEvent* event;

    NfcPollerBase* current_poller_base;
    NfcPoller* current_poller;
    NfcPoller* base_poller;

    NfcPollerList* list;
    NfcProtocolType protocol;

    NfcPollerCallback callback;
    void* context;
};

NfcPollerManager* nfc_poller_manager_alloc() {
    NfcPollerManager* instance = malloc(sizeof(NfcPollerManager));
    instance->nfc = nfc_alloc();

    return instance;
}

void nfc_poller_manager_free(NfcPollerManager* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    nfc_free(instance->nfc);
    free(instance);
}

// typedef NfcCommand (*NfcPollerManagerHandler)(NfcPollerManager* instance);

// NfcCommand nfc_poller_manager_handler_idle(NfcPollerManager* instance) {
//     NfcCommand command = NfcCommandContinue;

//     instance->base_protocol_total = COUNT_OF(nfc_poller_base_type);
//     instance->base_protocol_current = 0;

//     return command;
// }

// static const NfcPollerManagerHandler nfc_poller_manager_handler[NfcPollerManagerStateNum] = {
//     [NfcPollerManagerStateIdle] = nfc_poller_manager_handler_idle,
// };

// static NfcCommand nfc_poller_manager_scan_event_callback(NfcEvent event, void* context) {
//     furi_assert(context);

//     NfcPollerManager* instance = context;
//     furi_assert(instance->callback);

//     NfcPollerEvent poller_event;
//     NfcPollerCommand command = NfcPollerCommandContinue;

//     if(instance->session_state == NfcPollerManagerSessionStateStopRequest) {
//         command = NfcPollerCommandStop;
//     } else {
//         if(event.type == NfcEventTypeConfigureRequest) {
//             instance->current_poller = instance->current_poller_base->alloc(instance->base_poller);
//         } else if(event.type == NfcEventTypePollerReady) {
//             command = instance->current_poller_base->run(instance->current_poller);
//         } else if(event.type == NfcEventTypeReset) {
//             instance->current_poller_base->free(instance->current_poller);
//         }
//     }

//     return nfc_poller_manager_process_command(command);
// }

// void nfc_poller_manager_scan(NfcPollerManager* instance, NfcPollerCallback callback, void* context) {
//     furi_assert(instance);
//     furi_assert(callback);
//     furi_assert(instance->session_state == NfcPollerManagerSessionStateIdle);

//     instance->callback = callback;
//     instance->context = context;
//     instance->session_state = NfcPollerManagerSessionStateActive;

//     nfc_start_poller(instance->nfc, nfc_poller_manager_scan_event_callback, instance);
// }

static void nfc_poller_manager_poller_list_alloc(NfcPollerManager* instance) {
    furi_assert(instance->list == NULL);

    instance->list = malloc(sizeof(NfcPollerList));

    instance->list->head = malloc(sizeof(NfcPollerListElement));
    instance->list->head->protocol = instance->protocol;
    instance->list->head->poller_api = nfc_pollers_api[instance->protocol];
    instance->list->head->child = NULL;

    do {
        const NfcPollerTreeNode* node = &nfc_poller_nodes[instance->list->head->protocol];
        if(node->parent_protocol == NfcProtocolTypeInvalid) break;

        NfcPollerListElement* parent = malloc(sizeof(NfcPollerListElement));
        parent->protocol = node->parent_protocol;
        parent->poller_api = nfc_pollers_api[node->parent_protocol];
        parent->child = instance->list->head;
        instance->list->head = parent;
    } while(true);

    NfcPollerListElement* iter = instance->list->head;
    iter->poller = iter->poller_api->alloc(instance->nfc);

    do {
        if(iter->child == NULL) break;

        iter->child->poller = iter->child->poller_api->alloc(iter->poller);

        iter = iter->child;
    } while(true);
}

static void nfc_poller_manager_poller_list_free(NfcPollerManager* instance) {
    do {
        NfcPollerListElement* child = instance->list->head->child;
        if(child == NULL) break;

        instance->list->head->poller_api->free(instance->list->head->poller);
        free(instance->list->head);
        instance->list->head = child;
    } while(true);

    free(instance->list->head);
    free(instance->list);
    instance->list = NULL;
}

static NfcCommand nfc_poller_manager_start_event_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcPollerManager* instance = context;
    furi_assert(instance->callback);

    NfcCommand command = NfcCommandContinue;

    if(instance->session_state == NfcPollerManagerSessionStateStopRequest) {
        command = NfcCommandStop;
    } else {
        if(event.type == NfcEventTypeConfigureRequest) {
            nfc_poller_manager_poller_list_alloc(instance);
            instance->current_poller = instance->current_poller_base->alloc(instance->base_poller);
        } else if(event.type == NfcEventTypePollerReady) {
        } else if(event.type == NfcEventTypeReset) {
            nfc_poller_manager_poller_list_free(instance);
        }
    }

    return command;
}

void nfc_poller_manager_start(
    NfcPollerManager* instance,
    NfcProtocolType protocol,
    NfcPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(protocol < NfcProtocolTypeMax);
    furi_assert(instance->session_state == NfcPollerManagerSessionStateIdle);

    instance->protocol = protocol;
    instance->callback = callback;
    instance->context = context;
    instance->session_state = NfcPollerManagerSessionStateActive;

    nfc_start_poller(instance->nfc, nfc_poller_manager_start_event_callback, instance);
}

void nfc_poller_manager_stop(NfcPollerManager* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    instance->session_state = NfcPollerManagerSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = NfcPollerManagerSessionStateIdle;

    instance->callback = NULL;
    instance->context = NULL;
}
