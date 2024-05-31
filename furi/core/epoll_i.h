#pragma once

#include "epoll.h"

#include "check.h"
#include "thread.h"
#include "mutex.h"

#include "m-bptree.h"
#include "m-i-list.h"

#include <FreeRTOS.h>
#include <task.h>

#define FURI_EPOLL_NOTIFY_INDEX (2)

typedef enum {
    FuriEpollFlagEvent = (1 << 0),
    FuriEpollFlagStop = (1 << 1),
} FuriEpollFlag;

#define FuriEpollFlagAll (FuriEpollFlagEvent | FuriEpollFlagStop)

typedef enum {
    FuriEpollProcessStatusComplete,
    FuriEpollProcessStatusIncomplete,
    FuriEpollProcessStatusAgain,
} FuriEpollProcessStatus;

typedef enum {
    FuriEpollItemTypeMessageQueue,
} FuriEpollItemType;

typedef struct FuriEpollItem FuriEpollItem;

struct FuriEpollItem {
    // Source
    FuriEpoll* owner;

    // Tracking item
    FuriEpollItemType type;
    void* item;
    FuriEpollEvent event;

    // Callback and context
    FuriEpollMessageQueueCallback callback;
    void* callback_context;

    // Waiting list
    ILIST_INTERFACE(WaitingList, struct FuriEpollItem);
};

ILIST_DEF(WaitingList, FuriEpollItem, M_POD_OPLIST)

/* Epoll RB tree */
#define FURI_EPOLL_TREE_RANK (4)

BPTREE_DEF2(
    FuriEpollTree,
    FURI_EPOLL_TREE_RANK,
    void*, /* pointer to object we track */
    M_PTR_OPLIST,
    FuriEpollItem*, /* pointer to the FuriEpollItem */
    M_PTR_OPLIST)

#define M_OPL_FuriEpollTree_t() BPTREE_OPLIST(FuriEpollTree, M_POD_OPLIST)

struct FuriEpoll {
    // Only works if all operations are done from the same thread
    FuriThreadId thread_id;

    // Tree mutex
    FuriMutex* tree_mutex;

    // Tree
    FuriEpollTree_t tree;

    // Tick event
    uint32_t tick_interval;
    FuriEpollTickCallback tick_callback;
    void* tick_callback_context;

    // Waiting items
    WaitingList_t waiting_list;
};

FuriEpollItem* furi_epoll_item_alloc(FuriEpoll* owner);

void furi_epoll_item_free(FuriEpollItem* instance);

void furi_epoll_item_set_item(
    FuriEpollItem* instance,
    FuriEpollItemType type,
    void* item,
    FuriEpollEvent event);

void furi_epoll_item_set_callback(
    FuriEpollItem* instance,
    FuriEpollMessageQueueCallback callback,
    void* callback_context);

void furi_epoll_item_notify(FuriEpollItem* instance, FuriEpollItemType type, FuriEpollEvent event);
