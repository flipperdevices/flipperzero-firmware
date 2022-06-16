#include "../desktop_settings_app.h"
#include "applications.h"
#include "desktop_settings_scene.h"

static void desktop_settings_scene_favorite_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void add_favorite_submenu_item(DesktopSettingsApp* app, Submenu* submenu) {
    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS[i].name,
            i,
            desktop_settings_scene_favorite_submenu_callback,
            app);
    }
}

void add_game_submenu_item(DesktopSettingsApp* app, Submenu* submenu) {
    for(size_t i = 0; i < FLIPPER_GAMES_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_GAMES[i].name,
            i,
            desktop_settings_scene_favorite_submenu_callback,
            app);
    }
}

void desktop_settings_scene_favorite_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    uint32_t favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);

    switch(favorite) {
    case 0:
        add_favorite_submenu_item(app, submenu);
        submenu_set_header(app->submenu, "Primary favorite app:");
        submenu_set_selected_item(app->submenu, app->settings.favorite_primary);
        break;
    case 1:
        add_favorite_submenu_item(app, submenu);
        submenu_set_header(app->submenu, "Secondary favorite app:");
        submenu_set_selected_item(app->submenu, app->settings.favorite_secondary);
        break;
    case 2:
        add_game_submenu_item(app, submenu);
        submenu_set_header(app->submenu, "Favorite game:");
        submenu_set_selected_item(app->submenu, app->settings.favorite_game);
        break;
    default:
        break;
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_favorite_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    uint32_t favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);

    if(event.type == SceneManagerEventTypeCustom) {
        switch(favorite) {
        case 0:
            app->settings.favorite_primary = event.event;
            break;
        case 1:
            app->settings.favorite_secondary = event.event;
            break;
        case 2:
            app->settings.favorite_game = event.event;
            break;
        default:
            break;
        }
    }

    scene_manager_previous_scene(app->scene_manager);
    consumed = true;
    return consumed;
}

void desktop_settings_scene_favorite_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    SAVE_DESKTOP_SETTINGS(&app->settings);
    submenu_reset(app->submenu);
}
