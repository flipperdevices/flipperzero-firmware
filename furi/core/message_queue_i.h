#pragma once

#include "message_queue.h"

#include "kernel.h"
#include "epoll_i.h"
#include "check.h"

#include <FreeRTOS.h>
#include <queue.h>

struct FuriMessageQueue {
    // !!! Semi-Opaque type inheritance, Very Fragile, DO NOT MOVE !!!
    StaticQueue_t container;

    // Epoll
    FuriEpollItem* epoll_item_out; /**< Notify epoll item on message being taken from the queue */
    FuriEpollItem* epoll_item_in; /**< Notify epoll item on message being put into the queue */

    // !!! Data buffer, must be last in the structure, DO NOT MOVE !!!
    uint8_t buffer[];
};

void furi_message_queue_epoll_in_set(
    FuriMessageQueue* instance,
    FuriEpollItem* epoll_item,
    FuriEpollEvent event);
