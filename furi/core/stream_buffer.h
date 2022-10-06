#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void FuriStreamBuffer;

FuriStreamBuffer* furi_stream_buffer_alloc(size_t size, size_t trigger_level);

void furi_stream_buffer_free(FuriStreamBuffer* stream_buffer);

bool furi_stream_set_trigger_level(FuriStreamBuffer* stream_buffer, size_t trigger_level);

size_t furi_stream_buffer_send(
    FuriStreamBuffer* stream_buffer,
    const void* data,
    size_t length,
    uint32_t timeout);

size_t furi_stream_buffer_receive(
    FuriStreamBuffer* stream_buffer,
    void* data,
    size_t length,
    uint32_t timeout);

size_t furi_stream_buffer_bytes_available(FuriStreamBuffer* stream_buffer);

size_t furi_stream_buffer_spaces_available(FuriStreamBuffer* stream_buffer);

bool furi_stream_buffer_is_full(FuriStreamBuffer* stream_buffer);

bool furi_stream_buffer_is_empty(FuriStreamBuffer* stream_buffer);

FuriStatus furi_stream_buffer_reset(FuriStreamBuffer* stream_buffer);

#ifdef __cplusplus
}
#endif