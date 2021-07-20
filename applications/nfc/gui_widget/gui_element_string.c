#include "gui_element_i.h"
#include "gui_element_string.h"
#include "gui_widget.h"

#include <m-string.h>

struct GuiString {
    GuiElement element;
};

typedef struct {
    uint8_t x;
    uint8_t y;
    Align horizontal;
    Align vertical;
    Font font;
    string_t text;
} GuiStringModel;

static void gui_string_draw(Canvas* canvas, GuiElement* element) {
    furi_assert(canvas);
    furi_assert(element);
    GuiStringModel* model = element->model;

    if(string_size(model->text)) {
        canvas_set_font(canvas, model->font);
        canvas_draw_str_aligned(
            canvas,
            model->x,
            model->y,
            model->horizontal,
            model->vertical,
            string_get_cstr(model->text));
    }
}

GuiString* gui_string_alloc(
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text) {
    furi_assert(text);

    // Allocate and init model
    GuiStringModel* model = furi_alloc(sizeof(GuiStringModel));
    model->x = x;
    model->y = y;
    model->horizontal = horizontal;
    model->vertical = vertical;
    model->font = font;
    string_init_set_str(model->text, text);

    // Allocate and init Element
    GuiString* gui_string = furi_alloc(sizeof(GuiString));
    gui_string->element.parent = NULL;
    gui_string->element.input = NULL;
    gui_string->element.draw = gui_string_draw;
    gui_string->element.model = model;

    return gui_string;
}

void gui_string_free(GuiString* gui_string) {
    furi_assert(gui_string);

    GuiStringModel* model = gui_string->element.model;
    if(gui_string->element.parent != NULL) {
        // TODO deattach element
    }
    string_clear(model->text);
    free(gui_string->element.model);
    free(gui_string);
}

GuiElement* gui_string_get_element(GuiString* gui_string) {
    return &gui_string->element;
}
