#include "widget_element_i.h"
#include <m-string.h>
#include <gui/elements.h>
#include <m-array.h>

#define TAG "TextScroll"

typedef struct {
    Font font;
    Align horizontal;
    string_t text;
} GuiTextScrollLine;

ARRAY_DEF(GuiTextScrollLineArray, GuiTextScrollLine, M_POD_OPLIST)

typedef struct {
    GuiTextScrollLineArray_t line_array;
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    string_t text;
    uint8_t total_lines;
    uint8_t current_line;
    bool text_formatted;
} GuiTextScrollModel;

static bool gui_text_scroll_process_ctrl_symbols(GuiTextScrollLine* line, string_t text) {
    bool processed = false;

    do {
        if(string_get_char(text, 0) != '\e') break;
        char ctrl_symbol = string_get_char(text, 1);
        if(ctrl_symbol == 'l') {
            line->horizontal = AlignLeft;
        } else if(ctrl_symbol == 'c') {
            line->horizontal = AlignCenter;
        } else if(ctrl_symbol == 'r') {
            line->horizontal = AlignRight;
        } else if(ctrl_symbol == '#') {
            line->font = FontPrimary;
        }
        string_right(text, 2);
        processed = true;
    } while(false);

    return processed;
}

void widget_element_text_scroll_add_line(WidgetElement* element, GuiTextScrollLine* line) {
    GuiTextScrollModel* model = element->model;
    GuiTextScrollLine new_line;
    new_line.font = line->font;
    new_line.horizontal = line->horizontal;
    string_init_set(new_line.text, line->text);
    GuiTextScrollLineArray_push_back(model->line_array, new_line);
}

static void gui_text_scroll_fill_lines(Canvas* canvas, WidgetElement* element) {
    GuiTextScrollModel* model = element->model;
    GuiTextScrollLine line_tmp;
    uint8_t line_width_max = 128 - model->x - model->width - 4;
    bool all_text_processed = false;
    string_init(line_tmp.text);

    while(!all_text_processed) {
        // Set default line properties
        line_tmp.font = FontSecondary;
        line_tmp.horizontal = AlignLeft;
        string_reset(line_tmp.text);
        // Process control symbols
        while(gui_text_scroll_process_ctrl_symbols(&line_tmp, model->text))
            ;
        // Set canvas font
        canvas_set_font(canvas, line_tmp.font);

        uint8_t line_width = 0;
        uint16_t char_i = 0;
        while(true) {
            char next_char = string_get_char(model->text, char_i++);
            if(next_char == '\0') {
                widget_element_text_scroll_add_line(element, &line_tmp);
                all_text_processed = true;
                break;
            } else if(next_char == '\n') {
                widget_element_text_scroll_add_line(element, &line_tmp);
                string_right(model->text, char_i);
                break;
            } else {
                line_width += canvas_glyph_width(canvas, next_char);
                if(line_width > line_width_max) {
                    widget_element_text_scroll_add_line(element, &line_tmp);
                    string_right(model->text, char_i - 1);
                } else {
                    string_push_back(line_tmp.text, next_char);
                }
            }
        }
    }
}

static void gui_text_scroll_draw(Canvas* canvas, WidgetElement* element) {
    furi_assert(canvas);
    furi_assert(element);
    GuiTextScrollModel* model = element->model;
    if(!model->text_formatted) {
        gui_text_scroll_fill_lines(canvas, element);
        model->text_formatted = true;
    }

    uint8_t y = model->y;
    if(GuiTextScrollLineArray_size(model->line_array)) {
        GuiTextScrollLineArray_it_t it;
        for(GuiTextScrollLineArray_it(it, model->line_array); !GuiTextScrollLineArray_end_p(it);
            GuiTextScrollLineArray_next(it)) {
            GuiTextScrollLine* line = GuiTextScrollLineArray_ref(it);
            FURI_LOG_I(
                TAG, "Text width: %d", canvas_string_width(canvas, string_get_cstr(line->text)));
            canvas_draw_str_aligned(
                canvas, model->x, y, line->horizontal, AlignTop, string_get_cstr(line->text));
            // Rework with different fonts
            y += 11;
        }
    }
}

static bool gui_text_scroll_input(InputEvent* event, WidgetElement* element) {
    GuiTextScrollModel* model = element->model;
    bool consumed = false;

    if(event->key == InputKeyUp) {
        if(model->current_line > 0) {
            model->current_line--;
        }
        consumed = true;
    } else if(event->key == InputKeyDown) {
        if(model->current_line < model->total_lines) {
            model->current_line++;
        }
        consumed = true;
    }

    return consumed;
}

static void gui_text_scroll_free(WidgetElement* text_scroll) {
    furi_assert(text_scroll);

    GuiTextScrollModel* model = text_scroll->model;
    GuiTextScrollLineArray_it_t it;
    for(GuiTextScrollLineArray_it(it, model->line_array); !GuiTextScrollLineArray_end_p(it);
        GuiTextScrollLineArray_next(it)) {
        GuiTextScrollLine* line = GuiTextScrollLineArray_ref(it);
        string_clear(line->text);
    }
    string_clear(model->text);
    free(text_scroll->model);
    free(text_scroll);
}

WidgetElement* widget_element_text_scroll_create(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    const char* text) {
    furi_assert(text);

    // Allocate and init model
    GuiTextScrollModel* model = malloc(sizeof(GuiTextScrollModel));
    model->x = x;
    model->y = y;
    model->width = width;
    model->height = height;
    model->current_line = 0;
    model->total_lines = 0; //CALCULATE
    GuiTextScrollLineArray_init(model->line_array);
    string_init_set_str(model->text, text);

    // Allocate and init Element
    WidgetElement* text_scroll = malloc(sizeof(WidgetElement));
    text_scroll->parent = NULL;
    text_scroll->draw = gui_text_scroll_draw;
    text_scroll->input = gui_text_scroll_input;
    text_scroll->free = gui_text_scroll_free;
    text_scroll->model = model;
    return text_scroll;
}
