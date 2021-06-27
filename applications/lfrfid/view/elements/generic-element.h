#pragma once
#include <gui/gui.h>
#include <gui/view.h>

class GenericElement {
public:
    GenericElement(){};
    virtual ~GenericElement(){};
    virtual void draw(Canvas* canvas) = 0;
    virtual bool input(InputEvent* event) = 0;

    View* view;
};