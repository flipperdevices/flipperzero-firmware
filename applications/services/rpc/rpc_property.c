#include <flipper.pb.h>
#include <furi_hal.h>
#include <core/core_defines.h>

#include "rpc_i.h"

#define TAG "RpcProperty"

static void rpc_system_property_get_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_property_get_request_tag);

    FURI_LOG_D(TAG, "GetProperty");

    RpcSession* session = (RpcSession*)context;
    furi_assert(session);

    FuriString* selector = furi_string_alloc_set_str(request->content.property_get_request.key);

    PB_Main* response = malloc(sizeof(PB_Main));
    response->command_id = request->command_id;
    response->which_content = PB_Main_property_get_response_tag;
    response->command_status = PB_CommandStatus_OK;

    rpc_send_and_release(session, response);

    furi_string_free(selector);
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
