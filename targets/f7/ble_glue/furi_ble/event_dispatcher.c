#include "event_dispatcher.h"
#include <core/check.h>
#include <furi.h>
#include <ble/ble.h>

#include <m-list.h>

struct GapEventHandler {
    void* context;
    BleSvcEventHandlerCb callback;
};

LIST_DEF(GapSvcEventHandlerList, GapSvcEventHandler, M_POD_OPLIST);

static GapSvcEventHandlerList_t handlers;
static bool initialized = false;
static FuriMutex* handlers_mutex = NULL;

BleEventFlowStatus ble_event_dispatcher_process_event(void* payload) {
    furi_check(initialized);
    furi_check(furi_mutex_acquire(handlers_mutex, FuriWaitForever) == FuriStatusOk);

    GapSvcEventHandlerList_it_t it;
    BleEventAckStatus ack_status = BleEventNotAck;

    for(GapSvcEventHandlerList_it(it, handlers); !GapSvcEventHandlerList_end_p(it);
        GapSvcEventHandlerList_next(it)) {
        const GapSvcEventHandler* item = GapSvcEventHandlerList_cref(it);
        ack_status = item->callback(payload, item->context);
        if(ack_status == BleEventNotAck) {
            /* Keep going */
            continue;
        } else if((ack_status == BleEventAckFlowEnable) || (ack_status == BleEventAckFlowDisable)) {
            break;
        }
    }

    /* Registration and removal wait for callback execution to finish. This is
     * what makes it safe for a service to free its callback context immediately
     * after unregister_svc_handler returns. */
    furi_check(furi_mutex_release(handlers_mutex) == FuriStatusOk);

    /* Handlers for client-mode events are also to be implemented here. But not today. */

    /* Now, decide on a flow control action based on results of all handlers */
    switch(ack_status) {
    case BleEventNotAck:
        /* The event has NOT been managed yet. Pass to app for processing */
        return ble_event_app_notification(payload);
    case BleEventAckFlowEnable:
        return BleEventFlowEnable;
    case BleEventAckFlowDisable:
        return BleEventFlowDisable;
    default:
        return BleEventFlowEnable;
    }
}

void ble_event_dispatcher_init(void) {
    if(!initialized) {
        GapSvcEventHandlerList_init(handlers);
        handlers_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
        initialized = true;
    }
}

void ble_event_dispatcher_reset(void) {
    furi_check(initialized);
    furi_check(furi_mutex_acquire(handlers_mutex, FuriWaitForever) == FuriStatusOk);
    furi_check(GapSvcEventHandlerList_size(handlers) == 0);

    GapSvcEventHandlerList_clear(handlers);
    furi_check(furi_mutex_release(handlers_mutex) == FuriStatusOk);
}

GapSvcEventHandler*
    ble_event_dispatcher_register_svc_handler(BleSvcEventHandlerCb handler, void* context) {
    furi_check(handler);
    furi_check(context);
    furi_check(initialized);
    furi_check(furi_mutex_acquire(handlers_mutex, FuriWaitForever) == FuriStatusOk);

    GapSvcEventHandler* item = GapSvcEventHandlerList_push_raw(handlers);
    item->context = context;
    item->callback = handler;
    furi_check(furi_mutex_release(handlers_mutex) == FuriStatusOk);

    return item;
}

void ble_event_dispatcher_unregister_svc_handler(GapSvcEventHandler* handler) {
    furi_check(handler);
    furi_check(furi_mutex_acquire(handlers_mutex, FuriWaitForever) == FuriStatusOk);

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

    furi_check(furi_mutex_release(handlers_mutex) == FuriStatusOk);
    furi_check(found);
}
