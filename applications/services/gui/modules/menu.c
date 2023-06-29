#include "menu.h"

#include <gui/elements.h>
#include <assets_icons.h>
#include <gui/icon_i.h>
#include <gui/icon_animation_i.h>
#include <furi.h>
#include <m-array.h>
#include <cfw.h>
#include <m-string.h>

struct Menu {
    View* view;
    FuriTimer* scroll_timer;
};

typedef struct {
    const char* label;
    IconAnimation* icon;
    uint32_t index;
    MenuItemCallback callback;
    void* callback_context;
} MenuItem;

ARRAY_DEF(MenuItemArray, MenuItem, M_POD_OPLIST);

#define M_OPL_MenuItemArray_t() ARRAY_OPLIST(MenuItemArray, M_POD_OPLIST)

typedef struct {
    MenuItemArray_t items;
    size_t position;
    size_t scroll_counter;
} MenuModel;

static void menu_process_up(Menu* menu);
static void menu_process_down(Menu* menu);
static void menu_process_left(Menu* menu);
static void menu_process_right(Menu* menu);
static void menu_process_ok(Menu* menu);

static void menu_draw_callback(Canvas* canvas, void* _model) {
    MenuModel* model = _model;

    canvas_clear(canvas);

    size_t position = model->position;
    size_t items_count = MenuItemArray_size(model->items);
    if(items_count) {
        MenuItem* item;
        size_t shift_position;
        if(CFW_SETTINGS()->wii_menu) {
            FuriString* name = furi_string_alloc();
            if(position < 2) {
                shift_position = 0;
            } else if(position >= items_count - 2 + (items_count % 2)) {
                shift_position = position - (position % 2) - 4;
            } else {
                shift_position = position - (position % 2) - 2;
            }
            canvas_set_font(canvas, FontSecondary);
            size_t item_i;
            size_t x_off, y_off;
            for(int i = 0; i < 6; i++) {
                item_i = shift_position + i;
                if(item_i >= items_count) continue;
                x_off = (i / 2) * 43 + 1;
                y_off = (i % 2) * 32;
                size_t scroll_counter = 0;
                if(item_i == position) {
                    elements_slightly_rounded_box(canvas, 0 + x_off, 0 + y_off, 40, 30);
                    canvas_set_color(canvas, ColorWhite);
                    scroll_counter = model->scroll_counter;
                    if(scroll_counter < 1) {
                        scroll_counter = 0;
                    } else {
                        scroll_counter -= 1;
                    }
                }
                item = MenuItemArray_get(model->items, item_i);
                if(item->icon) {
                    canvas_draw_icon_animation(
                        canvas,
                        (40 - item->icon->icon->width) / 2 + x_off,
                        (20 - item->icon->icon->height) / 2 + y_off,
                        item->icon);
                }
                furi_string_set(name, item->label);
                if(furi_string_start_with_str(name, "[")) {
                    size_t trim = furi_string_search_str(name, "] ", 1);
                    if(trim != STRING_FAILURE) {
                        furi_string_right(name, trim + 2);
                    }
                }
                if(item->label == (char*)"Applications") furi_string_set(name, "Apps");
                if(item->label == (char*)"125 kHz RFID") furi_string_set(name, "RFID");
                if(item->label == (char*)"Sub-GHz") furi_string_set(name, "SubGHz");
                elements_scrollable_text_line(
                    canvas, 20 + x_off, 26 + y_off, 36, name, scroll_counter, false, true);
                if(item_i == position) {
                    canvas_set_color(canvas, ColorBlack);
                } else {
                    elements_frame(canvas, 0 + x_off, 0 + y_off, 40, 30);
                }
            }
            furi_string_free(name);
        } else {
            // First line
            canvas_set_font(canvas, FontSecondary);
            shift_position = (0 + position + items_count - 1) % items_count;
            item = MenuItemArray_get(model->items, shift_position);
            if(item->icon) {
                canvas_draw_icon_animation(
                    canvas,
                    4 + (14 - item->icon->icon->width) / 2,
                    3 + (14 - item->icon->icon->height) / 2,
                    item->icon);
            }
            canvas_draw_str(canvas, 22, 14, item->label);
            // Second line main
            canvas_set_font(canvas, FontPrimary);
            shift_position = (1 + position + items_count - 1) % items_count;
            item = MenuItemArray_get(model->items, shift_position);
            if(item->icon) {
                canvas_draw_icon_animation(
                    canvas,
                    4 + (14 - item->icon->icon->width) / 2,
                    25 + (14 - item->icon->icon->height) / 2,
                    item->icon);
            }
            size_t scroll_counter = model->scroll_counter;
            if(scroll_counter < 1) {
                scroll_counter = 0;
            } else {
                scroll_counter -= 1;
            }
            elements_scrollable_text_line_str(
                canvas, 22, 36, 98, item->label, scroll_counter, false, false);
            // Third line
            canvas_set_font(canvas, FontSecondary);
            shift_position = (2 + position + items_count - 1) % items_count;
            item = MenuItemArray_get(model->items, shift_position);
            if(item->icon) {
                canvas_draw_icon_animation(
                    canvas,
                    4 + (14 - item->icon->icon->width) / 2,
                    47 + (14 - item->icon->icon->height) / 2,
                    item->icon);
            }
            canvas_draw_str(canvas, 22, 58, item->label);
            // Frame and scrollbar
            elements_frame(canvas, 0, 21, 128 - 5, 21);
            elements_scrollbar(canvas, position, items_count);
        }
    } else {
        canvas_draw_str(canvas, 2, 32, "Empty");
        elements_scrollbar(canvas, 0, 0);
    }
}

