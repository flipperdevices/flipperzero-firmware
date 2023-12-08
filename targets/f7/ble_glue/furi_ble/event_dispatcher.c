#include "event_dispatcher.h"
#include <core/check.h>
#include <furi.h>
#include <ble/ble.h>

#include <m-list.h>

struct GapSvcEventHandler {
    void* context;
    BleServiceEventHandlerCb callback;
};

LIST_DEF(GapSvcEventHandlerList, GapSvcEventHandler, M_POD_OPLIST);

static GapSvcEventHandlerList_t handlers;
static bool initialized = false;

static SVCCTL_EvtAckStatus_t ble_service_event_dispatcher_dispatch_event(void* event) {
    furi_assert(initialized);

    GapSvcEventHandlerList_it_t it;
    SVCCTL_EvtAckStatus_t ack_status = SVCCTL_EvtNotAck;

    for(GapSvcEventHandlerList_it(it, handlers); !GapSvcEventHandlerList_end_p(it);
        GapSvcEventHandlerList_next(it)) {
        const GapSvcEventHandler* item = GapSvcEventHandlerList_cref(it);
        ack_status = item->callback(event, item->context);
        // Since we're replacing individual event handlers with a single event dispatcher, we need
        // to replicate its behavior of acking events. See AN5289, 7.6.10
        switch(ack_status) {
        case SVCCTL_EvtNotAck:
            continue; // Keep going
        case SVCCTL_EvtAckFlowEnable:
            return SVCCTL_EvtAckFlowEnable;
        case SVCCTL_EvtAckFlowDisable:
            return SVCCTL_EvtAckFlowDisable;
        }
    }

    return ack_status;
}

void ble_service_event_dispatcher_init() {
    furi_assert(!initialized);

    GapSvcEventHandlerList_init(handlers);
    SVCCTL_RegisterSvcHandler(ble_service_event_dispatcher_dispatch_event);
    initialized = true;
}

void ble_service_event_dispatcher_reset() {
    furi_assert(initialized);
    furi_check(GapSvcEventHandlerList_size(handlers) == 0);

    GapSvcEventHandlerList_clear(handlers);
}

GapSvcEventHandler*
    ble_service_event_dispatcher_register_handler(BleServiceEventHandlerCb handler, void* context) {
    furi_check(handler);
    furi_check(context);
    furi_check(initialized);

    GapSvcEventHandler* item = GapSvcEventHandlerList_push_raw(handlers);
    item->context = context;
    item->callback = handler;

    return item;
}

void ble_service_event_dispatcher_unregister_handler(GapSvcEventHandler* handler) {
    furi_check(handler);

    bool found = false;
    GapSvcEventHandlerList_it_t it;

    for(GapSvcEventHandlerList_it(it, handlers); !GapSvcEventHandlerList_end_p(it);
        GapSvcEventHandlerList_next(it)) {
        const GapSvcEventHandler* item = GapSvcEventHandlerList_cref(it);

        if(item == handler) {
            GapSvcEventHandlerList_remove(handlers, it);
            found = true;
            break;
        }
    }

    furi_check(found);
}
