#include "subbrute_main_view.h"
#include "../subbrute_i.h"
#include "../subbrute_protocols.h"

#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>

#define STATUS_BAR_Y_SHIFT 14
#define TAG "SubBruteMainView"

#define ITEMS_ON_SCREEN 3

struct SubBruteMainView {
    View* view;
    SubBruteMainViewCallback callback;
    void* context;
    uint8_t index;
    bool is_select_byte;
    const char* key_field;
    uint8_t extra_repeats;
    uint8_t window_position;
};

typedef struct {
    uint8_t index;
    uint8_t extra_repeats;
    uint8_t window_position;
    bool is_select_byte;
    const char* key_field;
} SubBruteMainViewModel;

void subbrute_main_view_set_callback(
    SubBruteMainView* instance,
    SubBruteMainViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

FuriString* center_displayed_key(const char* key_cstr, uint8_t index) {
    uint8_t str_index = (index * 3);

    char display_menu[] = {
        'X', 'X', ' ', 'X', 'X', ' ', '<', 'X', 'X', '>', ' ', 'X', 'X', ' ', 'X', 'X', '\0'};

    if(key_cstr != NULL) {
        if(index > 1) {
            display_menu[0] = key_cstr[str_index - 6];
            display_menu[1] = key_cstr[str_index - 5];
        } else {
            display_menu[0] = ' ';
            display_menu[1] = ' ';
        }

        if(index > 0) {
            display_menu[3] = key_cstr[str_index - 3];
            display_menu[4] = key_cstr[str_index - 2];
        } else {
            display_menu[3] = ' ';
            display_menu[4] = ' ';
        }

        display_menu[7] = key_cstr[str_index];
        display_menu[8] = key_cstr[str_index + 1];

        if((str_index + 4) <= (uint8_t)strlen(key_cstr)) {
            display_menu[11] = key_cstr[str_index + 3];
            display_menu[12] = key_cstr[str_index + 4];
        } else {
            display_menu[11] = ' ';
            display_menu[12] = ' ';
        }

        if((str_index + 8) <= (uint8_t)strlen(key_cstr)) {
            display_menu[14] = key_cstr[str_index + 6];
            display_menu[15] = key_cstr[str_index + 7];
        } else {
            display_menu[14] = ' ';
            display_menu[15] = ' ';
        }
    }
    return furi_string_alloc_set(display_menu);
}

void subbrute_main_view_draw(Canvas* canvas, SubBruteMainViewModel* model) {
    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_box(canvas, 0, 0, canvas_width(canvas), STATUS_BAR_Y_SHIFT);
    canvas_invert_color(canvas);
    canvas_draw_str_aligned(canvas, 64, 3, AlignCenter, AlignTop, "Sub-GHz BruteForcer 3.2");
    canvas_invert_color(canvas);

    uint16_t screen_width = canvas_width(canvas);
    uint16_t screen_height = canvas_height(canvas);

    if(model->is_select_byte) {
#ifdef FURI_DEBUG
        //FURI_LOG_D(TAG, "key_field: %s", model->key_field);
#endif
        char msg_index[18];
        snprintf(msg_index, sizeof(msg_index), "Field index : %d", model->index);
        canvas_draw_str_aligned(canvas, 64, 26, AlignCenter, AlignTop, msg_index);

        FuriString* menu_items;

        menu_items = center_displayed_key(model->key_field, model->index);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas, 64, 40, AlignCenter, AlignTop, furi_string_get_cstr(menu_items));

        elements_button_center(canvas, "Select");
        elements_button_left(canvas, "<");
        elements_button_right(canvas, ">");

        furi_string_reset(menu_items);
        furi_string_free(menu_items);
    } else {
        // Menu
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontSecondary);
        const uint8_t item_height = 16;

#ifdef FURI_DEBUG
        //FURI_LOG_D(TAG, "window_position: %d, index: %d", model->window_position, model->index);
#endif
        for(uint8_t position = 0; position < SubBruteAttackTotalCount; ++position) {
            uint8_t item_position = position - model->window_position;

            if(item_position < ITEMS_ON_SCREEN) {
                if(model->index == position) {
                    canvas_draw_str_aligned(
                        canvas,
                        4,
                        9 + (item_position * item_height) + STATUS_BAR_Y_SHIFT,
                        AlignLeft,
                        AlignCenter,
                        subbrute_protocol_name(position));

                    if(model->extra_repeats > 0) {
                        canvas_set_font(canvas, FontBatteryPercent);
                        char buffer[10];
                        snprintf(
                            buffer,
                            sizeof(buffer),
                            "x%d",
                            model->extra_repeats + subbrute_protocol_repeats_count(model->index));
                        canvas_draw_str_aligned(
                            canvas,
                            screen_width - 15,
                            9 + (item_position * item_height) + STATUS_BAR_Y_SHIFT,
                            AlignLeft,
                            AlignCenter,
                            buffer);
                        canvas_set_font(canvas, FontSecondary);
                    }

                    elements_frame(
                        canvas, 1, 1 + (item_position * item_height) + STATUS_BAR_Y_SHIFT, 124, 15);
                } else {
                    canvas_draw_str_aligned(
                        canvas,
                        4,
                        9 + (item_position * item_height) + STATUS_BAR_Y_SHIFT,
                        AlignLeft,
                        AlignCenter,
                        subbrute_protocol_name(position));
                }
            }
        }

        elements_scrollbar_pos(
            canvas,
            screen_width,
            STATUS_BAR_Y_SHIFT + 2,
            screen_height - STATUS_BAR_Y_SHIFT,
            model->index,
            SubBruteAttackTotalCount);
    }
}

