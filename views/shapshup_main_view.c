#include "shapshup_main_view_private.h"
#include "shapshup_main_view.h"
#include "../shapshup_i.h"
#include <input/input.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon.h>
#include <m-array.h>

#define TAG "ShapShupMainView"

void shapshup_main_view_set_callback(
    ShapShupMainView* instance,
    ShapShupMainViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

uint32_t calc_offset_per_page(uint32_t total, float scale) {
    return (uint32_t)(total * scale);
}

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model) {
    furi_assert(model);

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_box(canvas, 0, 0, canvas_width(canvas), STATUS_BAR_Y_SHIFT);
    canvas_invert_color(canvas);
    canvas_draw_str_aligned(canvas, 64, 3, AlignCenter, AlignTop, "Shap-Shup");
    canvas_invert_color(canvas);

    shapshup_main_view_draw_scale(canvas, model);

    //    size_t size = ShapShupTextItemArray_size(m->items->data);
    //    const uint8_t line_height = 10;
    //
    //    if(size > 0) {
    //        for(size_t item_position = 0; item_position < size; item_position++) {
    //            // ShapShupTextItem* current = ShapShupTextItemArray_get(m->items->data, item_position);
    //            // canvas_draw_str_aligned(
    //            //     canvas,
    //            //     4,
    //            //     9 + (item_position * line_height) + STATUS_BAR_Y_SHIFT,
    //            //     AlignLeft,
    //            //     AlignCenter,
    //            //     furi_string_get_cstr(current->item_str));
    //        }
    //    }
}

bool shapshup_main_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    bool consumed = false;

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        return consumed;
    }

    ShapShupMainView* instance = context;
    bool is_clicked = (event->type == InputTypeShort) || (event->type == InputTypeRepeat);

    if(is_clicked) {
        uint32_t offset_per_page =
            calc_offset_per_page(instance->raw_file->total_len, instance->scale);
        if(event->key == InputKeyOk) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "InputKeyOk");
#endif
            instance->callback(ShapShupCustomEventTypeLoadFile, instance->context);
        } else if(event->key == InputKeyLeft && instance->offset > 0) {
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG,
                "L offset_per_page: %ld, offset: %ld, total_len: %lld",
                offset_per_page,
                instance->offset,
                instance->raw_file->total_len);
#endif
            instance->offset = instance->offset < offset_per_page / 2 ?
                                   0 :
                                   instance->offset - offset_per_page / 2;

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->offset = instance->offset;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyRight) {
            uint32_t calc = instance->offset + offset_per_page / 2;
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG,
                "R offset_per_page: %ld, calc: %ld, total_len: %lld",
                offset_per_page,
                calc,
                instance->raw_file->total_len);
#endif
            if(calc < instance->raw_file->total_len) {
                instance->offset = calc;
            } else if(
                calc > instance->raw_file->total_len &&
                instance->raw_file->total_len - calc < offset_per_page) {
                instance->offset = calc;
            }

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->offset = instance->offset;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyDown && 1.0 - instance->scale > 0.01f) {
            instance->scale -= SCALE_STEP;
            if(instance->scale < SCALE_STEP) {
                instance->scale = 0.1f;
            }

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                { model->scale = instance->scale; },
                true);
        } else if(event->key == InputKeyUp) {
            if(instance->scale < SCALE_STEP) {
                instance->scale = SCALE_STEP;
            } else if(1.0 - instance->scale > 0.01f) {
                instance->scale += SCALE_STEP;
            }

            if(instance->scale > 1.0) {
                instance->scale = 1.0;
            }

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                { model->scale = instance->scale; },
                true);
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
    instance->raw_file = NULL;
    instance->offset = 0;
    instance->scale = 1.0;

    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
            model->raw_file = NULL;
            model->offset = 0;
            model->scale = 1.0;
        },
        true);

    return instance;
}

void shapshup_main_view_free(ShapShupMainView* instance) {
    furi_assert(instance);

    // Clean array and struct
    clean_raw_values(instance->raw_file);

    view_free(instance->view);
    free(instance);
}

View* shapshup_main_view_get_view(ShapShupMainView* instance) {
    furi_assert(instance);
    return instance->view;
}

ShapShupFileResults shapshup_main_view_load_file(ShapShupMainView* instance, const char* name) {
    furi_assert(instance);
    instance->raw_file = load_file_shapshup(name);

    if(instance->raw_file->result == ShapShupFileResultOk) {
        with_view_model(
            instance->view,
            ShapShupMainViewModel * model,
            {
                model->raw_file = instance->raw_file;
                model->offset = 0;
                model->scale = 1.0;
            },
            true);
    }

    return instance->raw_file->result;
}

