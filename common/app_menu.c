#include <furi.h>
#include "app_menu_i.h"

/**
 * @brief      Callback for navigation events, used to exit application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
static uint32_t navigation_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static uint32_t submenu_view_id(void* context) {
    UNUSED(context);
    return FLIPBOARD_APP_MENU_VIEW_ID;
}

static void app_menu_callback(void* context, uint32_t index) {
    AppMenu* app_menu = (AppMenu*)context;
    if(index < ViewIdsArray_size(app_menu->view_ids)) {
        view_dispatcher_switch_to_view(
            app_menu->view_dispatcher, *ViewIdsArray_get(app_menu->view_ids, index));
    }
}

AppMenu* app_menu_alloc(ViewDispatcher* view_dispatcher) {
    AppMenu* menu = (AppMenu*)malloc(sizeof(AppMenu));
    menu->view_dispatcher = view_dispatcher;
    menu->submenu = submenu_alloc();
    ViewIdsArray_init(menu->view_ids);
    view_set_previous_callback(submenu_get_view(menu->submenu), navigation_exit);
    view_dispatcher_add_view(
        menu->view_dispatcher, FLIPBOARD_APP_MENU_VIEW_ID, submenu_get_view(menu->submenu));
    return menu;
}

void app_menu_free(AppMenu* menu) {
    while(ViewIdsArray_size(menu->view_ids) > 0) {
        uint32_t idx = 0;
        ViewIdsArray_pop_back(&idx, menu->view_ids);
        view_dispatcher_remove_view(menu->view_dispatcher, idx);
    }
    ViewIdsArray_reset(menu->view_ids);
    ViewIdsArray_clear(menu->view_ids);
    view_dispatcher_remove_view(menu->view_dispatcher, FLIPBOARD_APP_MENU_VIEW_ID);

    if(menu->submenu) {
        submenu_free(menu->submenu);
    }

    free(menu);
}

View* app_menu_get_view(AppMenu* menu) {
    return submenu_get_view(menu->submenu);
}

void app_menu_add_item(AppMenu* menu, char* name, View* view, uint32_t view_id) {
    view_set_previous_callback(view, submenu_view_id);
    view_dispatcher_add_view(menu->view_dispatcher, view_id, view);
    submenu_add_item(
        menu->submenu, name, ViewIdsArray_size(menu->view_ids), app_menu_callback, menu);
    ViewIdsArray_push_back(menu->view_ids, view_id);
}

void app_menu_show(AppMenu* menu) {
    view_dispatcher_switch_to_view(menu->view_dispatcher, FLIPBOARD_APP_MENU_VIEW_ID);
}
