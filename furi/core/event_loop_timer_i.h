#pragma once

#include "event_loop_timer.h"

#include <m-list.h>
#include <m-i-list.h>

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

uint32_t furi_event_loop_get_wait_time(const FuriEventLoop* instance);

void furi_event_loop_process_request_queue(FuriEventLoop* instance);

bool furi_event_loop_process_expired_timer(FuriEventLoop* instance);

void furi_event_loop_process_tick(FuriEventLoop* instance);
