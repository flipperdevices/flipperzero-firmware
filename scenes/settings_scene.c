#include "../minesweeper.h"

typedef enum {
    MineSweeperSettingsScreenDifficultyTypeEasy,
    MineSweeperSettingsScreenDifficultyTypeMedium,
    MineSweeperSettingsScreenDifficultyTypeHard,
    MineSweeperSettingsScreenDifficultyTypeVeryHard,
    MineSweeperSettingsScreenDifficultyTypeImpossible,
    MineSweeperSettingsScreenDifficultyTypeNum,
} MineSweeperSettingsScreenDifficultyType;

typedef enum {
    MineSweeperSettingsScreenIndexDifficulty,
    MineSweeperSettingsScreenIndexWidth,
    MineSweeperSettingsScreenIndexHeight,
    MineSweeperSettingsScreenIndexNum,
} MineSweeperSettingsScreenIndex;

static char* settings_screen_difficulty_text[MineSweeperSettingsScreenDifficultyTypeNum] = {
    "Child",
    "Boy",
    "Man",
    "God",
    "???",
};

static void minesweeper_scene_settings_screen_set_difficulty(VariableItem* item) {
    furi_assert(item);
    UNUSED(item);

    MineSweeperApp* app = variable_item_get_context(item);

    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, settings_screen_difficulty_text[index]);

    view_dispatcher_send_custom_event(app->view_dispatcher, index);

    app->settings_info.difficulty = index;
}

void minesweeper_scene_settings_screen_on_enter(void* context) {
    furi_assert(context);

    MineSweeperApp* app = (MineSweeperApp*)context;
    VariableItemList* va = app->settings_screen;
    VariableItem* item;

    item = variable_item_list_add(
        va,
        "Difficulty",
        MineSweeperSettingsScreenDifficultyTypeNum,
        minesweeper_scene_settings_screen_set_difficulty,
        app);

    variable_item_set_current_value_index(
            item,
            app->settings_info.difficulty);

    variable_item_set_current_value_text(
            item,
            settings_screen_difficulty_text[app->settings_info.difficulty]);

    
    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperSettingsView);
}

bool minesweeper_scene_settings_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    MineSweeperApp* app = context; 
    bool consumed = false;
    
    if (event.type == SceneManagerEventTypeCustom) {

        consumed = true;

    } else if (event.type == SceneManagerEventTypeBack) {

        if (!scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, MineSweeperSceneMenuScreen)) {

            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);

        }

        consumed = true;
    }

    UNUSED(event);
    UNUSED(app);
    return consumed;
}

void minesweeper_scene_settings_screen_on_exit(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;
    VariableItemList* va = app->settings_screen;

    variable_item_list_reset(va);
}
