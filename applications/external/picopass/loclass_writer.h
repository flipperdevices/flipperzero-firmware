#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct LoclassWriter LoclassWriter;

LoclassWriter* loclass_writer_alloc();

void loclass_writer_free(LoclassWriter* instance);

bool loclass_writer_write_start_stop(LoclassWriter* instance, bool start);

bool loclass_writer_write_params(
    LoclassWriter* instance,
    uint8_t log_no,
    const uint8_t csn[8],
    const uint8_t epurse[8],
    const uint8_t nr[4],
    const uint8_t mac[4]);
