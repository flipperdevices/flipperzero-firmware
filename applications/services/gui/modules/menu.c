#include "menu.h"

#include <gui/elements.h>
#include <assets_icons.h>
#include <gui/icon_i.h>
#include <gui/icon_animation_i.h>
#include <gui/canvas_i.h>
#include <dolphin/dolphin_i.h>
#include <dolphin/helpers/dolphin_state.h>
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
    size_t vertical_offset;
} MenuModel;

static void menu_process_up(Menu* menu);
static void menu_process_down(Menu* menu);
static void menu_process_left(Menu* menu);
static void menu_process_right(Menu* menu);
static void menu_process_ok(Menu* menu);

static void menu_short_name(MenuItem* item, FuriString* name) {
    furi_string_set(name, item->label);
    if(furi_string_start_with_str(name, "[")) {
        size_t trim = furi_string_search_str(name, "] ", 1);
        if(trim != STRING_FAILURE) {
            furi_string_right(name, trim + 2);
        }
    }
}

static void menu_string_to_upper_case(FuriString* str) {
    for(size_t i = 0; i < furi_string_size(str); i++) {
        char c = furi_string_get_char(str, i);
        if(c >= 'a' && c <= 'z') {
            furi_string_set_char(str, i, c - 'a' + 'A');
        }
    }
}

static void menu_centered_icon(
    Canvas* canvas,
    MenuItem* item,
    size_t x,
    size_t y,
    size_t width,
    size_t height) {
    if(item->icon) {
        canvas_draw_icon_animation(
            canvas,
            x + (width - item->icon->icon->width) / 2,
            y + (height - item->icon->icon->height) / 2,
            item->icon);
    }
}

static size_t menu_scroll_counter(MenuModel* model, bool selected) {
    if(!selected) return 0;
    size_t scroll_counter = model->scroll_counter;
    if(scroll_counter > 0) {
        scroll_counter--;
    }
    return scroll_counter;
}

