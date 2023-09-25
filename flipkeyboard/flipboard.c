#include "flipboard_i.h"

#include "about.h"

Flipboard* flipboard_alloc(
    KeySettingModelFields fields,
    bool single_mode_button,
    bool attach_keyboard,
    GetPrimaryView get_primary_view) {
    Flipboard* app = (Flipboard*)malloc(sizeof(Flipboard));
    app->model = flipboard_model_alloc(FLIPBOARD_APP_NAME, single_mode_button, fields);
    if(attach_keyboard) {
        flipboard_keyboard_attach(flipboard_model_get_keyboard(app->model));
    }

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->key_config = key_config_alloc(app->model, FlipboardViewConfigureId);

    key_config_register_dispatcher(app->key_config, app->view_dispatcher);
    key_config_register_variable_item_list(app->key_config, FlipboardViewConfigureSubviewId);
    key_config_register_text_input(app->key_config, FlipboardViewConfigureTextInputId);

    app->view_primary = get_primary_view(app);

    app->app_menu = app_menu_alloc(app->view_dispatcher);
    app_menu_add_config(
        app->app_menu,
        key_config_get_view(app->key_config),
        key_config_get_view_id(app->key_config));
    app_menu_add_item(
        app->app_menu, FLIPBOARD_PRIMARY_ITEM_NAME, app->view_primary, FlipboardViewPrimaryId);
    app_menu_add_about(app->app_menu, ABOUT_TEXT, FlipboardViewAboutId);

    app_menu_show(app->app_menu);
    flipboard_leds_update(flipboard_model_get_leds(app->model));

    return app;
}

FlipboardModel* flipboard_get_model(Flipboard* app) {
    return app->model;
}

ViewDispatcher* flipboard_get_view_dispatcher(Flipboard* app) {
    return app->view_dispatcher;
}

View* flipboard_get_primary_view(Flipboard* app) {
    return app->view_primary;
}

uint32_t flipboard_navigation_show_app_menu(void* context) {
    UNUSED(context);
    return FLIPBOARD_APP_MENU_VIEW_ID;
}

void flipboard_free(Flipboard* app) {
    flipboard_model_free(app->model);

    view_free(app->view_primary);
    key_config_free(app->key_config);
    app_menu_free(app->app_menu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}
