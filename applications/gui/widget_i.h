#pragma once

void widget_gui_set(widget_t * widget, gui_t *gui);

void widget_draw(widget_t * widget, canvas_t *canvas);

void widget_input(widget_t * widget, InputEvent *event);