static void menu_draw_callback(Canvas* canvas, void* _model) {
    MenuModel* model = _model;

    canvas_clear(canvas);

    size_t position = model->position;
    size_t items_count = MenuItemArray_size(model->items);
    if(items_count) {
        MenuItem* item;
        size_t shift_position;
        switch(CFW_SETTINGS()->menu_style) {
        case MenuStyleList: {
            for(uint8_t i = 0; i < 3; i++) {
                canvas_set_font(canvas, i == 1 ? FontPrimary : FontSecondary);
                shift_position = (position + items_count + i - 1) % items_count;
                item = MenuItemArray_get(model->items, shift_position);
                menu_centered_icon(canvas, item, 4, 3 + 22 * i, 14, 14);
                size_t scroll_counter = menu_scroll_counter(model, i == 1);
                elements_scrollable_text_line_str(
                    canvas, 22, 14 + 22 * i, 98, item->label, scroll_counter, false, false);
            }
            // Frame and scrollbar
            elements_frame(canvas, 0, 21, 128 - 5, 21);
            elements_scrollbar(canvas, position, items_count);
            break;
        }
        case MenuStyleWii: {
            FuriString* name = furi_string_alloc();
            if(items_count > 6 && position >= 4) {
                if(position >= items_count - 2 + (items_count % 2)) {
                    shift_position = position - (position % 2) - 4;
                } else {
                    shift_position = position - (position % 2) - 2;
                }
            } else {
                shift_position = 0;
            }
            canvas_set_font(canvas, FontSecondary);
            size_t item_i;
            size_t x_off, y_off;
            for(uint8_t i = 0; i < 6; i++) {
                item_i = shift_position + i;
                if(item_i >= items_count) continue;
                x_off = (i / 2) * 43 + 1;
                y_off = (i % 2) * 32;
                bool selected = item_i == position;
                size_t scroll_counter = menu_scroll_counter(model, selected);
                if(selected) {
                    elements_slightly_rounded_box(canvas, 0 + x_off, 0 + y_off, 40, 30);
                    canvas_set_color(canvas, ColorWhite);
                }
                item = MenuItemArray_get(model->items, item_i);
                menu_centered_icon(canvas, item, x_off, y_off, 40, 20);
                menu_short_name(item, name);
                if(item->label == (char*)"125 kHz RFID") furi_string_set(name, "RFID");
                if(item->label == (char*)"Sub-GHz") furi_string_set(name, "SubGHz");
                elements_scrollable_text_line(
                    canvas, 20 + x_off, 26 + y_off, 36, name, scroll_counter, false, true);
                if(selected) {
                    canvas_set_color(canvas, ColorBlack);
                } else {
                    elements_frame(canvas, 0 + x_off, 0 + y_off, 40, 30);
                }
            }
            furi_string_free(name);
            break;
        }
        case MenuStyleDsi: {
            for(int8_t i = -2; i <= 2; i++) {
                shift_position = (position + items_count + i) % items_count;
                item = MenuItemArray_get(model->items, shift_position);
                size_t width = 24;
                size_t height = 26;
                size_t pos_x = 64;
                size_t pos_y = 36;
                if(i == 0) {
                    width += 6;
                    height += 4;
                    elements_bold_rounded_frame(
                        canvas, pos_x - width / 2, pos_y - height / 2, width, height + 5);
                    canvas_set_font(canvas, FontBatteryPercent);
                    canvas_draw_str_aligned(
                        canvas, pos_x - 9, pos_y + height / 2 + 1, AlignCenter, AlignBottom, "S");
                    canvas_draw_str_aligned(
                        canvas, pos_x, pos_y + height / 2 + 1, AlignCenter, AlignBottom, "TAR");
                    canvas_draw_str_aligned(
                        canvas, pos_x + 9, pos_y + height / 2 + 1, AlignCenter, AlignBottom, "T");

                    canvas_draw_rframe(canvas, 0, 0, 128, 18, 3);
                    canvas_draw_line(canvas, 60, 18, 64, 26);
                    canvas_draw_line(canvas, 64, 26, 68, 18);
                    canvas_set_color(canvas, ColorWhite);
                    canvas_draw_line(canvas, 60, 17, 68, 17);
                    canvas_draw_box(canvas, 62, 21, 5, 2);
                    canvas_set_color(canvas, ColorBlack);

                    canvas_set_font(canvas, FontPrimary);
                    size_t scroll_counter = menu_scroll_counter(model, true);
                    elements_scrollable_text_line_str(
                        canvas,
                        pos_x,
                        pos_y - height / 2 - 8,
                        126,
                        item->label,
                        scroll_counter,
                        false,
                        true);
                } else {
                    pos_x += (width + 6) * i;
                    pos_y += 2;
                    elements_slightly_rounded_frame(
                        canvas, pos_x - width / 2, pos_y - height / 2, width, height);
                }
                menu_centered_icon(canvas, item, pos_x - 7, pos_y - 7, 14, 14);
            }
            elements_scrollbar_horizontal(canvas, 0, 64, 128, position, items_count);
            break;
        }
        case MenuStylePs4: {
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str_aligned(
                canvas, 1, 1, AlignLeft, AlignTop, furi_hal_version_get_name_ptr());
            char str[10];
            Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
            snprintf(str, 10, "Level %i", dolphin_get_level(dolphin->state->data.icounter));
            furi_record_close(RECORD_DOLPHIN);
            canvas_draw_str_aligned(canvas, 127, 1, AlignRight, AlignTop, str);
            for(int8_t i = -1; i <= 4; i++) {
                shift_position = position + i;
                if(shift_position >= items_count) continue;
                item = MenuItemArray_get(model->items, shift_position);
                size_t width = 20;
                size_t height = 20;
                size_t pos_x = 36;
                size_t pos_y = 27;
                if(i == 0) {
                    width += 10;
                    height += 10;
                    pos_y += 2;
                    canvas_draw_box(canvas, pos_x - width / 2, pos_y + height / 2, width, 9);
                    canvas_set_color(canvas, ColorWhite);
                    canvas_set_font(canvas, FontBatteryPercent);
                    canvas_draw_str_aligned(
                        canvas, pos_x, pos_y + height / 2 + 1, AlignCenter, AlignTop, "Start");

                    canvas_set_color(canvas, ColorBlack);
                    canvas_set_font(canvas, FontSecondary);
                    size_t scroll_counter = menu_scroll_counter(model, true);
                    elements_scrollable_text_line_str(
                        canvas,
                        pos_x + width / 2 + 2,
                        pos_y + height / 2 + 7,
                        74,
                        item->label,
                        scroll_counter,
                        false,
                        false);
                } else {
                    pos_x += (width + 1) * i + (i < 0 ? -6 : 6);
                }
                canvas_draw_frame(canvas, pos_x - width / 2, pos_y - height / 2, width, height);
                menu_centered_icon(canvas, item, pos_x - 7, pos_y - 7, 14, 14);
            }
            elements_scrollbar_horizontal(canvas, 0, 64, 128, position, items_count);
            break;
        }
        case MenuStyleVertical: {
            canvas_set_orientation(canvas, CanvasOrientationVertical);
            FuriString* name = furi_string_alloc();
            shift_position = model->vertical_offset;
            canvas_set_font(canvas, FontSecondary);
            size_t item_i;
            size_t y_off;
            for(size_t i = 0; i < 8; i++) {
                item_i = shift_position + i;
                if(item_i >= items_count) continue;
                y_off = 16 * i;
                bool selected = item_i == position;
                size_t scroll_counter = menu_scroll_counter(model, selected);
                if(selected) {
                    elements_slightly_rounded_box(canvas, 0, y_off, 64, 16);
                    canvas_set_color(canvas, ColorWhite);
                }
                item = MenuItemArray_get(model->items, item_i);
                menu_centered_icon(canvas, item, 0, y_off, 16, 16);
                menu_short_name(item, name);
                elements_scrollable_text_line(
                    canvas, 17, y_off + 12, 46, name, scroll_counter, false, false);
                if(selected) {
                    canvas_set_color(canvas, ColorBlack);
                }
            }
            furi_string_free(name);
            canvas_set_orientation(canvas, CanvasOrientationHorizontal);
            break;
        }

        case MenuStyleC64: {
            FuriString* memstr = furi_string_alloc();
            FuriString* name = furi_string_alloc();

            size_t index;
            size_t y_off, x_off;

            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str_aligned(
                canvas, 64, 0, AlignCenter, AlignTop, "* FLIPPADORE 64 BASIC *");

            furi_string_printf(memstr, "%d BASIC BYTES FREE", memmgr_get_free_heap());

            canvas_draw_str_aligned(
                canvas, 64, 9, AlignCenter, AlignTop, furi_string_get_cstr(memstr));

            canvas_set_font(canvas, FontKeyboard);

            for(size_t i = 0; i < 2; i++) {
                for(size_t j = 0; j < 5; j++) {
                    index = i * 5 + j + (position - (position % 10));
                    if(index >= items_count) continue;
                    y_off = (9 * j) + 13;
                    x_off = 64 * i;
                    bool selected = index == position;
                    size_t scroll_counter = menu_scroll_counter(model, selected);
                    if(selected) {
                        canvas_draw_box(canvas, x_off, y_off + 4, 64, 9);
                        canvas_set_color(canvas, ColorWhite);
                    }
                    item = MenuItemArray_get(model->items, index);
                    menu_short_name(item, name);

                    FuriString* item_str = furi_string_alloc();

                    furi_string_printf(item_str, "%d.%s", index, furi_string_get_cstr(name));

                    elements_scrollable_text_line(
                        canvas, x_off + 2, y_off + 12, 64, item_str, scroll_counter, false, false);

                    furi_string_free(item_str);

                    if(selected) {
                        canvas_set_color(canvas, ColorBlack);
                    }
                }
            }

            furi_string_free(memstr);
            furi_string_free(name);

            break;
        }
        case MenuStyleEurocorp: {
            FuriString* name = furi_string_alloc();

#ifdef CANVAS_HAS_FONT_EUROCORP
            canvas_set_font(canvas, FontEurocorp);
#else
            canvas_set_font(canvas, FontPrimary);
#endif
            for(uint8_t i = 0; i < 3; i++) {
                canvas_set_color(canvas, ColorBlack);
                shift_position = (position + items_count + i - 1) % items_count;
                item = MenuItemArray_get(model->items, shift_position);
                menu_short_name(item, name);
                menu_string_to_upper_case(name);
                size_t scroll_counter = menu_scroll_counter(model, i == 1);
                if(i == 1) {
                    canvas_draw_box(canvas, 0, 22, 128, 22);
                    canvas_set_color(canvas, ColorWhite);
                    // Clip corner
                    for(uint8_t i = 0; i < 6; i++) {
                        for(uint8_t j = 0; j < 6; j++) {
                            if(j - i >= 0) {
                                canvas_draw_dot(canvas, 128 - i, 22 + j - i);
                            }
                        }
                    }
                }
                elements_scrollable_text_line(
                    canvas, 2, 19 + 22 * i, 128 - 3, name, scroll_counter, false, false);
            }

            furi_string_free(name);

            break;
        }
        default:
            break;
        }
    } else {
        canvas_draw_str(canvas, 2, 32, "Empty");
        elements_scrollbar(canvas, 0, 0);
    }
}

