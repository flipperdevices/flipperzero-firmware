#include "text_box.h"
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>
#include <stdint.h>

#define TEXT_BOX_TEXT_WIDTH (120)
#define TEXT_BOX_TEXT_HEIGHT (56)

struct TextBox {
    View* view;

    uint16_t button_held_for_ticks;
};

typedef struct {
    const char* text;
    char* text_pos;
    FuriString* text_formatted;
    int32_t scroll_pos;
    int32_t scroll_num;
    TextBoxFont font;
    TextBoxFocus focus;
    bool formatted;

    FuriString* text_on_screen;
    FuriString* text_line;
    int32_t start_line;
    int32_t start_line_offset;
    int32_t lines_on_screen;
    int32_t lines_num;
} TextBoxModel;

static void text_box_process_down(TextBox* text_box, uint8_t lines) {
    if(lines > 0) {
        with_view_model(
            text_box->view,
            TextBoxModel * model,
            {
                if(model->scroll_pos < model->scroll_num - lines) {
                    model->scroll_pos += lines;
                } else {
                    model->scroll_pos = model->scroll_num - 1;
                }
            },
            true);
    }
}

static void text_box_process_up(TextBox* text_box, uint8_t lines) {
    if(lines > 0) {
        with_view_model(
            text_box->view,
            TextBoxModel * model,
            {
                if(model->scroll_pos >= lines) {
                    model->scroll_pos -= lines;
                } else {
                    model->scroll_pos = 0;
                }
            },
            true);
    }
}

static uint32_t text_box_calculate_lines_num(Canvas* canvas, const char* text) {
    size_t i = 0;
    size_t line_width = 0;
    size_t lines_num = 0;

    while(text[i] != '\0') {
        char symb = text[i++];
        if(symb != '\n') {
            size_t glyph_width = canvas_glyph_width(canvas, symb);
            if(line_width + glyph_width > TEXT_BOX_TEXT_WIDTH) {
                lines_num++;
                line_width = 0;
            }
            line_width += glyph_width;
        } else {
            lines_num++;
            line_width = 0;
        }
    }

    // Last line is empty
    lines_num++;

    return lines_num;
}

static void text_box_get_next_line_index(
    Canvas* canvas,
    const char* text,
    int32_t start_offset,
    int32_t* next_line_offset) {
    size_t i = start_offset;
    size_t line_width = 0;

    while(true) {
        char symb = text[i];
        if(symb == '\0') {
            *next_line_offset = i;
            break;
        } else if(symb == '\n') {
            *next_line_offset = i + 1;
            break;
        } else {
            size_t glyph_width = canvas_glyph_width(canvas, symb);
            if(line_width + glyph_width > TEXT_BOX_TEXT_WIDTH) {
                *next_line_offset = i;
                break;
            }
            line_width += glyph_width;
            i++;
        }
    }
}

static void text_box_get_prev_line_index(
    Canvas* canvas,
    const char* text,
    int32_t start_offset,
    int32_t* prev_line_offset) {
    if(start_offset == 0) {
        *prev_line_offset = 0;
    } else {
        size_t i = start_offset - 1;

        // Ignore newline charachter at the end of previous line
        if(text[i] == '\n') {
            i--;
        }

        // Search for the start of the previous paragraph
        while(i > 0) {
            if(text[i--] == '\n') break;
        }

        // Search for the line offset before given start offset
        int32_t next_line_offset = 0;
        while(true) {
            text_box_get_next_line_index(canvas, text, i, &next_line_offset);
            if(next_line_offset == start_offset) break;
            i = next_line_offset;
        }
        *prev_line_offset = i;
    }
}

static void text_box_move_start_line(Canvas* canvas, TextBoxModel* model, int32_t line_offset) {
    if(line_offset > 0) {
        int32_t next_line_offset = 0;
        for(int32_t i = 0; i < line_offset; i++) {
            text_box_get_next_line_index(
                canvas, model->text, model->start_line_offset, &next_line_offset);
            model->start_line_offset = next_line_offset;
        }
    } else {
        int32_t prev_line_offset = 0;
        for(int32_t i = 0; i < (-line_offset); i++) {
            text_box_get_prev_line_index(
                canvas, model->text, model->start_line_offset, &prev_line_offset);
            model->start_line_offset = prev_line_offset;
        }
    }
}

static void text_box_update_screen_text(Canvas* canvas, TextBoxModel* model) {
    furi_string_reset(model->text_on_screen);
    furi_string_reset(model->text_line);

    FURI_LOG_D("TB", "Scrol pos: %ld", model->scroll_pos);

    int32_t accum_line_offset = model->start_line_offset;
    for(int32_t i = 0; i < model->lines_on_screen; i++) {
        FURI_LOG_D("TB", "Line: %ld, Current line offset: %ld", i, accum_line_offset);

        int32_t next_line_offset = 0;
        text_box_get_next_line_index(canvas, model->text, accum_line_offset, &next_line_offset);
        if(next_line_offset == accum_line_offset) {
            furi_string_set_str(model->text_line, "\n");
        } else {
            furi_string_set_strn(
                model->text_line,
                &model->text[accum_line_offset],
                next_line_offset - accum_line_offset);
        }

        size_t str_len = furi_string_size(model->text_line);
        if(furi_string_get_char(model->text_line, str_len - 1) != '\n') {
            furi_string_push_back(model->text_line, '\n');
        }
        furi_string_cat(model->text_on_screen, model->text_line);
        accum_line_offset = next_line_offset;
    }
}

