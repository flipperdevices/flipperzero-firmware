#include "../color_guess.h"
#include "../digits.h"
#include "../views/color_guess_color_set.h"

/*static void color_set_render_callback(Canvas* const canvas, void* context) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)context, 25);
    const int cursorOffset = 30;
    const int newCursorPos = (plugin_state->cursorpos * 12) + cursorOffset;
    //const Icon* icon = &I_ButtonUp_7x4;
    if (plugin_state == NULL) {
        return;
    }
    //Border around the edge of the screen
    // canvas_draw_frame(canvas, 0, 0, 128, 64);

    // canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_icon(canvas, newCursorPos, 18, &I_ButtonUp_10x5);
    canvas_draw_icon(canvas, newCursorPos, 41, &I_ButtonDown_10x5);
    canvas_draw_icon(canvas, 18, 25, digits[16]);
    canvas_draw_icon(canvas, 30, 25, digits[plugin_state->digit[0]]);
    canvas_draw_icon(canvas, 42, 25, digits[plugin_state->digit[1]]);
    canvas_draw_icon(canvas, 54, 25, digits[plugin_state->digit[2]]);
    canvas_draw_icon(canvas, 66, 25, digits[plugin_state->digit[3]]);
    canvas_draw_icon(canvas, 78, 25, digits[plugin_state->digit[4]]);
    canvas_draw_icon(canvas, 90, 25, digits[plugin_state->digit[5]]);
    release_mutex((ValueMutex*)ctx, plugin_state);
}*/

/*static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}*/

/*static void color_guess_state_init(PluginState* const plugin_state) {
    plugin_state->scene = 0;
    plugin_state->cursorpos = 0;
    plugin_state->red = 0;
    for (int i = 0;i < 6; i++) {
        plugin_state->digit[i] = 0;
    }
    plugin_state->green = 0;
    plugin_state->blue = 0;
}*/

void color_guess_scene_color_set_on_enter(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    UNUSED(app);
    //color_guess_state_init(app->plugin_state);


    view_dispatcher_switch_to_view(app->view_dispatcher, ColorGuessViewIdColorSet);
}

bool color_guess_scene_color_set_on_event(void* context, SceneManagerEvent event) {
    ColorGuess* app = context;
    UNUSED(app);
    UNUSED(event);
    //color_set_render_callback();
    // if(event.event == SubmenuIndexColorSet) {
    //     scene_manager_set_scene_state(
    //         app->scene_manager, ColorGuessSceneStart, SubmenuIndexPlay);
    //     scene_manager_next_scene(app->scene_manager, ColorGuessSceneColorSet);
    //     return true;
    // }
    return false;
}

void color_guess_scene_color_set_on_exit(void* context) {
    ColorGuess* app = context;
    UNUSED(app);

    //submenu_reset(app->submenu);
}

