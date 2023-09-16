#include "shapshup_main_view_private.h"
#include "shapshup_main_view.h"
#include "../shapshup_i.h"
#include <input/input.h>
#include <gui/elements.h>
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

void shapshup_main_view_show_alert(ShapShupMainView* instance, const char* text, uint32_t ttl) {
    furi_assert(instance);
    instance->alert_dismiss_time = furi_get_tick() + furi_ms_to_ticks(ttl);
    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
            snprintf(model->alert_text, SHAPSHUP_TEXT_STORE_SIZE, "%s", text);
            model->alert_dismiss_time = instance->alert_dismiss_time;
        },
        true);
}

void shapshup_main_view_reset_alert(ShapShupMainView* instance) {
    furi_assert(instance);
    instance->alert_dismiss_time = 0;
}

void shapshup_main_view_check_alert(ShapShupMainView* instance) {
    furi_assert(instance);
    if(instance->alert_dismiss_time > furi_get_tick()) {
        return;
    }
    with_view_model(
        instance->view, ShapShupMainViewModel * model, { model->alert_dismiss_time = 0; }, true);
}

void shapshup_main_view_draw_alert_if_needed(Canvas* canvas, ShapShupMainViewModel* model) {
    if(model->alert_dismiss_time < furi_get_tick()) {
        return;
    }

    /* Show the alert. A box with black border and a text inside. */
    canvas_set_font(canvas, FontPrimary);
    uint8_t w = canvas_string_width(canvas, model->alert_text);
    uint8_t h = 8; // Font height.
    uint8_t text_x = 64 - (w / 2);
    uint8_t text_y = 32 + 4;
    uint8_t padding = 3;
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(
        canvas, text_x - padding, text_y - padding - h, w + padding * 2, h + padding * 2);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(
        canvas,
        text_x - padding + 1,
        text_y - padding - h + 1,
        w + padding * 2 - 2,
        h + padding * 2 - 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str(canvas, text_x, text_y, model->alert_text);
}

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model) {
    furi_assert(model);

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
        //char format_buffer[32];
        //format_number(model->calc_total_len, format_buffer);
        snprintf(
            buffer, sizeof(buffer), "%lld %s", model->calc_total_len, model->is_ms ? "ms" : "us");

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

    if(model->count_shapes > 0) {
    for
        M_EACH(item, model->shape_list->data, ShapShupShapeItemArray_t) {
            canvas_draw_box(canvas, item->x, item->y, item->width, item->height);
        }
    }

    shapshup_main_view_draw_alert_if_needed(canvas, model);
}