static void text_box_update_text_on_screen(Canvas* canvas, TextBoxModel* model) {
    int32_t line_offset = model->scroll_pos - model->start_line;
    text_box_move_start_line(canvas, model, line_offset);
    text_box_update_screen_text(canvas, model);
    model->start_line = model->scroll_pos;
}

static void text_box_prepare_model(Canvas* canvas, TextBoxModel* model) {
    model->lines_num = text_box_calculate_lines_num(canvas, model->text);
    model->lines_on_screen = TEXT_BOX_TEXT_HEIGHT / canvas_current_font_height(canvas);

    if(model->lines_num > model->lines_on_screen - 1) {
        model->scroll_num = model->lines_num - (model->lines_on_screen - 1);
        model->scroll_pos = (model->focus == TextBoxFocusEnd) ? model->scroll_num - 1 : 0;
    } else {
        model->scroll_pos = 0;
        model->scroll_num = 0;
    }

    text_box_update_text_on_screen(canvas, model);
}

static void text_box_view_draw_callback(Canvas* canvas, void* _model) {
    TextBoxModel* model = _model;

    if(!model->text) {
        return;
    }

    canvas_clear(canvas);
    if(model->font == TextBoxFontText) {
        canvas_set_font(canvas, FontSecondary);
    } else if(model->font == TextBoxFontHex) {
        canvas_set_font(canvas, FontKeyboard);
    }

    if(!model->formatted) {
        text_box_prepare_model(canvas, model);
        model->formatted = true;
        FURI_LOG_I("TB", "Scrol num: %ld, Lines num: %ld", model->scroll_num, model->lines_num);
    }

    elements_slightly_rounded_frame(canvas, 0, 0, 124, 64);
    elements_scrollbar(canvas, model->scroll_pos, model->scroll_num);

    if(model->start_line != model->scroll_pos) {
        text_box_update_text_on_screen(canvas, model);
    }
    elements_multiline_text(canvas, 3, 11, furi_string_get_cstr(model->text_on_screen));
}

static bool text_box_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    TextBox* text_box = context;
    bool consumed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        int32_t scroll_speed = 1;
        if(text_box->button_held_for_ticks > 5) {
            if(text_box->button_held_for_ticks % 2) {
                scroll_speed = 0;
            } else {
                scroll_speed = text_box->button_held_for_ticks > 9 ? 5 : 3;
            }
        }

        if(event->key == InputKeyDown) {
            text_box_process_down(text_box, scroll_speed);
            consumed = true;
        } else if(event->key == InputKeyUp) {
            text_box_process_up(text_box, scroll_speed);
            consumed = true;
        }

        text_box->button_held_for_ticks++;
    } else if(event->type == InputTypeRelease) {
        text_box->button_held_for_ticks = 0;
        consumed = true;
    }

    return consumed;
}

TextBox* text_box_alloc(void) {
    TextBox* text_box = malloc(sizeof(TextBox));
    text_box->view = view_alloc();
    view_set_context(text_box->view, text_box);
    view_allocate_model(text_box->view, ViewModelTypeLocking, sizeof(TextBoxModel));
    view_set_draw_callback(text_box->view, text_box_view_draw_callback);
    view_set_input_callback(text_box->view, text_box_view_input_callback);

    with_view_model(
        text_box->view,
        TextBoxModel * model,
        {
            model->text = NULL;
            model->text_formatted = furi_string_alloc_set("");
            model->text_on_screen = furi_string_alloc_set_str("");
            model->text_line = furi_string_alloc_set_str("");
            model->formatted = false;
            model->font = TextBoxFontText;
        },
        true);

    return text_box;
}

void text_box_free(TextBox* text_box) {
    furi_check(text_box);

    with_view_model(
        text_box->view,
        TextBoxModel * model,
        {
            furi_string_free(model->text_formatted);
            furi_string_free(model->text_on_screen);
            furi_string_free(model->text_line);
        },
        true);
    view_free(text_box->view);
    free(text_box);
}

View* text_box_get_view(TextBox* text_box) {
    furi_check(text_box);
    return text_box->view;
}

void text_box_reset(TextBox* text_box) {
    furi_check(text_box);

    with_view_model(
        text_box->view,
        TextBoxModel * model,
        {
            model->text = NULL;
            furi_string_set(model->text_formatted, "");
            model->font = TextBoxFontText;
            model->focus = TextBoxFocusStart;
            model->formatted = false;
        },
        true);
}

void text_box_set_text(TextBox* text_box, const char* text) {
    furi_check(text_box);
    furi_check(text);
    size_t str_length = strlen(text);
    size_t formating_margin = 0;

    with_view_model(
        text_box->view,
        TextBoxModel * model,
        {
            model->text = text;
            furi_string_reset(model->text_formatted);
            furi_string_reserve(model->text_formatted, str_length + formating_margin);
            model->formatted = false;
        },
        true);
}

void text_box_set_font(TextBox* text_box, TextBoxFont font) {
    furi_check(text_box);

    with_view_model(
        text_box->view, TextBoxModel * model, { model->font = font; }, true);
}

void text_box_set_focus(TextBox* text_box, TextBoxFocus focus) {
    furi_check(text_box);

    with_view_model(
        text_box->view, TextBoxModel * model, { model->focus = focus; }, true);
}
