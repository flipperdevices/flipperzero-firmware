#pragma once
#include "_base.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/bluetooth-swift-pair

typedef struct {
    char display_name[25];
} SwiftpairMsg;

extern const BleSpamProtocol ble_spam_protocol_swiftpair;
