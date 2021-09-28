#include "flipper.pb.h"
#include "furi/common_defines.h"
#include "furi/memmgr.h"
#include "furi/record.h"
#include "rpc/rpc.h"
#include "rpc_i.h"
#include "storage.pb.h"
#include "storage/storage.h"

#define RPC_TAG     "RPC_STORAGE"
#define MAX_NAME_LENGTH         255

#define DEBUG_PRINT     1

typedef struct {
    RpcInstance* rpc;
    Storage* storage;

} RpcStorageSystem;


static void rpc_system_storage_list_process(const PB_Main* request, void* context) {
    furi_assert(request);

    Storage* fs_api = furi_record_open("storage");
    File* dir = storage_file_alloc(fs_api);

    FURI_LOG_I(RPC_TAG, "Storage list path: \'%.128s\', cmd_id: %d", request->content.storage_list_request.path, request->command_id);

    PB_Main response = {
        .command_id = request->command_id,
        .not_last = false,
        .which_content = PB_Main_storage_list_request_tag,
    };
    PB_Storage_ListResponse* list = &response.content.storage_list_response;
    response.which_content = PB_Main_storage_list_response_tag;

    bool finish = false;
    int i = 0;

    if(storage_dir_open(dir, request->content.storage_list_request.path)) {
        response.command_status = PB_CommandStatus_OK;
    } else {
        response.command_status = PB_CommandStatus_ERROR_NO_FILE;
        finish = true;
    }

    while(!finish) {
        FileInfo fileinfo;
        char* name = furi_alloc(MAX_NAME_LENGTH + 1);
        if (storage_dir_read(dir, &fileinfo, name, MAX_NAME_LENGTH)) {
            if ((i + 1) == COUNT_OF(list->storage_element)) {
                list->storage_element_count = i;
                response.not_last = true;
#if DEBUG_PRINT
                PB_Storage_Element* element = response.content.storage_list_response.storage_element;
                size_t element_count = response.content.storage_list_response.storage_element_count;
                FURI_LOG_I(RPC_TAG, "Encode (result %d) cmd_id %d (%s) %d elements:", 
                        response.command_status,
                        response.command_id,
                        (!response.not_last) ? "last" : "not_last",
                        element_count);
                for (int j = 0; j < element_count; ++j) {
                    FURI_LOG_I(RPC_TAG, "\t \'%s\' %d  %s", element->name, element->size,
                            element->type == PB_Storage_Element_FileType_DIR ? "d" : "f");
                    element++;
                }
#endif  // DEBUG_PRINT
                rpc_encode_and_send(context, &response);
                i = 0;
            }
            list->storage_element[i].type = (fileinfo.flags & FSF_DIRECTORY)
                                            ? PB_Storage_Element_FileType_DIR
                                            : PB_Storage_Element_FileType_FILE;
            list->storage_element[i].size = fileinfo.size;
            list->storage_element[i].data.funcs.encode = NULL;
            /* memory free inside rpc_encode_and_send() -> pb_release() */
            list->storage_element[i].name = name;
            ++i;
        } else {
            finish = true;
            free(name);
        }
    }

    list->storage_element_count = i;
    response.not_last = false;
#if DEBUG_PRINT
                PB_Storage_Element* element = response.content.storage_list_response.storage_element;
                size_t element_count = response.content.storage_list_response.storage_element_count;
                FURI_LOG_I(RPC_TAG, "Encode (result %d) cmd_id %d (%s) %d elements:", 
                        response.command_status,
                        response.command_id,
                        (!response.not_last) ? "last" : "not_last",
                        element_count);
                for (int j = 0; j < element_count; ++j) {
                    FURI_LOG_I(RPC_TAG, "\t \'%s\' %d  %s", element->name, element->size,
                            element->type == PB_Storage_Element_FileType_DIR ? "d" : "f");
                    element++;
                }
#endif  // DEBUG_PRINT
    rpc_encode_and_send(context, &response);

    storage_dir_close(dir);
    storage_file_free(dir);

    furi_record_close("storage");
}

static void rpc_system_storage_read_process(const PB_Main* request, void* context) {
}

static void rpc_system_storage_write_process(const PB_Main* request, void* context) {
}

static void rpc_system_storage_delete_process(const PB_Main* request, void* context) {
    PB_Main msg_response = PB_Main_init_default;
    msg_response.not_last = false;
    msg_response.command_status = PB_CommandStatus_OK;
    msg_response.command_id = request->command_id;
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

