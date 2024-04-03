#include "action_menu.h"

#include <gui/canvas.h>
#include <gui/elements.h>
#include <input/input.h>

#include <furi.h>

#include <stdint.h>
#include <m-array.h>

#include "quac_icons.h"

#define ITEM_FIRST_OFFSET 17
#define ITEM_NEXT_OFFSET 4
#define ITEM_HEIGHT 14
#define ITEM_WIDTH 64
#define BUTTONS_PER_SCREEN 6

#define ITEMS_PER_SCREEN_LANDSCAPE 3
#define ITEMS_PER_SCREEN_PORTRAIT 6

static const Icon* ActionMenuIcons[] = {
    [ActionMenuItemTypeSubGHz] = &I_SubGHz_10px,
    [ActionMenuItemTypeRFID] = &I_RFID_10px,
    [ActionMenuItemTypeIR] = &I_IR_10px,
    [ActionMenuItemTypeNFC] = &I_NFC_10px,
    [ActionMenuItemTypePlaylist] = &I_Playlist_10px,
    [ActionMenuItemTypeGroup] = &I_Directory_10px,
    [ActionMenuItemTypeSettings] = &I_Settings_10px,
    [ActionMenuItemTypeUnknown] = &I_Unknown_10px,
};

struct ActionMenuItem {
    const char* label;
    uint32_t index;
    ActionMenuItemCallback callback;
    ActionMenuItemType type;
    void* callback_context;
};

ARRAY_DEF(ActionMenuItemArray, ActionMenuItem, M_POD_OPLIST);
#define M_OPL_ActionMenuItemArray_t() ARRAY_OPLIST(ActionMenuItemArray, M_POD_OPLIST)

struct ActionMenu {
    View* view;
};

typedef struct {
    ActionMenuItemArray_t items;
    size_t position;
    size_t window_position;
    FuriString* header;
    ActionMenuLayout layout;
    bool show_icons;
    bool show_headers;
} ActionMenuModel;

static void action_menu_draw_landscape(Canvas* canvas, ActionMenuModel* model) {
    const uint8_t item_height = 16;
    uint8_t item_width = canvas_width(canvas) - 5; // space for scrollbar

    const bool have_header = furi_string_size(model->header) && model->show_headers;

    canvas_clear(canvas);
    if(have_header) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 11, furi_string_get_cstr(model->header));
    }
    canvas_set_font(canvas, FontSecondary);

    size_t position = 0;
    const size_t items_on_screen = ITEMS_PER_SCREEN_LANDSCAPE + (have_header ? 0 : 1);
    uint8_t y_offset = have_header ? 16 : 0;
    const size_t x_txt_start = model->show_icons ? 18 : 4;

    ActionMenuItemArray_it_t it;
    for(ActionMenuItemArray_it(it, model->items); !ActionMenuItemArray_end_p(it);
        ActionMenuItemArray_next(it)) {
        const size_t item_position = position - model->window_position;

        if(item_position < items_on_screen) {
            if(position == model->position) {
                canvas_set_color(canvas, ColorBlack);
                elements_slightly_rounded_box(
                    canvas,
                    0,
                    y_offset + (item_position * item_height) + 1,
                    item_width,
                    item_height - 2);
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            const ActionMenuItem* item = ActionMenuItemArray_cref(it);
            if(model->show_icons) {
                canvas_draw_icon(
                    canvas,
                    4,
                    y_offset + (item_position * item_height) + 3,
                    ActionMenuIcons[item->type]);
            }

            FuriString* disp_str;
            disp_str = furi_string_alloc_set(item->label);
            elements_string_fit_width(canvas, disp_str, item_width - (6 * 2));

            canvas_draw_str(
                canvas,
                x_txt_start, // 6
                y_offset + (item_position * item_height) + item_height - 4,
                furi_string_get_cstr(disp_str));
            furi_string_free(disp_str);
        }
        position++;
    }

    elements_scrollbar(canvas, model->position, ActionMenuItemArray_size(model->items));
}