static bool menu_input_callback(InputEvent* event, void* context) {
    Menu* menu = context;
    bool consumed = true;
    if(CFW_SETTINGS()->menu_style == MenuStyleVertical &&
       furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient)) {
        if(event->key == InputKeyLeft) {
            event->key = InputKeyRight;
        } else if(event->key == InputKeyRight) {
            event->key = InputKeyLeft;
        }
    }

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            menu_process_up(menu);
            break;
        case InputKeyDown:
            menu_process_down(menu);
            break;
        case InputKeyLeft:
            menu_process_left(menu);
            break;
        case InputKeyRight:
            menu_process_right(menu);
            break;
        case InputKeyOk:
            menu_process_ok(menu);
            break;
        default:
            consumed = false;
            break;
        }
    } else if(event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyUp:
            menu_process_up(menu);
            break;
        case InputKeyDown:
            menu_process_down(menu);
            break;
        case InputKeyLeft:
            menu_process_left(menu);
            break;
        case InputKeyRight:
            menu_process_right(menu);
            break;
        default:
            consumed = false;
            break;
        }
    } else {
        consumed = false;
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

Menu* menu_alloc() {
    return menu_pos_alloc(0);
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
            if(index < MenuItemArray_size(model->items) && index != model->position) {
                model->scroll_counter = 0;

                MenuItem* item = MenuItemArray_get(model->items, model->position);
                if(item && item->icon) {
                    icon_animation_stop(item->icon);
                }

                item = MenuItemArray_get(model->items, index);
                if(item && item->icon) {
                    icon_animation_start(item->icon);
                }

                model->position = index;
            }
        },
        true);
}

