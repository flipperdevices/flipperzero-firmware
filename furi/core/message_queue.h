#pragma once

#include "core/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \details Message Queue ID identifies the message queue.
typedef void FuriMessageQueue;

/// Create and Initialize a Message Queue object.
/// \param[in]     msg_count     maximum number of messages in queue.
/// \param[in]     msg_size      maximum message size in bytes.
/// \param[in]     attr          message queue attributes; NULL: default values.
/// \return message queue ID for reference by other functions or NULL in case of error.
FuriMessageQueue* furi_message_queue_alloc(uint32_t msg_count, uint32_t msg_size);

/// Put a Message into a Queue or timeout if Queue is full.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \param[in]     msg_ptr       pointer to buffer with message to put into a queue.
/// \param[in]     msg_prio      message priority.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_message_queue_put(FuriMessageQueue* instance, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);

/// Get a Message from a Queue or timeout if Queue is empty.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \param[out]    msg_ptr       pointer to buffer for message to get from a queue.
/// \param[out]    msg_prio      pointer to buffer for message priority or NULL.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_message_queue_get(FuriMessageQueue* instance, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);

/// Get maximum number of messages in a Message Queue.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \return maximum number of messages.
uint32_t furi_message_queue_get_capacity(FuriMessageQueue* instance);

/// Get maximum message size in a Message Queue.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \return maximum message size in bytes.
uint32_t furi_message_queue_get_message_size(FuriMessageQueue* instance);

/// Get number of queued messages in a Message Queue.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \return number of queued messages.
uint32_t furi_message_queue_get_count(FuriMessageQueue* instance);

/// Get number of available slots for messages in a Message Queue.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \return number of available slots for messages.
uint32_t furi_message_queue_get_space(FuriMessageQueue* instance);

/// Reset a Message Queue to initial empty state.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_message_queue_reset(FuriMessageQueue* instance);

/// Delete a Message Queue object.
/// \param[in]     instance         message queue ID obtained by \ref furi_message_queue_alloc.
/// \return status code that indicates the execution status of the function.
void furi_message_queue_free(FuriMessageQueue* instance);

#ifdef __cplusplus
}
#endif
