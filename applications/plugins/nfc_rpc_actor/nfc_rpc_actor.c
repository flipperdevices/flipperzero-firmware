#include "nfc_rpc_actor.h"
#include "furi_hal_nfc.h"

typedef enum {
    NfcRpcActorViewMain,
} NfcRpcActorView;

typedef struct NfcRpcActorModel {
    char text[100];
} NfcRpcActorModel;

uint32_t nfc_rpc_actor_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

void nfc_rpc_actor_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 32, 0, "NFC RPC Actor running...");
}

bool nfc_rpc_actor_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static void nfc_rpc_actor_receive_data_exchange_callback(
    const uint8_t* data,
    size_t data_size,
    void* context) {
    NfcRpcActor* app = context;
    NfcRpcActorEvent event;
    if(data) {
        FURI_LOG_I("NfcRpcActor", "Received %d bytes", data_size);
        FURI_LOG_I("NfcRpcActor", "Data: %.*s", data_size, data);
        event.type = NfcRpcActorEventDataExchange;
        event.data = data;
        furi_message_queue_put(app->queue, &event, FuriWaitForever);
    }
}

static void nfc_rpc_actor_rpc_command_callback(RpcAppSystemEvent event, void* context) {
    furi_assert(context);
    NfcRpcActor* app = context;

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

static bool nfc_rpc_actor_rpc_init_rpc(NfcRpcActor* app, const char* args) {
    bool ret = false;
    if(args && strlen(args)) {
        uint32_t rpc = 0;
        if(sscanf(args, "RPC %lX", &rpc) == 1) {
            app->rpc = (RpcAppSystem*)rpc;
            rpc_system_app_set_callback(app->rpc, nfc_rpc_actor_rpc_command_callback, app);
            rpc_system_app_send_started(app->rpc);
            ret = true;
        }
    }
    return ret;
}

void nfc_rpc_actor_tick_callback(void* context) {
    NfcRpcActor* app = context;
    NfcRpcActorEvent event;
    FuriHalNfcDevData dev_data;
    furi_assert(app->rpc);
    pb_istream_t istream;
    pb_ostream_t ostream;
    PB_NfcAnticollisionRequest request = PB_NfcAnticollisionRequest_init_default;
    PB_NfcAAnticollisionResponse response;
    uint8_t response_buffer[512];
    memset(response_buffer, 0, sizeof(response_buffer));
    if(furi_message_queue_get(app->queue, &event, 100) == FuriStatusOk) {
        istream = pb_istream_from_buffer(event.data, sizeof(event.data));
        pb_decode(&istream, PB_NfcAnticollisionRequest_fields, &request);
        FURI_LOG_I("NfcRpcActor", "Received type enum: %d", request.type);
        switch(request.type) {
        case PB_anticollisionType_NFCA:
            FURI_LOG_I("NfcRpcActor", "Received NFCA");
            if(furi_hal_nfc_detect(&dev_data, 500)) {
                memcpy(response.uid.bytes, dev_data.uid, sizeof(dev_data.uid));
                response.uid.size = sizeof(dev_data.uid);
                memcpy(response.sak.bytes, &dev_data.sak, sizeof(dev_data.sak));
                response.sak.size = sizeof(dev_data.sak);
                memcpy(response.atqa.bytes, dev_data.atqa, sizeof(dev_data.atqa));
                response.atqa.size = sizeof(dev_data.atqa);
                response.uid_len = dev_data.uid_len;
                response.ok = true;
                FURI_LOG_D("NfcRpcActor", "NFCA detected");
            } else {
                response.ok = false;
                FURI_LOG_D("NfcRpcActor", "NFCA not detected");
            }
            // Encode into protobuf and send response
            ostream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));
            pb_encode(&ostream, PB_NfcAAnticollisionResponse_fields, &response);
            FURI_LOG_D("NfcRpcActor", "Sending %d bytes", ostream.bytes_written);
            FURI_LOG_D("NfcRpcActor", "Data: %.*s", ostream.bytes_written, response_buffer);
            rpc_system_app_exchange_data(app->rpc, response_buffer, ostream.bytes_written);
            break;

        default:
            response.ok = false;
            FURI_LOG_D("NfcRpcActor", "Received unknown type, sending default response");
            ostream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));
            pb_encode(&ostream, PB_NfcAAnticollisionResponse_fields, &request);
            FURI_LOG_D("NfcRpcActor", "Sending %d bytes", ostream.bytes_written);
            FURI_LOG_D("NfcRpcActor", "Data: %.*s", ostream.bytes_written, response_buffer);
            rpc_system_app_exchange_data(app->rpc, response_buffer, ostream.bytes_written);
            break;
        }
    }
}

static NfcRpcActor* nfc_rpc_actor_app_alloc() {
    NfcRpcActor* app = malloc(sizeof(NfcRpcActor));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->view = view_alloc();

    view_set_previous_callback(app->view, nfc_rpc_actor_exit_callback);
    view_set_input_callback(app->view, nfc_rpc_actor_input_callback);
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, nfc_rpc_actor_draw_callback);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(app->view_dispatcher, NfcRpcActorViewMain, app->view);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, nfc_rpc_actor_tick_callback, 10);

    app->queue = furi_message_queue_alloc(8, sizeof(NfcRpcActorEvent));

    return app;
}

void nfc_rpc_actor_app_free(NfcRpcActor* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, NfcRpcActorViewMain);
    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);
    furi_message_queue_free(app->queue);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    app->view_dispatcher = NULL;

    if(app->rpc) {
        rpc_system_app_set_callback(app->rpc, NULL, NULL);
        rpc_system_app_send_exited(app->rpc);
        app->rpc = NULL;
    }

    free(app);
}

int32_t nfc_rpc_actor_app(void* p) {
    UNUSED(p);
    NfcRpcActor* app = nfc_rpc_actor_app_alloc();

    nfc_rpc_actor_rpc_init_rpc(app, p);

    rpc_system_app_set_data_exchange_callback(
        app->rpc, nfc_rpc_actor_receive_data_exchange_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, NfcRpcActorViewMain);
    view_dispatcher_run(app->view_dispatcher);

    nfc_rpc_actor_app_free(app);

    return 0;
}