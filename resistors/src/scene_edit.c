#include "flipper.h"
#include "resistors_app.h"
#include "resistor_logic.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_edit.h"
#include <applications/services/gui/modules/widget.h>
#include <applications/services/gui/modules/widget_elements/widget_element.h>
#include <applications/services/gui/view.h>

const int values_left = 64;
const int rows_tops[] = {0, 9, 18};
const int footer_top = 55;

const int bands_top = 30;
const int bands_lefts[] = {18, 34, 50, 66, 86, 102};
const int band_indices[6][6] =
    {{}, {}, {0, 1, 2}, {0, 1, 2, 5}, {0, 1, 2, 3, 5}, {0, 1, 2, 3, 4, 5}};
const int band_w = 8;
const int band_h = 22;

void resistors_edit_view_redraw_widget(App* app) {
    widget_reset(app->widget);

    Icon* icon;
    switch(app->state->resistor_type) {
    case R3:
        icon = (Icon*)&I_r3;
        break;
    case R4:
        icon = (Icon*)&I_r4;
        break;
    case R5:
        icon = (Icon*)&I_r5;
        break;
    case R6:
        icon = (Icon*)&I_r6;
        break;
    default:
        FURI_LOG_E(TAG, "Unrecognised resistor type in resistors_edit_view_redraw_widget");
        app_quit(app);
        return;
    }

    // render resistor graphic
    widget_add_icon_element(app->widget, 0, 0, icon);

    // render band indicator
    if(app->state->edit_selection < app->state->resistor_type) {
        int band_index = band_indices[app->state->resistor_type - 1][app->state->edit_selection];
        widget_add_icon_element(app->widget, bands_lefts[band_index], bands_top, &I_box_8x22);
    }

    // render band colour descriptors (short)
    for(int i = 0; i < app->state->resistor_type; i++) {
        int description_index = band_indices[app->state->resistor_type - 1][i];
        int description_left = bands_lefts[description_index];
        widget_add_string_element(
            app->widget,
            description_left,
            footer_top,
            AlignLeft,
            AlignTop,
            FontPrimary,
            get_colour_short_description(app->state->resistor_bands[i]));
    }

    // render calculation
    char calculation[CALCULATION_LEN];
    update_resistance_calculation(
        app->state->resistor_type, app->state->resistor_bands, calculation);
    widget_add_string_element(
        app->widget, values_left, rows_tops[0], AlignLeft, AlignTop, FontSecondary, calculation);

    // TODO: update_tolerance

    // TODO: update_temp_coefficient

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
            consumed = true;
            break;
        case InputKeyLeft:
            if(app->state->edit_selection > 0) {
                app->state->edit_selection -= 1;
            }
            consumed = true;
            break;
        case InputKeyUp:
            app->state->resistor_bands[app->state->edit_selection] = alter_resistor_band(
                app->state->resistor_type,
                app->state->edit_selection,
                app->state->resistor_bands[app->state->edit_selection],
                1);
            consumed = true;
            break;
        case InputKeyDown:
            app->state->resistor_bands[app->state->edit_selection] = alter_resistor_band(
                app->state->resistor_type,
                app->state->edit_selection,
                app->state->resistor_bands[app->state->edit_selection],
                -1);
            consumed = true;
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
    }
    if(consumed) resistors_edit_view_redraw_widget(app);
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
