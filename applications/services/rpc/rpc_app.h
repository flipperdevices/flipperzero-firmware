#pragma once
#include "rpc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RpcAppSystemEventDataTypeNone,
    RpcAppSystemEventDataTypeCStr,
    RpcAppSystemEventDataTypeInt32,
} RpcAppSystemEventDataType;

typedef struct {
    RpcAppSystemEventDataType type;
    union {
        const char* cstr;
        int32_t i32;
    };
} RpcAppSystemEventData;

typedef enum {
    RpcAppEventTypeSessionClose,
    RpcAppEventTypeAppExit,
    RpcAppEventTypeLoadFile,
    RpcAppEventTypeButtonPress,
    RpcAppEventTypeButtonRelease,
} RpcAppSystemEventType;

typedef struct {
    RpcAppSystemEventType type;
    RpcAppSystemEventData data;
} RpcAppSystemEvent;

typedef void (*RpcAppSystemCallback)(const RpcAppSystemEvent* event, void* context);
typedef void (
    *RpcAppSystemDataExchangeCallback)(const uint8_t* data, size_t data_size, void* context);

typedef struct RpcAppSystem RpcAppSystem;

void rpc_system_app_set_callback(
    RpcAppSystem* rpc_app,
    RpcAppSystemCallback callback,
    void* context);

void rpc_system_app_send_started(RpcAppSystem* rpc_app);

void rpc_system_app_send_exited(RpcAppSystem* rpc_app);

void rpc_system_app_confirm(RpcAppSystem* rpc_app, RpcAppSystemEventType event_type, bool result);

void rpc_system_app_set_error_code(RpcAppSystem* rpc_app, uint32_t error_code);

void rpc_system_app_set_error_text(RpcAppSystem* rpc_app, const char* error_text);

void rpc_system_app_error_reset(RpcAppSystem* rpc_app);

void rpc_system_app_set_data_exchange_callback(
    RpcAppSystem* rpc_app,
    RpcAppSystemDataExchangeCallback callback,
    void* ctx);

void rpc_system_app_exchange_data(RpcAppSystem* rpc_app, const uint8_t* data, size_t data_size);

#ifdef __cplusplus
}
#endif
