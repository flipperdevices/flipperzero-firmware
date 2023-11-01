#pragma once

#include "../helpers/shapshup_files.h"
#include "../shapshup_custom_event.h"
#include <gui/view.h>
#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>

/**
 * @brief ShapShupMainViewCallback
 * 
 */
typedef void (*ShapShupMainViewCallback)(ShapshupCustomEvent event, void* context);

/**
 * @brief ShapShupMainView
 * 
 */
typedef struct ShapShupMainView ShapShupMainView;

/**
 * @brief shapshup_main_view_set_callback
 * 
 * @param instance ShapShupMainView*
 * @param callback 
 * @param context 
 */
void shapshup_main_view_set_callback(
    ShapShupMainView* instance,
    ShapShupMainViewCallback callback,
    void* context);

/**
 * @brief shapshup_main_view_alloc
 * 
 * @return ShapShupMainView* 
 */
ShapShupMainView* shapshup_main_view_alloc();

/**
 * @brief shapshup_main_view_free
 * 
 * @param instance 
 */
void shapshup_main_view_free(ShapShupMainView* instance);

/**
 * @brief shapshup_main_view_get_view
 * 
 * @param instance ShapShupMainView*
 * @return View* 
 */
View* shapshup_main_view_get_view(ShapShupMainView* instance);

/**
 * @brief shapshup_main_view_check_alert
 * 
 * @param instance 
 */
void shapshup_main_view_check_alert(ShapShupMainView* instance);

/**
 * @brief shapshup_main_view_load_file
 * 
 * @param instance ShapShupMainView*
 * @param name const char*
 * @return ShapShupFileResults 
 */
ShapShupFileResults shapshup_main_view_load_file(ShapShupMainView* instance, const char* name);

/**
 * @brief shapshup_main_view_no_file
 * 
 * @param instance ShapShupMainView*
 * @return true 
 * @return false 
 */
bool shapshup_main_view_no_file(ShapShupMainView* instance);
