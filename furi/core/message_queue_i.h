#pragma once

#include "message_queue.h"

#include "kernel.h"
#include "event_loop_i.h"
#include "check.h"

#include <FreeRTOS.h>
#include <queue.h>

FuriEventLoopLink* furi_message_queue_event_loop_get_link(void* instance);

uint32_t furi_message_queue_event_loop_get_level(void* instance, FuriEventLoopEvent event);

extern const FuriEventLoopContract furi_message_queue_event_loop_contract;