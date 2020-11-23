#pragma once
#include "ibutton.h"

class AppiButtonModeCyfralRead : public AppTemplateMode<AppiButtonState, AppiButtonEvent> {
public:
    const char* name = "cyfral emulate";
    AppiButton* app;

    void event(AppiButtonEvent* event, AppiButtonState* state);
    void render(CanvasApi* canvas, AppiButtonState* state);
    void acquire();
    void release();

    AppiButtonModeCyfralRead(AppiButton* parent_app) {
        app = parent_app;
    };
};

void AppiButtonModeCyfralRead::event(AppiButtonEvent* event, AppiButtonState* state) {
    if(event->type == AppiButtonEvent::EventTypeTick) {
    } else if(event->type == AppiButtonEvent::EventTypeKey) {
        if(event->value.input.state && event->value.input.input == InputUp) {
            app->decrease_cyfral_address();
        }

        if(event->value.input.state && event->value.input.input == InputDown) {
            app->increase_cyfral_address();
        }
    }
}

void AppiButtonModeCyfralRead::render(CanvasApi* canvas, AppiButtonState* state) {
    canvas->set_font(canvas, FontSecondary);
    canvas->draw_str(canvas, 2, 25, "< Cyfral read >");

    app->render_cyfral_list(canvas, state);
}

void AppiButtonModeCyfralRead::acquire() {
}

void AppiButtonModeCyfralRead::release() {
}