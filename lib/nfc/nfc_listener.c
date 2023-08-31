#include "nfc_listener.h"

#include <nfc/protocols/nfc_listener_defs.h>
#include <nfc/protocols/nfc_device_defs.h>

#include <furi.h>

typedef struct NfcListenerListElement {
    NfcProtocol protocol;
    NfcDeviceData* data;
    NfcGenericInstance* listener;
    const NfcListenerBase* listener_api;
    struct NfcListenerListElement* child;
} NfcListenerListElement;

typedef struct {
    NfcListenerListElement* head;
    NfcListenerListElement* tail;
} NfcListenerList;

struct NfcListener {
    NfcProtocol protocol;
    Nfc* nfc;
    NfcListenerList list;
};

static void nfc_listener_list_alloc(NfcListener* instance, const NfcDeviceData* data) {
    instance->list.head = malloc(sizeof(NfcListenerListElement));
    instance->list.head->protocol = instance->protocol;

    // Allocate and copy data to emulate
    instance->list.head->data = nfc_devices[instance->protocol]->alloc();
    nfc_devices[instance->protocol]->copy(instance->list.head->data, data);

    instance->list.head->listener_api = nfc_listeners_api[instance->protocol];
    instance->list.head->child = NULL;
    instance->list.tail = instance->list.head;

    // Build linked list
    do {
        NfcProtocol parent_protocol = nfc_protocol_get_parent(instance->list.head->protocol);
        if(parent_protocol == NfcProtocolInvalid) break;

        NfcListenerListElement* parent = malloc(sizeof(NfcListenerListElement));
        parent->protocol = parent_protocol;
        parent->data =
            nfc_devices[instance->list.head->protocol]->get_base_data(instance->list.head->data);
        parent->listener_api = nfc_listeners_api[parent_protocol];
        parent->child = instance->list.head;

        instance->list.head = parent;
    } while(true);

    // Allocate listener instances
    NfcListenerListElement* iter = instance->list.head;
    iter->listener = iter->listener_api->alloc(instance->nfc, iter->data);

    do {
        if(iter->child == NULL) break;
        iter->child->listener =
            iter->child->listener_api->alloc(iter->listener, iter->child->data);
        iter->listener_api->set_callback(
            iter->listener, iter->child->listener_api->run, iter->child->listener);

        iter = iter->child;
    } while(true);
}

static void nfc_listener_list_free(NfcListener* instance) {
    // Free protocol data
    nfc_devices[instance->protocol]->free(instance->list.tail->data);

    // Free listener instances
    do {
        instance->list.head->listener_api->free(instance->list.head->listener);
        NfcListenerListElement* child = instance->list.head->child;
        free(instance->list.head);
        if(child == NULL) break;
        instance->list.head = child;
    } while(true);
}

NfcListener* nfc_listener_alloc(Nfc* nfc, NfcProtocol protocol, const NfcDeviceData* data) {
    furi_assert(nfc);
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(data);
    furi_assert(nfc_listeners_api[protocol]);

    NfcListener* instance = malloc(sizeof(NfcListener));
    instance->nfc = nfc;
    instance->protocol = protocol;
    nfc_listener_list_alloc(instance, data);

    return instance;
}

void nfc_listener_free(NfcListener* instance) {
    furi_assert(instance);

    nfc_listener_list_free(instance);
    free(instance);
}

NfcCommand nfc_listener_start_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcListener* instance = context;
    furi_assert(instance->list.head);

    NfcCommand command = NfcCommandContinue;
    NfcGenericEvent generic_event = {
        .protocol = NfcProtocolInvalid,
        .instance = instance->nfc,
        .data = &event,
    };

    NfcListenerListElement* head_listener = instance->list.head;
    command = head_listener->listener_api->run(generic_event, head_listener->listener);

    return command;
}

void nfc_listener_start(NfcListener* instance, NfcGenericCallback callback, void* context) {
    furi_assert(instance);

    NfcListenerListElement* tail_element = instance->list.tail;
    tail_element->listener_api->set_callback(tail_element->listener, callback, context);
    nfc_start_listener(instance->nfc, nfc_listener_start_callback, instance);
}

void nfc_listener_stop(NfcListener* instance) {
    furi_assert(instance);

    nfc_listener_abort(instance->nfc);
}

const NfcDeviceData* nfc_listener_get_data(NfcListener* instance, NfcProtocol protocol) {
    furi_assert(instance);
    furi_assert(instance->protocol == protocol);

    NfcListenerListElement* tail_element = instance->list.tail;
    return tail_element->listener_api->get_data(tail_element->listener);
}
