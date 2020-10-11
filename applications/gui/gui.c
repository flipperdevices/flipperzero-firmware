#include "gui.h"
#include "gui_i.h"

#include <furi.h>
#include <stdio.h>
#include <m-array.h>

#include "gui_event.h"
#include "canvas.h"
#include "canvas_i.h"
#include "widget.h"
#include "widget_i.h"

ARRAY_DEF(widget_array, widget_t *, M_PTR_OPLIST);

struct gui_t {
    gui_event_t         *event;
    canvas_t            *canvas;
    widget_array_t      widgets_status_bar;
    widget_array_t      widgets;
    widget_array_t      widgets_fs;
    widget_array_t      widgets_dialog;
};

void gui_widget_status_bar_add(gui_t *gui, widget_t *widget)
{
    assert(gui);
    assert(widget);
    widget_array_push_back(gui->widgets_status_bar, widget);
    widget_gui_set(widget, gui);
}

void gui_widget_add(gui_t *gui, widget_t *widget)
{
    assert(gui);
    assert(widget);
    widget_array_push_back(gui->widgets, widget);
    widget_gui_set(widget, gui);
}

void gui_widget_fs_add(gui_t *gui, widget_t *widget)
{
    assert(gui); assert(widget);

    widget_array_push_back(gui->widgets_fs, widget);
    widget_gui_set(widget, gui);
}

void gui_widget_dialog_add(gui_t *gui, widget_t *widget)
{
    assert(gui); assert(widget);

    widget_array_push_back(gui->widgets_dialog, widget);
    widget_gui_set(widget, gui);
}

void gui_update(gui_t *gui)
{
    assert(gui);
    gui_message_t message;
    message.type = GUI_MESSAGE_TYPE_REDRAW;
    gui_event_messsage_send(gui->event, &message);
}

bool gui_redraw_fs(gui_t *gui) {
    canvas_frame_set(gui->canvas, 0, 0, 128, 64);
    bool is_fullscreen_drawn = false;
    size_t widgets_fs_count = widget_array_size(gui->widgets_fs);
    for (size_t i=0; i<widgets_fs_count; i++) {
        widget_t *widget = *widget_array_get(gui->widgets_fs, widgets_fs_count-i-1);
        if (widget_is_enabled(widget)) {
            widget_draw(widget, gui->canvas);
            is_fullscreen_drawn = true;
            break;
        }
    }
    return is_fullscreen_drawn;
}

void gui_redraw_status_bar(gui_t *gui)
{
    canvas_frame_set(gui->canvas, 0, 0, 128, 64);
    widget_draw(*widget_array_back(gui->widgets_status_bar), gui->canvas);
}

void gui_redraw_normal(gui_t *gui)
{
    canvas_frame_set(gui->canvas, 0, 9, 128, 55);
    widget_draw(*widget_array_back(gui->widgets), gui->canvas);
}

void gui_redraw_dialogs(gui_t *gui)
{
    canvas_frame_set(gui->canvas, 10, 20, 118, 44);
    size_t widgets_dialog_count = widget_array_size(gui->widgets_dialog);
    for (size_t i=0; i<widgets_dialog_count; i++) {
        widget_t *widget = *widget_array_get(gui->widgets_dialog, widgets_dialog_count-i-1);
        if (widget_is_enabled(widget)) {
            widget_draw(widget, gui->canvas);
            break;
        }
    }
}

void gui_redraw(gui_t *gui)
{
    assert(gui);

    if (!gui_redraw_fs(gui)) {
        gui_redraw_status_bar(gui);
        gui_redraw_normal(gui);
    }
    gui_redraw_dialogs(gui);

    canvas_commit(gui->canvas);
}

void gui_input(gui_t *gui, InputEvent input_event)
{
    assert(gui);

    size_t widgets_fs_count = widget_array_size(gui->widgets_fs);
    for (size_t i=0; i<widgets_fs_count; i++) {
        widget_t *widget = *widget_array_get(gui->widgets_fs, widgets_fs_count-i-1);
        if (widget_is_enabled(widget)) {
            widget_input(widget, &input_event);
            return;
        }
    }
}

gui_t * gui_alloc()
{
    gui_t *gui = furi_alloc(sizeof(gui_t));
    // Initialize widget arrays
    widget_array_init(gui->widgets_status_bar);
    widget_array_init(gui->widgets);
    widget_array_init(gui->widgets_fs);
    widget_array_init(gui->widgets_dialog);
    // Event dispatcher
    gui->event = gui_event_alloc();
    // Drawing canvas
    gui->canvas = canvas_alloc();

    return gui;
}

void gui_task(void *p)
{
    gui_t *gui = gui_alloc();
    // Create FURI record
    if(!furi_create("gui", gui, sizeof(gui))) {
        printf("[gui_task] cannot create the gui record\n");
        furiac_exit(NULL);
    }
    // Forever dispatch 
    while (1) {
        gui_message_t message = gui_event_message_next(gui->event);
        if (message.type == GUI_MESSAGE_TYPE_REDRAW) {
            // TODO: furi lock maybe?
            gui_redraw(gui);
        } else if (message.type == GUI_MESSAGE_TYPE_INPUT) {
            gui_input(gui, message.input);
        }
    }
}
