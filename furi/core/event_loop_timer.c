#include "event_loop_i.h"

#include "check.h"
#include "thread.h"

#include <FreeRTOS.h>
#include <task.h>

/*
 * Private functions
 */

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

static inline bool furi_event_loop_timer_is_expired(const FuriEventLoopTimer* timer) {
    return furi_event_loop_timer_get_elapsed_time(timer) >= timer->interval;
}

static inline bool furi_event_loop_tick_is_expired(const FuriEventLoop* instance) {
    return furi_event_loop_tick_get_elapsed_time(instance) >= instance->tick_interval;
}

static inline bool
    furi_event_loop_timer_in_list(const FuriEventLoop* instance, const FuriEventLoopTimer* timer) {
    TimerList_it_ct it;

    for(TimerList_it(it, instance->timer_list); !TimerList_end_p(it); TimerList_next(it)) {
        if(TimerList_cref(it) == timer) {
            return true;
        }
    }

    return false;
}

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

    // At this point, TimerList_front() points to the first timer to expire
}

static void furi_event_loop_enqueue_request(
    FuriEventLoop* instance,
    const FuriEventLoopRequestQueueItem* item) {
    RequestQueue_push_front(instance->request_queue, *item);

    xTaskNotifyIndexed(
        instance->thread_id, FURI_EVENT_LOOP_FLAG_NOTIFY_INDEX, FuriEventLoopFlagRequest, eSetBits);
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

/*
 * Private API
 */

uint32_t furi_event_loop_get_wait_time(const FuriEventLoop* instance) {
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

void furi_event_loop_process_request_queue(FuriEventLoop* instance) {
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

bool furi_event_loop_process_expired_timer(FuriEventLoop* instance) {
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

void furi_event_loop_process_tick(FuriEventLoop* instance) {
    if(instance->tick_callback && furi_event_loop_tick_is_expired(instance)) {
        instance->tick_prev_time = xTaskGetTickCount();
        instance->tick_callback(instance->tick_callback_context);
    }
}

/*
 * Public timer API
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
 * Public tick API
 *
 * Not directly related to timers, but it makes sense to keep it in this file.
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
 * Public deferred function call API
 *
 * Not directly related to timers, but it makes sense to keep it in this file.
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
