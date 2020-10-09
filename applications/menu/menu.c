#include "menu.h"
#include <cmsis_os2.h>
#include <stdio.h>
#include <stdbool.h>

#include <furi.h>
#include <gui/widget.h>
#include <gui/canvas.h>

#include "menu_event.h"
#include "menu_item.h"
typedef struct {
    widget_t                *widget;
    menu_event_t            *event;
    menu_item_t             *current;
    uint32_t                position;
    bool                    is_active;
} menu_t;

menu_t * menu;

void menu_widget_callback(canvas_t *canvas, void *context);

menu_t * menu_alloc()
{
    menu_t * menu = furi_alloc(sizeof(menu_t));
    menu->widget = widget_alloc(menu_widget_callback, menu);
    menu->event = menu_event_alloc();
    return menu;
}

void menu_build_main()
{
    // Root point
    menu->current = menu_item_alloc_menu(NULL, NULL);

    menu_subitem_add(menu->current, menu_item_alloc_function("Sub 1 gHz", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("125 kHz RFID", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("Infrared", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("I-Button", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("USB", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("Bluetooth", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("GPIO / HW", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("NFC", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("U2F", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("Tamagotchi", NULL, NULL));
    menu_subitem_add(menu->current, menu_item_alloc_function("Plugins", NULL, NULL));
    menu_item_t *settings = menu_item_alloc_menu("Setting", NULL);
    menu_subitem_add(settings, menu_item_alloc_function("one", NULL, NULL));
    menu_subitem_add(settings, menu_item_alloc_function("two", NULL, NULL));
    menu_subitem_add(settings, menu_item_alloc_function("three", NULL, NULL));
    menu_subitem_add(menu->current, settings);
}

void menu_widget_callback(canvas_t *canvas, void *context)
{
    if (!menu->is_active) {
        canvas_clear(canvas);
        return;
    }

    menu_items_t *items = menu_item_get_subitems(menu->current);

    canvas_clear(canvas);
    canvas_color_set(canvas, COLOR_BLACK);
    canvas_font_set(canvas, CANVAS_FONT_SECONDARY);

    for (size_t i=0; i<5; i++) {
        size_t shift_position = i + menu->position + menu_items_size(*items) - 2;
        shift_position = shift_position % (menu_items_size(*items));
        menu_item_t *item = *menu_items_get(*items, shift_position);
        canvas_str_draw(canvas, 2, 12*(i+1), menu_item_get_label(item));
    }
}

void menu_update() {
    menu_event_active_notify(menu->event);
    widget_update(menu->widget);
}

void menu_up()
{
    menu_items_t *items = menu_item_get_subitems(menu->current);
    if (menu->position == 0) menu->position = menu_items_size(*items);
    menu->position--;
    menu_update();
}

void menu_down()
{
    menu_items_t *items = menu_item_get_subitems(menu->current);
    menu->position++;
    menu->position = menu->position % menu_items_size(*items);
    menu_update();
}

void menu_ok()
{
    if (!menu->is_active) {
        menu->is_active = true;
        menu_update();
        return;
    }

    menu_items_t *items = menu_item_get_subitems(menu->current);
    menu_item_t *item = *menu_items_get(*items, menu->position);
    menu_item_type_t type = menu_item_get_type(item);

    if (type == MENU_ITEM_TYPE_MENU) {
        menu->current = item;
        menu->position = 0;
        menu_update();
    } else if (type == MENU_ITEM_TYPE_FUNCTION) {
        menu_function_t function = menu_item_get_function(item);
        if (function) function();
    }
}

void menu_back()
{
    menu_item_t *parent = menu_item_get_parent(menu->current);
    if (parent) {
        menu->current = parent;
        menu->position = 0;
        menu_update();
    } else {
        menu_exit();
    }
}

void menu_exit()
{
    menu->position = 0;
    menu->is_active = false;
    menu_update();
}

void menu_task(void * p)
{
    menu = menu_alloc();
    menu_build_main();
    menu_update();

    while(1) {
        menu_message_t * m = menu_event_next(menu->event);
        if (m == NULL) {
            continue;
        }

        if (!menu->is_active && m->type != MENU_MESSAGE_TYPE_OK) {
            continue;
        } else if (m->type == MENU_MESSAGE_TYPE_UP) {
            menu_up();
        } else if (m->type == MENU_MESSAGE_TYPE_DOWN) {
            menu_down();
        } else if (m->type == MENU_MESSAGE_TYPE_OK) {
            menu_ok();
        } else if (m->type == MENU_MESSAGE_TYPE_LEFT) {
            menu_back();
        } else if (m->type == MENU_MESSAGE_TYPE_RIGHT) {
            menu_ok();
        } else if (m->type == MENU_MESSAGE_TYPE_BACK) {
            menu_back();
        } else if (m->type == MENU_MESSAGE_TYPE_IDLE) {
            menu_exit();
        } else {
            // TODO: fail somehow?
        }
    }
}
