#pragma once

#include "message_queue.h"

#include "kernel.h"
#include "event_loop_i.h"
#include "check.h"

#include <FreeRTOS.h>
#include <queue.h>

struct FuriMessageQueue {
    // !!! Semi-Opaque type inheritance, Very Fragile, DO NOT MOVE !!!
    StaticQueue_t container;

    // Event Loop
    FuriEventLoopItem*
        event_loop_item_out; /**< Notify event_loop item on message being taken from the queue */
    FuriEventLoopItem*
        event_loop_item_in; /**< Notify event_loop item on message being put into the queue */

    // !!! Data buffer, must be last in the structure, DO NOT MOVE !!!
    uint8_t buffer[];
};

void furi_message_queue_event_loop_in_set(
    FuriMessageQueue* instance,
    FuriEventLoopItem* event_loop_item,
    FuriEventLoopEvent event);
