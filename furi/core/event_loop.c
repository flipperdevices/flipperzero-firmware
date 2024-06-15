#include "event_loop_i.h"
#include "message_queue_i.h"

#include "log.h"
#include "check.h"
#include "thread.h"

#include <m-algo.h>
#include <m-list.h>
#include <m-bptree.h>
#include <m-i-list.h>

#include <FreeRTOS.h>
#include <task.h>

#define TAG "FuriEventLoop"

struct FuriEventLoopTimer {
    FuriEventLoop* owner;

    FuriEventLoopTimerCallback callback;
    void* context;

    uint32_t interval;
    uint32_t start_time;
    bool periodic;
};

LIST_DEF(TimerList, FuriEventLoopTimer*, M_PTR_OPLIST) // NOLINT

#define M_OPL_TimerList_t() LIST_OPLIST(TimerList, M_PTR_OPLIST)

ALGO_DEF(TimerList, TimerList_t) // NOLINT

struct FuriEventLoopItem {
    // Source
    FuriEventLoop* owner;

    // Tracking item
    const FuriEventLoopContract* contract;
    void* object;
    FuriEventLoopEvent event;

    // Callback and context
    FuriEventLoopMessageQueueCallback callback;
    void* callback_context;

    // Waiting list
    ILIST_INTERFACE(WaitingList, struct FuriEventLoopItem);
};

ILIST_DEF(WaitingList, FuriEventLoopItem, M_POD_OPLIST)

static FuriEventLoopItem* furi_event_loop_item_alloc(
    FuriEventLoop* owner,
    const FuriEventLoopContract* contract,
    void* object,
    FuriEventLoopEvent event);

static void furi_event_loop_item_free(FuriEventLoopItem* instance);

static void furi_event_loop_item_set_callback(
    FuriEventLoopItem* instance,
    FuriEventLoopMessageQueueCallback callback,
    void* callback_context);

static void furi_event_loop_item_notify(FuriEventLoopItem* instance);

/* Event Loop RB tree */
#define FURI_EVENT_LOOP_TREE_RANK (4)

BPTREE_DEF2( // NOLINT
    FuriEventLoopTree,
    FURI_EVENT_LOOP_TREE_RANK,
    void*, /* pointer to object we track */
    M_PTR_OPLIST,
    FuriEventLoopItem*, /* pointer to the FuriEventLoopItem */
    M_PTR_OPLIST)

#define M_OPL_FuriEventLoopTree_t() BPTREE_OPLIST(FuriEventLoopTree, M_POD_OPLIST)

#define FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX (2)

typedef enum {
    FuriEventLoopFlagEvent = (1 << 0),
    FuriEventLoopFlagStop = (1 << 1),
    FuriEventLoopFlagTimer = (1 << 2),
} FuriEventLoopFlag;

#define FuriEventLoopFlagAll \
    (FuriEventLoopFlagEvent | FuriEventLoopFlagStop | FuriEventLoopFlagTimer)

typedef enum {
    FuriEventLoopProcessStatusComplete,
    FuriEventLoopProcessStatusIncomplete,
    FuriEventLoopProcessStatusAgain,
} FuriEventLoopProcessStatus;

typedef enum {
    FuriEventLoopStateIdle,
    FuriEventLoopStateProcessing,
} FuriEventLoopState;

struct FuriEventLoop {
    // Only works if all operations are done from the same thread
    FuriThreadId thread_id;

    // Poller state
    volatile FuriEventLoopState state;

    // Tree
    FuriEventLoopTree_t tree;
    // Tree waiting list
    WaitingList_t waiting_list;
    // Timer list
    TimerList_t timer_list;
    TimerList_t ready_timer_list;

    // Tick event
    uint32_t tick_interval;
    FuriEventLoopTickCallback tick_callback;
    void* tick_callback_context;
};

FuriEventLoop* furi_event_loop_alloc(void) {
    FuriEventLoop* instance = malloc(sizeof(FuriEventLoop));

    instance->thread_id = furi_thread_get_current_id();

    FuriEventLoopTree_init(instance->tree);
    WaitingList_init(instance->waiting_list);
    TimerList_init(instance->timer_list);
    TimerList_init(instance->ready_timer_list);

    // Clear notification state and value
    xTaskNotifyStateClearIndexed(instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX);
    ulTaskNotifyValueClearIndexed(
        instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, 0xFFFFFFFF);

    return instance;
}

void furi_event_loop_free(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(TimerList_empty_p(instance->timer_list));

    FuriEventLoopTree_clear(instance->tree);
    WaitingList_clear(instance->waiting_list);
    TimerList_clear(instance->timer_list);
    TimerList_clear(instance->ready_timer_list);

    uint32_t flags = 0;
    BaseType_t ret = xTaskNotifyWaitIndexed(
        FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, 0, FuriEventLoopFlagAll, &flags, 0);
    if(ret == pdTRUE) {
        FURI_LOG_D(TAG, "Some events was not processed: 0x%lx", flags);
    }

    free(instance);
}

