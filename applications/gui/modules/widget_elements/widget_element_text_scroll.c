#include "widget_element_i.h"
#include <m-string.h>
#include <gui/elements.h>
#include <m-array.h>

typedef struct {
    uint8_t x;
    uint8_t y;
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
    uint16_t next_line_y;
} GuiTextScrollModel;

static void gui_text_scroll_fill_lines(WidgetElement* element, const char* text) {
    while(true) {
        
    }
}

static void gui_text_scroll_draw(Canvas* canvas, WidgetElement* element) {
    furi_assert(canvas);
    furi_assert(element);
    GuiTextScrollModel* model = element->model;

    if(GuiTextScrollLineArray_size(model->line_array)) {
        GuiTextScrollLineArray_it_t it;
        for(GuiTextScrollLineArray_it(it, model->line_array); !GuiTextScrollLineArray_end_p(it);
            GuiTextScrollLineArray_next(it)) {
            GuiTextScrollLine* line = GuiTextScrollLineArray_ref(it);
            canvas_draw_str_aligned(
                canvas, line->x, line->y, line->horizontal, AlignTop, string_get_cstr(line->text));
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

    free(text_scroll->model);
    free(text_scroll);
}

void widget_element_text_scroll_add_line(
    WidgetElement* element,
    Align horizontal,
    Font font,
    const char* text) {
    furi_assert(element);
    furi_assert(text);

    GuiTextScrollModel* model = element->model;
    GuiTextScrollLine line;
    line.font = font;
    line.horizontal = horizontal;
    line.x = model->x;
    line.y = model->next_line_y;
    string_init_set_str(line.text, text);
    GuiTextScrollLineArray_push_back(model->line_array, line);
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
    model->next_line_y = y;
    GuiTextScrollLineArray_init(model->line_array);

    // Allocate and init Element
    WidgetElement* text_scroll = malloc(sizeof(WidgetElement));
    text_scroll->parent = NULL;
    text_scroll->draw = gui_text_scroll_draw;
    text_scroll->input = gui_text_scroll_input;
    text_scroll->free = gui_text_scroll_free;
    text_scroll->model = model;

    widget_element_text_scroll_add_line(text_scroll, AlignLeft, FontSecondary, text);

    return text_scroll;
}
