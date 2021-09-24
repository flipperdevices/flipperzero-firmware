#include "flipper.pb.h"
#include "furi/record.h"
#include "rpc/rpc.h"
#include "rpc_i.h"
#include "storage.pb.h"
#include "storage/storage.h"


typedef struct {
    RpcInstance* rpc;
    Storage* storage;

} RpcStorageSystem;


static void rpc_system_storage_list_process(const PB_Main* msg_request, void* context) {
    RpcStorageSystem* storage_system = context;
    File* file = storage_file_alloc(storage_system->storage);
    (void) file;
}

static void rpc_system_storage_read_process(const PB_Main* msg_request, void* context) {
}

static void rpc_system_storage_write_process(const PB_Main* msg_request, void* context) {
}

static void rpc_system_storage_delete_process(const PB_Main* msg_request, void* context) {
    PB_Main msg_response = PB_Main_init_default;
    msg_response.not_last = false;
    msg_response.command_status = PB_CommandStatus_OK;
    msg_response.command_id = msg_request->command_id;
    msg_response.which_content = PB_Main_ping_response_tag;

    rpc_encode_and_send(context, &msg_response);
}

void* rpc_system_storage_alloc(RpcInstance* rpc) {
    RpcHandler rpc_handler = {
        .message_handler = NULL,
        .decode_submessage = NULL,
        .context = rpc,
    };

    RpcStorageSystem* storage_system = furi_alloc(sizeof(RpcStorageSystem));
    storage_system->storage = furi_record_open("storage");
    storage_system->rpc = rpc;

    rpc_handler.message_handler = rpc_system_storage_list_process;
    rpc_add_handler(rpc, PB_Main_storage_list_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_storage_read_process;
    rpc_add_handler(rpc, PB_Main_storage_read_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_storage_write_process;
    rpc_add_handler(rpc, PB_Main_storage_write_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_storage_delete_process;
    rpc_add_handler(rpc, PB_Main_storage_delete_request_tag, &rpc_handler);

    return storage_system;
}

void rpc_system_storage_free(void* ctx) {
    RpcStorageSystem* storage_system = ctx;
    furi_record_close("storage");
    free(storage_system);
}

