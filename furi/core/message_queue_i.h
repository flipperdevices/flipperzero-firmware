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
    FuriEventLoopLink event_loop_link;

    // !!! Data buffer, must be last in the structure, DO NOT MOVE !!!
    uint8_t buffer[];
};

FuriEventLoopLink* furi_message_queue_event_loop_get_link(void* instance);

uint32_t furi_message_queue_event_loop_get_level(void* instance, FuriEventLoopEvent event);

extern const FuriEventLoopContract furi_message_queue_event_loop_contract;