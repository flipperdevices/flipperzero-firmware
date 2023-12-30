#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view_stack.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/loading.h>
#include <gui/modules/variable_item_list.h>

#include "subghz_bruteforcer_icons.h"

#include <assets_icons.h>

#include <dialogs/dialogs.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "subbrute.h"
#include "subbrute_device.h"
#include "subbrute_settings.h"
#include "helpers/subbrute_worker.h"
#include "views/subbrute_attack_view.h"
#include "views/subbrute_main_view.h"

#define SUBBRUTEFORCER_VER "Sub-GHz BruteForcer 3.A"

#ifdef FURI_DEBUG
//#define SUBBRUTE_FAST_TRACK false
#endif

/**
 * @enum SubBruteView
 * @brief An enumeration representing the different views of the SubBrute application
 *
 * This enumeration represents the possible views that the SubBrute application can have. Each view
 * corresponds to a specific screen or UI element in the application.
 */
typedef enum {
    SubBruteViewNone,
    SubBruteViewMain,
    SubBruteViewAttack,
    SubBruteViewTextInput,
    SubBruteViewDialogEx,
    SubBruteViewPopup,
    SubBruteViewWidget,
    SubBruteViewStack,
    SubBruteViewVarList,
} SubBruteView;

/**
 * @class SubBruteState
 * @brief Represents the state of a SubBrute application.
 *
 * This class contains the various elements and variables necessary for the functioning of a SubBrute application.
 */
struct SubBruteState {
    // GUI elements
    NotificationApp* notifications;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewStack* view_stack;
    TextInput* text_input;
    Popup* popup;
    Widget* widget;
    VariableItemList* var_list;
    DialogsApp* dialogs;
    const SubGhzDevice* radio_device;

    // Text store
    char text_store[SUBBRUTE_MAX_LEN_NAME];
    FuriString* file_path;

    // Views
    SubBruteMainView* view_main;
    SubBruteAttackView* view_attack;
    SubBruteView current_view;

    // Scene
    SceneManager* scene_manager;

    // SubBruteDevice
    SubBruteDevice* device;
    // SubBruteWorker
    SubBruteWorker* worker;
    // Last used settings
    SubBruteSettings* settings;
};

/**
 * @brief Function to show or hide a loading popup.
 *
 * This function is used to display or hide a loading popup in a user interface.
 * The loading popup appears when an action is being performed that may take some time to complete.
 *
 * @param context A pointer to the context object associated with the loading popup.
 * @param show A boolean value indicating whether to display or hide the loading popup.
 */
void subbrute_show_loading_popup(void* context, bool show);

/**
 * @brief Callback function for text input in the sub-brute module.
 *
 * This function serves as a callback for handling text input in the sub-brute module.
 * It should be registered as a callback function for text input events.
 *
 * @param context A pointer to additional context data (if any).
 *
 * @return None
 */
void subbrute_text_input_callback(void* context);

/**
 * @brief Callback function called when a popup window is closed.
 *
 * This function is called when a popup window is closed. It can be used to perform
 * any necessary cleanup or additional processing after the popup window is closed.
 *
 * @param context A void pointer to the context data associated with the popup window.
 *                It can be used to pass any additional information needed by the callback function.
 *
 * @return void
 */
void subbrute_popup_closed_callback(void* context);
