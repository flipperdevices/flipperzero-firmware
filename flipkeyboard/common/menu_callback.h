#pragma once

/**
 * @file menu_callback.h
 * @brief This file contains a method to get a ViewNavigationCallback
 * that returns a specific view id.  
 * @details The menu callback module is used to return a specific view id
 * from ViewNavigationCallback. The id is matched by a large switch so it
 * can only handle a limited number of values (if to large of id is requested, 
 * the 0 view will be returned).
*/

#include <furi.h>
#include <gui/view.h>

/**
 * @brief Callback for menu navigation
 * @param return_index The value the callback should return.
 * @return The callback to be used for menu navigation.
 */
ViewNavigationCallback get_menu_callback(uint32_t return_index);