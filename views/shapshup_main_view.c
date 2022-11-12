#include "shapshup_main_view.h"
#include "../shapshup_i.h"

#include <input/input.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon.h>
#include <m-array.h>

#define STATUS_BAR_Y_SHIFT 14
#define SUBGHZ_READ_RAW_RSSI_HISTORY_SIZE 100
#define TAG "ShapShupMainView"

typedef struct {
    FuriString* item_str;
    uint8_t type;
} ShapShupTextItem;

ARRAY_DEF(ShapShupTextItemArray, ShapShupTextItem, M_POD_OPLIST)

#define M_OPL_ShapShupTextItemArray_t() ARRAY_OPLIST(ShapShupTextItemArray, M_POD_OPLIST)

struct ShapShupText {
    ShapShupTextItemArray_t data;
};

typedef struct ShapShupText ShapShupText;

struct ShapShupMainView {
    View* view;
    ShapShupMainViewCallback callback;
    void* context;
};

typedef struct {
    uint8_t index;
    ShapShupText* items;
    bool rssi_history_end;
    uint8_t offset;
    uint8_t scale;
} ShapShupMainViewModel;

void shapshup_main_view_set_callback(
    ShapShupMainView* instance,
    ShapShupMainViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model) {
    furi_assert(model);
    ShapShupMainViewModel* m = model;
    furi_assert(m);

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_box(canvas, 0, 0, canvas_width(canvas), STATUS_BAR_Y_SHIFT);
    canvas_invert_color(canvas);
    canvas_draw_str_aligned(canvas, 64, 3, AlignCenter, AlignTop, "shapshup");
    canvas_invert_color(canvas);

    size_t size = ShapShupTextItemArray_size(m->items->data);
    const uint8_t line_height = 10;

    if(size > 0) {
        for(size_t item_position = 0; item_position < size; item_position++) {
            ShapShupTextItem* current = ShapShupTextItemArray_get(m->items->data, item_position);
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

    ShapShupMainView* instance = context;
    uint8_t index = 0;

    with_view_model(
        instance->view, ShapShupMainViewModel * model, { index = model->index; }, false);

    furi_assert(index != 100);
    if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
#ifdef FURI_DEBUG
        FURI_LOG_D(TAG, "Index: %d", index);
#endif
        with_view_model(
            instance->view, ShapShupMainViewModel * model, { model->index = index; }, true);

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

ShapShupMainView* shapshup_main_view_alloc() {
    ShapShupMainView* instance = malloc(sizeof(ShapShupMainView));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ShapShupMainViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)shapshup_main_view_draw);
    view_set_input_callback(instance->view, shapshup_main_view_input);
    view_set_enter_callback(instance->view, shapshup_main_view_enter);
    view_set_exit_callback(instance->view, shapshup_main_view_exit);

    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
            model->index = 0;
            model->items = malloc(sizeof(ShapShupText));
            model->offset = 0;
            model->scale = 0;
            ShapShupTextItemArray_init(model->items->data);
        },
        true);

    return instance;
}

void shapshup_main_view_free(ShapShupMainView* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
                for
                    M_EACH(item_menu, model->items->data, ShapShupTextItemArray_t) {
                        furi_string_free(item_menu->item_str);
                        item_menu->type = 0;
                    }
                ShapShupTextItemArray_clear(model->items->data);
                free(model->items);
        },
        true);

    view_free(instance->view);
    free(instance);
}

View* shapshup_main_view_get_view(ShapShupMainView* instance) {
    furi_assert(instance);
    return instance->view;
}

void shapshup_main_view_set_index(ShapShupMainView* instance, uint8_t idx) {
    furi_assert(instance);
    with_view_model(
        instance->view, ShapShupMainViewModel * model, { model->index = idx; }, true);
}

void shapshup_main_view_add_item(ShapShupMainView* instance, const char* name, uint8_t type) {
    furi_assert(instance);
    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
            ShapShupTextItem* item_menu = ShapShupTextItemArray_push_raw(model->items->data);
            item_menu->item_str = furi_string_alloc_printf("%s", name);
            item_menu->type = type;
            model->index++;
        },
        true);
}

uint8_t shapshup_main_view_get_index(ShapShupMainView* instance) {
    furi_assert(instance);

    uint8_t idx = 0;
    with_view_model(
        instance->view, ShapShupMainViewModel * model, { idx = model->index; }, false);

    return idx;
}

void subghz_read_raw_draw_scale(Canvas* canvas, SubGhzReadRAWModel* model) {
#define SUBGHZ_RAW_TOP_SCALE 14
#define SUBGHZ_RAW_END_SCALE 115

    if(model->rssi_history_end == false) {
        for(int i = SUBGHZ_RAW_END_SCALE; i > 0; i -= 15) {
            canvas_draw_line(canvas, i, SUBGHZ_RAW_TOP_SCALE, i, SUBGHZ_RAW_TOP_SCALE + 4);
            canvas_draw_line(canvas, i - 5, SUBGHZ_RAW_TOP_SCALE, i - 5, SUBGHZ_RAW_TOP_SCALE + 2);
            canvas_draw_line(
                canvas, i - 10, SUBGHZ_RAW_TOP_SCALE, i - 10, SUBGHZ_RAW_TOP_SCALE + 2);
        }
    } else {
        for(int i = SUBGHZ_RAW_END_SCALE - model->ind_write % 15; i > -15; i -= 15) {
            canvas_draw_line(canvas, i, SUBGHZ_RAW_TOP_SCALE, i, SUBGHZ_RAW_TOP_SCALE + 4);
            if(SUBGHZ_RAW_END_SCALE > i + 5)
                canvas_draw_line(
                    canvas, i + 5, SUBGHZ_RAW_TOP_SCALE, i + 5, SUBGHZ_RAW_TOP_SCALE + 2);
            if(SUBGHZ_RAW_END_SCALE > i + 10)
                canvas_draw_line(
                    canvas, i + 10, SUBGHZ_RAW_TOP_SCALE, i + 10, SUBGHZ_RAW_TOP_SCALE + 2);
        }
    }
}