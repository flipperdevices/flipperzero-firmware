#include "event_loop_i.h"
#include "message_queue_i.h"

#include "log.h"
#include "check.h"
#include "thread.h"

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

    ILIST_INTERFACE(TimerList, FuriEventLoopTimer);
};

ILIST_DEF(TimerList, FuriEventLoopTimer, M_POD_OPLIST)

typedef enum {
    FuriEventLoopTimerRequestTypeStart,
    FuriEventLoopTimerRequestTypeStop,
    FuriEventLoopTimerRequestTypeFree,
} FuriEventLoopTimerRequestType;

typedef struct {
    FuriEventLoopTimerRequestType type;
    FuriEventLoopTimer* timer;
    uint32_t interval;
} FuriEventLoopTimerRequest;

typedef struct {
    FuriEventLoopTickCallback callback;
    uint32_t interval;
    void* context;
} FuriEventLoopTickRequest;

typedef struct {
    FuriEventLoopPendingCallback callback;
    void* context;
} FuriEventLoopPendingRequest;

typedef enum {
    FuriEventLoopRequestTypeTimer,
    FuriEventLoopRequestTypeTick,
    FuriEventLoopRequestTypePending,
} FuriEventLoopRequestType;

typedef struct {
    FuriEventLoopRequestType type;
    union {
        FuriEventLoopTimerRequest timer_request;
        FuriEventLoopTickRequest tick_request;
        FuriEventLoopPendingRequest pending_request;
    };
} FuriEventLoopRequestQueueItem;

LIST_DUAL_PUSH_DEF(RequestQueue, FuriEventLoopRequestQueueItem, M_POD_OPLIST)

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
    FuriEventLoopFlagRequest = (1 << 2),
} FuriEventLoopFlag;

#define FuriEventLoopFlagAll \
    (FuriEventLoopFlagEvent | FuriEventLoopFlagStop | FuriEventLoopFlagRequest)

typedef enum {
    FuriEventLoopProcessStatusComplete,
    FuriEventLoopProcessStatusIncomplete,
    FuriEventLoopProcessStatusAgain,
} FuriEventLoopProcessStatus;

typedef enum {
    FuriEventLoopStateStopped,
    FuriEventLoopStateIdle,
    FuriEventLoopStateProcessing,
} FuriEventLoopState;

struct FuriEventLoop {
    // Only works if all operations are done from the same thread
    FuriThreadId thread_id;

    // Poller state
    volatile FuriEventLoopState state;

    // Event handling
    FuriEventLoopTree_t tree;
    WaitingList_t waiting_list;

    // Pending request handling
    TimerList_t timer_list;
    RequestQueue_t request_queue;

    // Tick event
    uint32_t tick_interval;
    uint32_t tick_prev_time;
    FuriEventLoopTickCallback tick_callback;
    void* tick_callback_context;
};

FuriEventLoop* furi_event_loop_alloc(void) {
    FuriEventLoop* instance = malloc(sizeof(FuriEventLoop));

    instance->thread_id = furi_thread_get_current_id();

    FuriEventLoopTree_init(instance->tree);
    WaitingList_init(instance->waiting_list);
    TimerList_init(instance->timer_list);
    RequestQueue_init(instance->request_queue);

    // Clear notification state and value
    xTaskNotifyStateClearIndexed(instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX);
    ulTaskNotifyValueClearIndexed(
        instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, 0xFFFFFFFF);

    return instance;
}

static void furi_event_loop_process_request_queue(FuriEventLoop* instance);

