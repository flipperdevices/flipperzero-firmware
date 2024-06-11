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
#include "event_loop_contract.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Event Loop events */
typedef enum {
    FuriEventLoopEventOut, /**< On departure: item was retrieved from container, flag reset, etc... */
    FuriEventLoopEventIn, /**< On arrival: item was inserted into container, flag set, etc... */
} FuriEventLoopEvent;

/** Opaque event loop type */
typedef struct FuriEventLoop FuriEventLoop;

/** Opaque type of an object participating in event loop */
typedef void FuriEventLoopObject;

/** Callback type
 *
 * @param      object   The object that triggered the event
 * @param      context  The context that was provided on
 *                      furi_event_loop_subscribe call
 *
 * @return     true if event was processed, false if we need to delay processing
 */
typedef bool (*FuriEventLoopCallback)(FuriEventLoopObject* object, void* context);

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
 * Tick related API
 */

/** Tick callback type
 *
 * @param      context  The context for callback
 */
typedef void (*FuriEventLoopTickCallback)(void* context);

/** Set Event Loop tick callback
 *
 * Tick callback called after specified inactivity time. It's not periodic. If
 * Event Loop is busy then ticks will be skipped.
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

/** Subscribe to events produced by the object
 * 
 * @warning you can only have one subscription for one event type.
 *
 * @param      instance       The Event Loop instance
 * @param      object         The message object to add
 * @param[in]  contract       The Event Loop interface for the object
 * @param[in]  event          The Event Loop event to trigger on
 * @param[in]  callback       The callback to call on event
 * @param      context        The context for callback
 */
void furi_event_loop_subscribe(
    FuriEventLoop* instance,
    FuriEventLoopObject* object,
    const FuriEventLoopContract* contract,
    FuriEventLoopEvent event,
    FuriEventLoopCallback callback,
    void* context);

/** Unsubscribe from events produced by the object
 *
 * @param      instance       The Event Loop instance
 * @param      object         The object to unsubscribe from
 */
void furi_event_loop_unsubscribe(FuriEventLoop* instance, FuriEventLoopObject* object);

#ifdef __cplusplus
}
#endif
