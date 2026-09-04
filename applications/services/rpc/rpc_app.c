#include "flipper.pb.h"
#include <core/record.h>
#include "rpc_i.h"
#include <furi.h>
#include <loader/loader.h>
#include "rpc_app.h"

#define TAG "RpcSystemApp"

struct RpcAppSystem {
    RpcSession* session;

    RpcAppSystemCallback callback;
    void* callback_context;

    uint32_t error_code;
    char* error_text;

    uint32_t last_command_id;
    RpcAppSystemEventType last_event_type;
};

#define RPC_SYSTEM_APP_TEMP_ARGS_SIZE 16

/* Upper bound on how long session teardown waits for the app to acknowledge the
 * session close (by clearing its callback) before proceeding regardless. */
#define RPC_APP_SESSION_CLOSE_TIMEOUT_MS 250

static void rpc_system_app_send_state_response(
    RpcAppSystem* rpc_app,
    PB_App_AppState state,
    const char* name) {
    PB_Main* response = malloc(sizeof(PB_Main));

    response->which_content = PB_Main_app_state_response_tag;
    response->content.app_state_response.state = state;

    FURI_LOG_D(TAG, "%s", name);
    rpc_send(rpc_app->session, response);

    free(response);
}

static void rpc_system_app_send_error_response(
    RpcAppSystem* rpc_app,
    uint32_t command_id,
    PB_CommandStatus status,
    const char* name) {
    // Not describing all possible errors as only APP_NOT_RUNNING is used
    const char* status_str = status == PB_CommandStatus_ERROR_APP_NOT_RUNNING ? "APP_NOT_RUNNING" :
                                                                                "UNKNOWN";
    FURI_LOG_E(TAG, "%s: %s, id %lu, status: %d", name, status_str, command_id, status);
    rpc_send_and_release_empty(rpc_app->session, command_id, status);
}

static void rpc_system_app_set_last_command(
    RpcAppSystem* rpc_app,
    uint32_t command_id,
    const RpcAppSystemEvent* event) {
    furi_assert(rpc_app->last_command_id == 0);
    furi_assert(rpc_app->last_event_type == RpcAppEventTypeInvalid);

    rpc_app->last_command_id = command_id;
    rpc_app->last_event_type = event->type;
}

static void rpc_system_app_start_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_start_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);
    furi_assert(rpc_app->last_command_id == 0);
    furi_assert(rpc_app->last_event_type == RpcAppEventTypeInvalid);

    FURI_LOG_D(TAG, "StartProcess: id %lu", request->command_id);

    Loader* loader = furi_record_open(RECORD_LOADER);
    const char* app_name = request->content.app_start_request.name;

    PB_CommandStatus result;

    if(app_name) {
        rpc_system_app_error_reset(rpc_app);

        char app_args_temp[RPC_SYSTEM_APP_TEMP_ARGS_SIZE];
        const char* app_args = request->content.app_start_request.args;

        if(app_args && strcmp(app_args, "RPC") == 0) {
            // If app is being started in RPC mode - pass RPC context via args string
            snprintf(app_args_temp, RPC_SYSTEM_APP_TEMP_ARGS_SIZE, "RPC %08lX", (uint32_t)rpc_app);
            app_args = app_args_temp;
        }

        const LoaderStatus status = loader_start(loader, app_name, app_args, NULL);
        if(status == LoaderStatusErrorAppStarted) {
            result = PB_CommandStatus_ERROR_APP_SYSTEM_LOCKED;
        } else if(status == LoaderStatusErrorInternal) {
            result = PB_CommandStatus_ERROR_APP_CANT_START;
        } else if(status == LoaderStatusErrorUnknownApp) {
            result = PB_CommandStatus_ERROR_INVALID_PARAMETERS;
        } else if(status == LoaderStatusOk) {
            result = PB_CommandStatus_OK;
        } else {
            furi_crash();
        }
    } else {
        result = PB_CommandStatus_ERROR_INVALID_PARAMETERS;
    }

    furi_record_close(RECORD_LOADER);

    FURI_LOG_D(TAG, "StartProcess: response id %lu, result %d", request->command_id, result);
    rpc_send_and_release_empty(rpc_app->session, request->command_id, result);
}

