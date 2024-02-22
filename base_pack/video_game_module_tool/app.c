#include <furi.h>

#include <furi_hal_rtc.h>
#include <furi_hal_debug.h>

#include <gui/gui.h>
#include <expansion/expansion.h>

#include "app_i.h"

static bool custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void tick_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));

    app->file_path = furi_string_alloc();

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&scene_handlers, app);

    app->widget = widget_alloc();
    app->submenu = submenu_alloc();
    app->progress = progress_alloc();

    view_dispatcher_add_view(app->view_dispatcher, ViewIdWidget, widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher, ViewIdSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, ViewIdProgress, progress_get_view(app->progress));

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, back_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, tick_event_callback, 500);

    app->notification = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

static void app_free(App* app) {
    furi_record_close(RECORD_NOTIFICATION);

    for(uint32_t i = 0; i < ViewIdMax; ++i) {
        view_dispatcher_remove_view(app->view_dispatcher, i);
    }

    progress_free(app->progress);
    submenu_free(app->submenu);
    widget_free(app->widget);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_string_free(app->file_path);

    free(app);
}

void submenu_item_common_callback(void* context, uint32_t index) {
    furi_assert(context);

    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

int32_t vgm_tool_app(void* arg) {
    UNUSED(arg);

    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    const bool is_debug_enabled = furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug);
    if(is_debug_enabled) {
        furi_hal_debug_disable();
    }

    App* app = app_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);

    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, SceneProbe);

    view_dispatcher_run(app->view_dispatcher);

    flasher_deinit();
    app_free(app);

    furi_record_close(RECORD_GUI);

    if(is_debug_enabled) {
        furi_hal_debug_enable();
    }

    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
