#include <furi.h>
#include "element_i.h"
#include "gui_widget.h"

#define MAX_GUI_ELEMENTS 8

struct GuiWidget {
    View* view;
    void* context;
};

// for simplicity i don use any dynamic structures
typedef struct {
    GuiElement* element[MAX_GUI_ELEMENTS];
} GuiWidgetModel;

// fn that render all holded elements
static void gui_widget_view_draw_callback(Canvas* canvas, void* _model) {
    GuiWidgetModel* model = _model;
    canvas_clear(canvas);

    for(uint8_t i = 0; i < MAX_GUI_ELEMENTS; i++) {
        if(model->element[i] != NULL) {
            if(model->element[i]->render != NULL) {
                model->element[i]->render(canvas, model->element[i]);
            }
        }
    };
}

// fn that call all holded elements input callbacks
static bool gui_widget_view_input_callback(InputEvent* event, void* context) {
    GuiWidget* gui_widget = context;
    bool consumed = false;

    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            for(uint8_t i = 0; i < MAX_GUI_ELEMENTS; i++) {
                if(model->element[i] != NULL) {
                    if(model->element[i]->input != NULL) {
                        consumed = model->element[i]->input(event, model->element[i]);
                    }
                }
            };
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
            for(uint8_t i = 0; i < MAX_GUI_ELEMENTS; i++) {
                model->element[i] = NULL;
            };
        });

    return gui_widget;
}

void gui_widget_free(GuiWidget* gui_widget) {
    furi_assert(gui_widget);
    view_free(gui_widget->view);
    free(gui_widget);
}

View* gui_widget_get_view(GuiWidget* gui_widget) {
    furi_assert(gui_widget);
    return gui_widget->view;
}

void gui_widget_add_element(GuiWidget* gui_widget, GuiElement* element) {
    furi_assert(gui_widget);
    with_view_model(
        gui_widget->view, (GuiWidgetModel * model) {
            // add element to first null position
            for(uint8_t i = 0; i < MAX_GUI_ELEMENTS; i++) {
                if(model->element[i] == NULL) {
                    model->element[i] = element;
                    element->parent = gui_widget;
                    break;
                }
            };
        });
}
