#pragma once

#include "continuity.h"
#include "fastpair.h"
#include "swiftpair.h"

union BleSpamProtocolCfg {
    ContinuityCfg continuity;
    FastpairCfg fastpair;
    SwiftpairCfg swiftpair;
};

extern const BleSpamProtocol* ble_spam_protocols[];

extern const size_t ble_spam_protocols_count;

typedef struct {
    bool random_mac;
    BleSpamProtocolCfg cfg;
} BleSpamPayload;
