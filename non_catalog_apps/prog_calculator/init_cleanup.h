#ifndef INIT_CLEANUP_H
#define INIT_CLEANUP_H

#include "calculator_state.h"
#include <furi.h>
#include <gui/gui.h>

FuriMessageQueue* initialize_event_queue();
Calculator* initialize_calculator_state();
ViewPort* initialize_view_port(Calculator* calculator_state, FuriMessageQueue* event_queue);
Gui* initialize_gui(ViewPort* view_port);
void cleanup_resources(
    Calculator* calculator_state, 
    ViewPort* view_port, 
    Gui* gui, 
    FuriMessageQueue* event_queue);

#endif // INIT_CLEANUP_H