static void rpc_system_app_lock_status_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_lock_status_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    rpc_system_app_error_reset(rpc_app);

    FURI_LOG_D(TAG, "LockStatus");

    PB_Main* response = malloc(sizeof(PB_Main));

    response->command_id = request->command_id;
    response->which_content = PB_Main_app_lock_status_response_tag;

    Loader* loader = furi_record_open(RECORD_LOADER);
    response->content.app_lock_status_response.locked = loader_is_locked(loader);
    furi_record_close(RECORD_LOADER);

    FURI_LOG_D(TAG, "LockStatus: response");
    rpc_send_and_release(rpc_app->session, response);

    free(response);
}

static void rpc_system_app_exit_request(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_exit_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    if(rpc_app->callback) {
        FURI_LOG_D(TAG, "ExitRequest: id %lu", request->command_id);

        const RpcAppSystemEvent event = {
            .type = RpcAppEventTypeAppExit,
            .data =
                {
                    .type = RpcAppSystemEventDataTypeNone,
                    {0},
                },
        };

        rpc_system_app_error_reset(rpc_app);
        rpc_system_app_set_last_command(rpc_app, request->command_id, &event);

        rpc_app->callback(&event, rpc_app->callback_context);

    } else {
        rpc_system_app_send_error_response(
            rpc_app, request->command_id, PB_CommandStatus_ERROR_APP_NOT_RUNNING, "ExitRequest");
    }
}

static void rpc_system_app_load_file(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_load_file_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    if(rpc_app->callback) {
        FURI_LOG_D(TAG, "LoadFile: id %lu", request->command_id);

        const RpcAppSystemEvent event = {
            .type = RpcAppEventTypeLoadFile,
            .data =
                {
                    .type = RpcAppSystemEventDataTypeString,
                    .string = request->content.app_load_file_request.path,
                },
        };

        rpc_system_app_error_reset(rpc_app);
        rpc_system_app_set_last_command(rpc_app, request->command_id, &event);

        rpc_app->callback(&event, rpc_app->callback_context);

    } else {
        rpc_system_app_send_error_response(
            rpc_app, request->command_id, PB_CommandStatus_ERROR_APP_NOT_RUNNING, "LoadFile");
    }
}

static void rpc_system_app_button_press(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_button_press_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    if(rpc_app->callback) {
        FURI_LOG_D(TAG, "ButtonPress");

        RpcAppSystemEvent event;
        event.type = RpcAppEventTypeButtonPress;

        if(strlen(request->content.app_button_press_request.args) != 0) {
            event.data.type = RpcAppSystemEventDataTypeString;
            event.data.string = request->content.app_button_press_request.args;
        } else {
            event.data.type = RpcAppSystemEventDataTypeInt32;
            event.data.i32 = request->content.app_button_press_request.index;
        }

        rpc_system_app_error_reset(rpc_app);
        rpc_system_app_set_last_command(rpc_app, request->command_id, &event);

        rpc_app->callback(&event, rpc_app->callback_context);

    } else {
        rpc_system_app_send_error_response(
            rpc_app, request->command_id, PB_CommandStatus_ERROR_APP_NOT_RUNNING, "ButtonPress");
    }
}

static void rpc_system_app_button_release(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_button_release_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    if(rpc_app->callback) {
        FURI_LOG_D(TAG, "ButtonRelease");

        const RpcAppSystemEvent event = {
            .type = RpcAppEventTypeButtonRelease,
            .data =
                {
                    .type = RpcAppSystemEventDataTypeNone,
                    {0},
                },
        };

        rpc_system_app_error_reset(rpc_app);
        rpc_system_app_set_last_command(rpc_app, request->command_id, &event);

        rpc_app->callback(&event, rpc_app->callback_context);

    } else {
        rpc_system_app_send_error_response(
            rpc_app, request->command_id, PB_CommandStatus_ERROR_APP_NOT_RUNNING, "ButtonRelease");
    }
}