void furi_event_loop_free(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(instance->state == FuriEventLoopStateStopped);

    furi_event_loop_process_request_queue(instance);
    furi_check(TimerList_empty_p(instance->timer_list));

    FuriEventLoopTree_clear(instance->tree);
    RequestQueue_clear(instance->request_queue);

    uint32_t flags = 0;
    BaseType_t ret = xTaskNotifyWaitIndexed(
        FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, 0, FuriEventLoopFlagAll, &flags, 0);
    if(ret == pdTRUE) {
        FURI_LOG_D(TAG, "Some events were not processed: 0x%lx", flags);
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

static inline uint32_t furi_event_loop_tick_get_elapsed_time(const FuriEventLoop* instance) {
    return xTaskGetTickCount() - instance->tick_prev_time;
}

static inline uint32_t
    furi_event_loop_timer_get_remaining_time_private(const FuriEventLoopTimer* timer) {
    const uint32_t elapsed_time = furi_event_loop_timer_get_elapsed_time(timer);
    return elapsed_time < timer->interval ? timer->interval - elapsed_time : 0;
}

static inline uint32_t furi_event_loop_tick_get_remaining_time(const FuriEventLoop* instance) {
    const uint32_t elapsed_time = furi_event_loop_tick_get_elapsed_time(instance);
    return elapsed_time < instance->tick_interval ? instance->tick_interval - elapsed_time : 0;
}

static uint32_t furi_event_loop_get_wait_time(const FuriEventLoop* instance) {
    uint32_t wait_time = FuriWaitForever;

    if(!TimerList_empty_p(instance->timer_list)) {
        FuriEventLoopTimer* timer = TimerList_front(instance->timer_list);
        wait_time = furi_event_loop_timer_get_remaining_time_private(timer);
    }

    if(instance->tick_callback) {
        wait_time = MIN(wait_time, furi_event_loop_tick_get_remaining_time(instance));
    }

    return wait_time;
}

static inline bool furi_event_loop_timer_is_expired(const FuriEventLoopTimer* timer) {
    return furi_event_loop_timer_get_elapsed_time(timer) >= timer->interval;
}

static inline bool furi_event_loop_tick_is_expired(const FuriEventLoop* instance) {
    return furi_event_loop_tick_get_elapsed_time(instance) >= instance->tick_interval;
}

// Schedule the timer so that front() points to the earliest-expiring timer
static void furi_event_loop_schedule_timer(FuriEventLoop* instance, FuriEventLoopTimer* timer) {
    FuriEventLoopTimer* timer_pos = NULL;

    FURI_CRITICAL_ENTER();

    const uint32_t remaining_time = furi_event_loop_timer_get_remaining_time_private(timer);

    TimerList_it_t it;
    for(TimerList_it_last(it, instance->timer_list); !TimerList_end_p(it);
        TimerList_previous(it)) {
        FuriEventLoopTimer* tmp = TimerList_ref(it);
        if(remaining_time >= furi_event_loop_timer_get_remaining_time_private(tmp)) {
            timer_pos = tmp;
            break;
        }
    }

    FURI_CRITICAL_EXIT();

    if(timer_pos) {
        TimerList_push_after(timer_pos, timer);
    } else {
        TimerList_push_front(instance->timer_list, timer);
    }
}

static bool furi_event_loop_process_expired_timer(FuriEventLoop* instance) {
    if(TimerList_empty_p(instance->timer_list)) {
        return false;
    }
    // The front() element contains the earliest-expiring timer
    FuriEventLoopTimer* timer = TimerList_front(instance->timer_list);

    if(!furi_event_loop_timer_is_expired(timer)) {
        return false;
    }

    TimerList_unlink(timer);

    if(timer->periodic) {
        timer->start_time = xTaskGetTickCount();
        furi_event_loop_schedule_timer(instance, timer);
    }

    timer->callback(timer->context);
    return true;
}

static bool
    furi_event_loop_timer_in_list(const FuriEventLoop* instance, const FuriEventLoopTimer* timer) {
    TimerList_it_ct it;

    for(TimerList_it(it, instance->timer_list); !TimerList_end_p(it); TimerList_next(it)) {
        if(TimerList_cref(it) == timer) {
            return true;
        }
    }

    return false;
}

static void furi_event_loop_process_timer_request(
    FuriEventLoop* instance,
    const FuriEventLoopTimerRequest* request) {
    FuriEventLoopTimer* timer = request->timer;

    if(furi_event_loop_timer_in_list(instance, timer)) {
        TimerList_unlink(timer);
    }

    if(request->type == FuriEventLoopTimerRequestTypeStart) {
        timer->interval = request->interval;
        timer->start_time = xTaskGetTickCount();

        furi_event_loop_schedule_timer(instance, timer);

    } else if(request->type == FuriEventLoopTimerRequestTypeStop) {
        // Do nothing

    } else if(request->type == FuriEventLoopTimerRequestTypeFree) {
        free(timer);

    } else {
        furi_crash();
    }
}

static void furi_event_loop_process_tick_request(
    FuriEventLoop* instance,
    const FuriEventLoopTickRequest* request) {
    instance->tick_callback = request->callback;
    instance->tick_callback_context = request->context;

    if(request->callback) {
        instance->tick_interval = request->interval;
        instance->tick_prev_time = xTaskGetTickCount();
    }
}

static void furi_event_loop_process_request_queue(FuriEventLoop* instance) {
    for(; !RequestQueue_empty_p(instance->request_queue);
        RequestQueue_pop_back(NULL, instance->request_queue)) {
        const FuriEventLoopRequestQueueItem* item = RequestQueue_back(instance->request_queue);

        if(item->type == FuriEventLoopRequestTypeTimer) {
            const FuriEventLoopTimerRequest* request = &item->timer_request;
            furi_event_loop_process_timer_request(instance, request);

        } else if(item->type == FuriEventLoopRequestTypeTick) {
            const FuriEventLoopTickRequest* request = &item->tick_request;
            furi_event_loop_process_tick_request(instance, request);

        } else if(item->type == FuriEventLoopRequestTypePending) {
            const FuriEventLoopPendingRequest* request = &item->pending_request;
            request->callback(request->context);

        } else {
            furi_crash();
        }
    }
}

static void furi_event_loop_process_tick(FuriEventLoop* instance) {
    if(instance->tick_callback && furi_event_loop_tick_is_expired(instance)) {
        instance->tick_prev_time = xTaskGetTickCount();
        instance->tick_callback(instance->tick_callback_context);
    }
}

static void furi_event_loop_enqueue_request(
    FuriEventLoop* instance,
    const FuriEventLoopRequestQueueItem* item) {
    RequestQueue_push_front(instance->request_queue, *item);

    xTaskNotifyIndexed(
        instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, FuriEventLoopFlagRequest, eSetBits);
}

static void furi_event_loop_restore_flags(FuriEventLoop* instance, uint32_t flags) {
    if(flags) {
        xTaskNotifyIndexed(
            instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, flags, eSetBits);
    }
}

void furi_event_loop_run(FuriEventLoop* instance) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());

    while(true) {
        instance->state = FuriEventLoopStateIdle;

        const TickType_t xTicksToWait = furi_event_loop_get_wait_time(instance);

        uint32_t flags = 0;
        BaseType_t ret = xTaskNotifyWaitIndexed(
            FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, 0, FuriEventLoopFlagAll, &flags, xTicksToWait);

        instance->state = FuriEventLoopStateProcessing;

        if(ret == pdTRUE) {
            if(flags & FuriEventLoopFlagStop) {
                instance->state = FuriEventLoopStateStopped;
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

                furi_event_loop_restore_flags(instance, flags & ~FuriEventLoopFlagEvent);

            } else if(flags & FuriEventLoopFlagRequest) {
                furi_event_loop_process_request_queue(instance);

            } else {
                furi_crash();
            }

        } else if(!furi_event_loop_process_expired_timer(instance)) {
            furi_event_loop_process_tick(instance);
        }
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
    FuriEventLoop* instance,
    FuriEventLoopTimerCallback callback,
    FuriEventLoopTimerType type,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(callback);
    furi_check(type <= FuriEventLoopTimerTypePeriodic);

    FuriEventLoopTimer* timer = malloc(sizeof(FuriEventLoopTimer));

    timer->owner = instance;
    timer->callback = callback;
    timer->context = context;
    timer->periodic = (type == FuriEventLoopTimerTypePeriodic);

    TimerList_init_field(timer);

    return timer;
}

void furi_event_loop_timer_free(FuriEventLoopTimer* timer) {
    furi_check(timer);
    furi_check(timer->owner->thread_id == furi_thread_get_current_id());

    const FuriEventLoopRequestQueueItem item = {
        .type = FuriEventLoopRequestTypeTimer,
        .timer_request =
            {
                .type = FuriEventLoopTimerRequestTypeFree,
                .timer = timer,
            },
    };

    furi_event_loop_enqueue_request(timer->owner, &item);
}

void furi_event_loop_timer_start(FuriEventLoopTimer* timer, uint32_t interval) {
    furi_check(timer);
    furi_check(timer->owner->thread_id == furi_thread_get_current_id());

    const FuriEventLoopRequestQueueItem item = {
        .type = FuriEventLoopRequestTypeTimer,
        .timer_request =
            {
                .type = FuriEventLoopTimerRequestTypeStart,
                .timer = timer,
                .interval = interval,
            },
    };

    furi_event_loop_enqueue_request(timer->owner, &item);
}

void furi_event_loop_timer_restart(FuriEventLoopTimer* timer) {
    furi_check(timer);
    furi_check(timer->owner->thread_id == furi_thread_get_current_id());

    const FuriEventLoopRequestQueueItem item = {
        .type = FuriEventLoopRequestTypeTimer,
        .timer_request =
            {
                .type = FuriEventLoopTimerRequestTypeStart,
                .timer = timer,
                .interval = timer->interval,
            },
    };

    furi_event_loop_enqueue_request(timer->owner, &item);
}

void furi_event_loop_timer_stop(FuriEventLoopTimer* timer) {
    furi_check(timer);
    furi_check(timer->owner->thread_id == furi_thread_get_current_id());

    const FuriEventLoopRequestQueueItem item = {
        .type = FuriEventLoopRequestTypeTimer,
        .timer_request =
            {
                .type = FuriEventLoopTimerRequestTypeStop,
                .timer = timer,
            },
    };

    furi_event_loop_enqueue_request(timer->owner, &item);
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
    return !furi_event_loop_timer_is_expired(timer);
}

/*
 * Deferred function call API
 */

void furi_event_loop_pend_callback(
    FuriEventLoop* instance,
    FuriEventLoopPendingCallback callback,
    void* context) {
    furi_check(instance);
    furi_check(instance->thread_id == furi_thread_get_current_id());
    furi_check(callback);

    const FuriEventLoopRequestQueueItem item = {
        .type = FuriEventLoopRequestTypePending,
        .pending_request =
            {
                .callback = callback,
                .context = context,
            },
    };

    furi_event_loop_enqueue_request(instance, &item);
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
    furi_check(callback ? interval > 0 : true);

    const FuriEventLoopRequestQueueItem item = {
        .type = FuriEventLoopRequestTypeTick,
        .tick_request =
            {
                .callback = callback,
                .interval = interval,
                .context = context,
            },
    };

    furi_event_loop_enqueue_request(instance, &item);
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
    furi_check(instance->state == FuriEventLoopStateStopped);
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
    furi_check(instance->state == FuriEventLoopStateStopped);
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
