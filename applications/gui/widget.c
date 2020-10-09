#include "widget.h"

#include <cmsis_os2.h>
#include <furi.h>

#include "gui.h"

struct widget_t {
    widget_draw_callback_t callback;
    void * callback_context;
};

widget_t * widget_alloc(widget_draw_callback_t callback, void *callback_context)
{
    widget_t * widget = furi_alloc(sizeof(widget_t));
    widget->callback = callback;
    widget->callback_context = callback_context;

    gui_widget_set(widget);

    return widget;
}

void widget_free(widget_t * widget)
{
    free(widget);
}

void widget_draw(widget_t * widget, canvas_t *canvas)
{
    if (widget->callback) widget->callback(canvas, widget->callback_context);
}

void widget_update(widget_t * widget)
{
    gui_update();
}