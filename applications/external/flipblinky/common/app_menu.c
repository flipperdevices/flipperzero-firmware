#include "app_menu_i.h"

static ViewDispatcher* global_view_dispatcher;

/**
 * @brief      Callback for navigation events, used to exit application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      _context  Unused parameter.
 * @return     VIEW_NONE to exit application.
*/
static uint32_t navigation_exit(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}

/**
 * @brief      Callback for submenu view, used to return to main menu.
 * @details    This function is called when user press back button.  We return
 *            FLIPBOARD_APP_MENU_VIEW_ID to indicate that we want to return to main menu.
 * @param      _context  Unused parameter.
 * @return     FLIPBOARD_APP_MENU_VIEW_ID to return to main menu.
*/
static uint32_t submenu_view_id(void* _context) {
    UNUSED(_context);
    return FLIPBOARD_APP_MENU_VIEW_ID;
}

/**
 * @brief      Callback for submenu view, used to switch to selected view.
 * @details    This function is called when user select an item in the submenu.  We use
 *           view_dispatcher_switch_to_view to switch to the selected view.
 * @param      context  Pointer to AppMenu structure.
 * @param      index    Index of the selected item in the submenu
*/
static void app_menu_callback(void* context, uint32_t index) {
    AppMenu* app_menu = (AppMenu*)context;
    if(index < ViewIdsArray_size(app_menu->view_ids)) {
        view_dispatcher_switch_to_view(
            app_menu->view_dispatcher, *ViewIdsArray_get(app_menu->view_ids, index));
    }
}

/**
 * @brief      Callback for when submenu view is displayed.
 * @details    This function is called when submenu view is displayed.  We use
 *          view_dispatcher_send_custom_event to send CustomEventAppMenuEnter event.
 * @param      _context  Unused parameter. 
*/
static void app_menu_enter_callback(void* _context) {
    // context is Submenu, which cannot access the global_view_dispatcher.
    UNUSED(_context);
    view_dispatcher_send_custom_event(global_view_dispatcher, CustomEventAppMenuEnter);
}

/**
 * @brief      Callback for when submenu view is exited.
 * @details    This function is called when submenu view is exited.  We use
 *         view_dispatcher_send_custom_event to send CustomEventAppMenuExit event.
 * @param      _context  Unused parameter. 
*/
static void app_menu_exit_callback(void* _context) {
    // context is Submenu, which cannot access the global_view_dispatcher.
    UNUSED(_context);
    view_dispatcher_send_custom_event(global_view_dispatcher, CustomEventAppMenuExit);
}

/**
 * @brief      Allocate and initialize AppMenu structure.
 * @details    This function allocate and initialize AppMenu structure.  It also allocate
 *           and initialize submenu structure.
 * @param      view_dispatcher  Pointer to ViewDispatcher structure.
 * @return     Pointer to allocated AppMenu structure.
*/
AppMenu* app_menu_alloc(ViewDispatcher* view_dispatcher) {
    AppMenu* menu = (AppMenu*)malloc(sizeof(AppMenu));
    menu->view_dispatcher = view_dispatcher;
    global_view_dispatcher = view_dispatcher;
    menu->submenu = submenu_alloc();
    ViewIdsArray_init(menu->view_ids);
    view_set_enter_callback(submenu_get_view(menu->submenu), app_menu_enter_callback);
    view_set_exit_callback(submenu_get_view(menu->submenu), app_menu_exit_callback);
    view_set_previous_callback(submenu_get_view(menu->submenu), navigation_exit);
    view_dispatcher_add_view(
        menu->view_dispatcher, FLIPBOARD_APP_MENU_VIEW_ID, submenu_get_view(menu->submenu));
    return menu;
}

/**
 * @brief      Free AppMenu structure.
 * @details    This function free AppMenu structure.  It also free submenu structure.
 * @param      menu  Pointer to AppMenu structure.
*/
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

/**
 * @brief      Get view of AppMenu structure.
 * @details    This function return view of AppMenu structure.  It is used to add AppMenu
 *          view to ViewDispatcher.
 * @param      menu  Pointer to AppMenu structure.
 * @return     Pointer to view of AppMenu structure.
*/
View* app_menu_get_view(AppMenu* menu) {
    return submenu_get_view(menu->submenu);
}

/**
 * @brief      Add item to AppMenu structure.
 * @details    This function add item to AppMenu structure.  It is used to add items to
 *         submenu.
 * @param      menu     Pointer to AppMenu structure.
 * @param      name     Name of the item.
 * @param      view     Pointer to view structure.
 * @param      view_id  View id to associate with the view.
 */
void app_menu_add_item(AppMenu* menu, char* name, View* view, uint32_t view_id) {
    view_set_previous_callback(view, submenu_view_id);
    view_dispatcher_add_view(menu->view_dispatcher, view_id, view);
    submenu_add_item(
        menu->submenu, name, ViewIdsArray_size(menu->view_ids), app_menu_callback, menu);
    ViewIdsArray_push_back(menu->view_ids, view_id);
}

/**
 * @brief      Show AppMenu structure.
 * @details    This function show AppMenu structure.  It is used to show AppMenu view.
 * @param      menu  Pointer to AppMenu structure.
*/
void app_menu_show(AppMenu* menu) {
    view_dispatcher_switch_to_view(menu->view_dispatcher, FLIPBOARD_APP_MENU_VIEW_ID);
}
