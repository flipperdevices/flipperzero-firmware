#include "shapshup_main_view_private.h"
#include "shapshup_main_view.h"
#include "../shapshup_i.h"
#include "../helpers/gui_top_buttons.h"
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

uint64_t calc_offset_per_page(uint64_t total, float scale) {
    return (uint64_t)(total * scale);
}

void format_number(uint64_t n, char* out) {
    int c;
    char buf[32];
    char* p;

    snprintf(buf, sizeof(buf), "%lld", n);
    c = 2 - strlen(buf) % 3;
    for(p = buf; *p != 0; p++) {
        *out++ = *p;
        if(c == 1) {
            *out++ = ',';
        }
        c = (c + 1) % 3;
    }
    *--out = 0;
}

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model) {
    furi_assert(model);

    // Title
    if(model->raw_file != NULL) {
        char buffer[64];
        uint8_t width = canvas_width(canvas);
        canvas_draw_rbox(canvas, 0, 0, width, STATUS_BAR_Y_SHIFT, 2);
        canvas_invert_color(canvas);

        canvas_set_font(canvas, FontSecondary);
        snprintf(
            buffer,
            sizeof(buffer),
            "%03ld.%03ld MHz",
            model->raw_file->frequency / 1000000 % 1000,
            model->raw_file->frequency / 1000 % 1000);
        canvas_draw_str(canvas, 2, STATUS_BAR_INFO_SHIFT, buffer);

        memset(buffer, 0, sizeof(buffer));
        char format_buffer[32];
        format_number(model->raw_file->total_len, format_buffer);
        snprintf(buffer, sizeof(buffer), "%s us", format_buffer);
        FURI_LOG_D(TAG, "%s", buffer);

        uint16_t value_width = canvas_string_width(canvas, buffer);
        canvas_draw_str(canvas, width - value_width - 2, STATUS_BAR_INFO_SHIFT, buffer);
        canvas_invert_color(canvas);

        if(model->offset > 0) {
            elements_button_left(canvas, " ");
        }
        if(model->offset + model->offset_per_page < model->raw_file->total_len) {
            elements_button_right(canvas, " ");
        }
        elements_button_center(canvas, "Load");
        // elements_button_top_left(canvas, "Zoom out");
        // elements_button_top_right(canvas, "Zoom in");
    } else {
        // canvas_set_font(canvas, FontPrimary);
        // canvas_draw_box(canvas, 0, 0, canvas_width(canvas), STATUS_BAR_Y_SHIFT);
        // canvas_invert_color(canvas);
        // canvas_draw_str_aligned(canvas, 64, 3, AlignCenter, AlignTop, "Shap-Shup");
        // canvas_invert_color(canvas);
    }
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
        uint64_t offset_per_page =
            instance->raw_file != NULL ?
                calc_offset_per_page(instance->raw_file->total_len, instance->scale) :
                0;
        if(event->key == InputKeyOk) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "InputKeyOk");
#endif
            instance->callback(ShapShupCustomEventTypeLoadFile, instance->context);
        } else if(event->key == InputKeyLeft && instance->offset > 0) {
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG,
                "L offset_per_page: %lld, offset: %lld, total_len: %lld",
                offset_per_page,
                instance->offset,
                instance->raw_file->total_len);
#endif
            instance->offset =
                instance->offset < offset_per_page ? 0 : instance->offset - offset_per_page;

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->offset = instance->offset;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyRight) {
            uint64_t calc = instance->offset + offset_per_page;
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG,
                "R offset_per_page: %lld, calc: %lld, total_len: %lld",
                offset_per_page,
                calc,
                instance->raw_file->total_len);
#endif
            if(calc < instance->raw_file->total_len) {
                instance->offset = calc;
            } else if(
                calc > instance->raw_file->total_len &&
                instance->raw_file->total_len - calc < offset_per_page) {
                instance->offset = instance->offset > 0 ? calc : 0;
            }

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->offset = instance->offset;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyDown) {
            instance->scale -= SCALE_STEP;
            if(instance->scale < SCALE_STEP) {
                instance->scale = 0.1f;
            }
            offset_per_page =
                instance->raw_file != NULL ?
                    calc_offset_per_page(instance->raw_file->total_len, instance->scale) :
                    0;
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "Scale: %f", (double)instance->scale);
#endif

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->scale = instance->scale;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyUp) {
            if(instance->scale < SCALE_STEP) {
                instance->scale = SCALE_STEP;
            } else if(DEFAULT_SCALE_STEP - instance->scale > 0.01f) {
                instance->scale += SCALE_STEP;
            }

            if(instance->scale > DEFAULT_SCALE_STEP) {
                instance->scale = DEFAULT_SCALE_STEP;
            }
            offset_per_page =
                instance->raw_file != NULL ?
                    calc_offset_per_page(instance->raw_file->total_len, instance->scale) :
                    0;
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "Scale: %f", (double)instance->scale);
#endif

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->scale = instance->scale;
                    model->offset_per_page = offset_per_page;
                },
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
    instance->scale = DEFAULT_SCALE_STEP;

    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
            model->raw_file = NULL;
            model->offset = 0;
            model->offset_per_page = 0;
            model->scale = DEFAULT_SCALE_STEP;
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
    if(instance->raw_file != NULL) {
        free(instance->raw_file);
    }
    instance->raw_file = load_file_shapshup(name);

    if(instance->raw_file->result == ShapShupFileResultOk) {
        with_view_model(
            instance->view,
            ShapShupMainViewModel * model,
            {
                model->raw_file = instance->raw_file;
                model->offset = 0;
                if(model->raw_file != NULL) {
                    model->offset_per_page = model->raw_file->total_len;
                }
                model->scale = DEFAULT_SCALE_STEP;
            },
            true);
    }

    return instance->raw_file->result;
}

