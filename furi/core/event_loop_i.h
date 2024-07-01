#pragma once

#include "event_loop.h"
#include "event_loop_link_i.h"
#include "event_loop_timer_i.h"

#include <m-bptree.h>
#include <m-i-list.h>

#include "thread.h"

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
    FuriEventLoopFlagRequest = (1 << 3),
} FuriEventLoopFlag;

#define FuriEventLoopFlagAll                                                   \
    (FuriEventLoopFlagEvent | FuriEventLoopFlagStop | FuriEventLoopFlagTimer | \
     FuriEventLoopFlagRequest)

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

    // Active timer list
    TimerList_t timer_list;
    // Timer request queue
    TimerQueue_t timer_queue;
    // Pending request handling
    RequestQueue_t request_queue;

    // Tick event
    uint32_t tick_interval;
    uint32_t tick_prev_time;
    FuriEventLoopTickCallback tick_callback;
    void* tick_callback_context;
};
