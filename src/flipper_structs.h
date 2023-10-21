#ifndef __flipper_structs_h__
#define __flipper_structs_h__

#include <gui/gui.h>
#include <core/thread.h>
#include <core/message_queue.h>

#include "game_structs.h"

/* Application context structure */
struct ApplicationContext {
    Gui *gui;
    ViewPort *view_port;
    FuriThread *secondary_thread;
    FuriMessageQueue *user_input_queue; // Input event notification from input callback to main thread
    FuriMessageQueue *game_event_queue; // Application event to be processed
    struct GameState *game_state; // Read by draw_callback thread, written by secondary_thread
};

#endif