bool subbrute_main_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
#ifdef FURI_DEBUG
        FURI_LOG_I(TAG, "InputKey: BACK");
#endif
        return false;
    }

    SubBruteMainView* instance = context;
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "InputKey: %d, extra_repeats: %d", event->key, instance->extra_repeats);
#endif
    const uint8_t min_value = 0;
    const uint8_t correct_total = SubBruteAttackTotalCount - 1;
    uint8_t max_repeats = 9 - subbrute_protocol_repeats_count(instance->index);
    uint8_t index = 0;

    bool updated = false;
    bool consumed = false;
    bool is_short = (event->type == InputTypeShort) || (event->type == InputTypeRepeat);

    if(!instance->is_select_byte) {
        if(event->key == InputKeyUp && is_short) {
            if(instance->index == min_value) {
                instance->index = correct_total;
            } else {
                instance->index = CLAMP(instance->index - 1, correct_total, min_value);
            }
            instance->extra_repeats = 0;
            updated = true;
            consumed = true;
        } else if(event->key == InputKeyDown && is_short) {
            if(instance->index == correct_total) {
                instance->index = min_value;
            } else {
                instance->index = CLAMP(instance->index + 1, correct_total, min_value);
            }
            instance->extra_repeats = 0;
            updated = true;
            consumed = true;
        } else if(event->key == InputKeyLeft && is_short) {
            instance->extra_repeats = CLAMP(instance->extra_repeats - 1, max_repeats, 0);
            updated = true;
            consumed = true;
        } else if(event->key == InputKeyRight && is_short) {
            instance->extra_repeats = CLAMP(instance->extra_repeats + 1, max_repeats, 0);
            updated = true;
            consumed = true;
        } else if(event->key == InputKeyOk && is_short) {
            if(index == SubBruteAttackLoadFile) {
                instance->callback(SubBruteCustomEventTypeLoadFile, instance->context);
            } else {
                instance->callback(SubBruteCustomEventTypeMenuSelected, instance->context);
            }
            consumed = true;
            updated = true;
        }
        if(updated) {
            instance->window_position = instance->index;
            if(instance->window_position > 0) {
                instance->window_position -= 1;
            }

            if(SubBruteAttackTotalCount <= ITEMS_ON_SCREEN) {
                instance->window_position = 0;
            } else {
                if(instance->window_position >= (SubBruteAttackTotalCount - ITEMS_ON_SCREEN)) {
                    instance->window_position = (SubBruteAttackTotalCount - ITEMS_ON_SCREEN);
                }
            }
        }
    } else {
        if(event->key == InputKeyLeft && is_short) {
            if(instance->index > 0) {
                instance->index--;
            }
            updated = true;
        } else if(event->key == InputKeyRight && is_short) {
            if(instance->index < 7) {
                instance->index++;
            }
            updated = true;
        } else if(event->key == InputKeyOk && is_short) {
            instance->callback(SubBruteCustomEventTypeIndexSelected, instance->context);
            consumed = true;
            updated = true;
        }
    }

    if(updated) {
        with_view_model(
            instance->view,
            SubBruteMainViewModel * model,
            {
                model->index = instance->index;
                model->window_position = instance->window_position;
                model->key_field = instance->key_field;
                model->is_select_byte = instance->is_select_byte;
                model->extra_repeats = instance->extra_repeats;
            },
            true);
    }

    return consumed;
}

