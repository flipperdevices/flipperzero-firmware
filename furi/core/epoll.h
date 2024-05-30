/**
 * @file epoll.h
 * Event poller
 */
#pragma once

#include <core/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FuriEpollEventOut, /**< On departure: item was retrieved from container, flag reset, etc... */
    FuriEpollEventIn, /**< On arrival: item was inserted into container, flag set, etc... */
} FuriEpollEvent;

typedef struct FuriEpoll FuriEpoll;

/** Allocate Epoll instance
 * 
 * Couple things to keep in mind:
 * - You can have 1 epoll per 1 thread
 * - You can not use epoll instance in the other thread
 * - Do not use blocking api to query object delegated to Epoll
 *
 * @return     The Epoll instance
 */
FuriEpoll* furi_epoll_alloc(void);

/** Free Epoll instance
 *
 * @param      instance  The instance
 */
void furi_epoll_free(FuriEpoll* instance);

/** Continuously poll for events
 * 
 * Can be stopped with `furi_epoll_stop`
 *
 * @param      instance  The instance
 */
void furi_epoll_poll(FuriEpoll* instance);

/** Stop Epoll instance
 *
 * @param      instance  The instance
 */
void furi_epoll_stop(FuriEpoll* instance);

/*
 * Tick related API
 */

typedef void (*FuriEpollTickCallback)(void* context);

/** Set Epoll tick callback
 * 
 * Tick callback called after specified inactivity time.
 * It's not periodic. If Epoll is busy then ticks will be skipped.
 *
 * @param      instance  The Epoll instance
 * @param[in]  interval  The tick interval
 * @param[in]  callback  The callback to call
 * @param      context   The context for callback
 */
void furi_epoll_tick_set(
    FuriEpoll* instance,
    uint32_t interval,
    FuriEpollTickCallback callback,
    void* context);

/*
 * Message queue related APIs
 */
typedef struct FuriMessageQueue FuriMessageQueue;

typedef void (*FuriEpollMessageQueueCallback)(FuriMessageQueue* queue, void* context);

void furi_epoll_message_queue_add(
    FuriEpoll* instance,
    FuriMessageQueue* message_queue,
    FuriEpollEvent event,
    FuriEpollMessageQueueCallback callback,
    void* context);

void furi_epoll_message_queue_remove(FuriEpoll* instance, FuriMessageQueue* message_queue);

#ifdef __cplusplus
}
#endif
