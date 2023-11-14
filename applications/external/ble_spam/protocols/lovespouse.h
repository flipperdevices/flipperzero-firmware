#pragma once
#include "_base.h"

// Hacked together by @Willy-JL
// Discovered by @mandomat
// Blog post at https://mandomat.github.io/2023-11-13-denial-of-pleasure/

typedef enum {
    LovespouseStatePlay = 0x01, // Skip 0 as it means unset
    LovespouseStateStop,
    LovespouseStateCOUNT,
} LovespouseState;

typedef struct {
    LovespouseState state;
    uint32_t mode;
} LovespouseCfg;

extern const Protocol protocol_lovespouse;
