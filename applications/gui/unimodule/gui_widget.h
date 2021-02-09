#pragma once
#include <gui/view.h>

typedef struct GuiWidget GuiWidget;
typedef struct GuiElement GuiElement;

// allocate widget that holds elements
GuiWidget* gui_widget_alloc();

// free widget that holds elements
void gui_widget_free(GuiWidget* gui_widget);

// get widget view
View* gui_widget_get_view(GuiWidget* gui_widget);

// add element to widget
void gui_widget_add_element(GuiWidget* gui_widget, GuiElement* element);