static bool menu_input_callback(InputEvent* event, void* context) {
    Menu* menu = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            menu_process_up(menu);
            break;
        case InputKeyDown:
            consumed = true;
            menu_process_down(menu);
            break;
        case InputKeyLeft:
            consumed = true;
            menu_process_left(menu);
            break;
        case InputKeyRight:
            consumed = true;
            menu_process_right(menu);
            break;
        case InputKeyOk:
            consumed = true;
            menu_process_ok(menu);
            break;
        default:
            break;
        }
    } else if(event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            menu_process_up(menu);
            break;
        case InputKeyDown:
            consumed = true;
            menu_process_down(menu);
            break;
        case InputKeyLeft:
            consumed = true;
            menu_process_left(menu);
            break;
        case InputKeyRight:
            consumed = true;
            menu_process_right(menu);
            break;
        default:
            break;
        }
    }

    return consumed;
}

static void menu_scroll_timer_callback(void* context) {
    Menu* menu = context;
    with_view_model(
        menu->view, MenuModel * model, { model->scroll_counter++; }, true);
}

static void menu_enter(void* context) {
    Menu* menu = context;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            MenuItem* item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_start(item->icon);
            }
            model->scroll_counter = 0;
        },
        true);
    furi_timer_start(menu->scroll_timer, 333);
}

static void menu_exit(void* context) {
    Menu* menu = context;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            MenuItem* item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_stop(item->icon);
            }
        },
        false);
    furi_timer_stop(menu->scroll_timer);
}

Menu* menu_pos_alloc(size_t pos) {
    Menu* menu = malloc(sizeof(Menu));
    menu->view = view_alloc(menu->view);
    view_set_context(menu->view, menu);
    view_allocate_model(menu->view, ViewModelTypeLocking, sizeof(MenuModel));
    view_set_draw_callback(menu->view, menu_draw_callback);
    view_set_input_callback(menu->view, menu_input_callback);
    view_set_enter_callback(menu->view, menu_enter);
    view_set_exit_callback(menu->view, menu_exit);
    menu->scroll_timer = furi_timer_alloc(menu_scroll_timer_callback, FuriTimerTypePeriodic, menu);
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            MenuItemArray_init(model->items);
            model->position = pos;
        },
        true);

    return menu;
}

void menu_free(Menu* menu) {
    furi_assert(menu);
    menu_reset(menu);
    with_view_model(
        menu->view, MenuModel * model, { MenuItemArray_clear(model->items); }, false);
    view_free(menu->view);
    furi_timer_free(menu->scroll_timer);
    free(menu);
}

