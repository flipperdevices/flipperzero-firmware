#include "basic_scenes_split.h"
#include "main_menu.h"

typedef enum {
    BasicScenesMainMenuSceneLottoNumbersEvent,
    BasicScenesMainMenuSceneGreetingEvent,
} BasicScenesMainMenuEvent;

static void basic_scenes_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case BasicScenesMainMenuSceneLottoNumbers:
        scene_manager_handle_custom_event(
            app->scene_manager, BasicScenesMainMenuSceneLottoNumbersEvent);
        break;
    case BasicScenesMainMenuSceneGreeting:
        scene_manager_handle_custom_event(
            app->scene_manager, BasicScenesMainMenuSceneGreetingEvent);
        break;
    }
}

void basic_scenes_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Basic Scenes Demo");
    submenu_add_item(
        app->submenu,
        "Lotto Numbers",
        BasicScenesMainMenuSceneLottoNumbers,
        basic_scenes_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Greeting",
        BasicScenesMainMenuSceneGreeting,
        basic_scenes_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesSubmenuView);
}

bool basic_scenes_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case BasicScenesMainMenuSceneLottoNumbersEvent:
            scene_manager_next_scene(app->scene_manager, BasicScenesLotteryScene);
            consumed = true;
            break;
        case BasicScenesMainMenuSceneGreetingEvent:
            scene_manager_next_scene(app->scene_manager, BasicScenesGreetingInputScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void basic_scenes_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}