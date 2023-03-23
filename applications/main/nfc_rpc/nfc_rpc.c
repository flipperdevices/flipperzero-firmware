#include "nfc_rpc_i.h"
#include <lib/nfc/protocols/nfca_poller.h>
#include <m-dict.h>

#include "assets/compiled/main.pb.h"

#define TAG "NfcRpc"

DICT_DEF2(NfcRpcHandlerDict, pb_size_t, M_DEFAULT_OPLIST, NfcRpcHandler, M_POD_OPLIST)

typedef void (*NfcRpcHandlersAlloc)(void* context);
typedef void (*NfcRpcHandlersFree)(void* context);

typedef enum {
    NfcRpcViewMain,
} NfcRpcView;

typedef enum {
    NfcRpcCustomEventMessageReceived,
} NfcRpcCustomEvent;

typedef enum {
    NfcRpcEventDataExchange,
} NfcRpcEventType;

typedef struct {
    NfcRpcEventType type;
    const uint8_t* data;
    size_t data_size;
} NfcRpcEvent;

struct NfcRpc {
    Gui* gui;
    RpcAppSystem* rpc;
    FuriMessageQueue* queue;
    ViewDispatcher* view_dispatcher;
    View* view;

    NfcRpcHandlerDict_t handlers;
};

typedef struct {
    NfcRpcHandlersAlloc alloc;
    NfcRpcHandlersFree free;
} NfcRpcCallbacks;

static const NfcRpcCallbacks nfc_rpc_callbacks[] = {
    {
        .alloc = nfc_rpc_nfca_alloc,
        .free = NULL,
    },
    {
        .alloc = nfc_rpc_mf_ultralight_alloc,
        .free = NULL,
    },
};

uint32_t nfc_rpc_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

void nfc_rpc_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 32, 0, "NFC RPC Actor running...");
}

bool nfc_rpc_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static void
    nfc_rpc_receive_data_exchange_callback(const uint8_t* data, size_t data_size, void* context) {
    NfcRpc* app = context;
    NfcRpcEvent event;
    FURI_LOG_I(TAG, "Received %d bytes", data_size);
    event.type = NfcRpcEventDataExchange;
    event.data = data;
    event.data_size = data_size;
    furi_message_queue_put(app->queue, &event, FuriWaitForever);
    view_dispatcher_send_custom_event(app->view_dispatcher, NfcRpcCustomEventMessageReceived);
}

static void nfc_rpc_rpc_command_callback(RpcAppSystemEvent event, void* context) {
    furi_assert(context);
    NfcRpc* app = context;

    if(event == RpcAppEventSessionClose) {
        rpc_system_app_set_callback(app->rpc, NULL, NULL);
        rpc_system_app_set_data_exchange_callback(app->rpc, NULL, NULL);
        view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_NONE);
        view_dispatcher_stop(app->view_dispatcher);
        app->rpc = NULL;
    }
    if(event == RpcAppEventAppExit) {
        view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_NONE);
        view_dispatcher_stop(app->view_dispatcher);
        rpc_system_app_confirm(app->rpc, RpcAppEventAppExit, true);
    }
}

static bool nfc_rpc_init_rpc(NfcRpc* app, const char* args) {
    bool ret = false;
    if(args && strlen(args)) {
        uint32_t rpc = 0;
        if(sscanf(args, "RPC %lX", &rpc) == 1) {
            app->rpc = (RpcAppSystem*)rpc;
            rpc_system_app_set_callback(app->rpc, nfc_rpc_rpc_command_callback, app);
            rpc_system_app_send_started(app->rpc);
            ret = true;
        }
    }
    return ret;
}

void nfc_rpc_add_handler(NfcRpc* instance, pb_size_t message_tag, NfcRpcHandler handler) {
    furi_assert(instance);
    furi_assert(instance->handlers);
    furi_assert(handler);
    furi_assert(NfcRpcHandlerDict_get(instance->handlers, message_tag) == NULL);

    NfcRpcHandlerDict_set_at(instance->handlers, message_tag, handler);
}

