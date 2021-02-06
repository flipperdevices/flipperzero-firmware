#include "ibutton-app.h"

void iButtonApp::run(void) {
    iButtonEvent event;
    bool consumed;
    bool exit = false;

    mode_data[current_mode]->on_enter(this);

    while(!exit) {
        view.receive_event(&event);

        consumed = mode_data[current_mode]->on_event(this, &event);

        if(!consumed) {
            if(event.type == iButtonEvent::Type::EventTypeBack) {
                exit = switch_to_prevous();
            }
        }
    };
}

iButtonApp::iButtonApp() {
}

iButtonApp::~iButtonApp() {
}

iButtonAppView* iButtonApp::get_view() {
    return &view;
}

void iButtonApp::switch_to_next(Mode mode) {
    prevous_mode.push_front(current_mode);

    if(mode != Mode::iButtonAppModeExit) {
        mode_data[current_mode]->on_exit(this);
        current_mode = mode;
        mode_data[current_mode]->on_enter(this);
    }
}

bool iButtonApp::switch_to_prevous() {
    Mode mode = prevous_mode.front();
    prevous_mode.pop_front();

    if(mode == Mode::iButtonAppModeExit) {
        return true;
    } else {
        mode_data[current_mode]->on_exit(this);
        current_mode = mode;
        mode_data[current_mode]->on_enter(this);
        return false;
    }
}