static void action_menu_draw_portrait(Canvas* canvas, ActionMenuModel* model) {
    const bool have_header = furi_string_size(model->header) && model->show_headers;
    const size_t items_per_screen = have_header ? ITEMS_PER_SCREEN_PORTRAIT :
                                                  ITEMS_PER_SCREEN_PORTRAIT + 1;
    const size_t active_screen = model->position / items_per_screen;
    const size_t items_size = ActionMenuItemArray_size(model->items);
    const size_t max_screen = items_size ? (items_size - 1) / items_per_screen : 0;

    canvas_clear(canvas);

    // Draw up/down arrows, as needed
    if(active_screen > 0) {
        canvas_draw_icon(canvas, 28, 1, &I_ArrowUp_8x4);
    }
    if(max_screen > active_screen) {
        canvas_draw_icon(canvas, 28, 123, &I_ArrowDown_8x4);
    }

    if(have_header) {
        canvas_set_font(canvas, FontPrimary);
        elements_string_fit_width(canvas, model->header, ITEM_WIDTH - 6);
        canvas_draw_str_aligned(
            canvas, 32, 10, AlignCenter, AlignCenter, furi_string_get_cstr(model->header));
    }
    canvas_set_font(canvas, FontSecondary);

    size_t item_position = 0;
    const size_t x_txt_start = model->show_icons ? 16 : 4;
    const size_t y_offset = have_header ? ITEM_FIRST_OFFSET : 6;
    const size_t item_next_offset = have_header ? ITEM_NEXT_OFFSET : ITEM_NEXT_OFFSET - 1;

    ActionMenuItemArray_it_t it;
    for(ActionMenuItemArray_it(it, model->items); !ActionMenuItemArray_end_p(it);
        ActionMenuItemArray_next(it), ++item_position) {
        if(active_screen == (item_position / items_per_screen)) {
            uint8_t position_offset = item_position % items_per_screen;
            bool selected = item_position == model->position;

            // draw the item
            uint8_t item_y = y_offset + (position_offset * (ITEM_HEIGHT + item_next_offset));

            canvas_set_color(canvas, ColorBlack);

            if(selected) {
                // Same as elements_slightly_rounded_box with radius of 5
                canvas_draw_rbox(canvas, 0, item_y, ITEM_WIDTH, ITEM_HEIGHT, 1);
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_draw_rframe(canvas, 0, item_y, ITEM_WIDTH, ITEM_HEIGHT, 1);
            }

            const ActionMenuItem* item = ActionMenuItemArray_cref(it);
            if(model->show_icons) {
                canvas_draw_icon(canvas, 3, item_y + 2, ActionMenuIcons[item->type]);
            }

            FuriString* disp_str;
            disp_str = furi_string_alloc_set(item->label);
            elements_string_fit_width(canvas, disp_str, ITEM_WIDTH - 6);

            canvas_draw_str(
                canvas,
                x_txt_start,
                item_y + (ITEM_HEIGHT / 2) + 3,
                furi_string_get_cstr(disp_str));
            furi_string_free(disp_str);
        }
    }
}

static void action_menu_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(canvas);
    ActionMenuModel* model = (ActionMenuModel*)context;

    if(model->layout == ActionMenuLayoutLandscape) {
        action_menu_draw_landscape(canvas, model);
    } else {
        action_menu_draw_portrait(canvas, model);
    }
}

static void action_menu_process_up(ActionMenu* action_menu) {
    furi_assert(action_menu);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            const size_t items_size = ActionMenuItemArray_size(model->items);
            if(model->layout == ActionMenuLayoutPortrait) {
                if(model->position > 0) {
                    model->position--;
                } else {
                    model->position = items_size - 1;
                }
            } else {
                const size_t items_on_screen = furi_string_empty(model->header) ? 4 : 3;
                if(model->position > 0) {
                    model->position--;
                    if((model->position == model->window_position) &&
                       (model->window_position > 0)) {
                        model->window_position--;
                    }
                } else {
                    model->position = items_size - 1;
                    if(model->position > items_on_screen - 1) {
                        model->window_position = model->position - (items_on_screen - 1);
                    }
                }
            }
        },
        true);
}