void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model) {
    char buffer[64];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontBatteryPercent);
    uint8_t width = canvas_width(canvas);

    // First and last scale pin
    canvas_draw_line(canvas, 1, SUBGHZ_RAW_TOP_SCALE, 1, SUBGHZ_RAW_TOP_SCALE + 4);
    canvas_draw_line(canvas, width - 1, SUBGHZ_RAW_TOP_SCALE, width - 1, SUBGHZ_RAW_TOP_SCALE + 4);
    for(int i = width; i > SUBGHZ_RAW_START_SCALE; i -= 16) {
        canvas_draw_line(canvas, i, SUBGHZ_RAW_TOP_SCALE, i, SUBGHZ_RAW_TOP_SCALE + 4);
        canvas_draw_line(canvas, i - 5, SUBGHZ_RAW_TOP_SCALE, i - 5, SUBGHZ_RAW_TOP_SCALE + 2);
        canvas_draw_line(canvas, i - 10, SUBGHZ_RAW_TOP_SCALE, i - 10, SUBGHZ_RAW_TOP_SCALE + 2);
    }

    canvas_draw_line(canvas, 0, SUBGHZ_RAW_TOP_SCALE, width, SUBGHZ_RAW_TOP_SCALE);
    canvas_draw_line(canvas, 0, SUBGHZ_RAW_BOTTOM_SCALE, width, SUBGHZ_RAW_BOTTOM_SCALE);

    char format_buffer[32];
    format_number(model->offset, format_buffer);
    snprintf(buffer, sizeof(buffer), "%s", format_buffer);
    uint16_t font_height = canvas_current_font_height(canvas);
    canvas_draw_str(canvas, 0, SUBGHZ_RAW_BOTTOM_SCALE + font_height + 1, buffer);

    memset(buffer, 0, sizeof(buffer));
    memset(format_buffer, 0, sizeof(format_buffer));
    format_number(model->offset + model->offset_per_page, format_buffer);
    snprintf(buffer, sizeof(buffer), "%s us", format_buffer);
    uint16_t last_value_width = canvas_string_width(canvas, buffer);
    canvas_draw_str(
        canvas, width - last_value_width, SUBGHZ_RAW_BOTTOM_SCALE + font_height + 1, buffer);

    if(model->raw_file != NULL) {
        uint64_t current_offset = 0;
        uint64_t current_x = 0;
        bool is_negative = false;
        bool before_negative = false;
        bool last = false;
        uint32_t chunk = (model->offset_per_page - model->offset) / 128;
#ifdef FURI_DEBUG
        FURI_LOG_W(
            TAG,
            "offset: %lld, offset_per_page: %lld, chunk: %ld",
            model->offset,
            model->offset_per_page,
            chunk);
#endif
#ifdef FURI_DEBUG
        for(uint64_t i = 0; i < 1000 && !last; i++) {
#else
        for(uint64_t i = 0; i < model->raw_file->total_count && !last; i++) {
#endif
            int32_t value = *array_raw_get(model->raw_file->values, i);
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "value: %ld, step: %lld", value, i);
#endif
            uint64_t abs_value = 0;
            uint64_t y = 0;
            before_negative = is_negative;

            if(value < 0) {
                abs_value = value * -1;
                is_negative = true;
                y = CHART_LOWEST_POINT;
            } else {
                abs_value = value;
                is_negative = false;
                y = CHART_HIGHTEST_POINT;
            }
            if(current_offset + abs_value < model->offset) {
                current_offset += abs_value;
#ifdef FURI_DEBUG
                FURI_LOG_D(
                    TAG,
                    "continue current_offset: %lld, abs_value: %lld",
                    current_offset,
                    abs_value);
#endif
                continue;
            }

            uint64_t skip_value = model->offset < current_offset ? 0 :
                                                                   model->offset - current_offset;
            uint64_t taken_value = skip_value + abs_value;
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "skip_value: %lld, taken_value: %lld", skip_value, taken_value);
#endif
            if(taken_value > model->offset_per_page) {
                last = true;
                taken_value = taken_value - (taken_value - model->offset_per_page);
            }
            //uint64_t from_offset = current_offset;
            current_offset += taken_value;
            taken_value = (uint64_t)taken_value / chunk;
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG, "taken_value: %lld, current_offset: %lld", taken_value, current_offset);
#endif
            if(is_negative && !before_negative) {
                canvas_draw_line(
                    canvas, current_x, CHART_HIGHTEST_POINT, current_x, CHART_LOWEST_POINT);
            } else if(!is_negative && before_negative) {
                canvas_draw_line(
                    canvas, current_x, CHART_LOWEST_POINT, current_x, CHART_HIGHTEST_POINT);
            }
            canvas_draw_line(canvas, current_x, y, current_x + taken_value, y);

            current_x += taken_value == 0 ? 1 : taken_value;
        }
    }
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

bool shapshup_main_view_no_file(ShapShupMainView* instance) {
    return instance->raw_file == NULL || instance->raw_file->result != ShapShupFileResultOk;
}