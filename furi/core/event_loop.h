/**
 * @file event_loop.h
 * @brief      Furi Event Loop
 *
 *             This module is designed to handle application event loop in fully
 *             asynchronous, reactive nature. On the low level this modules is
 *             inspired by epoll/kqueue concept, on the high level by asyncio
 *             event loop.
 *
 *             This module is trying to best fit into Furi OS, so we don't
 *             provide any compatibility with other event driven APIs. But
 *             programming concepts are the same, except some runtime
 *             limitations from our side.
 */
#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Event Loop events */
typedef enum {
    FuriEventLoopEventOut, /**< On departure: item was retrieved from container, flag reset, etc... */
    FuriEventLoopEventIn, /**< On arrival: item was inserted into container, flag set, etc... */
} FuriEventLoopEvent;

/** Anonymous message queue type */
typedef struct FuriEventLoop FuriEventLoop;

/** Allocate Event Loop instance
 *
 * Couple things to keep in mind:
 * - You can have 1 event_loop per 1 thread
 * - You can not use event_loop instance in the other thread
 * - Do not use blocking api to query object delegated to Event Loop
 *
 * @return     The Event Loop instance
 */
FuriEventLoop* furi_event_loop_alloc(void);

/** Free Event Loop instance
 *
 * @param      instance  The Event Loop instance
 */
void furi_event_loop_free(FuriEventLoop* instance);

/** Continuously poll for events
 *
 * Can be stopped with `furi_event_loop_stop`
 *
 * @param      instance  The Event Loop instance
 */
void furi_event_loop_run(FuriEventLoop* instance);

/** Stop Event Loop instance
 *
 * @param      instance  The Event Loop instance
 */
void furi_event_loop_stop(FuriEventLoop* instance);

/*
 * Timer related API
 */

/**
 * @brief Enumeration of possible timer types.
 */
typedef enum {
    FuriEventLoopTimerTypeOnce = 0, /**< One-shot timer. */
    FuriEventLoopTimerTypePeriodic = 1, /**< Repeating timer. */
} FuriEventLoopTimerType;

/**
 * @brief Timer callback type for functions to be called when a timer expires.
 *
 * In the timer callback, it is ALLOWED:
 * - To start, stop, or restart an existing timer,
 * - To create new timers using furi_event_loop_timer_alloc(),
 * - To delete timers using furi_event_loop_timer_free().
 *
 * @param[in] time_elapsed time counted by the timer since its start, in ticks
 * @param[in,out] context pointer to a user-specific object that was provided during timer creation
 */
typedef void (*FuriEventLoopTimerCallback)(uint32_t time_elapsed, void* context);

/**
 * @brief Opaque event loop timer type.
 */
typedef struct FuriEventLoopTimer FuriEventLoopTimer;

/**
 * @brief Create a new event loop timer instance.
 *
 * Event loop timers can ONLY be used in conjunction with a FuriEventLoop instance.
 * Each timer MUST be used in exactly ONE event loop.
 *
 * @param[in] callback pointer to the callback function to be executed upon timer timeout
 * @param[in] type timer type value to determine its behavior (signle-shot or periodic)
 * @param[in,out] context pointer to a user-specific object (will be passed to the callback)
 * @returns pointer to the created timer instance
 */
FuriEventLoopTimer* furi_event_loop_timer_alloc(
    FuriEventLoopTimerCallback callback,
    FuriEventLoopTimerType type,
    void* context);

/**
 * @brief Delete an event loop timer instance.
 *
 * @param[in,out] timer pointer to the timer instance to be deleted
 */
void furi_event_loop_timer_free(FuriEventLoopTimer* timer);

/**
 * @brief Start a timer or restart it with a new interval.
 *
 * @param[in,out] instance pointer to the current FuriEventLoop instance
 * @param[in,out] timer pointer to the timer instance to be (re)started
 * @param[in] interval timer interval in ticks
 */
void furi_event_loop_timer_start(
    FuriEventLoop* instance,
    FuriEventLoopTimer* timer,
    uint32_t interval);

/**
 * @brief Stop a timer without firing its callback.
 *
 * It is safe to call this function on an already stopped timer (it will do nothing).
 *
 * @param[in,out] timer pointer to the timer instance to be stopped
 */
void furi_event_loop_timer_stop(FuriEventLoopTimer* timer);

/**
 * @brief Get the time remaining before the timer becomes expires.
 *
 * For stopped or expired timers, this function returns 0.
 *
 * @param[in] timer pointer to the timer to be queried
 * @returns remaining time in ticks
 */
uint32_t furi_event_loop_timer_get_remaining_time(const FuriEventLoopTimer* timer);

/**
 * @brief Get the timer interval.
 *
 * @param[in] timer pointer to the timer to be queried
 * @returns timer interval in ticks
 */
uint32_t furi_event_loop_timer_get_interval(const FuriEventLoopTimer* timer);

/**
 * @brief Check if the timer is currently running.
 *
 * A timer is considered running if it has not expired yet.
 * @param[in] timer pointer to the timer to be queried
 * @returns true if the timer is running, false otherwise
 */
bool furi_event_loop_timer_is_running(const FuriEventLoopTimer* timer);

/*
 * Tick related API
 */

/** Tick callback type
 *
 * @param      context  The context for callback
 */
typedef void (*FuriEventLoopTickCallback)(void* context);

/** Set Event Loop tick callback
 *
 * Tick callback is called periodically after specified inactivity time.
 * It acts like a low-priority timer: it will only fire if there is time
 * left after processing the synchronisation primitives and the regular timers.
 * Therefore, it is not monotonic: ticks will be skipped if the event loop is busy.
 *
 * @param      instance  The Event Loop instance
 * @param[in]  interval  The tick interval
 * @param[in]  callback  The callback to call
 * @param      context   The context for callback
 */
void furi_event_loop_tick_set(
    FuriEventLoop* instance,
    uint32_t interval,
    FuriEventLoopTickCallback callback,
    void* context);

/*
 * Message queue related APIs
 */

/** Anonymous message queue type */
typedef struct FuriMessageQueue FuriMessageQueue;

/** Callback type for message queue
 *
 * @param      queue    The queue that triggered event
 * @param      context  The context that was provided on
 *                      furi_event_loop_message_queue_subscribe call
 *
 * @return     true if event was processed, false if we need to delay processing
 */
typedef bool (*FuriEventLoopMessageQueueCallback)(FuriMessageQueue* queue, void* context);

/** Subscribe to message queue events
 * 
 * @warning you can only have one subscription for one event type.
 *
 * @param      instance       The Event Loop instance
 * @param      message_queue  The message queue to add
 * @param[in]  event          The Event Loop event to trigger on
 * @param[in]  callback       The callback to call on event
 * @param      context        The context for callback
 */
void furi_event_loop_message_queue_subscribe(
    FuriEventLoop* instance,
    FuriMessageQueue* message_queue,
    FuriEventLoopEvent event,
    FuriEventLoopMessageQueueCallback callback,
    void* context);

/** Unsubscribe from message queue
 *
 * @param      instance       The Event Loop instance
 * @param      message_queue  The message queue
 */
void furi_event_loop_message_queue_unsubscribe(
    FuriEventLoop* instance,
    FuriMessageQueue* message_queue);

#ifdef __cplusplus
}
#endif
