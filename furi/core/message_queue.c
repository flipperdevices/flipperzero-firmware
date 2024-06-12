#include "message_queue.h"

#include "kernel.h"
#include "event_loop_i.h"
#include "check.h"

#include <FreeRTOS.h>
#include <queue.h>

// Internal FreeRTOS member names
#define uxMessagesWaiting uxDummy4[0]
#define uxLength uxDummy4[1]
#define uxItemSize uxDummy4[2]

struct FuriMessageQueue {
    FuriEventLoopBase event_loop_base;
    StaticQueue_t static_queue;
    uint8_t buffer[];
};

// IMPORTANT: event_loop_base MUST be the FIRST struct member
static_assert(offsetof(FuriMessageQueue, event_loop_base) == 0);
// IMPORTANT: buffer MUST be the LAST struct member
static_assert(offsetof(FuriMessageQueue, buffer) == sizeof(FuriMessageQueue));

const FuriEventLoopInterface furi_message_queue_event_loop_iface;

FuriMessageQueue* furi_message_queue_alloc(uint32_t msg_count, uint32_t msg_size) {
    furi_check((furi_kernel_is_irq_or_masked() == 0U) && (msg_count > 0U) && (msg_size > 0U));

    FuriMessageQueue* instance = malloc(sizeof(FuriMessageQueue) + msg_count * msg_size);
    instance->event_loop_base.vtable = &furi_message_queue_event_loop_iface;

    QueueHandle_t hQueue =
        xQueueCreateStatic(msg_count, msg_size, instance->buffer, &instance->static_queue);
    furi_check(hQueue == (QueueHandle_t)&instance->static_queue);

    return instance;
}

void furi_message_queue_free(FuriMessageQueue* instance) {
    furi_check(furi_kernel_is_irq_or_masked() == 0U);
    furi_check(instance);

    // Event Loop MUST be disconnected
    furi_check(!instance->event_loop_base.item_in);
    furi_check(!instance->event_loop_base.item_out);

    vQueueDelete((QueueHandle_t)&instance->static_queue);
    free(instance);
}

FuriStatus
    furi_message_queue_put(FuriMessageQueue* instance, const void* msg_ptr, uint32_t timeout) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)&instance->static_queue;
    FuriStatus stat;
    BaseType_t yield;

    stat = FuriStatusOk;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        if((msg_ptr == NULL) || (timeout != 0U)) {
            stat = FuriStatusErrorParameter;
        } else {
            yield = pdFALSE;

            if(xQueueSendToBackFromISR(hQueue, msg_ptr, &yield) != pdTRUE) {
                stat = FuriStatusErrorResource;
            } else {
                portYIELD_FROM_ISR(yield);
            }
        }
    } else {
        if(msg_ptr == NULL) {
            stat = FuriStatusErrorParameter;
        } else {
            if(xQueueSendToBack(hQueue, msg_ptr, (TickType_t)timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = FuriStatusErrorTimeout;
                } else {
                    stat = FuriStatusErrorResource;
                }
            }
        }
    }

    if(stat == FuriStatusOk) {
        furi_event_loop_base_notify(&instance->event_loop_base, FuriEventLoopEventIn);
    }

    return stat;
}

FuriStatus furi_message_queue_get(FuriMessageQueue* instance, void* msg_ptr, uint32_t timeout) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)&instance->static_queue;
    FuriStatus stat;
    BaseType_t yield;

    stat = FuriStatusOk;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        if((msg_ptr == NULL) || (timeout != 0U)) {
            stat = FuriStatusErrorParameter;
        } else {
            yield = pdFALSE;

            if(xQueueReceiveFromISR(hQueue, msg_ptr, &yield) != pdPASS) {
                stat = FuriStatusErrorResource;
            } else {
                portYIELD_FROM_ISR(yield);
            }
        }
    } else {
        if(msg_ptr == NULL) {
            stat = FuriStatusErrorParameter;
        } else {
            if(xQueueReceive(hQueue, msg_ptr, (TickType_t)timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = FuriStatusErrorTimeout;
                } else {
                    stat = FuriStatusErrorResource;
                }
            }
        }
    }

    if(stat == FuriStatusOk) {
        furi_event_loop_base_notify(&instance->event_loop_base, FuriEventLoopEventOut);
    }

    return stat;
}

uint32_t furi_message_queue_get_capacity(FuriMessageQueue* instance) {
    furi_check(instance);

    return instance->static_queue.uxLength;
}

uint32_t furi_message_queue_get_message_size(FuriMessageQueue* instance) {
    furi_check(instance);

    return instance->static_queue.uxItemSize;
}

uint32_t furi_message_queue_get_count(FuriMessageQueue* instance) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)&instance->static_queue;
    UBaseType_t count;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        count = uxQueueMessagesWaitingFromISR(hQueue);
    } else {
        count = uxQueueMessagesWaiting(hQueue);
    }

    return (uint32_t)count;
}

uint32_t furi_message_queue_get_space(FuriMessageQueue* instance) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)&instance->static_queue;
    uint32_t space;
    uint32_t isrm;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        isrm = taskENTER_CRITICAL_FROM_ISR();

        space = instance->static_queue.uxLength - instance->static_queue.uxMessagesWaiting;

        taskEXIT_CRITICAL_FROM_ISR(isrm);
    } else {
        space = (uint32_t)uxQueueSpacesAvailable(hQueue);
    }

    return space;
}

FuriStatus furi_message_queue_reset(FuriMessageQueue* instance) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)&instance->static_queue;
    FuriStatus stat;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        stat = FuriStatusErrorISR;
    } else {
        stat = FuriStatusOk;
        (void)xQueueReset(hQueue);
    }

    if(stat == FuriStatusOk) {
        furi_event_loop_base_notify(&instance->event_loop_base, FuriEventLoopEventOut);
    }

    return stat;
}

static uint32_t
    furi_message_queue_event_loop_get_level(FuriEventLoopBase* object, FuriEventLoopEvent event) {
    FuriMessageQueue* instance = (FuriMessageQueue*)object;
    furi_assert(instance);

    if(event == FuriEventLoopEventIn) {
        return furi_message_queue_get_count(instance);
    } else if(event == FuriEventLoopEventOut) {
        return furi_message_queue_get_space(instance);
    } else {
        furi_crash();
    }
}

const FuriEventLoopInterface furi_message_queue_event_loop_iface = {
    .get_level = furi_message_queue_event_loop_get_level,
};
