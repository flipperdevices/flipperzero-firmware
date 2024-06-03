#include "core/event_loop.h"
#include "core/check.h"
#include "core/message_queue.h"
#include "event_loop_i.h"
#include "message_queue_i.h"

FuriEventLoop* furi_event_loop_alloc(void) {
    FuriEventLoop* instance = malloc(sizeof(FuriEventLoop));

    instance->thread_id = furi_thread_get_current_id();
    FuriEventLoopTree_init(instance->tree);
    WaitingList_init(instance->waiting_list);

    return instance;
}

void furi_event_loop_free(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    FuriEventLoopTree_clear(instance->tree);
    free(instance);
}

static FuriEventLoopProcessStatus
    furi_event_loop_poll_process_event(FuriEventLoop* instance, FuriEventLoopItem* item) {
    UNUSED(instance);

    if(item->type == FuriEventLoopItemTypeMessageQueue) {
        if(item->event == FuriEventLoopEventIn && !furi_message_queue_get_count(item->item)) {
            return FuriEventLoopProcessStatusComplete;
        }

        if(item->event == FuriEventLoopEventOut && !furi_message_queue_get_space(item->item)) {
            return FuriEventLoopProcessStatusComplete;
        }

        if(item->callback(item->item, item->callback_context)) {
            return FuriEventLoopProcessStatusIncomplete;
        } else {
            return FuriEventLoopProcessStatusAgain;
        }
    }

    furi_crash();
}

void furi_event_loop_run(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    uint32_t timeout = instance->tick_callback ? instance->tick_interval : FuriWaitForever;

    while(true) {
        uint32_t flags = 0;
        BaseType_t ret = xTaskNotifyWaitIndexed(
            FURI_EVENT_LOOP_NOTIFY_INDEX, 0, FuriEventLoopFlagAll, &flags, timeout);

        instance->state = FuriEventLoopStateProcessing;
        if(ret == pdTRUE) {
            if(flags & FuriEventLoopFlagStop) {
                instance->state = FuriEventLoopStateIdle;
                break;
            } else if(flags & FuriEventLoopFlagEvent) {
                FuriEventLoopItem* item = NULL;
                FURI_CRITICAL_ENTER();
                if(!WaitingList_empty_p(instance->waiting_list)) {
                    item = WaitingList_pop_front(instance->waiting_list);
                    WaitingList_init_field(item);
                }
                FURI_CRITICAL_EXIT();
                if(item) {
                    while(true) {
                        FuriEventLoopProcessStatus ret =
                            furi_event_loop_poll_process_event(instance, item);
                        if(ret == FuriEventLoopProcessStatusComplete) {
                            // Event processing complete, break from loop
                            break;
                        } else if(ret == FuriEventLoopProcessStatusIncomplete) {
                            // Event processing incomplete more processing needed
                        } else if(ret == FuriEventLoopProcessStatusAgain) {
                            furi_event_loop_item_notify(item, item->type, item->event);
                            break;
                        } else {
                            furi_crash();
                        }
                    }
                }
            }
        } else {
            if(instance->tick_callback) {
                instance->tick_callback(instance->tick_callback_context);
            }
        }
        instance->state = FuriEventLoopStateIdle;
    }
}

void furi_event_loop_stop(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    xTaskNotifyIndexed(
        instance->thread_id, FURI_EVENT_LOOP_NOTIFY_INDEX, FuriEventLoopFlagStop, eSetBits);
}

void furi_event_loop_tick_set(
    FuriEventLoop* instance,
    uint32_t interval,
    FuriEventLoopTickCallback callback,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(callback ? interval > 0 : true);

    instance->tick_interval = interval;
    instance->tick_callback = callback;
    instance->tick_callback_context = context;
}

void furi_event_loop_message_queue_subscribe(
    FuriEventLoop* instance,
    FuriMessageQueue* message_queue,
    FuriEventLoopEvent event,
    FuriEventLoopMessageQueueCallback callback,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(instance->state == FuriEventLoopStateIdle);
    furi_check(message_queue);

    FURI_CRITICAL_ENTER();

    furi_check(FuriEventLoopTree_get(instance->tree, message_queue) == NULL);

    // Allocate and setup item
    FuriEventLoopItem* item = furi_event_loop_item_alloc(instance);
    furi_event_loop_item_set_item(item, FuriEventLoopItemTypeMessageQueue, message_queue, event);
    furi_event_loop_item_set_callback(item, callback, context);

    FuriEventLoopTree_set_at(instance->tree, message_queue, item);

    furi_message_queue_event_loop_in_set(message_queue, item, item->event);

    if(item->event == FuriEventLoopEventIn && furi_message_queue_get_count(item->item)) {
        furi_event_loop_item_notify(item, item->type, item->event);
    }

    if(item->event == FuriEventLoopEventOut && furi_message_queue_get_space(item->item)) {
        furi_event_loop_item_notify(item, item->type, item->event);
    }

    FURI_CRITICAL_EXIT();
}

void furi_event_loop_message_queue_unsubscribe(
    FuriEventLoop* instance,
    FuriMessageQueue* message_queue) {
    furi_check(instance);
    furi_check(instance->state == FuriEventLoopStateIdle);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    FURI_CRITICAL_ENTER();

    FuriEventLoopItem** item_ptr = FuriEventLoopTree_get(instance->tree, message_queue);
    furi_check(item_ptr);

    FuriEventLoopItem* item = *item_ptr;
    furi_check(item);
    furi_check(item->owner == instance);

    furi_message_queue_event_loop_in_set(message_queue, NULL, item->event);

    furi_event_loop_item_free(item);

    FuriEventLoopTree_erase(instance->tree, message_queue);

    FURI_CRITICAL_EXIT();
}

/* 
 * Event Loop Item API, used internally
 */

FuriEventLoopItem* furi_event_loop_item_alloc(FuriEventLoop* owner) {
    FuriEventLoopItem* instance = malloc(sizeof(FuriEventLoopItem));

    instance->owner = owner;
    WaitingList_init_field(instance); // not really necessary

    return instance;
}

void furi_event_loop_item_free(FuriEventLoopItem* instance) {
    furi_check(instance);
    free(instance);
}

void furi_event_loop_item_set_item(
    FuriEventLoopItem* instance,
    FuriEventLoopItemType type,
    void* item,
    FuriEventLoopEvent event) {
    furi_check(instance);
    furi_check(!instance->item);

    instance->type = type;
    instance->item = item;
    instance->event = event;
}

void furi_event_loop_item_set_callback(
    FuriEventLoopItem* instance,
    FuriEventLoopMessageQueueCallback callback,
    void* callback_context) {
    furi_check(instance);
    furi_check(!instance->callback);

    instance->callback = callback;
    instance->callback_context = callback_context;
}

void furi_event_loop_item_notify(
    FuriEventLoopItem* instance,
    FuriEventLoopItemType type,
    FuriEventLoopEvent event) {
    furi_check(instance);
    furi_check(instance->type == type);
    furi_check(instance->event == event);

    FURI_CRITICAL_ENTER();
    if(!instance->WaitingList.prev && !instance->WaitingList.next) {
        WaitingList_push_back(instance->owner->waiting_list, instance);
    }
    FURI_CRITICAL_EXIT();

    xTaskNotifyIndexed(
        instance->owner->thread_id, FURI_EVENT_LOOP_NOTIFY_INDEX, FuriEventLoopFlagEvent, eSetBits);
}
