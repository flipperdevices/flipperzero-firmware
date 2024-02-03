#include "basic_scenes_split.h"
#include "basic_scenes_split_i.h"

static void (*const basic_scenes_scene_on_enter_handlers[])(void*) = {
    basic_scenes_main_menu_scene_on_enter,
    basic_scenes_lottery_scene_on_enter,
    basic_scenes_greeting_input_scene_on_enter,
    basic_scenes_greeting_message_scene_on_enter,
};

static bool (*const basic_scenes_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    basic_scenes_main_menu_scene_on_event,
    basic_scenes_lottery_scene_on_event,
    basic_scenes_greeting_input_scene_on_event,
    basic_scenes_greeting_message_scene_on_event,
};

static void (*const basic_scenes_scene_on_exit_handlers[])(void*) = {
    basic_scenes_main_menu_scene_on_exit,
    basic_scenes_lottery_scene_on_exit,
    basic_scenes_greeting_input_scene_on_exit,
    basic_scenes_greeting_message_scene_on_exit,
};

static const SceneManagerHandlers basic_scenes_scene_manager_handlers = {
    .on_enter_handlers = basic_scenes_scene_on_enter_handlers,
    .on_event_handlers = basic_scenes_scene_on_event_handlers,
    .on_exit_handlers = basic_scenes_scene_on_exit_handlers,
    .scene_num = BasicScenesSceneCount,
};

static bool basic_scene_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

static bool basic_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->user_name_size = 16;
    app->user_name = malloc(app->user_name_size);
    app->scene_manager = scene_manager_alloc(&basic_scenes_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, basic_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, basic_scene_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BasicScenesSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BasicScenesWidgetView, widget_get_view(app->widget));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BasicScenesTextInputView, text_input_get_view(app->text_input));
    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, BasicScenesSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, BasicScenesWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, BasicScenesTextInputView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    free(app);
}

int32_t basic_scenes_split_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, BasicScenesMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
