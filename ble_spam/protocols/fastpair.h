#pragma once
#include "_base.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://developers.google.com/nearby/fast-pair/specifications/introduction

typedef struct {
    uint32_t model;
} FastpairCfg;

extern const Protocol protocol_fastpair;
