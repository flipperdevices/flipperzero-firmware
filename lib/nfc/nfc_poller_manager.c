#include "nfc_poller_manager.h"
#include "nfc_poller_defs.h"

#include <furi.h>

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

    NfcEvent* event;
    NfcPollerList* list;
    NfcProtocolType protocol;

    NfcPollerCallback callback;
    void* context;
};

NfcPollerManager* nfc_poller_manager_alloc(Nfc* nfc) {
    NfcPollerManager* instance = malloc(sizeof(NfcPollerManager));
    instance->nfc = nfc;
    instance->event = malloc(sizeof(NfcEvent));

    return instance;
}

void nfc_poller_manager_free(NfcPollerManager* instance) {
    furi_assert(instance);
    furi_assert(instance->event);
    furi_assert(instance->nfc);

    free(instance->event);
    free(instance);
}

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
        iter->poller_api->set_callback(
            iter->poller, iter->child->poller_api->run, iter->child->poller);

        iter = iter->child;
    } while(true);

    iter->poller_api->set_callback(iter->poller, instance->callback, instance->context);
}

static void nfc_poller_manager_poller_list_free(NfcPollerManager* instance) {
    do {
        instance->list->head->poller_api->free(instance->list->head->poller);
        NfcPollerListElement* child = instance->list->head->child;
        free(instance->list->head);
        if(child == NULL) break;
        instance->list->head = child;
    } while(true);


    free(instance->list);
    instance->list = NULL;
}

static NfcCommand nfc_poller_manager_start_event_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcPollerManager* instance = context;
    furi_assert(instance->callback);

    NfcCommand command = NfcCommandContinue;
    NfcPollerEvent poller_manager_event = {
        .protocol_type = NfcProtocolTypeInvalid,
        .poller = instance->nfc,
    };

    if(event.type == NfcEventTypeConfigureRequest) {
        nfc_poller_manager_poller_list_alloc(instance);
    } else if(event.type == NfcEventTypePollerReady) {
        *instance->event = event;
        poller_manager_event.data = instance->event;
        NfcPollerListElement* head = instance->list->head;
        command = head->poller_api->run(poller_manager_event, head->poller);
    } else if(event.type == NfcEventTypeReset) {
        nfc_poller_manager_poller_list_free(instance);
    }

    if(instance->session_state == NfcPollerManagerSessionStateStopRequest) {
        command = NfcCommandStop;
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