static void rpc_system_app_button_press_release(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_button_press_release_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    if(rpc_app->callback) {
        FURI_LOG_D(TAG, "ButtonPressRelease");

        RpcAppSystemEvent event;
        event.type = RpcAppEventTypeButtonPressRelease;

        if(strlen(request->content.app_button_press_release_request.args) != 0) {
            event.data.type = RpcAppSystemEventDataTypeString;
            event.data.string = request->content.app_button_press_release_request.args;
        } else {
            event.data.type = RpcAppSystemEventDataTypeInt32;
            event.data.i32 = request->content.app_button_press_release_request.index;
        }

        rpc_system_app_error_reset(rpc_app);
        rpc_system_app_set_last_command(rpc_app, request->command_id, &event);

        rpc_app->callback(&event, rpc_app->callback_context);

    } else {
        rpc_system_app_send_error_response(
            rpc_app,
            request->command_id,
            PB_CommandStatus_ERROR_APP_NOT_RUNNING,
            "ButtonPressRelease");
    }
}

static void rpc_system_app_get_error_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_get_error_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    PB_Main* response = malloc(sizeof(PB_Main));

    response->command_id = request->command_id;
    response->which_content = PB_Main_app_get_error_response_tag;
    response->content.app_get_error_response.code = rpc_app->error_code;
    response->content.app_get_error_response.text = rpc_app->error_text;

    FURI_LOG_D(TAG, "GetError");
    rpc_send(rpc_app->session, response);

    free(response);
}

static void rpc_system_app_data_exchange_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_app_data_exchange_request_tag);

    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);

    if(rpc_app->callback) {
        FURI_LOG_D(TAG, "DataExchange");

        const pb_bytes_array_t* data = request->content.app_data_exchange_request.data;

        const RpcAppSystemEvent event = {
            .type = RpcAppEventTypeDataExchange,
            .data =
                {
                    .type = RpcAppSystemEventDataTypeBytes,
                    .bytes =
                        {
                            .ptr = data ? data->bytes : NULL,
                            .size = data ? data->size : 0,
                        },
                },
        };

        rpc_system_app_error_reset(rpc_app);
        rpc_system_app_set_last_command(rpc_app, request->command_id, &event);

        rpc_app->callback(&event, rpc_app->callback_context);
    } else {
        rpc_system_app_send_error_response(
            rpc_app, request->command_id, PB_CommandStatus_ERROR_APP_NOT_RUNNING, "DataExchange");
    }
}

/* The functions below form the app-facing RPC API. They are invoked by the
 * owning application, usually from the application's own thread. When the RPC
 * session is torn down (transport disconnect), the service clears the app's
 * context on a different thread; the application, racing that teardown, can
 * call these with an already-cleared (NULL) handle. Treat a NULL handle as a
 * no-op instead of crashing on furi_check - the operation is meaningless once
 * the session is gone (issue #4073). */

void rpc_system_app_send_started(RpcAppSystem* rpc_app) {
    if(rpc_app == NULL) return;
    rpc_system_app_send_state_response(rpc_app, PB_App_AppState_APP_STARTED, "SendStarted");
}

void rpc_system_app_send_exited(RpcAppSystem* rpc_app) {
    if(rpc_app == NULL) return;
    rpc_system_app_send_state_response(rpc_app, PB_App_AppState_APP_CLOSED, "SendExit");
}

void rpc_system_app_confirm(RpcAppSystem* rpc_app, bool result) {
    if(rpc_app == NULL) return;
    furi_check(rpc_app->last_command_id != 0);
    /* Ensure that only commands of these types can be confirmed */
    furi_check(
        rpc_app->last_event_type == RpcAppEventTypeAppExit ||
        rpc_app->last_event_type == RpcAppEventTypeLoadFile ||
        rpc_app->last_event_type == RpcAppEventTypeButtonPress ||
        rpc_app->last_event_type == RpcAppEventTypeButtonRelease ||
        rpc_app->last_event_type == RpcAppEventTypeButtonPressRelease ||
        rpc_app->last_event_type == RpcAppEventTypeDataExchange);

    const uint32_t last_command_id = rpc_app->last_command_id;
    const RpcAppSystemEventType last_event_type = rpc_app->last_event_type;

    rpc_app->last_command_id = 0;
    rpc_app->last_event_type = RpcAppEventTypeInvalid;

    const PB_CommandStatus status = result ? PB_CommandStatus_OK :
                                             PB_CommandStatus_ERROR_APP_CMD_ERROR;
    FURI_LOG_D(
        TAG,
        "AppConfirm: event %d last_id %lu status %d",
        last_event_type,
        last_command_id,
        status);

    rpc_send_and_release_empty(rpc_app->session, last_command_id, status);
}

