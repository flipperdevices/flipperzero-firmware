#include "../minesweeper.h"
#include "../views/minesweeper_game_screen.h"

typedef enum {
    MineSweeperSettingsScreenDifficultyTypeEasy,
    MineSweeperSettingsScreenDifficultyTypeMedium,
    MineSweeperSettingsScreenDifficultyTypeHard,
    MineSweeperSettingsScreenDifficultyTypeNum,
} MineSweeperSettingsScreenDifficultyType;

typedef enum {
    MineSweeperSettingsScreenIndexDifficulty,
    MineSweeperSettingsScreenIndexWidth,
    MineSweeperSettingsScreenIndexHeight,
    MineSweeperSettingsScreenIndexNum,
} MineSweeperSettingsScreenIndex;

typedef enum {
    MineSweeperSettingsScreenEventDifficultyChange,
    MineSweeperSettingsScreenEventWidthChange,
    MineSweeperSettingsScreenEventHeightChange,
    MineSweeperSettingsScreenEventSolvableChange,
    MineSweeperSettingsScreenEventInfoChange,
} MineSweeperSettingsScreenEvent;

static const char* settings_screen_difficulty_text[MineSweeperSettingsScreenDifficultyTypeNum] = {
    "Easy",
    "Medium",
    "Hard",
};

static const char* settings_screen_verifier_text[2] = {
    "False",
    "True",
};

static void minesweeper_scene_settings_screen_set_difficulty(VariableItem* item) {
    furi_assert(item);

    MineSweeperApp* app = variable_item_get_context(item);
    app->t_settings_info.difficulty_item = item;

    uint8_t index = variable_item_get_current_value_index(app->t_settings_info.difficulty_item);

    app->t_settings_info.difficulty = index;

    variable_item_set_current_value_text(
        app->t_settings_info.difficulty_item, settings_screen_difficulty_text[index]);

    view_dispatcher_send_custom_event(
        app->view_dispatcher, MineSweeperSettingsScreenEventDifficultyChange);
}

static void minesweeper_scene_settings_screen_set_width(VariableItem* item) {
    char source[5];
    uint8_t index = 0;
    uint8_t curr_board_height = 7;
    bool is_over_max_tiles = false;

    furi_assert(item);

    MineSweeperApp* app = variable_item_get_context(item);
    app->t_settings_info.width_item = item;

    index = variable_item_get_current_value_index(app->t_settings_info.width_item);
    app->t_settings_info.board_width = index + 16;

    curr_board_height = app->t_settings_info.board_height;

    is_over_max_tiles = (app->t_settings_info.board_width * curr_board_height) >
                        MINESWEEPER_BOARD_MAX_TILES;
    if(is_over_max_tiles) {
        do {
            is_over_max_tiles = (app->t_settings_info.board_width * --curr_board_height) >
                                MINESWEEPER_BOARD_MAX_TILES;
        } while(is_over_max_tiles);

        app->t_settings_info.board_height = curr_board_height;

        snprintf(source, 5, "%" PRIu8, curr_board_height);
        source[4] = '\0';

        furi_string_set_strn(app->t_settings_info.height_str, source, 5);

        variable_item_set_current_value_index(
            app->t_settings_info.height_item, app->t_settings_info.board_height - 7);

        variable_item_set_current_value_text(
            app->t_settings_info.height_item,
            furi_string_get_cstr(app->t_settings_info.height_str));
    }

    snprintf(source, 5, "%" PRIu8, index + 16);
    source[4] = '\0';

    furi_string_set_strn(app->t_settings_info.width_str, source, 5);

    variable_item_set_current_value_text(
        app->t_settings_info.width_item, furi_string_get_cstr(app->t_settings_info.width_str));

    view_dispatcher_send_custom_event(
        app->view_dispatcher, MineSweeperSettingsScreenEventHeightChange);
}

static void minesweeper_scene_settings_screen_set_height(VariableItem* item) {
    char source[5];
    uint8_t index = 0;
    uint8_t curr_board_width = 16;
    bool is_over_max_tiles = false;

    furi_assert(item);

    MineSweeperApp* app = variable_item_get_context(item);
    app->t_settings_info.height_item = item;

    index = variable_item_get_current_value_index(app->t_settings_info.height_item);
    app->t_settings_info.board_height = index + 7;

    curr_board_width = app->t_settings_info.board_width;

    is_over_max_tiles = (app->t_settings_info.board_height * curr_board_width) >
                        MINESWEEPER_BOARD_MAX_TILES;
    if(is_over_max_tiles) {
        do {
            is_over_max_tiles = (app->t_settings_info.board_height * --curr_board_width) >
                                MINESWEEPER_BOARD_MAX_TILES;
        } while(is_over_max_tiles);

        app->t_settings_info.board_width = curr_board_width;

        snprintf(source, 5, "%" PRIu8, curr_board_width);
        source[4] = '\0';

        furi_string_set_strn(app->t_settings_info.width_str, source, 5);

        variable_item_set_current_value_index(
            app->t_settings_info.width_item, app->t_settings_info.board_width - 16);

        variable_item_set_current_value_text(
            app->t_settings_info.width_item, furi_string_get_cstr(app->t_settings_info.width_str));
    }

    snprintf(source, 5, "%" PRIu8, index + 7);
    source[4] = '\0';

    furi_string_set_strn(app->t_settings_info.height_str, source, 5);

    variable_item_set_current_value_text(
        app->t_settings_info.height_item, furi_string_get_cstr(app->t_settings_info.height_str));

    view_dispatcher_send_custom_event(
        app->view_dispatcher, MineSweeperSettingsScreenEventWidthChange);
}

