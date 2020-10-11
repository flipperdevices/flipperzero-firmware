#pragma once

#include <input/input.h>

typedef struct gui_t gui_t;
typedef struct canvas_t canvas_t;
typedef struct widget_t widget_t;

typedef void (*widget_draw_callback_t)(canvas_t *canvas, void *context);
typedef void (*widget_input_callback_t)(InputEvent *event, void *context);

widget_t * widget_alloc();
void widget_free(widget_t * widget);

void widget_enabled_set(widget_t * widget, bool enabled);
bool widget_is_enabled(widget_t * widget);

void widget_draw_callback_set(widget_t * widget, widget_draw_callback_t callback, void *context);
void widget_input_callback_set(widget_t * widget, widget_input_callback_t callback, void *context);

// emit update signal
void widget_update(widget_t * widget);
