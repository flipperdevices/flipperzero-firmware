#include "menu.h"
#include <cmsis_os2.h>
#include <stdio.h>
#include <stdbool.h>

#include <furi.h>
#include <widgets/widget_menu.h>

#include "menu_event.h"
#include "menu_item.h"

typedef struct {
    widget_menu_t           *widget;
    menu_event_t            *event;
    menu_item_t             *current;
    uint32_t                position;
    bool                    is_active;
} menu_t;

menu_t * menu;

menu_t * menu_alloc()
{
    menu_t * menu = malloc(sizeof(menu_t));
    assert(menu != NULL);
    memset(menu, 0, sizeof(menu_t));
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

void menu_draw()
{
    const size_t widget_items_size = 5;
    widget_items_t widget_items[widget_items_size];
    menu_items_t *items = menu_item_get_subitems(menu->current);

    for (size_t i=0; i<widget_items_size; i++) {
        size_t shift_position = i + menu->position + menu_items_size(*items) - 2;
        shift_position = shift_position % (menu_items_size(*items));
        widget_items[i].label = menu_item_get_label(*menu_items_get(*items, shift_position));
    }

    widget_menu_draw_main(menu->widget, widget_items, widget_items_size);
    menu_event_active_notify(menu->event);
}

void menu_up()
{
    menu_items_t *items = menu_item_get_subitems(menu->current);
    if (menu->position == 0) {
        menu->position = menu_items_size(*items)-1;
    } else {
        menu->position--;
    }
    menu_draw();
}

void menu_down()
{
    menu_items_t *items = menu_item_get_subitems(menu->current);
    if (menu->position >= menu_items_size(*items)) {
        menu->position = 0;
    } else {
        menu->position++;
    }
    menu_draw();
}

void menu_ok()
{
    if (!menu->is_active) {
        menu->is_active = true;
        menu_draw();
        return;
    }

    menu_items_t *items = menu_item_get_subitems(menu->current);
    menu_item_t *item = *menu_items_get(*items, menu->position);
    menu_item_type_t type = menu_item_get_type(item);

    if (type == MENU_ITEM_TYPE_MENU) {
        menu->current = item;
        menu->position = 0;
        menu_draw();
    } else if (type == MENU_ITEM_TYPE_FUNCTION) {
        menu_function_t function = menu_item_get_function(item);
        function();
    }
}

void menu_back()
{
    menu_item_t *parent = menu_item_get_parent(menu->current);
    if (parent) {
        menu->current = parent;
        menu->position = 0;
        menu_draw();
    } else {
        menu_exit();
    }
}

void menu_exit()
{
    menu->position = 0;
    menu->is_active = false;
    widget_menu_draw_idle(menu->widget);
}

void menu_task(void * p)
{
    menu = menu_alloc();

    menu->widget = widget_menu_alloc();
    menu->event = menu_event_alloc();

    menu_build_main();

    widget_menu_draw_idle(menu->widget);

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
