#pragma once
#include <furi.h>
#include <gui/view.h>

typedef struct GuiElement GuiElement;
typedef struct GuiWidget GuiWidget;

struct GuiElement {
    // generic render and input callbacks
    void (*render)(Canvas* canvas, GuiElement* element);
    bool (*input)(InputEvent* event, GuiElement* element);

    // generic model holder
    void* model;

    // pointer to widget that hold our element
    GuiWidget* parent;
};