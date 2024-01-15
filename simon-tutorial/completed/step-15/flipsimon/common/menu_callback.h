/**
 * @file menu_callback.h
 * @brief This file contains a method to get a ViewNavigationCallback
 * that returns a specific view id.  
 * @details The menu callback module is used to return a specific view id
 * from ViewNavigationCallback.
*/

#pragma once

#include <furi.h>
#include <gui/view.h>

/**
 * @brief Callback for menu navigation
 * @note if return_index is out of range, it will return a callback that
 * returns 0.
 * @param return_index The value the callback should return (1-14).
 * @return The callback to be used for menu navigation.
 */
ViewNavigationCallback get_menu_callback(uint32_t return_index);