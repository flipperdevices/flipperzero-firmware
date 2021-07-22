#include <furi.h>
#include "gui_element_i.h"
#include "gui_widget.h"
#include <m-array.h>

ARRAY_DEF(ElementArray, GuiElement*, M_PTR_OPLIST);

struct GuiWidget {
    View* view;
    void* context;
};

typedef struct {
    ElementArray_t element;
} GuiWidgetModel;

static void gui_widget_view_draw_callback(Canvas* canvas, void* _model) {
    GuiWidgetModel* model = _model;
    canvas_clear(canvas);

    // Draw all elements
    ElementArray_it_t it;
    ElementArray_it(it, model->element);
    while(!ElementArray_end_p(it)) {
        GuiElement* element = *ElementArray_ref(it);
        if(element->draw != NULL) {
            element->draw(canvas, element);
        }
        ElementArray_next(it);
    }
}

static bool gui_widget_view_input_callback(InputEvent* event, void* context) {
    GuiWidget* gui_widget = context;
    bool consumed = false;

    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            ElementArray_it_t it;
            ElementArray_it(it, model->element);
            while(!ElementArray_end_p(it)) {
                GuiElement* element = *ElementArray_ref(it);
                if(element->input != NULL) {
                    consumed |= element->input(event, element);
                }
                ElementArray_next(it);
            }
            return true;
        });

    return consumed;
}

GuiWidget* gui_widget_alloc() {
    GuiWidget* gui_widget = furi_alloc(sizeof(GuiWidget));
    gui_widget->view = view_alloc();
    view_set_context(gui_widget->view, gui_widget);
    view_allocate_model(gui_widget->view, ViewModelTypeLocking, sizeof(GuiWidgetModel));
    view_set_draw_callback(gui_widget->view, gui_widget_view_draw_callback);
    view_set_input_callback(gui_widget->view, gui_widget_view_input_callback);

    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            ElementArray_init(model->element);
            return true;
        });

    return gui_widget;
}

void gui_widget_clear(GuiWidget* gui_widget) {
    furi_assert(gui_widget);

    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            ElementArray_it_t it;
            ElementArray_it(it, model->element);
            while(!ElementArray_end_p(it)) {
                GuiElement* element = *ElementArray_ref(it);
                furi_assert(element->free);
                element->free(element);
                ElementArray_next(it);
            }
            ElementArray_clean(model->element);
            return true;
        });
}

void gui_widget_free(GuiWidget* gui_widget) {
    furi_assert(gui_widget);
    // Free all elements
    gui_widget_clear(gui_widget);
    // Free elements container
    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            ElementArray_clear(model->element);
            return true;
        });

    view_free(gui_widget->view);
    free(gui_widget);
}

View* gui_widget_get_view(GuiWidget* gui_widget) {
    furi_assert(gui_widget);
    return gui_widget->view;
}

void gui_widget_add_element(GuiWidget* gui_widget, GuiElement* element) {
    furi_assert(gui_widget);
    furi_assert(element);

    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            element->parent = gui_widget;
            ElementArray_push_back(model->element, element);
            return true;
        });
}

void gui_widget_add_string_element(
    GuiWidget* gui_widget,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text) {
    furi_assert(gui_widget);
    GuiElement* string_element = gui_string_create(x, y, horizontal, vertical, font, text);
    gui_widget_add_element(gui_widget, string_element);
}

void gui_widget_add_button_element(
    GuiWidget* gui_widget,
    GuiButtonType button_type,
    const char* text,
    ButtonCallback callback,
    void* context) {
    furi_assert(gui_widget);
    GuiElement* button_element = gui_button_create(button_type, text, callback, context);
    gui_widget_add_element(gui_widget, button_element);
}

void gui_widget_add_icon_element(GuiWidget* gui_widget, uint8_t x, uint8_t y, const Icon* icon) {
    furi_assert(gui_widget);
    furi_assert(icon);
    GuiElement* icon_element = gui_icon_create(x, y, icon);
    gui_widget_add_element(gui_widget, icon_element);
}
