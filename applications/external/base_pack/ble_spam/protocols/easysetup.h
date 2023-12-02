#pragma once
#include "_base.h"

// Hacked together by @Willy-JL and @Spooks4576
// Research by @Spooks4576

typedef enum {
    EasysetupTypeBuds = 0x01, // Skip 0 as it means unset
    EasysetupTypeWatch,
    EasysetupTypeCOUNT,
} EasysetupType;

typedef struct {
    EasysetupType type;
    union {
        struct {
            uint32_t model;
        } buds;
        struct {
            uint8_t model;
        } watch;
    } data;
} EasysetupCfg;

extern const Protocol protocol_easysetup;
