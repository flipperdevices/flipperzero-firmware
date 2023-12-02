#include "scene_credits.h"
#include "app.h"
#include "scene_management.h"
#include "racso_ultimate_tic_tac_toe_icons.h"

void credits_render_callback(Canvas* const canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 64 - 10, 1, AlignRight, AlignTop, "Made with");
    canvas_draw_icon(canvas, 64 - 4, 0, &I_ace_spades);
    canvas_draw_str_aligned(canvas, 65 + 10, 1, AlignLeft, AlignTop, "by Racso");

    int boxSize = 10;
    int x = 64 - boxSize * 3 / 2;
    int y = 32 - boxSize * 3 / 2;
    canvas_draw_box(canvas, x + boxSize, y, boxSize, boxSize);
    canvas_draw_box(canvas, x + 2 * boxSize, y, boxSize, boxSize);
    canvas_draw_box(canvas, x + boxSize, y + boxSize, boxSize, boxSize);
    canvas_draw_box(canvas, x + boxSize, y + 2 * boxSize, boxSize, boxSize);
    canvas_draw_box(canvas, x, y + boxSize, boxSize, boxSize);

    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "https://rac.so");

    for(int x = 0, y = 58, dx = 0; dx <= 3; dx++)
        canvas_draw_line(canvas, x + dx, y - dx, x + dx, y + dx);
}

void credits_input_callback(InputKey key, InputType type, void* context) {
    UNUSED(key);
    AppContext* app = (AppContext*)context;

    if(type == InputTypePress) scene_manager_set_scene(app->sceneManager, SceneType_Menu);
}