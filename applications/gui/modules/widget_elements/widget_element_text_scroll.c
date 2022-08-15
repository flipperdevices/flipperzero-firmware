#include "widget_element_i.h"
#include <m-string.h>
#include <gui/elements.h>
#include <m-array.h>

#define WIDGET_ELEMENT_TEXT_SCROLL_BAR_OFFSET (4)

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
    uint8_t scroll_pos_total;
    uint8_t scroll_pos_current;
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
    bool all_text_processed = false;
    string_init(line_tmp.text);
    bool reached_new_line = true;
    uint16_t total_height = 0;

    while(!all_text_processed) {
        if(reached_new_line) {
            // Set default line properties
            line_tmp.font = FontSecondary;
            line_tmp.horizontal = AlignLeft;
            string_reset(line_tmp.text);
            // Process control symbols
            while(gui_text_scroll_process_ctrl_symbols(&line_tmp, model->text))
                ;
        }
        // Set canvas font
        canvas_set_font(canvas, line_tmp.font);
        CanvasFontParameters* params = canvas_get_font_params(canvas, line_tmp.font);
        total_height += params->height;
        if(total_height > model->height) {
            model->scroll_pos_total++;
        }

        uint8_t line_width = 0;
        uint16_t char_i = 0;
        while(true) {
            char next_char = string_get_char(model->text, char_i++);
            if(next_char == '\0') {
                string_push_back(line_tmp.text, '\0');
                widget_element_text_scroll_add_line(element, &line_tmp);
                total_height += params->leading_default - params->height;
                all_text_processed = true;
                break;
            } else if(next_char == '\n') {
                string_push_back(line_tmp.text, '\0');
                widget_element_text_scroll_add_line(element, &line_tmp);
                string_right(model->text, char_i);
                total_height += params->leading_default - params->height;
                reached_new_line = true;
                break;
            } else {
                line_width += canvas_glyph_width(canvas, next_char);
                if(line_width > model->width) {
                    string_push_back(line_tmp.text, '\0');
                    widget_element_text_scroll_add_line(element, &line_tmp);
                    string_right(model->text, char_i - 1);
                    string_reset(line_tmp.text);
                    total_height += params->leading_default - params->height;
                    reached_new_line = false;
                    break;
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
    uint8_t x = model->x;
    uint16_t curr_line = 0;
    if(GuiTextScrollLineArray_size(model->line_array)) {
        GuiTextScrollLineArray_it_t it;
        for(GuiTextScrollLineArray_it(it, model->line_array); !GuiTextScrollLineArray_end_p(it);
            GuiTextScrollLineArray_next(it), curr_line++) {
            if(curr_line < model->scroll_pos_current) continue;
            GuiTextScrollLine* line = GuiTextScrollLineArray_ref(it);
            CanvasFontParameters* params = canvas_get_font_params(canvas, line->font);
            if(y + params->descender > model->y + model->height) break;
            canvas_set_font(canvas, line->font);
            if(line->horizontal == AlignLeft) {
                x = model->x;
            } else if(line->horizontal == AlignCenter) {
                x = (model->x + model->width) / 2;
            } else if(line->horizontal == AlignRight) {
                x = model->x + model->width;
            }
            canvas_draw_str_aligned(
                canvas, x, y, line->horizontal, AlignTop, string_get_cstr(line->text));
            y += params->leading_default;
        }
        // Draw scroll bar
        if(model->scroll_pos_total > 1) {
            elements_scrollbar_pos(
                canvas,
                model->x + model->width + WIDGET_ELEMENT_TEXT_SCROLL_BAR_OFFSET,
                model->y,
                model->height,
                model->scroll_pos_current,
                model->scroll_pos_total);
        }
    }
}

static bool gui_text_scroll_input(InputEvent* event, WidgetElement* element) {
    GuiTextScrollModel* model = element->model;
    bool consumed = false;

    if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
        if(event->key == InputKeyUp) {
            if(model->scroll_pos_current > 0) {
                model->scroll_pos_current--;
            }
            consumed = true;
        } else if(event->key == InputKeyDown) {
            if((model->scroll_pos_total > 1) &&
               (model->scroll_pos_current < model->scroll_pos_total - 1)) {
                model->scroll_pos_current++;
            }
            consumed = true;
        }
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
    model->width = width - WIDGET_ELEMENT_TEXT_SCROLL_BAR_OFFSET;
    model->height = height;
    model->scroll_pos_current = 0;
    model->scroll_pos_total = 1;
    GuiTextScrollLineArray_init(model->line_array);
    string_init_set_str(model->text, text);

    WidgetElement* text_scroll = malloc(sizeof(WidgetElement));
    text_scroll->parent = NULL;
    text_scroll->draw = gui_text_scroll_draw;
    text_scroll->input = gui_text_scroll_input;
    text_scroll->free = gui_text_scroll_free;
    text_scroll->model = model;
    return text_scroll;
}
