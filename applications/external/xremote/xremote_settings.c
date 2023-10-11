/*!
 *  @file flipper-xremote/xremote_settings.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief XRemote applications settings menu and functionality.
 */

#include "xremote_settings.h"

typedef struct {
    VariableItemList* item_list;
    XRemoteAppContext* app_ctx;
} XRemoteSettingsContext;

#define XREMOTE_ORIENTATION_TEXT_HORIZONTAL "Horizontal"
#define XREMOTE_ORIENTATION_TEXT_VERTICAL "Vertical"
#define XREMOTE_ORIENTATION_TEXT "Orientation"
#define XREMOTE_ORIENTATION_INDEX_HORIZONTAL 0
#define XREMOTE_ORIENTATION_INDEX_VERTICAL 1
#define XREMOTE_ORIENTATION_MAX 2

#define XREMOTE_EXIT_BEHAVIOR_TEXT_PRESS "Press"
#define XREMOTE_EXIT_BEHAVIOR_TEXT_HOLD "Hold"
#define XREMOTE_EXIT_BEHAVIOR_TEXT "Exit Apps"
#define XREMOTE_EXIT_BEHAVIOR_INDEX_PRESS 0
#define XREMOTE_EXIT_BEHAVIOR_INDEX_HOLD 1
#define XREMOTE_EXIT_BEHAVIOR_MAX 2

#define XREMOTE_REPEAT_TEXT "IR Msg Repeat"
#define XREMOTE_REPEAT_MAX 128

static uint32_t xremote_settings_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewSubmenu;
}

static uint32_t xremote_settings_get_exit_index(XRemoteAppSettings* settings) {
    return settings->exit_behavior == XRemoteAppExitPress ? XREMOTE_EXIT_BEHAVIOR_INDEX_PRESS :
                                                            XREMOTE_EXIT_BEHAVIOR_INDEX_HOLD;
}

static const char* xremote_settings_get_exit_str(XRemoteAppSettings* settings) {
    return settings->exit_behavior == XRemoteAppExitPress ? XREMOTE_EXIT_BEHAVIOR_TEXT_PRESS :
                                                            XREMOTE_EXIT_BEHAVIOR_TEXT_HOLD;
}

static XRemoteAppExit xremote_settings_get_exit_behavior(uint8_t exit_behavior) {
    return exit_behavior == XREMOTE_EXIT_BEHAVIOR_INDEX_PRESS ? XRemoteAppExitPress :
                                                                XRemoteAppExitHold;
}

static uint32_t xremote_settings_get_orientation_index(XRemoteAppSettings* settings) {
    return settings->orientation == ViewOrientationHorizontal ?
               XREMOTE_ORIENTATION_INDEX_HORIZONTAL :
               XREMOTE_ORIENTATION_INDEX_VERTICAL;
}

static const char* xremote_settings_get_orientation_str(XRemoteAppSettings* settings) {
    return settings->orientation == ViewOrientationHorizontal ?
               XREMOTE_ORIENTATION_TEXT_HORIZONTAL :
               XREMOTE_ORIENTATION_TEXT_VERTICAL;
}

static ViewOrientation xremote_settings_get_orientation(uint8_t orientation) {
    return orientation == XREMOTE_ORIENTATION_INDEX_HORIZONTAL ? ViewOrientationHorizontal :
                                                                 ViewOrientationVertical;
}

static void infrared_settings_orientation_changed(VariableItem* item) {
    XRemoteSettingsContext* ctx = variable_item_get_context(item);
    XRemoteAppSettings* settings = ctx->app_ctx->app_settings;

    uint8_t orientation = variable_item_get_current_value_index(item);
    settings->orientation = xremote_settings_get_orientation(orientation);
    const char* orientation_str = xremote_settings_get_orientation_str(settings);

    variable_item_set_current_value_text(item, orientation_str);
    xremote_app_settings_store(settings);
}

