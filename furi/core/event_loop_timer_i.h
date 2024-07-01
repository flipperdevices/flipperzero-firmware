#pragma once

#include "event_loop_timer.h"

#include <m-list.h>
#include <m-i-list.h>

typedef enum {
    FuriEventLoopTimerRequestStart,
    FuriEventLoopTimerRequestStop,
    FuriEventLoopTimerRequestFree,
} FuriEventLoopTimerRequest;

struct FuriEventLoopTimer {
    FuriEventLoop* owner;

    FuriEventLoopTimerCallback callback;
    void* context;

    uint32_t interval;
    uint32_t start_time;
    uint32_t next_interval;

    // Interface for the active timer list
    ILIST_INTERFACE(TimerList, FuriEventLoopTimer);

    // Interface for the timer request queue
    ILIST_INTERFACE(TimerQueue, FuriEventLoopTimer);

    FuriEventLoopTimerRequest request;

    bool active;
    bool periodic;
    bool request_pending;
};

ILIST_DEF(TimerList, FuriEventLoopTimer, M_POD_OPLIST)
ILIST_DEF(TimerQueue, FuriEventLoopTimer, M_POD_OPLIST)

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
    FuriEventLoopRequestTypeTick,
    FuriEventLoopRequestTypePending,
} FuriEventLoopRequestType;

typedef struct {
    FuriEventLoopRequestType type;
    union {
        FuriEventLoopTickRequest tick_request;
        FuriEventLoopPendingRequest pending_request;
    };
} FuriEventLoopRequestQueueItem;

LIST_DUAL_PUSH_DEF(RequestQueue, FuriEventLoopRequestQueueItem, M_POD_OPLIST)

uint32_t furi_event_loop_get_wait_time(const FuriEventLoop* instance);

void furi_event_loop_process_timer_queue(FuriEventLoop* instance);

void furi_event_loop_process_request_queue(FuriEventLoop* instance);

bool furi_event_loop_process_expired_timer(FuriEventLoop* instance);

void furi_event_loop_process_tick(FuriEventLoop* instance);
