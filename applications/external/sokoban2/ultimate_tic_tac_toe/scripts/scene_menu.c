#include "app.h"
#include "game.h"
#include "app_gameplay.h"
#include "racso_ultimate_tic_tac_toe_icons.h"
#include "scene_management.h"
#include <furi.h>
#include <gui/gui.h>

void menu_render_callback(Canvas* const canvas, void* context) {
    AppContext* app = (AppContext*)context;
    AppGameplayState* game = app->gameplay;
    static const char* playerTypeStrings[PlayerType_COUNT] = {
        "Player", "COM I", "COM II", "COM III", "COM IV", "COM V"};

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    // Left side: Title
    int titleX = 30;
    int titleY = 6;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, titleX, titleY, AlignCenter, AlignTop, "ULTIMATE");
    canvas_draw_str_aligned(canvas, 0, titleY + 16, AlignLeft, AlignTop, "TIC");
    canvas_draw_str_aligned(canvas, titleX, titleY + 28, AlignCenter, AlignTop, "TAC");
    canvas_draw_str_aligned(canvas, titleX * 2, titleY + 40, AlignRight, AlignTop, "TOE");

    // Version
    int versionX = 48, versionY = 61;
    canvas_draw_line(canvas, versionX, versionY - 2, versionX, versionY - 1);
    canvas_draw_line(canvas, versionX + 2, versionY - 2, versionX + 2, versionY - 1);
    canvas_draw_dot(canvas, versionX + 1, versionY);

    versionX += 5;
    canvas_draw_line(canvas, versionX, versionY - 3, versionX, versionY);

    versionX += 2;
    canvas_draw_dot(canvas, versionX, versionY);

    versionX += 2;
    canvas_draw_line(canvas, versionX, versionY - 2, versionX, versionY - 1);
    canvas_draw_line(canvas, versionX + 2, versionY - 2, versionX + 2, versionY - 1);
    canvas_draw_dot(canvas, versionX + 1, versionY);
    canvas_draw_dot(canvas, versionX + 1, versionY - 3);

    for(int x = 0, y = 58, dx = 0; dx <= 3; dx++)
        canvas_draw_line(canvas, x + dx, y - dx, x + dx, y + dx);
    canvas_draw_icon(canvas, 6, 55, &I_question_mark);

    // Center Divider
    int centerX = 64;
    for(int symbolCount = 0, currentY = 1; currentY < 63; symbolCount++, currentY += 4) {
        int symbol = symbolCount % 2;

        if(symbolCount == 7) {
            // Easter Egg, by Racso.
            canvas_draw_dot(canvas, centerX, currentY - 1);
            canvas_draw_dot(canvas, centerX, currentY);
            canvas_draw_dot(canvas, centerX, currentY + 1);
            canvas_draw_dot(canvas, centerX - 1, currentY);
            canvas_draw_dot(canvas, centerX + 1, currentY - 1);
        } else if(symbol == 0) {
            canvas_draw_circle(canvas, centerX, currentY, 1);
        } else if(symbol == 1) {
            canvas_draw_line(canvas, centerX - 1, currentY - 1, centerX + 1, currentY + 1);
            canvas_draw_line(canvas, centerX - 1, currentY + 1, centerX + 1, currentY - 1);
        }
    }

    // Right side: Player Selection
    int elementsX = 96, triangleY;

    triangleY = 2;
    for(int dy = -1, h = 0; h <= 2; dy++, h++)
        canvas_draw_line(canvas, elementsX - h, triangleY + dy, elementsX + h, triangleY + dy);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        elementsX,
        10,
        AlignCenter,
        AlignCenter,
        playerTypeStrings[gameplay_get_player_type(game, 0)]);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, elementsX, 24, AlignCenter, AlignCenter, "VS");

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        elementsX,
        36,
        AlignCenter,
        AlignCenter,
        playerTypeStrings[gameplay_get_player_type(game, 1)]);

    triangleY = 44;
    for(int dy = 1, h = 0; h <= 2; dy--, h++)
        canvas_draw_line(canvas, elementsX - h, triangleY + dy, elementsX + h, triangleY + dy);

    canvas_draw_circle(canvas, 82, 59, 4);
    canvas_draw_disc(canvas, 82, 59, 2);
    canvas_draw_str_aligned(canvas, 90, 63, AlignLeft, AlignBottom, "Start");
}

void menu_set_next_player_type(AppGameplayState* gameplay, PlayerTurn player) {
    PlayerType nextType =
        (PlayerType)((gameplay_get_player_type(gameplay, player) + 1) % PlayerType_COUNT);
    gameplay_set_player_type(gameplay, player, nextType);
}

void menu_input_callback(InputKey key, InputType type, void* context) {
    AppContext* app = (AppContext*)context;

    if(key == InputKeyBack && type == InputTypePress)
        scene_manager_set_scene(app->sceneManager, SceneType_None);
    else if(key == InputKeyOk && type == InputTypePress)
        scene_manager_set_scene(app->sceneManager, SceneType_Game);
    else if(key == InputKeyLeft && type == InputTypePress)
        scene_manager_set_scene(app->sceneManager, SceneType_Credits);
    else if(key == InputKeyUp && type == InputTypePress)
        menu_set_next_player_type(app->gameplay, PlayerTurn_X);
    else if(key == InputKeyDown && type == InputTypePress)
        menu_set_next_player_type(app->gameplay, PlayerTurn_O);
}