static void nfc_rpc_message_receive_handler(NfcRpc* instance) {
    furi_assert(instance);
    furi_assert(instance->queue);
    furi_assert(instance->rpc);
    NfcRpcEvent event;
    pb_istream_t istream;
    pb_ostream_t ostream;
    uint8_t response_buffer[512] = {};

    if(furi_message_queue_get(instance->queue, &event, FuriWaitForever) == FuriStatusOk) {
        Nfc_Main cmd = Nfc_Main_init_default;
        istream = pb_istream_from_buffer(event.data, event.data_size);
        if(pb_decode(&istream, Nfc_Main_fields, &cmd)) {
            FURI_LOG_I(TAG, "Which: %d", cmd.which_content);
            NfcRpcHandler handler = *NfcRpcHandlerDict_get(instance->handlers, cmd.which_content);
            if(handler) {
                handler(&cmd, instance);
            } else {
                cmd.command_status = Nfc_CommandStatus_ERROR_NOT_IMPLEMENTED;
            }
            ostream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));
            pb_encode(&ostream, Nfc_Main_fields, &cmd);
            FURI_LOG_D("NfcRpc", "Sending %d bytes", ostream.bytes_written);
            rpc_system_app_exchange_data(instance->rpc, response_buffer, ostream.bytes_written);
        } else {
            FURI_LOG_E(TAG, "Failed to decode message: %s", istream.errmsg);
            Nfc_Main cmd = Nfc_Main_init_default;
            cmd.command_status = Nfc_CommandStatus_ERROR;
            ostream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));
            pb_encode(&ostream, Nfc_Main_fields, &cmd);
            rpc_system_app_exchange_data(instance->rpc, response_buffer, ostream.bytes_written);
        }
    }
}

bool nfc_rpc_custom_event_callback(void* context, uint32_t event) {
    NfcRpc* instance = context;
    if(event == NfcRpcCustomEventMessageReceived) {
        nfc_rpc_message_receive_handler(instance);
    }

    return true;
}

static NfcRpc* nfc_rpc_app_alloc() {
    NfcRpc* instance = malloc(sizeof(NfcRpc));

    NfcRpcHandlerDict_init(instance->handlers);
    for(size_t i = 0; i < COUNT_OF(nfc_rpc_callbacks); i++) {
        if(nfc_rpc_callbacks[i].alloc) {
            nfc_rpc_callbacks[i].alloc(instance);
        }
    }

    instance->gui = furi_record_open(RECORD_GUI);
    instance->view_dispatcher = view_dispatcher_alloc();
    instance->view = view_alloc();

    view_set_previous_callback(instance->view, nfc_rpc_exit_callback);
    view_set_input_callback(instance->view, nfc_rpc_input_callback);
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, nfc_rpc_draw_callback);

    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(instance->view_dispatcher, NfcRpcViewMain, instance->view);

    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, nfc_rpc_custom_event_callback);

    instance->queue = furi_message_queue_alloc(8, sizeof(NfcRpcEvent));

    return instance;
}

void nfc_rpc_app_free(NfcRpc* instance) {
    furi_assert(instance);

    for(size_t i = 0; i < COUNT_OF(nfc_rpc_callbacks); i++) {
        if(nfc_rpc_callbacks[i].free) {
            nfc_rpc_callbacks[i].free(instance);
        }
    }
    NfcRpcHandlerDict_clear(instance->handlers);

    view_dispatcher_remove_view(instance->view_dispatcher, NfcRpcViewMain);
    view_free(instance->view);
    view_dispatcher_free(instance->view_dispatcher);
    furi_message_queue_free(instance->queue);
    furi_record_close(RECORD_GUI);
    instance->gui = NULL;
    instance->view_dispatcher = NULL;

    if(instance->rpc) {
        rpc_system_app_set_callback(instance->rpc, NULL, NULL);
        rpc_system_app_send_exited(instance->rpc);
        instance->rpc = NULL;
    }

    free(instance);
}

int32_t nfc_rpc_app(void* p) {
    UNUSED(p);
    NfcRpc* app = nfc_rpc_app_alloc();

    nfc_rpc_init_rpc(app, p);

    rpc_system_app_set_data_exchange_callback(
        app->rpc, nfc_rpc_receive_data_exchange_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, NfcRpcViewMain);
    view_dispatcher_run(app->view_dispatcher);

    nfc_rpc_app_free(app);

    return 0;
}
