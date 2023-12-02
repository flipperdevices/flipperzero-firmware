#include "scene_credits.h"
#include "app.h"
#include "wave/calc.h"
#include "wave/scene_management.h"
#include "racso_sokoban_icons.h"
#include "wave/graphics/icons.h"
#include "wave/graphics/icon_drawing.h"
#include "gui/canvas.h"

typedef enum Screen
{
    Screen_ControlsGuide,
    Screen_Credits,
    Screen_COUNT
} Screen;

static Screen currentScreen;

void credits_transition_callback(int from, int to, void* context)
{
    UNUSED(from);
    UNUSED(to);
    UNUSED(context);

    currentScreen = 0;
}

void render_boxed_text(Canvas* canvas, char* text, int x, int y)
{
    int fontHeight = canvas_current_font_height(canvas);
    int textWidth = strlen(text) * fontHeight * 2 / 3;

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x, y - fontHeight / 2, textWidth, fontHeight - 1);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x - 1, y - fontHeight / 2 - 1, textWidth + 2, fontHeight + 1);

    canvas_draw_str_aligned(canvas, x + textWidth / 2, y, AlignCenter, AlignCenter, text);
}

void render_controls_guide(Canvas* canvas)
{
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 1, AlignCenter, AlignTop, "Controls");

    canvas_set_font(canvas, FontKeyboard);

    int x = 32, y = 32;
    draw_icon_aligned(canvas, x, y, AlignCenter, AlignCenter, &I_icon_button_ok);
    draw_icon_aligned(canvas, x, y - 10, AlignCenter, AlignBottom, &I_icon_button_up);
    draw_icon_aligned(canvas, x, y + 10, AlignCenter, AlignTop, &I_icon_button_down);
    draw_icon_aligned(canvas, x - 10, y, AlignRight, AlignCenter, &I_icon_button_left);
    draw_icon_aligned(canvas, x + 10, y, AlignLeft, AlignCenter, &I_icon_button_right);

    draw_icon_aligned(canvas, x + 40, y + 20, AlignLeft, AlignCenter, &I_icon_button_back);

    render_boxed_text(canvas, "Move", 36, 18);
    render_boxed_text(canvas, "Undo", 38, 34);
    render_boxed_text(canvas, "Exit", 90, 54);
}

void render_credits(Canvas* canvas)
{
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
}

void credits_render_callback(Canvas* const canvas, void* context)
{
    UNUSED(context);

    if (currentScreen == Screen_ControlsGuide)
        render_controls_guide(canvas);
    else if (currentScreen == Screen_Credits)
        render_credits(canvas);
}

void credits_input_callback(InputKey key, InputType type, void* context)
{
    UNUSED(key);
    AppContext* app = (AppContext*)context;

    if (type != InputTypePress)
        return;

    currentScreen += 1;
    if (currentScreen >= Screen_COUNT)
        scene_manager_set_scene(app->sceneManager, SceneType_Menu);
}