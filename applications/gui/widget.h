#pragma once

typedef struct canvas_t canvas_t;
typedef struct widget_t widget_t;
typedef void (*widget_draw_callback_t)(canvas_t *canvas, void *context);

widget_t * widget_alloc(widget_draw_callback_t callback, void *callback_context);

void widget_draw(widget_t * widget, canvas_t *canvas);

void widget_free(widget_t * widget);

void widget_update(widget_t * widget);