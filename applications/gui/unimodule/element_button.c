#include "element_i.h"
#include "element_button.h"
#include "gui_widget.h"
#include <gui/elements.h>

struct GuiButton {
    GuiElement element;
};

typedef struct {
    GuiButtonType button_type;
    const char* text;
    ButtonCallback callback;
    void* context;
} GuiButtonModel;

static void gui_button_render(Canvas* canvas, GuiElement* element) {
    furi_assert(canvas);
    furi_assert(element);
    GuiButtonModel* model = element->model;

    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontSecondary);

    // Draw buttons
    if(model->button_type == ButtonLeft) {
        elements_button_left(canvas, model->text);
    } else if(model->button_type == ButtonRight) {
        elements_button_right(canvas, model->text);
    } else if(model->button_type == ButtonCenter) {
        elements_button_center(canvas, model->text);
    }
}

static bool gui_button_input(InputEvent* event, GuiElement* element) {
    GuiButtonModel* model = element->model;
    bool consumed = false;

    // Process key presses only
    if(event->state) {
        if(model->callback) {
            if(model->button_type == ButtonLeft && event->input == InputLeft) {
                model->callback(model->context);
                consumed = true;
            } else if(model->button_type == ButtonRight && event->input == InputRight) {
                model->callback(model->context);
                consumed = true;
            } else if(model->button_type == ButtonCenter && event->input == InputOk) {
                model->callback(model->context);
                consumed = true;
            }
        }
    }

    return consumed;
}

GuiButton* gui_button_alloc(
    GuiButtonType button_type,
    const char* text,
    ButtonCallback callback,
    void* context) {
    // allocate and init model
    GuiButtonModel* model = furi_alloc(sizeof(GuiButtonModel));
    model->button_type = button_type;
    model->text = text;
    model->callback = callback;
    model->context = context;

    // allocate and init element
    GuiButton* gui_string = furi_alloc(sizeof(GuiButton));
    gui_string->element.parent = NULL;
    gui_string->element.input = gui_button_input;
    gui_string->element.render = gui_button_render;
    gui_string->element.model = model;

    return gui_string;
}

void gui_button_free(GuiButton* gui_button) {
    furi_assert(gui_button);

    if(gui_button->element.parent != NULL) {
        // TODO deattach element
    }

    free(gui_button->element.model);
    free(gui_button);
}

GuiElement* gui_button_get_element(GuiButton* gui_button) {
    return &gui_button->element;
}

void gui_button_set_text(GuiButton* gui_button, const char* text) {
    furi_assert(gui_button);
    GuiButtonModel* model = gui_button->element.model;

    if(gui_button->element.parent == NULL) {
        model->text = text;
    } else {
        view_get_model(gui_widget_get_view(gui_button->element.parent));
        model->text = text;
        view_commit_model(gui_widget_get_view(gui_button->element.parent));
    }
}
