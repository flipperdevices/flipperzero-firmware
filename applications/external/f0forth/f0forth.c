#include <gui/view_dispatcher_i.h>

#include "f0forth_i.h"
#include "f0forth_cli.h"

#include "bgloader_api.h"

static bool f0forth_navigation_event_callback(void* context) {
    UNUSED(context);

    return false;
}

static void f0forth_hooked_input_callback(InputEvent* event, void* context) {
    F0ForthState* f0f_state = context;

    if(event->key == InputKeyBack) {
        if(event->type == InputTypeLong) {
            f0f_state->exit_for_real = true;
            view_dispatcher_stop(f0f_state->view_dispatcher);
            return;
        }
    }

    f0f_state->orig_input_cb(event, f0f_state->view_dispatcher);
}

static const char* f0forth_get_bgloader_app_path(const char* args) {
    size_t base_args_len = strlen(APP_BASE_ARGS);

    return (args + base_args_len + 1);
}

static bool f0forth_run_with_bgloader(const char* args) {
    size_t base_args_len = strlen(APP_BASE_ARGS);

    if(args == NULL) {
        return false;
    }

    if(strncmp(args, APP_BASE_ARGS, base_args_len) != 0) {
        return false;
    }

    if(strlen(args) < base_args_len + 2) {
        return false;
    }

    if(args[base_args_len] != ':') {
        return false;
    }

    const char* app_path = f0forth_get_bgloader_app_path(args);
    return furi_record_exists(app_path);
}

static void f0forth_attach_to_gui(F0ForthState* f0f_state) {
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(f0f_state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_switch_to_view(f0f_state->view_dispatcher, F0ForthView_TextBox);
}

static void f0forth_detach_from_gui(F0ForthState* f0f_state) {
    gui_remove_view_port(f0f_state->view_dispatcher->gui, f0f_state->view_dispatcher->view_port);
    f0f_state->view_dispatcher->gui = NULL;
    furi_record_close(RECORD_GUI);
}

static void f0forth_bgloader_loop(F0ForthState* f0f_state, const char* bg_app_path) {
    while(true) {
        view_dispatcher_run(f0f_state->view_dispatcher);

        if(f0f_state->exit_for_real) {
            // exit for real
            break;
        }

        BGLoaderApp* bg_app = furi_record_open(bg_app_path);

        // signal loader that we're ready to go to background
        BGLoaderMessage msg;
        msg.type = BGLoaderMessageType_LoaderBackground;
        furi_check(
            furi_message_queue_put(bg_app->to_loader, &msg, FuriWaitForever) == FuriStatusOk);

        f0forth_detach_from_gui(f0f_state);

        // wait for loader to wake us up again
        furi_check(furi_message_queue_get(bg_app->to_app, &msg, FuriWaitForever) == FuriStatusOk);
        switch(msg.type) {
        case BGLoaderMessageType_AppReattached:
            break;
        default:
            furi_check(0);
        }

        furi_record_close(bg_app_path);

        f0forth_attach_to_gui(f0f_state);
    }
}

static F0ForthState* f0forth_state_alloc(void) {
    F0ForthState* f0f_state = malloc(sizeof(F0ForthState));

    f0f_state->view_dispatcher = view_dispatcher_alloc();
    f0f_state->text_box = text_box_alloc();
    f0f_state->text_box_store = furi_string_alloc();
    f0f_state->cli_running_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    f0f_state->exit_for_real = false;

    return f0f_state;
}

static void f0forth_state_free(F0ForthState* f0f_state) {
    furi_string_free(f0f_state->text_box_store);
    text_box_free(f0f_state->text_box);
    view_dispatcher_free(f0f_state->view_dispatcher);
    furi_mutex_free(f0f_state->cli_running_mutex);
    free(f0f_state);
}

int32_t f0forth(const char* args) {
    F0ForthState* f0f_state = f0forth_state_alloc();

    furi_string_printf(
        f0f_state->text_box_store, "Use the \"" F0FORTH_CMD "\" command on the CLI.\n");

    text_box_set_text(f0f_state->text_box, furi_string_get_cstr(f0f_state->text_box_store));
    text_box_set_focus(f0f_state->text_box, TextBoxFocusEnd);

    f0f_state->orig_input_cb = f0f_state->view_dispatcher->view_port->input_callback;
    view_port_input_callback_set(
        f0f_state->view_dispatcher->view_port, f0forth_hooked_input_callback, f0f_state);

    view_dispatcher_enable_queue(f0f_state->view_dispatcher);

    view_dispatcher_set_event_callback_context(f0f_state->view_dispatcher, f0f_state);
    view_dispatcher_set_navigation_event_callback(
        f0f_state->view_dispatcher, f0forth_navigation_event_callback);

    view_dispatcher_add_view(
        f0f_state->view_dispatcher, F0ForthView_TextBox, text_box_get_view(f0f_state->text_box));

    f0forth_attach_to_gui(f0f_state);

    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, F0FORTH_CMD, CliCommandFlagParallelSafe, f0forth_cmd, f0f_state);
    furi_record_close(RECORD_CLI);

    if(!f0forth_run_with_bgloader(args)) {
        view_dispatcher_run(f0f_state->view_dispatcher);
    } else {
        const char* bg_app_path = f0forth_get_bgloader_app_path(args);
        f0forth_bgloader_loop(f0f_state, bg_app_path);
    }

    furi_string_cat_printf(
        f0f_state->text_box_store,
        "Waiting for all "
        "CLI commands to terminate...\n");
    text_box_set_text(f0f_state->text_box, furi_string_get_cstr(f0f_state->text_box_store));
    text_box_set_focus(f0f_state->text_box, TextBoxFocusEnd);

    cli = furi_record_open(RECORD_CLI);
    cli_delete_command(cli, F0FORTH_CMD);
    furi_record_close(RECORD_CLI);

    furi_check(furi_mutex_acquire(f0f_state->cli_running_mutex, FuriWaitForever) == FuriStatusOk);
    furi_check(furi_mutex_release(f0f_state->cli_running_mutex) == FuriStatusOk);

    furi_record_close(RECORD_GUI);

    view_dispatcher_remove_view(f0f_state->view_dispatcher, F0ForthView_TextBox);

    f0forth_state_free(f0f_state);

    return 0;
}
