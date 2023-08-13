#include "text_viewer.h"

static bool text_viewer_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    TextViewer* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool text_viewer_back_event_callback(void* context) {
    furi_assert(context);
    TextViewer* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

TextViewer* text_viewer_alloc() {
    TextViewer* app = malloc(sizeof(TextViewer));
    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&text_viewer_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, text_viewer_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, text_viewer_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, TextViewerViewWidget, widget_get_view(app->widget));

    app->path = furi_string_alloc();

    return app;
}

void text_viewer_free(TextViewer* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, TextViewerViewWidget);
    widget_free(app->widget);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_string_free(app->path);

    furi_record_close(RECORD_GUI);
    free(app);
}

extern int32_t text_viewer(void* p) {
    TextViewer* app = text_viewer_alloc();

    do {
        if(p && strlen(p)) {
            furi_string_set(app->path, (const char*)p);
        } else {
            furi_string_set(app->path, TEXT_VIEWER_PATH);

            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(
                &browser_options, TEXT_VIEWER_EXTENSION, &I_text_10px);
            browser_options.hide_ext = false;

            DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
            bool res = dialog_file_browser_show(dialogs, app->path, app->path, &browser_options);

            furi_record_close(RECORD_DIALOGS);
            if(!res) {
                break;
            }
        }

        scene_manager_next_scene(app->scene_manager, TextViewerSceneShow);
        view_dispatcher_run(app->view_dispatcher);
    } while(false);

    text_viewer_free(app);
    return 0;
}