void subbrute_main_view_enter(void* context) {
    furi_assert(context);
    SubBruteMainView* instance = context;

    with_view_model(
        instance->view,
        SubBruteMainViewModel * model,
        {
            model->key_field = NULL;
            model->is_select_byte = false;
        },
        true);

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "subbrute_main_view_enter");
#endif
}

void subbrute_main_view_exit(void* context) {
    furi_assert(context);

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "subbrute_main_view_exit");
#endif
}

SubBruteMainView* subbrute_main_view_alloc() {
    SubBruteMainView* instance = malloc(sizeof(SubBruteMainView));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(SubBruteMainViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)subbrute_main_view_draw);
    view_set_input_callback(instance->view, subbrute_main_view_input);
    view_set_enter_callback(instance->view, subbrute_main_view_enter);
    view_set_exit_callback(instance->view, subbrute_main_view_exit);

    with_view_model(
        instance->view,
        SubBruteMainViewModel * model,
        {
            model->index = 0;
            model->window_position = 0;
            model->key_field = NULL;
            model->is_select_byte = false;
            model->extra_repeats = 0;
        },
        true);

    instance->index = 0;
    instance->window_position = 0;
    instance->key_field = NULL;
    instance->is_select_byte = false;
    instance->extra_repeats = 0;

    return instance;
}

void subbrute_main_view_free(SubBruteMainView* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* subbrute_main_view_get_view(SubBruteMainView* instance) {
    furi_assert(instance);
    return instance->view;
}

void subbrute_main_view_set_index(
    SubBruteMainView* instance,
    uint8_t idx,
    bool is_select_byte,
    const char* key_field) {
    furi_assert(instance);
    furi_assert(idx < SubBruteAttackTotalCount);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "Set index: %d", idx);
#endif
    instance->is_select_byte = is_select_byte;
    instance->key_field = key_field;
    instance->index = idx;
    instance->window_position = idx;

    if(!is_select_byte) {
        if(instance->window_position > 0) {
            instance->window_position -= 1;
        }

        if(SubBruteAttackTotalCount <= ITEMS_ON_SCREEN) {
            instance->window_position = 0;
        } else {
            if(instance->window_position >= (SubBruteAttackTotalCount - ITEMS_ON_SCREEN)) {
                instance->window_position = (SubBruteAttackTotalCount - ITEMS_ON_SCREEN);
            }
        }
    }

    with_view_model(
        instance->view,
        SubBruteMainViewModel * model,
        {
            model->index = instance->index;
            model->window_position = instance->window_position;
            model->key_field = instance->key_field;
            model->is_select_byte = instance->is_select_byte;
            model->extra_repeats = instance->extra_repeats;
        },
        true);
}

SubBruteAttacks subbrute_main_view_get_index(SubBruteMainView* instance) {
    furi_assert(instance);
    return instance->index;
}

uint8_t subbrute_main_view_get_extra_repeats(SubBruteMainView* instance) {
    furi_assert(instance);
    return instance->extra_repeats;
}