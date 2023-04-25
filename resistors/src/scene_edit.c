#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_edit.h"

/* edit view scene */

const int left_R4 = 41;
const int positions_R4[] = {0, 13, 24, 36};

void resistors_edit_view_redraw_widget(App* app) {
    widget_reset(app->widget);
    widget_add_icon_element(app->widget, 5, 23, &I_resistor);
    if(app->state->edit_selection < COUNT_OF(positions_R4)) {
        widget_add_icon_element(
            app->widget, left_R4 + positions_R4[app->state->edit_selection], 13, &I_arrow);
    }

    widget_add_text_box_element(
        app->widget, 5, 2, 123, 10, AlignCenter, AlignCenter, "uncalculated", true);
    widget_add_text_box_element(
        app->widget, 5, 50, 123, 16, AlignCenter, AlignBottom, "X - X X X X", true);

    // widget_add_button_element(app->widget, GuiButtonTypeCenter, buttonText, callback, app);
}

/** main menu events */

static bool widget_input_callback(InputEvent* input_event, void* context) {
    App* app = context;
    bool consumed = false;
    if(input_event->type == InputTypeShort) {
        switch(input_event->key) {
        case InputKeyRight:
            if(app->state->edit_selection < 3) {
                app->state->edit_selection += 1;
            }
            resistors_edit_view_redraw_widget(app);
            consumed = true;
            break;
        case InputKeyLeft:
            if(app->state->edit_selection > 0) {
                app->state->edit_selection -= 1;
            }
            resistors_edit_view_redraw_widget(app);
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
    }
    return consumed;
}

/** edit view scene - resets the widget, and gives it content, callbacks and selection enums */
void resistors_edit_scene_on_enter(void* context) {
    App* app = context;
    resistors_edit_view_redraw_widget(app);
    view_set_context(widget_get_view(app->widget), app);
    view_set_input_callback(widget_get_view(app->widget), widget_input_callback);
    view_dispatcher_switch_to_view(app->view_dispatcher, ResistorsEditView);
}

/** edit view event handler - switches scene based on the event */
bool resistors_edit_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    // if(event.type == SceneManagerEventTypeCustom && event.event == ResistorsEditViewUpdateEvent) {
    //     ...
    //     consumed = true;
    // }
    return consumed;
}

void resistors_edit_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}
