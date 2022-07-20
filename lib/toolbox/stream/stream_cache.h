#pragma once

#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StreamCache StreamCache;

StreamCache* stream_cache_alloc();
void stream_cache_free(StreamCache* cache);

void stream_cache_reset(StreamCache* cache);

bool stream_cache_at_end(StreamCache* cache);
size_t stream_cache_size(StreamCache* cache);
size_t stream_cache_position(StreamCache* cache);

size_t stream_cache_fill(StreamCache* cache, Stream* stream);
size_t stream_cache_read(StreamCache* cache, uint8_t* data, size_t size);
int32_t stream_cache_seek(StreamCache* cache, int32_t offset);

#ifdef __cplusplus
}
#endif
