#include "flipboard_i.h"

/**
 * @brief Allocates a new Flipboard application.
 * @param app_name The name of the application.
 * @param primary_item_name The name of the primary action (the purpose of your app).
 * @param about_text The text to display in the about view.
 * @param fields The fields to display in the button model.
 * @param single_mode_button Whether to display the button model in single mode.
 * @param attach_keyboard Whether to attach the keyboard to the application.
 * @param keys The keys to display in the keystroke selector.
 * @param shift_keys The shift keys to display in the keystroke selector.
 * @param rows The number of rows to display in the keystroke selector.
 * @param get_primary_view Callback to get the primary view of the application.
 * @return A pointer to the allocated Flipboard application.
*/
Flipboard* flipboard_alloc(
    char* app_name,
    char* primary_item_name,
    char* about_text,
    ActionModelFields fields,
    bool single_mode_button,
    bool attach_keyboard,
    KeystrokeSelectorKey* keys,
    KeystrokeSelectorKey* shift_keys,
    uint8_t rows,
    GetPrimaryView get_primary_view) {
    Flipboard* app = (Flipboard*)malloc(sizeof(Flipboard));
    app->model = flipboard_model_alloc(app_name, single_mode_button, fields);
    if(attach_keyboard) {
        flipboard_keyboard_attach(flipboard_model_get_keyboard(app->model));
    }

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->action_config =
        action_config_alloc(app->model, FlipboardViewConfigureId, keys, shift_keys, rows);

    action_config_register_dispatcher(app->action_config, app->view_dispatcher);
    action_config_register_variable_item_list(app->action_config, FlipboardViewConfigureSubviewId);
    action_config_register_text_input(app->action_config, FlipboardViewConfigureTextInputId);
    action_config_register_keystroke_selector(
        app->action_config, FlipboardViewConfigureKeystrokeSelectorId);

    app->view_primary = get_primary_view(app);

    app->app_menu = app_menu_alloc(app->view_dispatcher);
    app_menu_add_item(
        app->app_menu,
        "Config",
        action_config_get_view(app->action_config),
        action_config_get_view_id(app->action_config));

    app_menu_add_item(app->app_menu, primary_item_name, app->view_primary, FlipboardViewPrimaryId);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(app->widget_about, 0, 0, 128, 64, about_text);
    app_menu_add_item(
        app->app_menu, "About", widget_get_view(app->widget_about), FlipboardViewAboutId);

    app_menu_show(app->app_menu);
    flipboard_leds_update(flipboard_model_get_leds(app->model));

    return app;
}

/**
 * @brief Frees a Flipboard application.
 * @param app The Flipboard application to free.
*/
void flipboard_free(Flipboard* app) {
    flipboard_model_free(app->model);

    view_free(app->view_primary);
    if(app->action_config) {
        action_config_free(app->action_config);
    }
    widget_free(app->widget_about);
    app_menu_free(app->app_menu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

/**
 * @brief Gets the model of a Flipboard application.
 * @param app The Flipboard application.
 * @return A pointer to the model of the Flipboard application.
*/
FlipboardModel* flipboard_get_model(Flipboard* app) {
    return app->model;
}

/**
 * @brief Gets the view dispatcher of a Flipboard application.
 * @param app The Flipboard application.
 * @return A pointer to the view dispatcher of the Flipboard application.
*/
ViewDispatcher* flipboard_get_view_dispatcher(Flipboard* app) {
    return app->view_dispatcher;
}

/**
 * @brief Gets the primary view of a Flipboard application.
 * @param app The Flipboard application.
 * @return A pointer to the primary view of the Flipboard application.
*/
View* flipboard_get_primary_view(Flipboard* app) {
    return app->view_primary;
}

/**
 * @brief Overrides the config view of a Flipboard application.
 * @param app The Flipboard application.
 * @param view The view to override the config view with.
*/
void flipboard_override_config_view(Flipboard* app, View* view) {
    if(app->action_config) {
        action_config_free(app->action_config);
        app->action_config = NULL;
    }
    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewConfigureId);
    view_dispatcher_add_view(app->view_dispatcher, FlipboardViewConfigureId, view);
}

/**
 * @brief Gets the button model of a Flipboard application.
 * @param app The Flipboard application.
 * @return The view id for the application menu (it should be 0.)
*/
uint32_t flipboard_navigation_show_app_menu(void* context) {
    UNUSED(context);
    return FLIPBOARD_APP_MENU_VIEW_ID;
}
