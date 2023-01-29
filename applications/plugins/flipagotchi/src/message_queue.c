#include "../include/message_queue.h"

MessageQueue* message_queue_alloc() {
    MessageQueue* queue = malloc(sizeof(MessageQueue));

    queue->queueSize = PWNAGOTCHI_PROTOCOL_QUEUE_SIZE;

    queue->messageQueue = malloc(sizeof(uint8_t) * queue->queueSize);

    // Set everything to 0
    memset(queue->messageQueue, 0, queue->queueSize);

    queue->writePtr = queue->messageQueue;
    queue->readPtr = queue->messageQueue;

    return queue;
}

void message_queue_free(MessageQueue* queue) {
    free(queue->messageQueue);
    free(queue);

    queue = NULL;
}

bool message_queue_has_message(MessageQueue* queue) {
    // Make sure readPtr is at the beginning of a message
    if(*(queue->readPtr) != PWNAGOTCHI_PROTOCOL_START) {
        return false;
    }

    // Now keep looking forward through the queue until we hit a 0 or the end byte
    bool alreadyWrapped = false;
    for(uint8_t* cursor = queue->readPtr;; cursor++) {
        // Let's check if we should wrap around
        if(cursor >= queue->messageQueue + queue->queueSize) {
            if(alreadyWrapped) {
                return false;
            }
            cursor = queue->messageQueue;
            alreadyWrapped = true;
        }

        // Otherwise see if we're at an end byte
        if(*cursor == PWNAGOTCHI_PROTOCOL_END) {
            return true;
        } else if(*cursor == 0x00) {
            return false;
        }
    }
}

void message_queue_push_byte(MessageQueue* queue, uint8_t data) {
    *(queue->writePtr) = data;

    // Now increment writePtr
    queue->writePtr += 1;

    // Check to make sure we didn't exceed the size
    if(queue->writePtr >= queue->messageQueue + queue->queueSize) {
        // Then start writing over the front again
        queue->writePtr = queue->messageQueue;
    }
}

void message_queue_wipe(MessageQueue* queue) {
    // Set everything to 0
    memset(queue->messageQueue, 0, queue->queueSize);

    queue->readPtr = queue->messageQueue;
    queue->writePtr = queue->messageQueue;
}

bool message_queue_validate(MessageQueue* queue) {
    UNUSED(queue);
    /*      AS OF RIGHT NOW VALIDATE IS NOT IMPLEMENTED
    // Loop through and confirm that each message is a valid byte
    for (size_t ii = 0; ii < queue->queueSize - PWNAGOTCHI_PROTOCOL_BYTE_LEN; ii += PWNAGOTCHI_PROTOCOL_BYTE_LEN) {
        if (queue->messageQueue[ii] == 0 && queue->messageQueue[ii + PWNAGOTCHI_PROTOCOL_BYTE_LEN - 1] == 0) {
            continue;
        }

        if (!(queue->messageQueue[ii] == PWNAGOTCHI_PROTOCOL_START &&
            queue->messageQueue[ii + PWNAGOTCHI_PROTOCOL_BYTE_LEN - 1] == PWNAGOTCHI_PROTOCOL_END)) {
                // This means it failed so we should wipe and breka
                message_queue_wipe(queue);
                return false;
        }
    }
    */

    return true;
}

bool message_queue_pop_message(MessageQueue* queue, PwnCommand* dest) {
    if(!message_queue_has_message(queue)) {
        return false;
    }

    // Otherwise let's grab the message! Currently readPtr is pointing at STX
    dest->parameterCode = *(queue->readPtr + 1);

    // Wipe current arguments to hold 0's
    memset(dest->arguments, 0, PWNAGOTCHI_PROTOCOL_ARGS_MAX);

    // Everything folowing is arguments until we hit the end
    // We'll count up the number of bytes in the transmission to know how many to wipe
    int argCount;
    for(argCount = 0; argCount < PWNAGOTCHI_PROTOCOL_ARGS_MAX; argCount++) {
        uint8_t* bytePtr = queue->readPtr + 2 + argCount;

        // Wrap around if needed
        if(bytePtr >= queue->messageQueue + queue->queueSize) {
            bytePtr = queue->messageQueue;
        }

        if(*bytePtr == PWNAGOTCHI_PROTOCOL_END) {
            break;
        }

        dest->arguments[argCount] = *bytePtr;
    }

    //                  STX + CODE + args + ETX
    size_t messageSize = 1 + 1 + argCount + 1;

    // Let's reset that message so we don't run across it again
    memset(queue->readPtr, 0, messageSize);

    // Now increment the readPtr
    queue->readPtr += messageSize;

    // Check if we are overflowing!
    if(queue->readPtr >= queue->messageQueue + queue->queueSize) {
        // Then start reading back from the front
        queue->readPtr = queue->messageQueue;
    }

    return true;
}