static FuriEventLoopProcessStatus
    furi_event_loop_poll_process_event(FuriEventLoop* instance, FuriEventLoopItem* item) {
    UNUSED(instance);

    if(!item->contract->get_level(item->object, item->event)) {
        return FuriEventLoopProcessStatusComplete;
    }

    if(item->callback(item->object, item->callback_context)) {
        return FuriEventLoopProcessStatusIncomplete;
    } else {
        return FuriEventLoopProcessStatusAgain;
    }
}

static inline uint32_t furi_event_loop_timer_get_elapsed_time(const FuriEventLoopTimer* timer) {
    return xTaskGetTickCount() - timer->start_time;
}

static inline uint32_t
    furi_event_loop_timer_get_remaining_time_private(const FuriEventLoopTimer* timer) {
    const uint32_t elapsed_time = furi_event_loop_timer_get_elapsed_time(timer);
    return elapsed_time < timer->interval ? timer->interval - elapsed_time : 0;
}

static uint32_t furi_event_loop_advance_timers(FuriEventLoop* instance) {
    uint32_t wait_time = FuriWaitForever;

    TimerList_it_t it;

    for(TimerList_it(it, instance->timer_list); !TimerList_end_p(it); TimerList_next(it)) {
        FuriEventLoopTimer* timer = *TimerList_ref(it);
        const uint32_t remaining_time = furi_event_loop_timer_get_remaining_time_private(timer);

        if(remaining_time < wait_time) {
            wait_time = remaining_time;
        }
    }

    return wait_time;
}

static inline bool furi_event_loop_timer_is_ready(const FuriEventLoopTimer* timer) {
    return furi_event_loop_timer_get_remaining_time(timer) == 0;
}

static void furi_event_loop_process_timers(FuriEventLoop* instance) {
    TimerList_it_t it;

    // Build a list of ready timers
    for(TimerList_it(it, instance->timer_list); !TimerList_end_p(it); TimerList_next(it)) {
        FuriEventLoopTimer* timer = *TimerList_ref(it);
        if(furi_event_loop_timer_is_ready(timer)) {
            TimerList_push_back(instance->ready_timer_list, timer);
        }
    }

    // Call respective callbacks for the ready timers
    for(TimerList_it(it, instance->ready_timer_list); !TimerList_end_p(it); TimerList_next(it)) {
        FuriEventLoopTimer* timer = *TimerList_ref(it);
        timer->callback(furi_event_loop_timer_get_elapsed_time(timer), timer->context);
    }

    // Reset the periodic timers and delete inactive single-shot timers
    for(TimerList_it(it, instance->timer_list); !TimerList_end_p(it);) {
        FuriEventLoopTimer* timer = *TimerList_ref(it);

        if(furi_event_loop_timer_is_ready(timer)) {
            if(timer->periodic) {
                timer->start_time = xTaskGetTickCount();

            } else {
                TimerList_remove(instance->timer_list, it);
                timer->owner = NULL;
                continue;
            }
        }

        TimerList_next(it);
    }

    TimerList_reset(instance->ready_timer_list);
}

void furi_event_loop_run(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    while(true) {
        const TickType_t xTicksToWait = furi_event_loop_advance_timers(instance);

        uint32_t flags = 0;
        BaseType_t ret = xTaskNotifyWaitIndexed(
            FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, 0, FuriEventLoopFlagAll, &flags, xTicksToWait);

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
                        } else if(ret == FuriEventLoopProcessStatusAgain) { //-V547
                            furi_event_loop_item_notify(item);
                            break;
                        } else {
                            furi_crash();
                        }
                    }
                }
            }

        } else {
            furi_event_loop_process_timers(instance);
        }

        instance->state = FuriEventLoopStateIdle;
    }
}

void furi_event_loop_stop(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    xTaskNotifyIndexed(
        instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, FuriEventLoopFlagStop, eSetBits);
}

/*
 * Timer API
 */

FuriEventLoopTimer* furi_event_loop_timer_alloc(
    FuriEventLoopTimerCallback callback,
    FuriEventLoopTimerType type,
    void* context) {
    furi_check(callback);
    furi_check(type <= FuriEventLoopTimerTypePeriodic);

    FuriEventLoopTimer* timer = malloc(sizeof(FuriEventLoopTimer));

    timer->callback = callback;
    timer->context = context;
    timer->periodic = (type == FuriEventLoopTimerTypePeriodic);

    return timer;
}

void furi_event_loop_timer_free(FuriEventLoopTimer* timer) {
    furi_check(timer);
    furi_check(timer->owner == NULL);

    free(timer);
}

