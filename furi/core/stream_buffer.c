#include "stream_buffer.h"
#include "common_defines.h"
#include <FreeRTOS.h>
#include <FreeRTOS-Kernel/include/stream_buffer.h>

FuriStreamBuffer* furi_stream_buffer_alloc(size_t size, size_t trigger_level) {
    return xStreamBufferCreate(size, trigger_level);
};

void furi_stream_buffer_free(FuriStreamBuffer* stream_buffer) {
    vStreamBufferDelete(stream_buffer);
};

size_t furi_stream_buffer_send(
    FuriStreamBuffer* stream_buffer,
    const void* data,
    size_t length,
    uint32_t timeout) {
    size_t ret;

    if(FURI_IS_IRQ_MODE() != 0U) {
        BaseType_t yield;
        ret = xStreamBufferSendFromISR(stream_buffer, data, length, &yield);
        portYIELD_FROM_ISR(yield);
    } else {
        ret = xStreamBufferSend(stream_buffer, data, length, timeout);
    }

    return ret;
};

size_t furi_stream_buffer_receive(
    FuriStreamBuffer* stream_buffer,
    void* data,
    size_t length,
    uint32_t timeout) {
    size_t ret;

    if(FURI_IS_IRQ_MODE() != 0U) {
        BaseType_t yield;
        ret = xStreamBufferReceiveFromISR(stream_buffer, data, length, &yield);
        portYIELD_FROM_ISR(yield);
    } else {
        ret = xStreamBufferReceive(stream_buffer, data, length, timeout);
    }

    return ret;
}

size_t furi_stream_buffer_spaces_available(FuriStreamBuffer* stream_buffer) {
    return xStreamBufferSpacesAvailable(stream_buffer);
};