void rpc_system_app_set_callback(RpcAppSystem* rpc_app, RpcAppSystemCallback callback, void* ctx) {
    if(rpc_app == NULL) return;

    rpc_app->callback = callback;
    rpc_app->callback_context = ctx;
}

void rpc_system_app_set_error_code(RpcAppSystem* rpc_app, uint32_t error_code) {
    if(rpc_app == NULL) return;
    rpc_app->error_code = error_code;
}

void rpc_system_app_set_error_text(RpcAppSystem* rpc_app, const char* error_text) {
    if(rpc_app == NULL) return;

    if(rpc_app->error_text) {
        free(rpc_app->error_text);
    }

    rpc_app->error_text = error_text ? strdup(error_text) : NULL;
}

void rpc_system_app_error_reset(RpcAppSystem* rpc_app) {
    if(rpc_app == NULL) return;

    rpc_system_app_set_error_code(rpc_app, 0);
    rpc_system_app_set_error_text(rpc_app, NULL);
}

void rpc_system_app_exchange_data(RpcAppSystem* rpc_app, const uint8_t* data, size_t data_size) {
    if(rpc_app == NULL) return;

    PB_Main* request = malloc(sizeof(PB_Main));

    request->which_content = PB_Main_app_data_exchange_request_tag;
    PB_App_DataExchangeRequest* content = &request->content.app_data_exchange_request;

    if(data && data_size) {
        content->data = malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(data_size));
        content->data->size = data_size;
        memcpy(content->data->bytes, data, data_size);
    } else {
        content->data = NULL;
    }

    rpc_send_and_release(rpc_app->session, request);

    free(request);
}

void* rpc_system_app_alloc(RpcSession* session) {
    furi_assert(session);

    RpcAppSystem* rpc_app = malloc(sizeof(RpcAppSystem));
    rpc_app->session = session;

    RpcHandler rpc_handler = {
        .message_handler = NULL,
        .decode_submessage = NULL,
        .context = rpc_app,
    };

    rpc_handler.message_handler = rpc_system_app_start_process;
    rpc_add_handler(session, PB_Main_app_start_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_lock_status_process;
    rpc_add_handler(session, PB_Main_app_lock_status_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_exit_request;
    rpc_add_handler(session, PB_Main_app_exit_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_load_file;
    rpc_add_handler(session, PB_Main_app_load_file_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_button_press;
    rpc_add_handler(session, PB_Main_app_button_press_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_button_release;
    rpc_add_handler(session, PB_Main_app_button_release_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_button_press_release;
    rpc_add_handler(session, PB_Main_app_button_press_release_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_get_error_process;
    rpc_add_handler(session, PB_Main_app_get_error_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_app_data_exchange_process;
    rpc_add_handler(session, PB_Main_app_data_exchange_request_tag, &rpc_handler);

    return rpc_app;
}

void rpc_system_app_free(void* context) {
    RpcAppSystem* rpc_app = context;
    furi_assert(rpc_app);
    furi_assert(rpc_app->session);

    if(rpc_app->callback) {
        const RpcAppSystemEvent event = {
            .type = RpcAppEventTypeSessionClose,
            .data =
                {
                    .type = RpcAppSystemEventDataTypeNone,
                    {0},
                },
        };

        rpc_app->callback(&event, rpc_app->callback_context);
    }

    /* Wait for the app to acknowledge the session close by clearing its callback,
     * but bound the wait so a stuck or already-gone app cannot wedge the RPC
     * service thread (which runs the timer/pending-callback context). In-tree
     * apps clear their callback synchronously inside the SessionClose callback
     * above, so for them this loop exits immediately; the timeout only matters
     * for out-of-tree apps that defer the acknowledgement, where proceeding
     * regardless is preferable to blocking the timer service forever. */
    uint32_t elapsed_ticks = 0;
    const uint32_t timeout_ticks = furi_ms_to_ticks(RPC_APP_SESSION_CLOSE_TIMEOUT_MS);
    while(rpc_app->callback && elapsed_ticks < timeout_ticks) {
        furi_delay_tick(1);
        elapsed_ticks++;
    }

    free(rpc_app);
}
