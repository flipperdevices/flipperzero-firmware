/*
#include "../include/uart_dev.h"

UartDevice* uart_device_alloc() {
    UartDevice* dev = malloc(sizeof(UartDevice));

    dev->channel = PWNAGOTCHI_UART_CHANNEL;
    dev->baudRate = PWNAGOTCHI_UART_BAUD;

    dev->bufferModel = buffer_model_alloc();

    return dev;
}

void uart_device_free(UartDevice* dev) {
    buffer_model_free(dev->bufferModel);

    free(dev);
    dev = NULL;
}

PwnCommand* uart_device_pop(UartDevice* dev);



BufferModel* buffer_model_alloc() {
    BufferModel* buf = malloc(sizeof(BufferModel));

    buf->messageQueue = malloc(sizeof(uint8_t) * PWNAGOTCHI_PROTOCOL_BYTE_LEN * PWNAGOTCHI_PROTOCOL_QUEUE_SIZE);
    buf->queueSize = PWNAGOTCHI_PROTOCOL_BYTE_LEN * PWNAGOTCHI_PROTOCOL_QUEUE_SIZE;
    buf->writePtr = buf->messageQueue;
    buf->readPtr = buf->messageQueue;
    
    return buf;
}

void buffer_model_free(BufferModel* model) {
    free(model->messageQueue);

    free(model);
    model = NULL;
}

bool buffer_model_has_message(BufferModel* model) {
    // Make sure there is enough data to read
    if (model->readPtr - PWNAGOTCHI_PROTOCOL_BYTE_LEN < model->messageQueue
        || model->readPtr == model->writePtr) {
        return false;
    }

    if (*(model->readPtr) == PWNAGOTCHI_PROTOCOL_START && *(model->readPtr + PWNAGOTCHI_PROTOCOL_BYTE_LEN) == PWNAGOTCHI_PROTOCOL_END) {
        return true;
    }

    return false;

}

void buffer_model_push(BufferModel* model, uint8_t data) {
}

uint8_t buffer_model_pop(BufferModel* model);
*/