bool shapshup_main_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    bool consumed = false;
    ShapShupMainView* instance = context;

    if(event->key == InputKeyBack && event->type == InputTypeLong) {
        // Consumed
        return false;
    } else if(event->key == InputKeyBack) {
        shapshup_main_view_show_alert(instance, "Press long BACK to exit", 150);
        return true;
    }

    bool is_clicked = (event->type == InputTypeShort) || (event->type == InputTypeRepeat);

    if(is_clicked) {
        uint64_t offset_per_page =
            instance->raw_file != NULL ?
                calc_offset_per_page(
                    instance->raw_file->total_len, instance->raw_file->min_len, instance->scale) :
                0;
        if(event->key == InputKeyOk) {
            shapshup_main_view_reset_alert(instance);
            instance->callback(ShapShupCustomEventTypeLoadFile, instance->context);
        } else if(event->key == InputKeyLeft && instance->offset > 0) {
            shapshup_main_view_reset_alert(instance);
            instance->offset =
                instance->offset < offset_per_page ? 0 : instance->offset - offset_per_page;
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG,
                "L offset: %lld, offset_per_page: %lld, offset: %lld, total_len: %lld",
                instance->offset,
                offset_per_page,
                instance->offset,
                instance->raw_file->total_len);
#endif
            shapshup_main_view_array_reset(instance);
            shapshup_main_view_create_shapes(instance, offset_per_page);

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->shape_list = instance->shape_list;
                    model->count_shapes = instance->count_shapes;
                    model->offset = instance->offset;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyRight) {
            shapshup_main_view_reset_alert(instance);
            uint64_t calc = instance->offset + offset_per_page;
#ifdef FURI_DEBUG
            FURI_LOG_D(
                TAG,
                "R offset: %lld, offset_per_page: %lld, calc: %lld, total_len: %lld",
                instance->offset,
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
            shapshup_main_view_array_reset(instance);
            shapshup_main_view_create_shapes(instance, offset_per_page);

            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->shape_list = instance->shape_list;
                    model->count_shapes = instance->count_shapes;
                    model->offset = instance->offset;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyDown) {
            instance->scale -= SCALE_STEP;
            if(instance->scale < SCALE_STEP) {
                instance->scale = 0.1f;
            }
            offset_per_page = instance->raw_file != NULL ? calc_offset_per_page(
                                                               instance->raw_file->total_len,
                                                               instance->raw_file->min_len,
                                                               instance->scale) :
                                                           0;
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "Scale: %f", (double)instance->scale);
#endif
            shapshup_main_view_array_reset(instance);
            shapshup_main_view_create_shapes(instance, offset_per_page);

            shapshup_main_view_show_alert(instance, "Zoom-in", 150);
            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->shape_list = instance->shape_list;
                    model->count_shapes = instance->count_shapes;
                    model->scale = instance->scale;
                    model->offset_per_page = offset_per_page;
                },
                true);
        } else if(event->key == InputKeyUp) {
            if(instance->scale < SCALE_STEP) {
                instance->scale = SCALE_STEP;
            } else if(SHAPSHUP_DEFAULT_SCALE_STEP - instance->scale > 0.01f) {
                instance->scale += SCALE_STEP;
            }

            if(instance->scale > SHAPSHUP_DEFAULT_SCALE_STEP) {
                instance->scale = SHAPSHUP_DEFAULT_SCALE_STEP;
            }
            offset_per_page = instance->raw_file != NULL ? calc_offset_per_page(
                                                               instance->raw_file->total_len,
                                                               instance->raw_file->min_len,
                                                               instance->scale) :
                                                           0;
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "Scale: %f", (double)instance->scale);
#endif
            shapshup_main_view_array_reset(instance);
            shapshup_main_view_create_shapes(instance, offset_per_page);

            shapshup_main_view_show_alert(instance, "Zoom-out", 150);
            with_view_model(
                instance->view,
                ShapShupMainViewModel * model,
                {
                    model->shape_list = instance->shape_list;
                    model->count_shapes = instance->count_shapes;
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
    instance->scale = SHAPSHUP_DEFAULT_SCALE_STEP;

    instance->shape_list = malloc(sizeof(ShapShupShapeStruct));
    ShapShupShapeItemArray_init(instance->shape_list->data);

    with_view_model(
        instance->view,
        ShapShupMainViewModel * model,
        {
            model->count_shapes = 0;
            model->shape_list = NULL;
            model->raw_file = NULL;
            model->offset = 0;
            model->offset_per_page = 0;
            model->calc_total_len = 0;
            model->is_ms = false;
            model->scale = SHAPSHUP_DEFAULT_SCALE_STEP;
        },
        true);

    return instance;
}

void shapshup_main_view_free(ShapShupMainView* instance) {
    furi_assert(instance);

    // Clean array and struct
    clean_raw_values(instance->raw_file);

    // Clean array
    ShapShupShapeItemArray_clear(instance->shape_list->data);
    free(instance->shape_list);

    view_free(instance->view);
    free(instance);
}

void shapshup_main_view_array_reset(ShapShupMainView* instance) {
    furi_assert(instance);

    ShapShupShapeItemArray_reset(instance->shape_list->data);
    instance->count_shapes = 0;
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
        instance->offset = 0;
        uint64_t offset_per_page = calc_offset_per_page(
            instance->raw_file->total_len, instance->raw_file->min_len, instance->scale);

        // Reset array
        shapshup_main_view_array_reset(instance);
        shapshup_main_view_create_shapes(instance, offset_per_page);

        with_view_model(
            instance->view,
            ShapShupMainViewModel * model,
            {
                model->raw_file = instance->raw_file;
                model->shape_list = instance->shape_list;
                model->count_shapes = instance->count_shapes;
                model->offset = 0;
                if(model->raw_file != NULL) {
                    model->offset_per_page = offset_per_page;
                    if(model->raw_file->total_len > 1000) {
                        model->calc_total_len = (uint64_t)model->raw_file->total_len / 1000;
                        model->is_ms = true;
                    } else {
                        model->is_ms = false;
                    }
                }
                model->scale = SHAPSHUP_DEFAULT_SCALE_STEP;
            },
            true);
    }

    return instance->raw_file->result;
}

