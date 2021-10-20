#pragma once
#include <stddef.h>
#include <stdint.h>
#include "cmsis_os.h"

typedef struct Rpc Rpc;
typedef struct RpcSession RpcSession;

typedef void (*RpcSendBytesCallback)(void* context, uint8_t* bytes, size_t bytes_len);
typedef void (*RpcSessionClosedCallback)(void*);

RpcSession* rpc_open_session(Rpc* rpc);
void rpc_close_session(RpcSession* session);
void rpc_set_session_context(RpcSession* session, void* context);
/* WARN: can't call RPC API within RpcSendBytesCallback */
void rpc_set_send_bytes_callback(RpcSession* session, RpcSendBytesCallback callback);
void rpc_set_session_closed_callback(RpcSession* session, RpcSessionClosedCallback callback);
size_t rpc_feed_bytes(RpcSession* session, uint8_t* encoded_bytes, size_t size, TickType_t timeout);

// USAGE:
// 1) rpc_open_session();
// 2) rpc_set_send_bytes_callback();
// 3) rpc_set_close_session_callback();
// 4) while(1) {
//      rpc_feed_bytes();
//    }
// 5) rpc_close_session();
//

// It is guaranteed that no callbacks will be called
// as soon as session is closed either by
// RPC-service (RPC-command) or by transport level.
// So no need in setting callbacks to NULL after session close.


