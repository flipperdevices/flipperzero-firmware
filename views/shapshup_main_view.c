#include "shapshup_main_view.h"
#include "../shapshup_i.h"

#include <input/input.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon.h>
#include <m-array.h>

#define STATUS_BAR_Y_SHIFT 14
#define TAG "ShapShupMainView"

typedef struct {
    FuriString* item_str;
    uint8_t type;
} shapshupTextItem;

ARRAY_DEF(shapshupTextItemArray, shapshupTextItem, M_POD_OPLIST)

#define M_OPL_shapshupTextItemArray_t() ARRAY_OPLIST(shapshupTextItemArray, M_POD_OPLIST)

struct shapshupText {
    shapshupTextItemArray_t data;
};

typedef struct shapshupText shapshupText;

struct shapshupMainView {
    View* view;
    shapshupMainViewCallback callback;
    void* context;
};

typedef struct {
    uint8_t index;
    shapshupText* items;
} shapshupMainViewModel;

void shapshup_main_view_set_callback(
    shapshupMainView* instance,
    shapshupMainViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void shapshup_main_view_draw(Canvas* canvas, shapshupMainViewModel* model) {
    furi_assert(model);
    shapshupMainViewModel* m = model;
    furi_assert(m);

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_box(canvas, 0, 0, canvas_width(canvas), STATUS_BAR_Y_SHIFT);
    canvas_invert_color(canvas);
    canvas_draw_str_aligned(canvas, 64, 3, AlignCenter, AlignTop, "shapshup");
    canvas_invert_color(canvas);

    size_t size = shapshupTextItemArray_size(m->items->data);
    const uint8_t line_height = 10;

    if(size > 0) {
        for(size_t item_position = 0; item_position < size; item_position++) {
            shapshupTextItem* current =
                shapshupTextItemArray_get(m->items->data, item_position);
            canvas_draw_str_aligned(
                canvas,
                4,
                9 + (item_position * line_height) + STATUS_BAR_Y_SHIFT,
                AlignLeft,
                AlignCenter,
                furi_string_get_cstr(current->item_str));
        }
    }
}

bool shapshup_main_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "InputKey: %d", event->key);
#endif
    bool consumed = false;

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        return consumed;
    }

    shapshupMainView* instance = context;
    uint8_t index = 0;

    with_view_model(
        instance->view, shapshupMainViewModel * model, { index = model->index; }, false);

    furi_assert(index != 100);
    if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
#ifdef FURI_DEBUG
        FURI_LOG_D(TAG, "Index: %d", index);
#endif
        with_view_model(
            instance->view,
            shapshupMainViewModel * model,
            {
                model->index = index;
            },
            true);

        if(event->key == InputKeyOk && event->type == InputTypeShort) {
            if(instance->callback == NULL || instance->context == NULL) {
                FURI_LOG_W(TAG, "Invalid call of callback!");
            } else {
                if(index == 255) {
                    instance->callback(shapshupCustomEventTypeLoadFile, instance->context);
                } else {
                    instance->callback(shapshupCustomEventTypeMenuSelected, instance->context);
                }
            }
        }

        consumed = true;
    }

    return consumed;
}

void shapshup_main_view_enter(void* context) {
    furi_assert(context);
}

void shapshup_main_view_exit(void* context) {
    furi_assert(context);
}

shapshupMainView* shapshup_main_view_alloc() {
    shapshupMainView* instance = malloc(sizeof(shapshupMainView));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(shapshupMainViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)shapshup_main_view_draw);
    view_set_input_callback(instance->view, shapshup_main_view_input);
    view_set_enter_callback(instance->view, shapshup_main_view_enter);
    view_set_exit_callback(instance->view, shapshup_main_view_exit);

    with_view_model(
        instance->view,
        shapshupMainViewModel * model,
        {
            model->index = 0;
            model->items = malloc(sizeof(shapshupText));
            shapshupTextItemArray_init(model->items->data);
        },
        true);

    return instance;
}

void shapshup_main_view_free(shapshupMainView* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        shapshupMainViewModel * model,
        {
                for
                    M_EACH(item_menu, model->items->data, shapshupTextItemArray_t) {
                        furi_string_free(item_menu->item_str);
                        item_menu->type = 0;
                    }
                shapshupTextItemArray_clear(model->items->data);
                free(model->items);
        },
        true);

    view_free(instance->view);
    free(instance);
}

View* shapshup_main_view_get_view(shapshupMainView* instance) {
    furi_assert(instance);
    return instance->view;
}

void shapshup_main_view_set_index(shapshupMainView* instance, uint8_t idx) {
    furi_assert(instance);
    with_view_model(
        instance->view, shapshupMainViewModel * model, { model->index = idx; }, true);
}

void shapshup_main_view_add_item(shapshupMainView* instance, const char* name, uint8_t type) {
    furi_assert(instance);
    with_view_model(
        instance->view,
        shapshupMainViewModel * model,
        {
            shapshupTextItem* item_menu = shapshupTextItemArray_push_raw(model->items->data);
            item_menu->item_str = furi_string_alloc_printf("%s", name);
            item_menu->type = type;
            model->index++;
        },
        true);
}

uint8_t shapshup_main_view_get_index(shapshupMainView* instance) {
    furi_assert(instance);

    uint8_t idx = 0;
    with_view_model(
        instance->view, shapshupMainViewModel * model, { idx = model->index; }, false);

    return idx;
}