#ifndef __structs_h__
#define __structs_h__

#include <gui/gui.h>
#include <gui/view_port.h>
#include <core/thread.h>
#include <core/message_queue.h>

/* Application context structure */
struct ApplicationContext {
    Gui* gui;
    ViewPort* view_port;
    FuriThread* secondary_thread;
    FuriMessageQueue* user_input_queue; // Input event notification from input callback to main thread
    FuriMessageQueue* game_event_queue; // Application event to be processed
};

enum GameEventType {
    IDLE_TIMEOUT, // No user input received, perform background work
    SAVE_AND_EXIT // Request to exit the program, save the state and quit the thread
};

/* Event to be processed by secondary thread to update the game state */
struct GameEvent {
    enum GameEventType type;
};

#endif
