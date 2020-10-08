#include "menu.h"
#include <cmsis_os2.h>
#include <stdio.h>

#include <furi.h>
#include <m-array.h>
#include <widgets/widget_menu.h>

#include "menu_event.h"
#include "menu_item.h"

ARRAY_DEF(menu_items, menu_item_t *, M_PTR_OPLIST)

typedef struct {
    widget_menu_t           * widget;
    menu_event_t            * event;
    uint32_t                position;
    menu_items_t            items;
} menu_t;

menu_t * menu;

menu_t * menu_alloc()
{
    menu_t * menu = malloc(sizeof(menu_t));
    assert(menu != NULL);
    memset(menu, 0, sizeof(menu_t));
    menu_items_init(menu->items);
    return menu;
}

void menu_item_add(uint8_t type, const char *label, void *icon, void *data)
{
    menu_item_t * menu_item = menu_item_alloc_init(type, label, icon, data);
    menu_items_push_back(menu->items, menu_item);
}

void menu_build_main()
{
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "Sub 1 gHz", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "125 kHz RFID", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "Infrared", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "I-Button", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "USB", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "Bluetooth", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "GPIO / HW", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "NFC", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "U2F", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "Tamagotchi", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "Plugins", NULL, NULL);
    menu_item_add(MENU_ITEM_TYPE_FUNCTION, "Setting", NULL, NULL);
}

void menu_draw()
{
    const size_t widget_items_size = 5;
    widget_items_t widget_items[widget_items_size];

    for (size_t i=0; i<widget_items_size; i++) {
        size_t shift_position = i + menu->position + menu_items_size(menu->items) - 2;
        shift_position = shift_position % (menu_items_size(menu->items));
        widget_items[i].label = menu_item_get_label(*menu_items_get(menu->items, shift_position));
    }

    widget_menu_draw_main(menu->widget, widget_items, widget_items_size);
    menu_event_active_notify(menu->event);
}

void menu_up()
{
    if (menu->position == 0) {
        menu->position = menu_items_size(menu->items)-1;
    } else {
        menu->position--;
    }
    menu_draw();
}

void menu_down()
{
    if (menu->position >= menu_items_size(menu->items)) {
        menu->position = 0;
    } else {
        menu->position++;
    }
    menu_draw();
}

void menu_back()
{
    menu->position = 0;
    widget_menu_draw_idle(menu->widget);
}

void menu_exit()
{
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

        if (m->type == MENU_MESSAGE_TYPE_UP) {
            menu_up();
        } else if (m->type == MENU_MESSAGE_TYPE_DOWN) {
            menu_down();
        } else if (m->type == MENU_MESSAGE_TYPE_OK) {
            menu_draw();
        } else if (m->type == MENU_MESSAGE_TYPE_LEFT) {
            menu_back();
        } else if (m->type == MENU_MESSAGE_TYPE_RIGHT) {
            menu_draw();
        } else if (m->type == MENU_MESSAGE_TYPE_BACK) {
            menu_back();
        } else if (m->type == MENU_MESSAGE_TYPE_IDLE) {
            menu_exit();
        } else {
            // TODO: fail somehow?
        }
    }
}
