#include <flipper_application/flipper_application.h>
#include <flipper_application/elf/elf_file_i.h>
#include <loader/firmware_api/firmware_api.h>
#include <toolbox/path.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include <gui/modules/popup.h>

#include "bgloader_api.h"

#define TAG "BG Loader"

#define APP_TERMINATED_DISPLAY_DELAY 2000

struct FlipperApplication {
    ELFDebugInfo state;
    FlipperApplicationManifest manifest;
    ELFFile* elf;
    FuriThread* thread;
    void* ep_thread_args;
};

typedef enum {
    BGLoaderView_Loading,
    BGLoaderView_AppTerminated,
} BGLoaderView;

static void bgloader_thread_state_callback(FuriThreadState state, void* context) {
    BGLoaderApp* app = context;

    if(state == FuriThreadStateStopped) {
        BGLoaderMessage msg;
        msg.type = BGLoaderMessageType_LoaderExit;
        furi_check(furi_message_queue_put(app->to_loader, &msg, FuriWaitForever) == FuriStatusOk);
    }
}

static BGLoaderApp* bgloader_app_alloc(void) {
    BGLoaderApp* app = malloc(sizeof(BGLoaderApp));

    app->fap = NULL;
    app->thread = NULL;
    app->to_app = NULL;
    app->to_loader = NULL;

    return app;
}

static void bgloader_app_init(BGLoaderApp* app, FlipperApplication* fap, FuriThread* thread) {
    app->fap = fap;
    app->thread = thread;
    app->to_app = furi_message_queue_alloc(1, sizeof(BGLoaderMessage));
    app->to_loader = furi_message_queue_alloc(1, sizeof(BGLoaderMessage));
}

static void bgloader_app_deinit(BGLoaderApp* app) {
    // this frees the thread too
    flipper_application_free(app->fap);
    furi_message_queue_free(app->to_app);
    furi_message_queue_free(app->to_loader);

    app->fap = NULL;
    app->thread = NULL;
    app->to_app = NULL;
    app->to_loader = NULL;
}

static void bgloader_check_app_dead(BGLoaderApp* app) {
    furi_check(app->fap == NULL);
    furi_check(app->thread == NULL);
    furi_check(app->to_app == NULL);
    furi_check(app->to_loader == NULL);
}

static void bgloader_check_app_alive(BGLoaderApp* app) {
    furi_check(app->fap != NULL);
    furi_check(app->thread != NULL);
    furi_check(app->to_app != NULL);
    furi_check(app->to_loader != NULL);
}

static bool bgloader_start_external_app(
    BGLoaderApp* app,
    Storage* storage,
    const char* path,
    const char* args) {
    FlipperApplication* fap = NULL;
    FuriThread* thread = NULL;
    bool success = false;

    do {
        fap = flipper_application_alloc(storage, firmware_api_interface);

        FURI_LOG_I(TAG, "Loading %s", path);

        FlipperApplicationPreloadStatus preload_res = flipper_application_preload(fap, path);
        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            const char* err_msg = flipper_application_preload_status_to_string(preload_res);
            FURI_LOG_W(TAG, "Preload failed, %s: %s", path, err_msg);
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(fap);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            const char* err_msg = flipper_application_load_status_to_string(load_status);
            FURI_LOG_W(TAG, "Load failed, %s: %s", path, err_msg);
            break;
        }

        FURI_LOG_I(TAG, "Loaded");

        thread = flipper_application_alloc_thread(fap, args);
        FuriString* app_name = furi_string_alloc();
        path_extract_filename_no_ext(path, app_name);
        furi_thread_set_appid(thread, furi_string_get_cstr(app_name));
        furi_string_free(app_name);

        success = true;
    } while(0);

    if(!success) {
        flipper_application_free(fap);
        return false;
    }

    bgloader_app_init(app, fap, thread);
    furi_thread_set_state_callback(thread, bgloader_thread_state_callback);
    furi_thread_set_state_context(thread, app);
    furi_thread_start(thread);

    return true;
}

static BGLoaderApp* bgloader_take_app(const char* path) {
    if(furi_record_exists(path)) {
        return furi_record_open(path);
    }

    BGLoaderApp* app = bgloader_app_alloc();

    furi_record_create(path, app);

    return furi_record_open(path);
}

static void bgloader_put_app(const char* path) {
    furi_record_close(path);
}

static bool bgloader_app_is_loaded(BGLoaderApp* app) {
    return (app->fap != NULL);
}

static bool bgloader_load_app(BGLoaderApp* app, const char* path) {
    bgloader_check_app_dead(app);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage_file_exists(storage, path)) {
        FURI_LOG_W(TAG, "Application \"%s\" not found", path);
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    FuriString* args = furi_string_alloc();
    furi_string_printf(args, "%s:%s", APP_BASE_ARGS, path);
    bool status = bgloader_start_external_app(app, storage, path, furi_string_get_cstr(args));
    furi_string_free(args);

    if(status) {
        bgloader_check_app_alive(app);
        if(app->fap->elf->fd != NULL) {
            storage_file_close(app->fap->elf->fd);
        }
    } else {
        bgloader_check_app_dead(app);
    }

    furi_record_close(RECORD_STORAGE);

    return status;
}

