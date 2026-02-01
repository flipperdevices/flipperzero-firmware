#pragma once
#include "rpc.h"
#include <storage/filesystem_api_defines.h>
#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <flipper.pb.h>
#include <toolbox/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*RpcSystemAlloc)(RpcSession* session);
typedef void (*RpcSystemFree)(void* context);
typedef void (*PBMessageHandler)(const PB_Main* msg_request, void* context);

typedef struct {
    bool (*decode_submessage)(pb_istream_t* stream, const pb_field_t* field, void** arg);
    PBMessageHandler message_handler;
    void* context;
} RpcHandler;

void rpc_send(RpcSession* session, PB_Main* main_message);

void rpc_send_and_release(RpcSession* session, PB_Main* main_message);

void rpc_send_and_release_empty(RpcSession* session, uint32_t command_id, PB_CommandStatus status);

void rpc_add_handler(RpcSession* session, pb_size_t message_tag, RpcHandler* handler);

void* rpc_system_system_alloc(RpcSession* session);
void* rpc_system_storage_alloc(RpcSession* session);
void rpc_system_storage_free(void* ctx);
void* rpc_system_app_alloc(RpcSession* session);
void rpc_system_app_free(void* ctx);
void* rpc_system_gui_alloc(RpcSession* session);
void rpc_system_gui_free(void* ctx);
void* rpc_system_gpio_alloc(RpcSession* session);
void rpc_system_gpio_free(void* ctx);
void* rpc_system_property_alloc(RpcSession* session);

void* rpc_desktop_alloc(RpcSession* session);
void rpc_desktop_free(void* ctx);

void* rpc_system_network_alloc(RpcSession* session);
void rpc_system_network_free(void* ctx);

// Public Network API for Flipper apps
// Returns connection_id on success, -1 on failure
int32_t rpc_network_connect(
    const char* host,
    uint16_t port,
    bool is_udp,
    uint32_t timeout_ms,
    char* resolved_ip_out,
    size_t resolved_ip_size);

// Returns bytes sent on success, -1 on failure
int32_t rpc_network_send(int32_t connection_id, const uint8_t* data, size_t size);

// Returns bytes received on success, 0 on timeout, -1 on failure/disconnect
int32_t rpc_network_receive(int32_t connection_id, uint8_t* buffer, size_t buffer_size, uint32_t timeout_ms);

// Close a connection
void rpc_network_close(int32_t connection_id);

// Check if network is available (RPC session active)
bool rpc_network_is_available(void);

void rpc_debug_print_message(const PB_Main* message);
void rpc_debug_print_data(const char* prefix, uint8_t* buffer, size_t size);

void rpc_cli_command_start_session(PipeSide* pipe, FuriString* args, void* context);

PB_CommandStatus rpc_system_storage_get_error(FS_Error fs_error);

#ifdef __cplusplus
}
#endif
