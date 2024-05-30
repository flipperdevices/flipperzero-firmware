#include "epoll_i.h"
#include "message_queue_i.h"

FuriEpoll* furi_epoll_alloc(void) {
    FuriEpoll* instance = malloc(sizeof(FuriEpoll));

    instance->thread_id = furi_thread_get_current_id();
    FuriEpollTree_init(instance->tree);
    WaitingList_init(instance->waiting_list);

    return instance;
}

void furi_epoll_free(FuriEpoll* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    // WaitingList_clear(instance->waiting_list);
    FuriEpollTree_clear(instance->tree);
    free(instance);
}

static bool furi_epoll_poll_process_event(FuriEpoll* instance, FuriEpollItem* item) {
    UNUSED(instance);

    if(item->type == FuriEpollItemTypeMessageQueue) {
        if(!furi_message_queue_get_count(item->item)) {
            return false;
        }
        item->callback(item->item, item->callback_context);
    } else {
        furi_crash();
    }

    return true;
}

void furi_epoll_poll(FuriEpoll* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    uint32_t timeout = instance->tick_callback ? instance->tick_interval : FuriWaitForever;

    while(true) {
        uint32_t flags = 0;
        BaseType_t ret =
            xTaskNotifyWaitIndexed(FURI_EPOLL_NOTIFY_INDEX, 0, FuriEpollFlagAll, &flags, timeout);
        if(ret == pdTRUE) {
            if(flags & FuriEpollFlagStop) {
                break;
            } else if(flags & FuriEpollFlagEvent) {
                if(!WaitingList_empty_p(instance->waiting_list)) {
                    FuriEpollItem* item = WaitingList_pop_front(instance->waiting_list);
                    WaitingList_init_field(item);
                    while(furi_epoll_poll_process_event(instance, item))
                        ;
                }
            }
        } else {
            if(instance->tick_callback) {
                instance->tick_callback(instance->tick_callback_context);
            }
        }
    }
}

void furi_epoll_stop(FuriEpoll* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    xTaskNotifyIndexed(instance->thread_id, FURI_EPOLL_NOTIFY_INDEX, FuriEpollFlagStop, eSetBits);
}

void furi_epoll_tick_set(
    FuriEpoll* instance,
    uint32_t interval,
    FuriEpollTickCallback callback,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(callback ? interval > 0 : true);

    instance->tick_interval = interval;
    instance->tick_callback = callback;
    instance->tick_callback_context = context;
}

void furi_epoll_message_queue_add(
    FuriEpoll* instance,
    FuriMessageQueue* message_queue,
    FuriEpollEvent event,
    FuriEpollMessageQueueCallback callback,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(message_queue);

    FURI_CRITICAL_ENTER();

    furi_check(FuriEpollTree_get(instance->tree, message_queue) == NULL);

    // Allocate and setup item
    FuriEpollItem* item = furi_epoll_item_alloc(instance);
    furi_epoll_item_set_item(item, FuriEpollItemTypeMessageQueue, message_queue, event);
    furi_epoll_item_set_callback(item, callback, context);

    FuriEpollTree_set_at(instance->tree, message_queue, item);

    furi_message_queue_epoll_in_set(message_queue, item, item->event);

    FURI_CRITICAL_EXIT();
}

void furi_epoll_message_queue_remove(FuriEpoll* instance, FuriMessageQueue* message_queue) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    FURI_CRITICAL_ENTER();

    FuriEpollItem** item_ptr = FuriEpollTree_get(instance->tree, message_queue);
    furi_check(item_ptr);

    FuriEpollItem* item = *item_ptr;
    furi_check(item);
    furi_check(item->owner == instance);

    furi_message_queue_epoll_in_set(message_queue, NULL, item->event);

    furi_epoll_item_free(item);

    FuriEpollTree_erase(instance->tree, message_queue);

    FURI_CRITICAL_EXIT();
}

/* 
 * Epoll Item API, used internally
 */

FuriEpollItem* furi_epoll_item_alloc(FuriEpoll* owner) {
    FuriEpollItem* instance = malloc(sizeof(FuriEpollItem));

    instance->owner = owner;
    WaitingList_init_field(instance); // not really necessary

    return instance;
}

void furi_epoll_item_free(FuriEpollItem* instance) {
    furi_check(instance);
    free(instance);
}

void furi_epoll_item_set_item(
    FuriEpollItem* instance,
    FuriEpollItemType type,
    void* item,
    FuriEpollEvent event) {
    furi_check(instance);
    furi_check(!instance->item);

    instance->type = type;
    instance->item = item;
    instance->event = event;
}

void furi_epoll_item_set_callback(
    FuriEpollItem* instance,
    FuriEpollMessageQueueCallback callback,
    void* callback_context) {
    furi_check(instance);
    furi_check(!instance->callback);

    instance->callback = callback;
    instance->callback_context = callback_context;
}

void furi_epoll_item_notify(FuriEpollItem* instance, FuriEpollItemType type, FuriEpollEvent event) {
    furi_check(instance);
    furi_check(instance->type == type);
    furi_check(instance->event == event);

    if(!instance->WaitingList.prev && !instance->WaitingList.next) {
        WaitingList_push_back(instance->owner->waiting_list, instance);
    }

    xTaskNotifyIndexed(
        instance->owner->thread_id, FURI_EPOLL_NOTIFY_INDEX, FuriEpollFlagEvent, eSetBits);
}
