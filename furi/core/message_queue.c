#include "message_queue_i.h"

FuriMessageQueue* furi_message_queue_alloc(uint32_t msg_count, uint32_t msg_size) {
    furi_check((furi_kernel_is_irq_or_masked() == 0U) && (msg_count > 0U) && (msg_size > 0U));

    FuriMessageQueue* instance = malloc(sizeof(FuriMessageQueue) + msg_count * msg_size);

    // 3 things happens here:
    // - create queue
    // - check results
    // - ensure that queue container is first in the FuriMessageQueue structure
    //
    // As a bonus it guarantees that FuriMessageQueue* can be casted into StaticQueue_t* or QueueHandle_t.
    furi_check(
        xQueueCreateStatic(msg_count, msg_size, instance->buffer, &instance->container) ==
        (void*)instance);

    return instance;
}

void furi_message_queue_free(FuriMessageQueue* instance) {
    furi_check(furi_kernel_is_irq_or_masked() == 0U);
    furi_check(instance);

    // Event Loop must be disconnected
    furi_check(!instance->event_loop_item_in);
    furi_check(!instance->event_loop_item_out);

    vQueueDelete((QueueHandle_t)instance);
    free(instance);
}

FuriStatus
    furi_message_queue_put(FuriMessageQueue* instance, const void* msg_ptr, uint32_t timeout) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)instance;
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
        FURI_CRITICAL_ENTER();
        if(instance->event_loop_item_in) {
            furi_event_loop_item_notify(
                instance->event_loop_item_in,
                FuriEventLoopItemTypeMessageQueue,
                FuriEventLoopEventIn);
        }
        FURI_CRITICAL_EXIT();
    }

    /* Return execution status */
    return (stat);
}

FuriStatus furi_message_queue_get(FuriMessageQueue* instance, void* msg_ptr, uint32_t timeout) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)instance;
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
        FURI_CRITICAL_ENTER();
        if(instance->event_loop_item_out) {
            furi_event_loop_item_notify(
                instance->event_loop_item_out,
                FuriEventLoopItemTypeMessageQueue,
                FuriEventLoopEventOut);
        }
        FURI_CRITICAL_EXIT();
    }

    return (stat);
}

uint32_t furi_message_queue_get_capacity(FuriMessageQueue* instance) {
    furi_check(instance);

    uint32_t capacity;

    /* capacity = pxQueue->uxLength */
    capacity = instance->container.uxDummy4[1];

    /* Return maximum number of messages */
    return (capacity);
}

uint32_t furi_message_queue_get_message_size(FuriMessageQueue* instance) {
    furi_check(instance);

    uint32_t size;

    /* size = pxQueue->uxItemSize */
    size = instance->container.uxDummy4[2];

    /* Return maximum message size */
    return (size);
}

uint32_t furi_message_queue_get_count(FuriMessageQueue* instance) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)instance;
    UBaseType_t count;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        count = uxQueueMessagesWaitingFromISR(hQueue);
    } else {
        count = uxQueueMessagesWaiting(hQueue);
    }

    /* Return number of queued messages */
    return ((uint32_t)count);
}

uint32_t furi_message_queue_get_space(FuriMessageQueue* instance) {
    furi_check(instance);

    uint32_t space;
    uint32_t isrm;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        isrm = taskENTER_CRITICAL_FROM_ISR();

        /* space = pxQueue->uxLength - pxQueue->uxMessagesWaiting; */
        space = instance->container.uxDummy4[1] - instance->container.uxDummy4[0];

        taskEXIT_CRITICAL_FROM_ISR(isrm);
    } else {
        space = (uint32_t)uxQueueSpacesAvailable((QueueHandle_t)instance);
    }

    /* Return number of available slots */
    return (space);
}

FuriStatus furi_message_queue_reset(FuriMessageQueue* instance) {
    furi_check(instance);

    QueueHandle_t hQueue = (QueueHandle_t)instance;
    FuriStatus stat;

    if(furi_kernel_is_irq_or_masked() != 0U) {
        stat = FuriStatusErrorISR;
    } else {
        stat = FuriStatusOk;
        (void)xQueueReset(hQueue);
    }

    if(stat == FuriStatusOk) {
        FURI_CRITICAL_ENTER();
        if(instance->event_loop_item_out) {
            furi_event_loop_item_notify(
                instance->event_loop_item_out,
                FuriEventLoopItemTypeMessageQueue,
                FuriEventLoopEventOut);
        }
        FURI_CRITICAL_EXIT();
    }

    /* Return execution status */
    return (stat);
}

void furi_message_queue_event_loop_in_set(
    FuriMessageQueue* instance,
    FuriEventLoopItem* event_loop_item,
    FuriEventLoopEvent event) {
    furi_check(instance);
    furi_check(!furi_kernel_is_irq_or_masked());

    FURI_CRITICAL_ENTER();

    if(event == FuriEventLoopEventIn) {
        furi_check(
            event_loop_item ? instance->event_loop_item_in == NULL :
                              instance->event_loop_item_in != NULL);
        instance->event_loop_item_in = event_loop_item;
    } else if(event == FuriEventLoopEventOut) {
        furi_check(
            event_loop_item ? instance->event_loop_item_out == NULL :
                              instance->event_loop_item_out != NULL);
        instance->event_loop_item_out = event_loop_item;
    } else {
        furi_crash();
    }

    FURI_CRITICAL_EXIT();
}
