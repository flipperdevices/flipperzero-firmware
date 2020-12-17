#pragma once

#include <stdint.h>

/* Countable deed that affects icounter*/
typedef enum {
    // iButton
    DolphinDeedIButtonRead,
    DolphinDeedIButtonWrite,
    DolphinDeedIButtonEmulate,
    // Special value, do not use
    DolphinDeedMax
} DolphinDeed;

typedef struct {
    int32_t     icounter;
    uint32_t    limit_value;
    uint32_t    limit_interval;
} DolphinDeedWeight;

const DolphinDeedWeight* dolphin_deed_weight(DolphinDeed deed);
