#include "wiegand.h"

const GpioPin* const pinD0 = &gpio_ext_pa4;
const GpioPin* const pinD1 = &gpio_ext_pa7;
volatile int bit_count = 0;
volatile bool data[MAX_BITS];
volatile uint32_t data_fall[MAX_BITS];
volatile uint32_t data_rise[MAX_BITS];
bool data_saved = false;

bool wiegand_empty_scene_on_event(void* _ctx, SceneManagerEvent _evt) {
    UNUSED(_ctx);
    UNUSED(_evt);
    return false;
}
void wiegand_empty_scene_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

void (*const basic_scenes_scene_on_enter_handlers[])(void*) = {
    wiegand_main_menu_scene_on_enter,
    wiegand_instructions_scene_on_enter,
    wiegand_read_scene_on_enter,
    wiegand_data_scene_on_enter,
    wiegand_save_scene_on_enter,
    wiegand_load_scene_on_enter,
};

bool (*const basic_scenes_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    wiegand_main_menu_scene_on_event,
    wiegand_empty_scene_on_event, // instructions
    wiegand_empty_scene_on_event, // read
    wiegand_data_scene_on_event,
    wiegand_save_scene_on_event,
    wiegand_empty_scene_on_event, // load
};

void (*const basic_scenes_scene_on_exit_handlers[])(void*) = {
    wiegand_empty_scene_on_exit, // main_menu
    wiegand_empty_scene_on_exit, // instructions
    wiegand_read_scene_on_exit,
    wiegand_empty_scene_on_exit, // data
    wiegand_empty_scene_on_exit, // save
    wiegand_empty_scene_on_exit, // load
};

const SceneManagerHandlers basic_scenes_scene_manager_handlers = {
    .on_enter_handlers = basic_scenes_scene_on_enter_handlers,
    .on_event_handlers = basic_scenes_scene_on_event_handlers,
    .on_exit_handlers = basic_scenes_scene_on_exit_handlers,
    .scene_num = WiegandSceneCount,
};

bool wiegand_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool wiegand_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&basic_scenes_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, wiegand_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, wiegand_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, WiegandSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, WiegandWidgetView, widget_get_view(app->widget));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, WiegandTextInputView, text_input_get_view(app->text_input));
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();
    app->timer = furi_timer_alloc(wiegand_timer_callback, FuriTimerTypePeriodic, app);
    return app;
}

void app_free(void* context) {
    App* app = context;
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, WiegandTextInputView);
    text_input_free(app->text_input);
    view_dispatcher_remove_view(app->view_dispatcher, WiegandWidgetView);
    widget_free(app->widget);
    view_dispatcher_remove_view(app->view_dispatcher, WiegandSubmenuView);
    submenu_free(app->submenu);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    furi_timer_free(app->timer);
    furi_record_close(RECORD_DIALOGS);
    free(app);
}

int wiegand_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, WiegandMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}