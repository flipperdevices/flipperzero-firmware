#pragma once
#include "rpc.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "flipper.pb.h"

typedef void* (*RpcSystemAlloc)(RpcInstance*);
typedef void (*RpcSystemFree)(void*);
typedef void (*PBMessageHandler)(const PB_Main* msg_request, void* context);

typedef struct {
    bool (*decode_submessage)(pb_istream_t *stream, const pb_field_t *field, void **arg);
    PBMessageHandler message_handler;
    void* context;
} RpcHandler;

void rpc_encode_and_send(RpcInstance* rpc, const PB_Main* main_message);
void rpc_add_handler(RpcInstance* rpc, pb_size_t message_tag, RpcHandler* handler);

void* rpc_system_status_alloc(RpcInstance* rpc);
void* rpc_system_storage_alloc(RpcInstance* rpc);
void rpc_system_storage_free(void* ctx);


