#include "widget.h"
#include "widget_i.h"

#include <cmsis_os.h>
#include <flipper.h>
#include <flipper_v2.h>

#include "gui.h"
#include "gui_i.h"

// TODO add mutex to widget ops

struct Widget {
    Gui* gui;
    bool is_enabled;
    WidgetDrawCallback draw_callback;
    void* draw_callback_context;
    WidgetInputCallback input_callback;
    void* input_callback_context;
};

Widget* widget_alloc(WidgetDrawCallback callback, void* callback_context) {
    Widget* widget = furi_alloc(sizeof(Widget));
    widget->is_enabled = true;
    return widget;
}

void widget_free(Widget* widget) {
    furi_check(widget);
    furi_check(widget->gui == NULL);
    free(widget);
}

void widget_enabled_set(Widget* widget, bool enabled) {
    furi_check(widget);
    if(widget->is_enabled != enabled) {
        widget->is_enabled = enabled;
        widget_update(widget);
    }
}

bool widget_is_enabled(Widget* widget) {
    furi_check(widget);
    return widget->is_enabled;
}

void widget_draw_callback_set(Widget* widget, WidgetDrawCallback callback, void* context) {
    furi_check(widget);
    widget->draw_callback = callback;
    widget->draw_callback_context = context;
}

void widget_input_callback_set(Widget* widget, WidgetInputCallback callback, void* context) {
    furi_check(widget);
    widget->input_callback = callback;
    widget->input_callback_context = context;
}

void widget_update(Widget* widget) {
    furi_check(widget);
    if(widget->gui) gui_update(widget->gui);
}

void widget_gui_set(Widget* widget, Gui* gui) {
    furi_check(widget);
    furi_check(gui);
    widget->gui = gui;
}

void widget_draw(Widget* widget, CanvasApi* canvas_api) {
    furi_check(widget);
    furi_check(canvas_api);
    furi_check(widget->gui);

    if(widget->draw_callback) {
        widget->draw_callback(canvas_api, widget->draw_callback_context);
    }
}

void widget_input(Widget* widget, InputEvent* event) {
    furi_check(widget);
    furi_check(event);
    furi_check(widget->gui);

    if(widget->input_callback) widget->input_callback(event, widget->input_callback_context);
}