void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model) {
    char buffer[64];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontBatteryPercent);

    for(int i = SUBGHZ_RAW_END_SCALE; i > SUBGHZ_RAW_START_SCALE; i -= 15) {
        canvas_draw_line(canvas, i, SUBGHZ_RAW_TOP_SCALE, i, SUBGHZ_RAW_TOP_SCALE + 4);
        canvas_draw_line(canvas, i - 5, SUBGHZ_RAW_TOP_SCALE, i - 5, SUBGHZ_RAW_TOP_SCALE + 2);
        canvas_draw_line(canvas, i - 10, SUBGHZ_RAW_TOP_SCALE, i - 10, SUBGHZ_RAW_TOP_SCALE + 2);
    }

    snprintf(buffer, sizeof(buffer), "%ld", model->offset);
    canvas_draw_str(canvas, SUBGHZ_RAW_START_SCALE, SUBGHZ_RAW_TOP_SCALE + 5, buffer);

    snprintf(buffer, sizeof(buffer), "%ld", model->offset + model->offset_per_page);
    canvas_draw_str(canvas, SUBGHZ_RAW_END_SCALE, SUBGHZ_RAW_TOP_SCALE + 5, buffer);

    /*} else {
        for(int i = SUBGHZ_RAW_END_SCALE - model->ind_write % 15; i > -15; i -= 15) {
            canvas_draw_line(canvas, i, SUBGHZ_RAW_TOP_SCALE, i, SUBGHZ_RAW_TOP_SCALE + 4);
            if(SUBGHZ_RAW_END_SCALE > i + 5)
                canvas_draw_line(
                    canvas, i + 5, SUBGHZ_RAW_TOP_SCALE, i + 5, SUBGHZ_RAW_TOP_SCALE + 2);
            if(SUBGHZ_RAW_END_SCALE > i + 10)
                canvas_draw_line(
                    canvas, i + 10, SUBGHZ_RAW_TOP_SCALE, i + 10, SUBGHZ_RAW_TOP_SCALE + 2);
        }
    }*/
}

void elements_button_top_left(Canvas* canvas, const char* str) {
    const Icon* icon = &I_ButtonUp_7x4;

    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 3;
    const uint8_t horizontal_offset = 3;
    const uint8_t string_width = canvas_string_width(canvas, str);
    const uint8_t icon_h_offset = 3;
    const uint8_t icon_width_with_offset = icon_get_width(icon) + icon_h_offset;
    const uint8_t icon_v_offset = icon_get_height(icon) + vertical_offset;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    const uint8_t x = 0;
    const uint8_t y = 0 + button_height;

    uint8_t line_x = x + button_width;
    uint8_t line_y = y - button_height;
    canvas_draw_box(canvas, x, line_y, button_width, button_height);
    canvas_draw_line(canvas, line_x + 0, line_y, line_x + 0, y - 1);
    canvas_draw_line(canvas, line_x + 1, line_y, line_x + 1, y - 2);
    canvas_draw_line(canvas, line_x + 2, line_y, line_x + 2, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_icon(canvas, x + horizontal_offset, y - icon_v_offset, icon);
    canvas_draw_str(
        canvas, x + horizontal_offset + icon_width_with_offset, y - vertical_offset, str);
    canvas_invert_color(canvas);
}

void elements_button_top_right(Canvas* canvas, const char* str) {
    const Icon* icon = &I_ButtonDown_7x4;

    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 3;
    const uint8_t horizontal_offset = 3;
    const uint8_t string_width = canvas_string_width(canvas, str);
    const uint8_t icon_h_offset = 3;
    const uint8_t icon_width_with_offset = icon_get_width(icon) + icon_h_offset;
    const uint8_t icon_v_offset = icon_get_height(icon) + vertical_offset + 1;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    const uint8_t x = canvas_width(canvas);
    const uint8_t y = 0 + button_height;

    uint8_t line_x = x - button_width;
    uint8_t line_y = y - button_height;
    canvas_draw_box(canvas, line_x, line_y, button_width, button_height);
    canvas_draw_line(canvas, line_x - 1, line_y, line_x - 1, y - 1);
    canvas_draw_line(canvas, line_x - 2, line_y, line_x - 2, y - 2);
    canvas_draw_line(canvas, line_x - 3, line_y, line_x - 3, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_str(canvas, x - button_width + horizontal_offset, y - vertical_offset, str);
    canvas_draw_icon(
        canvas, x - horizontal_offset - icon_get_width(icon), y - icon_v_offset, icon);
    canvas_invert_color(canvas);
}

bool shapshup_main_view_no_file(ShapShupMainView* instance) {
    return instance->raw_file == NULL;
}