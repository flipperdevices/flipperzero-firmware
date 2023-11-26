#include "hid_ptt_menu.h"
#include "hid_ptt.h"
#include <gui/elements.h>
#include <m-array.h>
#include "../hid.h"
#include "../views.h"

#define TAG "HidPushToTalkMenu"

struct HidPushToTalkMenu {
    View* view;
    Hid* hid;
};

typedef struct {
    FuriString* label;
    uint32_t index;
    PushToTalkMenuItemCallback callback;
    void* callback_context;
} PushToTalkMenuItem;

// Menu item
static void PushToTalkMenuItem_init(PushToTalkMenuItem* item) {
    item->label = furi_string_alloc();
    item->index = 0;
}

static void PushToTalkMenuItem_init_set(PushToTalkMenuItem* item, const PushToTalkMenuItem* src) {
    item->label = furi_string_alloc_set(src->label);
    item->index = src->index;
}

static void PushToTalkMenuItem_set(PushToTalkMenuItem* item, const PushToTalkMenuItem* src) {
    furi_string_set(item->label, src->label);
    item->index = src->index;
}

static void PushToTalkMenuItem_clear(PushToTalkMenuItem* item) {
    furi_string_free(item->label);
}

ARRAY_DEF(
    PushToTalkMenuItemArray,
    PushToTalkMenuItem,
    (INIT(API_2(PushToTalkMenuItem_init)),
     SET(API_6(PushToTalkMenuItem_set)),
     INIT_SET(API_6(PushToTalkMenuItem_init_set)),
     CLEAR(API_2(PushToTalkMenuItem_clear))))

// Menu list (horisontal, 2d array)
typedef struct {
    FuriString* label;
    uint32_t index;
    PushToTalkMenuItemArray_t items;
} PushToTalkMenuList;

typedef struct {
    size_t list_position;
    size_t position;
    size_t window_position;
    PushToTalkMenuList *lists;
    int lists_count;
} HidPushToTalkMenuModel;

static void hid_ptt_menu_draw_list(Canvas* canvas, void* context, const PushToTalkMenuItemArray_t items) {
    furi_assert(context);
    HidPushToTalkMenuModel* model = context;
    const uint8_t item_height = 16;
    uint8_t item_width = canvas_width(canvas) - 5;

    canvas_set_font(canvas, FontSecondary);
    size_t position = 0;
    PushToTalkMenuItemArray_it_t it;
    for(PushToTalkMenuItemArray_it(it, items); !PushToTalkMenuItemArray_end_p(it);
        PushToTalkMenuItemArray_next(it)) {
        const size_t item_position = position - model->window_position;
        const size_t items_on_screen = 3;
        uint8_t y_offset = 16;

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

            FuriString* disp_str;
            disp_str = furi_string_alloc_set(PushToTalkMenuItemArray_cref(it)->label);
            elements_string_fit_width(canvas, disp_str, item_width - (6 * 2));

            canvas_draw_str(
                canvas,
                6,
                y_offset + (item_position * item_height) + item_height - 4,
                furi_string_get_cstr(disp_str));

            furi_string_free(disp_str);
        }

        position++;
    }
    elements_scrollbar_pos(canvas, 128 , 17, 49, model->position, PushToTalkMenuItemArray_size(items));
}

PushToTalkMenuList * hid_ptt_menu_get_list_at_index(
    void* context,
    uint32_t index) {
    furi_assert(context);
    HidPushToTalkMenuModel* model = context;
    for (int i = 0; i < model->lists_count; i++) {
        PushToTalkMenuList* list = &model->lists[i];
        if(index == list->index) {
            return list;
        }
    }
    return NULL;
}