static void infrared_settings_repeat_changed(VariableItem* item) {
    XRemoteSettingsContext* ctx = variable_item_get_context(item);
    XRemoteAppSettings* settings = ctx->app_ctx->app_settings;
    char repeat_str[8];

    settings->repeat_count = variable_item_get_current_value_index(item);
    snprintf(repeat_str, sizeof(repeat_str), "%lu", settings->repeat_count);

    variable_item_set_current_value_text(item, repeat_str);
    xremote_app_settings_store(settings);
}

static void infrared_settings_exit_changed(VariableItem* item) {
    XRemoteSettingsContext* ctx = variable_item_get_context(item);
    XRemoteAppSettings* settings = ctx->app_ctx->app_settings;

    uint8_t exit = variable_item_get_current_value_index(item);
    settings->exit_behavior = xremote_settings_get_exit_behavior(exit);
    const char* exit_str = xremote_settings_get_exit_str(settings);

    variable_item_set_current_value_text(item, exit_str);
    xremote_app_settings_store(settings);
}

static XRemoteSettingsContext* xremote_settings_context_alloc(XRemoteAppContext* app_ctx) {
    XRemoteSettingsContext* context = malloc(sizeof(XRemoteSettingsContext));
    XRemoteAppSettings* settings = app_ctx->app_settings;

    context->item_list = variable_item_list_alloc();
    context->app_ctx = app_ctx;
    char repeat_str[8];

    /* Configure variable item list view */
    View* view = variable_item_list_get_view(context->item_list);
    view_set_previous_callback(view, xremote_settings_view_exit_callback);
    view_dispatcher_add_view(app_ctx->view_dispatcher, XRemoteViewSettings, view);

    /* Add settings to variable item list */
    VariableItem* item = variable_item_list_add(
        context->item_list,
        XREMOTE_ORIENTATION_TEXT,
        XREMOTE_ORIENTATION_MAX,
        infrared_settings_orientation_changed,
        context);

    /* Get application orientation settings */
    const char* orient_str = xremote_settings_get_orientation_str(settings);
    uint32_t orient_index = xremote_settings_get_orientation_index(settings);

    /* Set current orientation item index and string */
    variable_item_set_current_value_index(item, orient_index);
    variable_item_set_current_value_text(item, orient_str);

    /* Add IR message repeat counter to variable item list */
    item = variable_item_list_add(
        context->item_list,
        XREMOTE_REPEAT_TEXT,
        XREMOTE_REPEAT_MAX,
        infrared_settings_repeat_changed,
        context);

    /* Set repeat count item index and string */
    snprintf(repeat_str, sizeof(repeat_str), "%lu", settings->repeat_count);
    variable_item_set_current_value_index(item, settings->repeat_count);
    variable_item_set_current_value_text(item, repeat_str);

    /* Add exit behavior to variable item list */
    item = variable_item_list_add(
        context->item_list,
        XREMOTE_EXIT_BEHAVIOR_TEXT,
        XREMOTE_EXIT_BEHAVIOR_MAX,
        infrared_settings_exit_changed,
        context);

    /* Get exit behavior settings */
    const char* exit_str = xremote_settings_get_exit_str(settings);
    uint32_t exit_index = xremote_settings_get_exit_index(settings);

    /* Set exit behavior item index and string */
    variable_item_set_current_value_index(item, exit_index);
    variable_item_set_current_value_text(item, exit_str);

    return context;
}

static void xremote_settings_context_clear_callback(void* context) {
    XRemoteSettingsContext* ctx = (XRemoteSettingsContext*)context;
    ViewDispatcher* view_disp = ctx->app_ctx->view_dispatcher;
    view_dispatcher_remove_view(view_disp, XRemoteViewSettings);
    variable_item_list_free(ctx->item_list);
}

XRemoteApp* xremote_settings_alloc(XRemoteAppContext* app_ctx) {
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    XRemoteSettingsContext* context = xremote_settings_context_alloc(app_ctx);
    xremote_app_set_user_context(app, context, xremote_settings_context_clear_callback);
    return app;
}
