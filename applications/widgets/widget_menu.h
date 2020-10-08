#pragma once

#include <stdint.h>
#include <string.h>

typedef struct {
    void * icon;
    const char * label;
} widget_items_t;

typedef struct widget_menu_t widget_menu_t;

widget_menu_t * widget_menu_alloc();

void widget_menu_free(widget_menu_t *p);

void widget_menu_draw_main(widget_menu_t *p, widget_items_t items[], size_t items_count);

void widget_menu_draw_idle(widget_menu_t *p);
