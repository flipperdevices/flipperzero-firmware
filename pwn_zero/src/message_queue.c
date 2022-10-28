#include "../include/message_queue.h"

MessageQueue* message_queue_alloc() {
    MessageQueue* queue = malloc(sizeof(MessageQueue));

    queue->queueSize = PWNAGOTCHI_PROTOCOL_BYTE_LEN * PWNAGOTCHI_PROTOCOL_QUEUE_SIZE;

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
    if (*(queue->readPtr) == PWNAGOTCHI_PROTOCOL_START &&
        *(queue->readPtr + PWNAGOTCHI_PROTOCOL_BYTE_LEN - 1) == PWNAGOTCHI_PROTOCOL_END) {
            // This means that there is a seemingly valid command
            return true;
    }
    return false;
}

void message_queue_push_byte(MessageQueue* queue, uint8_t data) {
    *(queue->writePtr) = data;

    // Now increment writePtr
    queue->writePtr += 1;

    // Check to make sure we didn't exceed the size
    if (queue->writePtr >= queue->messageQueue + queue->queueSize - 1) {
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
    // Loop through and confirm that each message is a valid byte
    for (size_t ii = 0; ii < queue->queueSize; ii += PWNAGOTCHI_PROTOCOL_BYTE_LEN) {
        if (!(queue->messageQueue[ii] == PWNAGOTCHI_PROTOCOL_START &&
            queue->messageQueue[ii + PWNAGOTCHI_PROTOCOL_BYTE_LEN - 1] == PWNAGOTCHI_PROTOCOL_END)) {
                // This means it failed so we should wipe and breka
                message_queue_wipe(queue);
                return false;
        }
    }

    return true;
}

bool message_queue_pop_message(MessageQueue* queue, PwnCommand* dest) {
    if (!message_queue_has_message(queue)) {
        return false;
    }

    // Otherwise let's grab the message! Currently readPtr is pointing at STX
    dest->i = *(queue->readPtr + 1);
    dest->j = *(queue->readPtr + 2);
    dest->code = *(queue->readPtr + 3);

    // Now increment the readPtr
    queue->readPtr += 1;

    // Check if we are overflowing!
    if (queue->readPtr >= queue->messageQueue + queue->queueSize - 1) {
        // Then start reading back from the front
        queue->readPtr = queue->messageQueue;
    }

    return true;
}