static bool bgloader_continue_app(BGLoaderApp* app) {
    bgloader_check_app_alive(app);

    furi_thread_set_state_callback(app->thread, bgloader_thread_state_callback);
    furi_thread_set_state_context(app->thread, app);

    BGLoaderMessage msg;
    msg.type = BGLoaderMessageType_AppReattached;
    furi_check(furi_message_queue_put(app->to_app, &msg, FuriWaitForever) == FuriStatusOk);

    return true;
}

static bool bgloader_check_app_termination(BGLoaderApp* app) {
    bgloader_check_app_alive(app);

    BGLoaderMessage msg;
    furi_check(furi_message_queue_get(app->to_loader, &msg, FuriWaitForever) == FuriStatusOk);
    switch(msg.type) {
    case BGLoaderMessageType_LoaderBackground:
        return false;
    case BGLoaderMessageType_LoaderExit:
        return true;
    default:
        furi_check(0);
    }

    return false;
}

static bool bgloader_unload_app(BGLoaderApp* app) {
    bgloader_check_app_alive(app);

    furi_thread_join(app->thread);
    FURI_LOG_I(TAG, "App returned: %li", furi_thread_get_return_code(app->thread));

    bgloader_app_deinit(app);

    return true;
}

static bool bgloader_handle_app(const char* path) {
    BGLoaderApp* app = bgloader_take_app(path);

    if(!bgloader_app_is_loaded(app)) {
        // load app
        if(!bgloader_load_app(app, path)) {
            FURI_LOG_W(TAG, "Failed to load application \"%s\"", path);
            bgloader_put_app(path);
            return false;
        } else {
            FURI_LOG_I(TAG, "Application loaded");
        }
    } else {
        // continue app
        if(!bgloader_continue_app(app)) {
            FURI_LOG_W(
                TAG,
                "Failed to continue application "
                "\"%s\"",
                path);
            bgloader_put_app(path);
            return false;
        } else {
            FURI_LOG_I(TAG, "Application continued");
        }
    }

    bool app_terminated = bgloader_check_app_termination(app);
    furi_thread_set_state_callback(app->thread, NULL);
    furi_thread_set_state_context(app->thread, NULL);
    if(app_terminated) {
        // unload app
        if(!bgloader_unload_app(app)) {
            FURI_LOG_W(TAG, "Failed to unload application \"%s\"", path);
        } else {
            FURI_LOG_I(TAG, "Application unloaded");
            bgloader_put_app(path);
            return true;
        }
    } else {
        FURI_LOG_I(TAG, "Application continuing in background");
    }

    bgloader_put_app(path);

    return false;
}

int32_t bgloader(const char* args) {
    UNUSED(args);

    FuriString* fap_path = furi_string_alloc_set(EXT_PATH("apps"));
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    Gui* gui = furi_record_open(RECORD_GUI);
    Loading* loading = loading_alloc();
    Popup* term_popup = popup_alloc();
    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();

    popup_set_header(term_popup, "Terminated", 64, 24, AlignCenter, AlignTop);

    view_dispatcher_add_view(view_dispatcher, BGLoaderView_Loading, loading_get_view(loading));
    view_dispatcher_add_view(
        view_dispatcher, BGLoaderView_AppTerminated, popup_get_view(term_popup));
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_switch_to_view(view_dispatcher, BGLoaderView_Loading);

    const DialogsFileBrowserOptions browser_options = {
        .extension = ".fap",
        .skip_assets = true,
        .icon = NULL,
        .hide_ext = true,
        .item_loader_callback = NULL,
        .item_loader_context = NULL,
        .base_path = EXT_PATH("apps"),
    };

    bool fap_selected = dialog_file_browser_show(dialogs, fap_path, fap_path, &browser_options);

    if(fap_selected) {
        FURI_LOG_I(TAG, "Selected fap \"%s\"", furi_string_get_cstr(fap_path));
        if(bgloader_handle_app(furi_string_get_cstr(fap_path))) {
            view_dispatcher_switch_to_view(view_dispatcher, BGLoaderView_AppTerminated);
            furi_delay_ms(APP_TERMINATED_DISPLAY_DELAY);
        }
    } else {
        FURI_LOG_I(TAG, "No fap selected");
    }

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_DIALOGS);

    view_dispatcher_remove_view(view_dispatcher, BGLoaderView_Loading);
    view_dispatcher_remove_view(view_dispatcher, BGLoaderView_AppTerminated);

    view_dispatcher_free(view_dispatcher);
    popup_free(term_popup);
    loading_free(loading);
    furi_string_free(fap_path);

    FURI_LOG_I(TAG, "Exiting");

    return 0;
}