void shapshup_main_view_create_shapes(ShapShupMainView* instance, uint64_t offset_per_page) {
    uint64_t current_offset = 0;
    uint64_t current_x = 0;
    bool is_negative = false;
    bool last = false;
    uint32_t chunk = (uint32_t)offset_per_page / SCREEN_WIDTH;
    uint64_t skip_value;
#ifdef FURI_DEBUG
    // FURI_LOG_W(
    //     TAG,
    //     "offset: %lld, offset_per_page: %lld, chunk: %ld",
    //     model->offset,
    //     model->offset_per_page,
    //     chunk);
#endif
    for(uint64_t i = 0; i < instance->raw_file->total_count && !last; i++) {
        int32_t value = *array_raw_get(instance->raw_file->values, i);
#ifdef FURI_DEBUG
        //FURI_LOG_D(TAG, "value: %ld, step: %lld", value, i);
#endif
        uint64_t current_value;

        if(value < 0) {
            current_value = value * -1;
            is_negative = true;
        } else {
            current_value = value;
            is_negative = false;
        }
        if(current_offset + current_value < instance->offset) {
            current_offset += current_value;
#ifdef FURI_DEBUG
            // FURI_LOG_I(
            //     TAG,
            //     "continue current_offset: %lld, current_value: %lld",
            //     current_offset,
            //     current_value);
#endif
            continue;
            /*} else if(model->offset > current_offset) {
                skip_value = model->offset - current_offset;*/
        } else {
            skip_value = 0;
        }

        current_value = skip_value + current_value;
#ifdef FURI_DEBUG
        // FURI_LOG_D(TAG, "skip_value: %lld, current_value: %lld", skip_value, current_value);
#endif
        if(current_value > offset_per_page) {
            last = true;
            current_value = current_value - (current_value - offset_per_page);
            // FURI_LOG_I(
            //     TAG,
            //     "current_value > model->offset_per_page: %lld, model->offset_per_page: %lld",
            //     current_value,
            //     model->offset_per_page);
        } else if(current_offset + current_value > instance->offset + offset_per_page) {
            last = true;
            current_value = current_value - ((current_offset + current_value) -
                                             (instance->offset + offset_per_page));
            // FURI_LOG_I(
            //     TAG,
            //     "current_offset + current_value > current_offset + model->offset_per_page: %lld, model->offset_per_page: %lld",
            //     current_value,
            //     model->offset_per_page);
        }
        current_offset += current_value;
        current_value = (uint64_t)(current_value / chunk);

        ShapShupShapeItem* item = ShapShupShapeItemArray_push_raw(instance->shape_list->data);
        if(is_negative) {
            item->x = current_x;
            item->y = SHAPSHUP_CHART_LOWEST_POINT - 2;
            item->width = current_value + 1;
            item->height = 2;
        } else {
            item->x = current_x;
            item->y = SHAPSHUP_CHART_HIGHEST_POINT;
            item->width = current_value + 1;
            item->height = SHAPSHUP_CHART_LOWEST_POINT - SHAPSHUP_CHART_HIGHEST_POINT;
        }

        current_x += current_value == 0 ? 1 : current_value;
        current_x++;
        instance->count_shapes++;
    }
}

