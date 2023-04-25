#include "flipper.h"
#include "resistors_app.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_edit.h"

/* edit view scene */

const int resistor_image_top = 23;
const int resistor_image_le = 23;

const int resistor_image_left_R4 = 5;
const int resistor_image_top_R4 = 23;
const int resistor_arrow_left_R4 = 41;
const int resistor_arrow_top_R4 = 13;
const int arrow_positions_R4[] = {0, 13, 24, 36};

const int resistor_image_left_R5 = 5;
const int resistor_image_top_R5 = 23;
const int resistor_arrow_left_R5 = 48;
const int resistor_arrow_top_R5 = 13;
const int arrow_positions_R5[] = {0, 7, 15, 25, 32};

void resistors_edit_view_redraw_widget(App* app) {
    widget_reset(app->widget);

    int* arrow_positions;
    int arrow_left;
    int arrow_top;
    int image_left;
    int image_top;
    Icon* icon;

    switch(app->state->resistor_type) {
    case Resistor4Band:
        arrow_left = resistor_arrow_left_R4;
        arrow_top = resistor_arrow_top_R4;
        arrow_positions = (int*)arrow_positions_R4;
        image_left = resistor_image_left_R4;
        image_top = resistor_image_top_R4;
        icon = (Icon*)&I_resistor_4;
        break;
    case Resistor5Band:
        arrow_left = resistor_arrow_left_R5;
        arrow_top = resistor_arrow_top_R5;
        arrow_positions = (int*)arrow_positions_R5;
        image_left = resistor_image_left_R5;
        image_top = resistor_image_top_R5;
        icon = (Icon*)&I_resistor_5;
        break;
    default:
        FURI_LOG_E(TAG, "Unrecognised resistor type in resistors_edit_view_redraw_widget");
        scene_manager_stop(app->scene_manager);
        return;
    }

    widget_add_icon_element(app->widget, image_left, image_top, icon);
    if(app->state->edit_selection < app->state->resistor_type) {
        widget_add_icon_element(
            app->widget,
            arrow_left + *(arrow_positions + app->state->edit_selection),
            arrow_top,
            &I_arrow);
    }

    widget_add_text_box_element(
        app->widget, 5, 2, 123, 10, AlignCenter, AlignCenter, "uncalculated", true);
    widget_add_text_box_element(
        app->widget, 5, 50, 123, 16, AlignCenter, AlignBottom, "Xx Xx Xx Xx - Xx", true);

    // widget_add_button_element(app->widget, GuiButtonTypeCenter, buttonText, callback, app);
}

/** main menu events */

static bool widget_input_callback(InputEvent* input_event, void* context) {
    App* app = context;
    bool consumed = false;
    if(input_event->type == InputTypeShort) {
        switch(input_event->key) {
        case InputKeyRight:
            if(app->state->edit_selection < app->state->resistor_type - 1) {
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
