#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <core/string.h>

typedef void (*RpcHelperPropertyCallback)(const char* key, const char* value, bool last, void* context);

typedef struct {
    FuriString* key;
    FuriString* value;
    RpcHelperPropertyCallback out;
    char sep;
    bool last;
    void* context;
} RpcHelperPropertyContext;

void rpc_helper_property_out(RpcHelperPropertyContext* ctx, const char* fmt, unsigned int nparts, ...);

#ifdef __cplusplus
}
#endif