void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model) {
    char buffer[64];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontBatteryPercent);
    uint8_t width = canvas_width(canvas);

    // First and last scale pin
    canvas_draw_line(canvas, 1, SHAPSHUP_TOP_SCALE, 1, SHAPSHUP_TOP_SCALE + 4);
    canvas_draw_line(canvas, width - 1, SHAPSHUP_TOP_SCALE, width - 1, SHAPSHUP_TOP_SCALE + 4);
    for(int i = width; i > 0; i -= 16) {
        canvas_draw_line(canvas, i, SHAPSHUP_TOP_SCALE, i, SHAPSHUP_TOP_SCALE + 4);
        canvas_draw_line(canvas, i - 5, SHAPSHUP_TOP_SCALE, i - 5, SHAPSHUP_TOP_SCALE + 2);
        canvas_draw_line(canvas, i - 10, SHAPSHUP_TOP_SCALE, i - 10, SHAPSHUP_TOP_SCALE + 2);
    }

    canvas_draw_line(canvas, 0, SHAPSHUP_TOP_SCALE, width, SHAPSHUP_TOP_SCALE);
    canvas_draw_line(canvas, 0, SHAPSHUP_BOTTOM_SCALE, width, SHAPSHUP_BOTTOM_SCALE);

    char format_buffer[32];
    format_number(model->offset, format_buffer);
    snprintf(buffer, sizeof(buffer), "%s us", format_buffer);
    uint16_t font_height = canvas_current_font_height(canvas);
    canvas_draw_str(canvas, 0, SHAPSHUP_BOTTOM_SCALE + font_height + 1, buffer);

    memset(buffer, 0, sizeof(buffer));
    memset(format_buffer, 0, sizeof(format_buffer));
    format_number(model->offset + model->offset_per_page, format_buffer);
    snprintf(buffer, sizeof(buffer), "%s us", format_buffer);
    uint16_t last_value_width = canvas_string_width(canvas, buffer);
    canvas_draw_str(
        canvas, width - last_value_width, SHAPSHUP_BOTTOM_SCALE + font_height + 1, buffer);

    // if(model->raw_file != NULL) {
    //         uint64_t current_offset = 0;
    //         uint64_t current_x = 0;
    //         bool is_negative = false;
    //         //bool before_negative = !is_negative;
    //         bool last = false;
    //         uint32_t chunk = (uint32_t)model->offset_per_page / SCREEN_WIDTH;
    //         uint64_t skip_value = 0;
    // #ifdef FURI_DEBUG
    //         // FURI_LOG_W(
    //         //     TAG,
    //         //     "offset: %lld, offset_per_page: %lld, chunk: %ld",
    //         //     model->offset,
    //         //     model->offset_per_page,
    //         //     chunk);
    // #endif
    // #ifdef FURI_DEBUG
    //         for(uint64_t i = 0; i < 1000 && !last; i++) {
    // #else
    //         for(uint64_t i = 0; i < model->raw_file->total_count && !last; i++) {
    // #endif
    //             int32_t value = *array_raw_get(model->raw_file->values, i);
    // #ifdef FURI_DEBUG
    //             //FURI_LOG_D(TAG, "value: %ld, step: %lld", value, i);
    // #endif
    //             uint64_t current_value = 0;
    //             //uint64_t y = 0;
    //             //before_negative = is_negative;

    //             if(value < 0) {
    //                 current_value = value * -1;
    //                 is_negative = true;
    //                 //y = CHART_LOWEST_POINT;
    //             } else {
    //                 current_value = value;
    //                 is_negative = false;
    //                 //y = SHAPSHUP_CHART_HIGHTEST_POINT;
    //             }
    //             if(current_offset + current_value < model->offset) {
    //                 current_offset += current_value;
    //                 skip_value = 0;
    // #ifdef FURI_DEBUG
    //                 // FURI_LOG_I(
    //                 //     TAG,
    //                 //     "continue current_offset: %lld, current_value: %lld",
    //                 //     current_offset,
    //                 //     current_value);
    // #endif
    //                 continue;
    //                 /*} else if(model->offset > current_offset) {
    //                 skip_value = model->offset - current_offset;*/
    //             } else {
    //                 skip_value = 0;
    //             }

    //             current_value = skip_value + current_value;
    // #ifdef FURI_DEBUG
    //             // FURI_LOG_D(TAG, "skip_value: %lld, current_value: %lld", skip_value, current_value);
    // #endif
    //             if(current_value > model->offset_per_page) {
    //                 last = true;
    //                 current_value = current_value - (current_value - model->offset_per_page);
    //                 // FURI_LOG_I(
    //                 //     TAG,
    //                 //     "current_value > model->offset_per_page: %lld, model->offset_per_page: %lld",
    //                 //     current_value,
    //                 //     model->offset_per_page);
    //             } else if(current_offset + current_value > model->offset + model->offset_per_page) {
    //                 last = true;
    //                 current_value = current_value - ((current_offset + current_value) -
    //                                                  (model->offset + model->offset_per_page));
    //                 // FURI_LOG_I(
    //                 //     TAG,
    //                 //     "current_offset + current_value > current_offset + model->offset_per_page: %lld, model->offset_per_page: %lld",
    //                 //     current_value,
    //                 //     model->offset_per_page);
    //             }
    //             //uint64_t from_offset = current_offset;
    //             current_offset += current_value;
    //             current_value = (uint64_t)(current_value / chunk);

    //             if(is_negative) {
    //                 canvas_draw_box(canvas, current_x, CHART_LOWEST_POINT - 2, current_value + 1, 2);
    //                 // canvas_draw_line(
    //                 //     canvas, current_x, SHAPSHUP_CHART_HIGHTEST_POINT, current_x, CHART_LOWEST_POINT);
    //             } else {
    //                 canvas_draw_box(
    //                     canvas,
    //                     current_x,
    //                     SHAPSHUP_CHART_HIGHTEST_POINT,
    //                     current_value + 1,
    //                     CHART_LOWEST_POINT - SHAPSHUP_CHART_HIGHTEST_POINT);
    //                 // canvas_draw_line(
    //                 //     canvas, current_x, CHART_LOWEST_POINT, current_x, SHAPSHUP_CHART_HIGHTEST_POINT);
    //             }
    //             // canvas_draw_line(canvas, current_x, y, current_x + current_value, y);

    //             current_x += current_value == 0 ? 1 : current_value;
    //             current_x++;

    // #ifdef FURI_DEBUG
    //             // FURI_LOG_D(
    //             //     TAG,
    //             //     "current_value: %lld, current_offset: %lld, current_x: %lld",
    //             //     current_value,
    //             //     current_offset,
    //             //     current_x);
    // #endif
    //         }
    // }
}

bool shapshup_main_view_no_file(ShapShupMainView* instance) {
    return instance->raw_file == NULL || instance->raw_file->result != ShapShupFileResultOk;
}

uint64_t calc_offset_per_page(uint64_t total, uint64_t min_len, float scale) {
    uint64_t result = (uint64_t)(((float)total / (float)min_len * scale));
    // if(result > min_len) {
    //     result = min_len;
    // }
    return result;
}

void format_number(uint64_t number_to_format, char* output_buffer) {
    int32_t c;
    char buf[32];
    char* p;

    snprintf(buf, sizeof(buf), "%lld", number_to_format);
    c = 2 - (int32_t)(strlen(buf) % 3);
    for(p = buf; *p != 0; p++) {
        *output_buffer++ = *p;
        if(c == 1) {
            *output_buffer++ = ',';
        }
        c = (c + 1) % 3;
    }
    *--output_buffer = 0;
}
