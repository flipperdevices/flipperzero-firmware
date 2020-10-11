#include "menu.h"
#include <cmsis_os2.h>
#include <stdio.h>
#include <stdbool.h>

#include <furi.h>
#include <gui/gui.h>
#include <gui/canvas.h>
#include <gui/widget.h>

#include "menu_event.h"
#include "menu_item.h"

struct menu_t {
    menu_event_t            *event;
    // GUI
    FuriRecordSubscriber    *gui_record;
    widget_t                *widget;
    // State
    menu_item_t             *root;
    menu_item_t             *settings;
    menu_item_t             *current;
    uint32_t                position;
};

void menu_widget_callback(canvas_t *canvas, void *context);

menu_t * menu_alloc()
{
    menu_t *menu = furi_alloc(sizeof(menu_t));

    // Event dispatcher
    menu->event = menu_event_alloc();

    // Allocate and configure widget
    menu->widget = widget_alloc();
    widget_draw_callback_set(menu->widget, menu_widget_callback, menu);
    widget_input_callback_set(menu->widget, menu_event_input_callback, menu->event);

    // Open GUI and register fullscreen widget
    menu->gui_record = furi_open("gui", false, false, NULL, NULL, NULL);
    assert(menu->gui_record);
    gui_t *gui = furi_take(menu->gui_record);
    assert(gui);
    gui_widget_fs_add(gui, menu->widget);
    furi_commit(menu->gui_record);

    return menu;
}

void menu_build_main(menu_t *menu)
{
    assert(menu);
    // Root point
    menu->root = menu_item_alloc_menu(NULL, NULL);

    menu_item_add(menu, menu_item_alloc_function("Sub 1 gHz", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("125 kHz RFID", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("Infrared", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("I-Button", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("USB", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("Bluetooth", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("GPIO / HW", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("NFC", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("U2F", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("Tamagotchi", NULL, NULL));
    menu_item_add(menu, menu_item_alloc_function("Plugins", NULL, NULL));

    menu->settings = menu_item_alloc_menu("Setting", NULL);
    menu_item_subitem_add(menu->settings, menu_item_alloc_function("one", NULL, NULL));
    menu_item_subitem_add(menu->settings, menu_item_alloc_function("two", NULL, NULL));
    menu_item_subitem_add(menu->settings, menu_item_alloc_function("three", NULL, NULL));

    menu_item_add(menu, menu->settings);
}

void menu_item_add(menu_t *menu, menu_item_t *item)
{
    menu_item_subitem_add(menu->root, item);
}

void menu_settings_item_add(menu_t *menu, menu_item_t *item)
{
    menu_item_subitem_add(menu->settings, item);
}

void menu_widget_callback(canvas_t *canvas, void *context)
{
    assert(canvas); assert(context);

    menu_t *menu = context;
    if (!menu->current) {
        canvas_clear(canvas);
        canvas_color_set(canvas, COLOR_BLACK);
        canvas_font_set(canvas, CANVAS_FONT_PRIMARY);
        canvas_str_draw(canvas, 2, 32, "Idle Screen");
        return;
    }

    menu_items_array_t *items = menu_item_get_subitems(menu->current);

    canvas_clear(canvas);
    canvas_color_set(canvas, COLOR_BLACK);
    canvas_font_set(canvas, CANVAS_FONT_SECONDARY);

    for (size_t i=0; i<5; i++) {
        size_t shift_position = i + menu->position + menu_items_array_size(*items) - 2;
        shift_position = shift_position % (menu_items_array_size(*items));
        menu_item_t *item = *menu_items_array_get(*items, shift_position);
        canvas_str_draw(canvas, 2, 12*(i+1), menu_item_get_label(item));
    }
}

void menu_update(menu_t *menu)
{
    assert(menu);

    menu_event_activity_notify(menu->event);
    widget_update(menu->widget);
}

void menu_up(menu_t *menu)
{
    assert(menu);

    menu_items_array_t *items = menu_item_get_subitems(menu->current);
    if (menu->position == 0) menu->position = menu_items_array_size(*items);
    menu->position--;
    menu_update(menu);
}

void menu_down(menu_t *menu)
{
    assert(menu);

    menu_items_array_t *items = menu_item_get_subitems(menu->current);
    menu->position++;
    menu->position = menu->position % menu_items_array_size(*items);
    menu_update(menu);
}

void menu_ok(menu_t *menu)
{
    assert(menu);

    if (!menu->current) {
        menu->current = menu->root;
        menu_update(menu);
        return;
    }

    menu_items_array_t *items = menu_item_get_subitems(menu->current);
    menu_item_t *item = *menu_items_array_get(*items, menu->position);
    menu_item_type_t type = menu_item_get_type(item);

    if (type == MENU_ITEM_TYPE_MENU) {
        menu->current = item;
        menu->position = 0;
        menu_update(menu);
    } else if (type == MENU_ITEM_TYPE_FUNCTION) {
        menu_function_t function = menu_item_get_function(item);
        if (function) function();
    }
}

void menu_back(menu_t *menu)
{
    assert(menu);
    menu_item_t *parent = menu_item_get_parent(menu->current);
    if (parent) {
        menu->current = parent;
        menu->position = 0;
        menu_update(menu);
    } else {
        menu_exit(menu);
    }
}

void menu_exit(menu_t *menu)
{
    assert(menu);
    menu->position = 0;
    menu->current = NULL;
    menu_update(menu);
}

void menu_task(void *p)
{
    menu_t *menu = menu_alloc();
    menu_build_main(menu);
    menu_update(menu);

    if(!furi_create("menu", menu, sizeof(menu))) {
        printf("[menu_task] cannot create the menu record\n");
        furiac_exit(NULL);
    }

    while(1) {
        menu_message_t m = menu_event_next(menu->event);

        if (!menu->current && m.type != MENU_MESSAGE_TYPE_OK) {
            continue;
        } else if (m.type == MENU_MESSAGE_TYPE_UP) {
            menu_up(menu);
        } else if (m.type == MENU_MESSAGE_TYPE_DOWN) {
            menu_down(menu);
        } else if (m.type == MENU_MESSAGE_TYPE_OK) {
            menu_ok(menu);
        } else if (m.type == MENU_MESSAGE_TYPE_LEFT) {
            menu_back(menu);
        } else if (m.type == MENU_MESSAGE_TYPE_RIGHT) {
            menu_ok(menu);
        } else if (m.type == MENU_MESSAGE_TYPE_BACK) {
            menu_back(menu);
        } else if (m.type == MENU_MESSAGE_TYPE_IDLE) {
            menu_exit(menu);
        } else {
            // TODO: fail somehow?
        }
    }
}