static void menu_process_up(Menu* menu) {
    size_t position;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            position = model->position;
            size_t count = MenuItemArray_size(model->items);
            size_t vertical_offset = model->vertical_offset;

            switch(CFW_SETTINGS()->menu_style) {
            case MenuStyleList:
            case MenuStyleEurocorp:
                if(position > 0) {
                    position--;
                    if(vertical_offset && vertical_offset == position) {
                        vertical_offset--;
                    }
                } else {
                    position = count - 1;
                    vertical_offset = count - 8;
                }
                break;
            case MenuStyleWii:
                if(position % 2 || (position == count - 1 && count % 2)) {
                    position--;
                } else {
                    position++;
                }
                vertical_offset = CLAMP(MAX((int)position - 4, 0), MAX((int)count - 8, 0), 0);
                break;
            case MenuStyleC64:
                if(position > 0) {
                    position--;
                } else {
                    position = count - 1;
                }
                break;
            default:
                break;
            }

            model->vertical_offset = vertical_offset;
        },
        false);
    menu_set_selected_item(menu, position);
}

static void menu_process_down(Menu* menu) {
    size_t position;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            position = model->position;
            size_t count = MenuItemArray_size(model->items);
            size_t vertical_offset = model->vertical_offset;

            switch(CFW_SETTINGS()->menu_style) {
            case MenuStyleList:
            case MenuStyleEurocorp:
                if(position < count - 1) {
                    position++;
                    if(vertical_offset < count - 8 && vertical_offset == position - 7) {
                        vertical_offset++;
                    }
                } else {
                    position = 0;
                    vertical_offset = 0;
                }
                break;
            case MenuStyleWii:
                if(position % 2 || (position == count - 1 && count % 2)) {
                    position--;
                } else {
                    position++;
                }
                vertical_offset = CLAMP(MAX((int)position - 4, 0), MAX((int)count - 8, 0), 0);
                break;
            case MenuStyleC64:
                if(position < count - 1) {
                    position++;
                } else {
                    position = 0;
                }
                break;
            default:
                break;
            }

            model->vertical_offset = vertical_offset;
        },
        false);
    menu_set_selected_item(menu, position);
}