static void action_menu_process_down(ActionMenu* action_menu) {
    furi_assert(action_menu);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            const size_t items_size = ActionMenuItemArray_size(model->items);
            if(model->layout == ActionMenuLayoutPortrait) {
                if(model->position < items_size - 1) {
                    model->position++;
                } else {
                    model->position = 0;
                }
            } else {
                const size_t items_on_screen = furi_string_empty(model->header) ? 4 : 3;
                if(model->position < items_size - 1) {
                    model->position++;
                    if((model->position - model->window_position > items_on_screen - 2) &&
                       (model->window_position < items_size - items_on_screen)) {
                        model->window_position++;
                    }
                } else {
                    model->position = 0;
                    model->window_position = 0;
                }
            }
        },
        true);
}

// Used for both the Short and Long presses of OK
static void action_menu_process_ok(ActionMenu* action_menu, InputType type) {
    furi_assert(action_menu);

    // FURI_LOG_I("AM", "OK pressed! %d: %s", type, input_get_type_name(type));
    ActionMenuItem* item = NULL;

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            if(model->position < (ActionMenuItemArray_size(model->items))) {
                item = ActionMenuItemArray_get(model->items, model->position);
                if(item->callback) {
                    item->callback(item->callback_context, item->index, type);
                }
            }
        },
        false);
}

static bool action_menu_view_input_callback(InputEvent* event, void* context) {
    furi_assert(event);

    ActionMenu* action_menu = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyOk:
            consumed = true;
            action_menu_process_ok(action_menu, event->type);
            break;
        case InputKeyUp:
            consumed = true;
            action_menu_process_up(action_menu);
            break;
        case InputKeyDown:
            consumed = true;
            action_menu_process_down(action_menu);
            break;
        case InputKeyLeft:
            break;
        case InputKeyRight:
            break;
        default:
            // FURI_LOG_E("AM", "Unknown key!");
            break;
        }
    } else if(event->type == InputTypeLong) {
        if(event->key == InputKeyRight) {
            consumed = true;
            action_menu_process_ok(action_menu, event->type);
        }
    }

    return consumed;
}

View* action_menu_get_view(ActionMenu* action_menu) {
    furi_assert(action_menu);
    return action_menu->view;
}

void action_menu_reset(ActionMenu* action_menu) {
    furi_assert(action_menu);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            // for
            //     M_EACH(item, model->items, ActionMenuItemArray_t) {
            //         icon_animation_stop(item->icon);
            //         icon_animation_free(item->icon);
            //     }
            ActionMenuItemArray_reset(model->items);
            model->position = 0;
            model->window_position = 0;
            furi_string_reset(model->header);
        },
        true);
}

void action_menu_set_layout(ActionMenu* action_menu, ActionMenuLayout layout) {
    furi_assert(action_menu);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            model->layout = layout;
            if(model->layout == ActionMenuLayoutLandscape) {
                view_set_orientation(action_menu->view, ViewOrientationHorizontal);
            } else {
                view_set_orientation(action_menu->view, ViewOrientationVertical);
            }
        },
        true);
}

void action_menu_set_header(ActionMenu* action_menu, const char* header) {
    furi_assert(action_menu);
    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            if(header == NULL) {
                furi_string_reset(model->header);
            } else {
                furi_string_set_str(model->header, header);
            }
        },
        true);
}

void action_menu_set_show_icons(ActionMenu* action_menu, bool show_icons) {
    with_view_model(
        action_menu->view, ActionMenuModel * model, { model->show_icons = show_icons; }, true);
}

void action_menu_set_show_headers(ActionMenu* action_menu, bool show_headers) {
    with_view_model(
        action_menu->view, ActionMenuModel * model, { model->show_headers = show_headers; }, true);
}

