/**
 * @file app_menu.h
 * @brief This file contains the AppMenu type and related functions.
 * @details This file contains the AppMenu type and related functions.
 * The app_menu module is used to create and show the main application menu.
 * 
 * FLIPBOARD_APP_MENU_VIEW_ID is used to identify the main application menu view.
 * Exiting this menu (Back button) exits the application.
 * CustomEventAppMenuEnter happens on displaying main application menu.
 * CustomEventAppMenuExit happens on exit.
 */

#pragma once

#include <gui/view.h>
#include <gui/view_dispatcher.h>

// NOTE: FlipboardViewAppMenuId should also be 0.
#define FLIPBOARD_APP_MENU_VIEW_ID 0

typedef struct AppMenu AppMenu;

/**
 * @brief      Allocate and initialize AppMenu structure.
 * @details    This function allocate and initialize AppMenu structure.  It also allocate
 *           and initialize submenu structure.
 * @param      view_dispatcher  Pointer to ViewDispatcher structure.
 * @return     Pointer to allocated AppMenu structure.
*/
AppMenu* app_menu_alloc(ViewDispatcher* view_dispatcher);

/**
 * @brief      Free AppMenu structure.
 * @details    This function free AppMenu structure.  It also free submenu structure.
 * @param      menu  Pointer to AppMenu structure.
*/
void app_menu_free(AppMenu* menu);

/**
 * @brief      Get view of AppMenu structure.
 * @details    This function return view of AppMenu structure.  It is used to add AppMenu
 *          view to ViewDispatcher.
 * @param      menu  Pointer to AppMenu structure.
 * @return     Pointer to view of AppMenu structure.
*/
View* app_menu_get_view(AppMenu* menu);

/**
 * @brief      Add item to AppMenu structure.
 * @details    This function add item to AppMenu structure.  It is used to add items to
 *         submenu.
 * @param      menu     Pointer to AppMenu structure.
 * @param      name     Name of the item.
 * @param      view     Pointer to view structure.
 * @param      view_id  View id to associate with the view.
 */
void app_menu_add_item(AppMenu* menu, char* name, View* view, uint32_t view_id);

/**
 * @brief      Show AppMenu structure.
 * @details    This function show AppMenu structure.  It is used to show AppMenu view.
 * @param      menu  Pointer to AppMenu structure.
*/
void app_menu_show(AppMenu* menu);
