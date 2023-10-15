#pragma once

#include "continuity.h"
#include "fastpair.h"
#include "swiftpair.h"

union BleSpamMsg {
    ContinuityMsg continuity;
    FastpairMsg fastpair;
    SwiftpairMsg swiftpair;
};

extern const BleSpamProtocol* ble_spam_protocols[];

extern const size_t ble_spam_protocols_count;
