#pragma once

#include "ask_ctx.h"

#define ASK_CTX_CMD_READ_CTX256B (0x10U)
#define ASK_CTX_CMD_READ_CTX512B (0xC0U)

typedef struct {
    uint8_t blocks_total;
    uint8_t read_command;
    const char* full_name;
    const char* type_name;
} AskCtxFeatures;

extern const AskCtxFeatures ask_ctx_features[AskCtxTypeNum];
