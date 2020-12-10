#pragma once

#include "gui_i.h"

struct Widget {
    Gui* gui;
    bool is_enabled;
    uint8_t width;
    uint8_t height;
    WidgetDrawCallback draw_callback;
    void* draw_callback_context;
    WidgetInputCallback input_callback;
    void* input_callback_context;
};

/*
 * Set GUI referenec.
 * @param gui - gui instance pointer.
 */
void widget_gui_set(Widget* widget, Gui* gui);

/*
 * Process draw call. Calls draw callback.
 * @param canvas - canvas to draw at.
 */
void widget_draw(Widget* widget, Canvas* canvas);

/*
 * Process input. Calls input callback.
 * @param event - pointer to input event.
 */
void widget_input(Widget* widget, InputEvent* event);
