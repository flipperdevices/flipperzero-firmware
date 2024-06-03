#pragma once

#include "event_loop.h"

#include "check.h"
#include "thread.h"
#include "mutex.h"

#include "m-bptree.h"
#include "m-i-list.h"

#include <FreeRTOS.h>
#include <task.h>

#define FURI_EVENT_LOOP_NOTIFY_INDEX (2)

typedef enum {
    FuriEventLoopFlagEvent = (1 << 0),
    FuriEventLoopFlagStop = (1 << 1),
} FuriEventLoopFlag;

#define FuriEventLoopFlagAll (FuriEventLoopFlagEvent | FuriEventLoopFlagStop)

typedef enum {
    FuriEventLoopProcessStatusComplete,
    FuriEventLoopProcessStatusIncomplete,
    FuriEventLoopProcessStatusAgain,
} FuriEventLoopProcessStatus;

typedef enum {
    FuriEventLoopItemTypeMessageQueue,
} FuriEventLoopItemType;

typedef struct FuriEventLoopItem FuriEventLoopItem;

struct FuriEventLoopItem {
    // Source
    FuriEventLoop* owner;

    // Tracking item
    FuriEventLoopItemType type;
    void* item;
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

BPTREE_DEF2(
    FuriEventLoopTree,
    FURI_EVENT_LOOP_TREE_RANK,
    void*, /* pointer to object we track */
    M_PTR_OPLIST,
    FuriEventLoopItem*, /* pointer to the FuriEventLoopItem */
    M_PTR_OPLIST)

#define M_OPL_FuriEventLoopTree_t() BPTREE_OPLIST(FuriEventLoopTree, M_POD_OPLIST)

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

    // Tick event
    uint32_t tick_interval;
    FuriEventLoopTickCallback tick_callback;
    void* tick_callback_context;
};

FuriEventLoopItem* furi_event_loop_item_alloc(FuriEventLoop* owner);

void furi_event_loop_item_free(FuriEventLoopItem* instance);

void furi_event_loop_item_set_item(
    FuriEventLoopItem* instance,
    FuriEventLoopItemType type,
    void* item,
    FuriEventLoopEvent event);

void furi_event_loop_item_set_callback(
    FuriEventLoopItem* instance,
    FuriEventLoopMessageQueueCallback callback,
    void* callback_context);

void furi_event_loop_item_notify(
    FuriEventLoopItem* instance,
    FuriEventLoopItemType type,
    FuriEventLoopEvent event);
