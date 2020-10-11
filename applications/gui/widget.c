#include "widget.h"
#include "widget_i.h"

#include <cmsis_os2.h>
#include <furi.h>

#include "gui.h"
#include "gui_i.h"

struct widget_t {
    void                    *gui;
    bool                    is_enabled;
    widget_draw_callback_t  draw_callback;
    void                    *draw_callback_context;
    widget_input_callback_t input_callback;
    void                    *input_callback_context;
};

widget_t * widget_alloc(widget_draw_callback_t callback, void *callback_context)
{
    widget_t * widget = furi_alloc(sizeof(widget_t));
    widget->is_enabled = true;
    return widget;
}

void widget_free(widget_t * widget)
{
    assert(widget->gui == NULL);
    free(widget);
}

void widget_enabled_set(widget_t * widget, bool enabled)
{
    widget->is_enabled = enabled;
}

bool widget_is_enabled(widget_t * widget)
{
    return widget->is_enabled;
}

void widget_draw_callback_set(widget_t * widget, widget_draw_callback_t callback, void *context)
{
    assert(widget);
    widget->draw_callback = callback;
    widget->draw_callback_context = context;
}

void widget_input_callback_set(widget_t * widget, widget_input_callback_t callback, void *context)
{
    assert(widget);
    widget->input_callback = callback;
    widget->input_callback_context = context;
}

void widget_update(widget_t * widget)
{
    assert(widget);
    if (widget->gui) gui_update(widget->gui);
}


void widget_gui_set(widget_t * widget, gui_t *gui)
{
    assert(widget);
    assert(gui);
    widget->gui = gui;
}

void widget_draw(widget_t * widget, canvas_t *canvas)
{
    assert(widget);
    assert(widget->gui);

    if (widget->draw_callback) widget->draw_callback(canvas, widget->draw_callback_context);
}

void widget_input(widget_t * widget, InputEvent event)
{
    assert(widget);
    assert(widget->gui);

    if (widget->input_callback) widget->input_callback(event, widget->input_callback_context);
}
