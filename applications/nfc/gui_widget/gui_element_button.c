#include "gui_element_i.h"
#include "gui_element_button.h"
#include "gui_widget.h"
#include <gui/elements.h>
#include <m-string.h>

struct GuiButton {
    GuiElement element;
};

typedef struct {
    GuiButtonType button_type;
    string_t text;
    ButtonCallback callback;
    void* context;
} GuiButtonModel;

static void gui_button_draw(Canvas* canvas, GuiElement* element) {
    furi_assert(canvas);
    furi_assert(element);
    GuiButtonModel* model = element->model;

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    if(model->button_type == GuiButtonTypeLeft) {
        elements_button_left(canvas, string_get_cstr(model->text));
    } else if(model->button_type == GuiButtonTypeRight) {
        elements_button_right(canvas, string_get_cstr(model->text));
    } else if(model->button_type == GuiButtonTypeCenter) {
        elements_button_center(canvas, string_get_cstr(model->text));
    }
}

static bool gui_button_input(InputEvent* event, GuiElement* element) {
    GuiButtonModel* model = element->model;
    bool consumed = false;
    ButtonCallback callback;
    void* context;
    GuiButtonType button_type;

    view_get_model(gui_widget_get_view(element->parent));
    callback = model->callback;
    context = model->context;
    button_type = model->button_type;
    view_commit_model(gui_widget_get_view(element->parent), false);

    if((event->type == InputTypeShort) && callback) {
        if((button_type == GuiButtonTypeLeft) && (event->key == InputKeyLeft)) {
            callback(context);
            consumed = true;
        } else if((button_type == GuiButtonTypeRight) && (event->key == InputKeyRight)) {
            callback(context);
            consumed = true;
        } else if((button_type == GuiButtonTypeCenter) && (event->key == InputKeyOk)) {
            callback(context);
            consumed = true;
        }
    }

    return consumed;
}

GuiButton* gui_button_alloc(
    GuiButtonType button_type,
    const char* text,
    ButtonCallback callback,
    void* context) {
    // Allocate and init model
    GuiButtonModel* model = furi_alloc(sizeof(GuiButtonModel));
    model->button_type = button_type;
    model->callback = callback;
    model->context = context;
    string_init_set_str(model->text, text);

    // Allocate and init Element
    GuiButton* gui_button = furi_alloc(sizeof(GuiButton));
    gui_button->element.parent = NULL;
    gui_button->element.input = gui_button_input;
    gui_button->element.draw = gui_button_draw;
    gui_button->element.model = model;

    return gui_button;
}

void gui_button_free(GuiButton* gui_button) {
    furi_assert(gui_button);

    GuiButtonModel* model = gui_button->element.model;
    if(gui_button->element.parent != NULL) {
        // TODO deattach element
    }
    string_clear(model->text);
    free(gui_button->element.model);
    free(gui_button);
}

GuiElement* gui_button_get_element(GuiButton* gui_button) {
    return &gui_button->element;
}
