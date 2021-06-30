#pragma once
#include "irda.h"
#include <stddef.h>
#include "irda_encoder_i.h"

typedef enum {
    IrdaStatusError,
    IrdaStatusOk,
    IrdaStatusDone,
    IrdaStatusReady,
    IrdaStatusDoneRepeat,
} IrdaStatus;

typedef struct {
    uint16_t preamble_mark;
    uint16_t preamble_space;
    uint16_t bit1_mark;
    uint16_t bit1_space;
    uint16_t bit0_mark;
    uint16_t bit0_space;
    uint16_t silence_time;
    float    preamble_tolerance;
    uint32_t bit_tolerance;
    float    duty_cycle;
    uint32_t carrier_frequency;
} IrdaTimings;

typedef void* (*IrdaAlloc) (void);
typedef IrdaMessage* (*IrdaDecode) (void* ctx, bool level, uint32_t duration);
typedef void (*IrdaReset) (void*);
typedef void (*IrdaFree) (void*);

typedef IrdaStatus (*IrdaEncode)(void* encoder, uint32_t* out, bool* polarity);
typedef IrdaTimings (*IrdaTimingsGet)(void);

