#include <gui/modules/menu.h>
#include <gui/modules/popup.h>

#include "settings_scene.h"
#include "../app_context.h"
#include "../tone_gen.h"
#include "../utils/linked_list.h"

/** indices for menu items */
typedef enum {
    SettingsMenuOptions_WaveType,
    SettingsMenuOptions_Amplitude,
    SettingsMenuOptions_Period,
} SettingsMenuOptions;

/** main menu callback - sends a custom event to the scene manager based on the menu selection */
void menu_callback_settings_scene(void* context, uint32_t index) {
    UNUSED(context);
    // struct AppContext_t* app = context;
    switch(index) {
    case SettingsMenuOptions_WaveType:
        FURI_LOG_I(TAG, "selection one");
        // scene_manager_handle_custom_event(app->scene_manager, ToneGenAppEvent_StartPlayback);
        break;
    case SettingsMenuOptions_Amplitude:
        FURI_LOG_I(TAG, "selection two");
        // scene_manager_handle_custom_event(app->scene_manager, ToneGenAppEvent_AdjustTone);
        break;
    case SettingsMenuOptions_Period:
        FURI_LOG_I(TAG, "selection three");
        // scene_manager_handle_custom_event(app->scene_manager, ToneGenAppEvent_AdjustTone);
        break;
    }
}

/** resets the menu, gives it content, callbacks and selection enums */
void scene_on_enter_settings_scene(void* context) {
    FURI_LOG_I(TAG, "scene_on_enter_settings_scene");
    struct AppContext_t* app = (struct AppContext_t*)context;

    // Setup our menu
    FURI_LOG_D(TAG, "Adding view menu");
    struct View_t* menuView = app->activeViews[ToneGenAppView_SharedMenu];

    // Set the currently active view
    menu_reset(menuView->viewData);

    FURI_LOG_D(TAG, "Adding menu options for settings");
    menu_add_item(
        menuView->viewData,
        "Wave Type",
        NULL,
        SettingsMenuOptions_WaveType,
        menu_callback_settings_scene,
        app);
    menu_add_item(
        menuView->viewData,
        "Amplitude",
        NULL,
        SettingsMenuOptions_Amplitude,
        menu_callback_settings_scene,
        app);
    menu_add_item(
        menuView->viewData,
        "Period",
        NULL,
        SettingsMenuOptions_Period,
        menu_callback_settings_scene,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ToneGenAppView_SharedMenu);
}

/** main menu event handler - switches scene based on the event */
bool scene_on_event_settings_scene(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "scene_on_event_settings_scene");
    UNUSED(context);
    // struct AppContext_t* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        // switch(event.event) {
        // case ToneGenAppEvent_StartPlayback:
        //     scene_manager_next_scene(app->scene_manager, ToneGenAppScene_Playback);
        //     consumed = true;
        //     break;
        // case ToneGenAppEvent_AdjustTone:
        //     scene_manager_next_scene(app->scene_manager, ToneGenAppScene_AdjustTone);
        //     consumed = true;
        //     break;
        // }
        break;
    default: // eg. SceneManagerEventTypeBack, SceneManagerEventTypeTick
        consumed = false;
        break;
    }
    return consumed;
}

void scene_on_exit_settings_scene(void* context) {
    FURI_LOG_I(TAG, "scene_on_exit_settings_scene");
    UNUSED(context);
}
