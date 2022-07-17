#include "message_queue.h"
#include "core/common_defines.h"
#include <FreeRTOS.h>
#include <queue.h>
#include "check.h"

FuriMessageQueue* furi_message_queue_alloc(uint32_t msg_count, uint32_t msg_size) {
    furi_assert((furi_is_irq_context() == 0U) && (msg_count > 0U) && (msg_size > 0U));

    return ((FuriMessageQueue*)xQueueCreate(msg_count, msg_size));
}

/*
    Put a Message into a Queue or timeout if Queue is full.

    Limitations:
    - Message priority is ignored
*/
osStatus_t furi_message_queue_put (FuriMessageQueue* instance, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
    QueueHandle_t hQueue = (QueueHandle_t)instance;
    osStatus_t stat;
    BaseType_t yield;

    (void)msg_prio; /* Message priority is ignored */

    stat = osOK;

    if (furi_is_irq_context() != 0U) {
        if ((hQueue == NULL) || (msg_ptr == NULL) || (timeout != 0U)) {
            stat = osErrorParameter;
        }
        else {
            yield = pdFALSE;

            if (xQueueSendToBackFromISR (hQueue, msg_ptr, &yield) != pdTRUE) {
                stat = osErrorResource;
            } else {
                portYIELD_FROM_ISR (yield);
            }
        }
    }
    else {
        if ((hQueue == NULL) || (msg_ptr == NULL)) {
            stat = osErrorParameter;
        }
        else {
            if (xQueueSendToBack (hQueue, msg_ptr, (TickType_t)timeout) != pdPASS) {
                if (timeout != 0U) {
                    stat = osErrorTimeout;
                } else {
                    stat = osErrorResource;
                }
            }
        }
    }

    /* Return execution status */
    return (stat);
}

/*
    Get a Message from a Queue or timeout if Queue is empty.

    Limitations:
    - Message priority is ignored
*/
osStatus_t furi_message_queue_get (FuriMessageQueue* instance, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
    QueueHandle_t hQueue = (QueueHandle_t)instance;
    osStatus_t stat;
    BaseType_t yield;

    (void)msg_prio; /* Message priority is ignored */

    stat = osOK;

    if (furi_is_irq_context() != 0U) {
        if ((hQueue == NULL) || (msg_ptr == NULL) || (timeout != 0U)) {
            stat = osErrorParameter;
        }
        else {
            yield = pdFALSE;

            if (xQueueReceiveFromISR (hQueue, msg_ptr, &yield) != pdPASS) {
                stat = osErrorResource;
            } else {
                portYIELD_FROM_ISR (yield);
            }
        }
    }
    else {
        if ((hQueue == NULL) || (msg_ptr == NULL)) {
            stat = osErrorParameter;
        }
        else {
            if (xQueueReceive (hQueue, msg_ptr, (TickType_t)timeout) != pdPASS) {
                if (timeout != 0U) {
                    stat = osErrorTimeout;
                } else {
                    stat = osErrorResource;
                }
            }
        }
    }

    /* Return execution status */
    return (stat);
}

/*
    Get maximum number of messages in a Message Queue.
*/
uint32_t furi_message_queue_get_capacity (FuriMessageQueue* instance) {
    StaticQueue_t *mq = (StaticQueue_t *)instance;
    uint32_t capacity;

    if (mq == NULL) {
        capacity = 0U;
    } else {
        /* capacity = pxQueue->uxLength */
        capacity = mq->uxDummy4[1];
    }

    /* Return maximum number of messages */
    return (capacity);
}

/*
    Get maximum message size in a Message Queue.
*/
uint32_t furi_message_queue_get_message_size (FuriMessageQueue* instance) {
    StaticQueue_t *mq = (StaticQueue_t *)instance;
    uint32_t size;

    if (mq == NULL) {
        size = 0U;
    } else {
        /* size = pxQueue->uxItemSize */
        size = mq->uxDummy4[2];
    }

    /* Return maximum message size */
    return (size);
}

/*
    Get number of queued messages in a Message Queue.
*/
uint32_t furi_message_queue_get_count (FuriMessageQueue* instance) {
    QueueHandle_t hQueue = (QueueHandle_t)instance;
    UBaseType_t count;

    if (hQueue == NULL) {
        count = 0U;
    }
    else if (furi_is_irq_context() != 0U) {
        count = uxQueueMessagesWaitingFromISR (hQueue);
    }
    else {
        count = uxQueueMessagesWaiting (hQueue);
    }

    /* Return number of queued messages */
    return ((uint32_t)count);
}

/*
    Get number of available slots for messages in a Message Queue.
*/
uint32_t furi_message_queue_get_space (FuriMessageQueue* instance) {
    StaticQueue_t *mq = (StaticQueue_t *)instance;
    uint32_t space;
    uint32_t isrm;

    if (mq == NULL) {
        space = 0U;
    }
    else if (furi_is_irq_context() != 0U) {
        isrm = taskENTER_CRITICAL_FROM_ISR();

        /* space = pxQueue->uxLength - pxQueue->uxMessagesWaiting; */
        space = mq->uxDummy4[1] - mq->uxDummy4[0];

        taskEXIT_CRITICAL_FROM_ISR(isrm);
    }
    else {
        space = (uint32_t)uxQueueSpacesAvailable ((QueueHandle_t)mq);
    }

    /* Return number of available slots */
    return (space);
}

/*
    Reset a Message Queue to initial empty state.
*/
osStatus_t furi_message_queue_reset (FuriMessageQueue* instance) {
    QueueHandle_t hQueue = (QueueHandle_t)instance;
    osStatus_t stat;

    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else if (hQueue == NULL) {
        stat = osErrorParameter;
    }
    else {
        stat = osOK;
        (void)xQueueReset (hQueue);
    }

    /* Return execution status */
    return (stat);
}

void furi_message_queue_free (FuriMessageQueue* instance) {
    furi_assert(furi_is_irq_context() != 0U);
    furi_assert(instance);

    vQueueDelete((QueueHandle_t)instance);
}