static void menu_process_left(Menu* menu) {
    size_t position;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            position = model->position;
            size_t count = MenuItemArray_size(model->items);
            size_t vertical_offset = model->vertical_offset;

            switch(CFW_SETTINGS()->menu_style) {
            case MenuStyleWii:
                if(position < 2) {
                    if(count % 2) {
                        position = count - 1;
                    } else {
                        position = count - 2 + position % 2;
                    }
                } else {
                    position -= 2;
                }
                vertical_offset = CLAMP(MAX((int)position - 4, 0), MAX((int)count - 8, 0), 0);
                break;
            case MenuStyleDsi:
            case MenuStylePs4:
            case MenuStyleVertical:
                if(position > 0) {
                    position--;
                    if(vertical_offset && vertical_offset == position) {
                        vertical_offset--;
                    }
                } else {
                    position = count - 1;
                    vertical_offset = count - 8;
                }
                break;
            case MenuStyleC64:
                if((position % 10) < 5) {
                    position = position + 5;
                } else if((position % 10) >= 5) {
                    position = position - 5;
                }
            default:
                break;
            }

            model->vertical_offset = vertical_offset;
        },
        false);
    menu_set_selected_item(menu, position);
}

static void menu_process_right(Menu* menu) {
    size_t position;
    with_view_model(
        menu->view,
        MenuModel * model,
        {
            position = model->position;
            size_t count = MenuItemArray_size(model->items);
            size_t vertical_offset = model->vertical_offset;

            switch(CFW_SETTINGS()->menu_style) {
            case MenuStyleWii:
                if(count % 2) {
                    if(position == count - 1) {
                        position = 0;
                    } else if(position == count - 2) {
                        position = count - 1;
                    } else {
                        position += 2;
                    }
                } else {
                    position += 2;
                    if(position >= count) {
                        position = position % 2;
                    }
                }
                vertical_offset = CLAMP(MAX((int)position - 4, 0), MAX((int)count - 8, 0), 0);
                break;
            case MenuStyleDsi:
            case MenuStylePs4:
            case MenuStyleVertical:
                if(position < count - 1) {
                    position++;
                    if(vertical_offset < count - 8 && vertical_offset == position - 7) {
                        vertical_offset++;
                    }
                } else {
                    position = 0;
                    vertical_offset = 0;
                }
                break;
            case MenuStyleC64:
                if(position >= (count - count) && (position % 10) < 5) {
                    position = position + 5;
                } else if((position % 10) >= 5 && position < count) {
                    position = position - 5;
                }
            default:
                break;
            }

            model->vertical_offset = vertical_offset;
        },
        false);
    menu_set_selected_item(menu, position);
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