static void minesweeper_scene_settings_screen_set_solvable(VariableItem* item) {
    furi_assert(item);

    MineSweeperApp* app = variable_item_get_context(item);

    uint8_t index = variable_item_get_current_value_index(app->t_settings_info.solvable_item);

    app->t_settings_info.ensure_solvable_board = (index == 1) ? true : false;

    variable_item_set_current_value_text(item, settings_screen_verifier_text[index]);

    view_dispatcher_send_custom_event(
        app->view_dispatcher, MineSweeperSettingsScreenEventSolvableChange);
}

static void minesweeper_scene_settings_screen_set_info(VariableItem* item) {
    furi_assert(item);

    MineSweeperApp* app = variable_item_get_context(item);

    view_dispatcher_send_custom_event(
        app->view_dispatcher, MineSweeperSettingsScreenEventInfoChange);
}

void minesweeper_scene_settings_screen_on_enter(void* context) {
    furi_assert(context);

    MineSweeperApp* app = (MineSweeperApp*)context;
    VariableItemList* va = app->settings_screen;
    VariableItem* item;

    // If we are accessing the scene and have not changed the settings
    if(!app->is_settings_changed) {
        // Set temp setting buffer to current state
        app->t_settings_info = app->settings_info;
    }

    // Set Difficulty Item
    item = variable_item_list_add(
        va,
        "Difficulty",
        MineSweeperSettingsScreenDifficultyTypeNum,
        minesweeper_scene_settings_screen_set_difficulty,
        app);

    app->t_settings_info.difficulty_item = item;

    variable_item_set_current_value_index(item, app->t_settings_info.difficulty);

    variable_item_set_current_value_text(
        item, settings_screen_difficulty_text[app->t_settings_info.difficulty]);

    // Set Width Item
    item = variable_item_list_add(
        va, "Board Width", 33 - 16, minesweeper_scene_settings_screen_set_width, app);

    app->t_settings_info.width_item = item;

    variable_item_set_current_value_index(item, app->t_settings_info.board_width - 16);

    char source[5];
    snprintf(source, 5, "%" PRIu8, app->t_settings_info.board_width);
    source[4] = '\0';
    furi_string_set_strn(app->t_settings_info.width_str, source, 5);

    variable_item_set_current_value_text(
        item, furi_string_get_cstr(app->t_settings_info.width_str));

    // Set Height Item
    item = variable_item_list_add(
        va, "Board Height", 33 - 7, minesweeper_scene_settings_screen_set_height, app);

    app->t_settings_info.height_item = item;

    variable_item_set_current_value_index(item, app->t_settings_info.board_height - 7);

    snprintf(source, 5, "%" PRIu8, app->t_settings_info.board_height);
    source[4] = '\0';
    furi_string_set_strn(app->t_settings_info.height_str, source, 5);

    variable_item_set_current_value_text(
        item, furi_string_get_cstr(app->t_settings_info.height_str));

    // Set solvable item
    item = variable_item_list_add(
        va, "Ensure Solvable", 2, minesweeper_scene_settings_screen_set_solvable, app);

    app->t_settings_info.solvable_item = item;

    uint8_t idx = (app->t_settings_info.ensure_solvable_board) ? 1 : 0;

    variable_item_set_current_value_index(item, idx);

    variable_item_set_current_value_text(item, settings_screen_verifier_text[idx]);

    // Set info item
    item = variable_item_list_add(
        va, "Right For Info", 2, minesweeper_scene_settings_screen_set_info, app);

    variable_item_set_current_value_index(item, 0);

    variable_item_set_current_value_text(item, "-------");

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperSettingsView);
}

bool minesweeper_scene_settings_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    MineSweeperApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        app->is_settings_changed =
            (app->settings_info.board_width != app->t_settings_info.board_width ||
             app->settings_info.board_height != app->t_settings_info.board_height ||
             app->settings_info.difficulty != app->t_settings_info.difficulty ||
             app->settings_info.ensure_solvable_board !=
                 app->t_settings_info.ensure_solvable_board);

        switch(event.event) {
        case MineSweeperSettingsScreenEventDifficultyChange:

            break;

        case MineSweeperSettingsScreenEventWidthChange:

            break;

        case MineSweeperSettingsScreenEventHeightChange:

            break;

        case MineSweeperSettingsScreenEventSolvableChange:

            break;

        case MineSweeperSettingsScreenEventInfoChange:

            scene_manager_next_scene(app->scene_manager, MineSweeperSceneInfoScreen);
            break;

        default:
            break;
        };
        consumed = true;

    } else if(event.type == SceneManagerEventTypeBack) {
        // If there are changes in the width, height, or difficulty go to confirmation scren
        if(app->is_settings_changed) {
            scene_manager_next_scene(app->scene_manager, MineSweeperSceneConfirmationScreen);
        } else {
            memset(&app->t_settings_info, 0, sizeof(app->t_settings_info));

            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, MineSweeperSceneMenuScreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
        }

        consumed = true;
    }

    return consumed;
}

void minesweeper_scene_settings_screen_on_exit(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;
    VariableItemList* va = app->settings_screen;

    variable_item_list_reset(va);
}
