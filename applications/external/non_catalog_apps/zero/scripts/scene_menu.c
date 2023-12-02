#include "app.h"
#include "game.h"
#include "app_gameplay.h"
#include "racso_zero_icons.h"
#include "wave/scene_management.h"
#include <furi.h>
#include <gui/gui.h>

void menu_render_callback(Canvas* const canvas, void* context) {
    AppContext* app = (AppContext*)context;
    UNUSED(app);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontPrimary);
    //canvas_draw_str_aligned(canvas, 31, 6, AlignCenter, AlignTop, "ÚLTIMA");
    const int ZERO_WIDTH = 66, ZERO_HEIGHT = 14;
    canvas_draw_icon(canvas, (127 - ZERO_WIDTH) / 2 + 1, 0, &I_zero);

    // Version
    int versionX = (127 + ZERO_WIDTH) / 2 - 12, versionY = ZERO_HEIGHT + 5;
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

    // draw card icons
    const int CARD_ICON_WIDTH = 9, SPACING = 2, ICONS_Y = ZERO_HEIGHT + 10;
    const int ICONS_X = (127 - (4 * CARD_ICON_WIDTH + 3 * SPACING)) / 2;
    canvas_draw_icon(canvas, ICONS_X, ICONS_Y, &I_s1);
    canvas_draw_icon(canvas, ICONS_X + CARD_ICON_WIDTH + SPACING, ICONS_Y, &I_s2);
    canvas_draw_icon(canvas, ICONS_X + 2 * (CARD_ICON_WIDTH + SPACING), ICONS_Y, &I_s3);
    canvas_draw_icon(canvas, ICONS_X + 3 * (CARD_ICON_WIDTH + SPACING), ICONS_Y, &I_s4);

    for(int x = 0, y = 58, dx = 0; dx <= 3; dx++)
        canvas_draw_line(canvas, x + dx, y - dx, x + dx, y + dx);
    canvas_draw_icon(canvas, 6, 55, &I_question_mark);

    const int START_CENTER_X = 48, START_CENTER_Y = 59;
    canvas_draw_circle(canvas, START_CENTER_X, START_CENTER_Y, 4);
    canvas_draw_disc(canvas, START_CENTER_X, START_CENTER_Y, 2);
    canvas_draw_str_aligned(
        canvas, START_CENTER_X + 8, START_CENTER_Y + 4, AlignLeft, AlignBottom, "Start");
}

void menu_input_callback(InputKey key, InputType type, void* context) {
    AppContext* app = (AppContext*)context;

    if(key == InputKeyBack && type == InputTypePress)
        scene_manager_set_scene(app->sceneManager, SceneType_None);
    else if(key == InputKeyOk && type == InputTypePress)
        scene_manager_set_scene(app->sceneManager, SceneType_Game);
    else if(key == InputKeyLeft && type == InputTypePress)
        scene_manager_set_scene(app->sceneManager, SceneType_Credits);
}
