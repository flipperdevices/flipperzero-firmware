#include "element_i.h"
#include "element_string.h"
#include "gui_widget.h"

struct GuiString {
    GuiElement element;
};

typedef struct {
    uint8_t x;
    uint8_t y;
    const char* text;
} GuiStringModel;

static void gui_string_render(Canvas* canvas, GuiElement* element) {
    furi_assert(canvas);
    furi_assert(element);
    GuiStringModel* model = element->model;

    canvas_draw_str(canvas, model->x, model->y, model->text);
}

GuiString* gui_string_alloc(uint8_t x, uint8_t y, const char* text) {
    // allocate and init model
    GuiStringModel* model = furi_alloc(sizeof(GuiStringModel));
    model->x = x;
    model->y = y;
    model->text = text;

    // allocate and init element
    GuiString* gui_string = furi_alloc(sizeof(GuiString));
    gui_string->element.parent = NULL;
    gui_string->element.input = NULL;
    gui_string->element.render = gui_string_render;
    gui_string->element.model = model;

    return gui_string;
}

void gui_string_free(GuiString* gui_string) {
    furi_assert(gui_string);

    if(gui_string->element.parent != NULL) {
        // TODO deattach element
    }

    free(gui_string->element.model);
    free(gui_string);
}

GuiElement* gui_string_get_element(GuiString* gui_string) {
    return &gui_string->element;
}

void gui_string_set_x(GuiString* gui_string, uint8_t x) {
    furi_assert(gui_string);
    GuiStringModel* model = gui_string->element.model;

    if(gui_string->element.parent == NULL) {
        // if element dont attached to gui we can simply change value
        model->x = x;
    } else {
        // otherwise we must lock model
        // TODO think about lockable and non-lockable model
        view_get_model(gui_widget_get_view(gui_string->element.parent));
        model->x = x;
        view_commit_model(gui_widget_get_view(gui_string->element.parent));
    }
}

void gui_string_set_y(GuiString* gui_string, uint8_t y) {
    furi_assert(gui_string);
    GuiStringModel* model = gui_string->element.model;

    if(gui_string->element.parent == NULL) {
        model->y = y;
    } else {
        view_get_model(gui_widget_get_view(gui_string->element.parent));
        model->y = y;
        view_commit_model(gui_widget_get_view(gui_string->element.parent));
    }
}

void gui_string_set_text(GuiString* gui_string, const char* text) {
    furi_assert(gui_string);
    GuiStringModel* model = gui_string->element.model;

    if(gui_string->element.parent == NULL) {
        model->text = text;
    } else {
        view_get_model(gui_widget_get_view(gui_string->element.parent));
        model->text = text;
        view_commit_model(gui_widget_get_view(gui_string->element.parent));
    }
}
