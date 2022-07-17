#include "message_queue.h"
#include "core/common_defines.h"
#include <FreeRTOS.h>
#include <queue.h>

/*
    Create and Initialize a Message Queue object.

    Limitations:
    - The memory for control block and and message data must be provided in the
        osThreadAttr_t structure in order to allocate object statically.
*/
osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr) {
    QueueHandle_t hQueue;
    int32_t mem;

    hQueue = NULL;

    if ((furi_is_irq_context() == 0U) && (msg_count > 0U) && (msg_size > 0U)) {
        mem = -1;

        if (attr != NULL) {
            if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticQueue_t)) &&
                    (attr->mq_mem != NULL) && (attr->mq_size >= (msg_count * msg_size))) {
                /* The memory for control block and message data is provided, use static object */
                mem = 1;
            }
            else {
                if ((attr->cb_mem == NULL) && (attr->cb_size == 0U) &&
                        (attr->mq_mem == NULL) && (attr->mq_size == 0U)) {
                    /* Control block will be allocated from the dynamic pool */
                    mem = 0;
                }
            }
        }
        else {
            mem = 0;
        }

        if (mem == 1) {
            #if (configSUPPORT_STATIC_ALLOCATION == 1)
                hQueue = xQueueCreateStatic (msg_count, msg_size, attr->mq_mem, attr->cb_mem);
            #endif
        }
        else {
            if (mem == 0) {
                #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
                    hQueue = xQueueCreate (msg_count, msg_size);
                #endif
            }
        }

        #if (configQUEUE_REGISTRY_SIZE > 0)
        if (hQueue != NULL) {
            if ((attr != NULL) && (attr->name != NULL)) {
                /* Only non-NULL name objects are added to the Queue Registry */
                vQueueAddToRegistry (hQueue, attr->name);
            }
        }
        #endif

    }

    /* Return message queue ID */
    return ((osMessageQueueId_t)hQueue);
}

/*
    Put a Message into a Queue or timeout if Queue is full.

    Limitations:
    - Message priority is ignored
*/
osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
    QueueHandle_t hQueue = (QueueHandle_t)mq_id;
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
osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
    QueueHandle_t hQueue = (QueueHandle_t)mq_id;
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
uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id) {
    StaticQueue_t *mq = (StaticQueue_t *)mq_id;
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
uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id) {
    StaticQueue_t *mq = (StaticQueue_t *)mq_id;
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
uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id) {
    QueueHandle_t hQueue = (QueueHandle_t)mq_id;
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
uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id) {
    StaticQueue_t *mq = (StaticQueue_t *)mq_id;
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
osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id) {
    QueueHandle_t hQueue = (QueueHandle_t)mq_id;
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

/*
    Delete a Message Queue object.
*/
osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id) {
    QueueHandle_t hQueue = (QueueHandle_t)mq_id;
    osStatus_t stat;

#ifndef USE_FreeRTOS_HEAP_1
    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else if (hQueue == NULL) {
        stat = osErrorParameter;
    }
    else {
        #if (configQUEUE_REGISTRY_SIZE > 0)
        vQueueUnregisterQueue (hQueue);
        #endif

        stat = osOK;
        vQueueDelete (hQueue);
    }
#else
    stat = osError;
#endif

    /* Return execution status */
    return (stat);
}