void furi_event_loop_timer_start(
    FuriEventLoop* instance,
    FuriEventLoopTimer* timer,
    uint32_t interval) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(timer);

    if(!TimerList_contain_p(instance->timer_list, timer)) {
        furi_check(timer->owner == NULL);
        TimerList_push_back(instance->timer_list, timer);
    }

    timer->owner = instance;
    timer->interval = interval;
    timer->start_time = xTaskGetTickCount();

    xTaskNotifyIndexed(
        instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, FuriEventLoopFlagTimer, eSetBits);
}

void furi_event_loop_timer_stop(FuriEventLoop* instance, FuriEventLoopTimer* timer) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(timer);

    if(!timer->owner) {
        // Stopping an already stopped timer does nothing
        return;
    }

    TimerList_it_t it;
    TimerList_find(it, instance->timer_list, timer);

    furi_check(!TimerList_end_p(it));
    TimerList_remove(instance->timer_list, it);

    timer->owner = NULL;
}

uint32_t furi_event_loop_timer_get_remaining_time(const FuriEventLoopTimer* timer) {
    furi_check(timer);
    return furi_event_loop_timer_get_remaining_time_private(timer);
}

uint32_t furi_event_loop_timer_get_interval(const FuriEventLoopTimer* timer) {
    furi_check(timer);
    return timer->interval;
}

bool furi_event_loop_timer_is_running(const FuriEventLoopTimer* timer) {
    furi_check(timer);
    return timer->owner && !furi_event_loop_timer_is_ready(timer);
}

/*
 * Tick API
 */

void furi_event_loop_tick_set(
    FuriEventLoop* instance,
    uint32_t interval,
    FuriEventLoopTickCallback callback,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(callback);
    furi_check(interval > 0);

    instance->tick_interval = interval;
    instance->tick_callback = callback;
    instance->tick_callback_context = context;
}

/*
 * Message queue API
 */

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
    FuriEventLoopItem* item = furi_event_loop_item_alloc(
        instance, &furi_message_queue_event_loop_contract, message_queue, event);
    furi_event_loop_item_set_callback(item, callback, context);

    FuriEventLoopTree_set_at(instance->tree, message_queue, item);

    FuriEventLoopLink* link = item->contract->get_link(message_queue);

    if(item->event == FuriEventLoopEventIn) {
        furi_check(link->item_in == NULL);
        link->item_in = item;
    } else if(item->event == FuriEventLoopEventOut) {
        furi_check(link->item_out == NULL);
        link->item_out = item;
    } else {
        furi_crash();
    }

    if(item->contract->get_level(item->object, item->event)) {
        furi_event_loop_item_notify(item);
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

    FuriEventLoopLink* link = item->contract->get_link(message_queue);

    if(item->event == FuriEventLoopEventIn) {
        furi_check(link->item_in == item);
        link->item_in = NULL;
    } else if(item->event == FuriEventLoopEventOut) {
        furi_check(link->item_out == item);
        link->item_out = NULL;
    } else {
        furi_crash();
    }

    furi_event_loop_item_free(item);

    FuriEventLoopTree_erase(instance->tree, message_queue);

    FURI_CRITICAL_EXIT();
}

/* 
 * Event Loop Item API, used internally
 */

static FuriEventLoopItem* furi_event_loop_item_alloc(
    FuriEventLoop* owner,
    const FuriEventLoopContract* contract,
    void* object,
    FuriEventLoopEvent event) {
    furi_assert(owner);
    furi_assert(object);

    FuriEventLoopItem* instance = malloc(sizeof(FuriEventLoopItem));

    instance->owner = owner;
    instance->contract = contract;
    instance->object = object;
    instance->event = event;

    WaitingList_init_field(instance);

    return instance;
}

static void furi_event_loop_item_free(FuriEventLoopItem* instance) {
    furi_assert(instance);
    free(instance);
}

static void furi_event_loop_item_set_callback(
    FuriEventLoopItem* instance,
    FuriEventLoopMessageQueueCallback callback,
    void* callback_context) {
    furi_assert(instance);
    furi_assert(!instance->callback);

    instance->callback = callback;
    instance->callback_context = callback_context;
}

static void furi_event_loop_item_notify(FuriEventLoopItem* instance) {
    furi_assert(instance);

    FURI_CRITICAL_ENTER();

    if(!instance->WaitingList.prev && !instance->WaitingList.next) {
        WaitingList_push_back(instance->owner->waiting_list, instance);
    }

    FURI_CRITICAL_EXIT();

    xTaskNotifyIndexed(
        instance->owner->thread_id,
        FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX,
        FuriEventLoopFlagEvent,
        eSetBits);
}

void furi_event_loop_link_notify(FuriEventLoopLink* instance, FuriEventLoopEvent event) {
    furi_assert(instance);

    FURI_CRITICAL_ENTER();

    if(event == FuriEventLoopEventIn) {
        if(instance->item_in) furi_event_loop_item_notify(instance->item_in);
    } else if(event == FuriEventLoopEventOut) {
        if(instance->item_out) furi_event_loop_item_notify(instance->item_out);
    } else {
        furi_crash();
    }

    FURI_CRITICAL_EXIT();
}
