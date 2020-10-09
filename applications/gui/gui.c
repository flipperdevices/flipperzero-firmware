#include "gui.h"

#include <furi.h>

#include "canvas.h"
#include "widget.h"

#define GUI_EVENT_MQUEUE_SIZE 32
#define GUI_EVENT_TYPE_REDRAW 0x00

typedef struct {
    uint8_t type;
} gui_event_t;

typedef struct {
    osMessageQueueId_t  mqueue;
    gui_event_t         event;
    canvas_t            *canvas;
    widget_t            *widget;
} gui_t;

gui_t *gui;

void gui_widget_set(widget_t * widget)
{
    assert(gui);
    gui->widget = widget;
}

void gui_update()
{
    assert(gui);
    gui_event_t event;
    event.type = GUI_EVENT_TYPE_REDRAW;
    osMessageQueuePut(gui->mqueue, &event, 0, 0);
}

void gui_redraw()
{
    if (gui->widget) {
        widget_draw(gui->widget, gui->canvas);
        canvas_commit(gui->canvas);
    }
}

gui_t * gui_alloc()
{
    gui_t *gui = furi_alloc(sizeof(gui_t));

    gui->mqueue = osMessageQueueNew(GUI_EVENT_MQUEUE_SIZE, sizeof(gui_event_t), NULL);
    assert(gui->mqueue);

    gui->canvas = canvas_alloc();

    return gui;
}

void gui_task(void *p)
{
    gui = gui_alloc();

    while (1) {
        if (osMessageQueueGet(gui->mqueue, &gui->event, NULL, osWaitForever) == osOK) {
            if (gui->event.type == GUI_EVENT_TYPE_REDRAW) {
                gui_redraw();
            }
        } 
    }
}