View* menu_get_view(Menu* menu) {
    furi_assert(menu);
    return (menu->view);
}

void menu_add_item(
    Menu* menu,
    const char* label,
    const Icon* icon,
    uint32_t index,
    MenuItemCallback callback,
    void* context) {
    furi_assert(menu);
    furi_assert(label);

    MenuItem* item = NULL;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            item = MenuItemArray_push_new(model->items);
            item->label = label;
            item->icon = icon ? icon_animation_alloc(icon) : icon_animation_alloc(&A_Plugins_14);
            view_tie_icon_animation(menu->view, item->icon);
            item->index = index;
            item->callback = callback;
            item->callback_context = context;
        },
        true);
}

void menu_reset(Menu* menu) {
    furi_assert(menu);
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            for
                M_EACH(item, model->items, MenuItemArray_t) {
                    icon_animation_stop(item->icon);
                    icon_animation_free(item->icon);
                }

            MenuItemArray_reset(model->items);
            model->position = 0;
        },
        true);
}

void menu_set_selected_item(Menu* menu, uint32_t index) {
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            if(index < MenuItemArray_size(model->items)) {
                model->position = index;
            }
        },
        true);
}

static void menu_process_up(Menu* menu) {
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            size_t count = MenuItemArray_size(model->items);
            MenuItem* item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_stop(item->icon);
            }

            if(CFW_SETTINGS()->wii_menu) {
                if(!(model->position == count - 1 && count % 2)) {
                    if(model->position % 2) {
                        model->position--;
                    } else {
                        model->position++;
                    }
                    model->scroll_counter = 0;
                }
            } else {
                if(model->position > 0) {
                    model->position--;
                } else {
                    model->position = count - 1;
                }
            }

            item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_start(item->icon);
            }
        },
        true);
}

static void menu_process_down(Menu* menu) {
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            size_t count = MenuItemArray_size(model->items);
            MenuItem* item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_stop(item->icon);
            }

            if(CFW_SETTINGS()->wii_menu) {
                if(!(model->position == count - 1 && count % 2)) {
                    if(model->position % 2) {
                        model->position--;
                    } else {
                        model->position++;
                    }
                    model->scroll_counter = 0;
                }
            } else {
                if(model->position < count - 1) {
                    model->position++;
                } else {
                    model->position = 0;
                }
            }

            item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_start(item->icon);
            }
        },
        true);
}

static void menu_process_left(Menu* menu) {
    if(!CFW_SETTINGS()->wii_menu) return;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            MenuItem* item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_stop(item->icon);
            }

            size_t count = MenuItemArray_size(model->items);
            if(model->position < 2) {
                if(count % 2) {
                    model->position = count - 1;
                } else {
                    model->position = count - 2 + model->position % 2;
                }
            } else {
                model->position -= 2;
            }
            model->scroll_counter = 0;

            item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_start(item->icon);
            }
        },
        true);
}

static void menu_process_right(Menu* menu) {
    if(!CFW_SETTINGS()->wii_menu) return;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            MenuItem* item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_stop(item->icon);
            }

            size_t count = MenuItemArray_size(model->items);
            if(count % 2) {
                if(model->position == count - 1) {
                    model->position = 0;
                } else if(model->position == count - 2) {
                    model->position = count - 1;
                } else {
                    model->position += 2;
                }
            } else {
                model->position += 2;
                if(model->position >= count) {
                    model->position = model->position % 2;
                }
            }
            model->scroll_counter = 0;

            item = MenuItemArray_get(model->items, model->position);
            if(item && item->icon) {
                icon_animation_start(item->icon);
            }
        },
        true);
}

static void menu_process_ok(Menu* menu) {
    MenuItem* item = NULL;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            if(model->position < MenuItemArray_size(model->items)) {
                item = MenuItemArray_get(model->items, model->position);
            }
        },
        true);
    if(item && item->callback) {
        item->callback(item->callback_context, item->index);
    }
}
