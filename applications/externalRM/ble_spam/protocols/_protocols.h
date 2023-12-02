#pragma once

#include "continuity.h"
#include "easysetup.h"
#include "fastpair.h"
#include "lovespouse.h"
#include "swiftpair.h"

typedef enum {
    PayloadModeRandom,
    PayloadModeValue,
    PayloadModeBruteforce,
} PayloadMode;

struct Payload {
    bool random_mac;
    PayloadMode mode;
    struct {
        uint8_t counter;
        uint32_t value;
        uint8_t size;
    } bruteforce;
    union {
        ContinuityCfg continuity;
        EasysetupCfg easysetup;
        FastpairCfg fastpair;
        LovespouseCfg lovespouse;
        SwiftpairCfg swiftpair;
    } cfg;
};

extern const Protocol* protocols[];

extern const size_t protocols_count;

struct Attack {
    const char* title;
    const char* text;
    const Protocol* protocol;
    Payload payload;
};
