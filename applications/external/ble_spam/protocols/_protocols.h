#pragma once

#include "continuity.h"
#include "fastpair.h"
#include "easysetup.h"
#include "swiftpair.h"

typedef enum {
    ProtocolModeRandom,
    ProtocolModeValue,
    ProtocolModeBruteforce,
} ProtocolMode;

struct ProtocolCfg {
    ProtocolMode mode;
    struct {
        uint8_t counter;
        uint32_t value;
        uint8_t size;
    } bruteforce;
    union {
        ContinuityCfg continuity;
        FastpairCfg fastpair;
        EasysetupCfg easysetup;
        SwiftpairCfg swiftpair;
    } specific;
};

extern const Protocol* protocols[];

extern const size_t protocols_count;

typedef struct {
    bool random_mac;
    ProtocolCfg cfg;
} Payload;

struct Attack {
    const char* title;
    const char* text;
    const Protocol* protocol;
    Payload payload;
};
