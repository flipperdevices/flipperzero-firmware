#include "../minesweeper.h"
static const char* info_string = "GAME INFO BELOW\n\n"
                                 "1. Press OK to clear a tile.\n\n"
                                 "2. Hold OK on a numbered tile\n"
                                 "to clear all surrounding\n"
                                 "tiles if the correct amount\n"
                                 "of flags are set.\n\n"
                                 "3. Hold Back on a tile to\n"
                                 "mark it with a flag.\n\n"
                                 "4. Hold back on a cleared\n"
                                 "tile to jump to the\n"
                                 "closest tile.\n\n"
                                 "SETTINGS INFO\n\n"
                                 "Difficulty and map\n"
                                 "dimensions can be changed\n"
                                 "in the settings with a\n"
                                 "max map size of 1024\n"
                                 "tiles (32x32).\n\n"
                                 "ENSURE SOLVABLE\n"
                                 "This is a setting that\n"
                                 "enables a board verifier\n"
                                 "when generating a new\n"
                                 "board.\n\n"
                                 "-- WARNING --\n"
                                 "This setting will introduce\n"
                                 "a variable amount of\n"
                                 "overhead when generating\n"
                                 "a new map. It can take\n"
                                 "several seconds for a\n"
                                 "valid map to generate. The\n"
                                 "UI may hang and stop for a\n"
                                 "while but it should resolve\n"
                                 "in a few seconds.\n\n"
                                 "Enjoy the game and if you\n"
                                 "want to reach out about an\n"
                                 "issue go to the git hub repo\n"
                                 "for this app:\n\n"
                                 "'github.com/squee72564/\n"
                                 "F0_Minesweeper_Fap'\n\n"
                                 "Thanks and enjoy!";

void minesweeper_scene_info_screen_on_enter(void* context) {
    furi_assert(context);

    MineSweeperApp* app = (MineSweeperApp*)context;

    text_box_set_text(app->info_screen, info_string);
    text_box_set_font(app->info_screen, TextBoxFontText);
    text_box_set_focus(app->info_screen, TextBoxFocusStart);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperInfoView);
}

bool minesweeper_scene_info_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    MineSweeperApp* app = context;
    UNUSED(event);
    UNUSED(app);

    bool consumed = false;

    return consumed;
}

void minesweeper_scene_info_screen_on_exit(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;

    text_box_reset(app->info_screen);
}