ActionMenuItem* action_menu_add_item(
    ActionMenu* action_menu,
    const char* label,
    int32_t index,
    ActionMenuItemCallback callback,
    ActionMenuItemType type,
    void* callback_context) {
    ActionMenuItem* item = NULL;
    furi_assert(label);
    furi_assert(action_menu);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            item = ActionMenuItemArray_push_new(model->items);
            item->label = label;
            // item->icon = icon ? icon_animation_alloc(icon) : NULL; // or default icon?
            // view_tie_icon_animation(action_menu->view, item->icon);
            item->index = index;
            item->type = type;
            item->callback = callback;
            item->callback_context = callback_context;
        },
        true);

    return item;
}

ActionMenu* action_menu_alloc(void) {
    ActionMenu* action_menu = malloc(sizeof(ActionMenu));
    action_menu->view = view_alloc();
    view_set_orientation(action_menu->view, ViewOrientationHorizontal);
    view_set_context(action_menu->view, action_menu);
    view_allocate_model(action_menu->view, ViewModelTypeLocking, sizeof(ActionMenuModel));
    view_set_draw_callback(action_menu->view, action_menu_view_draw_callback);
    view_set_input_callback(action_menu->view, action_menu_view_input_callback);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            ActionMenuItemArray_init(model->items);
            model->position = 0;
            model->window_position = 0;
            model->header = furi_string_alloc();
            model->layout = ActionMenuLayoutLandscape; // TODO: ehhhhhhhhhhhhhhhhhhh
            model->show_icons = true;
            model->show_headers = true;
        },
        true);

    return action_menu;
}

void action_menu_free(ActionMenu* action_menu) {
    furi_assert(action_menu);

    with_view_model(
        action_menu->view,
        ActionMenuModel * model,
        {
            // for
            //     M_EACH(item, model->items, ActionMenuItemArray_t) {
            //         icon_animation_stop(item->icon);
            //         icon_animation_free(item->icon);
            //     }
            ActionMenuItemArray_clear(model->items);
            furi_string_free(model->header);
        },
        true);
    view_free(action_menu->view);
    free(action_menu);
}

void action_menu_set_selected_item(ActionMenu* action_menu, uint32_t index) {
    furi_assert(action_menu);

    ActionMenuModel* m = view_get_model(action_menu->view);
    if(m->layout == ActionMenuLayoutPortrait) {
        with_view_model(
            action_menu->view,
            ActionMenuModel * model,
            {
                size_t item_position = 0;
                ActionMenuItemArray_it_t it;
                for(ActionMenuItemArray_it(it, model->items); !ActionMenuItemArray_end_p(it);
                    ActionMenuItemArray_next(it), ++item_position) {
                    if((uint32_t)ActionMenuItemArray_cref(it)->index == index) {
                        model->position = item_position;
                        break;
                    }
                }
            },
            true);
    } else {
        with_view_model(
            action_menu->view,
            ActionMenuModel * model,
            {
                size_t position = 0;
                ActionMenuItemArray_it_t it;
                for(ActionMenuItemArray_it(it, model->items); !ActionMenuItemArray_end_p(it);
                    ActionMenuItemArray_next(it)) {
                    if(index == ActionMenuItemArray_cref(it)->index) {
                        break;
                    }
                    position++;
                }
                const size_t items_size = ActionMenuItemArray_size(model->items);

                if(position >= items_size) {
                    position = 0;
                }

                model->position = position;
                model->window_position = position;

                if(model->window_position > 0) {
                    model->window_position -= 1;
                }

                const size_t items_on_screen = furi_string_empty(model->header) ? 4 : 3;

                if(items_size <= items_on_screen) {
                    model->window_position = 0;
                } else {
                    const size_t pos = items_size - items_on_screen;
                    if(model->window_position > pos) {
                        model->window_position = pos;
                    }
                }
            },
            true);
    }
}