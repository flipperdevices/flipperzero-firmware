#include <flipper.pb.h>
#include <furi_hal.h>
#include <furi_hal_info.h>
#include <furi_hal_power.h>
#include <core/core_defines.h>

#include "rpc_i.h"

#define TAG "RpcProperty"

typedef struct {
    RpcSession* session;
    PB_Main* response;
} RpcPropertyContext;

static void rpc_system_property_get_system_callback(const char* key, const char* value, bool last, void* context) {
    furi_assert(key);
    furi_assert(value);
    RpcPropertyContext* ctx = context;
    furi_assert(ctx);

    furi_assert(key);
    furi_assert(value);
    char* str_key = strdup(key);
    char* str_value = strdup(value);

    ctx->response->has_next = !last;
    ctx->response->content.system_device_info_response.key = str_key;
    ctx->response->content.system_device_info_response.value = str_value;

    rpc_send_and_release(ctx->session, ctx->response);
}

static void rpc_system_property_get_power_callback(const char* key, const char* value, bool last, void* context) {
    furi_assert(key);
    furi_assert(value);
    RpcPropertyContext* ctx = context;
    furi_assert(ctx);

    furi_assert(key);
    furi_assert(value);
    char* str_key = strdup(key);
    char* str_value = strdup(value);

    ctx->response->has_next = !last;
    ctx->response->content.system_device_info_response.key = str_key;
    ctx->response->content.system_device_info_response.value = str_value;

    rpc_send_and_release(ctx->session, ctx->response);
}

static void rpc_system_property_get_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_property_get_request_tag);

    FURI_LOG_D(TAG, "GetProperty");

    RpcSession* session = (RpcSession*)context;
    furi_assert(session);

    FuriString* topkey = furi_string_alloc();
    FuriString* subkey = furi_string_alloc_set_str(request->content.property_get_request.key);

    const size_t sep_idx = furi_string_search_char(subkey, '.');

    if(sep_idx == FURI_STRING_FAILURE) {
        furi_string_swap(topkey, subkey);
    } else {
        furi_string_set_n(topkey, subkey, 0, sep_idx);
        furi_string_right(subkey, sep_idx + 1);
    }

    FURI_LOG_D(TAG, "Topkey: %s, Subkey: %s", furi_string_get_cstr(topkey), furi_string_size(subkey) ? furi_string_get_cstr(subkey) : "<empty>");

    PB_Main* response = malloc(sizeof(PB_Main));
    response->command_id = request->command_id;
    response->which_content = PB_Main_property_get_response_tag;
    response->command_status = PB_CommandStatus_OK;

    // RpcPropertyContext property_context = {
    //     .session = session,
    //     .response = response,
    // };

    if(!furi_string_cmp(topkey, "system")) {
        // furi_hal_power_info_get_by_key(rpc_system_property_get_system_callback, subkey, &property_context);
    } else if(!furi_string_cmp(topkey, "power")) {
        // furi_hal_power_info_get_by_key(rpc_system_property_get_power_callback, subkey, &property_context);
    } else {
        rpc_send_and_release_empty(session, request->command_id, PB_CommandStatus_ERROR_INVALID_PARAMETERS);
    }

    furi_string_free(subkey);
    furi_string_free(topkey);

    free(response);
}

void* rpc_system_property_alloc(RpcSession* session) {

    furi_assert(session);

    RpcHandler rpc_handler = {
        .message_handler = NULL,
        .decode_submessage = NULL,
        .context = session,
    };

    rpc_handler.message_handler = rpc_system_property_get_process;
    rpc_add_handler(session, PB_Main_property_get_request_tag, &rpc_handler);
    return NULL;
}