static void hid_ptt_menu_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidPushToTalkMenuModel* model = context;
    if (model->lists_count == 0){
        return;
    }
    uint8_t item_width = canvas_width(canvas) - 5;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, "<");
    canvas_draw_str(canvas, 121, 11, ">");

    PushToTalkMenuList* list = &model->lists[model->list_position];
    FuriString* disp_str;
    disp_str = furi_string_alloc_set(list->label);
    elements_string_fit_width(canvas, disp_str, item_width - (6 * 2));
    uint8_t x_pos = (canvas_width(canvas) - canvas_string_width(canvas,furi_string_get_cstr(disp_str))) / 2;
    canvas_draw_str(canvas,x_pos,11,furi_string_get_cstr(disp_str));
    furi_string_free(disp_str);
    canvas_set_font(canvas, FontSecondary);
    hid_ptt_menu_draw_list(
        canvas,
        context,
        list->items
    );
}

void ptt_menu_add_list(
    HidPushToTalkMenu* hid_ptt_menu,
    const char* label,
    uint32_t index) {
    furi_assert(label);
    furi_assert(hid_ptt_menu);
    FURI_LOG_I(TAG, "Adding list: %s [%li]", label, index);
    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model,
        {
            if (model->lists_count == 0) {
                FURI_LOG_I(TAG, "List array doesn't exist, creating");
                model->lists = (PushToTalkMenuList *)malloc(sizeof(PushToTalkMenuList));
            } else {
                FURI_LOG_I(TAG, "List array exists (%i), extending", model->lists_count);
                model->lists = (PushToTalkMenuList *)realloc(model->lists, (model->lists_count + 1) * sizeof(PushToTalkMenuList));
            }
            if (model->lists == NULL) {
                FURI_LOG_E(TAG, "Memory reallocation failed (%i)", model->lists_count);
                return;
            }
            PushToTalkMenuList* list = &model->lists[model->lists_count];
            PushToTalkMenuItemArray_init(list->items);
            list->label = furi_string_alloc_set(label);
            list->index = index;
            model->lists_count += 1;
        },
        true);
}


void ptt_menu_add_item_to_list(
    HidPushToTalkMenu* hid_ptt_menu,
    uint32_t list_index,
    const char* label,
    uint32_t index,
    PushToTalkMenuItemCallback callback,
    void* callback_context) {
    PushToTalkMenuItem* item = NULL;
    furi_assert(label);
    furi_assert(hid_ptt_menu);
    UNUSED(list_index);
    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model,
        {
            PushToTalkMenuList* list = hid_ptt_menu_get_list_at_index(model, list_index);
            if (list == NULL){
                FURI_LOG_E(TAG, "Adding item %s to unknown index %li", label, list_index);
                return;
            }
            item = PushToTalkMenuItemArray_push_new(list->items);
            furi_string_set_str(item->label, label);
            item->index = index;
            item->callback = callback;
            item->callback_context = callback_context;
        },
        true);
}

// // Supports only ±1
// static void hid_ptt_shift_app(HidPushToTalkMenuModel * model, int shift) {
//     UNUSED(model);
//     UNUSED(shift);
//     // int i = (short) model->appIndex;
//     // if (i + shift >= HidPttAppIndexSize) {
//     //     model->appIndex = 0;
//     // } else if(i + shift <= 0) {
//     //     model->appIndex = HidPttAppIndexSize - 1;
//     // } else {
//     //     model->appIndex += shift;
//     // }
//     // // Avoid showing facetime if not macos
//     // if (model->appIndex == HidPttAppIndexFaceTime && !model->is_mac_os) {
//     //     hid_ptt_shift_app(model, shift);
//     // }
// }

// void ptt_menu_shift_list(HidPushToTalkMenu* hid_ptt_menu, bool forward)
//     UNUSED(forward);
//     with_view_model(
//         hid_ptt_menu->view, HidPushToTalkMenuModel * model,
//         {
//             UNUSED(model);
//             // PushToTalkMenuList* list = &model->lists[model->list_position];
//         },
//         true);
// }

