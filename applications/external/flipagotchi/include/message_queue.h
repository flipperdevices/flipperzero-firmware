#pragma once

#include <furi.h>

#include "protocol.h"
#include "constants.h"

/**
 * Struct to contain and operate on given data as per the protocol specification
*/
typedef struct {
    /// Pointer to the beginning of the queue of bytes
    uint8_t* messageQueue;
    /// Contains the size of messageQueue
    size_t queueSize;

    /// Location of where we are currently writing
    uint8_t* writePtr;
    /// Location of where we are currently reading from
    uint8_t* readPtr;
} MessageQueue;

/**
 * Allocates memory to store the queue
 * 
 * @return Pointer to the newly created queue
 */
MessageQueue* message_queue_alloc();

/**
 * Destructs all memory that is stored at the pointer and sets pointer to null
 * 
 * @param queue Queue to operate on
 */
void message_queue_free(MessageQueue* queue);

/**
 * Decides if the queue has a full message available
 * 
 * @param queue Queue to check
 * @return If a message is available
 */
bool message_queue_has_message(MessageQueue* queue);

/**
 * Add a byte to the message queue
 * 
 * @param queue Queue to operate on
 * @param data Byte to add to the queue
 */
void message_queue_push_byte(MessageQueue* queue, uint8_t data);

/**
 * Wipes the entire message queue
 * 
 * @param queue Queue to wipe
 */
void message_queue_wipe(MessageQueue* queue);

/**
 * Loops through queue and ensures that all present bytes are valid according to the protocol,
 * if they are not it will wipe the queue
 * 
 * @param queue Queue to validate
 * @return If it was validated successfully. If it was wiped it will return false
 */
bool message_queue_validate(MessageQueue* queue);

/**
 * Pops a command off of the queue and saves into dest
 * 
 * @note This will call validate_wipe and clear the queue if it detects bad instructions
 * 
 * @param queue Queue to pop from
 * @param dest Where to save the command to
 * @return If there was a command to pop and a valid command entered the dest
 */
bool message_queue_pop_message(MessageQueue* queue, PwnCommand* dest);