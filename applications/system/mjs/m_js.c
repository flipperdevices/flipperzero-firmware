#include <furi.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include "mjs_thread.h"
#include <storage/storage.h>

#define TAG "MJS app"

typedef struct {
    MjsThread* mjs_thread;
} MjsApp;

void mjs_done_callback(void* context) {
    UNUSED(context);
    FURI_LOG_I(TAG, "mjs_done_callback");
}

int32_t m_js_app(void* arg) {
    UNUSED(arg);
    FuriString* name = furi_string_alloc_set(APP_ASSETS_PATH());

    MjsApp* app = malloc(sizeof(MjsApp));

    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    Loading* loading = loading_alloc();

    Gui* gui = furi_record_open("gui");
    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(view_dispatcher, 0, loading_get_view(loading));
    view_dispatcher_switch_to_view(view_dispatcher, 0);

    do {
        if(arg != NULL && strlen(arg) > 0) {
            furi_string_set(name, (const char*)arg);
        } else {
            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(&browser_options, ".js", NULL);
            DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
            if(!dialog_file_browser_show(dialogs, name, name, &browser_options)) break;
            furi_record_close(RECORD_DIALOGS);
        }

        app->mjs_thread = mjs_thread_run(furi_string_get_cstr(name), mjs_done_callback, app);
        // furi_delay_ms(3000);
        // mjs_thread_stop(app->mjs_thread);
        mjs_thread_free(app->mjs_thread);
    } while(false);

    view_dispatcher_remove_view(view_dispatcher, 0);
    loading_free(loading);
    view_dispatcher_free(view_dispatcher);
    furi_record_close("gui");

    furi_string_free(name);

    free(app);
    return 0;
}