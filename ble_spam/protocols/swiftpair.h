#pragma once
#include "_base.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/bluetooth-swift-pair

typedef struct {
    char name[25];
} SwiftpairCfg;

extern const Protocol protocol_swiftpair;