void ptt_menu_process_up(HidPushToTalkMenu* hid_ptt_menu) {
    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model,
        {
            PushToTalkMenuList* list = &model->lists[model->list_position];
            const size_t items_on_screen = 3;
            const size_t items_size = PushToTalkMenuItemArray_size(list->items);

            if(model->position > 0) {
                model->position--;
                if((model->position == model->window_position) && (model->window_position > 0)) {
                    model->window_position--;
                }
            } else {
                model->position = items_size - 1;
                if(model->position > items_on_screen - 1) {
                    model->window_position = model->position - (items_on_screen - 1);
                }
            }
        },
        true);
}

void ptt_menu_process_down(HidPushToTalkMenu* hid_ptt_menu) {
    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model,
        {
            PushToTalkMenuList* list = &model->lists[model->list_position];
            const size_t items_on_screen = 3;
            const size_t items_size = PushToTalkMenuItemArray_size(list->items);

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
        },
        true);
}

void ptt_menu_process_ok(HidPushToTalkMenu* hid_ptt_menu) {

    PushToTalkMenuList* list = NULL;
    
    PushToTalkMenuItem* item = NULL;

    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model,
        {
            list = &model->lists[model->list_position];
            const size_t items_size = PushToTalkMenuItemArray_size(list->items);
            if(model->position < items_size) {
                item = PushToTalkMenuItemArray_get(list->items, model->position);
            }
        },
        true);
    if(item && list && item->callback) {
        item->callback(item->callback_context, list->index, list->label, item->index, item->label);
    }
}

static bool hid_ptt_menu_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidPushToTalkMenu* hid_ptt_menu = context;
    bool consumed = false;
    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            ptt_menu_process_up(hid_ptt_menu);
            break;
        case InputKeyDown:
            consumed = true;
            ptt_menu_process_down(hid_ptt_menu);
            break;
        case InputKeyLeft:
            consumed = true;
            // ptt_menu_shift_list(hid_ptt_menu, false);
            break;
        case InputKeyRight:
            consumed = true;
            // ptt_menu_shift_list(hid_ptt_menu, true);
            break;
        case InputKeyOk:
            consumed = true;
            ptt_menu_process_ok(hid_ptt_menu);
            break;
        default:
            break;
        }
    } else if(event->type == InputTypeRepeat) {
        if(event->key == InputKeyUp) {
            consumed = true;
            ptt_menu_process_up(hid_ptt_menu);
        } else if(event->key == InputKeyDown) {
            consumed = true;
            ptt_menu_process_down(hid_ptt_menu);
        }
    }
    return consumed;
}

View* hid_ptt_menu_get_view(HidPushToTalkMenu* hid_ptt_menu) {
    furi_assert(hid_ptt_menu);
    return hid_ptt_menu->view;
}

HidPushToTalkMenu* hid_ptt_menu_alloc(Hid* hid) {
    HidPushToTalkMenu* hid_ptt_menu = malloc(sizeof(HidPushToTalkMenu));
    hid_ptt_menu->hid = hid;
    hid_ptt_menu->view = view_alloc();
    view_set_context(hid_ptt_menu->view, hid_ptt_menu);
    view_allocate_model(hid_ptt_menu->view, ViewModelTypeLocking, sizeof(HidPushToTalkMenuModel));
    view_set_draw_callback(hid_ptt_menu->view, hid_ptt_menu_draw_callback);
    view_set_input_callback(hid_ptt_menu->view, hid_ptt_menu_input_callback);

    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model, {
            model->lists_count = 0;
            model->position = 0;
            model->window_position = 0;
        }, true);
    return hid_ptt_menu;
}

void hid_ptt_menu_free(HidPushToTalkMenu* hid_ptt_menu) {
    furi_assert(hid_ptt_menu);
    with_view_model(
        hid_ptt_menu->view, HidPushToTalkMenuModel * model, {
            for (int i = 0; i < model->lists_count; i++) {
                PushToTalkMenuItemArray_clear(model->lists[i].items);
                furi_string_free(model->lists[i].label);
            }
            free(model->lists);
        }, true);
    view_free(hid_ptt_menu->view);
    free(hid_ptt_menu);
}
