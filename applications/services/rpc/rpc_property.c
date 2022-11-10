#include <flipper.pb.h>
#include <furi_hal.h>
#include <core/core_defines.h>

#include "rpc_i.h"

#define TAG "RpcProperty"

// static void rpc_system_property_get_process(const PB_Main* request, void* context) {
//
// }

void* rpc_system_property_alloc(RpcSession* session) {
    UNUSED(session);
    return NULL;
}
