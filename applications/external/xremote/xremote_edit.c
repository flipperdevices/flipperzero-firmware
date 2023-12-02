/*!
 *  @file flipper-xremote/xremote_edit.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Edit menu for XRemote custom layout buttons.
 */

#include "xremote_edit.h"

typedef struct {
    VariableItemList* item_list;
    XRemoteAppButtons* buttons;
} XRemoteEditContext;

static uint32_t xremote_edit_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewIRSubmenu;
}

static void xremote_edit_buttons_store(XRemoteAppButtons* buttons) {
    FuriString* path = buttons->app_ctx->file_path;
    infrared_remote_store(buttons->remote);
    xremote_app_extension_store(buttons, path);
}

static void xremote_item_update_item(VariableItem* item, FuriString* button) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    XRemoteAppButtons* buttons = ctx->buttons;

    int button_index = variable_item_get_current_value_index(item);
    const char* button_name = xremote_button_get_name(button_index);
    variable_item_set_current_value_text(item, button_name);

    furi_string_set_str(button, button_name);
    xremote_edit_buttons_store(buttons);
}

static void xremote_edit_ok_press_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_ok);
}

static void xremote_edit_up_press_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_up);
}

static void xremote_edit_down_press_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_down);
}

static void xremote_edit_left_press_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_left);
}

static void xremote_edit_right_press_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_right);
}

static void xremote_edit_ok_hold_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_ok_hold);
}

static void xremote_edit_up_hold_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_up_hold);
}

static void xremote_edit_down_hold_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_down_hold);
}

static void xremote_edit_left_hold_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_left_hold);
}

static void xremote_edit_right_hold_changed(VariableItem* item) {
    XRemoteEditContext* ctx = variable_item_get_context(item);
    xremote_item_update_item(item, ctx->buttons->custom_right_hold);
}

static void xremote_edit_list_add_item(
    XRemoteEditContext* context,
    const char* item_name,
    FuriString* button,
    VariableItemChangeCallback change_callback) {
    VariableItemList* list = context->item_list;
    VariableItem* item;

    /* Add custom_up to variable item list */
    item = variable_item_list_add(list, item_name, XREMOTE_BUTTON_COUNT, change_callback, context);

    /* Get button name and index */
    const char* button_name = furi_string_get_cstr(button);
    uint32_t button_index = xremote_button_get_index(button_name);

    /* Set button name and index to the list item */
    variable_item_set_current_value_index(item, button_index);
    variable_item_set_current_value_text(item, button_name);
}

static XRemoteEditContext* xremote_edit_context_alloc(XRemoteAppButtons* buttons) {
    XRemoteEditContext* context = malloc(sizeof(XRemoteEditContext));
    context->item_list = variable_item_list_alloc();

    XRemoteAppContext* app_ctx = buttons->app_ctx;
    context->buttons = buttons;

    /* Configure variable item list view */
    View* view = variable_item_list_get_view(context->item_list);
    view_set_previous_callback(view, xremote_edit_view_exit_callback);
    view_dispatcher_add_view(app_ctx->view_dispatcher, XRemoteViewIRCustomEditPage, view);

    /* Add press items to the variable list */
    xremote_edit_list_add_item(
        context, "Ok press", buttons->custom_ok, xremote_edit_ok_press_changed);
    xremote_edit_list_add_item(
        context, "Up press", buttons->custom_up, xremote_edit_up_press_changed);
    xremote_edit_list_add_item(
        context, "Down press", buttons->custom_down, xremote_edit_down_press_changed);
    xremote_edit_list_add_item(
        context, "Left press", buttons->custom_left, xremote_edit_left_press_changed);
    xremote_edit_list_add_item(
        context, "Right press", buttons->custom_right, xremote_edit_right_press_changed);
    xremote_edit_list_add_item(
        context, "Ok hold", buttons->custom_ok_hold, xremote_edit_ok_hold_changed);
    xremote_edit_list_add_item(
        context, "Up hold", buttons->custom_up_hold, xremote_edit_up_hold_changed);
    xremote_edit_list_add_item(
        context, "Down hold", buttons->custom_down_hold, xremote_edit_down_hold_changed);
    xremote_edit_list_add_item(
        context, "Left hold", buttons->custom_left_hold, xremote_edit_left_hold_changed);
    xremote_edit_list_add_item(
        context, "Right hold", buttons->custom_right_hold, xremote_edit_right_hold_changed);

    return context;
}

void xremote_edit_context_clear_callback(void* context) {
    XRemoteEditContext* ctx = (XRemoteEditContext*)context;
    variable_item_list_free(ctx->item_list);
}

void xremote_edit_view_alloc(XRemoteApp* app, uint32_t view_id, XRemoteAppButtons* buttons) {
    xremote_app_view_free(app);
    XRemoteView* remote_view = xremote_view_alloc_empty();
    XRemoteEditContext* context = xremote_edit_context_alloc(buttons);

    xremote_view_set_app_context(remote_view, buttons->app_ctx);
    xremote_view_set_view(remote_view, variable_item_list_get_view(context->item_list));
    xremote_view_set_context(remote_view, context, xremote_edit_context_clear_callback);

    app->view_ctx = remote_view;
    app->view_id = view_id;
}
